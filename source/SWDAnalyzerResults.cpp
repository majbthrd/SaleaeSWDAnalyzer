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

#include "SWDAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "SWDAnalyzer.h"
#include "SWDAnalyzerSettings.h"
#include <iostream>
#include <fstream>

SWDAnalyzerResults::SWDAnalyzerResults( SWDAnalyzer* analyzer, SWDAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

SWDAnalyzerResults::~SWDAnalyzerResults()
{
}

static void build_string ( char *number_str, Frame *frame )
{
	const char *op_names[4] =
	{
		"WriteDP",
		"ReadDP",
		"WriteAP",
		"ReadAP",
	};
	const char *reg_names[2][4][2] =
	{
		{
			/* SW-DP registers */
			{ "IDCODE", "ABORT" },
			{ "CTRL/STAT", "CTRL/STAT" },
			{ "RESEND", "SELECT" },
			{ "RDBUFF", "TARGETSEL" },
		},
		{
			/* AHB-AP registers */
			{ "CSW", "CSW" },
			{ "TAR", "TAR" },
			{ "N/A", "N/A" },
			{ "DRW", "DRW" },
		},
	};
	unsigned op_code, reg_addr, ack_code;
	const char *op_name, *reg_name;

	op_code  = (frame->mData1 & 0x0C) >> 2;
	reg_addr = (frame->mData1 & 0x03);
	ack_code = (frame->mData1 & 0x70) >> 4;

	op_name = op_names[op_code];
	reg_name = reg_names[(op_code & 2) ? 1 : 0][reg_addr][(op_code & 1) ? 0 : 1];

	if ( (0 == op_code) && (3 == reg_addr) ) ack_code = 8; /* writes to TARGETSEL don't get a response */

	switch (ack_code)
	{
	case 8: /* special case of TARGETSEL */
	case 0x1: /* OK */
		sprintf(number_str, "%s[%u=%s] %08x",   op_name, reg_addr, reg_name, frame->mData2);
		break;
	case 0x2: /* WAIT */
	case 0x4: /* FAULT */
		sprintf(number_str, "%s[%u=%s] %s",     op_name, reg_addr, reg_name, (0x2==ack_code) ? "WAIT" : "FAULT");
		break;
	default: /* unknown */
		sprintf(number_str, "%s[%u=%s] ACK=%x", op_name, reg_addr, reg_name, ack_code);
		break;
	}
}

void SWDAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );

	char number_str[128];

	build_string(number_str, &frame);

	AddResultString( number_str );
}

void SWDAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
	std::ofstream file_stream( file, std::ios::out );

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();

	file_stream << "Time [s],Value" << std::endl;

	U64 num_frames = GetNumFrames();
	for( U32 i=0; i < num_frames; i++ )
	{
		Frame frame = GetFrame( i );
		
		char time_str[128];
		AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

		char number_str[128];
		build_string(number_str, &frame);

		file_stream << time_str << "," << number_str << std::endl;

		if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
		{
			file_stream.close();
			return;
		}
	}

	file_stream.close();
}

void SWDAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
	Frame frame = GetFrame( frame_index );
	ClearTabularText();

	char number_str[128];
	build_string(number_str, &frame);
	AddTabularText( number_str );
}

void SWDAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	//not supported
}

void SWDAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	//not supported
}
