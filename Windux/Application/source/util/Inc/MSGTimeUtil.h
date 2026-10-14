/*-----------------------------------------------
     File name    : MSGTimeUtil.h
     Author       : Haibin Zheng
     Date         : Apr/28, 2005
     Description  : following functions are added to Util namespace for Dst/St time processing.
  -----------------------------------------------*/
#ifndef MSGTimeUtil_H
#define MSGTimeUtil_H

#include "MsgTime.h"

#define TZREG  L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones"
#define TZREG2 L"SYSTEM\\CurrentControlSet\\Control\\TimeZoneInformation"


#define TZ_MAX_SIZE 512
struct REG_TZI
{
	long Bias;
	long StandardBias;
	long DaylightBias;
	SYSTEMTIME StandardDate; 
	SYSTEMTIME DaylightDate;
};

struct LS_TZREG
{
	TCHAR tcName[TZ_MAX_SIZE];
	TCHAR tcDisp[TZ_MAX_SIZE];
	TCHAR tcDLT[TZ_MAX_SIZE];
	TCHAR tcSTD[TZ_MAX_SIZE];
	TCHAR MapID[TZ_MAX_SIZE];
	DWORD Index;
	DWORD ActiveTimeBias;
	REG_TZI TZI;
};

namespace Util
{
	extern std::wstring GetTimeZoneName(); //return the TimeZone name

	//return the DST/ST start time for a given year; by default it takes the current year.
	extern bool GetDstAndStStartTime(Util::CMSGTime & dst_start_time,
									 Util::CMSGTime & st_start_time, DWORD i_year = 0); 

	extern DWORD GetDstOffset();       //return the DST offset (in minutes) if it is in DST time zone.
	                            
	extern Util::CMSGTime DstTimeToStTime(const Util::CMSGTime& i_local_time);
	extern Util::CMSGTime StTimeToDstTime(const Util::CMSGTime& i_st_time);

	//the following routines are private to this scope only
	//convert the SYSTEMTIME to Msg time; by default it takes the current year.
	Util::CMSGTime SystemTimeToMsgTime(const SYSTEMTIME sys_time,DWORD i_year = 0);
	std::wstring GetFirstDayOfWeek(); //get the first day of week specified by the locale
	DWORD GetWeekDay(const Util::CMSGTime& i_time); //return the enum value of weekday
	bool IsInDstMode();         //check whether the computer is in DST mode for the current time
	bool IsInDstTimeZone();     //check whether the computer is set to DST time zone
	bool IsAutoAdjustClockOn(); //check whether auto adjust clock is on when it is in DST time zone
	//get the firm TimeZone info regardless the "auto adjust clock" is on/off
	bool GetTimeZoneInfo(const std::wstring& i_tz_name, REG_TZI& v_tz);
	
	DWORD GetSTLondonDiff(); //in minutes

}// end namespace Util
#endif // MSGTimeUtil_H
