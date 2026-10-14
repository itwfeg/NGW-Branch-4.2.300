#include "OpModeEvent.h"
#include "ServiceMode.h"
#include "Configuration.h"

#include "WeigherManager.h"

#include "WrapperManager.h"
#include "WrapperCycle.h"
#include "WrapperJog.h"
#include "PrinterLabelReady.h"
#include "LabelApplierLabelReady.h"
#include "EvolWrapperControl.h"
#include "EvolWrapperTest.h"
#include "ServiceSpeaker.h"

#include "PrinterStatus.h"
#include "PrinterLabelComplete.h"

#include "ProductUpdateMsg.h"
#include "ProductChangeMsg.h"

#include "ProductTransaction.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"
#include "Hex.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ServiceMode::dataType = 0;
std::string ServiceMode::dataTypeName("ServiceMode"); 
#endif

//ServiceMode::ServiceMode(WeigherManager *pWeigherManager,
						 //PrinterManager *pFirstPrinter,
						 //PrinterManager *pSecondPrinter,
						 //PrinterManager *pThirdPrinter,
						 //LabelApplierManager *pLabelApplierManager,
						 //WrapperManager *pWrapperManager)
ServiceMode::ServiceMode(Hobart::CEvolMsgReceiver *pWeigherManager,
						 Hobart::CEvolMsgReceiver *pFirstPrinter,
						 Hobart::CEvolMsgReceiver *pSecondPrinter,
						 Hobart::CEvolMsgReceiver *pThirdPrinter,
						 Hobart::CEvolMsgReceiver *pLabelApplierManager,
						 Hobart::CEvolMsgReceiver *pWrapperManager)
						 		 : OperatingMode("tSrvcMode",
						 												  NULL,
						 												  pWeigherManager,
						 												  pFirstPrinter,
						 												  pSecondPrinter,
						 												  pThirdPrinter,
						 												  pLabelApplierManager,
						 												  pWrapperManager)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	registerEvent(Wrapper, WrStatus);
	registerEvent(Wrapper, WrEvolStatus);
	registerEvent(Wrapper, WrWakeup);
	registerEvent(Wrapper, WrCycle);
	registerEvent(Wrapper, WrJog);
	registerEvent(Wrapper, WrSensors);
	registerEvent(Wrapper, WrTest);
	registerEvent(Printer, PrLabelReady);
	registerEvent(Wrapper, WrEvolControl);
	registerEvent(Wrapper, WrEvolTest);

	//registerEvent(ProductDev, ProductChange);
	registerEvent(ProductDev, ProductUpdate);
    registerEvent(ProductDev, ProductChangeSmart);

	registerEvent(Printer, PrStatus);
	registerEvent(Printer, PrLabelComplete);

	wrStateInfo = EvolWrapperStatusInfo::Unknown;
	wrapperCalibrating = false;

	setWrapperMode(WrServiceMode);
	setLabelApplierMode(CService);

	stateMachine = mwmInitial;

	requestWrapperStatus();

	laStateMachine1 = wmLaInitial;
	Debug::trace("ServiceMode", "laStateMachine1: wmLaInitial");

	laStateMachine2 = wmLaInitial;
	Debug::trace("ServiceMode", "laStateMachine2: wmLaInitial");

	laStateMachine3 = wmLaInitial;
	Debug::trace("ServiceMode", "laStateMachine3: wmLaInitial");

	if (Debug::isTracing("ServiceMode"))
	{
		Debug::trace("ServiceMode", "*****   Constructor   *****");
		Debug::trace("ServiceMode", "pWeigherManager:            0x" + HobString(Hex::toString((long long) pWeigherManager)));
		Debug::trace("ServiceMode", "pFirstPrinter:              0x" + HobString(Hex::toString((long long) pFirstPrinter)));
		Debug::trace("ServiceMode", "pSecondPrinter:             0x" + HobString(Hex::toString((long long) pSecondPrinter)));
		Debug::trace("ServiceMode", "pThirdPrinter:              0x" + HobString(Hex::toString((long long) pThirdPrinter)));
		Debug::trace("ServiceMode", "pLabelApplierManager:       0x" + HobString(Hex::toString((long long) pLabelApplierManager)));
//		Debug::trace("ServiceMode", "pBakeryLabelApplierManager: 0x" + HobString(Hex::toString((UINT32) pBakeryLabelApplierManager)));
		Debug::trace("ServiceMode", "pWrapperManager:            0x" + HobString(Hex::toString((long long) pWrapperManager)));
	}

	solenoidTests.insert(std::make_pair(WrapperTest::SlideBlockOnTest, new EncoderPositionRange(EncoderWindow_3) ));
	solenoidTests.insert(std::make_pair(WrapperTest::SlideBlockOffTest, new EncoderPositionRange(EncoderWindow_3) ));

	solenoidTests.insert(std::make_pair(WrapperTest::WideFilmWidthTest, new EncoderPositionRange(EncoderWindow_2) ));
	solenoidTests.insert(std::make_pair(WrapperTest::NarrowFilmWidthTest, new EncoderPositionRange(EncoderWindow_2) ));

	solenoidTests.insert(std::make_pair(WrapperTest::ShortFilmLengthTest, new EncoderPositionRange(EncoderWindow_1) ));
	solenoidTests.insert(std::make_pair(WrapperTest::MediumFilmLengthTest, new EncoderPositionRange(EncoderWindow_1) ));
	solenoidTests.insert(std::make_pair(WrapperTest::LongFilmLengthTest, new EncoderPositionRange(EncoderWindow_1) ));

	solenoidTests.insert(std::make_pair(WrapperTest::KnifeOnTest, new EncoderPositionRange(EncoderWindow_2) ));
	solenoidTests.insert(std::make_pair(WrapperTest::KnifeOffTest, new EncoderPositionRange(EncoderWindow_2) ));

	solenoidTests.insert(std::make_pair(WrapperTest::GripperOnTest, new EncoderPositionRange(EncoderWindow_3) ));
	solenoidTests.insert(std::make_pair(WrapperTest::GripperOffTest, new EncoderPositionRange(EncoderWindow_3) ));

	solenoidTests.insert(std::make_pair(WrapperTest::LeftSideClampOnTest, new EncoderPositionRange(EncoderWindow_ALL) ));
	solenoidTests.insert(std::make_pair(WrapperTest::LeftSideClampOffTest, new EncoderPositionRange(EncoderWindow_ALL) ));

	solenoidTests.insert(std::make_pair(WrapperTest::RightSideClampOnTest, new EncoderPositionRange(EncoderWindow_ALL) ));
	solenoidTests.insert(std::make_pair(WrapperTest::RightSideClampOffTest, new EncoderPositionRange(EncoderWindow_ALL) ));


	takeLabelTime = 5;
	setTakeLabelTime(FirstPrinter);
	setTakeLabelTime(SecondPrinter);
	setTakeLabelTime(ThirdPrinter);
}

