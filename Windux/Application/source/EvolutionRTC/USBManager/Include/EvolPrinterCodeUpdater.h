#ifndef EVOLPRINTERCODEUPDATER_H
#define EVOLPRINTERCODEUPDATER_H

#include "IUsbCodeUpdater.h"

#include "VersionManagerEvent.h"

class CUPrinterCommManager;
class HobVersion;

class EvolPrinterCodeUpdater : public IUsbCodeUpdater
{
    public:

        typedef enum _ResponseStateMachine
        {
            waitingForResponses,
            sendingOutCommand,
            checkForCommandComplete,
            failedDontRun
        } ResponseStateMachine;

        EvolPrinterCodeUpdater(USBD_NODE_ID usbNodeId, UINT16 usbDeviceType, HobVersion hardwareVersion);
        virtual ~EvolPrinterCodeUpdater();

        bool isUpdateComplete();

    private:
        void handleVersionManagerDevEvent(VersionManagerEvent verManagerEvent, const char *recvBuffer);
        void DoPeripheralUpdate(const char *recvBuffer);

        CUPrinterCommManager* pCommMgr_;
        bool forceQuit;
        int responseCounter_;
        ResponseStateMachine responseStateMachine_;
		bool lockUpdater_;

#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif
};
#endif //EVOLPRINTERCODEUPDATER_H


