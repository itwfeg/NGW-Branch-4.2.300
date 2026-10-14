/* This file is used for testing the application.
 * It is a test Weigher Manager used for testing the application.
 * Larry
 */

#include "Weigher.h"
#include "WeigherManager.h"
#include "WeigherStatus.h"
#include "WeigherInquire.h"
#include "WeigherWakeup.h"
#include "WeigherConfiguration.h"
#include "WeigherMode.h"
#include "WeigherGeneric.h"
#include "WeigherRequestStatus.h"
#include "WeigherRequestInquire.h"
#include "WeigherSystemStatus.h"
#include "WeigherSystemConfiguration.h"
#include "WeigherSystemInquire.h"
#include "WeigherSystemRequestStatus.h"
#include "WeigherSystemRequestInquire.h"
#include "SystemStatusInfo.h"
#include "SystemCalibrationSwitch.h"

#include "WSSession.h"
#include "Integer.h"
#include "Long.h"
/*#include "Double.h" */
#include "Hex.h"
#include "Configuration.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WSSession::_dataType_ = 0;
std::string WSSession::_dataTypeName_("WSSession");
#endif


using namespace std;


UINT32 WSSession::counts = 10000;
UINT32 WSSession::rawcounts = 10000;


WSSession::WSSession()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif
    //  this->pMyWSSession = pMyWSSession;

    // Register for all the OperationMode messages
    /*  registerEvent(OperationMode, OpModeIdle);
        registerEvent(OperationMode, OpModeOperate);    registerEvent(OperationMode, OpModePluEntry);
        registerEvent(OperationMode, OpModeAutoWeigh);
        registerEvent(OperationMode, OpModeSemiWeigh);
        registerEvent(OperationMode, OpModeManualWeigh);
        registerEvent(OperationMode, OpModeSupervisor);
        registerEvent(OperationMode, OpModeService);
        registerEvent(OperationMode, OpModePowerDown);
        */
    registerEvent(Weigher, WgReqInquire);
    registerEvent(Weigher, WgReqStatus);
    /*

       registerEvent(Weigher, WgEnable);
       registerEvent(Weigher, WgDisable);
       registerEvent(Weigher, WgRezero);
       registerEvent(Weigher, WgReset);
       registerEvent(Weigher, WgConfig);
       registerEvent(Weigher, WgMode);
       registerEvent(Weigher, WgNormal);
       registerEvent(Weigher, WgLock);
       registerEvent(Weigher, WgControl); // This is only a MFG Test message!!
       */

    registerEvent(Weigher, WgTestInit);
    registerEvent(Weigher, WgTestRezero);
    registerEvent(Weigher, WgTestReadConfig);

    registerEvent(Weigher, WgTestUp1);
    registerEvent(Weigher, WgTestUp10th);
    registerEvent(Weigher, WgTestUp100th);
    registerEvent(Weigher, WgTestUp1000th);

    registerEvent(Weigher, WgTestDown1);
    registerEvent(Weigher, WgTestDown10th);
    registerEvent(Weigher, WgTestDown100th);
    registerEvent(Weigher, WgTestDown1000th);

    registerEvent(Weigher, WgTestSwitch);

    //  registerEvent(WeigherSystem, SysConfig);
    //  registerEvent(WeigherSystem, SysReqStatus);
    //  registerEvent(WeigherSystem, SysReqInquire);

    //  registerEvent(ProductDev, ProductChange, msgQId);
    //  registerEvent(ProductDev, ProductUpdate, msgQId);

    counts = Configuration::getInstance()->toIntCfgI("fakeWeightStart");
    rawcounts = counts + 1000;

    dispatchUpdates = true;

    myScaleFactor = 1;

#ifdef LINUX_FIX
    //int weigherFrequency = 1000;
#else
    int weigherFrequency = 1000;
#endif
}


WSSession::~WSSession()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
    deregisterAllEvents();
}

