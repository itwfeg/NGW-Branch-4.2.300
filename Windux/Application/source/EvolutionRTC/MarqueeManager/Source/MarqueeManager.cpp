
#include "Configuration.h"

#include "MarqueeManager.h"

#include "MarqueeConfiguration.h"
#include "MarqueeGeneric.h"
#include "MarqueeInquire.h"
#include "MarqueeMarqueeKind.h"
#include "MarqueeMode.h"
#include "MarqueeRequestWakeup.h"
#include "MarqueeStatus.h"
#include "MarqueeTest.h"
#include "MarqueeUpdate.h"
#include "MarqueeWakeup.h"
#include "MarqueeSystemStatus.h"
#include "SystemRequestStatus.h"
#include "MarqueeOpMode.h"


#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int MarqueeManager::dataType = 0;
std::string MarqueeManager::dataTypeName("MarqueeManager"); 
#endif

MarqueeManager::MarqueeManager(Hobart::CEvolMsgReceiver *pMyMarquee,
							   UINT16 physicalAddress,
							   MarqueeKind mqKind)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	Debug::trace("MarqueeManager", "MarqueeManager");

	this->pMyMarquee_ = pMyMarquee;
	this->physicalAddress_ = physicalAddress;
	this->mqKind_ = mqKind;
	probeTimeoutCntr_ = 0;

	// Register for all the OperationMode messages
	registerEvent(OperationMode, OpModeIdle );
	registerEvent(OperationMode, OpModeOperate );
	registerEvent(OperationMode, OpModePluEntry);
	registerEvent(OperationMode, OpModeAutoWeigh);
	registerEvent(OperationMode, OpModeSemiWeigh);
	registerEvent(OperationMode, OpModeManualWeigh);
	registerEvent(OperationMode, OpModeSupervisor);
	registerEvent(OperationMode, OpModeService);
	registerEvent(OperationMode, OpModePowerDown);

	registerEvent(Marquee, MqReqWakeup);
	registerEvent(Marquee, MqReqInquire);

	registerEvent(Marquee, MqWakeup);
	registerEvent(Marquee, MqConfig);
	registerEvent(Marquee, MqInquire);
	registerEvent(Marquee, MqReset);
//	registerEvent(Marquee, MqStatus, msgQId); //not used
	registerEvent(Marquee, MqEnable);
	registerEvent(Marquee, MqDisable);
	registerEvent(Marquee, MqMode);
	registerEvent(Marquee, MqTest);
	registerEvent(Marquee, MqUpdate);
	registerEvent(Marquee, MqClear);

	registerEvent(MarqueeSystem, SysStatus);
	registerEvent(MarqueeSystem, SysReqStatus);

	registerEvent(Marquee, MqOperateMode);


	pConfig_ = Configuration::getInstance();

	marqueeProbeTime_ = pConfig_->getCustDispProbeTime() * 1000;
	Debug::trace("MarqueeManager.MarqueeProbe", HobString("marqueeProbeTime_ = ") + Integer::toString(marqueeProbeTime_));


	testAsciiChar_ = 0;

	asciiTestTime_ = 3000;

	mqOpMode_ = MqOpModeNormal;

	createTimer(marqueeProbeTime_, marqueeProbe, CHobartTimer::PERIODIC_TIMER);
	sendMarqueeKind();
}

MarqueeManager::~MarqueeManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
	shutdownTimerThreads();
	ShutdownNicely();
	
	Debug::trace("MarqueeManager", "~MarqueeManager");

	lastStatusMsg_.setManagerDeleted();
	dispatchEvent(&lastStatusMsg_);	
}

void MarqueeManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{

	if (msg.get() != NULL)
	{
		switch (msg->getDeviceType())
		{
			case Marquee		:	handleMarqueeEvent((MqEvent) msg->getEventType(), msg->Data());
									break;
			case MarqueeSystem	:	handleMarqueeSystemEvent((MqEvent) msg->getEventType(), msg->Data());
									break;
			//case OperationMode	:	handleOperationModeEvent((OpModeEvent) msg->getEventType(), msg->Data());
			//						break;
			default				:
									break;
		}
	}


}

void MarqueeManager::startAsciiCharacterTest()
{
	const char *pCC = NULL;

	Debug::trace("MarqueeManager.AsciiTest", HobString("start marquee timer"));


	testAsciiChar_ = 0x20;

	MarqueeMode mm(OnlyProductMode, this);

	pCC = mm.serialize();
	pMyMarquee_->send(pCC, mm.getSerializedLength());

	timeoutProcess( marqueeAsciiTest );

	createTimer(asciiTestTime_, marqueeAsciiTest, CHobartTimer::PERIODIC_TIMER);

}


