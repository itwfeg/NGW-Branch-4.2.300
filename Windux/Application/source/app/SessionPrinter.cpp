/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */
#include "Session.h"
#include "MetSig.h"
#include "SessionMessage.h"
#include "HfcResponse.h"
#include "ResponseMessageManager.h"
#include "ProductManager.h"
#include "HfcProductChangeMsg.h"
#include "ConfigurationManager.h"

#include "DBProductsRM.h"
#include "DBProductStatisticsRM.h"

#include "SystemEvent.h"

#include "HfcPrinterStatus.h"
#include "HfcPrinterSystemStatus.h"
#include "HfcPrinterSensorsTracking.h"
#include "PrinterRequestSensors.h"
#include "PrinterCutterStatus.h"
#include "PrinterHeadType.h"
#include "HfcPrinterHeadType.h"
#include "PrinterHeadDotStatus.h"
#include "HfcPrinterSensors.h"
#include "HfcPrinterRequestCalibrate.h"
#include "PrinterSizing.h"
#include "HfcPrinterPrinterTypeTracking.h"
#include "HfcPrinterPrinterType.h"
#include "PrinterRequestPrinterType.h"
#include "PrinterNewPrinterType.h"
#include "HfcPrinterAdvanceLabelStock.h"
#include "HfcPrinterSetPrinters.h"
#include "HfcPrinterNewAddress.h"
#include "PrinterNewAddress.h"
#include "PrinterAdvanceLabelStock.h"
#include "HfcPrinterStockMetrics.h"
#include "HfcPrinterPrintTestLabel.h"
#include "HfcPrinterRequestPrintheadPower.h"
#include "HfcPrinterCutterStatus.h"
#include "HfcPrinterHeadDotStatus.h"
#include "HfcPrinterHeadDotWear.h"
#include "AllExternalPrinters.h"
#include "AllProductTextCfgs.h"
#include "ProductTextCfg.h"
#include "HfcPrinterRequestInquire.h"
#include "HfcPrinterRequestLabelSizing.h"
#include "HfcPrinterUseContinuousStock.h"
#include "HfcPrinterDefaultsComplete.h"
#include "HfcPrinterRequestStockMetrics.h"
#include "HfcPrinterTestCassette.h"
#include "HfcPrinterGapCalStatus.h"
#include "HfcPrinterTest.h"
#include "PrinterSystemRequestStatus.h"

#include "HfcILASpeed.h"
#include "HfcILARequestSpeed.h"
#include "ILASpeed.h"
#include "ILARequestSpeed.h"
#include "HfcLabelApplierTest.h"
#include "HfcLabelApplierSetup.h"
#include "HfcLabelApplierStatus.h"
#include "HfcLabelApplierSensorsTracking.h"
#include "HfcLabelApplierSensors.h"
#include "HfcLabelApplierInquire.h"
#include "HfcEvolLabelApplierPositionInfo.h"
#include "HfcCLASysStatus.h"	//Alex Zavala 8/22
#include "HfcCLASysInfo.h"		//Alex Zavala 9/9
#include "HfcCLASysInfoWrite.h"	//Alex Zavala 9/9
#include "HfcCLATestComplete.h"	//Alex Zavala 9/9
#include "HfcCLARemoveUpgradeFiles.h"//Alex Zavala 10/3
#include "VersionManagerEvent.h"//Alex Zavala 10/3

#include "PeripheralManager.h"
#include "HfcEvolWrapperConfig.h"
#include "HfcEvolLabelApplierInquire.h"
#include "WrapperEvent.h"

#include "Device.h"
#include "DeviceEvent.h"

#include "SessionManager.h"

#include "PythonScripter.h"
#include "GenericHfcObject.h"

#include "LabelApplierSystemStatus.h"

#include "SystemPrinters.h"

#include "HobTokenizerCSV.h"

#include "WriteSingleField.h"

#include "GenericHfcObject.h"
#include "FpmManager.h"

#include "Integer.h"
#include "Bool.h"
#include "UInteger.h"
#include "Long.h"
#include "Double.h"
#include "Hex.h"
#include "ULong.h"

#include <HobartCustomerHook.h>

#include "util/e_string/utilstring.h"