ServiceMode::~ServiceMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	deregisterAllEvents();

	// NGW-259: prevent crash when processing message during destructor
	ShutdownNicely();

	setWeigherMode(ScaleManualMode);
	setLabelApplierMode(Conveyor);		//	Exiting ServiceMode

	EncoderPositionMap::iterator pos = solenoidTests.begin();
	while (pos != solenoidTests.end())
	{
		EncoderPositionRange *pEPR = pos->second;

		solenoidTests.erase(pos++);
		delete pEPR;
	}
	Debug::trace("ServiceMode", "Destructor");
}				

#include "ProductManager.h"

void ServiceMode::handleProductSmartPtr(Util::CSmartPtr<Product> &pCurrentProductSmartPtr)
{
	Product *pProduct = 0;

	pProduct = pCurrentProductSmartPtr.get();

	emptyProductQ();
Debug::trace("ServiceMode", "handleProductSmartPtr product*=" + util_string::ptrtostring(pProduct));

	if (pProduct)
		productQ.push(pCurrentProductSmartPtr);
	else
		pCurrentProduct = 0;
Debug::trace("ServiceMode.StateMachine", "stateMachine is " + util_string::inttostring(stateMachine));

	//if (stateMachine != mwmWrapPrinterHeadUp)
	if (stateMachine == mwmInitial || stateMachine == mwmWaitingForProductUpdate)
	{
		if (getNewProduct() == true)
		{
			changeProduct();
			preImagePrinters();	

			stateMachine = mwmWaitingForProductUpdate;
			Debug::trace("ServiceMode.StateMachine", "stateMachine = mwmWaitingForProductUpdate");
		}
		else if (Configuration::getInstance()->isSelfServiceScale())
		{
			// there was a system crash when booting up a SelfServe scale, not taking the label that sizes during bootup,
			// lookup a plu, press the 'Start Over' button, and then take the label.  The printer manager was
			// told to pre-image the label, but since sizing hadn't totally finished, the PrinterManager just
			// held on to the pointer to the current product; it would have pe imaged when the sizing finished.
			// But pressing 'Start Over' deleted that current product, but no one passed that along to the
			// PrinterManager, which resulted in it having a pointer to a product that has been deleted by the Session.
			// For now, since self-serve scales are the only ones that don't require the operator taking the label before
			// entering the first plu number, I'm only having this code execute if this is a self-serve scale.

			pCurrentProduct = 0;
			preImagePrinters();	
		}
	}
}


