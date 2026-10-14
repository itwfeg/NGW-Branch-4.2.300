#ifndef EVOLWEIGHERCODEUPDATER_H
#define EVOLWEIGHERCODEUPDATER_H

//#include "HobThread.h"
#include "IUsbCodeUpdater.h"

//#include "usbPlatform.h"
//#include "usbdLib.h"

#include "VersionManagerEvent.h"


#if defined ( __linux__ )
#include "CypressEzUsbUpdaterLinux.h"
#else
class CypressEzUsbUpdater;
#endif

class HobVersion;

class EvolWeigherCodeUpdater : public IUsbCodeUpdater
{
    public:    
        EvolWeigherCodeUpdater(USBD_NODE_ID usbNodeId, UINT16 usbDeviceType, HobVersion hardwareVersion);
        virtual ~EvolWeigherCodeUpdater();

    private:
        void handleVersionManagerDevEvent(VersionManagerEvent verManagerEvent, const char *recvBuffer);
        void DoPeripheralUpdate(const char *recvBuffer);

#if defined ( __linux__ )
        CypressEzUsbUpdaterLinux* pCommMgr_;
#else
        CypressEzUsbUpdater* pCommMgr_;
#endif
        bool forceQuit;
        util_filesystem::Directory *srcDir;

#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif
};
#endif //EVOLWEIGHERCODEUPDATER_H