void MarqueeManager::stopAsciiCharacterTest()
{
const char *pCC = NULL;

	Debug::trace("MarqueeManager.AsciiTest", HobString("stop marquee timer"));

	testAsciiChar_ = 0x0;

	cancelTimer(marqueeAsciiTest);

	MarqueeMode mm(SuspendProductMode, this);

	pCC = mm.serialize();
	pMyMarquee_->send(pCC, mm.getSerializedLength());

}

void MarqueeManager::handleMarqueeSystemEvent(MqEvent mqEvent, const char *recvBuffer)
{

	switch (mqEvent)
	{
		/*received fake system status from usb device*/
		case SysStatus:
		{
			Debug::trace("MarqueeManager.Messages.In", HobString("systemStatus"));

			MarqueeSystemStatus mqss(recvBuffer);
			mqss.setSourceObject(this);

			dispatchEvent(&mqss);

			if (Debug::isTracing("MarqueeManager.Messages.In") == true)
			{
				mqss.show();
			}

		}
		break;
		/*messages sent out to usb device*/
		case SysReqStatus:		
		{
			Debug::trace("MarqueeManager.Messages.Out", HobString("SysReqStatus"));

			SystemRequestStatus srs(recvBuffer);

			const char *pRSS = srs.serialize();
			pMyMarquee_->send(pRSS, srs.getSerializedLength());

		}
		break;

		default:
		{
			Debug::trace("MarqueeManager.Error", HobString("Some Other Sytem Event: ") +
												 HobString(Integer::toString((UINT16) mqEvent)));
		}
		break;

	}

}