void ServiceMode::handleProductMessage(ProductEvent productEvent, const char *recvBuffer)
{
	const bool traceOn( Debug::isTracing("ServiceMode") );

	switch (productEvent)
	{
		// Operator has chosen a new product
		//case ProductChange:
		case ProductChangeSmart:
		{
			if (traceOn)
				Debug::trace("ServiceMode", "handleProductMessage: ProductChangeSmart");

            ProductChangeMsg prodChange(recvBuffer);

			ProductManager *productManager = (ProductManager *) prodChange.getProductMgr();
			if (productManager)
			{
				pCurrentProductSmartPtr = productManager->getCurrentProductSmartPtr();
				handleProductSmartPtr(pCurrentProductSmartPtr);
			}

			break;
		}

		case ProductUpdate:
		{
			if (traceOn)
				Debug::trace("ServiceMode", "handleProductMessage: ProductUpdate");

			ProductUpdateMsg prodUpdate(recvBuffer);
			const ProductTransaction *pNextProductTransaction = prodUpdate.getProductTransaction();

/*
			if (Configuration::getInstance()->toBoolCfgI(CfgInternal::SIMULATEPRINTING))
			{
				if (pNextProductTransaction)
					sendProductComplete(pNextProductTransaction, true);
				break;
			}
*/

			if (stateMachine == mwmWaitingForProductUpdate)
			{
				pCurrentProductTransaction = pNextProductTransaction;

				if ( printerScenario == PrimaryPrinterOnly || printerScenario == SupportTextOnPrimary )
				{
					postImagePrinter(FirstPrinter, pFirstPrinter);

					stateMachine = mwmPrintingPrimary;
					Debug::trace("ServiceMode.StateMachine", "stateMachine = mwmPrintingPrimary");
				}
				else if ( printerScenario == SecondPrinterOnly || printerScenario == SupportTextOnSecondary )
				{
					postImagePrinter(SecondPrinter, pSecondPrinter);

					stateMachine = mwmPrintingSecond;
					Debug::trace("ServiceMode.StateMachine", "stateMachine = mwmPrintingSecond");
				}
				else if ( printerScenario == ThirdPrinterOnly || printerScenario == SupportTextOnTertiary )
				{
					postImagePrinter(ThirdPrinter, pThirdPrinter);

					stateMachine = mwmPrintingThird;
					Debug::trace("ServiceMode.StateMachine", "stateMachine = mwmPrintingThird");
				}
				else if (printerScenario == PrimaryAndSecondPrinter)
				{
					postImagePrinter(FirstPrinter, pFirstPrinter);
					postImagePrinter(SecondPrinter, pSecondPrinter);

					stateMachine = mwmPrintingPrimaryAndSecond;
					Debug::trace("ServiceMode.StateMachine", "stateMachine = mwmPrintingPrimaryAndSecond");
				}
				else if (printerScenario == PrimaryAndThirdPrinter)
				{
					postImagePrinter(FirstPrinter, pFirstPrinter);
					postImagePrinter(ThirdPrinter, pThirdPrinter);

					stateMachine = mwmPrintingPrimaryAndThird;
					Debug::trace("ServiceMode.StateMachine", "stateMachine = mwmPrintingPrimaryAndThird");
				}
				else if (printerScenario == SecondAndThirdPrinter)
				{
					postImagePrinter(SecondPrinter, pSecondPrinter);
					postImagePrinter(ThirdPrinter, pThirdPrinter);

					stateMachine = mwmPrintingSecondAndThird;
					Debug::trace("ServiceMode.StateMachine", "stateMachine = mwmPrintingSecondAndThird");
				}
				else if (printerScenario == PrimarySecondAndThirdPrinter)
				{
					postImagePrinter(FirstPrinter, pFirstPrinter);
					postImagePrinter(SecondPrinter, pSecondPrinter);
					postImagePrinter(ThirdPrinter, pThirdPrinter);

					stateMachine = mwmPrintingPrimarySecondAndThird;
					Debug::trace("ServiceMode.StateMachine", "stateMachine = mwmPrintingPrimarySecondAndThird");
				}
				else
				{
					Debug::trace("ServiceMode.Error", "Invalid printerScenario");
				}
			}
			else
			{
				// This is an error condition!!
				Debug::trace("ServiceMode.Error", "Missed Product Update");
				sendProductComplete(pNextProductTransaction, false);
			}

/*
			if (Configuration::getInstance()->toBoolCfgI(CfgInternal::SIMULATEPRINTING))
			{
				PAL_absclass::PAL_sleep(2000);
				sendProductComplete(pNextProductTransaction, true);
			}
*/

			break;
		}

		default:
		{
			Debug::trace("ServiceMode", "handleProductMessage():  ???");
			break;
		}
	}					
}

