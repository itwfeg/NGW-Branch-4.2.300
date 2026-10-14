//------------------------------------------------------------------------------
/// @file : chobart_d.cpp
/// Launches Hobart daemon.
//------------------------------------------------------------------------------

#include "PAL_platformheader.h"

#include <sys/wait.h>

#if 1
#undef DUMP_USB_MON
#else
#define DUMP_USB_MON
#endif

extern "C"
{
#include <libudev.h>
}

#include "UsbManager.h"
#include "UsbManagerLinux.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"
#include "Hex.h"
#include "device_event.h"

#include "Main.h"
#include "RealTimeController.h"
#include "SessionManager.h"


#define HOBART_SRV_FILE     "hobart_srv.lock"   ///< Lock filename
#define HOBART_RUNNING_DIR  "/"                 ///< Root directory
#define FILE_MASK           00022

#define HOBART_DEV_PATH         "/dev/Hobart/"

using namespace std;

int monitorUSB()
{
    struct udev *udev;
    struct udev_device *dev;
    struct udev_monitor *mon;
    fd_set fds;
    int ret;
    int fd;

    Debug::trace("chobart_d", "monitorUSB()");

    /* Create the udev object */
    udev = udev_new();
    if (!udev) {
	    Debug::trace("chobart_d", HobString("Can't create udev"));
        exit(1);
    }

    /* Set up a monitor to monitor devices */
    mon = udev_monitor_new_from_netlink(udev, "udev");
    /* add the usb subsystem to the filter */
    udev_monitor_filter_add_match_subsystem_devtype(mon, "usb", NULL);
    /* add the tty subsystem to the filter */
    // udev_monitor_filter_add_match_subsystem_devtype(mon, "tty", NULL);
    udev_monitor_enable_receiving(mon);
    /* Get the file descriptor (fd) for the monitor.
     *     This fd will get passed to select() */
    fd = udev_monitor_get_fd(mon);

    UsbManagerLinux *um = static_cast<UsbManagerLinux *>(UsbManager::getInstance());
    um->discovery();
#if 0 //SOF-4076
    // see what's connected up that we can deal with presently
    um->EnumerateDevices();
#endif
    // Done with everything plugged in (or if it has been plugged in
    // during the above it'll be on the event list) so process events
    // as they come
    while (1)
    {
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        ret = select(fd+1, &fds, NULL, NULL, NULL);

        /* Check if our file descriptor has received data. */
        if (ret > 0 && FD_ISSET(fd, &fds))
        {
            //printf("\nselect() says there should be data\n");

            /* Make the call to receive the device.
             * select() ensured that this will not block. */
            dev = udev_monitor_receive_device(mon);
            if (dev)
            {
                if(udev_device_get_devnode(dev))
                {
#define DUMP_USB_MON
#ifdef DUMP_USB_MON
                    int vid = 0, pid = 0;
                    const char *cvid = udev_device_get_sysattr_value(dev, "idVendor");
                    const char *cpid = udev_device_get_sysattr_value(dev, "idProduct");
                    if (cvid && cpid)
                    {
                        vid = strtol(cvid, NULL, 16);
                        pid = strtol(cpid, NULL, 16);
                    }
#endif
                    EVENT_INFO ev_info;
                    ev_info.event_type = HOB_DEVICE_DETACHED;
                    ev_info.dev_path = udev_device_get_devnode(dev);
                    if (strncasecmp("add", udev_device_get_action(dev), 3) == 0)
                    {

                        ev_info.event_type = HOB_DEVICE_ATTACHED;


#ifdef DUMP_USB_MON
						    Debug::trace("chobart_d", HobString("Got Device"));
						    Debug::trace("chobart_d", HobString("   Node: ") + HobString(udev_device_get_devnode(dev)));
						    Debug::trace("chobart_d", HobString("   Subsystem: ") + HobString(udev_device_get_subsystem(dev)));
						    Debug::trace("chobart_d", HobString("   Devtype: ") + HobString(udev_device_get_devtype(dev)));
						    Debug::trace("chobart_d", HobString("   Action: ") + HobString(udev_device_get_action(dev)));
						    Debug::trace("chobart_d", HobString("   Driver: ") + HobString(udev_device_get_driver(dev)));
						    Debug::trace("chobart_d", HobString("   VID/PID: ") + HobString(Hex::toString(vid)) + HobString(Hex::toString(pid)));
#endif
                        Debug::trace("UsbManagerLinux.Event", "chobart_d() HandleDeviceNotification");
					    Debug::trace("chobart_d", HobString("HOB_DEVICE_ATTACHED:  Calling HandleDevice for ") + HobString(ev_info.dev_path.c_str()));
                        UsbManager::getInstance()->HandleDeviceNotification( &ev_info, vid, pid );
                    }
                    else
                    {
                        // removing a device
					    Debug::trace("chobart_d", HobString("HOB_DEVICE_DETACHED:  Calling HandleDevice for ") + HobString(ev_info.dev_path.c_str()));
                        UsbManager::getInstance()->HandleDeviceNotification( &ev_info, vid, pid );
                    }
                    udev_device_unref(dev);
                }
            }
            else
            {
			    Debug::trace("chobart_d", HobString("No Device from receive_device(). An error occured."));
            }
        }
    }
    udev_unref(udev);
    return 0;
}