void MarqueeManager::handleMarqueeEvent(MqEvent mqEvent, const char *recvBuffer)
{
	switch (mqEvent)
	{
		/*received messages from usb device*/
		case MqInquire:
						{
							Debug::trace("MarqueeManager.Messages.In", HobString("MqInquire"));

							MarqueeInquire mqi(recvBuffer);
							mqi.setSourceObject(this);

							receivedMarqueeFeedback(lastStatusMsg_.updateStatus(mqi));

							if (Debug::isTracing("MarqueeManager.Messages.In") == true)
							{
								mqi.show();
							}

						}
						break;

		case MqWakeup:		
						{
							Debug::trace("MarqueeManager.Messages.In", HobString("MqWakeup"));

							MarqueeWakeup mqw(recvBuffer);
							mqw.setSourceObject(this);

							receivedMarqueeFeedback(lastStatusMsg_.updateStatus(mqw));

							if (Debug::isTracing("MarqueeManager.Messages.In") == true)
							{
								mqw.show();
							}

						}
						break;

		/*messages sent out to usb device*/
		case MqConfig:		
						{
							Debug::trace("MarqueeManager.Messages.Out", HobString("MqConfig"));

							MarqueeConfiguration mqc(recvBuffer);
							mqc.setSourceObject(this);

							const char *pCC = mqc.serialize();
							pMyMarquee_->send(pCC, mqc.getSerializedLength());

						}
						break;
		case MqEnable:		Debug::trace("MarqueeManager.Messages.Out", HobString("MqEnable"));
		case MqDisable:		Debug::trace("MarqueeManager.Messages.Out", HobString("MqDisable"));
		case MqReset:		Debug::trace("MarqueeManager.Messages.Out", HobString("MqReset"));
						{
							MarqueeGeneric mqg(recvBuffer);
							mqg.setSourceObject(this);

							const char *pCC = mqg.serialize();
							pMyMarquee_->send(pCC, mqg.getSerializedLength());
						}
						break;
		case MqClear:		Debug::trace("MarqueeManager.Messages.Out", HobString("MqClear"));
						{
							if (mqOpMode_ == MqOpModeNormal)
							{
								MarqueeGeneric mqg(recvBuffer);
								mqg.setSourceObject(this);

								const char *pCC = mqg.serialize();
								pMyMarquee_->send(pCC, mqg.getSerializedLength());
							}
						}
						break;

		case MqMode:		
						{
							Debug::trace("MarqueeManager.Messages.Out", HobString("MqMode"));

							MarqueeMode mqm(recvBuffer);
							mqm.setSourceObject(this);

							if (Debug::isTracing("MarqueeManager.Messages.Out") == true)
							{
								mqm.show();
							}

							const char *pCC = mqm.serialize();
							pMyMarquee_->send(pCC, mqm.getSerializedLength());
						}
						break;
		case MqTest:
						{
							Debug::trace("MarqueeManager.Messages.Out", HobString("MqTest"));

							MarqueeTest mqt(recvBuffer);

							if (mqt.getMqTestVal() == MqCharacter)
							{
								if (testAsciiChar_ == (char)0)
									startAsciiCharacterTest();
								
							}
							else
							{
								if (testAsciiChar_ > (char)0)
									stopAsciiCharacterTest();

								mqt.setSourceObject(this);

								if (Debug::isTracing("MarqueeManager.Messages.Out") == true)
								{
									mqt.show();
								}

								const char *pCC = mqt.serialize();
								pMyMarquee_->send(pCC, mqt.getSerializedLength());
							}
						}
						break;
		case MqUpdate:
						{
							Debug::trace("MarqueeManager.Messages.Out", HobString("MqUpdate"));

							if (mqOpMode_ == MqOpModeNormal)
							{

								MarqueeUpdate mqu(recvBuffer);
								mqu.setSourceObject(this);
								mqu.removeInvalidChars();

								if (Debug::isTracing("MarqueeManager.Messages.Out") == true)
								{
									mqu.show();
								}

								const char *pCC = mqu.serialize();
								pMyMarquee_->send(pCC, mqu.getSerializedLength());
							}
						}
						break;
		case MqReqInquire:
						{
							Debug::trace("MarqueeManager.Messages.Out", HobString("MqReqInquire"));

							MarqueeGeneric mqri(recvBuffer);
							mqri.setSourceObject(this);

							const char *pCC = mqri.serialize();
							pMyMarquee_->send(pCC, mqri.getSerializedLength());
						}
						break;
		case MqReqWakeup:
						{
							Debug::trace("MarqueeManager.Messages.Out", HobString("MqReqWakeup"));

							MarqueeRequestWakeup mqrw(recvBuffer);
							mqrw.setSourceObject(this);
							mqrw.setPhysicalAddress(physicalAddress_);

							const char *pCC = mqrw.serialize();
							pMyMarquee_->send(pCC, mqrw.getSerializedLength());
						}
						break;

		case MqOperateMode:
						{
							Debug::trace("MarqueeManager.Messages.Out", HobString("MqReqWakeup"));

							MarqueeOpMode mqOpMode(recvBuffer);

							mqOpMode_ = mqOpMode.getMqOpMode();

							switch (mqOpMode_)
							{
								case MqOpModeOutOfServ:
								{
									MarqueeUpdate mqUpdate(MqProduct, "Out of Service\\R");
									mqUpdate.setSourceObject(this);
									const char *pCC = mqUpdate.serialize();
									pMyMarquee_->send(pCC, mqUpdate.getSerializedLength());

								}
								break;

								case MqOpModeNormal:
								{
									MarqueeTest mqt(MqAllOff);
									mqt.setSourceObject(this);

									const char *pCC = mqt.serialize();
									pMyMarquee_->send(pCC, mqt.getSerializedLength());

								}
								break;

							}

							
						}
						break;
		default			:
						{
							Debug::trace("MarqueeManager.Error", HobString("Some Other Event: ") +
																 HobString(Integer::toString((UINT16) mqEvent)));
						}
						break;
	}
}


//void MarqueeManager::handleOperationModeEvent(OpModeEvent opModeEvent, const char * /* recvBuffer */)
//{
//	switch (opModeEvent)
//	{
//		default:
//						break;
//	}
//}