void WSSession::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
    //Debug::trace("WSSession", HobString("configuration's weigher frequency:") + HobString(Integer::toString(weigherFrequency)));

    //testWSSession(i++);

    //weigherFrequency = Configuration::getInstance()->getFakeWeigherFrequency();
    //if (weigherFrequency == 0 || (i % weigherFrequency) == 0)
    //  sendWeight();

    if ( msg.get() != NULL )
    {
        Debug::trace("WSSession", HobString("WSSession::processEvent "));
        switch (msg->getDeviceType())
        {
            case Weigher:
                handleWSSessionEvent((WgEvent) msg->getEventType(), msg->Data());
                break;
            case WeigherSystem:
                handleWeigherSysEvent((SysEvent) msg->getEventType(), msg->Data());
                break;
                //                      case ProductDev:
                //                              handleProductEvent((ProductEvent) msg->getEventType(), msg->Data());
                //                              break;
            default:
                break;
        }
    }
}


void WSSession::sendWeight()
{
    //  UINT32 counts = rand();
    //  UINT32 rawcounts = rand();
    counts += Configuration::getInstance()->toIntCfgI("fakeWeightIncrement");
    rawcounts += Configuration::getInstance()->toIntCfgI("fakeWeightIncrement");
    if (counts > 3999900)
    {
        counts = Configuration::getInstance()->toIntCfgI("fakeWeightStart");
        rawcounts = counts + 1000;
    }

    if (Debug::isTracing("WSSession.status"))
    {
        HobString hc = Long::toString((long)counts);
        HobString hrc = Long::toString((long)rawcounts);
        Debug::trace("WSSession.status", HobString("creating weigh status: rawcounts=") + hrc +
                HobString("  counts=") + hc);
    }

    WeigherStatus ws(this, 0, rawcounts, counts);
    if (counts <= 25)
        ws.setWithinZeroQuarterGrad();
    else
        ws.clearWithinZeroQuarterGrad();
    ws.clearSmallMotion();
    ws.clearLargeMotion();
    ws.setScaleFactor(myScaleFactor);

    const char *buf = ws.serialize();
    UINT len = ws.getSerializedLength();
    Debug::trace("WSSession.hex", HobString("sending weigh status: ") + HobString(Hex::toString(buf, len)));

    dispatchEvent(&ws);
}

