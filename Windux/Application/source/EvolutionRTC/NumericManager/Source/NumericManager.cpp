
#include "Configuration.h"

#include "NumericManager.h"

#include "NumericAnnunciator.h"
#include "NumericConfiguration.h"
#include "NumericField.h"
#include "NumericGeneric.h"
#include "NumericInquire.h"
#include "NumericMode.h"
#include "NumericNumericKind.h"
#include "NumericRequestWakeup.h"
#include "NumericStatus.h"
#include "NumericTest.h"
#include "NumericWakeup.h"

#include "NumericSystemStatus.h"
#include "SystemRequestStatus.h"

#include "NumericOpMode.h"

#include "VacuumFlourescentVirtualImage.h"


#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

static /*LOCAL*/ NumericManager* pManager = 0;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int NumericManager::dataType = 0;
std::string NumericManager::dataTypeName("NumericManager"); 
#endif

NumericManager::NumericManager(Hobart::CEvolMsgReceiver *pMyNumeric,
			UINT16 physicalAddress,
			NumericKind nmKind) 
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	Debug::trace("NumericManager", "NumericManager");

	this->pMyNumeric_ = pMyNumeric;
	this->physicalAddress_ = physicalAddress;
	this->nmKind_ = nmKind;
	this->numericVI_ = 0;

	// Register for all the OperationMode messages
	registerEvent(OperationMode, OpModeIdle);
	registerEvent(OperationMode, OpModeOperate);
	registerEvent(OperationMode, OpModePluEntry);
	registerEvent(OperationMode, OpModeAutoWeigh);
	registerEvent(OperationMode, OpModeSemiWeigh);
	registerEvent(OperationMode, OpModeManualWeigh);
	registerEvent(OperationMode, OpModeSupervisor);
	registerEvent(OperationMode, OpModeService);
	registerEvent(OperationMode, OpModePowerDown);

	registerEvent(Numeric, NmReqWakeup);
	registerEvent(Numeric, NmReqInquire);

	registerEvent(Numeric, NmWakeup);
	registerEvent(Numeric, NmConfig);
	registerEvent(Numeric, NmInquire);
	registerEvent(Numeric, NmReset);
	registerEvent(Numeric, NmEnable);
	registerEvent(Numeric, NmDisable);
	registerEvent(Numeric, NmMode);
	registerEvent(Numeric, NmTest);
	registerEvent(Numeric, NmField);
	registerEvent(Numeric, NmAnnuncicator);

	registerEvent(NumericSystem, SysStatus);
	registerEvent(NumericSystem, SysReqStatus);

	registerEvent(Numeric, NmOperateMode);

	pConfig_ = Configuration::getInstance();

	numericProbeTime_ = pConfig_->getCustDispProbeTime() * 1000;
	Debug::trace("NumericManager.NumericProbe", HobString("numericProbeTime_ = ") + Integer::toString(numericProbeTime_));

	annunciatorTestTime_ = 300;
	asciiTestTime_ = 3000;

	testAsciiChar_ = 0;

	testAnnunciatorIndex_ = 0;
	testAnnunciatorLastIndex_ = 0;

	nmOpMode_ = NmOpModeNormal;
	annunciatorsOnBitMask_ = 0;
	
	pManager = this;

	probeTimeoutCntr_ = 0;
	createTimer(numericProbeTime_, numericProbe, CHobartTimer::PERIODIC_TIMER);

	sendNumericKind();

	requestNumericWakeup();
}

NumericManager::~NumericManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
	shutdownTimerThreads();
	ShutdownNicely();
	
	Debug::trace("NumericManager", "~NumericManager");

	lastStatusMsg_.setManagerDeleted();
	dispatchEvent(&lastStatusMsg_);
	
	if (numericVI_)
		delete numericVI_;
			
}


void NumericManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{

	if ( msg.get() != NULL )
	{
		switch (msg->getDeviceType())
		{
			case Numeric		:	handleNumericEvent((NmEvent) msg->getEventType(), msg->Data());
									break;
			case NumericSystem	:	handleNumericSystemEvent((NmEvent) msg->getEventType(), msg->Data());
									break;
			//case OperationMode	:	handleOperationModeEvent((OpModeEvent) msg->getEventType(), msg->Data());
			//						break;
			default				:
									break;
		}
	}


}

void NumericManager::startAsciiCharacterTest()
{
	Debug::trace("NumericManager.AsciiTest", "start ascii test");

	testAsciiChar_ = 0x20;

	timeoutProcess( numericAsciiTest );

	createTimer(asciiTestTime_, numericAsciiTest, CHobartTimer::PERIODIC_TIMER);

}