void ServiceMode::handlePrinterMessage(PrEvent prEvent, const char *recvBuffer)
{
	bool traceOn = Debug::isTracing("ServiceMode");
	switch (prEvent)
	{
		// We listen to this message with the only purpose of relaying this
		// information to the corresponding label applier device.
		case PrLabelReady: 
		{
			if (traceOn)
				Debug::trace("ServiceMode", "handlePrinterMessage: PrLabelReady");

			PrinterLabelReady prLabelReady(recvBuffer);
			PrinterType printerType = prLabelReady.getPrinterType();

			if ( printerType == FirstPrinter )
			{
				laStateMachine1 = wmLaLabelReady;
				
				if (traceOn)
					Debug::trace("ServiceMode", "laStateMachine1 = wmLaLabelReady");

				sendLabelReady(PrimaryPrinterStation, prLabelReady.getPackId());
			}
			else if ( printerType == SecondPrinter )
			{
				laStateMachine2 = wmLaLabelReady;
				
				if (traceOn)
					Debug::trace("ServiceMode", "laStateMachine2 = wmLaLabelReady");

				sendLabelReady(SecondaryPrinterStation, prLabelReady.getPackId());
			}
			else if ( printerType == ThirdPrinter )
			{
				laStateMachine3 = wmLaLabelReady;
				if (traceOn)
					Debug::trace("ServiceMode", "laStateMachine3 = wmLaLabelReady");

				sendLabelReady(ThirdPrinterStation, prLabelReady.getPackId());
			}
			break;
		}

		case PrStatus: 
		{						
			PrinterStatus prStatus(recvBuffer);
			PrinterType printerType = prStatus.getPrinterType();

			if (prStatus.isHeadDisengaged() == true)
			{
				updatePrinterHeadUp(printerType);
				
				if (stateMachine != mwmWrapPrinterHeadUp)
				{
					stateMachine = mwmWrapPrinterHeadUp;
					Debug::trace("ServiceMode.StateMachine", "stateMachine = mwmWrapPrinterHeadUp");

					productFailed();
				}
			}
			else if (prStatus.isSizeCmdComplete() == true)
			{
				updateSizeCmdComplete(printerType);
				
				if (isPrinterHeadUp() == false)
				{
					if ((getNewProduct() == true) || (pCurrentProduct != NULL))
					{
						changeProduct();
						preImagePrinters();	

						stateMachine = mwmWaitingForProductUpdate;
						Debug::trace("ServiceMode.StateMachine", "stateMachine = mwmWaitingForProductUpdate");
					}
					else
					{
						stateMachine = mwmInitial;
						Debug::trace("ServiceMode.StateMachine", "stateMachine = mwmInitial");
					}
				}
			}
			break;
		}

		case PrLabelComplete: 
		{						
			Debug::trace("ServiceMode.StateMachine", HobString("mwm - handling PrLabelComplete - stateMachine upon entry is: ") + HobString(Integer::toString(stateMachine)));

			PrinterLabelComplete prLabelComplete(recvBuffer);
			PrinterType printerType = prLabelComplete.getPrinterType();
			const ProductTransaction *pPT = prLabelComplete.getProductTransaction();
			bool printOk = prLabelComplete.isPrintOk();

			if ( (((stateMachine == mwmPrintingPrimary) || (stateMachine == mwmWaitingForPrimary)) && (printerType == FirstPrinter)) ||
				 (((stateMachine == mwmPrintingSecond)  || (stateMachine == mwmWaitingForSecond))  && (printerType == SecondPrinter)) ||
				 (((stateMachine == mwmPrintingThird)   || (stateMachine == mwmWaitingForThird))   && (printerType == ThirdPrinter)) )
			{
				sendProductComplete(pPT, printOk);

				stateMachine = mwmWaitingForProductUpdate;
				Debug::trace("ServiceMode.StateMachine", "stateMachine = mwmWaitingForProductUpdate");

				if (getNewProduct() == true)
				{
					changeProduct();
					preImagePrinters();
				}
			}
			else
			{
				if (((stateMachine == mwmPrintingPrimaryAndSecond || stateMachine == mwmWaitingForPrimaryAndSecond) && printerType == SecondPrinter) ||
					((stateMachine == mwmPrintingPrimaryAndThird || stateMachine == mwmWaitingForPrimaryAndThird) && printerType == ThirdPrinter))
				{
					stateMachine = mwmWaitingForPrimary;
					Debug::trace("ServiceMode.StateMachine", "stateMachine = mwmWaitingForPrimary");
				}
				else if (((stateMachine == mwmPrintingPrimaryAndSecond || stateMachine == mwmWaitingForPrimaryAndSecond) && printerType == FirstPrinter) ||
						 ((stateMachine == mwmPrintingSecondAndThird || stateMachine == mwmWaitingForSecondAndThird) && printerType == ThirdPrinter))
				{
					stateMachine = mwmWaitingForSecond;
					Debug::trace("ServiceMode.StateMachine", "stateMachine = mwmWaitingForSecond");
				}
				else if (((stateMachine == mwmPrintingPrimaryAndThird || stateMachine == mwmWaitingForPrimaryAndThird) && printerType == FirstPrinter) ||
						 ((stateMachine == mwmPrintingSecondAndThird || stateMachine == mwmWaitingForSecondAndThird) && printerType == SecondPrinter))
				{
					stateMachine = mwmWaitingForThird;
					Debug::trace("ServiceMode.StateMachine", "stateMachine = mwmWaitingForThird");
				}
				else if (stateMachine == mwmPrintingPrimarySecondAndThird)
				{
					if (printerType == FirstPrinter)
					{
						stateMachine = mwmWaitingForSecondAndThird;
						Debug::trace("ServiceMode.StateMachine", "stateMachine = mwmWaitingForSecondAndThird");
					}
					else if (printerType == SecondPrinter)
					{
						stateMachine = mwmWaitingForPrimaryAndThird;
						Debug::trace("ServiceMode.StateMachine", "stateMachine = mwmWaitingForPrimaryAndThird");
					}
					else if (printerType == ThirdPrinter)
					{
						stateMachine = mwmWaitingForPrimaryAndSecond;
						Debug::trace("ServiceMode.StateMachine", "stateMachine = mwmWaitingForPrimaryAndSecond");
					}
				}
			}
			break;
		}	
	
		default:
			break;
	}
}				

