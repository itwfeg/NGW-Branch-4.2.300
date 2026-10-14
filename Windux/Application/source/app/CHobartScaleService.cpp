// CHobartScaleService.cpp: implementation of the CCHobartScaleService class.
//
//////////////////////////////////////////////////////////////////////

#include <PAL_platformheader.h>

#include "OutputLogger.h"
#include "DADFResourceUtils.h"
#include "CHobartScaleService.h"
#include "Main.h"
#include "UsbManager.h"
#include "Hex.h"
#include "HobString.h"
//#include "device_event.h"
#include "SessionManagerHelper.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif

#include "MessageLogger.h"

#pragma warning (disable: 4311)

namespace Hobart 
{

#ifdef USE_ACCOUNTANT

    int CHobartScaleService::dataType = 0;
    std::string CHobartScaleService::dataTypeName("CHobartScaleService"); 
#endif

    Util::CSmartPtr<CHobartScaleService> CHobartScaleService::mp_Instance(NULL);
    Util::CSmartCriticalSection CHobartScaleService::m_CritSec;
    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    // Function name	: CHobartScaleService::CHobartScaleService
    // Description	    : constructor
    // Return type		: 
    // Argument         : const std::wstring& i_ServiceName
    CHobartScaleService::CHobartScaleService(const std::wstring& i_ServiceName)
        : Util::CNTService(i_ServiceName)
    {
#ifdef USE_ACCOUNTANT
        if (dataType == 0)
            dataType = util_acc::registerDataType(dataTypeName);

        util_acc::addInstance(dataType);
#endif        
        notify_handler = NULL;
		PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService: contructor\r\n");   
#ifdef _DEBUG
        Debug::setEnabled(true);
        //Debug::setTraceAllCategories(true);
        Debug::addTraceCategory("ScaleService");
    	Debug::trace(HobString("ScaleService"), HobString("ScaleService.exe"));
        //Debug::setTraceAllCategoriesOf("UsbManager");
        //Debug::setTraceAllCategoriesOf("CUsbPipe");
        //Debug::setTraceAllCategoriesOf("CUsbCommBase");
        //Debug::setTraceAllCategoriesOf("EZUSB");
        //Debug::setTraceAllCategoriesOf("EvolWeigherCodeUpdater");
        //Debug::setTraceAllCategoriesOf("IUsbCodeUpdater");
        //Debug::setTraceAllCategoriesOf("EvolPrinterCodeUpdater");
        //Debug::setTraceAllCategoriesOf("CUPrinterCommManager");
        //Debug::setTraceAllCategoriesOf("UsbWrapperCodeUpdater");
        //Debug::setTraceAllCategoriesOf("Main");
        //Debug::setTraceAllCategoriesOf("Weigher");
        //Debug::setTraceAllCategoriesOf("WeigherCommManager");
        //Debug::setTraceAllCategoriesOf("WeigherManager");
        //Debug::setTraceAllCategoriesOf("Printer");
        //Debug::setTraceAllCategoriesOf("PrinterCommManager");
        //Debug::setTraceAllCategoriesOf("EvolWrapper");
        //Debug::setTraceAllCategoriesOf("RealTimeController");
        //Debug::setTraceAllCategoriesOf("CTLSThread");
        //Debug::setTraceAllCategoriesOf("VersionManager");
        //Debug::setTraceAllCategoriesOf("AtmelSambaUpdater");
        //Debug::setTraceAllCategoriesOf("AtmelDeviceHandler");

        //Debug::setTraceAllCategoriesOf("SoftwareSystem");
        //Debug::setTraceAllCategoriesOf("VersionManager");
        //Debug::setTraceAllCategoriesOf("SoftwareSystem.upgrade");
        //Debug::setTraceAllCategoriesOf("firmware.upgrade");

        //Debug::setTraceAllCategoriesOf("APCCommManager");
        //Debug::setTraceAllCategoriesOf("APCCommManager.PrinterSys");
        //Debug::setTraceAllCategoriesOf("APCCommManager.Printer");
        //Debug::setTraceAllCategoriesOf("APCCommManager.WeigherSys");
        //Debug::setTraceAllCategoriesOf("APCCommManager.Weigher");
        //"APCCommManager.Error"
        //"APCCommManager.Message.Out"

        //"APCCommManager.Numeric.Message.Out"
        //"APCCommManager.Marquee.Message.Out"
        //"APCCommManager.PrinterSys.Message.Out"
        //"APCCommManager.Printer.Message.Out"
        //"APCCommManager.WeigherSys.Message.Out"
        //"APCCommManager.Weigher.Message.Out"
        //"APCCommManager.System.Message.Out"

        //"APCCommManager.Numeric.Message.In"
        //"APCCommManager.Marquee.Message.In"
        //"APCCommManager.PrinterSys.Message.In"
        //"APCCommManager.Printer.Message.In"
        //"APCCommManager.WeigherSys.Message.In"
        //"APCCommManager.Weigher.Message.In"

        //"APCCommManager.UsbIrp"
        //"APCCommManager.BulkData"

#endif
    }

