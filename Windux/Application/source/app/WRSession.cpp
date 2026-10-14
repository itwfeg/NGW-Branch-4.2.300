/* This file is used for testing the application.
 * It is a test Wrapper Manager used for testing the application.
 * Larry
 */
#include "vxWorks.h"
#include "Wrapper.h"
#include "WrapperManager.h"
/*
#include "WrapperStatus.h"
#include "WrapperInquire.h"
#include "WrapperWakeup.h"
#include "WrapperMode.h"
#include "WrapperGeneric.h"
#include "WrapperRequestStatus.h"
#include "WrapperRequestInquire.h"
#include "WrapperSystemConfiguration.h"
#include "WrapperSystemInquire.h"
*/
#include "WrapperSystemStatus.h"
#include "WrapperSystemRequestStatus.h"
#include "WrapperSystemRequestInquire.h"
#include "SystemStatusInfo.h"
#include "WrapperSensorsTracking.h"
#include "WrapperStatusTracking.h"
#include "WrapperControl.h"
#include "WrapperIntakeMode.h"
#include "WrapperConfiguration.h"


#include "WRSession.h"
#include "MessageQueue.h"
#include "Integer.h"
#include "Long.h"
#include "Double.h"
#include "Hex.h"
#include "Configuration.h"

WRSession::WRSession() : HobThread((FUNCPTR) activeFunction, "tFWrMan", 100, 0, 16384, 10, 2048, MSG_Q_FIFO)
{
//	this->pMyWRSession = pMyWRSession;

	registerEvent(Wrapper, WrSensorsTracking, msgQId);
	registerEvent(Wrapper, WrStatusTracking, msgQId);
	registerEvent(Wrapper, WrReqInquire, msgQId);
	registerEvent(Wrapper, WrConfig, msgQId);
	registerEvent(Wrapper, WrReqStatus, msgQId);
/*

	registerEvent(Wrapper, WrEnable, msgQId);
	registerEvent(Wrapper, WrDisable, msgQId);
	registerEvent(Wrapper, WrRezero, msgQId);
	registerEvent(Wrapper, WrReset, msgQId);
	registerEvent(Wrapper, WrMode, msgQId);
	registerEvent(Wrapper, WrNormal, msgQId);
	registerEvent(Wrapper, WrLock, msgQId);
	registerEvent(WrapperSystem, SysConfig, msgQId);
*/
	registerEvent(Wrapper, WrLoad, msgQId);
	registerEvent(Wrapper, WrIntakeMode, msgQId);
	registerEvent(Wrapper, WrControl, msgQId); // This is only a MFG Test message!!
	registerEvent(WrapperSystem, SysReqStatus, msgQId);
/*
	registerEvent(WrapperSystem, SysReqInquire, msgQId);

	registerEvent(ProductDev, ProductChange, msgQId);
	registerEvent(ProductDev, ProductUpdate, msgQId);
*/
	registerEvent(Wrapper, WrCycle, msgQId);

	dispatchUpdates = true;
	monitorSensors = false;
	wrapperSenseData = NULL;
	sensorLoopCount = 0;
	sendWrapperStatusData = false;
	wrapperStatusData = NULL;
	activeFunctionStart = true;
	wrapperConfiguration = NULL;
}


WRSession::~WRSession()
{
	deregisterAllEvents(msgQId);
}

int WRSession::activeFunction(void *pObject)
{
	return ((WRSession *) pObject)->activeFunctionTask();
}

