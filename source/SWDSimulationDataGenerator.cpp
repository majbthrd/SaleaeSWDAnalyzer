#include "SWDSimulationDataGenerator.h"
#include "SWDAnalyzerSettings.h"

#include <AnalyzerHelpers.h>

SWDSimulationDataGenerator::SWDSimulationDataGenerator()
:	mSerialText( "My first analyzer, woo hoo!" ),
	mStringIndex( 0 )
{
}

SWDSimulationDataGenerator::~SWDSimulationDataGenerator()
{
}

void SWDSimulationDataGenerator::Initialize( U32 simulation_sample_rate, SWDAnalyzerSettings* settings )
{
}

U32 SWDSimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel )
{
	return 1;
}

void SWDSimulationDataGenerator::CreateSerialByte()
{
}