    // Function name	: CHobartScaleService::CHobartScaleService
    // Description	    : destructor
    // Return type		: none
    // Argument         : none
    CHobartScaleService::~CHobartScaleService()
    {
#ifdef USE_ACCOUNTANT
        util_acc::removeInstance(dataType);
#endif       
		PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService: destructor\r\n");   
    }


    // Function name	: CCHobartScaleService::Instance
    // Description	    : return singleton instance pointer
    // Return type		: CCHobartScaleService & 
    // Argument         : const std::wstring & i_ServiceName
    CHobartScaleService & CHobartScaleService::Instance(const std::wstring & i_ServiceName)
    {
        try
        {
            if( mp_Instance == NULL)
            {
                Util::CLockCriticalSection lock(m_CritSec);
                if( mp_Instance == NULL)
                    mp_Instance = new CHobartScaleService(i_ServiceName);
            }

            return *mp_Instance.get();
        }
        catch(...)
        {
	    	Debug::trace(HobString("ScaleService"), HobString("CHobartScaleService::Instance(name): exception"));
			PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService::Instance(name): exception \r\n");   
            LogCatchAndThrow;
        }
    }

    // Function name	: CCHobartScaleService::Instance
    // Description	    : return singleton instance pointer
    // Return type		: CCHobartScaleService & 
    // Argument         : none
    CHobartScaleService & CHobartScaleService::Instance()
    {

        try
        {
            if( mp_Instance == NULL)
            {
                Util::CLockCriticalSection lock(m_CritSec);
                if( mp_Instance == NULL)
                    mp_Instance = new CHobartScaleService(L"Hobart Scale Service");
            }
            return *mp_Instance.get();
        }
        catch(...)
        {
	    	Debug::trace(HobString("ScaleService"), HobString("CHobartScaleService::Instance(): exception"));
			PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService::Instance(): exception \r\n");   
            LogCatchAndThrow;
        }
    }

    // Function name	: CCHobartScaleService::OnInit
    // Description	    : 
    // Return type		: bool 
    // Argument         : none
    bool CHobartScaleService::OnInit()
    {
		PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService:OnInit: entry\r\n");
        try
        {
            PAL_absclass::PAL_OutputDbgStr(L"Initializing Hobart Scale Service\r\n");
        }
        catch(...)
        {
	    	Debug::trace(HobString("ScaleService"), HobString("CHobartScaleService::OnInit: exception"));
            LogCatch; 
            ReportToErrorLog(L"A serious exception has occured.");
			PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService:OnInit: exception. init failed\r\n");
            return false;
        }

		PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService:OnInit: init succeeded. \r\n");
        return true;
    };

    void CHobartScaleService::OnStop()
    {
		PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService:OnStop: entry\r\n");
        try
        {
            DoUnregisterDeviceInterface(notify_handler);
            StopUsbManager();
			MessageLogger::logMessage(MessageLogger::InfoLog, "Start Panic shutdown");
			SessionManagerHelper::getInstance()->panicShutdown();
			MessageLogger::logMessage(MessageLogger::InfoLog, "ScaleService stopped");
        }
        catch (...)
        {
	    	Debug::trace(HobString("ScaleService"), HobString("CHobartScaleService::OnStop: exception"));
			PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService:OnStop: handled exception\r\n");
        }
		PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService:OnStop: exit\r\n");
    }