int WRSession::activeFunctionTask()
{
	char recvBuffer[16 * 1024]; // CSM-120
	int msgSize = 0;
	UINT16	tempValue;

	while (!activeFunctionStart)
	{
		Debug::trace("WRSession", "WRSession Manager waiting to continue with active function ... ");
		taskDelay(10);
	}

	int i = 0;

	while (1)
	{
//		cout << "WRSession::numMsgs = " << pThreadQ->numMsgs() << endl;

		testWRSession(i++);

		if (pThreadQ->numMsgs() > 0)
		{
			msgSize = pThreadQ->receive(recvBuffer, sizeof(recvBuffer), WAIT_FOREVER);

			if (msgSize > 0)
			{
//				cout << "Message Arrived WRSession -- Size: " << msgSize << endl;
				EvolMessage evolMessage(recvBuffer);

				switch (evolMessage.getDeviceType())
				{
					case Wrapper		:	handleWRSessionEvent((WrEvent) evolMessage.getEventType(), recvBuffer);
											break;
					case WrapperSystem	:	handleWrapperSysEvent((SysEvent) evolMessage.getEventType(), recvBuffer);
											break;

					default				:
											break;

				}
			}
			else
			{
				cout << "WRSession Manager error on receive method - errno: " << errno << endl;
			}
		}
		if (monitorSensors && wrapperSenseData)
		{
			if (sensorLoopCount++ > 100)
			{
				sensorLoopCount = 0;

				tempValue = wrapperSenseData->getHeaterReading() + 5;
				if (tempValue > 2000)
					tempValue = 10;
				wrapperSenseData->setHeaterReading(tempValue);
				tempValue = wrapperSenseData->getEncoderCount() + 5;
				if (tempValue > 3000)
					tempValue = 10;
				wrapperSenseData->setEncoderCount(tempValue);
				tempValue = wrapperSenseData->getIntakeCount() + 5;
				if (tempValue > 2500)
					tempValue = 10;
				wrapperSenseData->setIntakeCount(tempValue);

				(wrapperSenseData->isStopSwitch()) ? (wrapperSenseData->clearStopSwitch()) : (wrapperSenseData->setStopSwitch());
				(wrapperSenseData->isPackageLengthSwitch()) ? (wrapperSenseData->clearPackageLengthSwitch()) : (wrapperSenseData->setPackageLengthSwitch());
				(wrapperSenseData->isPackageSenseSwitch()) ? (wrapperSenseData->clearPackageSenseSwitch()) : (wrapperSenseData->setPackageSenseSwitch());
				(wrapperSenseData->isPackageWidthSwitch()) ? (wrapperSenseData->clearPackageWidthSwitch()) : (wrapperSenseData->setPackageWidthSwitch());
				(wrapperSenseData->isPusherHomeSwitch()) ? (wrapperSenseData->clearPusherHomeSwitch()) : (wrapperSenseData->setPusherHomeSwitch());
				(wrapperSenseData->isPackageJamSwitch()) ? (wrapperSenseData->clearPackageJamSwitch()) : (wrapperSenseData->setPackageJamSwitch());
				(wrapperSenseData->isWideFilmSwitch()) ? (wrapperSenseData->clearWideFilmSwitch()) : (wrapperSenseData->setWideFilmSwitch());
				(wrapperSenseData->isNarrowFilmSwitch()) ? (wrapperSenseData->clearNarrowFilmSwitch()) : (wrapperSenseData->setNarrowFilmSwitch());
				(wrapperSenseData->isSlideBlockState()) ? (wrapperSenseData->clearSlideBlockState()) : (wrapperSenseData->setSlideBlockState());
				(wrapperSenseData->isHeaterState()) ? (wrapperSenseData->clearHeaterState()) : (wrapperSenseData->setHeaterState());
				(wrapperSenseData->isCompressorState()) ? (wrapperSenseData->clearCompressorState()) : (wrapperSenseData->setCompressorState());
				(wrapperSenseData->isMainMotorState()) ? (wrapperSenseData->clearMainMotorState()) : (wrapperSenseData->setMainMotorState());
				(wrapperSenseData->isDischargeMotorState()) ? (wrapperSenseData->clearDischargeMotorState()) : (wrapperSenseData->setDischargeMotorState());

				if (wrapperSenseData->isFilmLengthStateShortPackage())
					wrapperSenseData->setFilmLengthStateMediumPackage();
				else if (wrapperSenseData->isFilmLengthStateMediumPackage())
					wrapperSenseData->setFilmLengthStateLongPackage();
				else if (wrapperSenseData->isFilmLengthStateLongPackage())
					wrapperSenseData->setFilmLengthStateFilmLengthIdle();
				else if (wrapperSenseData->isFilmLengthStateFilmLengthIdle())
					wrapperSenseData->setFilmLengthStateAutoLengthSelect();
				else
					wrapperSenseData->setFilmLengthStateShortPackage();

				if (wrapperSenseData->isFilmWidthStateFilmWidthIdle())
					wrapperSenseData->setFilmWidthStateWidePackage();
				else if (wrapperSenseData->isFilmWidthStateWidePackage())
					wrapperSenseData->setFilmWidthStateNarrowPackage();
				else if (wrapperSenseData->isFilmWidthStateNarrowPackage())
					wrapperSenseData->setFilmWidthStateAutoWidthSelect();
				else
					wrapperSenseData->setFilmWidthStateFilmWidthIdle();

				(wrapperSenseData->isKnifeState()) ? (wrapperSenseData->clearKnifeState()) : (wrapperSenseData->setKnifeState());
				(wrapperSenseData->isLeftSideClampState()) ? (wrapperSenseData->clearLeftSideClampState()) : (wrapperSenseData->setLeftSideClampState());
				(wrapperSenseData->isRightSideClampState()) ? (wrapperSenseData->clearRightSideClampState()) : (wrapperSenseData->setRightSideClampState());
				(wrapperSenseData->isGripperState()) ? (wrapperSenseData->clearGripperState()) : (wrapperSenseData->setGripperState());

#if 0
				WrapperSensors ws(this);
				ws.setSourceObject(this);
				dispatchEvent(&ws, this);
#else
				const char *buf = wrapperSenseData->serialize();
				UINT len = wrapperSenseData->getSerializedLength();
				Debug::trace("WRSession", HobString("sending wrapper sense: ") + HobString(Hex::toString(buf, len)));

//				wrapperSenseData->setSourceObject(this);
				dispatchEvent(wrapperSenseData, this);
#endif
			}
		}

		if (sendWrapperStatusData && wrapperStatusData)
		{
			if (sensorLoopCount++ > 1000)
			{
				sensorLoopCount = 0;
				const char *buf = wrapperStatusData->serialize();
				UINT len = wrapperStatusData->getSerializedLength();
				Debug::trace("WRSession", HobString("sending wrapper status: ") + HobString(Hex::toString(buf, len)));

				dispatchEvent(wrapperStatusData, this);
			}
		}
		taskDelay(1);
	}

	return 0;
}

