/* This file is used for testing the application.
 * It is a test Label Applier Manager used for testing the application.
 * Larry
 */
 

#include "LASession.h"
#include "MessageQueue.h"
#include "Integer.h"
#include "Long.h"
#include "Double.h"
#include "Hex.h"

#include "LabelApplier.h"
#include "LabelApplierManager.h"
#include "LabelApplierStatus.h"
#include "LabelApplierInquire.h"
#include "LabelApplierWakeup.h"
#include "LabelApplierConfiguration.h"
#include "LabelApplierMode.h"
#include "LabelApplierGeneric.h"
#include "LabelApplierRequestStatus.h"
#include "LabelApplierRequestInquire.h"
#include "LabelApplierSystemRequestStatus.h"
#include "ILASpeed.h"
#include "ILACycle.h"


LASession::LASession() : HobThread((FUNCPTR) activeFunction, "tLbAplMan", 100, 0, 4096, 10, 1024, MSG_Q_FIFO)
{
//	this->pMyLASession = pMyLASession;
	sendSpeed = 0;

	registerEvent(LabelApplier, LaReqStatus, msgQId);
	registerEvent(LabelApplier, LaReqInquire, msgQId);
	registerEvent(LabelApplierSystem, SysStatus, msgQId);

	registerEvent(LabelApplier, LaEnable, msgQId);
	registerEvent(LabelApplier, LaDisable, msgQId);
	registerEvent(LabelApplier, LaReset, msgQId);
	registerEvent(LabelApplier, LaConfig, msgQId);
	registerEvent(LabelApplier, LaMode, msgQId);
	registerEvent(LabelApplier, LaILAReqSpeed, msgQId);

	dispatchUpdates = true;

	activeFunctionStart = true;
}


LASession::~LASession()
{
	deregisterAllEvents(msgQId);
}

int LASession::activeFunction(void *pObject)
{
	return ((LASession *) pObject)->activeFunctionTask();
}

int LASession::activeFunctionTask()
{
	char recvBuffer[16 * 1024]; // CSM-120
	int msgSize = 0;

	while (!activeFunctionStart)
	{
		taskDelay(10);
	}

	int i = 0;

	while (1)
	{
//		cout << "LASession::numMsgs = " << pThreadQ->numMsgs() << endl;



		if (sendSpeed > 0)
		{
			if (sendSpeed == 1)
			{
				taskDelay(100);
				int speed = rand() % 100;
				ILASpeed ls(this, speed);
				dispatchEvent(&ls, this);
				sendSpeed = 2;
			}
			else
			{
				taskDelay(200);
				ILACycle lc(this);
				dispatchEvent(&lc, this);
				sendSpeed = 1;
			}
		}


		testLASession(i++);

		if (pThreadQ->numMsgs() > 0)
		{
			msgSize = pThreadQ->receive(recvBuffer, sizeof(recvBuffer), WAIT_FOREVER);

			if (msgSize > 0)
			{
//				cout << "Message Arrived LASession -- Size: " << msgSize << endl;
				EvolMessage evolMessage(recvBuffer);

				switch (evolMessage.getDeviceType())
				{
					case LabelApplier:
						handleLASessionEvent((LaEvent) evolMessage.getEventType(), recvBuffer);
						break;
					case LabelApplierSystem:
						handleLASysSessionEvent((SysEvent) evolMessage.getEventType(), recvBuffer);
						break;

					default:
						break;

				}
			}
			else
			{
				cout << "LASession Manager errno: " << errno << endl;
			}
		}

		taskDelay(1);
	}

	return 0;
}

