// Header file included by ANTLR UniLexer
#ifndef REM_LINUX_ERR
#include <tchar.h>
#endif
#ifndef _CMSG_TIME_IMPL_HEADER_
#define _CMSG_TIME_IMPL_HEADER_
#include "utilunicode.h"


namespace Util
{
#if defined ( _WIN32 )
#pragma warning( push )
#pragma warning( disable : 4244 )
#endif      // _WIN32
    inline CMSGTime ::  CMSGTime(
            const WORD i_wYear,
            const WORD i_wMonth,
            const WORD i_wDay,
            const WORD i_wHour,
            const WORD i_wMinute,
            const WORD i_wSecond,
            const WORD i_wMilliseconds)
    {

        PAL_TIME_STRUCT l_sTimeHolder;
		l_sTimeHolder.wYear = (WORD) i_wYear;
		l_sTimeHolder.wMonth = (WORD) i_wMonth;
		l_sTimeHolder.wDay = (WORD) i_wDay;
		l_sTimeHolder.wHour = (WORD) i_wHour;
		l_sTimeHolder.wMinute = (WORD) i_wMinute;
		l_sTimeHolder.wSecond = (WORD) i_wSecond;
		l_sTimeHolder.wMilliseconds = (WORD) i_wMilliseconds;

        // Convert and check
        BOOL l_Ret = PAL_absclass::PAL_systemTimeToFileTime(&l_sTimeHolder,&m_sTimeHolder);
    #if defined(LINUX_FIX)
        Assert(l_Ret, CPlatformException());
    #else
        Assert(l_Ret, CNTException());
    #endif
        return;
    }


    inline CMSGTime :: CMSGTime(const PAL_TIME_STRUCT& i_sSystemTime)
    {
        BOOL l_Ret;
        l_Ret = PAL_absclass::PAL_systemTimeToFileTime(&i_sSystemTime,&m_sTimeHolder);
    #if defined(LINUX_FIX)
        Assert(l_Ret, CPlatformException());
    #else
        Assert(l_Ret, CNTException());
    #endif
        return;

    }

    inline CMSGTime :: CMSGTime(const CMSGTime & i_cTime)
    {
        m_sTimeHolder = i_cTime.GetNTFileTime();
        return;

    }

    inline CMSGTime :: CMSGTime(const PAL_FILETIME& i_sFileTime)
    {
        m_sTimeHolder = i_sFileTime;
        return ;
    }

    inline CMSGTime :: ~CMSGTime()
    {
        // The handle can take care of itself and the
        // other member variable is a struct which will be de-allocated anyway.
        return;
    }