void WRSession::handleWRSessionEvent(WrEvent wrEvent, char *recvBuffer)
{
	cout << "handleWRSessionEvent -- event = " << wrEvent << endl;

	switch (wrEvent)
	{
		case WrStatusTracking:		
		{
			cout << "WRSession -- WrStatusTracking!" << endl;
			WrapperStatusTracking wst(recvBuffer);
			if (wst.getStatusTracking())
				sendWrapperStatusData = true;
			else
				sendWrapperStatusData = false;

			if (sendWrapperStatusData == true)
			{
				if (!wrapperStatusData)
				{
					wrapperStatusData = new WrapperStatus(this);
					if (wrapperStatusData)
					{
						WrapperHeaterInfo &heaterInfo = wrapperStatusData->getHeaterInfo();
						heaterInfo.setState();		//	Power is ON
						heaterInfo.setCelsius();
						heaterInfo.setSetting(230);
						heaterInfo.setTemperature(198);
						wrapperStatusData->setHeaterInfo(heaterInfo);
		
						WrapperStatusInfo &wrapperStatusInfo = wrapperStatusData->getStatusInfo();
						wrapperStatusInfo.setAutoWidthSelect();
						wrapperStatusInfo.setMediumTension();
						wrapperStatusData->setStatusInfo(wrapperStatusInfo);
					}
				}

				if (wrapperStatusData)
				{
					const char *buf = wrapperStatusData->serialize();
					UINT len = wrapperStatusData->getSerializedLength();
					Debug::trace("WRSession", HobString("sending wrapper status: ") + HobString(Hex::toString(buf, len)));

					dispatchEvent(wrapperStatusData, this);
					sensorLoopCount = 0;
				}
			}
		}
		break;

		case WrSensorsTracking:		
		{
			cout << "WRSession -- WrSensorsTracking!" << endl;
			WrapperSensorsTracking wst(recvBuffer);
			if (wst.getSensorsTracking())
				monitorSensors = true;
			else
				monitorSensors = false;

			if (monitorSensors == true)
			{
			cout << "WRSession -- SensorsTracking is ON" << endl;
				if (!wrapperSenseData)
					wrapperSenseData = new WrapperSensors(this);

				if (wrapperSenseData)
				{
					wrapperSenseData->setEncoderCount(25);
					wrapperSenseData->setIntakeCount(50);
					wrapperSenseData->setStopSwitch();
					wrapperSenseData->setPackageLengthSwitch();
					wrapperSenseData->setPackageWidthSwitch();
					wrapperSenseData->setPackageJamSwitch();
					wrapperSenseData->setWideFilmSwitch();
					wrapperSenseData->setHeaterState();
					wrapperSenseData->setCompressorState();
					wrapperSenseData->setDischargeMotorState();
					wrapperSenseData->setFilmLengthStateAutoLengthSelect();
					wrapperSenseData->setFilmWidthStateWidePackage();
					wrapperSenseData->setLeftSideClampState();
					wrapperSenseData->setGripperState();

					sensorLoopCount = 0;
				}
			}
			else
			cout << "WRSession -- SensorsTracking is OFF" << endl;
		}
		break;

		case WrControl:
		{
			cout << "WRSession -- WrControl!" << endl;
			WrapperControl wc(recvBuffer);
			if (wrapperStatusData)
			{
				WrapperHeaterInfo &heaterInfo = wrapperStatusData->getHeaterInfo();
				heaterInfo.setSetting(wc.getHeaterSetting());
				if (wc.isHeaterEnable())
					heaterInfo.setState();
				else
					heaterInfo.clearState();
				wrapperStatusData->setHeaterInfo(heaterInfo);

				WrapperStatusInfo &wrapperStatusInfo = wrapperStatusData->getStatusInfo();
				wrapperStatusInfo.setPackWidth(wc.getFilmWidth());
				wrapperStatusInfo.setFilmTension(wc.getFilmTension());
				wrapperStatusData->setStatusInfo(wrapperStatusInfo);
			}
		}
		break;

		case WrIntakeMode:
		{
			cout << "WRSession -- WrIntakeMode!" << endl;
			WrapperIntakeMode wim(recvBuffer);
		}
		break;

		case WrLoad:
		{
			cout << "WRSession -- WrLoad!" << endl;
		}
		break;

		case WrReqInquire:
		{
			cout << "WRSession -- WrReqInquire!" << endl;
			if (!wrapperConfiguration)
				initializeWrapperConfig();
			else
				Debug::trace("WRSession", HobString("WrapperInquire already exists."));
			

			const char *buf = wrapperConfiguration->serialize();
			UINT len = wrapperConfiguration->getSerializedLength();
			Debug::trace("WRSession", HobString("sending wrapper config: ") + HobString(Hex::toString(buf, len)));

			dispatchEvent(wrapperConfiguration, this);
		}
		break;

		case WrConfig:		
		{
			cout << "WRSession -- WrConfig!" << endl;
			WrapperConfiguration wc(recvBuffer);
			saveWrapperConfig(&wc);
#if 0
			if (wrapperConfiguration)
			{
				cout << "WRSession -- WrConfig - saving to wrapperConfiguration." << endl;
				wc.isWideFilmTestEnable() ? wrapperConfiguration->setWideFilmTestEnable() : wrapperConfiguration->clearWideFilmTestEnable();
				wc.isNarrowFilmTestEnable() ? wrapperConfiguration->setNarrowFilmTestEnable() : wrapperConfiguration->clearNarrowFilmTestEnable();
				wrapperConfiguration->setIdleTimeout(wc.getIdleTimeout());
				wrapperConfiguration->setFilmTension(wc.getFilmTension());
				//	For Wrapper Error Totals
				wrapperConfiguration->setElevatorCollapseTotal(wc.getElevatorCollapseTotal());
				wrapperConfiguration->setIntakeJamTotal(wc.getIntakeJamTotal());
				wrapperConfiguration->setLengthJamTotal(wc.getLengthJamTotal());
				wrapperConfiguration->setWidthJamTotal(wc.getWidthJamTotal());
				wrapperConfiguration->setCoverOpenTotal(wc.getCoverOpenTotal());
				wrapperConfiguration->setSealerBeltOpenTotal(wc.getSealerBeltOpenTotal());
				wrapperConfiguration->setPackageTooTallTotal(wc.getPackageTooTallTotal());
				wrapperConfiguration->setPusherHomeErrorTotal(wc.getPusherHomeErrorTotal());
				wrapperConfiguration->setFilmPullErrorTotal(wc.getFilmPullErrorTotal());
				wrapperConfiguration->setErrorTotalsDate(wc.getErrorTotalsDate());
			}
#endif
		}
		break;

		case WrReqStatus:	
		{
			if (!wrapperStatusData)
				wrapperStatusData = new WrapperStatus(this);

			if (wrapperStatusData)
			{
				wrapperStatusData->getStatusInfo().setRate(123);
				const char *buf = wrapperStatusData->serialize();
				UINT len = wrapperStatusData->getSerializedLength();
				Debug::trace("WRSession", HobString("sending wrapper status: ") + HobString(Hex::toString(buf, len)));

				dispatchEvent(wrapperStatusData, this);
			}
		}
		break;

		case WrCycle:	
		{
			if (!wrapperStatusData)
			{
				wrapperStatusData = new WrapperStatus(this);
#if 1
					WrapperPackageInfo packageInfo = wrapperStatusData->getPackageInfo(IntakePosition);

					packageInfo.setPresent();
					packageInfo.setLength(68);
					packageInfo.setWidth(60);

					wrapperStatusData->setPackageInfo(packageInfo, IntakePosition);
#else
				for (int index = 0; index < NumWrapperPositions; index++)
				{
					WrapperPackageInfo packageInfo = wrapperStatusData->getPackageInfo(index);

					packageInfo.setPresent();
					packageInfo.setLength(packageInfo.getLength() + ((index * 2) + 10));
					packageInfo.setWidth(packageInfo.getWidth() + ((index * 3) + 5));

					wrapperStatusData->setPackageInfo(packageInfo, index);
				}
#endif
			}

			if (wrapperStatusData)
			{
#if 1
				WrapperPackageInfo packageInfo = wrapperStatusData->getPackageInfo(IntakePosition);

				packageInfo.setPresent();
				packageInfo.setLength(packageInfo.getLength() + 2);
				packageInfo.setWidth(packageInfo.getWidth() + 3);

				wrapperStatusData->setPackageInfo(packageInfo, IntakePosition);
#endif
				const char *buf = wrapperStatusData->serialize();
				UINT len = wrapperStatusData->getSerializedLength();
				Debug::trace("WRSession", HobString("sending wrapper status for a WrCycle: ") + HobString(Hex::toString(buf, len)));

				dispatchEvent(wrapperStatusData, this);
			}
		}
		break;

		default:
		{
			cout << "WRSession -- Some Other WrEvent: !" << (UINT16) wrEvent << endl;
		}
		break;
	}
}