void WSSession::handleWSSessionEvent(WgEvent wgEvent, const char *UNUSED(recvBuffer))
{
    switch (wgEvent)
    {
        case WgTestUp1:
        case WgTestUp10th:
        case WgTestUp100th:
        case WgTestUp1000th:
        case WgTestDown1:
        case WgTestDown10th:
        case WgTestDown100th:
        case WgTestDown1000th:
            {
                if (wgEvent == WgTestUp1)
                {
                    Debug::trace("WSSession", HobString("received WgTestUp1"));
                    counts += 10000;
                    rawcounts += 10000;
                }
                else if (wgEvent == WgTestUp10th)
                {
                    Debug::trace("WSSession", HobString("received WgTestUp10th"));
                    counts += 1000;
                    rawcounts += 1000;
                }
                else if (wgEvent == WgTestUp100th)
                {
                    Debug::trace("WSSession", HobString("received WgTestUp100th"));
                    counts += 100;
                    rawcounts += 100;
                }
                else if (wgEvent == WgTestUp1000th)
                {
                    Debug::trace("WSSession", HobString("received WgTestUp1000th"));
                    counts += 10;
                    rawcounts += 10;
                }
                else if (wgEvent == WgTestDown1)
                {
                    Debug::trace("WSSession", HobString("received WgTestDown1"));
                    counts -= 10000;
                    rawcounts -= 10000;
                }
                else if (wgEvent == WgTestDown10th)
                {
                    Debug::trace("WSSession", HobString("received WgTestDown10th"));
                    counts -= 1000;
                    rawcounts -= 1000;
                }
                else if (wgEvent == WgTestDown100th)
                {
                    Debug::trace("WSSession", HobString("received WgTestDown100th"));
                    counts -= 100;
                    rawcounts -= 100;
                }
                else if (wgEvent == WgTestDown1000th)
                {
                    Debug::trace("WSSession", HobString("received WgTestDown1000th"));
                    counts -= 10;
                    rawcounts -= 10;
                }

                WeigherStatus ws(this, 0, rawcounts, counts);
                if (counts <= 25)
                    ws.setWithinZeroQuarterGrad();
                else
                    ws.clearWithinZeroQuarterGrad();
                ws.clearSmallMotion();
                ws.clearLargeMotion();
                ws.setScaleFactor(myScaleFactor);
                dispatchEvent(&ws);

                Debug::trace("WSSession", "sending weigher status: ");
                break;
            }
        case WgTestInit:
            break;
        case WgTestSwitch:
            {
                Debug::trace("WSSession", HobString("received WgTestSwitch"));
                SystemCalibrationSwitch scs(WeigherSystem, this, true);
                dispatchEvent(&scs);
                break;
            }
        case WgTestRezero:
        case WgRezero:
            {
                Debug::trace("WSSession", HobString("received WgRezero or WgTestRezero"));
                counts = 0;
                rawcounts = 0;

                WeigherStatus ws(this, 0, rawcounts, counts);
                ws.setNewZero();
                ws.setWithinZeroQuarterGrad();
                ws.clearSmallMotion();
                ws.clearLargeMotion();
                ws.setScaleFactor(myScaleFactor);
                dispatchEvent(&ws);
                break;
            }
        case WgEnable:
        case WgNormal:
            {
                Debug::trace("WSSession", HobString("received WgEnable or WgNormal"));
                dispatchUpdates = true;
                break;
            }
        case WgDisable:
        case WgLock:
            {
                Debug::trace("WSSession", HobString("received WgDisable or WgLock"));
                dispatchUpdates = false;
                break;
            }
        case WgReqStatus:
            {
                Debug::trace("WSSession", HobString("received WgReqStatus"));
                // WeigherRequestStatus wrs(recvBuffer);
                // wrs.setSourceObject(this);
                // const char *pCC = wrs.serialize();

				sendWeight();
                Debug::trace("WSSession", "sending weigher status: ");
                break;
            }
        case WgTestReadConfig:
        case WgReqInquire:
            {
                Debug::trace("WSSession", HobString(" -- WgReqInquire ( ") + HobString(Integer::toString(wgEvent)) + HobString(" )"));
                WeigherInquire wi(this);

				// FAKEWEIGH To have a fake weigher with 3 digit precision
				wi.setWeigherModel( (WeigherModel) Configuration::getInstance()->toIntCfgI("fakeWeigherModel") );
				myScaleFactor = Configuration::getInstance()->toIntCfgI("fakeWeigherScaleFactor");
		        wi.setScaleFactor(myScaleFactor);

				wi.setFilterSpeed(NormalFilterSpeed);
				wi.setMinWeightToPrint( Configuration::getInstance()->toIntCfgI("fakeWeigherMinWeightToPrint") );

                wi.setMaxWeight( Configuration::getInstance()->toIntCfgI("fakeWeigherMaxWeight") );

                Configuration::getInstance()->setScaleWeighMode( (WeighMode) Configuration::getInstance()->toIntCfgI("fakeWeigherMode") );
				wi.setScaleWeighMode( (WeighMode) Configuration::getInstance()->toIntCfgI("fakeWeigherMode") );

                wi.setLastCalibrationDate  (1159856788);
                wi.setNumberOfCalibrations(10);
                wi.setLastConfigurationDate(1159845677);
                wi.setNumberOfConfigurations(20);

                Debug::trace("WSSession", HobString("sending weigher inquire: "));
                dispatchEvent(&wi);
                break;
            }

        case WgControl:
            Debug::trace("WSSession", HobString("received WgControl"));
            break;
        case WgInquire:
            {
                Debug::trace("WSSession", HobString("received WgInquire"));
                //WeigherInquire wi(recvBuffer);
                //wi.setSourceObject(this);
                //dispatchEvent(&wi);
                break;
            }
        case WgStatus:
            {
                Debug::trace("WSSession", HobString("received WgStatus"));
                //WeigherStatus ws(recvBuffer);
                //ws.setSourceObject(this);
                //dispatchEvent(&ws);
                break;
            }
        case WgWakeup:
            {
                Debug::trace("WSSession", HobString("received WgWakeup"));
                //                                                      WeigherWakeup wwu(recvBuffer);
                //                                                      wwu.setSourceObject(this);
                //                                                      const char *pCC = wwu.serialize();
                //                                                      pMyWSSession->send(pCC, wwu.getSerializedLength());
                break;
            }
        case WgConfig:
            {
                Debug::trace("WSSession", HobString("received WgConfig"));
                //                                                      WeigherConfiguration wc(recvBuffer);
                //                                                      wc.setSourceObject(this);
                //                                                      const char *pCC = wc.serialize();
                //                                                      pMyWSSession->send(pCC, wc.getSerializedLength());
                break;
            }
        case WgReset:
            {
                Debug::trace("WSSession", HobString("received WgReset"));
                //WeigherGeneric wg(recvBuffer);
                //wg.setSourceObject(this);
                //const char *pCC = wg.serialize();
                //pMyWSSession->send(pCC, wg.getSerializedLength());
                break;
            }
        case WgMode:
            {
                Debug::trace("WSSession", HobString("received WgMode"));
                //WeigherMode wm(recvBuffer);
                //wm.setSourceObject(this);
                //const char *pCC = wm.serialize();
                //pMyWSSession->send(pCC, wm.getSerializedLength());
                break;
            }
        default:
            {
                Debug::trace("WSSession", HobString("WSSession -- Some Other WgEvent: ") + HobString(Integer::toString((int)wgEvent)));
                break;
            }
    }
}

