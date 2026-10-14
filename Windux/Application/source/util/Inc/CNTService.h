// Header file included by ANTLR UniLexer
//#include <tchar.h>
#ifndef CNTService_h
#define CNTService_h
#include <vector>

// Ignore bool conversion warnings
#pragma warning( disable : 4800 ) 

/**************************************************************************
****  DADF MODULE HEADER   ***

TYPE:			Base Class 

PURPOSE:		WinNT Service Control Manager (SCM) Interface


FUNCTION:		This class  provides the interface to the WinNT Service
				Control Manager (SCM). It provides the functionality
				to install, deinstall a service into or from the 
				SCM-database. And it provides basic functionality to control
				a service. Controlling means to start, stop or abort a service
				and to set up a service handler which can receive control requests
				(commands) from other applications.


INTERFACES:		In addition to the following comments, refer to
				'INTERFACES:' in the module declaration below.
				The constructor of this class needs the name of the service 
				to be set in the SCM database. Additional it sets up the initial 
				service status as a Win32 process. 
				All other functions are pure virtual (to be overwritten 
				in derived	classes) or provide basic service 
				functionalty used by the SCM.
				
  

RESOURCES:		negligible


REFERENCES:		Visual C++ Online Help, topic: Windows NT Service API


PROCESSING:		CNTService()
				The constructor initialize the service status and the name 
				of the service.

				ParseStandardArgs()
				This function parses and executes the command line parameters 
				passed to the service excutable, if it is started as a console 
				application.
				This class provides two command line options: 
				
				1) -i:	install service. (That means create an entry in the
						service control database (SCM))
				2) -u:	deinstall service from the SCM database.

				Note that a service can only be started via SCM interface. That 
				means the service executable, started from a console, can only 
				install, deinstall itself or can return the current version 
				of itself. To start the processing of a service the SCM 
				interface is needed.

				Note:	the -i and -u options are not part of the Unit testing,
						because they are supposed not to be used (except as 
						means to install and deinstall services for testing).
				
				ServiceMain()
				The function is the function that a service program 
				specifies as the entry-point function of the service.
				The parameters of this function are a string array with 
				parameters passed via the SCM interface to the service.

				Handler()
				This function is a function that a service specifies as the 
				control handler function of the Win32-based service. 
				Other processes can send control codes to the service via the 
				SCM interface. The service invokes this function whenever a control
				code is passed to the service. The handler dispatches the control
				request.

				GetServiceName()
				This function returns a std::string containing the service name.


DATA:			See 'DATA:' in the class header below.


****  END MODULE HEADER   ***
**************************************************************************/
#include <PAL_platformheader.h>
#include "SmartHandle.h"

// define abort operation code
#define SERVICE_ABORT 128

#include "CTLSThread.h"
#include "CSmartPtr.h"

#ifdef ENABLE_SERVICE_KILLER
namespace Util
{
    class CServiceKiller : public Util::CRunnableClass
	{
	public:
		CServiceKiller(DWORD iTimeout = 120000);
		virtual ~CServiceKiller();
		DWORD Run();
	private:
		Util::CSmartPtr< Util::CTLSThread > mp_Thread;
		const DWORD m_Timeout;

	#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
	#endif

	};
};
#endif

namespace Util
{
	class CNTService
	{
		public:

        // INTERFACES:

        CNTService(const std::wstring& i_ServiceName,bool ibReportVersion = true);

		/*
        Description:
        The constructor sets the name of the service and sets all (default)
        configuration parameters of the service.
        Arguments:
        i_ServiceName	Name of the servcice in the SCM database
        Returns: none
        */

        virtual ~CNTService();
        /*
        Description:
        The destructor is empty in this base class.
        Arguments:
        Returns: none
        */

        virtual bool ParseStandardArgs(int i_argc, char* i_argv[]);
		/*
        Description:
        Parses and executes the command line parameters passed to
        the service executable when it is started from a console.
        Arguments:
        i_argc	count of commandline parameters
        i_argv	string array contains the command line parameters
        Returns: FALSE, if called without or with a wrong parameter
        TRUE, else
        */

        bool StartService();
        /*
        Description:
        The function calls the system function StartServiceCtrlDispatcher().
        This system function connects the main thread of a service process to the SCM, 
        which causes the thread to be the service control dispatcher thread for the 
        calling process. 
        Arguments:	none
        Returns: TRUE, if StartServiceCtrlDispatcher was called successfully
        FALSE, else
        */

        static void PAL_API ServiceMain(DWORD i_Argc, TCHAR** i_Argv);
        /*
        Description:
        The function is the function that a service specifies 
        as the entry-point function of the service.  
        Arguments:
        i_Argc	count of parameters passed via the SCM
        i_Argv	string array with the start parameters passd via the SCM.
        Returns: none (if this function returns, the service has stopped)
        */


		static LONG _stdcall UnhandledExceptionFilter(PAL_LPEXCEPTION_POINTERS lpexpExceptionInfo);

		/*
        Description:
        Called by the operating system in case of an unhandled exception
        Arguments:
        lpexpExceptionInfo: structure filled by the function with inforamtion 
        about the exception

        Returns: the exception-filter code
        */

		static void PAL_API TerminateFunction();

		/*
        Description:
        Called by the Operationsystem in case of an unhandled exception
        Arguments: none

        Returns: none
        */