void ServiceMode::handleWrapperMessage(WrEvent wrEvent, const char *recvBuffer)
{
	switch (wrEvent)
	{
		case WrStatus:
			handleWrapperStatus(recvBuffer);
			break;
		case WrEvolStatus:
			handleWrapperEvolStatus(recvBuffer);
			break;
		case WrCycle:
			handleWrapperCycle(recvBuffer);
			break;
		case WrJog:
			handleWrapperJog(recvBuffer);
			break;
		case WrSensors:
			handleWrapperSensors(recvBuffer);
			break;
		case WrTest:
			handleWrapperTest(recvBuffer);
			break;
		case WrEvolControl:
			handleEvolWrapperControl(recvBuffer);
			break;
		case WrEvolTest:
			handleEvolWrapperTest(recvBuffer);
			break;
		default:
			break;
	}						
}

void ServiceMode::handleWrapperJog(const char *recvBuffer)
{
	if (pWrapperManager != NULL)
	{
		WrapperJog wrJog(recvBuffer);
		wrJog.setPackageId(incPackId());

		const char *pCC = wrJog.serialize();
		pWrapperManager->send(pCC, wrJog.getSerializedLength());
	}
}

void ServiceMode::handleWrapperCycle(const char *recvBuffer)
{
	if (pWrapperManager != NULL)
	{
		WrapperCycle wrCycle(recvBuffer);
		wrCycle.setPackageId(incPackId());

		const char *pCC = wrCycle.serialize();
		pWrapperManager->send(pCC, wrCycle.getSerializedLength());
	}
}

void ServiceMode::handleWrapperSensors(const char *recvBuffer)
{
	if (pWrapperManager != NULL)
	{
		WrapperSensors wrapperSense(recvBuffer);
		wrapperSense.setSourceObject(this);
		lastSenseMsg = wrapperSense;

		if (Debug::isTracing("SeviceMode.WrapperSense") == true)
		{	
			lastSenseMsg.show();
		}
	}
}

