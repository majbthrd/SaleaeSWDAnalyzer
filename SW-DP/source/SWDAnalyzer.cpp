/*
    The SW-DP protocol is described by the following publicly available documents:

    DDI 0316 CoreSight™ DAP-Lite Technical Reference Manual
    http://infocenter.arm.com/help/topic/com.arm.doc.ddi0316d/DDI0316D_dap_lite_trm.pdf

    Programming Internal Flash Over the Serial Wire Debug Interface
    http://www.silabs.com/Support%20Documents/TechnicalDocs/AN0062.pdf

    CY8C41xx, CY8C42xx Programming Specifications
    http://www.cypress.com/?docID=48133

    SW-DP (Serial Wire Debug Port) Analyzer plugin for the Saleae Logic

    Copyright (C) 2015 Peter Lawrence.  All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 2.1, as
    published by the Free Software Foundation.  This program is
    distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include "SWDAnalyzer.h"
#include "SWDAnalyzerSettings.h"
#include <AnalyzerChannelData.h>

SWDAnalyzer::SWDAnalyzer()
:	Analyzer(),  
	mSettings( new SWDAnalyzerSettings() ),
	mSimulationInitialized( false )
{
	SetAnalyzerSettings( mSettings.get() );
}

SWDAnalyzer::~SWDAnalyzer()
{
	KillThread();
}

void SWDAnalyzer::WorkerThread()
{
	mResults.reset( new SWDAnalyzerResults( this, mSettings.get() ) );
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings->mSWDIOChannel );

	mSWDIO = GetAnalyzerChannelData( mSettings->mSWDIOChannel );
	mSWCLK = GetAnalyzerChannelData( mSettings->mSWCLKChannel );

	if( mSWCLK->GetBitState() == BIT_HIGH )
		mSWCLK->AdvanceToNextEdge();

	U64 current_sample, onset_sample, previous_current_sample;
	U32 ones_count, data_count;

	bool rise_bit, parity;

	enum state_enum
	{
		START,
		APnDP,
		RnW,
		A0,
		A1,
		PARITY,
		STOP,
		PARK,
		TRN,
		ACK0,
		ACK1,
		ACK2,
		ACKTRN,
		DATA,
		RST,
		ENDTRN,
	};

	enum state_enum state, next_state;

	U8 command, ack;
	U32 data;

	state = RST;
	ones_count = 6;
	previous_current_sample = 0;

	for( ; ; )
	{
		mSWCLK->AdvanceToNextEdge(); // rising edge
		current_sample = mSWCLK->GetSampleNumber();

		mSWDIO->AdvanceToAbsPosition(current_sample - 1);
		rise_bit = mSWDIO->GetBitState() == BIT_HIGH;

		Frame frame;

#if 1
		/* method of jump-starting decoding if capture doesn't include 50 ones */
		if ( (current_sample - previous_current_sample) > 10000 )
			state = START;