void WRSession::initializeWrapperConfig()
{
	const UINT8 name1[] = { '1','S',',','2','S',',','5','S',',','1','0','S',',','1','7','S',0 };
	const UINT8 name2[] = { '7','S',0 };
	const UINT8 name3[] = { '2','0','S',',','4','S',0 };
	const UINT8 name4[] = { '1','6','S',0 };
	const UINT8 name5[] = { '8','S',0 };
	const UINT8 name6[] = { '9','S',',','1','2','S',',','1','0','x','1','4',0 };
	const UINT8 name7[] = { '2','5',0 };
	const UINT8 name8[] = { 'U','s','e','r',' ','d','e','f','i','n','e','d',' ','8',0 };
	const UINT8 name9[] = { 'U','s','e','r',' ','d','e','f','i','n','e','d',' ','9',0 };
	const UINT8 name10[] = { 'U','s','e','r',' ','d','e','f','i','n','e','d',' ','1','0',0 };
	const UINT8 name11[] = { 'U','s','e','r',' ','d','e','f','i','n','e','d',' ','1','1',0 };
	const UINT8 name12[] = { 'U','s','e','r',' ','d','e','f','i','n','e','d',' ','1','2',0 };

	Debug::trace("WRSession", HobString("Creating a new WrapperInquire."));
	wrapperConfiguration = new WrapperInquire();
	wrapperConfiguration->setEventType(WrInquire);
	wrapperConfiguration->setWideFilmTestEnable();
	wrapperConfiguration->clearNarrowFilmTestEnable();
	wrapperConfiguration->setIdleTimeout(15);
	wrapperConfiguration->setFilmTension(MediumFilmTension);
	//	For Wrapper Error Totals
	wrapperConfiguration->setElevatorCollapseTotal(232);
	wrapperConfiguration->setIntakeJamTotal(13);
	wrapperConfiguration->setLengthJamTotal(43);
	wrapperConfiguration->setWidthJamTotal(4);
	wrapperConfiguration->setCoverOpenTotal(7);
	wrapperConfiguration->setSealerBeltOpenTotal(1);
	wrapperConfiguration->setPackageTooTallTotal(88);
	wrapperConfiguration->setPusherHomeErrorTotal(6);
	wrapperConfiguration->setFilmPullErrorTotal(73);
	wrapperConfiguration->setErrorTotalsDate(31403);

	//	For Wrapper Error Totals
	WrapperPackCodeInfo wrPackCode;
	int packages = 5, jams = 10;
	int offset;
	for (int packCodeNum = PackCode0; packCodeNum < NumberPackCodes; packCodeNum++)
	{
		wrPackCode = wrapperConfiguration->getWrapperPackCodeInfo((WrPackCode) packCodeNum);

		//	Pack code name/description
		switch (packCodeNum)
		{
			case 0:
				wrPackCode.setDescription((UINT8*) name1);
				break;
			case 1:
				wrPackCode.setDescription((UINT8*) name2);
				break;
			case 2:
				wrPackCode.setDescription((UINT8*) name3);
				break;
			case 3:
				wrPackCode.setDescription((UINT8*) name4);
				break;
			case 4:
				wrPackCode.setDescription((UINT8*) name5);
				break;
			case 5:
				wrPackCode.setDescription((UINT8*) name6);
				break;
			case 6:
				wrPackCode.setDescription((UINT8*) name7);
				break;
			case 7:
				wrPackCode.setDescription((UINT8*) name8);
				break;
			case 8:
				wrPackCode.setDescription((UINT8*) name9);
				break;
			case 9:
				wrPackCode.setDescription((UINT8*) name10);
				break;
			case 10:
				wrPackCode.setDescription((UINT8*) name11);
				break;
			case 11:
				wrPackCode.setDescription((UINT8*) name12);
				break;
			default:
				break;
		}
		//	Package Totals
		wrPackCode.setPackageTotal(packages);
		packages += 5;
		//	Jam Totals
		wrPackCode.setJamTotal(jams);
		jams += 5;

		if (packCodeNum < PackCode6)
		{
			offset = 5 * (packCodeNum + 1);
			wrPackCode.setPackSizeMinWidth(offset);
			wrPackCode.setPackSizeMaxWidth(offset + 10);
			wrPackCode.setPackSizeMinLength(offset + 15);
			wrPackCode.setPackSizeMaxLength(offset + 25);
		}

		wrPackCode.setSideClampOnMin(100);
		wrPackCode.setSideClampOnValue(117);
		wrPackCode.setSideClampOnMax(125);
		wrPackCode.setSideClampOffMin(180);
		wrPackCode.setSideClampOffValue(192);
		wrPackCode.setSideClampOffMax(200);
		wrPackCode.setGripperOnMin(125);
		wrPackCode.setGripperOnValue(133);
		wrPackCode.setGripperOnMax(140);
		wrPackCode.setGripperOffMin(220);
		wrPackCode.setGripperOffValue(229);
		wrPackCode.setGripperOffMax(240);
		wrPackCode.setKnifeOffMin(175);
		wrPackCode.setKnifeOffValue(184);
		wrPackCode.setKnifeOffMax(200);

		wrPackCode.setBooster();
//					wrPackCode.setPackWidth((WrPackWidth)(packCodeNum - (packCodeNum/2)));
//					wrPackCode.setPackLength((WrPackLength)(packCodeNum - (packCodeNum/3)));
		
		wrapperConfiguration->setWrapperPackCodeInfo(wrPackCode);
	}
	wrapperConfiguration->setWrapperCycleTotal(1234);
	wrapperConfiguration->setPackCodeTotalsDate(110802);
}