void NumericManager::stopAsciiCharacterTest()
{

	Debug::trace("NumericManager.AsciiTest", "stop ascii test");

	testAsciiChar_ = 0x0;

	cancelTimer(numericAsciiTest);

}

void NumericManager::startAnnunciatorTest()
{
	Debug::trace("NumericManager.AnnunciatorTest", "start annunciator test ");

	testAnnunciatorIndex_ = 1;
	testAnnunciatorLastIndex_ = 0;


	createTimer(annunciatorTestTime_, numericAnnunciatorTest, CHobartTimer::PERIODIC_TIMER);


}


void NumericManager::stopAnnunciatorTest()
{

	Debug::trace("NumericManager.AnnunciatorTest", "stop annunciator test ");

	testAnnunciatorIndex_ = 0;

	cancelTimer(numericAnnunciatorTest);

}

void NumericManager::handleNumericSystemEvent(NmEvent nmEvent, const char *recvBuffer)
{

	switch (nmEvent)
	{
		/*received fake system status from usb device*/
		case SysStatus:
		{
			Debug::trace("NumericManager.Messages.In", HobString("systemStatus"));

			NumericSystemStatus nmss(recvBuffer);
			nmss.setSourceObject(this);

			dispatchEvent(&nmss);

			if (Debug::isTracing("NumericManager.Messages.In") == true)
			{
				nmss.show();
			}

		}
		break;
		/*messages sent out to usb device*/
		case SysReqStatus:		
		{
			Debug::trace("NumericManager.Messages.Out", HobString("SysReqStatus"));

			SystemRequestStatus srs(recvBuffer);

			const char *pRSS = srs.serialize();
			pMyNumeric_->send(pRSS, srs.getSerializedLength());

		}
		break;

		default:
		{
			Debug::trace("NumericManager.Error", HobString("Some Other Sytem Event: ") +
												 HobString(Integer::toString((UINT16) nmEvent)));
		}
		break;

	}

}