///////////////////////////////////////////////////////
/// Handles signals received by the server.
///
/// @param sig - signal to handle
///
///
void SignalHandler(int32_t sig)
{
    switch(sig)
    {
        case(SIGCHLD):
            //
            //handle Hobart child proc
            //termination
            //
            break;
        case(SIGHUP):
            //
            //we are hung refresh the server
            //
            break;
        case(SIGTERM):
            //
            //gracefully terminate
            //
				Debug::trace("chobart_d", "Termination signal received shutdown");
				SessionManager::getInstance()->panicShutdown();
				PAL_absclass::PAL_sleep( 1000 );
				RealTimeController::getInstance()->detachUSBAPC();
				PAL_absclass::PAL_sleep( 1 );
				exit(EXIT_SUCCESS);
            break;
    }
}

///////////////////////////////////////////////////////
/// Starts the linux daemon.
///
/// @param file A pointer to a FILE pointer.
///
///
void initiateDaemon(FILE **file)
{
    pid_t pid;
    pid_t sid;
    PAL_ERRNO_TYPE l_err;

    if (NULL == *file)
    {
        l_err = PAL_INVALID_VAL;
        PAL_PRINT_LOG(PAL_ERR_LOG, "Invalid file handle.");
        exit(EXIT_FAILURE);
    }

    FILE *srv_file = *file;


    //
    //attemp to fork off of the parent.
    //terminate if an error occurs
    //terminate if we are parent (pid > 0).
    //Continue as daemon if we are a child proc (pid==0)
    //
    pid = fork();

    Debug::trace("chobart_d", HobString("pid = ") + HobString(Integer::toString(pid)));
    if (0 > pid)
    {
        l_err = PAL_CREATE_PROC_FAILED;
        PAL_PRINT_LOG(PAL_ERR_LOG, "Unable to fork daemon.");
        exit(EXIT_FAILURE);
    }
    if (0 < pid)
    {
        PAL_PRINT_LOG(PAL_INFO_LOG, "Parent process terminating.");
        exit(EXIT_SUCCESS);
    }

    //we dont need any logs as we'll continue to use PALs log!!!

    //
    //-create unique session ID
    //-change the file mode mask
    //-change working dir to root
    //-just in case attempt to close any open descriptor
    //
    sid = setsid();
    if (0 > sid)
    {
        l_err = PAL_SESSION_ID_FAILED;
        PAL_PRINT_LOG(PAL_ERR_LOG, "Unable to set session ID.");
        exit(EXIT_FAILURE);
    }

    //  umask(0);
    //or use this to set standard
    umask(FILE_MASK);


    if (0 > (chdir(HOBART_RUNNING_DIR)))
    {
        PAL_PRINT_LOG(PAL_ERR_LOG, "Unable to change working dir to root.");
        exit(EXIT_FAILURE);
    }

    int32_t i;
    FILE* std_io;

    for(i=getdtablesize();i>0;--i)
        close(i);

    //
    //fopen opens stdin and dup provides copy for stdout/err
    //
    std_io = fopen("/dev/null","w");
    i = (int32_t)dup(fileno(srv_file));  //Added casts to elimiante complier warning
    i = (int32_t)dup(fileno(srv_file));

    /************
      close(STDIN_FILENO);
      close(STDOUT_FILENO);
      close(STDERR_FILENO);
     ************/

    //
    //open server file and lock it so another instance
    //of the server can not start.
    //
    //srv_file = fopen(HOBART_SRV_FILE, O_RDWR | O_CREATE, 0640);
    srv_file = fopen(HOBART_SRV_FILE, "w");
    if (0 > srv_file)
    {
        PAL_PRINT_LOG(PAL_ERR_LOG, "Another instance of server running.");
        exit(EXIT_SUCCESS);
    }
    if(0 > lockf(fileno(srv_file), F_TLOCK, 0))
    {
        PAL_PRINT_LOG(PAL_ERR_LOG, "Unable to lock server file.");
        exit(EXIT_FAILURE);
    }

    const int32_t pid_str_size=10;
    char str[pid_str_size];

    sprintf(str, "%d\n", getpid());
    fprintf(srv_file, "%s", str);

    //
    //signal handling logic
    //
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    //ignore all tty sigs

    signal(SIGCHLD, SignalHandler);
    //Handle child terminate signal
    signal(SIGHUP, SignalHandler);
    //hangup signal
    signal(SIGTERM, SignalHandler);
    //termination sig from kill
}