    //
    // The next suite of member functions are the ceiling and floor functions.
    // Basic premise is to check the level down i.e. Minutes for hours and then
    // For floor:- is then to set that lower level to the minimum if needed.
    // For Ceiling:- add 1 to the higher level element and set lower level elements to 0.
    // Need to convert the internal file time to system time first.
    //
    inline CMSGTime  CMSGTime :: GetSecondFloor() const
    {
        try
        {
            CMSGTime l_cTmpTime(*this);

            l_cTmpTime.SetMilliseconds(0);
            return l_cTmpTime;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline CMSGTime CMSGTime :: GetSecondCeiling() const
    {
        try
        {
            CMSGTime l_cTmpTime(*this);
            if (l_cTmpTime.GetMilliseconds() > 0)
            {
                l_cTmpTime += CMSGTimeSpan::Second() ;
                l_cTmpTime.SetMilliseconds(0);
            }
            return l_cTmpTime;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }


    inline CMSGTime CMSGTime :: GetMinuteFloor() const
    {
        try
        {
            CMSGTime l_cTmpTime(*this);

            l_cTmpTime.SetSecond(0);
            // What ever happens set the milliseconds to 0
            l_cTmpTime.SetMilliseconds(0);
            return l_cTmpTime;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline CMSGTime CMSGTime :: GetMinuteCeiling() const
    {
        try
        {
            CMSGTime l_cTmpTime(*this);
            if (l_cTmpTime.GetSecond() > 0 || l_cTmpTime.GetMilliseconds() > 0)
            {
                l_cTmpTime += CMSGTimeSpan::Minute();
                l_cTmpTime.SetSecond(0);
            }

            // What ever happens set the Millisecs to
            l_cTmpTime.SetMilliseconds(0);
            return l_cTmpTime;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline CMSGTime CMSGTime :: GetHourFloor() const
    {
        try
        {
            CMSGTime l_cTmpTime(*this);

            l_cTmpTime.SetMinute(0);
            // Set the seonds and milliseconds to 0
            l_cTmpTime.SetSecond(0);
            l_cTmpTime.SetMilliseconds(0);
            return l_cTmpTime;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline CMSGTime CMSGTime :: GetHourCeiling() const
    {
        try
        {
            CMSGTime l_cTmpTime(*this);
            if (l_cTmpTime.GetMinute() > 0 ||l_cTmpTime.GetSecond() > 0 ||
                    l_cTmpTime.GetMilliseconds() > 0 )
            {
                l_cTmpTime+= CMSGTimeSpan::Hour();
                l_cTmpTime.SetMinute(0);
            }

            l_cTmpTime.SetSecond(0);
            l_cTmpTime.SetMilliseconds(0);
            return l_cTmpTime;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline CMSGTime CMSGTime :: GetDayFloor() const
    {
        try
        {
            CMSGTime l_cTmpTime(*this);

            l_cTmpTime.SetHour(0);
            // Set the stuff below to 0
            l_cTmpTime.SetMinute(0);
            l_cTmpTime.SetSecond(0);
            l_cTmpTime.SetMilliseconds(0);
            return l_cTmpTime;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline CMSGTime CMSGTime :: GetDayCeiling() const
    {
        try
        {
            CMSGTime l_cTmpTime(*this);
            if (l_cTmpTime.GetHour() > 0 || l_cTmpTime.GetMinute() > 0 ||
                    l_cTmpTime.GetSecond() > 0 || l_cTmpTime.GetMilliseconds() > 0)
            {
                l_cTmpTime += CMSGTimeSpan::Day();
                l_cTmpTime.SetHour(0);
            }
            l_cTmpTime.SetMinute(0);
            l_cTmpTime.SetSecond(0);
            l_cTmpTime.SetMilliseconds(0);
            return l_cTmpTime;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }



    inline CMSGTime CMSGTime :: GetMonthFloor() const
    {
        try
        {
            CMSGTime l_cTmpTime(*this);
            l_cTmpTime.SetDay(1);
            l_cTmpTime.SetHour(0);
            l_cTmpTime.SetMinute(0);
            l_cTmpTime.SetSecond(0);
            l_cTmpTime.SetMilliseconds(0);
            return l_cTmpTime;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

    }

    inline CMSGTime CMSGTime :: GetMonthCeiling() const
    {
        try
        {
            CMSGTime l_cTmpTime(*this);
            if (l_cTmpTime.GetDay() > 1 || l_cTmpTime.GetHour() > 0 ||
                    l_cTmpTime.GetMinute() > 0 ||l_cTmpTime.GetSecond() > 0 ||
                    l_cTmpTime.GetMilliseconds() > 0)
            {
                l_cTmpTime += Util::CMSGTimeSpan(GetDaysInMonth(l_cTmpTime.GetYear(),
                            l_cTmpTime.GetMonth()) * CMSGTimeSpan::Day());
                l_cTmpTime.SetDay((WORD) 1 );
            }

            l_cTmpTime.SetHour(0);
            l_cTmpTime.SetMinute(0);
            l_cTmpTime.SetSecond(0);
            l_cTmpTime.SetMilliseconds(0);
            return l_cTmpTime;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }


    inline CMSGTime CMSGTime :: GetYearFloor() const
    {
        try
        {
            CMSGTime l_cTmpTime(*this);

            l_cTmpTime.SetMonth((WORD) 1 );
            // Set the lower stuff to the base
            l_cTmpTime.SetDay(1);
            l_cTmpTime.SetHour(0);
            l_cTmpTime.SetMinute(0);
            l_cTmpTime.SetSecond(0);
            l_cTmpTime.SetMilliseconds(0);
            return l_cTmpTime;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline CMSGTime CMSGTime :: GetYearCeiling() const
    {
        try
        {
            CMSGTime l_cTmpTime(*this);
            if (l_cTmpTime.GetMonth() > 1 || l_cTmpTime.GetDay() > 1 ||
                    l_cTmpTime.GetHour() > 0 || l_cTmpTime.GetMinute() > 0 ||
                    l_cTmpTime.GetSecond() > 0 || l_cTmpTime.GetMilliseconds() > 0)
            {
                l_cTmpTime += Util::CMSGTimeSpan(GetDaysInYear(l_cTmpTime.GetYear()) * CMSGTimeSpan::Day());
                l_cTmpTime.SetMonth((WORD) 1 );
            }

            // Set the lower stuff to the base.
            l_cTmpTime.SetDay(1);
            l_cTmpTime.SetHour(0);
            l_cTmpTime.SetMinute(0);
            l_cTmpTime.SetSecond(0);
            l_cTmpTime.SetMilliseconds(0);
            return l_cTmpTime;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }


    inline WORD CMSGTime :: GetMilliseconds() const
    {
        try
        {
            PAL_TIME_STRUCT l_sTimeHolder;
            BOOL l_Ret = PAL_absclass::PAL_fileTimeToSystemTime(&m_sTimeHolder, &l_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret,CNTException());
        #endif

            return l_sTimeHolder.wMilliseconds;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline WORD CMSGTime :: GetSecond() const
    {
        try
        {
            PAL_TIME_STRUCT l_sTimeHolder;

            BOOL l_Ret = PAL_absclass::PAL_fileTimeToSystemTime(&m_sTimeHolder, &l_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret,CNTException());
        #endif

            return l_sTimeHolder.wSecond;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline WORD CMSGTime :: GetMinute() const
    {
        PAL_TIME_STRUCT l_sTimeHolder;
        try
        {
            BOOL l_Ret = PAL_absclass::PAL_fileTimeToSystemTime(&m_sTimeHolder, &l_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret,CNTException());
        #endif
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
        return l_sTimeHolder.wMinute;
    }

    inline WORD CMSGTime :: GetHour() const
    {
        PAL_TIME_STRUCT l_sTimeHolder;
        try
        {
            BOOL l_Ret = PAL_absclass::PAL_fileTimeToSystemTime(&m_sTimeHolder, &l_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret,CNTException());
        #endif
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
        return l_sTimeHolder.wHour;
    }

    inline WORD CMSGTime :: GetDay() const
    {
        PAL_TIME_STRUCT l_sTimeHolder;
        try
        {
            BOOL l_Ret = PAL_absclass::PAL_fileTimeToSystemTime(&m_sTimeHolder, &l_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret,CNTException());
        #endif
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
        return l_sTimeHolder.wDay;
    }

    //
    // Get the start of the year,
    // Subtract today.
    // Divide by the number of days.
    //
    inline WORD CMSGTime :: GetDayOfYear() const
    {
        try
        {
            CMSGTime l_sYearStart(GetYear());

            l_sYearStart.SetMonth(1);
            Util::CMSGTimeSpan l_Span(*this - l_sYearStart);
            WORD l_RetVal = (WORD)l_Span.GetSpanInDays() + 1;
            return l_RetVal;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }
    //
    // Get the day of the year and then divide that by 7
    // To get the week.
    //

    inline WORD CMSGTime :: GetMonth() const
    {
        PAL_TIME_STRUCT l_sTimeHolder;
        try
        {
            BOOL l_Ret = PAL_absclass::PAL_fileTimeToSystemTime(&m_sTimeHolder, &l_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret,CNTException());
        #endif
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
        return l_sTimeHolder.wMonth;
    }


    inline WORD CMSGTime :: GetYear() const
    {
        PAL_TIME_STRUCT l_sTimeHolder;
        try
        {
            BOOL l_Ret = PAL_absclass::PAL_fileTimeToSystemTime(&m_sTimeHolder, &l_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret,CNTException());
        #endif
        }
        catch(...)
        {
            LogCatchAndThrow ;
        }
        return l_sTimeHolder.wYear;
    }


    inline PAL_FILETIME CMSGTime :: GetNTFileTime() const
    {
        return m_sTimeHolder;
    }

    inline PAL_TIME_STRUCT CMSGTime :: GetNTSystemTime() const
    {
        PAL_TIME_STRUCT l_sTimeHolder;
        try
        {
            BOOL l_Ret = PAL_absclass::PAL_fileTimeToSystemTime(&m_sTimeHolder, &l_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret,CNTException());
        #endif
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
        return l_sTimeHolder;
    }


    ///////////////////////////////////////////////////////////////
    //
    //	Class Name:		CMSGTime
    //
    //	Member Name:	IsEpoch
    //
    //	Author:			Gareth Bowen
    //
    //	Date:			22/10/1998
    //
    //	Purpose:		returnts a boolean value indicating if the current date object
    //					is set to the default date.
    //
    ////////////////////////////////////////////////////////////////


    inline bool CMSGTime::IsEpoch() const
    {

        try
        {
            CMSGTime l_temptime(*this);
            CMSGTime l_tempcompare;
            if(l_temptime == l_tempcompare)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }




    ///////////////////////////////////////////////////////////////
    //
    //	Member Name:	CMSGTime GetTheCurrentTime
    //
    //	Author:			Paul Baker
    //
    //	Date:			15/12/1997
    //
    //	Purpose:		Use the function GetLocalTime to return the time in a systemtime
    //					format. Then place that into a CMSGTime class.
    //
    ////////////////////////////////////////////////////////////////
    inline CMSGTime CMSGTime :: GetTheCurrentTime()
    {
        try
        {
            PAL_TIME_STRUCT l_sTime;

            // CJC	 7/13/00 5:50:42 PM Changed to GetLocalTime
            //				GetSystemTime(&l_sTime);
            PAL_absclass::PAL_getLocalTime(&l_sTime);
            CMSGTime l_cRet(l_sTime);
            return l_cRet;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }


    // Function name	: CMSGTime :: GetTheCurrentSystemTime
    // Description	    : Returns the Current SYSTEM time
    // Return type		: inline CMSGTime
    inline CMSGTime CMSGTime :: GetTheCurrentSystemTime()
    {
        try
        {
            PAL_TIME_STRUCT l_sTime;
            PAL_absclass::PAL_getSystemTime(&l_sTime);
            CMSGTime l_cRet(l_sTime);
            return l_cRet;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }
    //HZ Aug 05,2004
    inline CMSGTime CMSGTime :: GetTheCurrentSTTime()
    {
        Util::CMSGTime time;
#if defined ( _WIN32 )
        TIME_ZONE_INFORMATION tz;
        DWORD ret;
        DWORD bias;
#endif      // _WIN32
        try
        {
            time = Util::CMSGTime::GetTheCurrentTime();

#if defined ( _WIN32 )
            ret = GetTimeZoneInformation(&tz);
            switch(ret)
            {
                case TIME_ZONE_ID_UNKNOWN:         //DST is not being used in this Time Zone
                case TIME_ZONE_ID_STANDARD:        //The system is now in the mode of Standard Time
                    break;
                case TIME_ZONE_ID_DAYLIGHT:        //The System is now in the mode of Daylight Time
                    bias = tz.DaylightBias;        //convert the local time to ST time
                    time += Util::CMSGTimeSpan::Minute() * bias;
                    break;
                default:
                    break;
            }
#endif      // _WIN32
            return time;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    ///////////////////////////////////////////////////////////////
    //
    //	Class Name:		CMSGTime
    //
    //	Member Name:	FormatTime
    //
    //	Author:			Gareth Bowen
    //
    //	Date:			26/10/98
    //
    //	Purpose:		Use the supplied format string to return the time stored in
    //					the current object. This in reality converts the held information into
    //					the PAL_TIME_STRUCT structures and then calls the Win32 functionality
    //					GetTimeFormat. The function returns the desired string in the necessary
    //					format. Please note however that some care needs to be taken as the format string
    //					is case sensitive when using the Win32 functionality.
    //
    ////////////////////////////////////////////////////////////////
    inline std::wstring CMSGTime::FormatTime(std::wstring i_csFormat ) const
    {
        try
        {
            PAL_TIME_STRUCT l_sNTTime = GetNTSystemTime();
            BOOL l_Ret;
            int l_stringsize;

            l_stringsize = PAL_absclass::PAL_getTimeFormat(PAL_LOCALE_SYS_DEFAULT,
                    0,
                    &l_sNTTime,
                    i_csFormat.c_str(),
                    NULL,
                    0); //when this is 0 function returns the size of the buffer needed

            //std::auto_ptr<char> buffer(new char[l_stringsize]);
            std::wstring l_Buffer(l_stringsize,0);

            l_Ret = PAL_absclass::PAL_getTimeFormat(PAL_LOCALE_SYS_DEFAULT,
                    0,
                    &l_sNTTime,
                    i_csFormat.c_str(),
                    &l_Buffer[0], //return buffer
                    l_stringsize);  //size of return buffer
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret,Util::CNTException());
        #endif


            // convert the char array back into a string
            //
            return std::wstring(l_Buffer.c_str());
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    ///////////////////////////////////////////////////////////////
    //
    //	Class Name:		CMSGTime
    //
    //	Member Name:	FormatDate
    //
    //	Author:			Gareth Bowen
    //
    //	Date:			26/10/98
    //
    //	Purpose:		Use the supplied format string to return the time stored in
    //					the current object. This in reality converts the held information into
    //					the PAL_TIME_STRUCT structures and then calls the Win32 functionality
    //					GetDateFormat. The function returns the desired string in the necessary
    //					format. Please note however that some care needs to be taken as the format string
    //					is case sensitive when using the Win32 functionality.
    //
    ////////////////////////////////////////////////////////////////

    inline std::wstring CMSGTime::FormatDate(std::wstring i_csFormat ) const
    {
        try
        {
            PAL_TIME_STRUCT l_sNTTime = GetNTSystemTime();
            BOOL l_Ret;
            int l_stringsize;


            l_stringsize = PAL_absclass::PAL_getDateFormat(PAL_LOCALE_SYS_DEFAULT,
                    0,
                    &l_sNTTime,
                    i_csFormat.c_str(),
                    NULL,
                    0);  //this call returns the size if the buffer needed



            //std::auto_ptr<char> buffer(new char[l_stringsize]);
            std::wstring l_Buffer(l_stringsize,0);

            l_Ret = PAL_absclass::PAL_getDateFormat(PAL_LOCALE_SYS_DEFAULT,
                    0,
                    &l_sNTTime,
                    i_csFormat.c_str(),
                    &l_Buffer[0],
                    l_stringsize);
        #if defined(LINUX_FIX)
            Assert(l_Ret, Util::CPlatformException());
        #else
            Assert(l_Ret,Util::CNTException());
        #endif


            // convert the char array back into a string
            //
            return std::wstring(l_Buffer.c_str());
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }


    ///////////////////////////////////////////////////////////////
    //
    //	Class Name:		CMSGTime
    //
    //	Member Name:	Parse
    //
    //	Author:			Paul Baker
    //
    //	Date:			8/1/1998
    //
    //	Purpose:		Use the supplied date in the i_csTimeString
    //					Supply a time in a string and return a CMSGTime class
    //					The format will be in the form.
    //					YYYY.DDD.HH.MM.SS
    //					YYYY =	Year
    //					DDD	=	Day of Year
    //					HH	=	Hour of Day
    //					MM	=	Minute of Hour
    //					SS	=	Second of Minute.
    //					If any other string is supplied it will be ignored.
    //					Note: This does not alter the current object.
    //
    ////////////////////////////////////////////////////////////////

    inline CMSGTime CMSGTime::Parse(std :: wstring i_csTimeString )
    {
        try
        {

            TCHAR l_cDelimiter[2] = _T(".");
            WORD l_wTimeArray[E_TIME_PARSE_ELEMENTS];
            TCHAR tempStorage[E_MAX_DATE_SEGMENT_SIZE + 1];
            int l_iPos, l_iLastPos;
            int count;
            l_cDelimiter[1] = (TCHAR)NULL;
            WORD l_wMonth, l_wDay;

            l_iPos  = 0;
            for (count = 0 ; count < (E_TIME_PARSE_ELEMENTS-1) ; count++)
            {
                // *** Do we need to increment l_iPos ?....
                l_iLastPos = l_iPos;
                l_iPos = (int)i_csTimeString.find(l_cDelimiter,l_iLastPos);
                i_csTimeString.copy(tempStorage,(l_iPos - l_iLastPos),l_iLastPos);
#if defined ( _WIN32 )
                l_wTimeArray[count] = (WORD) _ttoi(tempStorage);
#else
                l_wTimeArray[count] = (WORD) wcstol(tempStorage, NULL, 10);
#endif
                memset(tempStorage,0,E_MAX_DATE_SEGMENT_SIZE + 1);
                l_iPos++;
            }

            //
            // The Last time element has a . on the left but not on the right so....
            //
            l_iLastPos = l_iPos;
            i_csTimeString.copy(tempStorage,(i_csTimeString.size() - l_iLastPos),l_iLastPos);
#if defined ( _WIN32 )
            l_wTimeArray[count] = (WORD) _ttoi(tempStorage);
#else
            l_wTimeArray[count] = (WORD) wcstol(tempStorage, NULL, 10);
#endif

            //
            // Time array is now in the required format so
            // create our new object.
            //

            DayOfYearToMonthDay(l_wTimeArray[0],l_wTimeArray[1],l_wMonth,l_wDay);
            CMSGTime l_TempTime(l_wTimeArray[0],l_wMonth,l_wDay,l_wTimeArray[2],l_wTimeArray[3],l_wTimeArray[4]);
            return l_TempTime;
        }
        catch(...)
        {
            CMSGTime i_tempdefault;
            return i_tempdefault;
        }

    }


    //
    // The easeiest method for the moment appears to be to convert to a system time
    // and then set all the elements below this one to the base value.
    //
    inline void CMSGTime :: SetMilliseconds(WORD i_wMilliseconds)
    {

        // *** convert to try & catch
        // TODO WORD is unsigned. gcc warns about this
        Assert( /* (i_wMilliseconds >= 0) && */ (i_wMilliseconds <999), CParamException());
        PAL_TIME_STRUCT l_sTimeHolder;
        try
        {
            BOOL l_Ret;
            // Get the file time
            l_Ret = PAL_absclass::PAL_fileTimeToSystemTime(&m_sTimeHolder, &l_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret, CNTException());
        #endif
            l_sTimeHolder.wMilliseconds = i_wMilliseconds;

            // Get the system time
            l_Ret = PAL_absclass::PAL_systemTimeToFileTime(&l_sTimeHolder,&m_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret, CNTException());
        #endif
        }
        catch (...)
        {
            LogCatchAndThrow ;
        }
        return;
    }

    inline void CMSGTime :: SetSecond(WORD i_wSecond)
    {
        // *** convert to try & catch
        // TODO WORD is unsigned. gcc warns about this
        Assert( /* (i_wSecond >= 0) && */ (i_wSecond <=59), CParamException());
        PAL_TIME_STRUCT l_sTimeHolder;
        try
        {
            BOOL l_Ret;
            l_Ret = PAL_absclass::PAL_fileTimeToSystemTime(&m_sTimeHolder, &l_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret, CNTException());
        #endif
            l_sTimeHolder.wSecond = i_wSecond;

            l_Ret = PAL_absclass::PAL_systemTimeToFileTime(&l_sTimeHolder,&m_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret, CNTException());
        #endif
        }
        catch (...)
        {
            LogCatchAndThrow ;
        }
        return;
    }

    inline void CMSGTime :: SetMinute(WORD i_wMinute)
    {
        // *** convert to try & catch
        // TODO WORD is unsigned. gcc warns about this
        Assert( /* (i_wMinute >= 0) && */ (i_wMinute <=59), CParamException());
        PAL_TIME_STRUCT l_sTimeHolder;
        try
        {
            BOOL l_Ret;
            l_Ret = PAL_absclass::PAL_fileTimeToSystemTime(&m_sTimeHolder, &l_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret, CNTException());
        #endif
            l_sTimeHolder.wMinute = i_wMinute;

            l_Ret  =PAL_absclass::PAL_systemTimeToFileTime(&l_sTimeHolder,&m_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret, CNTException());
        #endif
        }
        catch (...)
        {
            LogCatchAndThrow ;
        }
        return;
    }

    inline void CMSGTime :: SetHour(WORD i_wHour)
    {
        // TODO WORD is unsigned. gcc warns about this
        Assert( /* (i_wHour>= 0) && */ (i_wHour <= 23), CParamException());
        PAL_TIME_STRUCT l_sTimeHolder;
        try
        {
            // Convert to system time
            BOOL l_Ret;
            l_Ret = PAL_absclass::PAL_fileTimeToSystemTime(&m_sTimeHolder, &l_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret, CNTException());
        #endif

            // Modify the variable
            l_sTimeHolder.wHour = i_wHour;

            // Convert back
            l_Ret  =PAL_absclass::PAL_systemTimeToFileTime(&l_sTimeHolder,&m_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret, CNTException());
        #endif
        }
        catch (...)
        {
            LogCatchAndThrow ;
        }
        return;
    }


    inline void CMSGTime :: SetDay(WORD i_wDay)
    {
        PAL_TIME_STRUCT l_sTimeHolder;
        try
        {
            // Convert to system time
            BOOL l_Ret;
            l_Ret = PAL_absclass::PAL_fileTimeToSystemTime(&m_sTimeHolder, &l_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret, CNTException());
        #endif

            l_sTimeHolder.wDay = i_wDay;

            // Convert back
            l_Ret  =PAL_absclass::PAL_systemTimeToFileTime(&l_sTimeHolder,&m_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret, CNTException());
        #endif
        }
        catch (...)
        {
            LogCatchAndThrow ;
        }
        return ;
    }


    inline void CMSGTime :: SetDayOfYear(WORD i_wDay)
    {
        try
        {
            if(IsLeapYear(GetYear()))
            {
                Assert(i_wDay <= e_DaysInLeapYear, Util::CParamException());
            }
            else
            {
                Assert(i_wDay <= e_DaysInNormalYear, Util::CParamException());
            }
            WORD i_wdayNum = GetDayOfYear(); //get current day of year
            *this -= Util::CMSGTimeSpan(i_wdayNum	* Util::CMSGTimeSpan::Day());	//subtract timespan
            *this += Util::CMSGTimeSpan(i_wDay      * Util::CMSGTimeSpan::Day());	//add new day of year
        }
        catch (...)
        {
            LogCatchAndThrow;
        }
        return ;
    }

    inline void CMSGTime :: SetMonth(WORD i_wMonth)
    {
        Assert((i_wMonth >= 1) && (i_wMonth <=12), CParamException());
        PAL_TIME_STRUCT l_sTimeHolder;
        try
        {
            // Convert to system time
            BOOL l_Ret;
            l_Ret = PAL_absclass::PAL_fileTimeToSystemTime(&m_sTimeHolder, &l_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret, CNTException());
        #endif

            l_sTimeHolder.wMonth = i_wMonth;

            // Convert back
            l_Ret  =PAL_absclass::PAL_systemTimeToFileTime(&l_sTimeHolder,&m_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret, CNTException());
        #endif
        }
        catch (...)
        {
            LogCatchAndThrow ;
        }
        return;
    }


    inline void CMSGTime :: SetYear(WORD i_wYear)
    {
        Assert(i_wYear >= 1958, CParamException());
        PAL_TIME_STRUCT l_sTimeHolder;
        try
        {
            // Convert to system time
            BOOL l_Ret;
            l_Ret = PAL_absclass::PAL_fileTimeToSystemTime(&m_sTimeHolder, &l_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret, CNTException());
        #endif

            l_sTimeHolder.wYear = i_wYear;


            // Convert back
            l_Ret  =PAL_absclass::PAL_systemTimeToFileTime(&l_sTimeHolder,&m_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret, CNTException());
        #endif
        }
        catch (...)
        {
            LogCatchAndThrow ;
        }
        return;
    }

    ///////////////////////////////////////////////////////////////
    //
    //	Class Name:		CMSGTime
    //
    //	Member Name:	IntNetworkLength
    //
    //	Author:			Paul Baker
    //
    //	Date:			25/3/1998
    //
    //	Purpose:		Report he size of the element when to be sent over the network.
    //
    ////////////////////////////////////////////////////////////////
    inline unsigned int CMSGTime :: IntNetworkLength() const
    {
        try
        {
            // Do not like this but it needs to be done
            // to remove the dependency on NetworkLength
            return sizeof(PAL_FILETIME);
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    //
    // Range of operators to allow manipulation of the time class.
    //
    ///////////////////////////////////////////////////////////////
    //
    //	Member Name:	CMSGTime operator +
    //
    //	Author:			Paul Baker
    //
    //	Date:			15/12/1997
    //
    //	Purpose:		Ability to add a timespan to a timestamp.
    //
    ////////////////////////////////////////////////////////////////
    inline CMSGTime CMSGTime::operator + (const CMSGTimeSpan& i_cTimeSpan) const
    {
        try
        {
            CMSGTime l_cTempTime(*this);
            LARGE_INTEGER l_sLargeInt;

            l_sLargeInt.LowPart = l_cTempTime.m_sTimeHolder.dwLowDateTime;
            l_sLargeInt.HighPart = l_cTempTime.m_sTimeHolder.dwHighDateTime;
            l_sLargeInt.QuadPart += i_cTimeSpan.GetTimeSpan();
            l_cTempTime.m_sTimeHolder.dwLowDateTime = l_sLargeInt.LowPart;
            l_cTempTime.m_sTimeHolder.dwHighDateTime = l_sLargeInt.HighPart;
            return l_cTempTime;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    ///////////////////////////////////////////////////////////////
    //
    //	Member Name:	CMSGTime operator -
    //
    //	Author:			Paul Baker
    //
    //	Date:			15/12/1997
    //
    //	Purpose:		To be able to show the difference between two
    //					CMSGTime elements is the form of a timestamp
    //
    ////////////////////////////////////////////////////////////////
    inline CMSGTime CMSGTime:: operator - (const CMSGTimeSpan& i_cTimeSpan) const
    {
        try
        {
            CMSGTime l_cTempTime(*this);
            LARGE_INTEGER l_sLargeInt;

            l_sLargeInt.LowPart = l_cTempTime.m_sTimeHolder.dwLowDateTime;
            l_sLargeInt.HighPart = l_cTempTime.m_sTimeHolder.dwHighDateTime;
            l_sLargeInt.QuadPart -= ((CMSGTimeSpan&)i_cTimeSpan).GetTimeSpan();
            l_cTempTime.m_sTimeHolder.dwLowDateTime = l_sLargeInt.LowPart;
            l_cTempTime.m_sTimeHolder.dwHighDateTime = l_sLargeInt.HighPart;
            return l_cTempTime;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    ///////////////////////////////////////////////////////////////
    //
    //	Member Name:	CMSGTime operator -
    //
    //	Author:			Paul Baker
    //
    //	Date:			15/12/1997
    //
    //	Purpose:		To be able to show the difference between two
    //					CMSGTime elements is the form of a timestamp
    //					Do not operate on FILE_TIME directly. Convert to
    //					Large integers and then operate on that.
    //
    ////////////////////////////////////////////////////////////////
    inline CMSGTimeSpan CMSGTime::operator - (const CMSGTime& i_cTime) const
    {
        try
        {
            CMSGTimeSpan i_cTimeSpan;
            PAL_FILETIME ft_1;
            LARGE_INTEGER l_sTempInt1, l_sTempInt2;

            // Put the local time into the struct.
            l_sTempInt1.LowPart = m_sTimeHolder.dwLowDateTime;
            l_sTempInt1.HighPart = m_sTimeHolder.dwHighDateTime;

            // Put the parameters time into the struct.
            ft_1 = i_cTime.GetNTFileTime();
            l_sTempInt2.LowPart = ft_1.dwLowDateTime;
            l_sTempInt2.HighPart = ft_1.dwHighDateTime;

            l_sTempInt1.QuadPart -= l_sTempInt2.QuadPart;
            i_cTimeSpan.SetTimeSpan(l_sTempInt1.QuadPart);
            return i_cTimeSpan;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }






    inline CMSGTime& CMSGTime :: operator = (const CMSGTime& i_cTime)
    {
        m_sTimeHolder = i_cTime.GetNTFileTime();
        return (*this);
    }

    inline CMSGTime& CMSGTime :: operator = (const PAL_TIME_STRUCT& i_sTime)
    {
        try
        {
            BOOL l_Ret;
            l_Ret = PAL_absclass::PAL_systemTimeToFileTime(&i_sTime,&m_sTimeHolder);
        #if defined(LINUX_FIX)
            Assert(l_Ret, CPlatformException());
        #else
            Assert(l_Ret, CNTException());
        #endif
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
        return (*this);
    }

    inline CMSGTime& CMSGTime :: operator = (const PAL_FILETIME& i_cTime)
    {
        try
        {
            m_sTimeHolder = i_cTime;
            return (*this);
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline CMSGTime& CMSGTime :: operator += (const CMSGTimeSpan& i_cTimeSpan)
    {
        *this = *this + i_cTimeSpan;
        return *this;
    }


    inline CMSGTime& CMSGTime :: operator -= (const CMSGTimeSpan& i_cTimeSpan)
    {
        *this = *this - i_cTimeSpan;
        return *this;
    }



    inline bool CMSGTime :: operator == (const CMSGTime& i_cTime) const
    {
        try
        {
            if (m_sTimeHolder.dwLowDateTime == ((PAL_FILETIME)i_cTime.GetNTFileTime()).dwLowDateTime &&
                    m_sTimeHolder.dwHighDateTime == ((PAL_FILETIME)i_cTime.GetNTFileTime()).dwHighDateTime)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline bool CMSGTime :: operator < (const CMSGTime& i_cTime) const
    {
        try
        {
            if (m_sTimeHolder.dwHighDateTime == i_cTime.m_sTimeHolder.dwHighDateTime)
                return (m_sTimeHolder.dwLowDateTime < i_cTime.m_sTimeHolder.dwLowDateTime);
            else
                return (m_sTimeHolder.dwHighDateTime < i_cTime.m_sTimeHolder.dwHighDateTime);
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline bool CMSGTime :: operator > (const CMSGTime& i_cTime) const
    {
        try
        {
            if (!(*this < i_cTime) && !(*this == i_cTime))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline bool CMSGTime :: operator <= (const CMSGTime& i_cTime) const
    {
        try
        {
            if ((*this < i_cTime) || (*this == i_cTime))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline bool CMSGTime :: operator >= (const CMSGTime& i_cTime) const
    {
        try
        {
            if ((*this > i_cTime) || (*this == i_cTime))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }








    //////////////////////////////////////////////////
    //
    // Title:	Implementation detail for CMSGTimeSpan....
    //
    // Author:	Paul Baker
    //
    //
    // Date:	15/12/1997
    //
    //
    // Purpose:	To allow the manipulation of the times.
    //			will enable times to be added subtracted etc...
    //
    ///////////////////////////////////////////////////



    inline CMSGTimeSpan :: CMSGTimeSpan(__int64 i_wHundredNanosecs)
    {
            m_iTimeSpan = i_wHundredNanosecs; 
    }

    inline CMSGTimeSpan :: CMSGTimeSpan(const CMSGTimeSpan&  i_cTimeSpan)
    {
        try
        {
            m_iTimeSpan = i_cTimeSpan.GetTimeSpan();
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline CMSGTimeSpan :: ~CMSGTimeSpan()
    {
        return;
    }

    // Getter functions.
    inline __int64 CMSGTimeSpan :: GetSpanInDays() const
    {
        try
        {
            return m_iTimeSpan / CMSGTimeSpan::Day();

        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }


    inline __int64 CMSGTimeSpan :: GetSpanInHours() const
    {
        try
        {
            return m_iTimeSpan / CMSGTimeSpan::Hour();

        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }


    inline __int64 CMSGTimeSpan :: GetSpanInMinutes() const
    {
        try
        {
            return m_iTimeSpan / CMSGTimeSpan::Minute();

        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }


    inline __int64 CMSGTimeSpan :: GetSpanInSeconds() const
    {
        try
        {
            return m_iTimeSpan / CMSGTimeSpan::Second();

        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }


    inline __int64 CMSGTimeSpan :: GetSpanInMilliseconds() const
    {
        try
        {
            return m_iTimeSpan / CMSGTimeSpan::Millisecond();
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }



    // More Getter functions.
    inline WORD CMSGTimeSpan :: GetDayPart() const
    {
        try
        {
            return (WORD)(m_iTimeSpan / CMSGTimeSpan::Day());
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

    }


    inline WORD CMSGTimeSpan :: GetHourPart() const
    {
        try
        {
            return (WORD)((m_iTimeSpan % CMSGTimeSpan::Day().GetTimeSpan()) / CMSGTimeSpan::Hour());
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

    }


    inline WORD CMSGTimeSpan :: GetMinutePart() const
    {
        try
        {
            return (WORD)((m_iTimeSpan % CMSGTimeSpan::Hour().GetTimeSpan()) / CMSGTimeSpan::Minute()) ;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

    }


    inline WORD CMSGTimeSpan :: GetSecondPart() const
    {
        try
        {
            return (WORD)((m_iTimeSpan % CMSGTimeSpan::Minute().GetTimeSpan()) / CMSGTimeSpan::Second());
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

    }




    inline WORD CMSGTimeSpan :: GetMillisecondPart() const
    {
        return (WORD)((m_iTimeSpan % CMSGTimeSpan::Second().GetTimeSpan()) / CMSGTimeSpan::Millisecond());
    }

    inline __int64  CMSGTimeSpan :: GetTimeSpan() const
    {
        return m_iTimeSpan;
    }


    //
    // Setter functions.
    //

    // Initially these will be simple functions ie just set the value, but this will come into
    // a more complex for in the future i.e. set seconds and it returns weeks/seconds time span if the
    // seconds are large..
    // ***
    inline void CMSGTimeSpan :: SetSpanInDays(const __int64 i_wDays)
    {
        try
        {
            m_iTimeSpan = i_wDays * CMSGTimeSpan::Day();
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

    }

    inline void CMSGTimeSpan :: SetSpanInHours(const __int64 i_wHours)
    {
        try
        {
            m_iTimeSpan= i_wHours * CMSGTimeSpan::Hour();
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

    }

    inline void CMSGTimeSpan :: SetSpanInMinutes(const __int64 i_wMinutes)
    {
        try
        {
            m_iTimeSpan  = i_wMinutes * CMSGTimeSpan::Minute();
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

    }

    inline void CMSGTimeSpan :: SetSpanInSeconds(const __int64 i_wSeconds)
    {
        try
        {
            m_iTimeSpan = i_wSeconds * CMSGTimeSpan::Second();
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

    }

    inline void CMSGTimeSpan :: SetSpanInMilliseconds(const __int64 i_wMilliseconds)
    {
        try
        {
            m_iTimeSpan = i_wMilliseconds * CMSGTimeSpan::Millisecond();
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

    }

    inline void CMSGTimeSpan :: SetTimeSpan(__int64 i_iTimeSpan)
    {
            m_iTimeSpan = i_iTimeSpan;
    }

    inline void CMSGTimeSpan :: SetTimeSpan(const CMSGTimeSpan& i_cTimeSpan)
    {
        m_iTimeSpan = i_cTimeSpan.GetTimeSpan();
        return;
    }


    //
    // Range of Inc functions.
    // They attempt to update the current timespan with the specified
    // range whilst keeping within the bounds of the current time portion.
    //
    inline void CMSGTimeSpan :: IncMilliseconds(__int64 i_wMilliseconds)
    {
        try
        {
            m_iTimeSpan += i_wMilliseconds * CMSGTimeSpan::Millisecond() ;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

        return;
    }

    inline void CMSGTimeSpan :: IncSecond(__int64 i_wSeconds)
    {
        try
        {
            m_iTimeSpan += i_wSeconds * CMSGTimeSpan::Second();
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

        return;
    }

    inline void CMSGTimeSpan :: IncMinute(__int64 i_wMinutes)
    {
        try
        {
            m_iTimeSpan += i_wMinutes * CMSGTimeSpan::Minute();
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

        return;
    }

    inline void CMSGTimeSpan :: IncHour(__int64 i_wHours)
    {
        try
        {
            m_iTimeSpan += i_wHours * CMSGTimeSpan::Hour();
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

        return;
    }

    //
    // Note this member function cannot increment the months as we do not know which month we are in
    // More processing needs to be done at the CMSGTime end of things to resolve this one.
    //
    inline void CMSGTimeSpan :: IncDay(__int64 i_wDays)
    {
        try
        {
            m_iTimeSpan += i_wDays * CMSGTimeSpan::Day();
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

        return;
    }

    //
    // Range of VDec functions.
    // They attempt to Decrement the current timespan with the specified
    // range whilst keeping within the bounds of the current time portion.
    //
    inline void CMSGTimeSpan :: DecMilliseconds(__int64 i_wMilliseconds)
    {
        try
        {
            m_iTimeSpan -= i_wMilliseconds * CMSGTimeSpan::Millisecond() ;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

        return;
    }

    inline void CMSGTimeSpan :: DecSecond(__int64 i_wSeconds)
    {
        try
        {
            m_iTimeSpan -= i_wSeconds * CMSGTimeSpan::Second();
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

        return;
    }

    inline void CMSGTimeSpan :: DecMinute(__int64 i_wMinutes)
    {
        try
        {
            m_iTimeSpan -= i_wMinutes * CMSGTimeSpan::Minute();
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

        return;
    }

    inline void CMSGTimeSpan :: DecHour(__int64 i_wHours)
    {
        try
        {
            m_iTimeSpan -= i_wHours * CMSGTimeSpan::Hour();
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

        return;
    }

    //
    // Note this member function cannot decrement the months as we do not know which month we are in
    // More processing needs to be done at the CMSGTime end of things to resolve this one.
    //
    inline void CMSGTimeSpan :: DecDay(__int64 i_wDays)
    {
        try
        {
            m_iTimeSpan -= i_wDays * CMSGTimeSpan::Day();
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

        return;
    }


    // Operations
    inline CMSGTimeSpan CMSGTimeSpan :: operator + (const CMSGTimeSpan& i_TimeSpan) const
    {
        try
        {
            CMSGTimeSpan l_TmpSpan(*this);
            l_TmpSpan.SetTimeSpan(GetTimeSpan() + i_TimeSpan.GetTimeSpan());
            return l_TmpSpan;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline CMSGTimeSpan CMSGTimeSpan :: operator * (const int i_iMultiplier) const
    {
        try
        {
            CMSGTimeSpan l_TmpSpan(*this);
            l_TmpSpan.SetTimeSpan(GetTimeSpan() * i_iMultiplier);
            return l_TmpSpan;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline CMSGTimeSpan CMSGTimeSpan :: operator / (const int i_iMultiplier) const
    {
        try
        {
            CMSGTimeSpan l_TmpSpan(*this);
            l_TmpSpan.SetTimeSpan(GetTimeSpan() / i_iMultiplier);
            return l_TmpSpan;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline CMSGTimeSpan CMSGTimeSpan :: operator - (const CMSGTimeSpan& i_TimeSpan) const
    {
        try
        {
            CMSGTimeSpan l_TmpSpan(*this);
            l_TmpSpan.SetTimeSpan(GetTimeSpan() - i_TimeSpan.GetTimeSpan());
            return l_TmpSpan;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }


    inline CMSGTimeSpan& CMSGTimeSpan :: operator = (const CMSGTimeSpan& i_TimeSpan)
    {
        try
        {
            m_iTimeSpan = i_TimeSpan.GetTimeSpan();
            return *this;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline CMSGTimeSpan& CMSGTimeSpan :: operator += (const CMSGTimeSpan& i_TimeSpan)
    {
        //		try
        //		{
        *this = *this + i_TimeSpan;
        return *this;
        //		}
        //		catch(...)
        //		{
        //			LogCatchAndThrow;
        //		}
    }

    inline CMSGTimeSpan& CMSGTimeSpan :: operator -= (const CMSGTimeSpan& i_TimeSpan)
    {
        *this = *this - i_TimeSpan;
        return *this;
    }

    // Boolean comparators.
    inline bool CMSGTimeSpan :: operator == (const CMSGTimeSpan& i_TimeSpan) const
    {
        if (m_iTimeSpan == i_TimeSpan.GetTimeSpan())
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    inline bool CMSGTimeSpan :: operator > (const CMSGTimeSpan& i_TimeSpan) const
    {
        try
        {
            if (m_iTimeSpan > i_TimeSpan.GetTimeSpan())
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline bool CMSGTimeSpan :: operator < (const CMSGTimeSpan& i_TimeSpan) const
    {
        try
        {
            if (!(*this > i_TimeSpan) && !(*this == i_TimeSpan))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }


    inline bool CMSGTimeSpan :: operator <= (const CMSGTimeSpan& i_TimeSpan) const
    {
        try
        {
            if ((*this < i_TimeSpan) || (*this == i_TimeSpan))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline bool CMSGTimeSpan :: operator >= (const CMSGTimeSpan& i_TimeSpan) const
    {
        try
        {
            if ((*this > i_TimeSpan) || (*this == i_TimeSpan))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }


    inline __int64 operator * ( const __int64 i_iMultiplier,const Util::CMSGTimeSpan& i_Span  )
    {
        try
        {
            __int64 l_iMult;
            l_iMult = (__int64) i_iMultiplier;
            return l_iMult * i_Span.GetTimeSpan();
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline __int64 operator + ( const __int64 i_iAdd,const Util::CMSGTimeSpan& i_Span  )
    {
        __int64 l_iAdd(i_iAdd);
        return l_iAdd + i_Span.GetTimeSpan();
    }

    inline __int64 operator / ( const __int64 i_iDiv,const Util::CMSGTimeSpan& i_Span  )
    {
        try
        {
            __int64 l_iDiv(i_iDiv);
            return l_iDiv / i_Span.GetTimeSpan();
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

    }

    inline __int64 operator - ( const __int64 i_iSub,const Util::CMSGTimeSpan& i_Span  )
    {
        try
        {
            __int64 l_iSub(i_iSub);
            return l_iSub - i_Span.GetTimeSpan();
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    //
    // Output operator.
    //

    inline   std :: wostream& operator << (std :: wostream& o_Stream, const Util::CMSGTime& time )
    {
        try
        {
            return o_Stream << time.FormatTime() << std::wstring(_T(" ")) << time.FormatDate() << std :: endl ;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

    }
    inline   std :: ostream& operator << (std :: ostream& o_Stream, const Util::CMSGTime& time )
    {
        try
        {
            std::wstring out;
            out = time.FormatTime();
            out += std::wstring(_T(" "));
            out += time.FormatDate();

            std::string  str;
            Util::wchartostring(str,out.c_str());
            return o_Stream << str << std::endl;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

    }

    inline   std :: wostream& operator << (std :: wostream& o_Stream, const Util::CMSGTimeSpan& span)
    {
        try
        {
            return o_Stream	<< std::wstring(_T("Days: ")) << span.GetDayPart() << std::wstring(_T(", Hour: "))
                << span.GetHourPart()
                << std::wstring(_T(", Minute: "))
                << span.GetMinutePart()
                << std::wstring(_T(", Second: "))
                << span.GetSecondPart()
                << std::wstring(_T(", Millisecond: "))
                << span.GetMillisecondPart() << std :: endl ;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }


    inline std :: istream& operator >> (std :: istream& i_Stream, Util::CMSGTime& time )
    {
        try
        {
            PAL_FILETIME f1;
            i_Stream >> f1.dwLowDateTime >> f1.dwHighDateTime ;
            time = f1;
            return i_Stream ;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline std :: istream& operator >> (std :: istream& i_Stream, Util::CMSGTimeSpan& /*span*/)
    {
        /* __int64 l_Span;
           i_Stream >> l_Span;
           span = l_Span;*/
        return i_Stream ;
    }





    inline CMSGTimeSpan& CMSGTimeSpan::Millisecond()
    {
        try
        {
            static  CMSGTimeSpan l_TimeSpan(10000);
            return l_TimeSpan;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline CMSGTimeSpan& CMSGTimeSpan::Second()
    {
        try
        {
            static  CMSGTimeSpan l_TimeSpan(1000 * Millisecond());
            return l_TimeSpan;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline CMSGTimeSpan& CMSGTimeSpan::Minute()
    {
        try
        {
            static  CMSGTimeSpan l_TimeSpan(60*Second());
            return l_TimeSpan;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline CMSGTimeSpan& CMSGTimeSpan::Hour()
    {
        try
        {
            static  CMSGTimeSpan l_TimeSpan(60*Minute());
            return l_TimeSpan;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    inline CMSGTimeSpan& CMSGTimeSpan::Day()
    {
        try
        {
            static  CMSGTimeSpan l_TimeSpan(24*Hour());
            return l_TimeSpan;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    ///////////////////////////////////////////////////////////////
    //
    //	Class Name:		CMSGTimeSpan
    //
    //	Member Name:	IntNetworkLength
    //
    //	Author:			Paul Baker
    //
    //	Date:			25/3/1998
    //
    //	Purpose:		Report the size of the element when to be sent over the network.
    //
    ////////////////////////////////////////////////////////////////
    inline unsigned int CMSGTimeSpan:: IntNetworkLength() const
    {
        try
        {
            return sizeof(__int64);
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

    }



    ///////////////////////////////////////////////////////////////
    //
    //	Class Name:		CMSGTimeSpan
    //
    //	Member Name:	ExtNetworkLength
    //
    //	Author:			Paul Baker
    //
    //	Date:			25/3/1998
    //
    //	Purpose:		This cannot be called.
    //
    ////////////////////////////////////////////////////////////////
    //	inline unsigned int CMSGTimeSpan:: ExtNetworkLength() const
    //	{
    //		try
    //		{
    //			Assert(0,Util::CIncompatibleNetworkPrototype());
    //			return (unsigned int)-1;
    //		}
    //		catch(...)
    //		{
    //			LogCatchAndThrow;
    //		}
    //	}

    ///////////////////////////////////////////////////////////////
    //
    //	Class Name		Global Function
    //
    //	Member Name:	GetDaysInMonth
    //
    //	Author:			Paul Baker
    //
    //	Date:			15/12/1997
    //
    //	Purpose:		Return the number of Days in the month
    //
    ////////////////////////////////////////////////////////////////
    inline WORD GetDaysInMonth (WORD i_wYear, WORD i_wMonth)
    {
        try
        {
            static WORD g_wDaysInMonth [] = {0,31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            if (IsLeapYear(i_wYear) && i_wMonth == 2)
            {
                return g_wDaysInMonth[i_wMonth] + 1;
            }
            else
            {
                return g_wDaysInMonth[i_wMonth];
            }
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }


    ///////////////////////////////////////////////////////////////
    //
    //	Class Name		Global Function
    //
    //	Member Name:	IsLeapYear
    //
    //	Author:			Paul Baker
    //
    //	Date:			15/12/1997
    //
    //	Purpose:		Is the Year a Leap Year
    //
    ////////////////////////////////////////////////////////////////
    inline bool IsLeapYear(WORD w_Year)
    {

        // If the year is divisible by 400 it is a leap year
        // If the year is divisible by 4 and not 100 then it is a leap year
        try
        {
            return (!(w_Year % 400) || (!(w_Year % 4) && (w_Year % 100)));
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

    }


    ///////////////////////////////////////////////////////////////
    //
    //	Class Name		Global Function
    //
    //	Member Name:	GetDaysInYear
    //
    //	Author:			Paul Baker
    //
    //	Date:			23/12/1997
    //
    //	Purpose:		Get the number of days in the year
    //
    ////////////////////////////////////////////////////////////////
    inline WORD GetDaysInYear(WORD w_Year)
    {
        try
        {
            return IsLeapYear(w_Year) ? 366 : 365 ;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

    }

    // TODO: gcc complains about the following for loop. Perhaps there is a problem in there
    inline void  DayOfYearToMonthDay(WORD i_wYear, WORD i_wDayOfYear, WORD& o_wMonth, WORD& o_wDay)
    {
        WORD count;
        try
        {
            //
            // Are the parameters in the right area
            //
            Assert(i_wDayOfYear <= GetDaysInYear(i_wYear),CParamException());
            (o_wMonth) = Util::CMSGTime::e_Jan;

            // Loop around
            for (count  =  GetDaysInMonth(i_wYear,(o_wMonth)); 
                #if defined(LINUX_FIX)
                    count < i_wDayOfYear;       // ***** WARNING: How come "(o_wMonth) <= Util::CMSGTime::e_Dec" has no effect under Linux ?
                #else
                    (o_wMonth) <= Util::CMSGTime::e_Dec, count < i_wDayOfYear;  // ***** WARNING: keep this for reference
                #endif
                    (o_wMonth)++,i_wDayOfYear = i_wDayOfYear - count, count = GetDaysInMonth(i_wYear,(o_wMonth)));

            (o_wDay) = i_wDayOfYear;

            Assert((o_wDay) <= GetDaysInMonth(i_wYear,(o_wMonth)) && (o_wMonth) < Util::CMSGTime::e_MSG_MONTH_TYPE_LEN
                    && (o_wMonth) > Util::CMSGTime::e_MSG_MONTH_TYPE_START,
                    CNamedException(_T("Invalid Day or Month Generated")));
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
        return;
    }

    inline void  MonthDayToDayOfYear(WORD i_wYear, WORD i_wMonth, WORD i_wDay,WORD& o_wDayOfYear)
    {
        WORD count;
        try
        {
            //
            // Are the parameters in the right area
            //
            Assert(i_wDay <= GetDaysInMonth(i_wYear,i_wMonth) && i_wMonth < Util::CMSGTime::e_MSG_MONTH_TYPE_LEN
                    && i_wMonth > Util::CMSGTime::e_MSG_MONTH_TYPE_START, CParamException());
            (o_wDayOfYear)  = 0;
            for (count  =  Util::CMSGTime::e_MSG_MONTH_TYPE_START; count < i_wMonth; count++)
            {
                (o_wDayOfYear) = (o_wDayOfYear) + GetDaysInMonth(i_wYear,count);
            }
            (o_wDayOfYear) = (o_wDayOfYear) + i_wDay;
            Assert((o_wDayOfYear) <= GetDaysInYear(i_wYear),CNamedException(_T("Invalid Day Generated")));
            return;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

#if defined ( _WIN32 )
#pragma warning( pop )
#endif      // _WIN32

} //  end of the namespace

#endif