// Private method
void Session::handleLabelApplierMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	switch(msg->getDeviceType())
	{
		case LabelApplierSystem:
		{
			Debug::trace("Stupid", HobString("Session::handleLabelApplierMessage():  LabelApplierSystem, ") + HobString(Integer::toString(msg->getEventType())));
			switch(msg->getEventType())
			{
				case SysReqStatus:
				{
					Debug::trace("Session.LaSystem", "handling Label Applier System Request Status message");

					if (msg == (SessionMessage *)NULL)
						return;

					//	Register for the LabelApplierSystem SysStatus message
					registerEvent(LabelApplierSystem, SysStatus,false);

					//	Not much to do here with a Label Applier System Request Status message except pass it on.
					//	But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);

					send(msg, Messenger::DEST_RTC);
				}
					break;

				case SysStatus:
				{
					Debug::trace("Session.LaSystem", "handling Label Applier System Status message");

					/* Change the format indicator, and send the same message to the Browser. */
					msg->setFormatType(BrowserFormat);
					send(msg, Messenger::DEST_UI);
				}
				break;

				default:
					Debug::trace("Stupid", "handleLabelApplierMessage():  LabelApplierSystem - default case. Doing nothing.");
					break;
			}
		}
		break;

		case LabelApplier:
		{
			Debug::trace("Stupid", HobString("Session::handleLabelApplierMessage():  LabelApplier, ") + HobString(Integer::toString(msg->getEventType())));
			switch(msg->getEventType())
			{
				case LaReqInquire:
				{
					Debug::trace("Session", getName() + HobString(" - handling LaReqInquire"));
//					configurationManager->handleMessage(msg);
					HfcPeripheralList currentList;
					if (peripheralManager)
					{
						currentList = peripheralManager->getCurrentList();
						if (currentList.getLabelApplier() == EvolutionLabelApplier)
						{
							//if (traceOn)
							//	Debug::trace("Session.LabelApplier", msgDebug + HobString(" - We have an EvolutionLabelApplier"));

							//	The EvolutionLabelApplier configuration is part of the EvolutionWrapper configuration.
							HfcEvolWrapperConfig *wrConfig = configurationManager->getCurrentEvolWrapperConfig();
							if (wrConfig)
							{
								EvolLabelApplierConfiguration laConfig = wrConfig->getLabelApplierConfig();
								EvolWrapperGeneralConfig wrGenConfig = wrConfig->getWrapperConfig();
								bool hasSecondPrinter = (wrGenConfig.getHardwareAddOn() == SecondPrinterApplier) ? true : false;
								HfcEvolLabelApplierInquire hfcLaInquire(hasSecondPrinter, laConfig.getSideDischargeType());

								SessionMessage response(LabelApplier, LaEvolInquire);
								response.setFormatType(BrowserFormat);
								response.setHfcObject(&hfcLaInquire);

								send(&response, Messenger::DEST_UI);
							}
							else
							{
								//	We don't have a Wrapper config. Request one.
								setCurrentModeOp(OP_WAITFORLABELAPPLIERINQUIRE);		//	We'll send the LabelApplier config when we have a Wrapper config.

								SessionMessage myMessage(Wrapper, WrReqInquire);
								myMessage.setFormatType(InternalFormat);

								send(&myMessage, Messenger::DEST_RTC);
							}
						}

						if ((currentList.getLabelApplier() == CompactLabelApplier) ||
							(currentList.getLabelApplier() == CompactLabelApplier_EPSAC2) ||
							(currentList.getLabelApplier() == ILA))
						{
							if (Debug::isTracing("Session.LabelApplier"))
							{
								HobString myDevice;
								if (currentList.getLabelApplier() == CompactLabelApplier)
									myDevice = " CompactLabelApplier";
								else if (currentList.getLabelApplier() == CompactLabelApplier_EPSAC2)
									myDevice = "CompactLabelApplier_EPSAC2";
								else if (currentList.getLabelApplier() == ILA)
									myDevice = "n ILA";
								Debug::trace("Session.LabelApplier", "The LabelApplier is a" + myDevice);
							}
							//	Register for the LaInquire message
							registerEvent(LabelApplier, LaInquire, false);

							//	Not much to do here with a Label Applier Request Inquire message except pass it on.
							//	But first, change the format from Browser to Internal.
							msg->setFormatType(InternalFormat);

							send(msg, Messenger::DEST_RTC);
						}
					}
				}
				break;

				case LaInquire:
				{
					Debug::trace("Session", getName() + HobString(" - handling LaInquire"));

					configurationManager->handleMessage(msg);

					if (isInOperateMode() || isInService())
					{
						SessionMessage response(LabelApplier, LaInquire);
						response.setFormatType(BrowserFormat);
						response.setHfcObject(dynamic_cast<HfcObject *> (configurationManager->getCurrentLabelApplierConfig()));

						const char *msgdata = response.toString();
						int msglen = response.getSerializedLength();
						Debug::trace("ConfigurationManager.response",	HobString("label applier inquire response: ") +
																		HobString(Hex::toString(msgdata, msglen)));
						send(&response, Messenger::DEST_UI);
					}
				}
				break;

				case LaConfig:
				{
					Debug::trace("Session", getName() + HobString(" - handling LaConfig"));
					configurationManager->handleMessage(msg);
				}
				break;
				//Alex Zavala 8/22
				case LaSysStatus:
				{
					Debug::trace("Session.LabelApplierSysStatus", getName() + HobString(" - handling LaSysStatus"));

					HfcCLASysStatus *CLASysStatus = dynamic_cast<HfcCLASysStatus *> (msg->getHfcObject());

#if defined( __linux__ )
                    // send version info to hook
                    if (( Configuration::getInstance()->getCustomerHooksEnabled() ) && ( SessionManager::getInstance()->getPrimaryConsoleSession() ))
                	{
						PeripheralManager *pm( SessionManager::getInstance()->getPrimaryConsoleSession()->getPeripheralManager() );

                        if (( CLASysStatus ) && ( pm ))
                        {
                            std::string sw;
							sw += util_string::inttostring( CLASysStatus->getAppFlashMajor() );
							sw += ".";
							sw += util_string::inttostring( CLASysStatus->getAppFlashMinor() );

                            const std::string hw( "unknown" );

                            const HfcPeripheralList list( pm->getCurrentList() );

							// CLASystemStatus
							CLASysStatus->show();

#if defined ( __linux__ )
							Debug::trace("Session", "label applier versions: sw " + sw + ", hw " + hw);
							hobart_cust_hook_update_peripheral_version( LabelApplier, 0, list.getLabelApplier(), sw.c_str(), hw.c_str() );
#endif
						}
                    }
#endif

					SessionMessage sysStatusMsg(LabelApplier, LaSysStatus);
					sysStatusMsg.setFormatType(BrowserFormat);
					sysStatusMsg.setHfcObject(CLASysStatus);

					send(&sysStatusMsg, Messenger::DEST_UI);
				}
				break;
				//Added 9/9 Alex Zavala
				case LaReqSysInfo:
				{
					Debug::trace("Session", getName() + HobString(" - handling LaReqSysInfo"));
					msg->setFormatType(InternalFormat);

					send(msg, Messenger::DEST_RTC);
				}
				break;

				case LaSysInfo:
					{
						Debug::trace("Session", getName() + HobString(" - handling LaSysInfo"));
						HfcCLASysInfo *CLASysInfo = dynamic_cast<HfcCLASysInfo *> (msg->getHfcObject());

						SessionMessage sysInfoMsg(LabelApplier, LaSysInfo);
						sysInfoMsg.setFormatType(BrowserFormat);
						sysInfoMsg.setHfcObject(CLASysInfo);

						send(&sysInfoMsg, Messenger::DEST_UI);
					}
					break;
				//Added 9/9 Alex Zavala
				case LaSysInfoErase:
					{
						Debug::trace("Session", getName() + HobString(" - handling LaSysInfoErase"));
						msg->setFormatType(InternalFormat);

						send(msg, Messenger::DEST_RTC);
					}
					break;
				case LaSysInfoWrite:
					{
						Debug::trace("Session", getName() + HobString(" - handling LaSysInfoWrite"));

						HfcCLASysInfoWrite *CLASysInfoWrite = dynamic_cast<HfcCLASysInfoWrite *> (msg->getHfcObject());

						SessionMessage sysInfoWriteMsg(LabelApplier, LaSysInfoWrite);
						sysInfoWriteMsg.setFormatType(InternalFormat);
						sysInfoWriteMsg.setHfcObject(CLASysInfoWrite);

						send(&sysInfoWriteMsg, Messenger::DEST_RTC);
					}
					break;
				case LaTestComplete:
					{
						Debug::trace("Session", getName() + HobString(" - handling LaTestComplete"));

						HfcCLATestComplete *CLATestComplete = dynamic_cast<HfcCLATestComplete *> (msg->getHfcObject());

						SessionMessage testCompleteMsg(LabelApplier, LaTestComplete);
						testCompleteMsg.setFormatType(BrowserFormat);
						testCompleteMsg.setHfcObject(CLATestComplete);

						send(&testCompleteMsg, Messenger::DEST_UI);
					}
					break;
				case LaRemoveUpgradeFiles:
				{
					Debug::trace("Session", getName() + "LaRemoveUpgradeFiles");

					 HfcCLARemoveUpgradeFiles *laMsg = dynamic_cast<HfcCLARemoveUpgradeFiles *> (msg->getHfcObject());

					if (laMsg == 0)
					{
						Debug::trace("Session", HobString("CLARemoveUpgradeFiles:  laMsg is 0!"));
						return;
					}
					
					laMsg->show();
					if (laMsg->getState() == CLARemoveUpgradeFiles::BeginRemoval)
					{
						if (responseMessageManager)
							responseMessageManager->addMessage(ErrLabelApplier, LaErrBeginDeletingFirmware, HfcResponse::RESPONSESTYLE_OK);

						SessionMessage removeMsg(VersionManagerDev, RemoveCLASWComponents);
						removeMsg.setFormatType(InternalFormat);

						Debug::trace("Session", HobString("CLARemoveUpgradeFiles:  Sending RemoveCLASWComponents to VersionManagerDev"));

						send(&removeMsg, Messenger::DEST_RTC);
					}
					else if (laMsg->getState() == CLARemoveUpgradeFiles::RemovalComplete)
					{
						Debug::trace("Session", HobString("CLARemoveUpgradeFiles:  RemovalComplete."));
						if (responseMessageManager)
						{
							PAL_absclass::PAL_sleep(30);
							responseMessageManager->removeMessage(ErrLabelApplier, LaErrBeginDeletingFirmware, HfcResponse::RESPONSESTYLE_OK);
							responseMessageManager->addMessage(ErrLabelApplier, LaErrFirmwareDeleted, HfcResponse::RESPONSESTYLE_OK);
						}
					}		
				}
				break;
				case LaMode:
				{
					Debug::trace("Session.LabelApplierMode", getName() + HobString(" - handling LaMode"));

					//	Not much to do here with a Printer Sensors Tracking message except pass it on.
					//	But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);

					send(msg, Messenger::DEST_RTC);
				}
				break;

				case LaILAReqSpeed:
				{
//					// Browser requests this when on the ILA service screen, showing ILA ticks, and cycles per minute.
					Debug::trace("Session", getName() + HobString(" - handling LaILAReqSpeed - Doing nothing!"));
//
//					HfcILARequestSpeed *request = dynamic_cast<HfcILARequestSpeed *> (msg->getHfcObject());
//					if (request)
//					{
//						if (request->getSpeedState() == 0)  // turn off
//						{
//							// Register for the messages the ILA will send.
//							deregisterEvent(LabelApplier, LaILACycle);
//							deregisterEvent(LabelApplier, LaILASpeed);
//
//							if (currentILASpeed)
//							{
//								delete currentILASpeed;
//								currentILASpeed = (HfcILASpeed *)NULL;
//							}
//						}
//						else  // turn on
//						{
//							// Register for the messages the ILA will send.
//							registerEvent(LabelApplier, LaILACycle);
//							registerEvent(LabelApplier, LaILASpeed);
//
//							/* If I already have the object keeping track of the speed and cycle counts, and they tell
//							 * me again to start monitoring the speed and counts, then start over (create a new HfcILASpeed object).
//							 */
//							if (currentILASpeed)
//							{
//								delete currentILASpeed;
//								currentILASpeed = (HfcILASpeed *)NULL;
//							}
//
//							currentILASpeed = new HfcILASpeed();
//
//							msg->setFormatType(InternalFormat);
//							int bytesSent = send(msg, Messenger::DEST_RTC);
//							if (bytesSent < 0)
//							{
//								// todo - error handling?
//								Debug::trace("Session.LabelApplier", "LabelApplier, LaILAReqSpeed MESSAGE COULD NOT BE SENT TO THE RTC");
//							}
//						}
//					}
				}
				break;

				case LaILACycle:
				case LaILASpeed:
				{
					// Browser requests this when on the ILA service screen, showing ILA ticks, and cycles per minute.
					Debug::trace("Session", getName() + HobString(" - handling LaILACycle/LaILASpeed - Doing nothing!"));
//
//					if (currentILASpeed)
//					{
//						if (msg->getEventType() == LaILASpeed)
//						{
//							HfcILASpeed *iLASpeed = dynamic_cast<HfcILASpeed *> (msg->getHfcObject());
//							if (iLASpeed)
//								currentILASpeed->setSpeed(iLASpeed->getSpeed());
//						}
//						else
//							currentILASpeed->incrementCycleCount();
//
//						SessionMessage speedMsg(LabelApplier, LaILASpeed);
//						speedMsg.setFormatType(BrowserFormat);
//						speedMsg.setHfcObject(currentILASpeed);
//						int bytesSent = send(&speedMsg, Messenger::DEST_UI);
//						if (bytesSent < 0)
//						{
//							// todo - error handling?
//							Debug::trace("Session.LabelApplier", "LabelApplier, LaILASpeed MESSAGE COULD NOT BE SENT TO THE UI");
//						}
//					}
				}
				break;

				case LaTest:
				{
					HfcLabelApplierTest *laTest = dynamic_cast<HfcLabelApplierTest *> (msg->getHfcObject());

					Debug::trace("LabelApplierManager.Message", getName() + HobString(" - Session handling LaTest"));

					laTest->setFormatType(InternalFormat);
					send(laTest, Messenger::DEST_RTC);
				}
				break;

				case LaSetup:
				{
					Debug::trace("Session", getName() + HobString(" - handling LaSetup"));
					HfcLabelApplierSetup *laSetup = dynamic_cast<HfcLabelApplierSetup *> ( msg->getHfcObject() );

					msg->setFormatType(InternalFormat);
					send(msg, Messenger::DEST_RTC);
				}
				break;

				case LaStatus:
				{
					Debug::trace("Session", getName() + HobString(" - handling LaStatus"));
					HfcLabelApplierStatus *laStatus = dynamic_cast<HfcLabelApplierStatus *> (msg->getHfcObject());

					if ((laStatus != NULL) && (responseMessageManager != NULL))
					{
						responseMessageManager->manageStatus(laStatus);
					}
				}
				break;

				case LaSensorsTracking:
				{
					Debug::trace("Session.LabelApplierSensors", getName() + HobString(" - handling LaSensorsTracking"));

					HfcLabelApplierSensorsTracking *currentSensorsTracking = dynamic_cast<HfcLabelApplierSensorsTracking *> (msg->getHfcObject());

					if (currentSensorsTracking->isSensorsTracking() == true)
					{
						registerEvent(LabelApplier, LaSensors);
					}
					else
					{
						deregisterEvent(LabelApplier, LaSensors);
					}

					//	Not much to do here with a Printer Sensors Tracking message except pass it on.
					//	But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);

					send(msg, Messenger::DEST_RTC);
				}
				break;

				case LaSensors:
				{
					Debug::trace("Session.LabelApplierSensors", getName() + HobString(" - handling LaSensorsTracking"));

					HfcLabelApplierSensors *LASensors = dynamic_cast<HfcLabelApplierSensors *> (msg->getHfcObject());

					SessionMessage sensorsMsg(LabelApplier, LaSensors);
					sensorsMsg.setFormatType(BrowserFormat);
					sensorsMsg.setHfcObject(LASensors);

					send(&sensorsMsg, Messenger::DEST_UI);
				}
				break;

				case LaSndLabelPos:
				{
					HfcEvolLabelApplierPositionInfo *laPosInfo = dynamic_cast<HfcEvolLabelApplierPositionInfo *> (msg->getHfcObject());

					Debug::trace("LabelApplierManager.Message", getName() + HobString(" - Session handling LaSndLabelPos"));

					laPosInfo->setFormatType(InternalFormat);
					send(laPosInfo, Messenger::DEST_RTC);
				}
				break;

				default:
					Debug::trace("Stupid", "handleLabelApplierMessage():  LabelApplier - default case. Doing nothing.");
				break;
			}
		}
		break;

		default:
			Debug::trace("Stupid", "handleLabelApplierMessage():  Device = " + Integer::toString(msg->getDeviceType()) + " - Why am I here???");
		break;
	}
}