///////////////////////////////////////////////////////
/// Starts the daemon and instantiates appropriates classes
/// to monitor and handle USB activity.
///
/// @return 0 on success.
///
int main(void)
{
    //FILE *srv_file;

    PAL_absclass::PAL_logging = PAL_NO_LOG;
    //PAL_absclass::PAL_logging = 0xFFFFFFFF;
    //PAL_absclass::PAL_logging = 0xFFFFFFFF-PAL_WHERE_IS_LOG;

    PAL_PRINT_LOG(PAL_INFO_LOG, "chobart daemon startup.");

    //register or signal handlers
    signal(SIGCHLD, SignalHandler);
    //Handle child terminate signal
    signal(SIGHUP, SignalHandler);
    //hangup signal
    signal(SIGTERM, SignalHandler);
    //termination sig from kill

    pid_t usb_pid=0;
    while(1)
    {
        {
            EvolMain main;

            main.startAll( 1, NULL, NULL, NULL, NULL );
            //
            // Enumerate all USB devices and launch required service classes.
            // Note: this must be done after registering for device notifications,
            // because the enumeration process may launch device firmware updaters
            // which cause device resets after the firmware is downloaded, which in turn
            // causes removal/arrival notifications for the device in question.
            //
#if 0
            Debug::addTraceCategory("ScaleService");
            Debug::setTraceAllCategoriesOf("UsbManager");
            Debug::setTraceAllCategoriesOf("UsbManagerLinux");
            Debug::setTraceAllCategoriesOf("CUsbPipe");
            Debug::setTraceAllCategoriesOf("CUsbCommBase");
            Debug::setTraceAllCategoriesOf("EZUSB");
            Debug::setTraceAllCategoriesOf("EvolWeigherCodeUpdater");
            Debug::setTraceAllCategoriesOf("IUsbCodeUpdater");
            Debug::setTraceAllCategoriesOf("EvolPrinterCodeUpdater");
            Debug::setTraceAllCategoriesOf("CUPrinterCommManager");
            Debug::setTraceAllCategoriesOf("UsbWrapperCodeUpdater");
            Debug::setTraceAllCategoriesOf("Main");
            Debug::setTraceAllCategoriesOf("Weigher");
            Debug::setTraceAllCategoriesOf("WeigherCommManager");
            Debug::setTraceAllCategoriesOf("WeigherManager");
            Debug::setTraceAllCategoriesOf("Printer");
            Debug::setTraceAllCategoriesOf("PrinterManager");
            Debug::setTraceAllCategoriesOf("PrinterCommManager");
            Debug::setTraceAllCategoriesOf("EvolWrapper");
            Debug::setTraceAllCategoriesOf("RealTimeController");
            Debug::setTraceAllCategoriesOf("CTLSThread");
            Debug::setTraceAllCategoriesOf("VersionManager");
            Debug::setTraceAllCategoriesOf("AtmelSambaUpdater");
            Debug::setTraceAllCategoriesOf("AtmelDeviceHandler");

            //Debug::setTraceAllCategoriesOf("database");

            Debug::setTraceAllCategoriesOf("SoftwareSystem");
            Debug::setTraceAllCategoriesOf("VersionManager");
            //Debug::setTraceAllCategoriesOf("SoftwareSystem.upgrade");
            Debug::setTraceAllCategoriesOf("firmware.upgrade");

            Debug::setTraceAllCategoriesOf("APCCommManager");
            //Debug::setTraceAllCategoriesOf("APCCommManager.PrinterSys");
            //Debug::setTraceAllCategoriesOf("APCCommManager.Printer");
            //Debug::setTraceAllCategoriesOf("APCCommManager.WeigherSys");
            //Debug::setTraceAllCategoriesOf("APCCommManager.Weigher");
            Debug::setTraceAllCategoriesOf("Weigher");
            Debug::setTraceAllCategoriesOf("Session");
            Debug::setTraceAllCategoriesOf("SessionWrapper");
            Debug::setTraceAllCategoriesOf("AbstractSession");
#endif

            Debug::trace(("ScaleService"), ("ScaleService.exe"));

            //-Start monitoring USB devices
            monitorUSB();

            //if we ever reach here then it's
            //time to terminate.
            //
            PAL_PRINT_LOG(PAL_ERR_LOG, "chobart_d: Child process terminating.");
            exit(EXIT_FAILURE);
        }

        //
        //we must wait here until we find ourselves
        //hung or child terms.
        //
        //added the waitpid routine
        //as it should be...this logic may be enough for us
        //we may not need special handling in the sig handler.
        //not sure though and must be tested...
        //
        int32_t child_status;
        while (1)
        {
            int32_t err = waitpid(usb_pid, &child_status, WUNTRACED);
            if (PAL_FAILURE == err)
            {
                switch(errno)
                {
                    case(EINTR):
                        //we must loop back & wait
                        //again as we got interrupted
                        break;
                    case(ECHILD):
                        //we terminate wait and log error
                        //& go to the top of outter loop
                        //restart server.
                        break;
                    case(EINVAL):
                        //we terminate wait and log error
                        //& go to the top of outter loop
                        //restart server.
                        break;
                }
            }
        }
        //sleep(30);
    }//while forever

    PAL_PRINT_LOG(PAL_ERR_LOG, "chobart_d:  Terminating. Goodbye.");
    exit(EXIT_SUCCESS);
}