#endif

		switch (state)
		{
		case START:
			next_state = (rise_bit) ? APnDP : START;
			if (rise_bit)
			{
				mResults->AddMarker( current_sample, AnalyzerResults::Start, mSettings->mSWDIOChannel );
				onset_sample = current_sample;	
			}
			break;
		case APnDP:
			parity = rise_bit;
			next_state = RnW;
			command &= 0x7;
			if (rise_bit)
				command |= 0x8;
			mResults->AddMarker( current_sample, (rise_bit) ? AnalyzerResults::One : AnalyzerResults::Zero, mSettings->mSWDIOChannel );
			break;
		case RnW:
			parity ^= rise_bit;
			next_state = A0;
			command &= 0xB;
			if (rise_bit)
				command |= 0x4;
			mResults->AddMarker( current_sample, (rise_bit) ? AnalyzerResults::One : AnalyzerResults::Zero, mSettings->mSWDIOChannel );
			break;
		case A0:
			parity ^= rise_bit;
			next_state = A1;
			command &= 0xE;
			if (rise_bit)
				command |= 0x1;
			mResults->AddMarker( current_sample, (rise_bit) ? AnalyzerResults::One : AnalyzerResults::Zero, mSettings->mSWDIOChannel );
			break;
		case A1:
			parity ^= rise_bit;
			next_state = PARITY;
			command &= 0xD;
			if (rise_bit)
				command |= 0x2;
			mResults->AddMarker( current_sample, (rise_bit) ? AnalyzerResults::One : AnalyzerResults::Zero, mSettings->mSWDIOChannel );
			break;
		case PARITY:
			next_state = (parity == rise_bit) ? STOP : RST;
			mResults->AddMarker( current_sample, (parity == rise_bit) ? AnalyzerResults::Dot : AnalyzerResults::ErrorDot, mSettings->mSWDIOChannel );
			break;
		case STOP:
			next_state = (rise_bit) ? RST : PARK;
			mResults->AddMarker( current_sample, (rise_bit) ? AnalyzerResults::ErrorDot : AnalyzerResults::Stop, mSettings->mSWDIOChannel );
			break;
		case PARK:
			next_state = (rise_bit) ? TRN : RST;
			break;
		case TRN:
			mResults->AddMarker( current_sample, AnalyzerResults::Square, mSettings->mSWDIOChannel );
			next_state = ACK0;
			break;
		case ACK0:
			next_state = ACK1;
			ack &= 0x6;
			if (rise_bit)
				ack |= 0x1;
			mResults->AddMarker( current_sample, (rise_bit) ? AnalyzerResults::One : AnalyzerResults::Zero, mSettings->mSWDIOChannel );
			break;
		case ACK1:
			next_state = ACK2;
			ack &= 0x5;
			if (rise_bit)
				ack |= 0x2;
			mResults->AddMarker( current_sample, (rise_bit) ? AnalyzerResults::One : AnalyzerResults::Zero, mSettings->mSWDIOChannel );
			break;
		case ACK2:
			next_state = (command & 0x4) ? DATA : ACKTRN;
			data_count = 0;
			parity = false;
			mResults->AddMarker( current_sample, (rise_bit) ? AnalyzerResults::One : AnalyzerResults::Zero, mSettings->mSWDIOChannel );

			ack &= 0x3;
			if (rise_bit)
				ack |= 0x4;

			frame.mData1 = ack & 0x7;
			switch (ack & 0x7)
			{
			case 1: /* OK */
			case 2: /* WAIT */
				frame.mFlags = 0;
				break;
			case 4: /* FAULT */
				frame.mFlags = DISPLAY_AS_WARNING_FLAG;
				break;
			default:
				frame.mFlags = DISPLAY_AS_ERROR_FLAG;
				break;
			}

			switch (ack & 0x7)
			{
			case 1: /* OK */
				break;
			case 2: /* WAIT */
			case 4: /* FAULT */
			default:
				frame.mData1 = (U32)command + ( (U32)ack << 4 );
				frame.mData2 = data;
				frame.mFlags = 0;
				frame.mStartingSampleInclusive = onset_sample;
				frame.mEndingSampleInclusive = current_sample;
				mResults->AddFrame( frame );
				break;
			}
			break;
		case ACKTRN:
			mResults->AddMarker( current_sample, AnalyzerResults::Square, mSettings->mSWDIOChannel );
			next_state = DATA;
			break;
		case DATA:
			data_count++;

			if (33 == data_count)
			{
				next_state = (command & 0x4) ? ENDTRN : START;
				mResults->AddMarker( current_sample, (parity == rise_bit) ? AnalyzerResults::Dot : AnalyzerResults::ErrorDot, mSettings->mSWDIOChannel );

				frame.mData1 = (U32)command + ( (U32)ack << 4 );
				frame.mData2 = data;
				frame.mFlags = 0;
				frame.mStartingSampleInclusive = onset_sample;
				frame.mEndingSampleInclusive = current_sample;
				mResults->AddFrame( frame );
			}
			else
			{
				data >>= 1;
				if (rise_bit)
					data |= 0x80000000;
				parity ^= rise_bit;
				next_state = DATA;
				mResults->AddMarker( current_sample, (rise_bit) ? AnalyzerResults::One : AnalyzerResults::Zero, mSettings->mSWDIOChannel );
			}
			break;
		case ENDTRN:
			mResults->AddMarker( current_sample, AnalyzerResults::Square, mSettings->mSWDIOChannel );
			next_state = START;
			break;
		case RST:
			next_state = ( (ones_count >= 50) && !rise_bit) ? START : RST;
			if (ones_count >= 50)
				mResults->AddMarker( current_sample, AnalyzerResults::UpArrow, mSettings->mSWDIOChannel);
			break;
		default:
			break; 		
		}

		if ( rise_bit )
			ones_count++;
		else
			ones_count = 0;

		state = next_state;

		mResults->CommitResults();

		ReportProgress( current_sample );

		previous_current_sample = current_sample;

		mSWCLK->AdvanceToNextEdge(); // falling edge
	}
}

bool SWDAnalyzer::NeedsRerun()
{
	return false;
}

U32 SWDAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitialized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
		mSimulationInitialized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 SWDAnalyzer::GetMinimumSampleRateHz()
{
	return 0;
}

const char* SWDAnalyzer::GetAnalyzerName() const
{
	return "SW-DP";
}

const char* GetAnalyzerName()
{
	return "SW-DP";
}

Analyzer* CreateAnalyzer()
{
	return new SWDAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}