void WSSession::handleWeigherSysEvent(SysEvent sysEvent, const char *recvBuffer)
{
    switch (sysEvent)
    {
        case SysWakeup:
            {
                cout << "WSSession -- SysWakeup!" << endl;
            }
            break;

        case SysConfig:
            {
                cout << "WSSession -- SysConfig!" << endl;
                WeigherSystemConfiguration wsc(recvBuffer);
                wsc.setSourceObject(this);

                //                      const char *pCC = wsc.serialize();
                Debug::trace("WSSession", HobString("received weigher system config"));
            }
            break;

        case SysInquire:
            {
                cout << "WSSession -- SysInquire!" << endl;
                WeigherSystemInquire wsi(recvBuffer);
                wsi.setSourceObject(this);
                dispatchEvent(&wsi, this);
            }
            break;

        case SysReset:
            {
                cout << "WSSession -- SysReset!" << endl;
            }
            break;
        case SysStatus:
            {
            }
            break;
        case SysEnable:
            {
                cout << "WSSession -- SysEnable!" << endl;
            }
            break;
        case SysDisable:
            {
                cout << "WSSession -- SysDisable!" << endl;
            }
            break;
        case SysMode:
            {
                cout << "WSSession -- SysMode!" << endl;
            }
            break;
        case SysRead:
            {
                cout << "WSSession -- SysRead!" << endl;
            }
            break;
        case SysWrite:
            {
                cout << "WSSession -- SysWrite!" << endl;
            }
            break;
        case SysEEPBlockRead:
            {
                cout << "WSSession -- SysEEPBlockRead!" << endl;
            }
            break;
        case SysEEPBlockWrite:
            {
                cout << "WSSession -- SysEEPBlockWrite!" << endl;
            }
            break;

        case SysReqStatus:
            {
                const UINT8 partNumber[] = { 1,2,3,4,5,6,7,8,9,0 };
                SystemStatusInfo *ssi = new SystemStatusInfo(0x0AB50, 0, 0x0A5EE, (BitError | EepromError), partNumber, 0x0A5EE, 0);
                WeigherSystemStatus wss(this, ssi);

                const char *buf = wss.serialize();
                UINT len = wss.getSerializedLength();
                Debug::trace("WSSession", HobString("sending weigher system status: ") + HobString(Hex::toString(buf, len)));

                dispatchEvent(&wss, this);
                delete ssi;
            }
            break;

        case SysReqInquire:
            {
                SystemInquire si(WeigherSystem, NULL, SysDefault, NULL, 0x3333, 0x5555);

                const char *buf = si.serialize();
                UINT len = si.getSerializedLength();
                Debug::trace("WSSession", HobString("sending weigher system inquire: ") + HobString(Hex::toString(buf, len)));

                dispatchEvent(&si, this);
            }
            break;
#ifdef LINUX_FIX
        default:
            break;
#endif
    }
}


