#include "PAL_platformheader.h"

#include "HTTPUnitTester.h"



#include "HTTPURI.h"
#include "HTTPResponse.h"
#include "cgi/CGIHTTPAdapter.h"

#include "PAL_platformheader.h"



namespace comm
{


    HTTPUnitTester::HTTPUnitTester()
    {
#if !defined ( __linux__ )
        WSADATA wsaData;
        int err = WSAStartup( MAKEWORD( 2, 0 ), &wsaData );

        Assert(err == 0, Util::CNamedException( L"Socket Initialize error" ) );

        Assert(( LOBYTE( wsaData.wVersion ) == 2 ||HIBYTE( wsaData.wVersion ) == 0 ), Util::CNamedException( L"Socket Version Error" ) );
#endif      // !__linux__
    }


    bool HTTPUnitTester::testHTTPURIParsing()
    {
        HTTPURI uri("");
        if ( uri.isValid() )
            return false;

        uri.setLocation("dsd");
        if ( uri.isValid() )
            return false;

        uri.setLocation(":dsd");
        if ( uri.isValid() )
            return false;

        uri.setLocation("abc:def");
        if ( uri.isValid() )
            return false;

        uri.setLocation("1http:def");
        if ( uri.isValid() )
            return false;

        uri.setLocation("htt?:ded");
        if ( uri.isValid() )
            return false;

        uri.setLocation("hTTp:ded");
        if ( uri.isValid() )
            return false;

        uri.setLocation("hTTp:/ded");
        if ( uri.isValid() )
            return false;

        uri.setLocation("hTTp://ded");
        if ( !uri.isValid() )
            return false;

        uri.setLocation("hTTp://ded/");
        if ( !uri.isValid() )
            return false;

        uri.setLocation("hTTp://ded/a");
        if ( !uri.isValid() )
            return false;

        uri.setLocation("hTTp://ded/a/#");
        if ( !uri.isValid() )
            return false;

        uri.setLocation("hTTp://ded.def/a/?#");
        if ( !uri.isValid() )
            return false;

        uri.setLocation("hTTp://1ded-g.def/a/#");
        if ( !uri.isValid() )
            return false;

        uri.setLocation("hTTp://1ded-g.2def/a/#");
        if ( uri.isValid() )
            return false;

        uri.setLocation("hTTp://53.com.us:8080/a?a=1&b=2#new");
        if ( !uri.isValid() )
            return false;

        uri.setLocation("hTTp://10.3.198.45:8080/a/");
        if ( !uri.isValid() )
            return false;

        uri.setLocation("hTTp://10.3.198.45.:8080/a/");
        if ( uri.isValid() )
            return false;

        uri.setLocation("hTTp://10.3.198./a/");
        if ( uri.isValid() )
            return false;

        uri.setLocation("hTTp://10.3.198.45.22/a/");
        if ( uri.isValid() )
            return false;

        uri.setLocation("hTTp://10.3.198.45:a22/a/");
        if ( uri.isValid() )
            return false;

        return true;
    }


    bool HTTPUnitTester::testHTTPURIQueries()
    {
        HTTPURI uri("");
        uri.setLocation("hTTp://53.com.us:8080/a?a=1&b=2#new");
        if ( !uri.isValid() )
            return false;

        uri.setQueryParameter("a", "2");
        uri.setQueryParameter("b", "1");

        if ( uri.getQueryParameter("a") != "2" )
            return false;
        if ( uri.getQueryParameter("b") != "1" )
            return false;

        uri.getQuery();

        HTTPURI::parameterNames_iterator iter = uri.queryBegin();
        while ( iter != uri.queryEnd() )
        {
            if ( *iter == "a" )
                uri.setQueryParameter(*iter, util_string::longtostring(1L));
            else if ( *iter == "b" )
                uri.setQueryParameter(*iter, util_string::longtostring(2L));

            ++iter;
        }

        if ( uri.getQueryParameter("a") != "1" )
            return false;
        if ( uri.getQueryParameter("b") != "2" )
            return false;

        uri.formatURI();

        return true;
    }


    bool HTTPUnitTester::testHTTPResponse()
    {
        HTTPResponse response;
        /*
           if ( response.processStatusLine("") )
           return false;

           if ( response.processStatusLine("HTTP1.0") )
           return false;

           if ( response.processStatusLine("HTTP/11 200") )
           return false;

           if ( response.processStatusLine("HTTP/11 200 1.1") )
           return false;

           if ( response.processStatusLine("HTTP/1.1 2be ") )
           return false;

           if ( response.processStatusLine("HTTP/1.1 20 ") )
           return false;

           if ( response.processStatusLine("HTTP/1.1 2000 ") )
           return false;

           if ( !response.processStatusLine("HTTP/1.1 200") )
           return false;

           if ( !response.processStatusLine("HTTP/1.1 200 ") )
           return false;

           if ( !response.processStatusLine("HTTP/1.1 020 ") )
           return false;

           if ( !response.processStatusLine("HTTP/1.1 999 ") )
           return false;

           if ( !response.processStatusLine("http/1.1 200 This is a good reason") )
           return false;
           */
        if ( !response.processStatusLine("HTTP/1.1 200 This too is a good reason;sdfafk;klkj./dkj]\\ffffff") )
            return false;
        /*
           if ( response.processHeaderLine("HTTP/1.1 200 This is a good reason") )
           return false;

           if ( response.processHeaderLine("name ; value") )
           return false;

           if ( response.processHeaderLine("name : value") )
           return false;
           */
        if ( !response.processHeaderLine("Content-Length: 10000") )
            return false;

#if defined(LINUX_FIX)
        PAL_absclass::PAL_OutputDbgStr("     HTTP Response:\n");
        PAL_absclass::PAL_OutputDbgStr(response.dumpContents().c_str());
#else
        OutputDebugStringA("     HTTP Response:\n");
        OutputDebugStringA(response.dumpContents().c_str());
#endif

        return true;
    }


    bool HTTPUnitTester::testHTTPClient()
    {
        CGIHTTPAdapter adapter;

        adapter.performCGICall("http://10.3.198.167/cgi-bin/fpp.cgi?a=1&b=2");
        adapter.performCGICall("http://10.3.198.167/fpp.cgi?a=1&b=2");
        //		adapter.performCGICall("http://qawebserver/ptcgi/fpp.cgi?a=1&b=2");
        //		adapter.performCGICall("http://qawebserver/fpp.cgi?a=1&b=2");
        //		adapter.performCGICall("http://www.yahoo.com/ptcgi/fpp.cgi?a=1&b=2");
        //		adapter.performCGICall("http://www.yahoo.com/fpp.cgi?a=1&b=2");
        //		adapter.performCGICall("http://www.microsoft.com/ptcgi/fpp.cgi?a=1&b=2");
        //		adapter.performCGICall("http://www.microsoft.com/fpp.cgi?a=1&b=2");

#if defined(LINUX_FIX)
        PAL_absclass::PAL_sleep( 20*60*1000 );
#else
        Sleep(20*60*1000);
#endif

        return true;
    }


}
