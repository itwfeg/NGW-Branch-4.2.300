/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "evolution.h"
#include "MemShow.h"
#include "SessionManager.h"
#include "Main.h"
#include "Configuration.h"
#include "Debug.h"
#include "Integer.h"
#include "UInteger.h"
#include "Hex.h"
#include "Long.h"
#include "RealTimeController.h"
#include "UsbManager.h"
#include "VersionManager.h"

#include "DBManager.h"

#include "WSSession.h"
#if defined ( _WIN32 )
#include <shellapi.h>
#endif

#if defined( __linux__ )
	#include <sys/resource.h>
#endif


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolMain::_dataType_ = 0;
std::string EvolMain::_dataTypeName_("EvolMain"); 
#endif


EvolMain::EvolMain()
{
#if !defined ( __linux__ )
    try
    {
#ifdef USE_ACCOUNTANT
        if (_dataType_ == 0)
            _dataType_ = util_acc::registerDataType(_dataTypeName_);

        util_acc::addInstance(_dataType_);
#endif

        WORD wVersionRequested;
        WSADATA wsaData;
        int err;

        wVersionRequested = MAKEWORD( 2, 0 );

        err = WSAStartup( wVersionRequested, &wsaData );
        Assert(err == 0, Util::CNamedException( L"Socket Initialize error" ) );

        Assert(( LOBYTE( wsaData.wVersion ) == 2 ||HIBYTE( wsaData.wVersion ) == 0 ), Util::CNamedException( L"Socket Version Error" ) );
    }
    catch(...)
    {
        LogCatchAndThrow;
    }
#endif
}

EvolMain::~EvolMain()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
}

void EvolMain::initializeDatabase()
{
    /* Because this method is dependent on an internal Configuration parameter,
     * the Configuration::getInstance() method, with the parameter specifying the
     * directory of config.dat, needs to be called before calling this method.
     * This is currently handled by the startConfig() method below.
     */
    try
    {
        Properties *dbConfiguration = (Properties *)NULL;

        std::string dbDefaultsFileName(Configuration::getInstance()->getDbDefaultsFile());

        Debug::trace("Main", HobString("initializeDatabase() db defaults file: ") + dbDefaultsFileName);

        dbConfiguration = new Properties(dbDefaultsFileName);
        if (dbConfiguration)
        {
            Debug::trace("Main", "initializeDatabase() before DBManager::setConfiguration()");
            DBManager::setConfiguration(dbConfiguration);
            Debug::trace("Main", "initializeDatabase() after  DBManager::setConfiguration()");
        }
        else
            Debug::trace("Main", "initializeDatabase() - dbConfiguration Properties object is NULL");

        Debug::trace("Main", "initializeDatabase() before DBManager::setDebug()");
        DBManager::setDebug(Trace::getInstance());
        Debug::trace("Main", "initializeDatabase() after  DBManager::setDebug()");
    }
    catch (DBException e)
    {
        if (Debug::isTracing("DBAccess.exception"))
        {
            Debug::trace("DBAccess.exception", HobString("Main::initializeDatabase() ********** database exception **********"));
            DBException::Type t = e.getType();
            Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + e.info());
        }
    }
}


void EvolMain::startConfig(char *intConfigDirectoryName, char *netConfigDirectoryName)
{
    // If a directory name is passed in for the internal config file, update CfgInternal's static variable for the file name.
    if (intConfigDirectoryName && strlen(intConfigDirectoryName))
        CfgInternal::setInternalConfigDirectory(intConfigDirectoryName);
    
	Configuration *config = Configuration::getInstance();

	// The following will load internal configuration.
	config->loadCfgInternal();

	// Need to run the following after the above loading of CfgInternal was run
    Debug::trace("Main", "network config directory name is: " + CfgInternal::getNetworkConfigFileNameStr());
	config->loadCfgNetwork();


    /* The following method call has to occur after the Configuration::getInstance() above,
     * since an internal configuration parameter is use to open the database.
     * The database is not opened by the following call, only static variables are set;
     * thus, we still don't have database access.
     * The database will be opened by the SessionManager, since the SessionManager is a task
     * that will live as long as the scale is operating, and it's preferable (soon to be required)
     * that a VxWorks task that accesses the database has to continue to be alive in order to
     * access the database (once the task dies, all dbManagers will be deleted).
     * Because evolMain() will eventually die, it should be the one to open the database.
     */
    initializeDatabase();

}