// Private method
void Session::handlePrinterMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	Debug::trace("SessionPrinter.handlePrinterMessage", "START handlePrinterMessage()");

	switch(msg->getDeviceType())
	{
		case PrinterSystem:
		{
			switch(msg->getEventType())
			{
				case SysReqStatus:
				{
					Debug::trace("Session.PrSystem", "handling Printer System Request Status message");

					if (msg == (SessionMessage *)NULL)
						return;

					//	Not much to do here with a Printer System Request Status message except pass it on.
					//	But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);

					send(msg, Messenger::DEST_RTC);
				}
				break;

				case SysStatus:
				{
					Debug::trace("Session.PrSystem", "handling Printer System Status message");

					HfcPrinterSystemStatus *prSysStatus = dynamic_cast<HfcPrinterSystemStatus *> (msg->getHfcObject());
					if (prSysStatus)
					{
						// send version info to hte apphook
						if (getCurrentModeOp() == OP_SENDWRAPPERDATATOAPPHOOK)
						{
							Debug::trace("SessionPrinter.PrinterSystemStatus", "Sending HfcPrinterSystemStatus object to hook.");

							sendWrapperDataToHook( prSysStatus->getPrinterType(), prSysStatus->getSoftwareVersion().getRevMajMinStr() );
							hasProcessedPrSystemMsg = true;

							Debug::trace("SessionPrinter.PrinterSystemStatus", "Finished sending HfcPrinterSystemStatus object to hook.");

							break;
						}

						// Need to save the FPC version to the Config table in the database.  We always update it with the latest version set in this message received.

						prSysStatus->show();

						Debug::trace("Session.PrSystem", "handling Printer System Status message, have a HfcPrinterSystemStatus object");
						if ( prSysStatus->getPrinterType() == FirstPrinter /* && prSysStatus->getHardwareVersion().getVersionType() == HobVersion::ApcPrinterHW   don't need this part for now; so for an Access console (wrapper/handwrap system), report the version of the first EPP printer */ )
						{
							string s, sWeigher;
							/*
							if( prSysStatus->getSoftwareVersion().getVersionType() == HobVersion::KpcPrinterSW)
							{
								s = prSysStatus->getSoftwareVersion().getRevMajMinBuildStr();
								sWeigher = "";
								Debug::trace("Session.PrSystem", string("handling Printer System Status message, prSysStatus->getSoftwareVersion().getVersionType() == HobVersion::KpcPrinterSW, prSysStatus->getSoftwareVersion().getRevMajMinBuildStr() is: ") + s);
								Debug::trace("Session.PrSystem", string("handling Printer System Status message, prSysStatus->getSoftwareVersion().getVersionType() == HobVersion::KpcPrinterSW, prSysStatus->getSoftwareWeigherVersion().getRevMajMinStr() is: ") + sWeigher);
							}
							else
							*/
							{
								s = prSysStatus->getSoftwareVersion().getRevMajMinEngStr();
								Debug::trace("Session.PrSystem", string("handling Printer System Status message, prSysStatus->getSoftwareVersion().getVersionType() != HobVersion::KpcPrinterSW, prSysStatus->getSoftwareVersion().getRevMajMinStr() is: ") + s);

								sWeigher = prSysStatus->getSoftwareWeigherVersion().getRevNTEPVersionStr();
								Debug::trace("Session.PrSystem", string("handling Printer System Status message, prSysStatus->getSoftwareVersion().getVersionType() != HobVersion::KpcPrinterSW, prSysStatus->getSoftwareWeigherVersion is: ") + sWeigher);

								string ss = prSysStatus->getSoftwareVersion().getRevisionString();
								Debug::trace("Session.PrSystem", string("handling Printer System Status message, prSysStatus->getSoftwareVersion().getVersionType() != HobVersion::KpcPrinterSW, prSysStatus->getSoftwareWeigherVersion().getRevisionString() is: ") + ss);

								string sss = prSysStatus->getSoftwareVersion().getRevMajMinBuildStr();
								Debug::trace("Session.PrSystem", string("handling Printer System Status message, prSysStatus->getSoftwareVersion().getVersionType() == HobVersion::KpcPrinterSW, prSysStatus->getSoftwareVersion().getRevMajMinBuildStr() is: ") + sss);
							}

							Configuration::getInstance()->setCfg(fieldOffsetCfgFPCVersion, s);
							Configuration::getInstance()->saveField(fieldOffsetCfgFPCVersion, this);

							Configuration::getInstance()->setCfg(fieldOffsetCfgFPCWeigherVersion, sWeigher);
							Configuration::getInstance()->saveField(fieldOffsetCfgFPCWeigherVersion, this);

							// The purpose of the following CfgSaved is to tell SessionManager to check if the
							// FPC version being set is different than what is recorded in the main board's serial flash.
							// If they're different, then we need to record it in the FPC audit trail.
							if ( Configuration::getInstance()->isCAT3AuditTrailLicensed() )
							{
								Debug::trace("cat3", "Dispatching CfgSaved since we saved the FPC version to the database");
								EvolMessage message(ConfigurationDev, CfgSaved);
								dispatchEvent(&message);
							}
						}

#if defined( __linux__ )
						// send version info to hook
	                    if (( Configuration::getInstance()->getCustomerHooksEnabled() ) && ( SessionManager::getInstance()->getPrimaryConsoleSession() ))
						{
							PeripheralManager *pm( SessionManager::getInstance()->getPrimaryConsoleSession()->getPeripheralManager() );

							if ( pm )
							{
								const std::string sw( prSysStatus->getSoftwareVersion().getRevMajMinStr() );
								const std::string hw( prSysStatus->getHardwareVersion().getRevMajMinStr() );

								const HfcPeripheralList list( pm->getCurrentList() );

								int kind = NoPrinter;

								if ( FirstPrinter == prSysStatus->getPrinterType() )
									kind = list.getFirstPrinter();
								else if ( SecondPrinter == prSysStatus->getPrinterType() )
									kind = list.getSecondPrinter();
								else if ( ThirdPrinter == prSysStatus->getPrinterType() )
									kind = list.getThirdPrinter();

								// PrinterSystemStatus
								prSysStatus->show();

#if defined ( __linux__ )
								Debug::trace("Session", "printer " + util_string::inttostring( prSysStatus->getPrinterType() ) + " versions: sw " + sw + ", hw " + hw);
								hobart_cust_hook_update_peripheral_version( PrinterSystem, prSysStatus->getPrinterType(), kind, sw.c_str(), hw.c_str() );
#endif
							}
						}
#endif
					}

					/* Change the format indicator, and send the same message to the Browser. */
					msg->setFormatType(BrowserFormat);
					send(msg, Messenger::DEST_UI);
				}
					break;

				case SysReqInquire:
				{
					Debug::trace("Session.PrSystem", " - handling Printer System Request Inquire message");

					if (msg == (SessionMessage *)NULL)
						return;

					registerEvent(PrinterSystem, SysInquire, false);

					//	Not much to do here with a Weigher System Request Inquire message except pass it on.
					//	But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);

					send(msg, Messenger::DEST_RTC);
				}
					break;

				case SysInquire:
				{
					Debug::trace("Session.PrSystem", " - handling Printer System Inquire message");
					configurationManager->handleMessage(msg);
				}
				break;
			}
		}
			break;

		case Printer:
		{
			Debug::trace("SessionPrinter.handlePrinterMessage", std::string("case PRINTER: ") + getName());
			Debug::trace("SessionPrinter.handlePrinterMessage", "Printer() Device = " + Integer::toString(msg->getDeviceType()) + "   Printer() Event = " + Integer::toString(msg->getEventType()));

			switch(msg->getEventType())
			{
				case PrStatus:
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("handlePrinterMessage case PrStatus: ") );
					Debug::trace("Session", getName() + HobString(" - handling PrStatus"));
					HfcPrinterStatus *prStatus = dynamic_cast<HfcPrinterStatus *> (msg->getHfcObject());
					if (prStatus)
					{
						Debug::trace("Session", getName() + " - handling PrStatus: " + ptrtostring(prStatus));
						if (currentModeOp == OP_MEDIASENSORCALIBRATION)
						{
							if ((prCalibStep == MSCalibrating) && (prStatus->isCalibrateCmdComplete() == true))
							{
								prCalibStep = MSComplete;
								responseMessageManager->removePrinterMessage(PrinterErrMSCalibrating, HfcResponse::RESPONSESTYLE_CANCEL, currentPrinter);
								responseMessageManager->addPrinterMessage(PrinterErrMSComplete, HfcResponse::RESPONSESTYLE_OK, currentPrinter);
							}
						}
						else if (currentModeOp == OP_LABELTAKENCALIBRATION)
						{
							if ((prCalibStep == LTCalibrating) && (prStatus->isCalibrateCmdComplete() == true))
							{
								prCalibStep = LTComplete;
								responseMessageManager->removePrinterMessage(PrinterErrLTCalibrating, HfcResponse::RESPONSESTYLE_CANCEL, currentPrinter);
								responseMessageManager->addPrinterMessage(PrinterErrLTComplete, HfcResponse::RESPONSESTYLE_OK, currentPrinter);
							}
						}

						// The currentPrinterStatus vector is used to hold the most recent printer status for each printer.
						// We pass in the current status and the previous one to the responseMessageManager so it can determine which statuses that it reports to the UI have changed.
						// A Fastrack requirement is to not continuously report a printer status if that status hasn't changed from the previous state.
						short whichPrinter = (short)prStatus->getPrinterType();

						// Need to notify the Fastrack UI that all printer status "errors" have cleared.
						// If we previously notified the UI of relevant printer status(es), and now there are none, then we need to clear the "Printer Statuses exist" icon in the status bar.
						// Note that we are only doing this if we are configured (internal config) to not repeatedly notify the UI of printer status messages
						if (Configuration::getInstance()->toBoolCfgI(CfgInternal::PRINTERSTATUSNONREPEATINGMSGSTOUI) && (int)currentPrinterStatus.size() >= whichPrinter)
						{
							Debug::trace("Session", getName() + " - printerStatusNonRepeatingMsgsToUI, whichPrinter:" + inttostring(whichPrinter) + "  currentPrinterStatus:" + ptrtostring(currentPrinterStatus[whichPrinter]));
							
							if (responseMessageManager)
							{
								Debug::trace("SessionPrinter.handlePrinterMessage", getName() + " - handlePrinterMessage() - no repeating msgs CALL manageStatus(prStatus, 0)");
								responseMessageManager->manageStatus(prStatus, currentPrinterStatus[whichPrinter]);
							}

							if (prStatus->notificationReasonExists())
							{
								Debug::trace("Session", getName() + " - printerStatusNonRepeatingMsgsToUI, at least one printer status is set, tell UI so it can change printer icon");
								SessionMessage noStatuses(Printer, PrStatusNotClear);
								noStatuses.setFormatType(BrowserFormat);
								send(&noStatuses, Messenger::DEST_UI);
							}
							// If there were previous printer statuses, in the currentPrinterStatus vector, but they are now all cleared,
							// then send a message to the UI so it can clear the PrinterStatus icon.
							else if (currentPrinterStatus[whichPrinter] && currentPrinterStatus[whichPrinter]->notificationReasonExists())
							{
								// If there were previous printer statuses, in the currentPrinterStatus vector, but they are now all cleared,
								// then send a message to the UI so it can clear the PrinterStatus icon.
								Debug::trace("Session", getName() + " - printerStatusNonRepeatingMsgsToUI, all statuses have cleared, letting UI know about this");
								Debug::trace("SessionPrinter.handlePrinterMessage", getName() + " - printerStatusNonRepeatingMsgsToUI, all statuses have cleared, letting UI know about this");

								SessionMessage noStatuses(Printer, PrStatusClear);
								noStatuses.setFormatType(BrowserFormat);
								send(&noStatuses, Messenger::DEST_UI);
							}

							if (currentPrinterStatus[whichPrinter])
								delete currentPrinterStatus[whichPrinter];
							currentPrinterStatus[whichPrinter] = prStatus;
							Debug::trace("Session", getName() + " - printerStatusNonRepeatingMsgsToUI, set this PrinterStatus as current:" + ptrtostring(currentPrinterStatus[whichPrinter]));

							// Take control of the message's object; don't let it get deleted when the message
							// is deleted, since we just stored it in the currentPrinterStatus vector.
							msg->setHfcObjectCreatedInternally(false);
						}
						else
						{
							// If we want it to behave as it does in the HLX, always display any printer status,
							// then clear out the currentStatus passed in so there isn't any printer status to check against.
							Debug::trace("SessionPrinter.handlePrinterMessage", getName() + " - handlePrinterMessage() - final else CALL manageStatus(prStatus, 0)");

							if (responseMessageManager)
								responseMessageManager->manageStatus(prStatus, 0);
						}
					}
					break;
				}

				case PrReqCurrentStatus:
				{
					// When we receive this from the Browser (Fastrack), then it wants me to send any Printer Status error messages,
					// one at a time, to the UI so it can renotify the user of any printer "errors".  I created this new
					// Printer event to allow the UI to request this.  The Session is maintaining the most recent printer status
					// for each printer attached (really only one for the Fastrack, like the HLX), but just in case we need this
					// for the FastrackAccess, or if they can hook up a second/third printer via USB (who knows, it's possible, right?)
					// then we will loop through all the most recent status for each printer, if we have a status for that printer.
					// Note that putting the most recent/saved printer status as the first parameter, and zeroing out the second,
					// will force the relevant/set status messages to be sent (one at a time), voila.
					// So here goes ...

					Debug::trace("Session", getName() + " - handling PrReqCurrentStatus");

					bool aNotificationReasonExists = false;

					if (currentPrinterStatus.size() >= MaxPrinterTypes)
						for (int i = 0; i < MaxPrinterTypes; i++)
							if (currentPrinterStatus[i] && responseMessageManager)
							{
								Debug::trace("Session", getName() + " - having responseMessageManager handle printer status, offset:" + inttostring(i));
								responseMessageManager->manageStatus(currentPrinterStatus[i]);
								if (currentPrinterStatus[i]->notificationReasonExists())
									aNotificationReasonExists = true;
							}

					if (!aNotificationReasonExists)
					{
						// If we want to let the UI know that no printer satauses exist, do that here.  I'm currently waiting to see if we need this message.
						// Is this going to be a new message (7,1xx for example)?, or I could reuse PrStatusClear (7,PrStatusClear).  TBD!
						;
					}
					break;
				}

				case PrClearCurrentStatus:
				{
					// If we receive this from the Browser (Fastrack), then it wants me to stop remembering any Printer Statuses
					// that I've kept.  Zero out the vector and delete any status that are there.
					// This will also be used when the operator changes modes, like from Operate to Supervisor.

					Debug::trace("Session", getName() + " - handling PrClearCurrentStatus");
					for (int i = 0; i < currentPrinterStatus.size(); i++)
						if (currentPrinterStatus[i])
						{
							delete currentPrinterStatus[i];
							currentPrinterStatus[i] = 0;
							Debug::trace("Session", getName() + " - deleted printer status, offset:" + inttostring(i));
						}

					// Let the UI know that it can "clear" the printer status icon, change the color back to indicate no printer statuses.
					Debug::trace("Session", getName() + " - printerStatusNonRepeatingMsgsToUI, all statuses have cleared, letting UI know about this");
					SessionMessage noStatuses(Printer, PrStatusClear);
					noStatuses.setFormatType(BrowserFormat);
					send(&noStatuses, Messenger::DEST_UI);

					break;
				}

				case PrReqInquire:
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", getName() + HobString(" - (7,22) handling PrReqInquire"));
//					configurationManager->handleMessage(msg);

					if (msg == (SessionMessage *)NULL)
						return;

					//	Register for the PrInquire message
                    registerEvent(Printer, PrInquire);
#if 1
					HfcPrinterRequestInquire *hpri = dynamic_cast<HfcPrinterRequestInquire *> (msg->getHfcObject());
					PrinterRequestInquire prInqReq(this, hpri->getPrinterType());
					dispatchEvent(&prInqReq);
#else
					//	Not much to do here with a Printer Request Inquire message except pass it on.
					//	But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);

					Debug::trace("Session.response", HobString("printer request inquire response: "));

					send(msg, Messenger::DEST_RTC);
#endif
				}
				break;

				case PrInquire:
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", getName() + HobString(" - (7,2) handling PrInquire"));
					configurationManager->handleMessage(msg);
				}
				break;

				case PrConfig:
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", getName() + HobString(" - (7,1) handling PrConfig"));
					configurationManager->handleMessage(msg);
				}
				break;

				case PrRestorePrinterSettings:
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", getName() + HobString(" - (7,155) handling PrRestorePrinterSettings"));
					configurationManager->handleMessage(msg);
				}
				break;

				case PrReqInfo:
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", getName() + HobString(" - (7,100) handling PrReqInfo"));
					configurationManager->handleMessage(msg);
				}
					break;

				case PrPrintheadPower:
				{
					Debug::trace("PrinterManager.PrinterSensors", getName() + HobString(" - (7,9) handling PrPrintheadPower"));

					msg->setFormatType(BrowserFormat);
					send(msg, Messenger::DEST_UI);
				}
				break;

				case PrReqPrintheadPower:
				{
					Debug::trace("PrinterManager.PrinterSensors", getName() + HobString(" - handling PrReqPrintheadPower"));

					HfcPrinterRequestPrintheadPower *prPower = dynamic_cast<HfcPrinterRequestPrintheadPower *> (msg->getHfcObject());

					if ((prPower != NULL) && (prPower->getPowerState() == PrinterOn))
					{
						Debug::trace("PrinterManager.PrinterSensors", getName() + HobString(" - dispatching PrinterRequestSensors"));
						PrinterRequestSensors prReqSensors(this, prPower->getPrinterType());
						dispatchEvent(&prReqSensors);
					}

					prPower->setFormatType(InternalFormat);
					send(prPower, Messenger::DEST_RTC);

					if (prPower->isPowerState() == true)
					{
						registerEvent(Printer, PrPrintheadPower, false );
					}
					else
					{
					 	deregisterEvent(Printer, PrPrintheadPower);
					}
				}
				break;

				case PrReqCalibrate:
				{
					Debug::trace( "Session.PrinterCalibration", "handling Printer Request Calibrate" );
					registerEvent( Printer, PrStatus );

					HfcPrinterRequestCalibrate *reqCalib( dynamic_cast<HfcPrinterRequestCalibrate*>( msg->getHfcObject() ) );

					switch ( reqCalib->getPrinterCalibrateType() )
					{
					case PrCalibrateMediaSensor:
						if ( OP_MEDIASENSORCALIBRATION != currentModeOp )
						{
							Debug::trace( "Session.PrinterCalibration", "Media Sensor Calibration" );

							currentPrinter = reqCalib->getPrinterType();

							if ( !initPrCalib( PrCalibrateMediaSensor ) )
								Debug::trace( "Session.PrinterCalibration", "Media Sensor Calibration FAILED INIT" );
							else
							{
								PrinterSizing prSizing( this, currentPrinter, false );
								dispatchEvent( &prSizing );
							}
						}
						break;

					case PrCalibrateLabelTaken:
						{
							Debug::trace( "Session.PrinterCalibration", "Label Taken Calibration" );
							currentPrinter = reqCalib->getPrinterType();

							if ( !initPrCalib( PrCalibrateLabelTaken ) )
								Debug::trace( "Session.PrinterCalibration", "Label Taken Calibration FAILED INIT" );
						}
						break;

					default:
						{
							Debug::trace( "Session.PrinterCalibration", "Input Error" );

							// Forward message on to the RTC.
							msg->setFormatType( InternalFormat );
							send( msg, Messenger::DEST_RTC );
						}
						break;
					}
				}

				break;

				case PrPrinterType:
				{
					Debug::trace("Session.PrinterType", "handling Printer Printer Type");

				 	HfcPrinterPrinterType *printerPrinterType = dynamic_cast<HfcPrinterPrinterType *> (msg->getHfcObject());

					if (printerPrinterType == (HfcPrinterPrinterType *)NULL)
						return;

					SessionMessage response(Printer, PrPrinterType);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(printerPrinterType);

					const char *msgdata = response.toString();
					int msglen = response.getSerializedLength();

					Debug::trace("Session.PrinterType",	HobString("printer printerType response: ") +	HobString(Hex::toString(msgdata, msglen)));

					send(&response, Messenger::DEST_UI);
				}
					break;

				case PrPrinterTypeTracking:
				{
					Debug::trace("Session.PrinterType", getName() + HobString(" - handling PrPrinterTypeTracking"));

					HfcPrinterPrinterTypeTracking *currentPrinterTypeTracking = dynamic_cast<HfcPrinterPrinterTypeTracking *> (msg->getHfcObject());

					if(currentPrinterTypeTracking->getPrinterTypeTrackingState() == PrinterOn)
					{
						Debug::trace("Session.PrinterType", getName() + HobString(" - Register for PrPrinterType"));
						registerEvent(Printer, PrPrinterType);
					}
					else
					{
						Debug::trace("Session.PrinterType", getName() + HobString(" - Deregister for PrPrinterType"));
						deregisterEvent(Printer, PrPrinterType);
					}
	
					//	Request this even if we are not listening for it. This way, the RTC will call
					//	resetPrinters()
					PrinterRequestPrinterType prReqPrinterType(this);
					dispatchEvent(&prReqPrinterType);
				}
					break;

				case PrSetPrinters:
				{
					Debug::trace("Session.PrinterType", getName() + HobString(" - handling PrSetPrinters"));

					HfcPrinterSetPrinters *setPrinters = dynamic_cast<HfcPrinterSetPrinters *> (msg->getHfcObject());

					if (setPrinters)
					{
							Debug::trace( "Session.PrinterType", getName() + HobString(" - Changing Printer 0 type to ") + HobString(inttostring(setPrinters->getPrinterType0())) + string(" usbNodeID:") + HobString(ULong::toString(setPrinters->getPrinterNodeId0())) );
							Debug::trace( "Session.PrinterType", getName() + HobString(" - Changing Printer 1 type to ") + HobString(inttostring(setPrinters->getPrinterType1())) + string(" usbNodeID:") + HobString(ULong::toString(setPrinters->getPrinterNodeId1())) );
							Debug::trace( "Session.PrinterType", getName() + HobString(" - Changing Printer 2 type to ") + HobString(inttostring(setPrinters->getPrinterType2())) + string(" usbNodeID:") + HobString(ULong::toString(setPrinters->getPrinterNodeId2())) );

					}

					if (setPrinters->validSetup() == true)
					{
						if (setPrinters->getPrinterType0() != MaxPrinterTypes)
						{
							Debug::trace("Session.PrinterType", getName() + HobString(" - Changing Printer 0 type to ") + HobString(Integer::toString(setPrinters->getPrinterType0())));

							PrinterNewPrinterType newPrType(NULL, FirstPrinter, setPrinters->getPrinterType0(),
															(USBD_NODE_ID) setPrinters->getPrinterNodeId0(), 0);
							dispatchEvent(&newPrType);
						}

						if (setPrinters->getPrinterType1() != MaxPrinterTypes)
						{
							Debug::trace("Session.PrinterType", getName() + HobString(" - Changing Printer 1 type to ") + HobString(Integer::toString(setPrinters->getPrinterType1())));

							PrinterNewPrinterType newPrType(NULL, SecondPrinter, setPrinters->getPrinterType1(),
															(USBD_NODE_ID) setPrinters->getPrinterNodeId1(), 0);
							dispatchEvent(&newPrType);
						}

						if (setPrinters->getPrinterType2() != MaxPrinterTypes)
						{
							Debug::trace("Session.PrinterType", getName() + HobString(" - Changing Printer 2 type to ") + HobString(Integer::toString(setPrinters->getPrinterType2())));

							PrinterNewPrinterType newPrType(NULL, ThirdPrinter, setPrinters->getPrinterType2(),
															(USBD_NODE_ID) setPrinters->getPrinterNodeId2(), 0);
							dispatchEvent(&newPrType);
						}
					}
					else
					{
						// put up error dialog message!! and request setup!!
						if (responseMessageManager)
						{
							responseMessageManager->addPrinterMessage(PrinterErrInvalidTypeSetup, HfcResponse::RESPONSESTYLE_OK, currentPrinter);
						}

						PrinterRequestPrinterType prReqPrinterType(this);
						dispatchEvent(&prReqPrinterType);
					}
				}
					break;

				case PrInfo:
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", getName() + HobString(" - (7,101) handling PrInfo"));
					configurationManager->handleMessage(msg);
				}
					break;

				case PrSaveInfo:
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", getName() + HobString(" - (7,102) handling PrSaveInfo"));
					configurationManager->handleMessage(msg);
				}
					break;

				case PrSensorsTracking:
				{
					Debug::trace("Session.PrinterSensors", getName() + HobString(" - handling PrSensorsTracking"));

					HfcPrinterSensorsTracking *currentSensorsTracking = dynamic_cast<HfcPrinterSensorsTracking *> (msg->getHfcObject());

					if(currentSensorsTracking->isSensorsTracking())
					{
						registerEvent(Printer, PrSensors);
						registerEvent(Printer, PrAdvLabelStock);
					}
					else
					{
						deregisterEvent(Printer, PrAdvLabelStock);
						deregisterEvent(Printer, PrSensors);
					}

					//	Not much to do here with a Printer Sensors Tracking message except pass it on.
					//	But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);

					send(msg, Messenger::DEST_RTC);
				}
				break;

				case PrSensors:
				{
					Debug::trace( "Session.PrinterSensors", "handling Printer Sensors message" );

				 	HfcPrinterSensors *printerSensors( dynamic_cast<HfcPrinterSensors*>( msg->getHfcObject() ) );

					if ( !printerSensors )
						return;

					const bool isHeadEngaged( printerSensors->isHeadEngaged() );

					if ( OP_MEDIASENSORCALIBRATION != currentModeOp )
						Debug::trace( "Session.PrinterSensors", "not in media sensor calibration" );
					else
					{
						Debug::trace( "Session.PrinterSensors",
						              "media sensor cal mode, prCalibStep = " + Integer::toString( prCalibStep ) +
									  " isHeadEngaged = " + Bool::toString( isHeadEngaged ) );

						if ( MSDisengage == prCalibStep )
						{
							if ( !isHeadEngaged )
							{
								prCalibStep = MSSituateLabelStock;
								responseMessageManager->removePrinterMessage( PrinterErrMSDisengage, HfcResponse::RESPONSESTYLE_CANCEL, currentPrinter );
								responseMessageManager->addPrinterMessage( PrinterErrMSSituateLabelStock, HfcResponse::RESPONSESTYLE_OKCANCEL, currentPrinter );
							}
						}
						else if (( MSEngage == prCalibStep ) || ( MSSituateLabelStock == prCalibStep ))
						{
							if ( isHeadEngaged )
							{
								if ( MSEngage == prCalibStep )
									responseMessageManager->removePrinterMessage( PrinterErrMSEngage, HfcResponse::RESPONSESTYLE_CANCEL, currentPrinter );
								else
									responseMessageManager->removePrinterMessage( PrinterErrMSSituateLabelStock, HfcResponse::RESPONSESTYLE_OKCANCEL, currentPrinter );

								if ( Configuration::getInstance()->isSelfServiceScale() )
								{
									prCalibStep = MSCalibrating;
									responseMessageManager->addPrinterMessage( PrinterErrMSCalibrating, HfcResponse::RESPONSESTYLE_CANCEL, currentPrinter );

									PrinterRequestCalibrate reqCalib( this, currentPrinter, PrCalibrateMediaSensor );
									dispatchEvent( &reqCalib );
								}
								else
								{
									prCalibStep = MSLabelStockTaut;
									responseMessageManager->addPrinterMessage( PrinterErrMSLabelStockTaut, HfcResponse::RESPONSESTYLE_OKCANCEL, currentPrinter );
								}
							}
						}
						else if ( MSLabelStockTaut == prCalibStep )
						{
							// head was disengaged when it should not have been... backup a few steps
							if ( !isHeadEngaged )
							{
								prCalibStep = MSSituateLabelStock;
								responseMessageManager->removePrinterMessage( PrinterErrMSLabelStockTaut, HfcResponse::RESPONSESTYLE_OKCANCEL, currentPrinter );
								responseMessageManager->addPrinterMessage( PrinterErrMSSituateLabelStock, HfcResponse::RESPONSESTYLE_OKCANCEL, currentPrinter );
							}
						}
					}

					SessionMessage response( Printer, PrSensors );
					response.setFormatType( BrowserFormat );
					response.setHfcObject( printerSensors );

					if ( Debug::isTracing( "Session.PrinterSensors" ) )
					{
						const char *msgdata = response.toString();
						int msglen = response.getSerializedLength();

						Debug::trace( "Session.PrinterSensors",	"printer sensors response: " + Hex::toString( msgdata, msglen ) );
					}

					send( &response, Messenger::DEST_UI );
				}
				break;

				case PrTestLabel:
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("case PrTestLabel: ") );
					HfcPrinterPrintTestLabel *testLabel = dynamic_cast<HfcPrinterPrintTestLabel *> (msg->getHfcObject());
					if (testLabel)
					{
						PrinterType type = testLabel->getPrinterType();

						Device *printerDevice = Device::findDevice(Printer, type, this);
						if (!printerDevice)		//	This device does not exist, so just ignore the test print request
						{
							// Send error message to browser indicating Device not found.
							addDialogMessage(ErrDeviceDev, DeviceErrRecordNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);
							return;
						}
	
						if ( printerDevice->isAHobartLabelPrinter() )
						{
							//	Change the format and pass the Printer Print Test Label message to the PrinterManager.
							msg->setFormatType(InternalFormat);
							send(msg, Messenger::DEST_RTC);
						}

						else if ( printerDevice->isANonHobartLabelPrinter() )
						{
							// Printing to the Zebra printer, use the python script.
							SystemPrinters::printToUSBLabelPrinter( "/etc/hobart/checkerboardLabel.bmp" );
						}

						else
						{
							// Probably is a network printer; can't test print to them.
							;
						}

						delete printerDevice;
						printerDevice = 0;
					}
				}
					break;

				case PrReqStockMetrics:
				{
					Debug::trace("Session", getName() + HobString(" - handling PrReqStockMetrics"));

					//	Not much to do here with a Printer Request Stock Metrics message except pass it on.
					//	But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);
					registerEvent(Printer, PrStockMetrics);
					send(msg, Messenger::DEST_RTC);
				}
					break;

				case PrStockMetrics:
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", getName() + HobString(" - (7,41) handling PrStockMetrics"));
				 	HfcPrinterStockMetrics *prStockMetricsMsg = dynamic_cast<HfcPrinterStockMetrics *> (msg->getHfcObject());

					if (prStockMetricsMsg != NULL)
					{
						// We are going to keep this message
						msg->setHfcObjectCreatedInternally(false);
						setPrinterStockMetrics(prStockMetricsMsg);

						if (isPrimarySession() && isInOperateMode() && productManager)
						{
							bool ltChanged = productManager->hasLabelTypeChanged(prStockMetricsMsg->getPrinterType());
							if (ltChanged && prStockMetricsMsg->getLabelLengthInLines() > 0)
							{
								productManager->setUpLabelTypes();

								// Send a Product Change message to the RTC so it knows the Imager needs to image the new label.
								SessionMessage pluChg(ProductDev, ProductChange);
								pluChg.setFormatType(InternalFormat);
								HfcProductChangeMsg hpcm(this, productManager->getCurrentProduct());
								pluChg.setHfcObject(&hpcm);
								send(&pluChg, Messenger::DEST_RTC);

								SessionMessage pluChg2(ProductDev, ProductChangeSmart);
								pluChg2.setFormatType(InternalFormat);
								HfcProductChangeMsg hpcm2(this, productManager);
								pluChg2.setHfcObject(&hpcm2);
								send(&pluChg2, Messenger::DEST_RTC);
							}
						}

						// Forward message on to the Browser.
						msg->setFormatType(BrowserFormat);
						send(msg, Messenger::DEST_UI);
					}
				}
				break;

				case PrReqAdvLabelStock:
				{
					Debug::trace("Session", getName() + HobString(" - handling PrAdvLabelStock"));
				 	HfcPrinterAdvanceLabelStock *advLabelStock = dynamic_cast<HfcPrinterAdvanceLabelStock *> (msg->getHfcObject());

					if (advLabelStock == (HfcPrinterAdvanceLabelStock *) NULL)
						return;

					advLabelStock->setFormatType(InternalFormat);
					advLabelStock->setNumSteps(40);
					send(advLabelStock, Messenger::DEST_RTC);
				}
				break;

				case PrCutterCut:
				{
					// Tell the Printer to spin up the cutter
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("handlePrinterMessage() case PrCutterCut: ") + getName() + HobString(" - handling PrCutterCut"));

					if (msg == (SessionMessage *)NULL)
						return;

					//	Not much to do here with a Printer Request to spin the cutter.
					//	But first, change the format from Browser to Internal.
					//msg->setFormatType(InternalFormat);
					//send(msg, Messenger::DEST_RTC);

					PrinterGeneric cCutter(PrCutterCut, this, FirstPrinter);

					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("handlePrinterMessage() case PrCutterCut translatePrinterEvent this: ") + HobString(Hex::toString((long long)(this))));

					dispatchEvent(&cCutter);

					break;
				}
				case PrEjectCut:
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("handlePrinterMessage() case PrEjectCut: ") + getName() + HobString(" - handling PrEjectCut"));
					
					PrinterGeneric ejectCut(PrEjectCut, this, FirstPrinter);
					dispatchEvent(&ejectCut);

					break;
				}
				case PrReqHeadType:
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("handlePrinterMessage() case PrReqHeadType: ") + getName() + HobString(" - handling PrReqHeadType"));

					PrinterGeneric cHead(PrHeadType, this, FirstPrinter);

					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("handlePrinterMessage() case PrReqHeadType translatePrinterEvent this: ") + HobString(Hex::toString((long long)(this))));

					dispatchEvent(&cHead);

					break;
				}

				case PrHeadType:
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("handlePrinterMessage() case PrHeadType: "));

					HfcPrinterHeadType *Head = dynamic_cast<HfcPrinterHeadType *> (msg->getHfcObject());

					if (Head == (HfcPrinterHeadType *)NULL)
					{
						Debug::trace("SessionPrinter.handlePrinterMessage", std::string("handlePrinterMessage() Head is NULL: "));
						return;
					}
					SessionMessage response(Printer, PrHeadType);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(Head);

					const char *msgdata = response.toString();
					int msglen = response.getSerializedLength();

					Debug::trace("SessionPrinter.handlePrinterMessage", HobString("PrHeadType response: "));// + HobString(Hex::toString(msgdata, msglen)));

					send(&response, Messenger::DEST_UI);

					break;
				}

				case PrReqCutterStatus:
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("handlePrinterMessage() case PrReqCutterStatus: ") + getName() + HobString(" - handling PrReqCutterStatus"));

					registerEvent(Printer, PrCutterStatus);
					PrinterGeneric cStatus(PrReqCutterStatus, this, FirstPrinter);

					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("handlePrinterMessage() case PrReqCutterStatus translatePrinterEvent this: ") + HobString(Hex::toString((long long)(this))));

					dispatchEvent(&cStatus);

					break;
				}

				case PrCutterStatus:
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("handlePrinterMessage() case PrCutterStatus getName: ") + getName());

					HfcPrinterCutterStatus *stat = dynamic_cast<HfcPrinterCutterStatus *> (msg->getHfcObject());

					if (stat == (HfcPrinterCutterStatus *)NULL)
						return;
										
					if( stat->getInstalled() ) {	// SOF-3857	
						if( !stat->getInterlock() ) {
							if( responseMessageManager ) {
							if( !FpmManager::getInstance()->isFreeStandingSelfService() ) { // SOF-5059																	
									responseMessageManager->addPrinterMessage( PrinterErrLabelCutterInterlockOpen, HfcResponse::RESPONSESTYLE_OK, currentPrinter );
								} else {
									//SOF-5059: Suppress cutter door open while in service or supervisor modes									
									if( ( getCurrentMode() != OpModeSupervisor ) && (  getCurrentMode() != OpModeService ) ) {
										if( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgIsCutterEnabled) ) //SOF-5060	
											responseMessageManager->addPrinterMessage( PrinterErrFSSLabelCutterInterlockOpen, HfcResponse::RESPONSESTYLE_OK, currentPrinter ); //SOF-5065
										else
											responseMessageManager->addPrinterMessage( PrinterErrFSSLabelDoorInterlockOpen, HfcResponse::RESPONSESTYLE_OK, currentPrinter );									
									}
								}
							}
						} else { 					// SOF-3895
							if( responseMessageManager ) {
								if( !FpmManager::getInstance()->isFreeStandingSelfService() ) { 
									responseMessageManager->removePrinterMessage( PrinterErrLabelCutterInterlockOpen, HfcResponse::RESPONSESTYLE_OK, currentPrinter );
								} else {
									if( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgIsCutterEnabled) ) //SOF-5060	
										responseMessageManager->removePrinterMessage( PrinterErrFSSLabelCutterInterlockOpen, HfcResponse::RESPONSESTYLE_OK, currentPrinter ); //SOF-5065
									else
										responseMessageManager->removePrinterMessage( PrinterErrFSSLabelDoorInterlockOpen, HfcResponse::RESPONSESTYLE_OK, currentPrinter );
								}
							}
						}
							
						//SOF-5029
						//cutterInstalled = true;
					}
					else
					{
						//SOF-5029
						//cutterInstalled = false;
					}
					
					SessionMessage response(Printer, PrCutterStatus);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(stat);

					const char *msgdata = response.toString();
					int msglen = response.getSerializedLength();

					Debug::trace("SessionPrinter.handlePrinterMessage", HobString("printerCutterStatus response: ") + HobString(Hex::toString(msgdata, msglen)));

					send(&response, Messenger::DEST_UI);

					break;
				}

				case PrCutterEnabled:
				{
					Debug::trace("SessionPrinter.Cutter", "handle Enabling the Cutter");
					//SOF-5029
					Configuration::getInstance()->setCutterEnabled(true);
					Configuration::getInstance()->saveField(fieldOffsetCfgIsCutterEnabled, this);
					if (msg == (SessionMessage *)NULL)
						return;
					
					//	Not much to do here with a Printer System Request to enable the cutter.
					//	But first, change the format from Browser to Internal.

					msg->setFormatType(InternalFormat);
					send(msg, Messenger::DEST_RTC);
					break;
				}

				case PrCutterDisabled:
				{
					Debug::trace("SessionPrinter.Cutter", "handle Disabling the Cutter");
					//SOF-5029
					Configuration::getInstance()->setCutterEnabled(false);
					Configuration::getInstance()->saveField(fieldOffsetCfgIsCutterEnabled, this);

					if (msg == (SessionMessage *)NULL)
						return;

					//	Not much to do here with a Printer System Request to disable the cutter.
					//	But first, change the format from Browser to Internal.
					
					msg->setFormatType(InternalFormat);
					send(msg, Messenger::DEST_RTC);
					break;
				}

				case PrCutterEnableSave: 			//SOF-5110
				{
					Debug::trace("SessionPrinter.Cutter", "Saving enable bit to db.");
					Configuration::getInstance()->saveField(fieldOffsetCfgIsCutterEnabled, this);
					break;
				}

				case PrResetTeach:
				{

					if (msg == (SessionMessage *)NULL)
						return;
					
					PrinterGeneric pmsg(PrResetTeach, this, FirstPrinter);					
					dispatchEvent( &pmsg );
					break;
				} 

				case PrReadAllUsbPrinterNames:
				{
					// The UI wants the names of the USB printers (Zebra label printers) in the system.
					// This isn't for any Hobart printer (internal or EPP).  This also isn't for any network printer drivers.
					Debug::trace("Session", getName() + " - Received PrReadAllUsbPrinterNames message.");

					string usbPrinters = SystemPrinters::getUsbPrinterNamesAsList();

					SessionMessage response(Printer, PrWriteAllUsbPrinterNames);
					response.setFormatType(BrowserFormat);
					GenericHfcObject gho(usbPrinters);
					response.setHfcObject(&gho);
					send(&response, Messenger::DEST_UI);
					break;
				}

				case PrReadAllNetworkPrinterNames:
				{
					// The UI wants the names of the Network printer drivers installed in the system.
					// This isn't for any Hobart printer (internal or EPP).  This also isn't for any Zebra printer.
					Debug::trace("Session", getName() + " - Received PrReadAllNetworkPrinterNames message.");

					string networkPrinters = SystemPrinters::getSystemPrinterNamesAsList();

					SessionMessage response(Printer, PrWriteAllNetworkPrinterNames);
					response.setFormatType(BrowserFormat);
					GenericHfcObject gho(networkPrinters);
					response.setHfcObject(&gho);
					send(&response, Messenger::DEST_UI);
					break;
				}

				case PrReadAllExternalPrinters:
				{
					// The UI wants all of the info for the configured external printers.
					// Write back 5 HobLists: extPrinterNumList, extPrinterNamesList, extPrinterTypeList, extPrinterUsbPortList, extPrinterIpAddressList
					Debug::trace("Session", getName() + " - Received PrReadAllExternalPrinters message.");

					SystemPrinters::findAllSystemPrinters();
	
					AllExternalPrinters extPrinters;
					extPrinters.findAll(this);

					SessionMessage response(Printer, PrWriteAllExternalPrinters);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(&extPrinters);
					send(&response, Messenger::DEST_UI);
					break;
				}

				case PrSaveExternalPrinter:
				{
					Configuration *config = Configuration::getInstance();
					if (!config)
						break;

					Debug::trace("Session", getName() + " - Received PrSaveExternalPrinter message.");

					Device *msgDev = dynamic_cast<Device *> (msg->getHfcObject());
					if (msgDev)
					{
						bool currentFound = false;
						bool savedFakePrinterManager = false;
						PrinterType whichPrinter = (PrinterType)msgDev->getDeviceSubType();

						if ( msgDev->getUserName().empty() )
							msgDev->setUserName( (whichPrinter == SecondPrinter) ? "EXT1" : "EXT2" );

						// If the user is setting the printer to none, or to a network report printer (network printer but not a Zebra),
						// then turn off the Configuration flags allowing to print plu labels to this printer.
						// Also, send the new value of this config offset back to the UI.
						if ( !msgDev->isARealPrinter() || msgDev->isANetworkReportPrinter() )
						{
							CfgFieldOffsets offset = ( whichPrinter == SecondPrinter ) ? fieldOffsetCfgVirtualPrintingOverride2ndPrinter : ( ( whichPrinter == ThirdPrinter ) ? fieldOffsetCfgVirtualPrintingOverride3rdPrinter : endCfgFieldOffsets );
							if ( offset != endCfgFieldOffsets )
							{
								bool currVal = config->toBoolCfg(offset);
								if (currVal)
								{
									config->updateField(offset, "0");
									config->saveField(offset, this);

									if ( getConfigurationManager() )
									{
										WriteSingleField configUpdate;
										configUpdate.setFieldCode(offset);
										getConfigurationManager()->handleCfgReadSingleFieldMessage(&configUpdate);
									}
								}
							}
						}

						Device *currentDev = Device::findDevice((int)Printer, whichPrinter, this);
						if (currentDev)
						{
							Debug::trace("Session", getName() + " - Device already in db - updating it.");
							currentFound = true;

							// Delete the printer from the system, as it is known by its name, in case any of its configuration is changing.
							// It will get re-added afterwards.
							PrinterType otherExtPrinter = (whichPrinter == SecondPrinter) ? ThirdPrinter : ( (whichPrinter == ThirdPrinter) ? SecondPrinter : MaxPrinterTypes );
							if ( otherExtPrinter != MaxPrinterTypes )
							{
								string otherName;
								string currentName = currentDev->getUserName();
								Device *otherExtDev = Device::findDevice((int)Printer, otherExtPrinter, this);
								if (otherExtDev)
								{
									otherName = otherExtDev->getUserName();
									delete otherExtDev;
									otherExtDev = 0;
								}
								if ( !currentName.empty() && currentName != otherName )
									SystemPrinters::removePrinterFromSystem( currentName );
							}

							// If the current printer is a non-Hobart label printer (Zebra printer), we will now get rid of it's fake printer manager
							// that was created by the Primary Session when that Session was registered.
							if ( currentDev->isANonHobartLabelPrinter() )
							{
								if ( msgDev->isANonHobartLabelPrinter() )
									savedFakePrinterManager = true;
								else
								{
									Debug::trace( "ProductManager.supportPrint", "will now destroy the fake printer manager for printer:" + Integer::toString(whichPrinter) );
									destroyFakePrinterManager(whichPrinter);
								}
							}

							if ( !msgDev->isARealPrinter() )
							{
								currentDev->deleteRecord();
								delete currentDev;
								currentDev = 0;
							}
							else
							{
								currentDev->updateDevice(msgDev);
								currentDev->saveRecord();
							}
						}
						else if ( msgDev->isARealPrinter() )
						{
							Debug::trace("Session", getName() + " - Device not already in db - creating it.");

							currentDev = msgDev;  // will need this after this if/else block
							msgDev->setDBAccess(this);
							msgDev->saveRecord();
						}

						if (currentDev)
						{
							// if the newly-setup printer is a network printer, let the OS know about it by it's name.
							if ( currentDev->isANetworkPrinter() )
							{
								Debug::trace("Session", getName() + " - Device is a network printer, adding it to the system.");
								SystemPrinters::addNetworkPrinterToSystem( currentDev->getUserName(), currentDev->getIPAddress(), currentDev->getSystemName() );
							}

							// If it's a Zebra printer connected via USB, setup to print to it.  Note, a networked Zebra printer is setup directly above.
							else if ( currentDev->isANonHobartLabelPrinter() )
							{
								Debug::trace("Session", getName() + " - Device is a USB zebra printer, adding it to the system.");
								SystemPrinters::addUSBZebraPrinterToSystem( currentDev->getUserName() );
							}

							if ( currentDev->isANonHobartLabelPrinter() && !savedFakePrinterManager )
							{
								Debug::trace("Session", "have a new non-Hobart label printer (Zebra printer), will now setup a fake printer manager ...");
								setupFakePrinterManager(whichPrinter, true);   // PJR-444 - SOF-5063 - Print wider labels on Zebra printer
							}

							if (currentFound)
							{
								delete currentDev;
								currentDev = 0;
							}

							//SessionMessage cfgMsg(ConfigurationDev, CfgSaved);
							//dispatchMessage(&cfgMsg);
							//if ( isPrimarySession() )
							//{
								// Because you can't send a message to yourself, just handle the message that was sent to everyone else.
							//	handleMessage(&cfgMsg);
							//}
						}

						// This call to validateDefaultRecords returns true if it changed any ProductTextCfg record because it was invalid.
						// If any were changed, send them all to the UI.
						if ( ProductTextCfg::validateDefaultRecords(this) )
						{
							SessionMessage msg(Printer, PrReadAllProductTextCfgs);
							handlePrinterMessage(&msg);
						}
					}
					break;
				}

				case PrReadAllProductTextCfgs:
				{
					// The UI wants all of the label numbers and printer selections for the different Product Text types that can be printed on the right-hand side screen.
					// Write back 3 HobLists: productTextTypesList, labelNumsList, whichPrinterList
					Debug::trace("Session", getName() + " - Received PrReadAllProductTextCfgs message.");

					AllProductTextCfgs productTextPrintParams;
					productTextPrintParams.findAll(this);

					SessionMessage response(Printer, PrWriteAllProductTextCfgs);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(&productTextPrintParams);
					send(&response, Messenger::DEST_UI);
					break;
				}

				case PrSaveProductTextCfg:
				{
					// The UI wants to save a ProductTextCfg to the db.
					Debug::trace("Session", getName() + " - Received PrSaveProductTextCfg message.");

					ProductTextCfg *msgptc = dynamic_cast<ProductTextCfg *> (msg->getHfcObject());
					if (msgptc)
					{
						ProductTextCfg *dbptc = ProductTextCfg::findProductTextCfg(msgptc->getTextType(), this);
						if (dbptc)
						{
							dbptc->updateProductTextCfg(msgptc);
							dbptc->saveRecord();
							delete dbptc;
							dbptc = 0;
						}
						else
						{
							msgptc->setDBAccess(this);
							msgptc->saveRecord();
						}

						//SessionMessage cfgMsg(ConfigurationDev, CfgSaved);
						//dispatchMessage(&cfgMsg);
						//if ( isPrimarySession() )
						//{
							// Because you can't send a message to yourself, just handle the message that was sent to everyone else.
						//	handleMessage(&cfgMsg);
						//}
					}
					break;
				}

				case PrPrintPdfDocument:
				case PrPrintWebDocument:
				{
					// Data from the UI:  QString::number(printerNumber) + "," + pdfFileNameToPrint
					Debug::trace("Session", getName() + " - Received PrPrintPdfDocument message.");

					GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
					if (gho)
					{
						int printerNum = 0;
						string filename;
						HobString msgData = gho->getData();
						HobTokenizerCSV tok(&msgData, BrowserFormatSeparator);
						HobString token;
						if (tok.next(&token) != -1)
							printerNum = Integer::valueOf(token);
						if (tok.next(&token) != -1)
							filename = token;

						Debug::trace("Session", getName() + " printerNum: " + Integer::toString(printerNum) + "  filename: " + filename);

						File theFile(filename);
						if ( !theFile.exists() )
						{
							Debug::trace("Session", getName() + " file doesn't exist: " + theFile.getFullPath());
							break;
						}

						HobString ext = theFile.getFileNameExtension();
						Debug::trace("Session", getName() + "  file extension: " + ext);

						if ( printerNum == (int)SecondPrinter || printerNum == (int)ThirdPrinter )
						{
							Device *dev = Device::findDevice((int)Printer, printerNum, this);
							if ( dev )
							{
								if ( dev->isANetworkReportPrinter() )
								{
									Debug::trace("Session", getName() + " - Device is a network printer, will now print file");

									util_string::toupper(ext);

									string pdfExtensions = Configuration::getInstance()->toStringCfgI(CfgInternal::EXTPRINTERSPDFEXTENSIONS);
									util_string::toupper( pdfExtensions );
									HobList pdfList;
									pdfList.fillListFromCSVString( pdfExtensions );

									string docExtensions = Configuration::getInstance()->toStringCfgI(CfgInternal::EXTPRINTERSDOCEXTENSIONS);
									util_string::toupper( docExtensions );
									HobList docList;
									docList.fillListFromCSVString( docExtensions );

									if ( pdfList.exists(ext) )
										SystemPrinters::printPdfFile( dev->getUserName(), dev->getSystemName(), filename );
									else if ( docList.exists(ext) )
										SystemPrinters::printDocFile( dev->getUserName(), dev->getSystemName(), filename );
									else
										SystemPrinters::printToNetworkReportPrinter( dev->getUserName(), filename );
								}
								delete dev;
								dev = 0;
							}
						}
					}
					break;
				}

				case PrReqLabelSizing:
				{
					// Tell the printer to size the labels.
					// This message was created when we were minimizing label loss when sizing, primarily for continuous stock.
					// Once we have continuous stock, we no longer size labels until the user requests to, 
					// (i.e. when they replace the continuous stock with die-cut label stock) using this message
					// which is received when the user presses the "Size Labels" button on the Operate-mode menu "Size Labels" panel.
					Debug::trace("Session.Printer", getName() + HobString(" - handling PrReqLabelSizing"));

					Configuration *config = Configuration::getInstance();
					if (!config)
						break;

					// SOF-1593 - user needs to opt-in for saving continuous label stock.
					// Requesting to size labels automatically turns off the configuration parameter "choseToUseContinuousStock"!!!
					config->setCfg(fieldOffsetCfgChoseToUseContinuousStock, "0");
					config->saveField(fieldOffsetCfgChoseToUseContinuousStock, this);

					HfcPrinterRequestLabelSizing *hprls = dynamic_cast<HfcPrinterRequestLabelSizing *> (msg->getHfcObject());
					if (hprls)
					{
						PrinterType printerNum = hprls->getPrinterType();
						Debug::trace("Session.Printer", getName() + HobString(" - handling PrReqLabelSizing - printerNum:") + inttostring(printerNum));

						if ( printerNum >= FirstPrinter && printerNum < MaxPrinterTypes )
						{
							// Pass this message along to the PrinterManager; it has the ability to resize the label.
							// After the printer sizes the labels, the Session will receive the PrinterStockMetrics for the printer,
							// it will then save the configuration table element for the associated printer's label length to what was sized.
							// Also, if we sized continuous stock, the PrinterManager will set the teach table up so that it doesn't size labels when the cassette is inserted.
							PrinterGeneric prReqLS(PrReqLabelSizing, this, printerNum);
							dispatchEvent(&prReqLS);
						}
					}

					if (config->toBoolCfgI(CfgInternal::SIMULATEPRINTING))
					{
						HfcPrinterRequestStockMetrics prReqStockMetricsMsg;
						SessionMessage request(Printer, PrReqStockMetrics);
						request.setFormatType(InternalFormat);
						request.setHfcObject(&prReqStockMetricsMsg);
						prReqStockMetricsMsg.setPrinterType(FirstPrinter);
						send(&request, Messenger::DEST_RTC, false);
					}

					break;
				}

				case PrUseContinuousStock:
				{
					// Tell the PrinterManager to set up for continuous stock - no longer size labels - regardless of the stock loaded, die-cut or continuous.
					HfcPrinterUseContinuousStock *hpucs = dynamic_cast<HfcPrinterUseContinuousStock *> (msg->getHfcObject());
					if (hpucs)
					{
						Configuration *config = Configuration::getInstance();
						if (!config)
							break;

						PrinterType printerNum = hpucs->getPrinterType();
						Debug::trace("Session.Printer", getName() + HobString(" - handling PrUseContinuousStock - printerNum:") + inttostring(printerNum));

						// SOF-1593 - user needs to opt-in for saving continuous label stock.
						// The next 2 lines need to be done before calling isConserveContinuousStockByNotSizingEnabled() directly below.
						config->setCfg(fieldOffsetCfgChoseToUseContinuousStock, "1");
						config->saveField(fieldOffsetCfgChoseToUseContinuousStock, this);

						// Can only set this for the primary printer for counter scale, not for an EPCP / EPP.
						if (config->isConserveContinuousStockByNotSizingEnabled() && printerNum == FirstPrinter )
						{
							Debug::trace("Session.Printer", getName() + " processing PrUseContinuousStock message");

							// Tell the PrinterManager to set the teach tables up for continuous stock, during which the PrinterManager
							// will dispatch the PrinterStockMetrics message which this Session will process and save the label length
							// to the database.
							PrinterGeneric prReqUCS(PrUseContinuousStock, this, printerNum);
							dispatchEvent(&prReqUCS);
						}

						if (config->toBoolCfgI(CfgInternal::SIMULATEPRINTING))
						{
							HfcPrinterRequestStockMetrics prReqStockMetricsMsg;
							SessionMessage request(Printer, PrReqStockMetrics);
							request.setFormatType(InternalFormat);
							request.setHfcObject(&prReqStockMetricsMsg);
							prReqStockMetricsMsg.setPrinterType(FirstPrinter);
							send(&request, Messenger::DEST_RTC, false);
						}
					}
					break;
				}

				case PrReqAreWeSizingLabels:
				{
					Debug::trace("Session.Printer", "Session received PrReqAreWeSizingLabels.");
					bool isSizing = Configuration::getInstance()->toBoolCfg(fieldOffsetCfgChoseToUseContinuousStock);
					GenericHfcObject sizing( Bool::toString(!isSizing) );
					SessionMessage msg(Printer, PrAreWeSizingLabels);
					msg.setFormatType(BrowserFormat);
					msg.setHfcObject(&sizing);
					send(&msg, Messenger::DEST_UI);
					break;
				}

				case PrResetFactoryDefaults:
				{
					Debug::trace("Session", "received: PrResetFactoryDefaults");
					PrinterGeneric msg( PrResetFactoryDefaults, this, FirstPrinter );

					registerEvent( Printer, PrResetFactoryDefaultsComplete );

					// SOF-1737 - Need to set the horizontal print position (which is in the database Configs table, not the printer eep) to the default.
					// Note:  We are only doing this for the FirstPrinter.
					Configuration *config = Configuration::getInstance();
					if (config)
					{
						//SOF-4001 as per Hillsboro changing from 0 to 5.
						config->setCfg(fieldOffsetCfgHorizLabelSkew1, "5");
						config->saveField(fieldOffsetCfgHorizLabelSkew1, this);
					}

					dispatchEvent( &msg );
					break;
				}
				case PrResetFactoryDefaultsComplete:
				{
					Debug::trace("Session", "received: PrResetFactoryDefaultsComplete");
					HfcPrinterDefaultsComplete *complete = dynamic_cast<HfcPrinterDefaultsComplete *> ( msg->getHfcObject() );
					if( !complete ) {

						Debug::trace("Session", "HfcWeigherDefaultsComplete is NULL!");
						return;
					}
					SessionMessage response(Printer, PrResetFactoryDefaultsComplete);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(complete);

					send(&response, Messenger::DEST_UI);

					break;
				}
				
				case PrReqHeadDotWear:
				{
                    // THIS STEP DOES NOT EVEN HAPPEN BECAUSE IT HAPPENS IN THE RTC LAYER
					// step 3, if there was a status that indicates wear, request all of the wear dots
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("case PrReqHeadDotWear Step 3: ") + getName());
					
					// This is never called because the message is ambushed
					// NOT CALLED - registerEvent(Printer, PrHeadDotWear);
					PrinterGeneric dotWear(PrReqHeadDotWear, this, FirstPrinter);

					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("handlePrinterMessage() case PrReqHeadDotWear this: ") + HobString(Hex::toString((long long)(this))));

					dispatchEvent(&dotWear);

					break;
				}

				case PrHeadDotWear:
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("START case PrHeadDotWear Step 4: ") + getName());

					// Step 4 Dot Wear: get back the Wear

                    // when thsi is invoked it creates a call to the *data constructor()
                    //HfcPrinterHeadDotWear *wear = dynamic_cast<HfcPrinterHeadDotWear *> (msg->getHfcObject());

                    //if (wear == (HfcPrinterHeadDotWear *)NULL)
                    //	return;

                    // Dont have to do this, but could
                    //SessionMessage response(Printer, PrHeadDotWear);        // 7,175
                    //response.setFormatType(BrowserFormat);
                    //response.setHfcObject(wear);

                    //const char *msgdata = response.toString();              // this does a serialize on the msg
                    //int msglen = response.getSerializedLength();

                    msg->setFormatType(BrowserFormat);

                    Debug::trace("SessionPrinter.handlePrinterMessage", HobString("Get ready to send it back") );

                    send(msg, Messenger::DEST_UI);

                    Debug::trace("SessionPrinter.handlePrinterMessage", std::string("DONE case PrHeadDotWear") );

					break;
				}

				case PrReqHeadDotStatus:
				{
					// Step 1 Dot Wear: Request the status
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("case PrReqHeadDotStatus Step 1: ") + getName() );

					registerEvent(Printer, PrHeadDotStatus);
					registerEvent(Printer, PrHeadDotWear);
					
					PrinterGeneric dotStatus(PrReqHeadDotStatus, this, FirstPrinter);
					
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("handlePrinterMessage() case PrReqHeadDotStatus this: ") + HobString(Hex::toString((long long)(this))));

					dispatchEvent(&dotStatus);

					break;
				}

				case PrHeadDotStatus:
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("case PrHeadDotStatus Step 2: ") + getName());

					// Step 2, get the head dot status back
					HfcPrinterHeadDotStatus *stat = dynamic_cast<HfcPrinterHeadDotStatus *> (msg->getHfcObject());

					if (stat == (HfcPrinterHeadDotStatus *)NULL)
						return;

					SessionMessage response(Printer, PrHeadDotStatus);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(stat);

					const char *msgdata = response.toString();
					int msglen = response.getSerializedLength();

					Debug::trace("SessionPrinter.handlePrinterMessage", HobString("HeadDotStatus response: ") + HobString(Hex::toString(msgdata, msglen)));

					send(&response, Messenger::DEST_UI);

					break;
				}
				case PrReqTestCassette: 
				{
					registerEvent( Printer, PrTestCassette );
					
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("PrReqTestCassette"));
					//SOF-3751 This test is only intended for the primary printer 
					PrinterGeneric genMsg(PrReqTestCassette, this, FirstPrinter);					
					dispatchEvent(&genMsg);					
					break;
				}
				case PrTestCassette: 
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("PrTestCassette"));
					deregisterEvent( Printer, PrTestCassette );
					
					HfcPrinterTestCassette *pTestMsg = dynamic_cast<HfcPrinterTestCassette *> ( msg->getHfcObject() );
					if( pTestMsg != NULL ) {
						SessionMessage uIMsg( Printer, PrTestCassette );
						uIMsg.setFormatType( BrowserFormat );
						uIMsg.setHfcObject( pTestMsg );

						send( &uIMsg, Messenger::DEST_UI );
					}
					break;
				}
				case PrRfidTestRequest:
				{
					PrinterGeneric request(PrRfidTestRequest, this, FirstPrinter);
					registerEvent(Printer, PrRfidTestPassed);
					registerEvent(Printer, PrRfidTestFailed);
					dispatchEvent(&request);
					break;
				}
				case PrRfidTestPassed:
				case PrRfidTestFailed:
				{
					const bool passed = msg->getEventType() == PrRfidTestPassed;
					SessionMessage response(Printer, passed ? PrRfidTestPassed : PrRfidTestFailed);
					response.setFormatType(BrowserFormat);
					deregisterEvent(Printer, PrRfidTestPassed);
					deregisterEvent(Printer, PrRfidTestFailed);
					send(&response, Messenger::DEST_UI);
					break;
				}
				case PrRfidCalRequest:
				{
					PrinterGeneric request(PrRfidCalRequest, this, FirstPrinter);
					registerEvent(Printer, PrRfidCalPassed);
					registerEvent(Printer, PrRfidCalFailed);
					dispatchEvent(&request);
					break;
				}
				case PrRfidCalPassed:
				case PrRfidCalFailed:
				{
					const bool passed = msg->getEventType() == PrRfidCalPassed;
					SessionMessage response(Printer, passed ? PrRfidCalPassed : PrRfidCalFailed);
					response.setFormatType(BrowserFormat);
					deregisterEvent(Printer, PrRfidCalPassed);
					deregisterEvent(Printer, PrRfidCalFailed);
					send(&response, Messenger::DEST_UI);
					break;
				}
				case PrRfidUpgradeRequest:
				{
					PrinterGeneric request(PrRfidUpgradeRequest, this, FirstPrinter);
					registerEvent(Printer, PrRfidUpgradePassed);
					registerEvent(Printer, PrRfidUpgradeFailed);
					dispatchEvent(&request);
					break;
				}
				case PrRfidUpgradePassed:
				case PrRfidUpgradeFailed:
				{
					const bool passed = msg->getEventType() == PrRfidUpgradePassed;
					SessionMessage response(Printer, passed ? PrRfidUpgradePassed : PrRfidUpgradeFailed);
					response.setFormatType(BrowserFormat);
					deregisterEvent(Printer, PrRfidUpgradePassed);
					deregisterEvent(Printer, PrRfidUpgradeFailed);
					send(&response, Messenger::DEST_UI);
					break;
				}
				case PrRfidQryRequest:
				{
					Debug::trace("SessionPrinter.rfid", std::string("PrRfidQryRequest"));

					static bool qryInit = false;

					if(qryInit == false)
					{
						Debug::trace("SessionPrinter.rfid", std::string("PrRfidQryRequest register"));

						qryInit = true;
						PrinterGeneric request(PrRfidQryRequest, this, FirstPrinter);
						registerEvent(Printer, PrRfidQryBusy);
						registerEvent(Printer, PrRfidQryPassed);
						registerEvent(Printer, PrRfidQryFailed);
						dispatchEvent(&request);
					}
					else
					{
						Debug::trace("SessionPrinter.rfid", std::string("PrRfidQryRequest deregister"));

						qryInit = false;
						PrinterGeneric request(PrRfidQryRequest, this, FirstPrinter);
						deregisterEvent(Printer, PrRfidQryBusy);
						deregisterEvent(Printer, PrRfidQryPassed);
						deregisterEvent(Printer, PrRfidQryFailed);
						dispatchEvent(&request);
					}
					
					break;
				}
				case PrRfidQryBusy:
				case PrRfidQryPassed:
				case PrRfidQryFailed:
				{
					Debug::trace("SessionPrinter.rfid", std::string("handlePrinterMsg"));

					const int passed = msg->getEventType();

					if(passed == PrRfidQryBusy)
					{
						Debug::trace("SessionPrinter.rfid", std::string("PrRfidQryBusy"));
						SessionMessage response(Printer, PrRfidQryBusy);
						response.setFormatType(BrowserFormat);
						send(&response, Messenger::DEST_UI);
					}
					else if(passed == PrRfidQryPassed)
					{
						Debug::trace("SessionPrinter.rfid", std::string("PrRfidQryPassed"));

						SessionMessage response(Printer, PrRfidQryPassed);
						response.setFormatType(BrowserFormat);
						send(&response, Messenger::DEST_UI);
					}
					else
					{
						Debug::trace("SessionPrinter.rfid", std::string("PrRfidQryFailed"));

						SessionMessage response(Printer, PrRfidQryFailed);
						response.setFormatType(BrowserFormat);
						send(&response, Messenger::DEST_UI);
					}

					break;
				}
				case PrCutterBitDflt:
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("PrCutterBitDflt"));
					PrinterGeneric msg( PrCutterBitDflt, this, FirstPrinter );
					dispatchEvent( &msg );					
					break;
				}
				case PrStartGapCalibration:		//SOF-4727
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("PrStartGapCalibration"));
					PrinterGeneric msg( PrStartGapCalibration, this, FirstPrinter );
					dispatchEvent( &msg );					
					break;
				}
				case PrGapCalStatus:			//SOF-4727
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("PrGapCalStatus"));
					HfcPrinterGapCalStatus *pStatusMsg = dynamic_cast<HfcPrinterGapCalStatus *> ( msg->getHfcObject() );					
					if( pStatusMsg != NULL ) {
						SessionMessage uIMsg( Printer, PrGapCalStatus );
						uIMsg.setFormatType( BrowserFormat );
						uIMsg.setHfcObject( pStatusMsg );

						send( &uIMsg, Messenger::DEST_UI );
					}					
					break;
				} 
				case PrGapCalNext:  
				{					
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("PrGapCalNext"));
					PrinterGeneric msg( PrGapCalNext, this, FirstPrinter );
					dispatchEvent( &msg );					
					break;
				}
				case PrTest: 
				{
					Debug::trace("SessionPrinter.handlePrinterMessage", std::string("PrTest"));
					
				 	HfcPrinterTest *pTest = dynamic_cast<HfcPrinterTest *> (msg->getHfcObject());
					if( pTest ) {
						pTest->setFormatType(InternalFormat);					
						send( pTest, Messenger::DEST_RTC );
					}		
					break;
				}            
				default:
					break;
			}
		}
		break;

		default:
		break;
	}
}