void MarqueeManager::timeoutProcess( __int32 timer_id )
{

	if ( timer_id == marqueeProbe )
	{
		probeTimeoutCntr_++;
		switch (probeTimeoutCntr_)
		{
		case 0:
		case 1://have received some messages from the weigher since last timeout.
			break;
		case 2:
			{
				Debug::trace("MarqueeManager.MarqueeProbe", HobString("Probe Timeout requesting wakeup"));

				requestMarqueeWakeup();

			}
			break;
		case 3:
			{
				Debug::trace("MarqueeManager.MarqueeProbe", HobString("Marquee Lost Contact"));

				probeTimeoutCntr_ = 0;

				if (lastStatusMsg_.isPeripheralComm() == true)
				{
					lastStatusMsg_.clearPeripheralComm();
					dispatchEvent(&lastStatusMsg_);
				}

			}
			break;
		}

	}
	else if (timer_id == marqueeAsciiTest)
	{
		const char *pCC = NULL;
		std::string displayBuffer;
		Debug::trace("MarqueeManager.AsciiTest", HobString("timer timed out"));


		testAsciiChar_ ++;
		if (testAsciiChar_ >= 0x7f)
			testAsciiChar_ = 0x21;

		Debug::trace("MarqueeManager.AsciiTest", std::string("ascii char = '") + testAsciiChar_ + std::string("'"));

		displayBuffer = "\\F";

		for (int i = 0; i < 20; i++)
			displayBuffer += testAsciiChar_;

		MarqueeUpdate mu(MqProduct, const_cast<char *>(displayBuffer.c_str()), this);

		pCC = mu.serialize();
		pMyMarquee_->send(pCC, mu.getSerializedLength());



	}	

}


void MarqueeManager::receivedMarqueeFeedback(int msgStats)
{

	if ((msgStats &  MarqueeStatus::COMM_CHANGED) > 0) 
	{
		Debug::trace("MarqueeManager.MarqueeProbe", HobString("enabling marquee"));

		//always keep the marquee enabled
		MarqueeGeneric mmg(MqEnable, this);

		const char *pCC = mmg.serialize();
		pMyMarquee_->send(pCC, mmg.getSerializedLength());

		sendMarqueeKind();
	}
	if (msgStats >= MarqueeStatus::COMM_CHANGED)
		dispatchEvent(&lastStatusMsg_);


	probeTimeoutCntr_ = 0;

}

void MarqueeManager::requestMarqueeWakeup()
{
	MarqueeRequestWakeup mqrw(this, physicalAddress_);

	const char *pCC = mqrw.serialize();
	pMyMarquee_->send(pCC, mqrw.getSerializedLength());
}

void MarqueeManager::sendMarqueeKind()
{
	MarqueeMarqueeKind mqk(this, mqKind_);
	dispatchEvent(&mqk);
}

void MarqueeManager::testMarquee(int i)
{
	const char *pCC = NULL;

	if ((i & 0x400) > 0)
	{
		MarqueeMode mm(ProductMode2, this);

		pCC = mm.serialize();
		pMyMarquee_->send(pCC, mm.getSerializedLength());
	}


	if ((i & 0x200) > 0)
	{
		MarqueeMode mm(OnlyProductMode, this);

		pCC = mm.serialize();
		pMyMarquee_->send(pCC, mm.getSerializedLength());
	}


	if ((i & 0x100) > 0)
	{

		MarqueeGeneric mg(MqDisable, this);

		pCC = mg.serialize();
		pMyMarquee_->send(pCC, mg.getSerializedLength());
	}

	if ((i & 0x80) > 0)
	{
		requestMarqueeWakeup();
	}
	if ((i & 0x40) > 0)
	{

		MarqueeTest mt(MqCheckerBoard, this);

		pCC = mt.serialize();
		pMyMarquee_->send(pCC, mt.getSerializedLength());
	}

	if ((i & 0x20) > 0)
	{

		MarqueeGeneric mg(MqReqInquire, this);

		pCC = mg.serialize();
		pMyMarquee_->send(pCC, mg.getSerializedLength());
	}

	if ((i & 0x10) > 0)
	{

		MarqueeGeneric mg(MqEnable, this);

		pCC = mg.serialize();
		pMyMarquee_->send(pCC, mg.getSerializedLength());
	}

	if ((i & 0x08) > 0)
	{

		MarqueeUpdate mu(MqProduct, "hobart product", this);

		pCC = mu.serialize();
		pMyMarquee_->send(pCC, mu.getSerializedLength());
	}

	if ((i & 0x04) > 0)
	{

		MarqueeUpdate mu(MqProduct, "hobart product\\R", this);

		pCC = mu.serialize();
		pMyMarquee_->send(pCC, mu.getSerializedLength());
	}

	if ((i & 0x02) > 0)
	{

		MarqueeUpdate mu(MqDaily, "\\BDaily message\\B\\R", this);

		pCC = mu.serialize();
		pMyMarquee_->send(pCC, mu.getSerializedLength());
	}

	if ((i & 0x01) > 0)
	{

		MarqueeGeneric mg(MqClear, this);

		pCC = mg.serialize();
		pMyMarquee_->send(pCC, mg.getSerializedLength());
	}




}
