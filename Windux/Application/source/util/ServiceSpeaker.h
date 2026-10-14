#if !defined ServiceSpeaker_h
#define ServiceSpeaker_h

class ServiceSpeaker
{
public:

	static void goodBeep( int times );
	static void badBeep( int times );
	static void offlineBeep( int times );

	static void setGoodBeepEnabled(bool enabled) {goodBeepEnabled = enabled;}
	static void setGoodBeepFrequency(int freq) {goodBeepFrequency = freq;}
	static void setGoodBeepPeriod(int period) {goodBeepPeriod = period;}
	static void setGoodBeepDelay(int delay) {goodBeepDelay = delay;}

	static void setBadBeepEnabled(bool enabled) {badBeepEnabled = enabled;}
	static void setBadBeepFrequency(int freq) {badBeepFrequency = freq;}
	static void setBadBeepPeriod(int period) {badBeepPeriod = period;}
	static void setBadBeepDelay(int delay) {badBeepDelay = delay;}

	static void setOfflineBeepEnabled(bool enabled) {offlineBeepEnabled = enabled;}
	static void setOfflineBeepFrequency(int freq) {offlineBeepFrequency = freq;}
	static void setOfflineBeepPeriod(int period) {offlineBeepPeriod = period;}
	static void setOfflineBeepDelay(int delay) {offlineBeepDelay = delay;}
	static void setOfflineBeepVolume(int volume) {offlineBeepVolume = volume;}

private:
	static bool goodBeepEnabled;
	static int  goodBeepFrequency;
	static int  goodBeepPeriod;
	static int  goodBeepDelay;

	static bool badBeepEnabled;
	static int  badBeepFrequency;
	static int  badBeepPeriod;
	static int  badBeepDelay;

	static bool offlineBeepEnabled;
	static int  offlineBeepFrequency;
	static int  offlineBeepPeriod;
	static int  offlineBeepDelay;
	static int  offlineBeepVolume;
};

#endif 