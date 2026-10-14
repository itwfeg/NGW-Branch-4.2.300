// HobartScaleService.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "ErrorHandling.h"
#include "OutputLogger.h"
#include "CHobartScaleService.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _DEBUG

extern "C"
{
    Util::CSmartCriticalSection aCrit;
    void SetMainErrorLogger( Util::CSmartPtr<Util::COutputLogger> & pother)
    {
        try
        {
            PAL_absclass::PAL_OutputDbgStr(L" SetMainErrorLogger called \r\n");
            if( Util::COutputLogger::GetErrorLogPtr().get() == NULL)
            {
                Util::CLockCriticalSection lock(aCrit);
                if( Util::COutputLogger::GetErrorLogPtr().get() == NULL)
                {
                    std::wstring image;
                    Util::COutputLogger::GetImageFile(image);
                    Util::COutputLogger::GetErrorLogger(image.c_str(),L"HobartScaleService");
                }
            }
            pother =     Util::COutputLogger::GetErrorLogPtr();
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    };
}

int main(int argc, char *argv[])
{
#ifdef _DEBUG
    // Add braces here to create an additional local context which 
    // can be exited prior to calling _CrtDumpMemoryLeaks so any local 
    // vars aren't reported as leaks.
    { 
#endif // _DEBUG

#if  0 //DBG
        DebugBreak();
#endif

        static wchar_t s[250];
        wsprintf(s,L"main: ScaleService.exe built on %s %s\r\n",TEXT(__DATE__),TEXT(__TIME__));
        PAL_absclass::PAL_OutputDbgStr(s);

        _set_se_translator(Util::TranslateSE);

        PAL_absclass::PAL_OutputDbgStr(L"main: creating ref to CHobartScaleService\r\n");
        Hobart::CHobartScaleService & scale_service
            = Hobart::CHobartScaleService::Instance(L"Hobart Scale Service");
        try
        {
            // Parse arguments! If called without arguments, FALSE is returned
            // So: to install the service, use -i option

            // TODO convert char to wchar and pass
            // TRS
            PAL_absclass::PAL_OutputDbgStr(L"main: calling CHobartScaleService.ParseStandardArgs\r\n");
            if (!scale_service.ParseStandardArgs(argc, argv))
            {
                PAL_absclass::PAL_OutputDbgStr(L"main: calling CHobartScaleService.StartService\r\n");
                scale_service.StartService();
            }
            else
            {
                PAL_absclass::PAL_OutputDbgStr(L"main: parse args failed, did not start service\r\n");
            }
        }
        catch(...)
        {
            std::wcout << L"main: Input Error." << std::endl;
            PAL_absclass::PAL_OutputDbgStr(L"main: exception caught\r\n");
        }
        PAL_absclass::PAL_OutputDbgStr(L"main: exiting ScaleService.exe\r\n");

#ifdef _DEBUG
    } // exit local context prior to dumping leaks

    //_CrtDumpMemoryLeaks();

#endif // _DEBUG

#if 0 //def _DEBUG
    MessageBox(NULL,L"Service has shut down successfully",L"Hobart Scale Service",MB_OK | MB_SERVICE_NOTIFICATION | MB_SETFOREGROUND);
#endif

    return 0;
}