        static DWORD PAL_API HandlerEx(DWORD i_Opcode, DWORD i_EventType, LPVOID i_EventData, LPVOID i_Context );
        /*
        Description:
        This function is a function that a service specifies as the 
        control handler function of the Win32-based service. 
        Other processes can send control codes to the service via the 
        SCM interface. The service invokes this function whenever a control
        code is passed to the service. The handler dispatches the control
        request.
        Arguments:
        i_Opcode  the control code
        Returns: none
        */

        static std::wstring GetServiceName();
        /*
        Description:
        This function return the service name.
        Arguments:
        none
        Returns: std::string containing the service name.
        */

    protected:
        // CJC Jan/10 2003, added function to report version info and process executable name 
        // to error log
        void ReportVersionInfoToErrorLog();

        // INTERFACES:

        virtual void Run();
        /*
        Description:
        This function must be overwritten in the derived service class. This 
        function includes the main processing.
        Arguments:	none
        Returns:	none
        */

        virtual bool OnInit();
        /*
        Description:
        This function could be overwritten in a derived service class to provide
        special initialisations.
        Arguments:	none
        Returns:	TRUE
        */

        virtual void OnStop();
        /*
        Description:
        This function could be overwritten in a derived service class to provide
        a special stop mechanism.
        Arguments:	none
        Returns:	none
        */

        virtual void OnInterrogate();
        /*
        Description:
        Requests the service to update immediately its current status 
        information to the SCM. Could be overrwriiten in a derived class.
        Arguments:	none
        Returns:	none
        */

        virtual void OnPause();
        /*
        Description:
        This function could be overwritten in a derived service class to provide
        a pause status of the service.
        Arguments:	none
        Returns:	none
        */

        virtual void OnContinue();
        /*
        Description:
        This function could be overwritten in a derived service class to provide
        a continuation of the service after it has paused.
        Arguments:	none
        Returns:	none
        */

        virtual void OnDeviceEvent(DWORD i_EventType, LPVOID i_EventData, LPVOID i_Context );

        /*
        Description:
        This function could be overwritten in a derived service class to provide
        function to check device PnP.
        Arguments:	none
        Returns:	none
        */

        virtual void OnShutdown();
        /*
        Description:
        This function could be overwritten in a derived service class to provide
        a shutdown mechanism.
        Arguments:	none
        Returns:	none
        */

        virtual bool OnAbort();
        /*
        Description:
        This function could be overwritten in a derived service class to provide
        an abort mechanism.
        Arguments:	none
        Returns:	if it returns, it returns FALSE (process was not terminated sucessfully)
        */

        void SetStatusError(DWORD dwError);

        void SetStatus(DWORD dwState);
        /*
        Description:
        This function sets the current state of the service.
        Arguments: 
        i_State	the new state of the service
        Returns:	none
        */

        void SetStatusEx( 
            DWORD dwState, 
            DWORD dwWin32ExitCode, 
            DWORD dwServiceSpecificExitCode, 
            DWORD dwCheckPoint,
            DWORD dwWaitHint
            );

        SERVICE_STATUS_HANDLE & GetStatusHandle(){ return m_hServiceStatus;}
        SERVICE_STATUS & GetStatus(){ return m_Status;}
        // DATA:
        //	The following event handle provides overall control of the SU
        static Util::CSmartHandle	ms_ShutdownEvent;
        // A vector containing the strings passed to ServiceMain as argument
        std::vector<std::wstring> m_ServiceArguments;

    private:

		// DEFINITION:
        #define MAX_ACCOUNT_PASSWORD_SIZE       (63)    // For Install()

        static const DWORD dwShutdownHint =18000;

        // DATA:
        //	name of the service in the SCM database
        std::wstring					m_ServiceName;
        //	service status handle
        SERVICE_STATUS_HANDLE		m_hServiceStatus;
        //	structure which provides the service status
        SERVICE_STATUS				m_Status;
        //	nasty hack to get object ptr
        static CNTService*			ms_pThis; 


        // INTERFACES:
        bool Initialize();
        /*
        Description:
        This function initializes the service when it starts.
        Arguments:	none
        Returns:	TRUE on success, FALSE else
        */

        bool IsInstalled();
        /*
        Description:
        This function tests whether the service is installed in the SCM database.
        Arguments:	none
        Returns:	none
        */

        // CJC	 8/17/00 11:46:49 AM
        // Added account and password to allow network access

		bool Install(char * pAccount = NULL, char * pPassword = NULL);

		/*
        Description:
        This function installes the services. That means it creates an entry
        for the service in the SCM database.
        Arguments: none
        Returns:	TRUE on success, FALSE else
        */

        bool Uninstall();
        /*
        Description:
        This function deinstalls the service. It deletes the entry from the 
        SCM database.
        Arguments:	none
        Returns:	TRUE on success, FALSE else
        */

#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif

        // Description:	Returns a refernece to the shutdown event.
        // Returns:		Nothing.
    public:
        static Util::CSmartHandle& GetShutdownEvent()
        {
            return ms_ShutdownEvent;
        }


#if _DEBUG
		void Util::CNTService::DebugMsg(const wchar_t* pszFormat, ...);
#endif
	};

} // end namespace Util

#endif