void ServiceMode::handleWrapperTest(const char *recvBuffer)
{
	if (pWrapperManager != NULL)
	{
		WrapperTest wt(recvBuffer);
		WrapperTest::WrTestCode testCode = wt.getWrTestCode();

		if (Debug::isTracing("SeviceMode.WrapperTest") == true)
		{	
			wt.show();
			lastSenseMsg.show();
			solenoidTests[testCode]->show();
		}

		if (testCode == WrapperTest::InitWrapperForTests)
		{
			sendWrapperTest(WrapperTest::CompressorOffTest);
			sendWrapperTest(WrapperTest::ShortFilmLengthTest);
			sendWrapperTest(WrapperTest::WideFilmWidthTest);
			sendWrapperTest(WrapperTest::LeftSideClampOffTest);
			sendWrapperTest(WrapperTest::RightSideClampOffTest);
			sendWrapperTest(WrapperTest::KnifeOffTest);
			sendWrapperTest(WrapperTest::GripperOffTest);
			sendWrapperTest(WrapperTest::SlideBlockOffTest);
		}
		else
		{
			if (passEncoderRangeTest(testCode) == true)
			{
				wt.setSourceObject(this);

				const char *pCC = wt.serialize();
				pWrapperManager->send(pCC, wt.getSerializedLength());
			}
			else
			{
				switch (testCode)
				{
					case WrapperTest::ShortFilmLengthTest:
					case WrapperTest::MediumFilmLengthTest:
					case WrapperTest::LongFilmLengthTest:
					{
												opModeStatus.setEncoderWindow_1();
												dispatchEvent(&opModeStatus);
												opModeStatus.clearEncoderWindow_1();
												break;
					}
					case WrapperTest::WideFilmWidthTest:
					case WrapperTest::NarrowFilmWidthTest:
					case WrapperTest::KnifeOnTest:
					case WrapperTest::KnifeOffTest:
					{
												opModeStatus.setEncoderWindow_2();
												dispatchEvent(&opModeStatus);
												opModeStatus.clearEncoderWindow_2();
												break;
					}
					case WrapperTest::SlideBlockOnTest:
					case WrapperTest::SlideBlockOffTest:
					case WrapperTest::GripperOnTest:
					case WrapperTest::GripperOffTest:
					{
												opModeStatus.setEncoderWindow_3();
												dispatchEvent(&opModeStatus);
												opModeStatus.clearEncoderWindow_3();
												break;
					}
					default:
												break;
				}
				Debug::trace("ServiceMode.WrapperTest", HobString("invalid encoder position for this test: ") +
														HobString(Integer::toString(lastSenseMsg.getEncoderCount())));
			}
		}
	}
}

bool ServiceMode::passEncoderRangeTest(WrapperTest::WrTestCode testCode)
{
	bool rv = true;

	if (solenoidTests.find(testCode) != solenoidTests.end())
	{
		rv = solenoidTests[testCode]->isEncoderInRange(lastSenseMsg.getEncoderCount());
	}

	return rv;
}

void ServiceMode::wrapperAttached(Hobart::CEvolMsgReceiver *pWrapperManager, WrapperSetWrapMode::WrapMode /* wrapMode */)
{
	_Mybase::wrapperAttached( pWrapperManager );
	setWrapperMode(WrServiceMode);
}

void ServiceMode::labelApplierAttached(Hobart::CEvolMsgReceiver *pLabelApplierManager)
{
	Debug::trace("ServiceMode", "labelApplierAttached()");
	_Mybase::labelApplierAttached( pLabelApplierManager );

	setLabelApplierMode(CService);
}


void ServiceMode::sendLabelReady(LaStationID stationId, UINT8 packageId)
{
	LabelApplierLabelReady labelReady(this, stationId, packageId);
	dispatchEvent(&labelReady);
}

void ServiceMode::handleEvolWrapperControl(const char *recvBuffer)
{
	bool traceOn = Debug::isTracing("ServiceMode");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "handleEvolWrapperControl()";
		Debug::trace("ServiceMode", msgDebug);
		msgDebug.append(":  ");
	}
	if (!recvBuffer)
	{
		if (traceOn)
		{
			HobString msgDebug2 = "recvBuffer is 0. Exiting!!!";
			Debug::trace("ServiceMode", msgDebug + msgDebug2);
		}
		return;
	}

	EvolWrapperControl wrControl(recvBuffer);

	if (traceOn)
	{
		Debug::trace("ServiceMode", "showing received EvolWrapperControl object -----------------");
		wrControl.show();
		Debug::trace("ServiceMode", "done showing received EvolWrapperControl object -----------------");
	}

	wrControl.setSourceObject(this);

	if (pWrapperManager != NULL)
	{
		const char *pCC = wrControl.serialize();
		pWrapperManager->send(pCC, wrControl.getSerializedLength());
	}
}