void WRSession::saveWrapperConfig(WrapperConfiguration *newConfig)
{
	if (!wrapperConfiguration || !newConfig)
		return;

	Debug::trace("WRSession", HobString("Saving the new WrapperConfiguration."));

	newConfig->isWideFilmTestEnable() ? wrapperConfiguration->setWideFilmTestEnable() : wrapperConfiguration->clearWideFilmTestEnable();
	newConfig->isNarrowFilmTestEnable() ? wrapperConfiguration->setNarrowFilmTestEnable() : wrapperConfiguration->clearNarrowFilmTestEnable();

	wrapperConfiguration->setIdleTimeout(newConfig->getIdleTimeout());
	wrapperConfiguration->setFilmTension(newConfig->getFilmTension());
	//	For Wrapper Error Totals
	wrapperConfiguration->setElevatorCollapseTotal(newConfig->getElevatorCollapseTotal());
	wrapperConfiguration->setIntakeJamTotal(newConfig->getIntakeJamTotal());
	wrapperConfiguration->setLengthJamTotal(newConfig->getLengthJamTotal());
	wrapperConfiguration->setWidthJamTotal(newConfig->getWidthJamTotal());
	wrapperConfiguration->setCoverOpenTotal(newConfig->getCoverOpenTotal());
	wrapperConfiguration->setSealerBeltOpenTotal(newConfig->getSealerBeltOpenTotal());
	wrapperConfiguration->setPackageTooTallTotal(newConfig->getPackageTooTallTotal());
	wrapperConfiguration->setPusherHomeErrorTotal(newConfig->getPusherHomeErrorTotal());
	wrapperConfiguration->setFilmPullErrorTotal(newConfig->getFilmPullErrorTotal());
	wrapperConfiguration->setErrorTotalsDate(newConfig->getErrorTotalsDate());

	//	For Wrapper Error Totals
	WrapperPackCodeInfo newPackCode;
	WrapperPackCodeInfo currentPackCode;
	for (int packCodeNum = PackCode0; packCodeNum < NumberPackCodes; packCodeNum++)
	{
		currentPackCode = wrapperConfiguration->getWrapperPackCodeInfo((WrPackCode) packCodeNum);
		newPackCode = newConfig->getWrapperPackCodeInfo((WrPackCode) packCodeNum);

		//	Pack code name/description
		UINT8 description[PackCodeDescriptionLength];
		newPackCode.getDescription(description);
		currentPackCode.setDescription(description);
		//	Package Totals
		currentPackCode.setPackageTotal(newPackCode.getPackageTotal());
		//	Jam Totals
		currentPackCode.setJamTotal(newPackCode.getJamTotal());

		currentPackCode.setPackWidth(newPackCode.getPackWidth());
		currentPackCode.setPackLength(newPackCode.getPackLength());

		currentPackCode.setPackSizeMinWidth(newPackCode.getPackSizeMinWidth());
		currentPackCode.setPackSizeMaxWidth(newPackCode.getPackSizeMaxWidth());
		currentPackCode.setPackSizeMinLength(newPackCode.getPackSizeMinLength());
		currentPackCode.setPackSizeMaxLength(newPackCode.getPackSizeMaxLength());

		currentPackCode.setSideClampOnMin(newPackCode.getSideClampOnMin());
		currentPackCode.setSideClampOnValue(newPackCode.getSideClampOnValue());
		currentPackCode.setSideClampOnMax(newPackCode.getSideClampOnMax());
		currentPackCode.setSideClampOffMin(newPackCode.getSideClampOffMin());
		currentPackCode.setSideClampOffValue(newPackCode.getSideClampOffValue());
		currentPackCode.setSideClampOffMax(newPackCode.getSideClampOffMax());
		currentPackCode.setGripperOnMin(newPackCode.getGripperOnMin());
		currentPackCode.setGripperOnValue(newPackCode.getGripperOnValue());
		currentPackCode.setGripperOnMax(newPackCode.getGripperOnMax());
		currentPackCode.setGripperOffMin(newPackCode.getGripperOffMin());
		currentPackCode.setGripperOffValue(newPackCode.getGripperOffValue());
		currentPackCode.setGripperOffMax(newPackCode.getGripperOffMax());
		currentPackCode.setKnifeOffMin(newPackCode.getKnifeOffMin());
		currentPackCode.setKnifeOffValue(newPackCode.getKnifeOffValue());
		currentPackCode.setKnifeOffMax(newPackCode.getKnifeOffMax());

		newPackCode.isSlideBlock() ? currentPackCode.setSlideBlock() : currentPackCode.clearSlideBlock();
		newPackCode.isBooster() ? currentPackCode.setBooster() : currentPackCode.clearBooster();
		
		wrapperConfiguration->setWrapperPackCodeInfo(currentPackCode);
	}
}