    void CHobartScaleService::OnShutdown()
    {
		PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService:OnShutdown: entry\r\n");
        try
        {
            DoUnregisterDeviceInterface(notify_handler);
            StopUsbManager();
			MessageLogger::logMessage(MessageLogger::InfoLog, "Start Panic shutdown");
			SessionManagerHelper::getInstance()->panicShutdown();
			MessageLogger::logMessage(MessageLogger::InfoLog, "ScaleService shutdown");
        }
        catch (...)
        {
	    	Debug::trace(HobString("ScaleService"), HobString("CHobartScaleService::OnShutdown: exception"));
			PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService:OnShutdown: handled exception\r\n");
        }
		PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService:OnShutdown: exit\r\n");
    }

    void CHobartScaleService::OnDeviceEvent(DWORD i_EventType, LPVOID i_EventData, LPVOID UNUSED(i_Context) )
    {
        // Note: This is equivalent to WM_DEVICECHANGE.
        //          i_EventType = wParam;
        //          i_EventData = lParam;
        try
        {
            Debug::trace("ScaleService", HobString("OnDeviceEvent: received notification, data ") 
                    + HobString(Hex::toString(i_EventData))
                    + HobString(" type ") 
                    + HobString(Hex::toString((long)i_EventType)) );

            PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR) i_EventData;

            if (   lpdb->dbch_devicetype ==  DBT_DEVTYP_DEVICEINTERFACE 
                    || lpdb->dbch_devicetype == DBT_DEVTYP_HANDLE )
            {
				EVENT_INFO ev;

				ev.service = GetStatusHandle();
				ev.event_dat = i_EventData;
				ev.event_type = i_EventType;
                UsbManager::getInstance()->HandleDeviceNotification(&ev);
            }
        }
        catch (...)
        {
	    	Debug::trace(HobString("ScaleService"), HobString("CHobartScaleService::OnDeviceEvent: exception"));
            Debug::trace("ScaleService", HobString("OnDeviceEvent: exception"));
        }
    }

    PAL_HDEVNOTIFY CHobartScaleService::DoRegisterDeviceInterface()
    {
        // Construct the device notification filter
        // which limits the notifications to only those
        // of type DBT_DEVTYP_DEVICEINTERFACE.
        DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;

        ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
        NotificationFilter.dbcc_size = 	sizeof(DEV_BROADCAST_DEVICEINTERFACE);
        NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

        // Register for device notification messages for ALL interface classes
        // to be sent to the service handle. The CNTService base class then calls 
        // our notification handler OnDeviceEvent, which decides what to do based 
        // on the notification contents.
        PAL_HDEVNOTIFY h = PAL_absclass::PAL_registerDeviceNotification( GetStatusHandle(), 
                &NotificationFilter,
                DEVICE_NOTIFY_SERVICE_HANDLE | DEVICE_NOTIFY_ALL_INTERFACE_CLASSES	);
        if (h == NULL)
        {
            DWORD err = PAL_absclass::PAL_getLastError();
            Debug::trace("ScaleService", 
                    HobString("DoRegisterDeviceInterface: failed. error ") 
                    + HobString(Hex::toString(err)) );
        }
        return h;
    }

    // Function name	: CCHobartScaleService::Run
    // Description	    : executive loop for service, note : NOT a CTLSThread Run function
    // Return type		: void 
    void CHobartScaleService::Run()
    {
		PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService:Run: entry\r\n");
        try
        {
            ReportToErrorLog(L"Hobart Scale Service started.");

			StallForRemoteDebugger();

			PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService:Run: calling EvolMain::startAll\r\n");
            EvolMain main;
            main.startAll( 1, 0, 0 );
			
#ifdef _DEBUG
            // EvolMain::startAll loads the default configuration, which contains a debug enable 
            // property that forces setEnabled(false). Therefore, setEnabled(true) must be 
            // called here to override that.
#ifdef REENABLE_DEBUG_AFTER_CONFIG_LOAD
            Debug::setEnabled(true);
#endif
            
#endif
            notify_handler = DoRegisterDeviceInterface();
			PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService:Run: registered for device notifications\r\n");

            // Enumerate all USB devices and launch required service classes.
            // Note: this must be done after registering for device notifications,
            // because the enumeration process may launch device firmware updaters 
            // which cause device resets after the firmware is downloaded, which in turn
            // causes removal/arrival notifications for the device in question. 
            Debug::trace("ScaleService", HobString("Run: enumerating USB devices"));
			PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService::Run: enumerating USB devices\r\n");
			
			UsbManager::getInstance()->EnumerateDevices();

            //
            // wait forever.
            //
            Util::MSGWaitForSingleObject(ms_ShutdownEvent);

			PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService::Run: shutdown event signaled.\r\n");
        }
        catch(...)
        {
	    	Debug::trace(HobString("ScaleService"), HobString("CHobartScaleService::Run: unspecified exception"));
			PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService::Run: unspecified exception.\r\n");
            LogCatch;
        }

        ReportToErrorLog(L"Hobart Scale Service terminated. ");
		PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService:Run: exit\r\n");
    };

    void CHobartScaleService::ReportToErrorLog(const wchar_t * pbuff)
    {
        try
        {
            Util::COutputLogger::ReportToErrorLog(pbuff,L"");
        }
        catch(...)
        {
	    	Debug::trace(HobString("ScaleService"), HobString("CHobartScaleService::ReportToErrorLog: exception"));
			PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService::ReportToErrorLog:exception\r\n");
            LogCatchAndThrow;
        }
    }    

    void CHobartScaleService::StallForRemoteDebugger()
    {
        // If the appropriate registry flag is set, display a message box 
        // prompting for remote debug connection. this allow an opportunity
        // to "Attach to process" from within VS2005 in order to remote debug
        // the service application running on the target system. 

        // If the registry flag is not set, continues without blocking.

        DWORD dwRemoteDebugStall = 0;
        HKEY hKey = NULL;

        if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T ("SYSTEM\\CurrentControlSet\\Services\\Hobart Scale Service"),
		            0, KEY_QUERY_VALUE, &hKey))
        {
            // found the key, read the value
            DWORD dwValue = 0;
            DWORD keySize = sizeof(DWORD);

            if (ERROR_SUCCESS == RegQueryValueExA(hKey, "RemoteDebugStall", NULL, NULL, (unsigned char*) &dwValue, &keySize))
            {
                dwRemoteDebugStall = dwValue;
            }
            RegCloseKey(hKey);
        }

        if (dwRemoteDebugStall)
        {
			PAL_absclass::PAL_sleep(dwRemoteDebugStall * 1000);
			PAL_absclass::PAL_sleep(1000);
	        //PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService:Run: ATTACH DEBUGGER NOW\r\n");
            //MessageBox(NULL,L"Attach remote debugger now",L"Hobart Scale Service",MB_OK | MB_SERVICE_NOTIFICATION | MB_SETFOREGROUND);
        }
    }


    void CHobartScaleService::DoUnregisterDeviceInterface(PAL_HDEVNOTIFY h)
    {
	    PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService:: unregistering device notifications\r\n");
        PAL_absclass::PAL_unregisterDeviceNotification( h );
    }

    void CHobartScaleService::StopUsbManager()
    {
        try
        {
            PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService: stopping UsbManager\r\n");
            UsbManager::getInstance()->RemoveAllDevices();
            //delete (UsbManager::getInstance());
        }
        catch(...)
        {
	    	Debug::trace(HobString("ScaleService"), HobString("CHobartScaleService::StopUsbManager: exception"));
            PAL_absclass::PAL_OutputDbgStr(L"CHobartScaleService: exception while stopping UsbManager\r\n");
        }
    }

} // end namespace Hobart