void ServiceMode::handleWrapperEvolStatus(const char *recvBuffer)
{
	bool traceOn = Debug::isTracing("ServiceMode");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "handleWrapperEvolStatus()";
		Debug::trace("ServiceMode", msgDebug);
		msgDebug.append(":  ");
	}
	EvolWrapperStatus statusMsg(recvBuffer);

	if (traceOn)
	{
		Debug::trace("ServiceMode", msgDebug + "**********   Start EvolWrapperStatus show()   **********");
		statusMsg.show();
		Debug::trace("ServiceMode", msgDebug + "**********    End  EvolWrapperStatus show()   **********");
	}

	if ((statusMsg.isPeripheralComm() == true) && (statusMsg.isManagerDeleted() == false))
	{
		wrStateInfo = statusMsg.getStatusInfo().getWrapperState();
		EvolWrapperStatusInfo::EvolWrapperState lastStateInfo = lastWrapperStatus.getStatusInfo().getWrapperState();
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			HobString tempState;

			EvolWrapperStatusInfo::getWrapperStateName(wrStateInfo, tempState);
			msgDebug2.append("wrStateInfo = ").append(tempState);
			msgDebug2.append(",   mainPower = ").append(Bool::toString(statusMsg.getStatusInfo().getMainPower()));
			Debug::trace("ServiceMode", msgDebug2);

			msgDebug2 = msgDebug;
			EvolWrapperStatusInfo::getWrapperStateName(lastStateInfo, tempState);
			msgDebug2.append("lastStateInfo = ").append(tempState);
			msgDebug2.append(",   mainPower = ").append(Bool::toString(lastWrapperStatus.getStatusInfo().getMainPower()));
			Debug::trace("ServiceMode", msgDebug2);
		}

		if (wrapperCalibrating)
		{
			if (wrStateInfo == EvolWrapperStatusInfo::Running)
				wrapperCalibrating = false;
		}
		else
		{
			if ((wrStateInfo == EvolWrapperStatusInfo::Calibrate) ||
				(wrStateInfo == EvolWrapperStatusInfo::CalibratePlacePkgOnPrimary) ||
				(wrStateInfo == EvolWrapperStatusInfo::CalibrateRemovePkgOnPrimary))
				wrapperCalibrating = true;
		}
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("wrapperCalibrating = ").append(Bool::toString(wrapperCalibrating));
			Debug::trace("ServiceMode", msgDebug2);
		}

		if ((wrStateInfo == EvolWrapperStatusInfo::CalibratePlacePkgOnPrimary) && (lastStateInfo != EvolWrapperStatusInfo::CalibratePlacePkgOnPrimary))
		{
			opModeStatus.setEvolWrapperCalibratePlacePkg();
			dispatchEvent(&opModeStatus);
			if (traceOn)
				Debug::trace("ServiceMode", msgDebug + "  opModeStatus.setEvolWrapperCalibratePlacePkg()");
		}
		else if ((wrStateInfo != EvolWrapperStatusInfo::CalibratePlacePkgOnPrimary) && (lastStateInfo == EvolWrapperStatusInfo::CalibratePlacePkgOnPrimary))
		{
			opModeStatus.clearEvolWrapperCalibratePlacePkg();
			dispatchEvent(&opModeStatus);
			if (traceOn)
				Debug::trace("ServiceMode", msgDebug + "  opModeStatus.clearEvolWrapperCalibratePlacePkg()");
		}

		if ((wrStateInfo == EvolWrapperStatusInfo::BuildingAirPressure) && (lastStateInfo != EvolWrapperStatusInfo::BuildingAirPressure))
		{
			opModeStatus.setEvolWrapperBuildingAirPressure();
			dispatchEvent(&opModeStatus);
			if (traceOn)
				Debug::trace("ServiceMode", msgDebug + "  opModeStatus.setEvolWrapperBuildingAirPressure()");
		}
		else if ((wrStateInfo != EvolWrapperStatusInfo::BuildingAirPressure) && (lastStateInfo == EvolWrapperStatusInfo::BuildingAirPressure))
		{
			opModeStatus.clearEvolWrapperBuildingAirPressure();
			dispatchEvent(&opModeStatus);
			if (traceOn)
				Debug::trace("ServiceMode", msgDebug + "  opModeStatus.clearEvolWrapperBuildingAirPressure()");
		}

		if (!wrapperCalibrating && ((wrStateInfo == EvolWrapperStatusInfo::Homing) && statusMsg.getStatusInfo().getMainPower()) &&
			((lastStateInfo != EvolWrapperStatusInfo::Homing) || !(lastWrapperStatus.getStatusInfo().getMainPower())))
		{
			//	Set EvolWrapperHoming only if the state is Homing AND we have MainPower.
			opModeStatus.setEvolWrapperHoming();
			dispatchEvent(&opModeStatus);
		}
		else if ((wrStateInfo != EvolWrapperStatusInfo::Homing) && (lastStateInfo == EvolWrapperStatusInfo::Homing))
		{
			opModeStatus.clearEvolWrapperHoming();
			/*why are we sending this message to ourself???? -- ats
			dispatchEvent(&opModeStatus, this);
			*/
			dispatchEvent(&opModeStatus);
		}

		machineRunning = (wrStateInfo == EvolWrapperStatusInfo::Running);

		if (statusMsg.getStatusInfo().isMainPower() && opModeStatus.isWrapperPressStart())
		{
			if (traceOn)
				Debug::trace("ServiceMode", msgDebug + "Calling clearPressStart()");
			clearPressStart();
		}

		EvolWrapperErrorInfo currentErrorInfo = statusMsg.getCurrErrorInfo();
		EvolWrapperErrorInfo::EvolWrapperErrorSource errorSource = currentErrorInfo.getEvolWrapperErrorSource();
		EvolWrapperErrorInfo::EvolWrapperErrorSource currErrorSource = opModeStatus.getEvolWrapperErrorSource();
		EvolWrapperErrorInfo::EvolWrapperError error = currentErrorInfo.getEvolWrapperError(errorSource);
		EvolWrapperErrorInfo::EvolWrapperError currError = opModeStatus.getEvolWrapperError();
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("errorSource = ").append(EvolWrapperErrorInfo::getEvolWrapperErrorSourceText(errorSource));
			msgDebug2.append(",   currErrorSource = ").append(EvolWrapperErrorInfo::getEvolWrapperErrorSourceText(currErrorSource));
			Debug::trace("ServiceMode", msgDebug2);

			msgDebug2 = msgDebug;
			msgDebug2.append("error = ").append(EvolWrapperErrorInfo::getEvolWrapperErrorText(error));
			msgDebug2.append(",   currError = ").append(EvolWrapperErrorInfo::getEvolWrapperErrorText(currError));
			Debug::trace("ServiceMode", msgDebug2);
		}

		if ((errorSource != currErrorSource) || (error != currError))
		{
			if (!wrapperCalibrating || (wrapperCalibrating && (errorSource == EvolWrapperErrorInfo::GeneralWrapperSource)))
			{
				if (traceOn)
					Debug::trace("ServiceMode", msgDebug + "Error is different");

				// leave the error message up until the stop switch is cycled
				if (errorSource != EvolWrapperErrorInfo::NoWrapperErrorSource)
				{
					if (!wrapperCalibrating || (wrapperCalibrating && (errorSource == EvolWrapperErrorInfo::GeneralWrapperSource)))
						setNewEvolWrapperError(error, errorSource);
				}
			}
		}
	}
	if (Debug::isTracing("Clare"))
	{
		HobString msgDebug = "ServiceMode::handleWrapperEvolStatus()";
		msgDebug.append("\n           errorSource = ").append(Integer::toString(opModeStatus.getEvolWrapperErrorSource()));
		msgDebug.append("\n                 error = ").append(Integer::toString(opModeStatus.getEvolWrapperError()));
		msgDebug.append("\n                packId = ").append(Integer::toString(packId));
		msgDebug.append("\n        machineRunning = ").append(Bool::toString(machineRunning));
		msgDebug.append("\n      wrapStateMachine = ").append(Integer::toString(wrapStateMachine));
		msgDebug.append("\n isNewEvolWrapperError = ").append(Bool::toString(opModeStatus.isNewEvolWrapperError()));
		Debug::trace("Clare", msgDebug);
	}

	lastWrapperStatus = statusMsg;
}

