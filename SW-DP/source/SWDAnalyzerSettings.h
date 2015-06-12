#ifndef SWD_ANALYZER_SETTINGS
#define SWD_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class SWDAnalyzerSettings : public AnalyzerSettings
{
public:
	SWDAnalyzerSettings();
	virtual ~SWDAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
	Channel mSWDIOChannel;
	Channel mSWCLKChannel;

protected:
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mSWDIOChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mSWCLKChannelInterface;
};

#endif //SWD_ANALYZER_SETTINGS