bool Session::initPrCalib( PrinterCalibrateType prType )
{
	if ( responseMessageManager )
	{
		if ( PrCalibrateMediaSensor == prType )
		{
			Debug::trace( "Session.PrinterCalibration", " start calibrating media sensor" );
			currentModeOp = OP_MEDIASENSORCALIBRATION;
			prCalibStep = MSDisengage;
			responseMessageManager->addPrinterMessage( PrinterErrMSDisengage, HfcResponse::RESPONSESTYLE_CANCEL, currentPrinter );

			// Make sure we get current state!
			PrinterRequestSensors reqSensors( this, currentPrinter );
			dispatchEvent( &reqSensors );

			return true;
		}

		else if ( PrCalibrateLabelTaken == prType )
		{
			currentModeOp = OP_LABELTAKENCALIBRATION;
			prCalibStep = LTClearSensor;
			responseMessageManager->addPrinterMessage( PrinterErrLTClearSensor, HfcResponse::RESPONSESTYLE_OKCANCEL, currentPrinter );

			return true;
		}
	}

	return false;
}

bool Session::handlePrCalib( SessionMessage *msg )
{
	// only handle removal messages
	if (( Error != msg->getDeviceType() ) || ( ErrRemoveAnyModalDialog != msg->getEventType() ))
		return false;

	HfcResponse *errResponse( dynamic_cast<HfcResponse*>( msg->getHfcObject() ) );

	// ensure we were able to cast object
	if ( !errResponse )
		return false;

	const long errorCode( errResponse->getCode() - (currentPrinter * 100) ); //	PrinterErrorOffset = 100

	// only interested in media sensor calibration
	if (( errorCode < PrinterErrMSDisengage ) || ( PrinterErrMSComplete < errorCode ))
		return false;

	bool opError( true );

	if ( HfcResponse::RESPONSEANSWER_OK == errResponse->getAnswer() )
	{
		switch ( errorCode )
		{
		case PrinterErrMSSituateLabelStock:
			if ( prCalibStep == MSSituateLabelStock )
			{
				Debug::trace( "Session.PrinterCalibration", "handlePrCalib: prCalibStep = MSEngage" );

				prCalibStep = MSEngage;
				responseMessageManager->addPrinterMessage( PrinterErrMSEngage, HfcResponse::RESPONSESTYLE_CANCEL, currentPrinter );

				// Make sure we get current state!
				PrinterRequestSensors reqSensors( this, currentPrinter );
				dispatchEvent( &reqSensors );

				opError = false;
			}

			break;

		case PrinterErrLTClearSensor:
			if ( prCalibStep == LTClearSensor )
			{
				Debug::trace( "Session.PrinterCalibration", "handlePrCalib: prCalibStep = LTCalibrating" );

				prCalibStep = LTCalibrating;
				responseMessageManager->addPrinterMessage( PrinterErrLTCalibrating, HfcResponse::RESPONSESTYLE_CANCEL, currentPrinter );

				PrinterRequestCalibrate reqCalib( this, currentPrinter, PrCalibrateLabelTaken );
				dispatchEvent( &reqCalib );

				opError = false;
			}

			break;

		case PrinterErrMSLabelStockTaut:
			if ( prCalibStep == MSLabelStockTaut )
			{
				Debug::trace( "Session.PrinterCalibration", "handlePrCalib: prCalibStep = MSCalibrating" );

				prCalibStep = MSCalibrating;
				responseMessageManager->addPrinterMessage( PrinterErrMSCalibrating, HfcResponse::RESPONSESTYLE_CANCEL, currentPrinter );

				PrinterRequestCalibrate reqCalib( this, currentPrinter, PrCalibrateMediaSensor );
				dispatchEvent( &reqCalib );

				opError = false;
			}

			break;

		default:
			Debug::trace( "Session.PrinterCalibration", "handlePrCalib: errorCode = " + Long::toString(errorCode) );
			break;

		}
	}

	if ( opError )
	{
		// SOF-4472
		// issue where we remove a dialog and then add a new dialog, then the cancel
		// message comes in for the first and removes the second...

		if ( MSSituateLabelStock == prCalibStep )
			if ( PrinterErrMSDisengage == errorCode )
			{
				Debug::trace( "Session.PrinterCalibration", "handlePrCalib: disengage dialog during situate label stock!" );
				return true;
			}

		if (( MSCalibrating == prCalibStep ) && ( Configuration::getInstance()->isSelfServiceScale() ))
			if (( PrinterErrMSEngage == errorCode ) || ( PrinterErrMSSituateLabelStock == errorCode ))
			{
				Debug::trace( "Session.PrinterCalibration", "handlePrCalib: engage/situate dialog during calibrating!" );
				return true;
			}

		if (( MSLabelStockTaut == prCalibStep ) && ( !Configuration::getInstance()->isSelfServiceScale() ))
			if (( PrinterErrMSEngage == errorCode ) || ( PrinterErrMSSituateLabelStock == errorCode ))
			{
				Debug::trace( "Session.PrinterCalibration", "handlePrCalib: engage/situate dialog during stock taut!" );
				return true;
			}

		// Finished Media Sensor Calibration
		currentModeOp = Session::OP_UNKNOWN;
		prCalibStep = PrNoCalib;

		PrinterSizing prSizing( this, currentPrinter, true );
		dispatchEvent( &prSizing );

		deregisterEvent( Printer, PrStatus );
	}

	return true;
}