void NumericManager::handleNumericEvent(NmEvent nmEvent, const char *recvBuffer)
{


	switch (nmEvent)
	{
		/*received messages from usb device*/
		case NmInquire:
						{
							Debug::trace("NumericManager.Messages.In", HobString("NmInquire"));

							NumericInquire nmi(recvBuffer);
							nmi.setSourceObject(this);

							receivedNumericFeedback(lastStatusMsg_.updateStatus(nmi));

							if (Debug::isTracing("NumericManager.Messages.In") == true)
							{
								nmi.show();
							}

						}
						break;

		case NmWakeup:		
						{
							Debug::trace("NumericManager.Messages.In", HobString("NmWakeup"));

							NumericWakeup nmw(recvBuffer);
							nmw.setSourceObject(this);

							receivedNumericFeedback(lastStatusMsg_.updateStatus(nmw));

							if (Debug::isTracing("NumericManager.Messages.In") == true)
							{
								nmw.show();
							}

							if (numericVI_ == 0)
							{
								switch (nmw.getNmDisplayType())
								{
									case NumericVacuumFlourescentDisplay:
										numericVI_ = new VacuumFlourescentVirtualImage();
										break;
									default:
										numericVI_ = new VacuumFlourescentVirtualImage();
										break;
								}

							}


						}
						break;

		/*messages sent out to usb device*/
		case NmConfig:		
						{
							Debug::trace("NumericManager.Messages.Out", HobString("NmConfig"));

							NumericConfiguration nmc(recvBuffer);
							nmc.setSourceObject(this);

							const char *pCC = nmc.serialize();
							pMyNumeric_->send(pCC, nmc.getSerializedLength());

						}
						break;
		case NmEnable:		Debug::trace("NumericManager.Messages.Out", HobString("NmEnable"));
		case NmDisable:		Debug::trace("NumericManager.Messages.Out", HobString("NmDisable"));
						{
							NumericGeneric nmg(recvBuffer);
							nmg.setSourceObject(this);

							const char *pCC = nmg.serialize();
							pMyNumeric_->send(pCC, nmg.getSerializedLength());
						}
						break;

		case NmReset:		Debug::trace("NumericManager.Messages.Out", HobString("NmReset"));
						{
							clearAnnunciators();

							NumericGeneric nmg(recvBuffer);
							nmg.setSourceObject(this);

							const char *pCC = nmg.serialize();
							pMyNumeric_->send(pCC, nmg.getSerializedLength());
						}
						break;


		case NmMode:		
						{
							Debug::trace("NumericManager.Messages.Out", HobString("NmMode"));

							NumericMode nmm(recvBuffer);
							nmm.setSourceObject(this);

							if (Debug::isTracing("NumericManager.Messages.Out") == true)
							{
								nmm.show();
							}

							const char *pCC = nmm.serialize();
							pMyNumeric_->send(pCC, nmm.getSerializedLength());
						}
						break;
		case NmTest:
						{
							Debug::trace("NumericManager.Messages.Out", HobString("NmTest"));

							NumericTest nmt(recvBuffer);

							switch(nmt.getNmTestVal())
							{
								case NmCharacter:
								{
									if (testAsciiChar_ == (char)0)
										startAsciiCharacterTest();
									break;								
								}
								case NmAnnunciator:
								{
									if (testAnnunciatorIndex_ == (char)0)
										startAnnunciatorTest();
									break;
								}
								default:								
								{

									if (testAsciiChar_ > (char)0)
										stopAsciiCharacterTest();

									if (testAnnunciatorIndex_ > (char)0)
										stopAnnunciatorTest();


									nmt.setSourceObject(this);

									if (Debug::isTracing("NumericManager.Messages.Out") == true)
									{
										nmt.show();
									}

									const char *pCC = nmt.serialize();
									pMyNumeric_->send(pCC, nmt.getSerializedLength());

									break;
								}
							}


						}
						break;
		case NmField:
						{
							Debug::trace("NumericManager.Messages.Out", HobString("NmField"));

							if (nmOpMode_ == NmOpModeNormal)
							{
								NumericField nmf(recvBuffer);
								NmFieldType ft = nmf.getNmFieldType();

								//numeric is display wide, no justification is done in this case
								//so just pass the message on to the usbcomm manager.
								if (ft == NmNumeric)
								{
									const char *pCC = nmf.serialize();
									pMyNumeric_->send(pCC, nmf.getSerializedLength());
								}
								else if (numericVI_)
								{
									char* justified = numericVI_->setField(ft, nmf.getFieldData());

									nmf.setFieldData(justified);

									//special case. pounds per price is actually sent to the unit price field
									if (ft == NmPoundsPerPrice)
										nmf.setNmFieldType(NmUnitPrice);									

									nmf.setSourceObject(this);

									const char *pCC = nmf.serialize();
									pMyNumeric_->send(pCC, nmf.getSerializedLength());
								}
								if (Debug::isTracing("NumericManager.Messages.Out") == true)
								{
									nmf.show();
								}
							}
						}
						break;
		case NmAnnuncicator :
						{
							Debug::trace("NumericManager.Messages.Out", HobString("NmAnnuncicator"));

							if (nmOpMode_ == NmOpModeNormal)
							{
								NumericAnnunciator nma(recvBuffer);
								nma.setSourceObject(this);
								int shiftIndex = (int)nma.getNmAnnuncicatorIndex();
								UINT32 bitMask = 1 << shiftIndex;

								if (nma.getNmState() == NmOn)
									annunciatorsOnBitMask_ |= bitMask;
								else
									annunciatorsOnBitMask_ &= (~bitMask);

								if (Debug::isTracing("NumericManager.Messages.Out") == true)
								{
									nma.show();
								}

								const char *pCC = nma.serialize();
								pMyNumeric_->send(pCC, nma.getSerializedLength());
							}
						}
						break;
		case NmReqInquire:
						{
							Debug::trace("NumericManager.Messages.Out", HobString("NmReqInquire"));

							NumericGeneric nmri(recvBuffer);
							nmri.setSourceObject(this);

							const char *pCC = nmri.serialize();
							pMyNumeric_->send(pCC, nmri.getSerializedLength());
						}
						break;
		case NmReqWakeup:
						{
							Debug::trace("NumericManager.Messages.Out", HobString("NmReqWakeup"));

							NumericRequestWakeup nrw(recvBuffer);
							nrw.setSourceObject(this);
							nrw.setPhysicalAddress(physicalAddress_);

							const char *pCC = nrw.serialize();
							pMyNumeric_->send(pCC, nrw.getSerializedLength());
						}
						break;

		case NmOperateMode:
						{
							Debug::trace("NumericManager.Messages.Out", HobString("NmReqWakeup"));

							NumericOpMode nmOpMode(recvBuffer);

							nmOpMode_ = nmOpMode.getNmOpMode();

							switch (nmOpMode_)
							{
								case NmOpModeOutOfServ:
								case NmOpModeNormal:
								{
									NumericField nmField(NmNumeric, "                         ");

									nmField.setSourceObject(this);
									const char *pCC = nmField.serialize();
									pMyNumeric_->send(pCC, nmField.getSerializedLength());

									clearAnnunciators();
								}
								break;

							}

							
						}
						break;
		default			:
						{
							Debug::trace("NumericManager.Error", HobString("Some Other Event: ") +
																 HobString(Integer::toString((UINT16) nmEvent)));
						}
						break;
	}
}