void ServiceMode::handleEvolWrapperTest(const char *recvBuffer)
{
	bool traceOn = Debug::isTracing("ServiceMode");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "handleEvolWrapperTest()";
		Debug::trace("ServiceMode", msgDebug);
		msgDebug.append(":  ");
	}
	if (!recvBuffer)
	{
		Debug::trace("ServiceMode", msgDebug + "recvBuffer is NULL - Exiting!");
		return;
	}

	EvolWrapperTest testMsg(recvBuffer);
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		HobString tempName;

		msgDebug2.append("Test # = ").append(Integer::toString(testMsg.getTestCode()));

		testMsg.getTestName(testMsg.getTestCode(), tempName);
		msgDebug2.append(",  ").append(tempName);

		EvolWrapperStatusInfo::getWrapperStateName(wrStateInfo, tempName);
		msgDebug2.append("  wrapperState = ").append(tempName);
		msgDebug2.append("  mainPower = ").append(Bool::toString(lastWrapperStatus.getStatusInfo().isMainPower()));
		Debug::trace("ServiceMode", msgDebug2);
	}
	//SOF-1219 No need to press start when running camera scan
	if( testMsg.getTestCode() != EvolWrapperTest::CameraTest ) {
		if (!lastWrapperStatus.getStatusInfo().isMainPower())
		{
			if (traceOn)
				Debug::trace("ServiceMode", msgDebug + "Calling setPressStart()");
			setPressStart();
		}
		else if (lastWrapperStatus.getStatusInfo().isMainPower() && opModeStatus.isWrapperPressStart())
		{
			if (traceOn)
				Debug::trace("ServiceMode", msgDebug + "Calling clearPressStart()");
			clearPressStart();
		}
	}
}


void ServiceMode::productFailed()
{
	Debug::trace("ManualWeighMode", "productFailed()");

	sendProductComplete(pCurrentProductTransaction, false);
	pCurrentProductTransaction = NULL;

	unlockWeigher();
}