void EvolMain::startAll(int ssncnt, char *internalConfigDirectoryName, char *networkConfigDirectoryName, char *primaryInputFileName, char *remoteInputFileName)
{
    /* Debug (Trace) is not setup yet, so do not try to debug anything yet. */
	util_filesystem::File cfgDebugFile(DEBUG_CONFIG_FILE_NAME);
	if (cfgDebugFile.exists())
	{
		Properties traceProps(DEBUG_CONFIG_FILE_NAME);
		Trace::getInstance(traceProps);
	}
	else
		Trace::getInstance();


    DWORD delayTime = Trace::getInstance()->getDebugStartupDelay();
	if (delayTime)
	{
	    Debug::trace("Main", HobString(" bootup delaying for ") + HobString(Long::toString((long)delayTime)));
		PAL_absclass::PAL_sleep(delayTime*1000);
	}


    char *intConfigDir = (char *)NULL;
    if (internalConfigDirectoryName)
        intConfigDir = internalConfigDirectoryName;

    char *netConfigDir = (char *)NULL;
    if (networkConfigDirectoryName)
        netConfigDir = networkConfigDirectoryName;

    if (!primaryInputFileName)
        primaryInputFileName = NULL;
    if (!remoteInputFileName)
        remoteInputFileName = NULL;


    startConfig(intConfigDir, netConfigDir);

#if defined( __linux__ )
	if ( Configuration::getInstance()->toBoolCfgI(CfgInternal::COREDUMPENABLED) )
	{
        // core dumps may be disallowed by parent of this process; change that
        struct rlimit core_limits;
        core_limits.rlim_cur = core_limits.rlim_max = RLIM_INFINITY;
        setrlimit(RLIMIT_CORE, &core_limits);
	}

	// allow 16mb stack
    struct rlimit stack_limit;

    if ( 0 == getrlimit( RLIMIT_STACK, &stack_limit ) )
    {
    	stack_limit.rlim_cur = 16 * 1024 * 1024;

    	if (( RLIM_INFINITY != stack_limit.rlim_max ) && ( stack_limit.rlim_max < stack_limit.rlim_cur ))
    		stack_limit.rlim_max = stack_limit.rlim_cur;

    	setrlimit( RLIMIT_STACK, &stack_limit );
    }
#endif

    if (Debug::isTracing("Memory"))
        memShow(1);

    Debug::trace("Main", "Evolution Application\n");
    Debug::trace("Main", HobString("parameters input to evolMain():"));
    Debug::trace("Main", HobString("  sessionCount=") + HobString(Integer::toString(ssncnt)));
    Debug::trace("Main", HobString("  internal config directory name=") + HobString(intConfigDir));
    Debug::trace("Main", HobString("  network  config directory name=") + HobString(netConfigDir));
    Debug::trace("Main", HobString("  primary filename=") + HobString(primaryInputFileName));
    Debug::trace("Main", HobString("  remote filename=") + HobString(remoteInputFileName));

	if (Configuration::getInstance()->isSessionManagerEnabled())
	{
		int sMgrDelay = Configuration::getInstance()->toIntCfgI(CfgInternal::SESSIONMANAGERSTARTUPDELAY);

		Debug::trace("Main", "Starting Session Manager ...");
		SessionManager::getInstance();
		PAL_absclass::PAL_sleep( (sMgrDelay > 0) ? sMgrDelay : 100 );

		if (sMgrDelay == 0)
		{
			SessionManager *sm = SessionManager::getInstance();
			if (sm)
			{
				while ( !sm->isInitialized() )
				{
					Debug::trace("Main", "Waiting for SessionManager to initialize ...");
					PAL_absclass::PAL_sleep(250);
				}
			}
		}
	}

    // TODO HFC project
    if (Configuration::getInstance()->isRealTimeControllerEnabled())
    {

        Debug::trace("Main", "Starting Real-Time Controller ...");
        RealTimeController::getInstance();
        PAL_absclass::PAL_sleep(500);



#if !defined ( __linux__ )
        // register Atmel AT91Boot_DLL.DLL COM server
        File unregAtmelDll(Configuration::getInstance()->toStringCfgI(CfgInternal::EVOLUTIONDIRECTORY), "AT91Boot_DLL.unreg");
        if (unregAtmelDll.exists())
        {
            File registeredAtmelDll(unregAtmelDll);
            registeredAtmelDll.setFileNameExtension("dll");
            unregAtmelDll.moveTo(registeredAtmelDll);

            std::string argumentList("/s \"");
            argumentList += registeredAtmelDll.getFullPath();
            argumentList += '"';

            Debug::trace("Main", "registering AT91Boot_DLL.dll ...");

            PAL_HINSTANCE res = ShellExecuteA (NULL,"open","regsvr32.exe",argumentList.c_str(),"",SW_SHOW);

        }
#endif

    }

    // TODO HFC project
    if (Configuration::getInstance()->isVersionManagerEnabled())
    {
        Debug::trace("Main", "Starting version manager ...");
        VersionManager::getInstance();
    }

    if (Configuration::getInstance()->isFakeWeigherManagerEnabled())
    {
        Debug::trace("Main", "Starting FAKE Weigher Manager (WSSession) ...");
        new WSSession();
        PAL_absclass::PAL_sleep(100);
    }
    Debug::trace( "Main","End .. ");
}