void LASession::handleLASessionEvent(LaEvent laEvent, char *recvBuffer)
{
	switch (laEvent)
	{
		case LaInquire:
						{
							//cout << "LASession -- LaInquire!" << endl;
							LabelApplierInquire li(recvBuffer);
							li.setSourceObject(this);
							dispatchEvent(&li, this);
						}
						break;
		case LaStatus:		
						{
							cout << "LASession -- LaStatus!" << endl;
							LabelApplierStatus ls(recvBuffer);
							ls.setSourceObject(this);
							dispatchEvent(&ls, this);
						}
						break;
		case LaWakeup:		
						{
							//cout << "LASession -- LaWakeup!" << endl;
							LabelApplierWakeup lwu(recvBuffer);
							lwu.setSourceObject(this);

//							const char *pCC = lwu.serialize();
//							pMyLASession->send(pCC, lwu.getSerializedLength());
						}
						break;
		case LaConfig:		
						{
							//cout << "LASession -- LaConfig!" << endl;
							LabelApplierConfiguration lc(recvBuffer);
							lc.setSourceObject(this);

//							const char *pCC = lc.serialize();
//							pMyLASession->send(pCC, lc.getSerializedLength());
							Debug::trace("LASession", HobString("received label applier config"));
						}
						break;
		case LaEnable:		//cout << "LASession -- LaEnable!" << endl;
		case LaDisable:		//cout << "LASession -- LaDisable!" << endl;
		case LaReset:		//cout << "LASession -- LaReset!" << endl;
						{
							LabelApplierGeneric la(recvBuffer);
							la.setSourceObject(this);

//							const char *pCC = la.serialize();
//							pMyLASession->send(pCC, la.getSerializedLength());
						}
						break;
		case LaMode:		
						{
							cout << "LASession -- LaMode!" << endl;
							LabelApplierMode lm(recvBuffer);
							lm.setSourceObject(this);

//							const char *pCC = lm.serialize();
//							pMyLASession->send(pCC, lm.getSerializedLength());
						}
						break;
		case LaReqStatus:	
						{
							//cout << "LASession -- LaReqStatus!" << endl;
							LabelApplierRequestStatus lrs(recvBuffer);
							lrs.setSourceObject(this);

//							const char *pCC = lrs.serialize();
//							pMyLASession->send(pCC, lrs.getSerializedLength());
						}
						break;
		case LaReqInquire:
						{
							//cout << "LASession -- LaReqInquire!" << endl;
							LabelApplierInquire li(this);

							li.setTransferStationType(HandWrapper);
							li.setWeigherStationAttached(true);
							li.setSecondaryStationAttached(false);
							li.setLeftToRight(true);

							li.setConfigType(LaTemporary);
							li.setPrimaryStationAttached(true);
							li.setFrontDischarge(false);
							li.setPrimaryJustifyFont(true);
							li.setSecondaryJustifyFont(false);
							li.show();
#if 0
setPrimaryPrinterLabelOffset(UINT8 primaryPrinterLabelOffset);
setSecondaryPrinterLabelOffset(UINT8 secondaryPrinterLabelOffset);
setWeigherConveyorSensitivity(UINT8 weigherConveyorSensitivity);
setPrimaryPrinterConveyorSensitivity(UINT8 primaryPrinterConveyorSensitivity);
setSecondaryPrinterConveyorSensitivity(UINT8 secondaryPrinterConveyorSensitivity);
setPrimaryPrinterMaxJustificationDistance(UINT8 primaryPrinterMaxJustificationDistance);
setSecondaryPrinterMaxJustificationDistance(UINT8 secondaryPrinterMaxJustificationDistance);
setDiagonalLabelPlacement(bool value);
setDiagonalOffset(UINT8 diagonalOffset);
setMinistationLabelPlacement(UINT8 ministationLabelPlacement);
#endif
							const char *buf = li.serialize();
							UINT len = li.getSerializedLength();
							Debug::trace("LASession", HobString("sending label applier inquire: ") + HobString(Hex::toString(buf, len)));

							dispatchEvent(&li, this);
						}
						break;
		case LaILAReqSpeed:	
						{
							//cout << "LASession -- LaILAReqSpeed!" << endl;
							sendSpeed = 1;
						}
						break;
		default			:
						{
							cout << "LASession -- Some Other LaEvent: !" << (UINT16) laEvent << endl;
						}
						break;
	}
}

void LASession::handleLASysSessionEvent(SysEvent sysEvent, char *recvBuffer)
{
	switch (sysEvent)
	{
		case SysStatus:
						{
							cout << "LASession -- SysStatus!" << endl;
							LabelApplierSystemRequestStatus lrss(recvBuffer);
							lrss.setSourceObject(this);

//							const char *pCC = lrss.serialize();
//							pMyLASession->send(pCC, lrss.getSerializedLength());
						}
						break;
		default			:
						{
							cout << "LASession -- Some Other SysLaEvent: !" << (UINT16) sysEvent << endl;
						}
						break;
	}
}

void LASession::testLASession(int i)
{
/*	const char *pCC = NULL;

	if ((i % 983) == 0)
	{
		LabelApplierRequestStatus lrs(this);

		pCC = lrs.serialize();
		pMyLabelApplier->send(pCC, lrs.getSerializedLength());
	}

	if ((i % 1781) == 0)
	{
		LabelApplierRequestInquire li(this);

		pCC = li.serialize();
		pMyLabelApplier->send(pCC, li.getSerializedLength());
	}

	if ((i % 2757) == 0)
	{
		LabelApplierConfiguration lc(this);

		// Set config to This Scale!!
		// These are the only values that changed!!
		lc.setConfigType(LaTemporary);
		lc.setCenterOfMaintenanceZone(0x0000726D);
		lc.setMaxWeight(0x00049764);
		lc.setGainFactor(0x000022F6);
		lc.setZeroReference(0x0000A09E);
		lc.setCalibrationReference(0x0000995F);
		lc.setSmallMotionLimit(0x003F);

		pCC = lc.serialize();
		pMyLabelApplier->send(pCC, lc.getSerializedLength());
	}

	if ((i % 10000) == 0)		
	{
		LabelApplierMode lm(this, ScalePrepackMode);

		pCC = lm.serialize();
		pMyLabelApplier->send(pCC,  lm.getSerializedLength());
	}
	else if ((i % 7500) == 0)
	{
		LabelApplierMode lm(this, ScaleWeightTrackingMode);

		pCC = lm.serialize();
		pMyLabelApplier->send(pCC,  lm.getSerializedLength());
	}
	else if ((i % 5000) == 0)
	{
		LabelApplierMode lm(this, ScaleDumbMode);

		pCC = lm.serialize();
		pMyLabelApplier->send(pCC,  lm.getSerializedLength());
	}
	else if ((i % 2500) == 0)
	{
		LabelApplierMode lm(this, ScaleManualMode);

		pCC = lm.serialize();
		pMyLabelApplier->send(pCC,  lm.getSerializedLength());
	}
*/
}