void NumericManager::clearAnnunciators()
{
int endOfList = (int)NmCodeInvalid;
const char *pCC = 0;
UINT32 bitmask = 0;
int i = 0;

	for (i = 0, bitmask = 1; i < endOfList; i++, bitmask <<= 1)
	{
		if (annunciatorsOnBitMask_ & bitmask)
		{
			annunciatorsOnBitMask_ &= (~bitmask);

			NumericAnnunciator nma((NmAnnuncicatorIndex)i, NmOff);
			nma.setSourceObject(this);
			pCC = nma.serialize();
			pMyNumeric_->send(pCC, nma.getSerializedLength());
		}
	}
}

//void NumericManager::handleOperationModeEvent(OpModeEvent opModeEvent, const char * /* recvBuffer */)
//{
//	switch (opModeEvent)
//	{
//		default:
//						break;
//	}
//}

void NumericManager::timeoutProcess( __int32 timer_id )
{
	if ( timer_id == numericProbe )
	{
		probeTimeoutCntr_++;
		switch (probeTimeoutCntr_)
		{
		case 0:
		case 1://have received some messages from the weigher since last timeout.
			break;
		case 2:
			{
				Debug::trace("NumericManager.NumericProbe", HobString("Probe timeout, requesting wakeup message"));

				requestNumericWakeup();

			}
			break;
		case 3:
			{
				Debug::trace("NumericManager.NumericProbe", HobString("Numeric Lost Contact"));

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
	else if (timer_id == numericAnnunciatorTest)
	{
		const char *pCC = NULL;
		Debug::trace("NumericManager.AnnunciatorTest", HobString("timer timed out"));

		testAnnunciatorIndex_ ++;
		if (testAnnunciatorIndex_ > 20)
			testAnnunciatorIndex_ = 1;


		if (testAnnunciatorLastIndex_ > 0)
		{
			int lastIndex = (testAnnunciatorLastIndex_ - 1);

			Debug::trace("NumericManager.AnnunciatorTest", HobString("turn off annunciator index = ") + Integer::toString(lastIndex));
			NumericAnnunciator na((NmAnnuncicatorIndex)lastIndex, NmOff, this);

			pCC = na.serialize();
			pMyNumeric_->send(pCC, na.getSerializedLength());
		}

		int newIndex = (testAnnunciatorIndex_ - 1);

		Debug::trace("NumericManager.AnnunciatorTest", HobString("turn on annunciator index = ") + Integer::toString(newIndex));

		NumericAnnunciator na((NmAnnuncicatorIndex)newIndex, NmOn, this);

		pCC = na.serialize();
		pMyNumeric_->send(pCC, na.getSerializedLength());

		testAnnunciatorLastIndex_ =  testAnnunciatorIndex_;


	}
	else if (timer_id == numericAsciiTest)
	{
		const char *pCC = NULL;
		std::string displayBuffer;
		Debug::trace("NumericManager.AsciiTest", HobString("timer timed out"));


		testAsciiChar_ ++;
		if (testAsciiChar_ >= 0x7f)
			testAsciiChar_ = 0x21;

		Debug::trace("NumericManager.AsciiTest", std::string("ascii char = '") + testAsciiChar_ + std::string("'"));

		for (int i = 0; i < 20; i++)
			displayBuffer += testAsciiChar_;

		NumericField nf(NmNumeric,  const_cast<char *>(displayBuffer.c_str()), this);

		pCC = nf.serialize();
		pMyNumeric_->send(pCC, nf.getSerializedLength());


	}


}


void NumericManager::receivedNumericFeedback(int msgStats)
{

	//we have now just established/re-established communications
	if ((msgStats & NumericStatus::COMM_CHANGED) > 0)
	{
		Debug::trace("NumericManager.NumericProbe", HobString("enabling numeric"));

		sendNumericKind();

		//always keep the numeric enabled
		NumericGeneric nmg(NmEnable, this);

		const char *pCC = nmg.serialize();
		pMyNumeric_->send(pCC, nmg.getSerializedLength());
	}

	if (msgStats >= NumericStatus::COMM_CHANGED)
		dispatchEvent(&lastStatusMsg_);

	probeTimeoutCntr_ = 0;

}

void NumericManager::requestNumericWakeup()
{
	NumericRequestWakeup nmrw(this, physicalAddress_);

	const char *pCC = nmrw.serialize();
	pMyNumeric_->send(pCC, nmrw.getSerializedLength());
}

void NumericManager::sendNumericKind()
{
	NumericNumericKind nmk(this, nmKind_);
	dispatchEvent(&nmk);
}