void Session::sendPrinterSysReqStatusMsgs(Session *session)
{
	EvolMessage *printer1(new PrinterSystemRequestStatus(session, FirstPrinter));
	send(printer1, Messenger::DEST_RTC);

	EvolMessage *printer2(new PrinterSystemRequestStatus(session, SecondPrinter));
	send(printer2, Messenger::DEST_RTC);
}

void Session::sendWrapperDataToHook(int printerType, std::string softwareVersion)
{
	if (softwareVersion.empty())
	{
		Debug::trace("SessionPrinter.sendWrapperDataToHook", "Recieved an empty printer software version string. Aborting hook action.");
		return;
	}

	Debug::trace("SessionPrinter.sendWrapperDataToHook", "Processing hook actions for printer software version string (printer version data).");

	std::string primarySecondary;
	if (printerType == FirstPrinter)
	{
		primarySecondary = "primary";
	} else if (printerType == SecondPrinter)
	{
		primarySecondary = "secondary";
	}

	std::string data(std::string("SendWrapperData-").append(primarySecondary).append(std::string("PrinterVersion-")).append(softwareVersion));
	performHookAction(data.c_str());

	Debug::trace("SessionPrinter.sendWrapperDataToHook", "Finished processing hook actions for printer software version string (printer version data).");
}
