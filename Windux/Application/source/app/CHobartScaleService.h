// CHobartScaleService.h: interface for the CHobartScaleService class.
//
//////////////////////////////////////////////////////////////////////

#if !defined CHobartScaleService_h
#define CHobartScaleService_h

#include "CNTService.h"
#include "CCriticalSection.h"
#include "CSmartPtr.h"
#include "CHobartScaleService.h"

namespace Hobart
{

    class CHobartScaleService  : public Util::CNTService
    {
    public:
		virtual ~CHobartScaleService();
		bool OnInit();
		void OnDeviceEvent(DWORD i_EventType, LPVOID i_EventData, LPVOID i_Context );
		void Run();
		void ReportToErrorLog(const wchar_t * pbuff);
		static CHobartScaleService & Instance(const std::wstring & i_ServiceName);
		static CHobartScaleService & Instance();
		void OnStop();
        void OnShutdown();

    protected:
        CHobartScaleService(const std::wstring& i_ServiceName);

    private:
        PAL_HDEVNOTIFY DoRegisterDeviceInterface();
        void StallForRemoteDebugger();
        void DoUnregisterDeviceInterface(PAL_HDEVNOTIFY h);
        void StopUsbManager();


    private:

        PAL_HDEVNOTIFY notify_handler;
        static Util::CSmartCriticalSection m_CritSec;
        static Util::CSmartPtr<CHobartScaleService> mp_Instance;

#ifdef USE_ACCOUNTANT
            static int dataType;
            static std::string dataTypeName;
#endif

    };

} // end namespace

#endif //#if !defined CHobartScaleService_h
