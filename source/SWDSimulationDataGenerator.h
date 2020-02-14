#ifndef SWD_SIMULATION_DATA_GENERATOR
#define SWD_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
class SWDAnalyzerSettings;

class SWDSimulationDataGenerator
{
public:
	SWDSimulationDataGenerator();
	~SWDSimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, SWDAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	SWDAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

protected:
	void CreateSerialByte();
	std::string mSerialText;
	U32 mStringIndex;

	SimulationChannelDescriptor mSerialSimulationData;

};
#endif //SWD_SIMULATION_DATA_GENERATOR