void WSSession::testWSSession(int UNUSED(i))
{
    /*  const char *pCC = NULL;

        if ((i % 983) == 0)
        {
        WeigherRequestStatus wrs(this);

        pCC = wrs.serialize();
        pMyWSSession->send(pCC, wrs.getSerializedLength());
        }

        if ((i % 1781) == 0)
        {
        WeigherRequestInquire wi(this);

        pCC = wi.serialize();
        pMyWSSession->send(pCC, wi.getSerializedLength());
        }

        if ((i % 2757) == 0)
        {
        WeigherConfiguration wc(this);

    // Set config to This Scale!!
    // These are the only values that changed!!
    wc.setConfigType(WgTemporary);
    wc.setCenterOfMaintenanceZone(0x0000726D);
    wc.setMaxWeight(0x00049764);
    wc.setGainFactor(0x000022F6);
    wc.setZeroReference(0x0000A09E);
    wc.setCalibrationReference(0x0000995F);
    wc.setSmallMotionLimit(0x003F);

    pCC = wc.serialize();
    pMyWSSession->send(pCC, wc.getSerializedLength());
    }

    if ((i % 10000) == 0)
    {
    WeigherMode wm(this, ScalePrepackMode);

    pCC = wm.serialize();
    pMyWSSession->send(pCC,  wm.getSerializedLength());
    }
    else if ((i % 7500) == 0)
    {
    WeigherMode wm(this, ScaleWeightTrackingMode);

    pCC = wm.serialize();
    pMyWSSession->send(pCC,  wm.getSerializedLength());
    }
    else if ((i % 5000) == 0)
    {
    WeigherMode wm(this, ScaleDumbMode);

    pCC = wm.serialize();
    pMyWSSession->send(pCC,  wm.getSerializedLength());
    }
    else if ((i % 2500) == 0)
    {
    WeigherMode wm(this, ScaleManualMode);

    pCC = wm.serialize();
    pMyWSSession->send(pCC,  wm.getSerializedLength());
    }
    */
}

/*
   void WSSession::handleProductEvent(ProductEvent prEvent, char *recvBuffer, int len)
   {
   switch (prEvent)
   {
   case ProductChange:
   {
   Debug::trace("WSSession.Product", HobString("Received ProductChange event: ") + HobString(Hex::toString(recvBuffer, len)));
   ProductChangeMsg pr(recvBuffer);

   Debug::trace("WSSession.Product", HobString("ProductChange event: ") +
   HobString(Long::toString(pr.getProduct()->getProductNum())) + HobString(" ") +
   HobString(pr.getProduct()->getDescription()) + HobString(" ") +
   HobString(Double::toString(pr.getProduct()->getUnitPrice())));

   }
   break;
   case ProductUpdate:
   {
   Debug::trace("WSSession.Product", HobString("Received ProductChange event: ") + HobString(Hex::toString(recvBuffer, len)));
//                                                      cout << "WSSession -- ProductEvent:" << prEvent << endl;
ProductUpdateMsg pr(recvBuffer);
Debug::trace("WSSession.Product", HobString("ProductChange event: ") +
HobString(Long::toString(pr.getProduct()->getProductNum())) + HobString(" ") +
HobString(pr.getProduct()->getDescription()) + HobString(" ") +
HobString(Double::toString(pr.getProduct()->getUnitPrice()));

}
break;
default:
Debug::trace("WSSession.Product", HobString("WSSession - Unhandled Product Event:") +
HobString(Integer::toString(prEvent)));
break;
}
}
*/