void WRSession::handleWrapperSysEvent(SysEvent sysEvent, char *recvBuffer)
{
	switch (sysEvent)
	{
/*
		case SysWakeup:
						{
							cout << "WRSession -- SysWakeup!" << endl;
						}
						break;

		case SysConfig:
		{
			cout << "WRSession -- SysConfig!" << endl;
			WrapperSystemConfiguration wsc(recvBuffer);
			wsc.setSourceObject(this);

			const char *pCC = wsc.serialize();
			Debug::trace("WRSession", HobString("received wrapper system config"));
		}
		break;

		case SysInquire:
		{
			cout << "WRSession -- SysInquire!" << endl;
			WrapperSystemInquire wsi(recvBuffer);
			wsi.setSourceObject(this);
			dispatchEvent(&wsi, this);
		}
		break;

		case SysReset:
						{
							cout << "WRSession -- SysReset!" << endl;
						}
						break;
		case SysEnable:
						{
							cout << "WRSession -- SysEnable!" << endl;
						}
						break;
		case SysDisable:
						{
							cout << "WRSession -- SysDisable!" << endl;
						}
						break;
		case SysMode:
						{
							cout << "WRSession -- SysMode!" << endl;
						}
						break;
		case SysRead:
						{
							cout << "WRSession -- SysRead!" << endl;
						}
						break;
		case SysWrite:
						{
							cout << "WRSession -- SysWrite!" << endl;
						}
						break;
		case SysEEPBlockRead:
						{
							cout << "WRSession -- SysEEPBlockRead!" << endl;
						}
						break;
		case SysEEPBlockWrite:
						{
							cout << "WRSession -- SysEEPBlockWrite!" << endl;
						}
						break;
*/
		case SysReqStatus:
		{
			const UINT8 partNumber[] = { 1,2,3,4,5,6,7,8,9,0 };
			SystemStatusInfo *ssi = new SystemStatusInfo(0x0CCD8, 0, 0x01108, (BitError | EepromError), partNumber, 0x01108, 0);
			WrapperSystemStatus wss(this, ssi);

			const char *buf = wss.serialize();
			UINT len = wss.getSerializedLength();
			Debug::trace("WRSession", HobString("sending wrapper system status: ") + HobString(Hex::toString(buf, len)));

			dispatchEvent(&wss, this);
			delete ssi;
		}
		break;

		case SysStatus:
		{
		}
		break;
/*
		case SysReqInquire:
		{
			SystemInquire si(WrapperSystem, NULL, SysDefault, NULL, 0x3333, 0x5555);

			const char *buf = si.serialize();
			UINT len = si.getSerializedLength();
			Debug::trace("WRSession", HobString("sending wrapper system inquire: ") + HobString(Hex::toString(buf, len)));

			dispatchEvent(&si, this);
		}
		break;
*/
		default:
		break;
	}
}


