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

#include "SWDAnalyzerSettings.h"
#include <AnalyzerHelpers.h>

SWDAnalyzerSettings::SWDAnalyzerSettings()
:	mSWDIOChannel( UNDEFINED_CHANNEL ),
	mSWCLKChannel( UNDEFINED_CHANNEL )
{
	mSWDIOChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mSWDIOChannelInterface->SetTitleAndTooltip( "SWDIO", "SWDIO" );
	mSWDIOChannelInterface->SetChannel( mSWDIOChannel );

	mSWCLKChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mSWCLKChannelInterface->SetTitleAndTooltip( "SWCLK", "SWCLK" );
	mSWCLKChannelInterface->SetChannel( mSWCLKChannel );

	AddInterface( mSWDIOChannelInterface.get() );
	AddInterface( mSWCLKChannelInterface.get() );

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	AddChannel( mSWDIOChannel, "SWDIO", false );
	AddChannel( mSWCLKChannel, "SWCLK", false );
}

SWDAnalyzerSettings::~SWDAnalyzerSettings()
{
}

bool SWDAnalyzerSettings::SetSettingsFromInterfaces()
{
	mSWDIOChannel = mSWDIOChannelInterface->GetChannel();
	mSWCLKChannel = mSWCLKChannelInterface->GetChannel();

	ClearChannels();
	AddChannel( mSWDIOChannel, "SWDIO", true );
	AddChannel( mSWCLKChannel, "SWCLK", true );

	return true;
}

void SWDAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mSWDIOChannelInterface->SetChannel( mSWDIOChannel );
	mSWCLKChannelInterface->SetChannel( mSWCLKChannel );
}

void SWDAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	text_archive >> mSWDIOChannel;
	text_archive >> mSWCLKChannel;

	ClearChannels();
	AddChannel( mSWDIOChannel, "SWDIO", true );
	AddChannel( mSWCLKChannel, "SWCLK", true );

	UpdateInterfacesFromSettings();
}

const char* SWDAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << mSWDIOChannel;
	text_archive << mSWCLKChannel;

	return SetReturnString( text_archive.GetString() );
}