void WRSession::testWRSession(int i)
{
/*	const char *pCC = NULL;

	if ((i % 983) == 0)
	{
		WrapperRequestStatus wrs(this);

		pCC = wrs.serialize();
		pMyWRSession->send(pCC, wrs.getSerializedLength());
	}

	if ((i % 1781) == 0)
	{
		WrapperRequestInquire wi(this);

		pCC = wi.serialize();
		pMyWRSession->send(pCC, wi.getSerializedLength());
	}

	if ((i % 2757) == 0)
	{
		WrapperConfiguration wc(this);

		// Set config to This Scale!!
		// These are the only values that changed!!
		wc.setConfigType(WrTemporary);
		wc.setCenterOfMaintenanceZone(0x0000726D);
		wc.setMaxWeight(0x00049764);
		wc.setGainFactor(0x000022F6);
		wc.setZeroReference(0x0000A09E);
		wc.setCalibrationReference(0x0000995F);
		wc.setSmallMotionLimit(0x003F);

		pCC = wc.serialize();
		pMyWRSession->send(pCC, wc.getSerializedLength());
	}

	if ((i % 10000) == 0)		
	{
		WrapperMode wm(this, ScalePrepackMode);

		pCC = wm.serialize();
		pMyWRSession->send(pCC,  wm.getSerializedLength());
	}
	else if ((i % 7500) == 0)
	{
		WrapperMode wm(this, ScaleWeightTrackingMode);

		pCC = wm.serialize();
		pMyWRSession->send(pCC,  wm.getSerializedLength());
	}
	else if ((i % 5000) == 0)
	{
		WrapperMode wm(this, ScaleDumbMode);

		pCC = wm.serialize();
		pMyWRSession->send(pCC,  wm.getSerializedLength());
	}
	else if ((i % 2500) == 0)
	{
		WrapperMode wm(this, ScaleManualMode);

		pCC = wm.serialize();
		pMyWRSession->send(pCC,  wm.getSerializedLength());
	}
*/
}
