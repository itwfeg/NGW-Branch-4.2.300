/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Session.h"
#include "SessionMessage.h"
#include "ConfigurationManager.h"
#include "ResponseMessageManager.h"
#include "EvolWrapperTimingsManager.h"
#include "SessionManager.h"

#include "Peripheral.h"
#include "PeripheralManager.h"

#include "SystemEvent.h"
#include "HfcWrapperSensorsTracking.h"
#include "HfcWrapperSensors.h"
#include "HfcWrapperStatusTracking.h"
#include "HfcWrapperStatus.h"
#include "HfcEvolWrapperStatus.h"
#include "HfcWrapperTestTracking.h"
#include "HfcWrapperErrorTotals.h"
#include "HfcWrapperPackageTotals.h"
#include "HfcWrapperPackCodeNames.h"
#include "HfcWrapperPackCodeInfo.h"
#include "HfcWrapperCycleMode.h"
#include "HfcWrapperIntakePkgSize.h"
#include "HfcWrapperErrorHistoryTracking.h"
#include "HfcWrapperErrorHistory.h"
#include "HfcWrapperSettings.h"
#include "HfcEvolWrapperSettings.h"
#include "HfcWrapperControl.h"
#include "HfcEvolWrapperControl.h"
#include "HfcEvolWrapperConfig.h"
#include "HfcWrapperPackageHistory.h"
#include "HfcEvolWrapperTest.h"
#include "HfcEvolWrapperSensors.h"
#include "HfcEvolWrapperTestComplete.h"
#include "HfcEvolWrapperPackageDesc.h"
#include "HfcEvolWrapperPackageHistory.h"
#include "HfcEvolWrapperCamerasTracking.h"
#include "HfcEvolWrapperCameraScan.h"
#include "HfcEvolWrapperInquire.h"
#include "HfcEvolWrapperCameraHeightScan.h"
#include "HfcEvolWrapperRemoveUpgradeFiles.h"
#include "HfcEvolWrapperCameraCalibration.h"
#include "HfcEvolWrapperEventCapture.h"
#include "HfcEvolLabelApplierInquire.h"
#include "HfcEvolWrapperKnifeTimeHistory.h"
#include "EvolWrapperKnifeHistoryManager.h"
#include "EvolWrapperKnifeHistory.h"
#include "HfcEvolWrapperErrorHistory.h"
#include "EvolWrapperErrorHistory.h"
#include "EvolWrapperErrorHistoryManager.h"
#include "HfcEvolWrapperErrorHistoryList.h"
#include "HfcEvolWrapperPkgDetectorStatus.h"
#include "HfcEvolWrapperCycleCount.h"
#include "HfcEvolWrapperCameraThreshold.h"
#include "HfcEvolWrapperConfigLabelWidthUpdate.h"
#include "HfcEvolWrapperSystemInfo.h"
#include "HfcEvolWrapperSystemInfoWrite.h"
#include "HfcEvolWrapperMotorProfile.h"
#include "HfcEvolWrapperReqUIModel.h"
#include "HfcEvolWrapperUIModel.h"
#include "HfcNGWDimensionOverrides.h"
#include "HfcWrPPPDistance.h"
#include "HfcWrPPPCalibration.h"
#include "HfcWrPPPConfig.h"
#include "HfcWrPPPRequest.h"
#include "HfcWrPPPVersion.h"
#include "HfcWrPPPStatus.h"
#include "HfcNGWWrapperPause.h"
#include "HfcNGWWrapperResume.h"
#include "HfcWeigherStatus.h"

#include "WrapperCycle.h"
#include "WrapperMode.h"
#include "WrapperManager.h"
#include "LabelApplierEvent.h"
#include "VersionManagerEvent.h"
#include "TimerManager.h"
#include "HfcPrinterConfig.h"
#include "Printer.h"
#include "GenericHfcObject.h"

#include "FpmManager.h"

#ifndef returnToLater
#include "HfcWrapperSensors.h"
#endif

#include "HfcNGWUpgradeStatus.h"
#include "HfcNGWUpgradeInfo.h"
#include "HfcNGWStartUpgrade.h"
#include "HfcNGWUpgradeProgress.h"
#include "HfcNGWUpgradeFinished.h"
#include "HfcNGWUpgradeRequired.h"
#include "NGWUpgradeUIReady.h"
#include "SystemStartManager.h"
#include "RealTimeController.h"

#include "EvolWrapperSystemStatus.h"

#include <HobartCustomerHook.h>

#include "Integer.h"
#include "UInteger.h"
#include "Long.h"
#include "Double.h"
#include "Hex.h"
#include "ULong.h"
#include "Bool.h"
#include <cstring>

// Private method
void Session::handleWrapperMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	bool traceOn = Debug::isTracing("SessionWrapper");
	HobString msgDebug(getName());

	Debug::trace("SessionWrapper", "Session received a wrapper message.");

	bool wrapperIsTwoPrinter = FpmManager::getInstance()->isWrapperTwoPrinter();

	switch(msg->getDeviceType())
	{
		case WrapperSystem:
		{
			switch(msg->getEventType())
			{
				case SysReqStatus:
				{
					Debug::trace("Session.WrSystem", "handling Wrapper System Request Status message");

					if (msg == (SessionMessage *)NULL)
						return;

					//	Register for the WrapperSystem SysUsbWrStatus or SysStatus message
					if (peripheralManager && peripheralManager->getCurrentList().isEvolutionWrapperAttached())
						registerEvent(WrapperSystem, SysUsbWrStatus, false);
					else
						registerEvent(WrapperSystem, SysStatus,false );
					//  Not much to do here with a Wrapper System Request Status message except pass it on.
					//  But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);

					send(msg, Messenger::DEST_RTC);
				}
				break;

				case SysStatus:
				{
					Debug::trace("Session.WrSystem", "handling Wrapper System Status message");

					/* Change the format indicator, and send the same message to the Browser. */
					msg->setFormatType(BrowserFormat);
					send(msg, Messenger::DEST_UI);
				}
				break;
#if 0
				case SysReqInquire:
				{
					Debug::trace("Session.WgSystem", "handling Weigher System Request Inquire message");

					if (msg == (SessionMessage *)NULL)
						return;

					registerEvent(WeigherSystem, SysInquire, msgQId, Temporary);

					//  Not much to do here with a Weigher System Request Inquire message except pass it on.
					//  But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);

					send(msg, Messenger::DEST_RTC);
				}
				break;
#endif
				case SysInquire:
				{
					Debug::trace("Session.WgSystem", getName() + HobString(" - handling SysInquire"));
					configurationManager->handleMessage(msg);
				}
				break;

				case SysUsbWrStatus:
				{
					Debug::trace("Session.WrSystem", "handling SysUsbWrStatus message");

#if defined( __linux__ )
					// send version info to hook
                    if (( Configuration::getInstance()->getCustomerHooksEnabled() ) && ( SessionManager::getInstance()->getPrimaryConsoleSession() ))
					{
						EvolWrapperSystemStatus *status = dynamic_cast<EvolWrapperSystemStatus*>( msg->getHfcObject() );
						PeripheralManager *pm( SessionManager::getInstance()->getPrimaryConsoleSession()->getPeripheralManager() );

						if (( status ) && ( pm ))
						{
							for ( unsigned int i = 0; i < status->getNodeCount(); i++ )
							{
								const EvolWrapperNode& node( status->getNode( i ) );
								const EvolWrapperNode::EvolWrapperVersions versions( node.getAppFlash() );

								std::string sw;
								sw += util_string::inttostring( versions.getMajorRevision() );
								sw += ".";
								sw += util_string::inttostring( versions.getMinorRevision() );

								const std::string hw( "unknown" );

	                            const HfcPeripheralList list( pm->getCurrentList() );

								// EvolWrapperSystemStatus
								status->show();

#if defined ( __linux__ )
								Debug::trace("Session", "node " + util_string::inttostring( node.getNodeId() ) + " versions: sw " + sw + ", hw " + hw);
								hobart_cust_hook_update_peripheral_version( WrapperSystem, node.getNodeId(), list.getWrapper(), sw.c_str(), hw.c_str() );
#endif
							}
						}
					}
#endif

					/* Change the format indicator, and send the same message to the Browser. */
					msg->setFormatType(BrowserFormat);
					send(msg, Messenger::DEST_UI);
					break;
				}

				default:
					break;
			}
			break;
		}

		case Wrapper:
		{
			switch(msg->getEventType())
			{
				case WrReqInquire:
				{
					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrMessage: WrReqInquire"));

					if (msg == (SessionMessage *)NULL)
						return;

					//  Not much to do here with a WrReqInquire message except pass it on.
					//  But first, change the format from Browser to Internal.
					registerEvent(Wrapper, WrInquire,false );
					msg->setFormatType(InternalFormat);

					send(msg, Messenger::DEST_RTC);
					break;
				}

				case WrReqStatus:
//				case WrTest:
//				case WrLoad:
				{
					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrMessage: ") + HobString(Integer::toString(msg->getEventType())));

					if (msg == (SessionMessage *)NULL)
						return;

					//  Not much to do here with a Wrapper Control message except pass it on.
					//  But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);

					send(msg, Messenger::DEST_RTC);
					break;
				}

				case WrReqSettings:
				{
					if (traceOn)
						Debug::trace("SessionWrapper", msgDebug + HobString("WrReqSettings"));

					//  Not much to do here with a Wrapper Request Settings message except pass it on.
					//  But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);

					send(msg, Messenger::DEST_RTC);
					break;
				}

				case WrMode:
				{
					if ( traceOn )
						Debug::trace( "SessionWrapper", msgDebug + HobString("WrMode") );

					GenericHfcObject *obj( dynamic_cast<GenericHfcObject*>( msg->getHfcObject() ) );

					if ( obj )
						Debug::trace( "SessionWrapper", msgDebug + HobString("WrMode is ") + obj->getData() );
					else
					{
						Debug::trace( "SessionWrapper", msgDebug + HobString("WrMode bad cast") );
						break;
					}

					// SOF-1001
					if ( "0" == obj->getData() )
					{
						WrapperMode mode;
						mode.setWrOpMode( WrOperateMode );

						send( &mode, Messenger::DEST_RTC );
					}
					else if ( "1" == obj->getData() )
					{
						WrapperMode mode;
						mode.setWrOpMode( WrServiceMode );

						send( &mode, Messenger::DEST_RTC );
					}

					break;
				}

//				case WrSettings:
//				{
//					if (traceOn)
//					{
//						HobString msgDebug2 = msgDebug;
//						msgDebug2.append(" - handling WrSettings.");
//						Debug::trace("SessionWrapper", msgDebug2);
//					}
//					if (msg == (SessionMessage *)NULL)
//						return;
//
//					if (traceOn)
//					{
//						HfcWrapperSettings *wrSettings = dynamic_cast<HfcWrapperSettings *> (msg->getHfcObject());
//						Debug::trace("WrapperSettings", "HfcWrapperSettings->show() --- start");
//						wrSettings->show();
//						Debug::trace("WrapperSettings", "HfcWrapperSettings->show() --- end");
//
//						HobString msgDebug2 = msgDebug;
//						msgDebug2.append(" passing msg to configurationManager.");
//						Debug::trace("SessionWrapper", msgDebug2);
//					}
//					//  Save the current settings in configurationManager
//					configurationManager->handleMessage(msg);
//
//					if (wrapperSealerBeltOn || wrapperSealerBeltOff)
//					{
//						if (traceOn)
//						{
//							HobString msgDebug2 = msgDebug;
//							msgDebug2.append("handling a SealerBelt on/off command.");
//							Debug::trace("SessionWrapper", msgDebug2);
//						}
//						sendWrapperSealerBeltMessage();
//					}
//					else
//					{
//						//	  Just pass the message on to the UI
//						msg->setFormatType(BrowserFormat);
//
//						send(msg, Messenger::DEST_UI);
//					}
//					break;
//				}
//
//				case WrControl:
//				{
//					if (traceOn)
//					{
//						HobString msgDebug2 = msgDebug;
//						msgDebug2.append(" - handling WrControl.");
//						Debug::trace("SessionWrapper", msgDebug2);
//					}
//					if (!msg)
//						return;
//
//					if (traceOn)
//					{
//						HobString msgDebug2 = msgDebug;
//						msgDebug2.append(" passing msg to configurationManager.");
//						Debug::trace("SessionWrapper", msgDebug2);
//					}
//					configurationManager->handleMessage(msg);
//					break;
//				}

				case WrEvolInquire:
				{
					if (traceOn)
						Debug::trace("SessionWrapper", msgDebug + HobString(" - WrEvolInquire"));

					HfcEvolWrapperInquire *wrapperInquire = dynamic_cast<HfcEvolWrapperInquire *> (msg->getHfcObject());

					if (!wrapperInquire)
					{
						Debug::trace("SessionWrapper", msgDebug + "wrapperInquire is NULL! Exiting!!!");
						return;
					}
					if (Debug::isTracing("SessionWrapper.EvolWrapperInquire"))
					{
						Debug::trace("SessionWrapper.EvolWrapperInquire", msgDebug + "----  Show wrapperInquire ------  START");
						wrapperInquire->show();
						Debug::trace("SessionWrapper.EvolWrapperInquire", msgDebug + "----  Show wrapperInquire ------  END");
					}

					//check if hardware add on field is set if we are a two printer system
					if( wrapperIsTwoPrinter )
					{
						if( SecondPrinterApplier != wrapperInquire->getHardwareAddOn() ) {
							Debug::trace("SessionWrapper.EvolWrapperInquire","Hardware add on not set!");
							if (responseMessageManager)
								responseMessageManager->addMessage(ErrWrapper, 9742, HfcResponse::RESPONSESTYLE_OK);
						}	
					}

					//	Save the current values.
					msg->setHfcObjectCreatedInternally(false);
					configurationManager->setCurrentEvolWrapperConfig(wrapperInquire);
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append(" - handling WrEvolInquire.");
						Debug::trace("SessionWrapper", msgDebug2);
					}

					if (getCurrentModeOp() == OP_WAITFORLABELAPPLIERINQUIRE)
					{
						if (traceOn)
							Debug::trace("SessionWrapper", msgDebug + "WrEvolInquire, while modeOp = OP_WAITFORLABELAPPLIERINQUIRE");

						setCurrentModeOp(OP_UNKNOWN);
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
					}
					else
					{
						bool	controlChanged = false;
						HfcEvolWrapperControl *myWrControl = configurationManager->getCurrentEvolWrapperControl();
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("currentEvolWrapperControl = ").append(Hex::toString((long long) myWrControl));
							Debug::trace("SessionWrapper", msgDebug2);
						}
						if (!myWrControl)
						{
							if (traceOn)
							{
								HobString msgDebug2 = msgDebug;
								msgDebug2.append("Creating a new HfcEvolWrapperControl object from the HfcEvolWrapperInquire");
								Debug::trace("SessionWrapper", msgDebug2);
							}
							//	Copy the HfcEvolWrapperInquire into an HfcEvolWrapperControl object that we can keep track of.
							myWrControl = new HfcEvolWrapperControl(wrapperInquire);
							controlChanged = true;

							//	Now make this the current HfcEvolWrapperControl object.
							configurationManager->setCurrentEvolWrapperControl(myWrControl);
						}
						else
						{
							if (isInOperateMode())
							{
								//	This assumes that either the wrapper went down while the scale was still up,
								//	or we returned to Operate Mode. If the new control is different from the current control,
								//	we will send the current control to the wrapper so that things are consistent
								//	from the Operator's point of view.
								if (traceOn)
									Debug::trace("SessionWrapper", msgDebug + "Got a WrEvolInquire msg. Comparing it with the current WrapperControl.");

								HfcEvolWrapperControl newWrControl(wrapperInquire);
								if (!(newWrControl == *myWrControl))
								{
									SessionMessage response(Wrapper, WrEvolControl);
									response.setFormatType(InternalFormat);
									response.setHfcObject(myWrControl);

									if (traceOn)
									{
										HobString msgDebug2 = msgDebug;
												msgDebug2.append("Updating the Wrapper with the current HfcEvolWrapperControl object.");
										Debug::trace("SessionWrapper", msgDebug2);
									}
									send(&response, Messenger::DEST_RTC);
								}
							}
						}

						if (Debug::isTracing("SessionWrapper.EvolWrapperControl"))
						{
							HobString msgDebug2 = "Show wrapperControl --------------  ";
							Debug::trace("SessionWrapper.EvolWrapperControl", msgDebug + msgDebug2 + "START");
							myWrControl->show();
							Debug::trace("SessionWrapper.EvolWrapperControl", msgDebug + msgDebug2 + " END");
						}

						if (controlChanged)
						{
							if (traceOn)
							{
								HobString msgDebug2 = msgDebug;
								msgDebug2.append("Updating the RTC with the Control data");
								Debug::trace("SessionWrapper", msgDebug2);
							}
							//	Send this new Control message to the wrapper so we have a known state
							//	for the temporary settings, like the sealer belt temp.
							SessionMessage response(Wrapper, WrEvolControl);
							response.setFormatType(InternalFormat);
							response.setHfcObject(myWrControl);

							send(&response, Messenger::DEST_RTC);
						}

						// HTi-Access: Do not restrict this message to service mode!  It is needed
						// for label placement screen in EWS auto rotation cases.
						//if (isInService())
						{
							if (traceOn)
							{
								HobString msgDebug2 = msgDebug;
								msgDebug2.append(" - Sending EvolWrapper Inquire to Browser.");
								Debug::trace("SessionWrapper", msgDebug2);
							}

							HfcEvolWrapperConfig *wrapperConfig = configurationManager->getCurrentEvolWrapperConfig();
							SessionMessage response(Wrapper, WrEvolInquire);
							response.setFormatType(BrowserFormat);
							response.setHfcObject(wrapperConfig);

							send(&response, Messenger::DEST_UI);
						}
					}
					break;
				}

				case WrEvolConfig:
				{
					msgDebug += " - WrEvolConfig - ";
					if (traceOn)
					{
						Debug::trace("SessionWrapper", msgDebug);
						Debug::trace("SessionWrapper", msgDebug + "getting currentEvolWrapperConfig from configurationManager.");
					}

					HfcEvolWrapperConfig *currentWrapperConfig = configurationManager->getCurrentEvolWrapperConfig();
					if (!currentWrapperConfig)
					{
						if (traceOn)
							Debug::trace("SessionWrapper", msgDebug + "currentEvolWrapperConfig is 0. Exiting!");
						return;
					}

					if (traceOn)
					{
						Debug::trace("SessionWrapper", msgDebug + "currentEvolWrapperConfig is OK.");
						currentWrapperConfig->show();
						Debug::trace("SessionWrapper", msgDebug + "currentEvolWrapperConfig is OK. Checking newWrapperConfig.");
					}

					HfcEvolWrapperConfig *newWrapperConfig = dynamic_cast<HfcEvolWrapperConfig *> (msg->getHfcObject());
					if (!newWrapperConfig)
					{
						if (traceOn)
							Debug::trace("SessionWrapper", msgDebug + "newWrapperConfig is NULL!");
						return;
					}

					if ( wrapperIsTwoPrinter )
					{
						int isWrapperLR = FpmManager::getInstance()->isWrapperLeftToRight();
						LaSideDischarge newDischarge = newWrapperConfig->getLabelApplierConfig().getSideDischargeType();

						if (isWrapperLR == -1)
						{
							Debug::trace("SessionWrapper", "Could not get system style from FPM Manager");
						}
						else
						{
							if ( ( isWrapperLR == 1 && newDischarge == RightDischarge ) ||
								 ( isWrapperLR == 0 && newDischarge == LeftDischarge )
							)
							{
								Debug::trace( "SessionWrapper", "Config discharge matches model style" );
							}
							else
							{
								Debug::trace( "SessionWrapper", "Config discharge does not march model style, throwing dialog" );
								if ( responseMessageManager )
								{
									responseMessageManager->addMessage( ErrWrapper, 9743, HfcResponse::RESPONSESTYLE_OK );
								}
							}
						}
					}

					if (traceOn)
					{
						Debug::trace("SessionWrapper", msgDebug + "newWrapperConfig is OK.");
						newWrapperConfig->show();
						Debug::trace("SessionWrapper", msgDebug + "newWrapperConfig is OK. Calling currentWrapperConfig->update(newWrapperConfig).");
					}

					//add the label width for the first printer
					HfcPrinterConfig *pHfcPrinterConfig = configurationManager->getCurrentPrinterConfig(FirstPrinter);
					if (pHfcPrinterConfig)
					{
						if (traceOn)
							Debug::trace("SessionWrapper", msgDebug + "newWrapperConfig->setPriLabelWidth(): " + inttostring(pHfcPrinterConfig->getLabelWidth()));
						newWrapperConfig->setPriLabelWidth(pHfcPrinterConfig->getLabelWidth());
					}
					if ( newWrapperConfig->getHardwareAddOn() == SecondPrinterApplier )
					{
						if (traceOn)
							Debug::trace("SessionWrapper", msgDebug + "newWrapperConfig->getHardwareAddOn() == SecondPrinterApplier is true");

						//add the label width for second printer
						pHfcPrinterConfig = configurationManager->getCurrentPrinterConfig(SecondPrinter);
						if (pHfcPrinterConfig)
						{
							if (traceOn)
								Debug::trace("SessionWrapper", msgDebug + "newWrapperConfig->setSecLabelWidth(): " + inttostring(pHfcPrinterConfig->getLabelWidth()));
							newWrapperConfig->setSecLabelWidth(pHfcPrinterConfig->getLabelWidth());
						}
					}

					if (currentWrapperConfig->update(newWrapperConfig) == true)
					{
						if (traceOn)
							Debug::trace("SessionWrapper", msgDebug + "currentWrapperConfig has changed.");

						currentWrapperConfig->setEventType(WrEvolConfig);
						if (traceOn)
						{
							Debug::trace("SessionWrapper", msgDebug + "This is the updated currentWrapperConfig that ConfigurationManager keeps - will be sending it back to the wrapper:");
							currentWrapperConfig->show();
						}

						SessionMessage response(Wrapper, WrEvolConfig);
						response.setFormatType(InternalFormat);
						response.setHfcObject(currentWrapperConfig);

						if (traceOn)
						{
							const char *msgdata = response.serialize();
							int msglen = response.getSerializedLength();
							Debug::trace("SessionWrapper", msgDebug + "wrapper configuration response: " + Hex::toString(msgdata, msglen));
						}

						send(&response, Messenger::DEST_RTC);

						//	Since WrapperConfig changed, update the WrapperControl
						HfcEvolWrapperControl *currentWrapperControl = configurationManager->getCurrentEvolWrapperControl();
						if (!currentWrapperControl)
						{
							if (traceOn)
								Debug::trace("SessionWrapper", msgDebug + "currentWrapperControl is 0. Exiting!");
							return;
						}
						if (traceOn)
							Debug::trace("SessionWrapper", msgDebug + "currentWrapperControl is OK. Updating it with currentWrapperConfig.");

						if (currentWrapperControl->update(currentWrapperConfig) == true)
						{
							if (traceOn)
								Debug::trace("SessionWrapper", msgDebug + "sending updated EvolWrapper Control to the RTC.");

							SessionMessage response(Wrapper, WrEvolControl);
							response.setFormatType(InternalFormat);
							response.setHfcObject(currentWrapperControl);
							send(&response, Messenger::DEST_RTC);
						}
					}
					else
					{
						if (traceOn)
							Debug::trace("SessionWrapper", msgDebug + "currentWrapperConfig did not change.");
					}
					
					if (!currentWrapperConfig->wasSealerBeltTempValid())
					{
						currentWrapperConfig->setEventType(WrEvolInquire);

						SessionMessage response(Wrapper, WrEvolInquire);
						response.setFormatType(BrowserFormat);
						response.setHfcObject(currentWrapperConfig);
						send(&response, Messenger::DEST_UI);

						if (responseMessageManager)
							responseMessageManager->addMessage(ErrWrapper, 9711, HfcResponse::RESPONSESTYLE_OK);
					}
					
					break;
				}
				case WrEvolUpdateConfigLabelWidth:
				{
					HfcEvolWrapperConfigLabelWidthUpdate *newLabelWidthUpdates = dynamic_cast<HfcEvolWrapperConfigLabelWidthUpdate *> (msg->getHfcObject());
					if (newLabelWidthUpdates)
					{
						HfcEvolWrapperConfig *currentWrapperConfig = configurationManager->getCurrentEvolWrapperConfig();
						if(currentWrapperConfig)
						{
							if(currentWrapperConfig->update(newLabelWidthUpdates))
							{
								currentWrapperConfig->setEventType(WrEvolConfig);

								SessionMessage response(Wrapper, WrEvolConfig);
								response.setFormatType(InternalFormat);
								response.setHfcObject(currentWrapperConfig);
								
								send(&response, Messenger::DEST_RTC);
							}
						}
						else
						{
							Debug::trace("SessionWrapper", "Evol Wrapper Configuration is NULL");						
						}
					}
					else
					{
						Debug::trace("SessionWrapper", "Hfcobject: HfcEvolWrapperConfigLabelWidthUpdate NULL");						
					}
					break;
				}
				case WrEvolSettings:
				{
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append(" - handling WrEvolSettings.");
						Debug::trace("SessionWrapper", msgDebug2);
					}
					if (!msg)
						return;

					if (traceOn)
					{
						HfcEvolWrapperSettings *wrSettings = dynamic_cast<HfcEvolWrapperSettings *> (msg->getHfcObject());
						Debug::trace("EvolWrapperSettings", "HfcEvolWrapperSettings->show() --- start");
						wrSettings->show();
						Debug::trace("EvolWrapperSettings", "HfcEvolWrapperSettings->show() --- end");

						HobString msgDebug2 = msgDebug;
						msgDebug2.append(" passing msg to configurationManager.");
						Debug::trace("SessionWrapper", msgDebug2);
					}
					//  Save the current settings in configurationManager
					configurationManager->handleMessage(msg);

					if (wrapperSealerBeltOn || wrapperSealerBeltOff)
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("handling a SealerBelt on/off command.");
							Debug::trace("SessionWrapper", msgDebug2);
						}
						sendEvolWrapperSealerBeltMessage(msg);
					}

					//  Pass the message on to the UI
					msg->setFormatType(BrowserFormat);

					send(msg, Messenger::DEST_UI);
					break;
				}


				case WrEvolControl:
				{
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append(" - handling WrEvolControl.");
						Debug::trace("SessionWrapper", msgDebug2);
					}

					HfcEvolWrapperControl *currentWrControl = configurationManager->getCurrentEvolWrapperControl();
					if (!currentWrControl)
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("- currentWrControl is 0. Exiting!");
							Debug::trace("SessionWrapper", msgDebug2);
						}
								return;
					}
					HfcEvolWrapperControl *wrControl = dynamic_cast<HfcEvolWrapperControl *> (msg->getHfcObject());
					if (!wrControl)
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("- wrControl is 0. Exiting!");
							Debug::trace("SessionWrapper", msgDebug2);
						}
						return;
					}

					bool sbTempValid = true;
					bool sbTempChanged = false;
					bool controlChanged = currentWrControl->update(wrControl, sbTempValid, sbTempChanged);
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append(" passing msg to configurationManager.");
						Debug::trace("SessionWrapper", msgDebug2);
					}
					if (controlChanged)
					{
						if (Debug::isTracing("SessionWrapper.EvolWrapperControl"))
						{
							Debug::trace("SessionWrapper.EvolWrapperControl", "-----88888  currentWrControl  88888-----");
							currentWrControl->show();
							Debug::trace("SessionWrapper.EvolWrapperControl", "-----88888		 end		88888-----");
						}

						SessionMessage response(Wrapper, WrEvolControl);
						response.setFormatType(InternalFormat);
						response.setHfcObject(currentWrControl);

						if (traceOn)
						{
							const char *msgdata = response.serialize();
							int msglen = response.getSerializedLength();

							HobString msgDebug2 = msgDebug;
							msgDebug2.append("WrEvolControl response to DEST_RTC: ").append(Hex::toString(msgdata, msglen));
							Debug::trace("SessionWrapper", msgDebug2);
						}

						send(&response, Messenger::DEST_RTC);
					}

					HfcEvolWrapperConfig *currentWrConfig = configurationManager->getCurrentEvolWrapperConfig();
					if (currentWrConfig)
					{
						//	If they changed the Sealer Belt temp on the Wrapper settings page, set the new temp to the default Wrapper config temp.
						if (sbTempChanged && currentWrControl && isInOperateMode())
							currentWrConfig->getHeaterConfig().getSealerHeaterConfig().setDefaultTempSetting(currentWrControl->getSealerBeltTempSetting());

						bool rollWidthChanged = currentWrConfig->update(wrControl);
						if (rollWidthChanged || sbTempChanged)
						{
							if (traceOn)
							{
								if (rollWidthChanged)
									Debug::trace("SessionWrapper", msgDebug + " - Roll width's have changed, sending the updated Config.");
								else if (sbTempChanged)
									Debug::trace("SessionWrapper", msgDebug + " - Sealer belt temp changed, sending the updated Config.");
							}

							currentWrConfig->setEventType(WrEvolConfig);

							SessionMessage response(Wrapper, WrEvolConfig);
							response.setFormatType(InternalFormat);
							response.setHfcObject(currentWrConfig);

							if (traceOn)
							{
								const char *msgdata = response.serialize();
								int msglen = response.getSerializedLength();

								HobString msgDebug2 = msgDebug;
								msgDebug2.append("WrEvolConfig response to DEST_RTC: ").append(Hex::toString(msgdata, msglen));
								Debug::trace("SessionWrapper", msgDebug2);
							}

							send(&response, Messenger::DEST_RTC);
						}
					}
					if (!sbTempValid)
					{
						HobString msgDebug2;
						HfcEvolWrapperSettings wrSettings(currentWrConfig);
						if (traceOn)
						{
							msgDebug2 = msgDebug;
							msgDebug2.append("Sealer Belt temp is invalid!   ");
							Debug::trace("SessionWrapper", msgDebug2 + "- SHOW wrSettings from currentWrConfig");
							wrSettings.show();
							Debug::trace("SessionWrapper", msgDebug2 + "- DONE wrSettings from currentWrConfig");

							Debug::trace("SessionWrapper", msgDebug2 + "- SHOW currentWrControl");
							currentWrControl->show();
							Debug::trace("SessionWrapper", msgDebug2 + "- DONE currentWrControl");
						}

						wrSettings.update(currentWrControl);

						if (traceOn)
						{
							Debug::trace("SessionWrapper", msgDebug2 + "- SHOW wrSettings after update from currentWrControl");
							wrSettings.show();
							Debug::trace("SessionWrapper", msgDebug2 + "- DONE wrSettings after update from currentWrControl");
						}

						SessionMessage response(Wrapper, WrEvolSettings);
						response.setFormatType(BrowserFormat);
						response.setHfcObject(&wrSettings);

						send(&response, Messenger::DEST_UI);

						if (responseMessageManager)
							responseMessageManager->addMessage(ErrWrapper, 9711, HfcResponse::RESPONSESTYLE_OK);
					}
					break;
				}

				case WrEvolLoad:
				{
					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrEvolLoad"));
					//  Not much to do here with a Wrapper Evol Load message except pass it on.
					//  But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);

					send(msg, Messenger::DEST_RTC);
					break;
				}

				case WrEvolTest:
				{
					if ( traceOn )
						Debug::trace("SessionWrapper", msgDebug + HobString("WrEvolTest"));

					registerEvent( Wrapper, WrEvolTestComplete );

					//	Not much to do here with a Wrapper Control message except pass it on.
					//	But first, change the format from Browser to Internal.
					msg->setFormatType( InternalFormat );
					send( msg, Messenger::DEST_RTC );

					break;
				}

				case WrEvolTestComplete:
				{
					if ( traceOn )
						Debug::trace("SessionWrapper", msgDebug + HobString("WrEvolTestComplete"));

					//	What do we do with this message?
					HfcEvolWrapperTestComplete *testComplete( dynamic_cast<HfcEvolWrapperTestComplete*>( msg->getHfcObject() ) );

					if ( !testComplete )
					{
						if ( traceOn )
							Debug::trace("SessionWrapper", msgDebug + HobString("WrEvolTestComplete:  HfcEvolWrapperTestComplete msg is 0!!!"));

						return;
					}

					msg->setFormatType( BrowserFormat );
					send( msg, Messenger::DEST_UI );

					break;
				}

				case WrProductLoaded:
				{
					if (traceOn)
						Debug::trace("SessionWrapper", msgDebug + HobString(" - handling WrProductLoaded - Product Reload Wrapper Controls."));
					if (isPrimarySession() && configurationManager)
						configurationManager->handleMessage(msg);

					SessionMessage request(Wrapper, WrReqSettings);
					request.setFormatType(InternalFormat);

					send(&request, Messenger::DEST_RTC);
					break;
				}

//				case WrSensorsTracking:
//				{
//					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrSensorsTracking"));
//
//					HfcWrapperSensorsTracking *currentSensorsTracking = dynamic_cast<HfcWrapperSensorsTracking *> (msg->getHfcObject());
//
//					if(currentSensorsTracking)
//					{
//						if(currentSensorsTracking->isSensorsTracking())
//						{
//							Debug::trace("SessionWrapper", getName() + HobString(" - SensorsTracking ON"));
//							registerEvent(Wrapper, WrSensors);
//						}
//						else
//						{
//							Debug::trace("SessionWrapper", getName() + HobString(" - SensorsTracking OFF"));
//							deregisterEvent(Wrapper, WrSensors);
//						}
//					}
//
//					//  Not much to do here with a Wrapper Sensors Tracking message except pass it on.
//					//  But first, change the format from Browser to Internal.
//					msg->setFormatType(InternalFormat);
//
//					send(msg, Messenger::DEST_RTC);
//					break;
//				}
//
//				case WrSensors:
//				{
//					Debug::trace("SessionWrapper", "handling Wrapper Sensors message");
//
//					HfcWrapperSensors *wrapperSensors = dynamic_cast<HfcWrapperSensors *> (msg->getHfcObject());
//
//					if (wrapperSensors == (HfcWrapperSensors *) 0)
//						return;
//
//					if (getCurrentModeOp() == OP_WRAPPERTESTS)
//					{
//						SessionMessage response(Wrapper, WrTestSense);
//						response.setFormatType(WrapperTestSensorsFormat);
//						response.setHfcObject(wrapperSensors);
//
//						send(&response, Messenger::DEST_UI);
//					}
//					else
//					{
//						SessionMessage response(Wrapper, WrSensors);
//						response.setFormatType(BrowserFormat);
//						response.setHfcObject(wrapperSensors);
//
//						send(&response, Messenger::DEST_UI);
//					}
//					break;
//				}

				case WrEvolSensors:
				{
					if (traceOn)
						Debug::trace("SessionWrapper", msgDebug + HobString("WrEvolSensors"));

					 HfcEvolWrapperSensors *wrapperSensors = dynamic_cast<HfcEvolWrapperSensors *> (msg->getHfcObject());

					if (wrapperSensors == 0)
						return;

					SessionMessage response(Wrapper, WrEvolSensors);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(wrapperSensors);

					send(&response, Messenger::DEST_UI);
					break;
				}

				case WrStatusTracking:
				{
					if (traceOn)
						Debug::trace("SessionWrapper", msgDebug + HobString("WrStatusTracking"));

					HfcWrapperStatusTracking *trackingMsg = dynamic_cast<HfcWrapperStatusTracking *> (msg->getHfcObject());

					if ((trackingMsg) && (trackingMsg->isStatusTracking() == false))
					{
						setCurrentModeOp(OP_UNKNOWN);
					}

					if (trackingMsg->isStatusTracking())
					{
//						registerEvent(Wrapper, WrStatus, false);
						registerEvent(Wrapper, WrEvolStatus, false);
					}
					else
					{
//						deregisterEvent(Wrapper, WrStatus);
						deregisterEvent(Wrapper, WrEvolStatus);
					}

					//  Not much to do here with a Wrapper Status Tracking message except pass it on.
					//  But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);

					send(msg, Messenger::DEST_RTC);
					break;
				}

//				case WrStatus:
//				{
//					bool traceOn2 = Debug::isTracing("SessionWrapper.WrapperStatus");
//					if (traceOn2)
//					{
//						HobString msgDebug2 = msgDebug;
//						msgDebug2.append(" - handling WrStatus");
//						Debug::trace("SessionWrapper.WrapperStatus", msgDebug2);
//					}
//
//					HfcWrapperStatus *wrapperStatus = dynamic_cast<HfcWrapperStatus *> (msg->getHfcObject());
//
//					if (wrapperStatus == 0)
//						return;
//
//					if (traceOn2)
//					{
//						HobString msgDebug2 = msgDebug;
//						msgDebug2.append("  currentModeOp = ").append(Integer::toString(getCurrentModeOp()));
//						msgDebug2.append("  filmTension = ").append(Integer::toString(wrapperStatus->getStatusInfo().getFilmTension()));
//						msgDebug2.append("  filmWidth = ").append(Integer::toString((WrPackWidth) wrapperStatus->getStatusInfo().getPackWidth()));
//						Debug::trace("SessionWrapper.WrapperStatus", msgDebug2);
//					}
//					if (getCurrentModeOp() == OP_WRAPPERINTAKEPACKAGESIZES)
//					{
//						Debug::trace("SessionWrapper", "Wrapper Status: CurrentModeOp = OP_WRAPPERINTAKEPACKAGESIZES");
//						if (currentPackCode == UndefinedPackCode)
//						{
//							break;
//						}
//
//						HfcWrapperIntakePkgSize *packageSize = new HfcWrapperIntakePkgSize(wrapperStatus, configurationManager->getCurrentWrapperConfig(), currentPackCode);
//						if (packageSize)
//						{
//							if (packageSize->isPresent())
//							{
//								//	  Once we get the message, we are done tracking.
//								setCurrentModeOp(OP_UNKNOWN);
//
//								//	  Send the current Package size
//								sendPackageSizeToUI(packageSize);
//
//								if (!packageSize->thePackageFits())
//								{
//									Debug::trace("SessionWrapper", "Wrapper: THE LAST PACKAGE CYCLED DOES NOT FIT INTO THIS PACK CODE.");
//									wrapperStatus->setInvalidPackSize();
//								}
//							}
//							delete packageSize;
//						}
//					}
//					else if (getCurrentModeOp() == OP_WRAPPERAUTODETECT)
//					{
//						Debug::trace("SessionWrapper", "Wrapper Status: CurrentModeOp = OP_WRAPPERAUTODETECT");
//						HfcWrapperIntakePkgSize *packageSize = new HfcWrapperIntakePkgSize(wrapperStatus);
//						if (packageSize)
//						{
//							if (packageSize->isPresent())
//							{
//								//	  Once we get the message, we are done tracking.
//								setCurrentModeOp(OP_UNKNOWN);
//
//								if ((packageSize->getLength() != 255) || (packageSize->getWidth() != 255))
//								{
//									WrapperPackageInfo intakePackageInfo = wrapperStatus->getPackageInfo(IntakePosition);
//									WrPackCode thisPackCode = intakePackageInfo.getPackCode();
//									//  Send the new Pack code info
//									sendPackCodeToUI(thisPackCode);
//
//									//  Send the current Package size
//									sendPackageSizeToUI(packageSize);
//								}
//							}
//							delete packageSize;
//						}
//					}
//					else if (getCurrentModeOp() == OP_WRAPPERAUTOCONFIG)
//					{
//						Debug::trace("SessionWrapper", "Wrapper Status: CurrentModeOp = OP_WRAPPERAUTOCONFIG");
//						HfcWrapperIntakePkgSize *packageSize = new HfcWrapperIntakePkgSize(wrapperStatus);
//						if (packageSize)
//						{
//							if (packageSize->isPresent())
//							{
//								//	  Once we get the message, we are done tracking.
//								setCurrentModeOp(OP_UNKNOWN);
//
//								if ((packageSize->getLength() != 255) || (packageSize->getWidth() != 255))
//								{
//									//  Set minLength
//									int value = packageSize->getLength() - 5;
//									if ((autoConfigSize.getMinLength() == 0) || (value < autoConfigSize.getMinLength()))
//										autoConfigSize.setMinLength(value);
//
//									//  Set maxLength
//									value = packageSize->getLength() + 5;
//									if ((autoConfigSize.getMaxLength() == 0) || (value > autoConfigSize.getMaxLength()))
//										autoConfigSize.setMaxLength(value);
//
//									//  Set minWidth
//									value = packageSize->getWidth() - 5;
//									if ((autoConfigSize.getMinWidth() == 0) || (value < autoConfigSize.getMinWidth()))
//										autoConfigSize.setMinWidth(value);
//
//									//  Set maxWidth
//									value = packageSize->getWidth() + 5;
//									if ((autoConfigSize.getMaxWidth() == 0) || (value > autoConfigSize.getMaxWidth()))
//										autoConfigSize.setMaxWidth(value);
//
//									WrapperPackCodeInfo packCodeInfo = configurationManager->getCurrentWrapperConfig()->getWrapperPackCodeInfo(currentPackCode);
//
//									//  Do we need to validate these first?
//									packCodeInfo.setPackSizeMinLength(autoConfigSize.getMinLength());
//									packCodeInfo.setPackSizeMaxLength(autoConfigSize.getMaxLength());
//									packCodeInfo.setPackSizeMinWidth(autoConfigSize.getMinWidth());
//									packCodeInfo.setPackSizeMaxWidth(autoConfigSize.getMaxWidth());
//
//									configurationManager->getCurrentWrapperConfig()->setWrapperPackCodeInfo(packCodeInfo);
//
//									//  Send the current Pack code info
//									sendPackCodeToUI(currentPackCode);
//
//									//  Send the current Package size
//									sendPackageSizeToUI(packageSize);
//								}
//							}
//							delete packageSize;
//						}
//					}
//					else
//					{
//						if (traceOn2)
//						{
//							HobString msgDebug2 = msgDebug;
//							msgDebug2.append(" passing msg to configurationManager.");
//							Debug::trace("SessionWrapper.WrapperStatus", msgDebug2);
//						}
//						//	  This msg came from the Wrapper, so set all of the received flags.
//						wrapperStatus->setAllReceived(true);
//						configurationManager->handleMessage(msg);
//
//						if (traceOn2)
//						{
//							HobString msgDebug2 = msgDebug;
//							msgDebug2.append(" Sending Wrapper Status to Browser");
//							Debug::trace("SessionWrapper.WrapperStatus", msgDebug2);
//						}
//
//						msg->setFormatType(BrowserFormat);
//
//						send(msg, Messenger::DEST_UI)
//					}
//
//					if (responseMessageManager)
//						responseMessageManager->manageStatus(wrapperStatus);
//					break;

				case WrEvolStatus:
				{
					bool traceOn2 = Debug::isTracing("SessionWrapper.WrapperStatus");
					if (traceOn2)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append(" - handling WrEvolStatus");
						Debug::trace("SessionWrapper.WrapperStatus", msgDebug2);
					}

					HfcEvolWrapperStatus *wrapperStatus = dynamic_cast<HfcEvolWrapperStatus *> (msg->getHfcObject());

					// Before we get too far along, check the session type to determine whether this message is meant
					// to be sent to the HTe Enterprise app hook.
					// I used to evaluate this via a getTimer call, but the checks below (if true) imply that the relevant 
					// timer is running, and I really shouldn't acquire a thread lock for *every* WrEvolStatus message...
					if (wrapperStatus && getCurrentModeOp() == OP_SENDWRAPPERDATATOAPPHOOK)
					{
						Debug::trace("SessionWrapper.WrapperStatus", "Sending HfcEvolWrapperStatus object to hook.");
						
						sendWrapperDataToHook(wrapperStatus);
						hasProcessedWrStatusMsg = true;

						Debug::trace("SessionWrapper.WrapperStatus", "Finished sending HfcEvolWrapperStatus object to hook.");
						
						break;
					}

					if (wrapperStatus == 0)
						return;

					msg->setFormatType(BrowserFormat);

					send(msg, Messenger::DEST_UI);

					if (responseMessageManager)
						responseMessageManager->manageStatus(wrapperStatus);


					
					break;
				}

//				case WrInquire:
//				{
//					if (traceOn)
//					{
//						HobString msgDebug2 = msgDebug;
//						msgDebug2.append(" - handling WrInquire.");
//						Debug::trace("SessionWrapper", msgDebug2);
//					}
//					if (msg == (SessionMessage *)NULL)
//						return;
//
//					if (traceOn)
//					{
//						HobString msgDebug2 = msgDebug;
//						msgDebug2.append(" passing msg to configurationManager.");
//						Debug::trace("SessionWrapper", msgDebug2);
//					}
//					configurationManager->handleMessage(msg);
//
//					if (getCurrentModeOp() == OP_WRAPPERERRORTOTALS)
//					{
//						Debug::trace("SessionWrapper", "Wrapper Inquire: CurrentModeOp = OP_WRAPPERERRORTOTALS");
//						HfcWrapperConfig *wrapperConfig = configurationManager->getCurrentWrapperConfig();
//						if (wrapperConfig)
//						{
//							HfcWrapperErrorTotals *wrapperErrorTotals = new HfcWrapperErrorTotals(wrapperConfig);
//							if (wrapperErrorTotals)
//							{
//								SessionMessage myMessage(Wrapper, WrErrorTotals);
//								myMessage.setFormatType(BrowserFormat);
//								myMessage.setHfcObject(wrapperErrorTotals);
//
//								send(&myMessage, Messenger::DEST_UI);
//								delete wrapperErrorTotals;
//							}
//						}
//						setCurrentModeOp(OP_UNKNOWN);
//					}
//					else if (getCurrentModeOp() == OP_WRAPPERPACKAGETOTALS)
//					{
//						Debug::trace("SessionWrapper", "Wrapper Inquire: CurrentModeOp = OP_WRAPPERPACKAGETOTALS");
//						HfcWrapperConfig *wrapperConfig = configurationManager->getCurrentWrapperConfig();
//						if (wrapperConfig)
//						{
//							HfcWrapperPackageTotals *wrapperPackageTotals = new HfcWrapperPackageTotals(wrapperConfig);
//							if (wrapperPackageTotals)
//							{
//								SessionMessage myMessage(Wrapper, WrPackageTotals);
//								myMessage.setFormatType(BrowserFormat);
//								myMessage.setHfcObject(wrapperPackageTotals);
//
//								send(&myMessage, Messenger::DEST_UI);
//								delete wrapperPackageTotals;
//							}
//						}
//						setCurrentModeOp(OP_UNKNOWN);
//					}
//					else if (getCurrentModeOp() == OP_WRAPPERPACKCODENAMES)
//					{
//						Debug::trace("SessionWrapper", "Wrapper Inquire: CurrentModeOp = OP_WRAPPERPACKCODENAMES");
//						HfcWrapperConfig *wrapperConfig = configurationManager->getCurrentWrapperConfig();
//						if (wrapperConfig)
//						{
//							HfcWrapperPackCodeNames *wrapperPackCodeNames = new HfcWrapperPackCodeNames(wrapperConfig);
//							if (wrapperPackCodeNames)
//							{
//								SessionMessage myMessage(Wrapper, WrAllPackCodeNames);
//								myMessage.setFormatType(BrowserFormat);
//								myMessage.setHfcObject(wrapperPackCodeNames);
//
//								send(&myMessage, Messenger::DEST_UI);
//								delete wrapperPackCodeNames;
//							}
//						}
//						setCurrentModeOp(OP_UNKNOWN);
//					}
//					else if (isInService())
//					{
//						Debug::trace("SessionWrapper", getName() + HobString(" Sending Wrapper Inquire to Browser"));
//
//						msg->setFormatType(BrowserFormat);
//
//						send(msg, Messenger::DEST_UI);
//					}
//					break;
//				}
//
//				case WrConfig:
//				{
//					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrConfig message"));
//					configurationManager->handleMessage(msg);
//					break;
//				}

				case WrResetFactoryDefaults:
				{
					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrResetFactoryDefaults message"));
					configurationManager->handleMessage(msg);
					break;
				}

//				case WrCycle:
//				{
//					if (traceOn)
//						Debug::trace("SessionWrapper", msgDebug + HobString("WrCycle"));
//
//					//  Not much to do here with a Wrapper Cycle message except pass it on.
//					//  But first, change the format from Browser to Internal.
//					msg->setFormatType(InternalFormat);
//
//					send(msg, Messenger::DEST_RTC);
//					break;
//				}

				case WrEvolCycle:
				{
					if (traceOn)
						Debug::trace("SessionWrapper", msgDebug + HobString("WrEvolCycle"));

					//	Not much to do here with a Wrapper Cycle message except pass it on.
					//	But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);

					send(msg, Messenger::DEST_RTC);
					break;
				}

				case WrTestTracking:
				{
					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrTestTracking"));

					HfcWrapperTestTracking *currentTestTracking = dynamic_cast<HfcWrapperTestTracking *> (msg->getHfcObject());
					if(currentTestTracking)
					{
						if(currentTestTracking->isSensorsTracking())
						{
							Debug::trace("SessionWrapper", getName() + HobString(" - TestTracking ON"));
							setCurrentModeOp(OP_WRAPPERTESTS);
							registerEvent(Wrapper, WrSensors);
						}
						else
						{
							Debug::trace("SessionWrapper", getName() + HobString(" - TestTracking OFF"));
							setCurrentModeOp(OP_UNKNOWN);
							deregisterEvent(Wrapper, WrSensors);
						}

						//	  The Wrapper Manager knows nothing about a WrTestTracking message. It's really
						//	  a WrSensorsTracking message so that is what we will send.
						HfcWrapperSensorsTracking *currentSensorsTracking =  new HfcWrapperSensorsTracking(currentTestTracking->isSensorsTracking());
						if (currentSensorsTracking)
						{
							SessionMessage forwardMsg(Wrapper, WrSensorsTracking);
							forwardMsg.setFormatType(InternalFormat);
							forwardMsg.setHfcObject(currentSensorsTracking);

							send(&forwardMsg, Messenger::DEST_RTC);
							delete currentSensorsTracking;
						}
					}
					break;
				}

				case WrReqErrorTotals:
				{
					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrReqErrorTotals"));

					setCurrentModeOp(OP_WRAPPERERRORTOTALS);

					registerEvent(Wrapper, WrInquire,false);
					SessionMessage request(Wrapper, WrReqInquire);
					request.setFormatType(InternalFormat);

					send(&request, Messenger::DEST_RTC);
					break;
				}

				case WrResetErrorTotals:
				{
					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrResetErrorTotals"));
					configurationManager->handleMessage(msg);
					break;
				}

				case WrReqPackageTotals:
				{
					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrReqPackageTotals"));

					setCurrentModeOp(OP_WRAPPERPACKAGETOTALS);

					registerEvent(Wrapper, WrInquire,false);
					SessionMessage request(Wrapper, WrReqInquire);
					request.setFormatType(InternalFormat);

					send(&request, Messenger::DEST_RTC);
					break;
				}

				case WrResetPackageTotals:
				{
					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrResetPackageTotals"));
					configurationManager->handleMessage(msg);
					break;
				}

//				case WrAllPackCodeNames:
//				{
//					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrAllPackCodeNames"));
//
//					setCurrentModeOp(OP_WRAPPERPACKCODENAMES);
//
//					registerEvent(Wrapper, WrInquire,false);
//					SessionMessage request(Wrapper, WrReqInquire);
//					request.setFormatType(InternalFormat);
//
//					send(&request, Messenger::DEST_RTC);
//					break;
//				}
//
//				case WrReadPackCode:
//				{
//					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrReadPackCode"));
//
//					HfcWrapperPackCodeInfo *readPackCode = dynamic_cast<HfcWrapperPackCodeInfo *> (msg->getHfcObject());
//					if (readPackCode)
//					{
//						WrPackCode thisPackCode = readPackCode->getPackCode();
//						sendPackCodeToUI(thisPackCode);
//					}
//					break;
//				}
//
//				case WrWritePackCode:
//				{
//					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrWritePackCode"));
//					configurationManager->handleMessage(msg);
//					break;
//				}
//
//				case WrSavePackCode:
//				{
//					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrSavePackCode"));
//					configurationManager->handleMessage(msg);
//					break;
//				}

				case WrEvolRestorePackageTimings:		 // Restore from backed up data.
				{
					//	Exit only if there is no Access wrapper attached
					if (!SessionManager::getInstance()->isEvolutionWrapperAttached())
					{
						Debug::trace("SessionWrapper", msgDebug + HobString("Wrapper not attached! Ignoring WrEvolRestorePackageTimings."));
						break;
					}
				}
				case WrEvolLoadPackageTimings:			// Load Current vs. Manufacturing Default Timings
				case WrEvolPackageTimings:				// Timings coming from the Wrapper (via a Pointer)
				case WrEvolSavePackageTimingsComplete:	// Wrapper has processed the message.
				case WrEvolResetDefaultPackageTimings:	// Reset wrapper timings with manufacturing defaults
				case WrEvolDonePackageTimings:			// Ended viewing/updating timings
				case WrEvolAutoDetectTrayType:			// What's the tray type for this tray?
				case WrEvolAutoConfigTrayType:			// Configure a new tray type
				case WrEvolCreateTrayType:				// Create a tray type
				case WrEvolChangeTrayTypePrecedence:	  // Change precedence on a tray type
				case WrEvolWriteTrayType:				 // Update a tray type
				case WrEvolDeleteTrayType:				// Delete a tray type
				case WrEvolReadWrapBox:				   // Read a wrap box
				case WrEvolWriteWrapBox:				  // Update a wrap box
				case WrEvolPrintWrapBox:				  // Print a wrap box settings
				case WrEvolCancelWrapBoxChanges:		  // Cancel Wrap Box changes
				case WrEvolDoneWrapBox:				   // Ended viewing/updating a wrap box
				case WrEvolReadHeightBox:				 // Load a height box
				case WrEvolWriteHeightBox:				// Update a height box
				case WrEvolRenameHeightBox:			   // Change the height of a height box
				case WrEvolCreateHeightBox:			   // Create a new height box
				case WrEvolVerifyInHeightBox:			 // Verify a tray in a height box
				case WrEvolDeleteHeightBox:			   // Delete a height box
				case WrEvolReadAllForAttribute:
				case WrEvolSaveAttributeChanges:
				case WrEvolCancelAttributeChanges:
				case WrEvolDoneAttributeChanges:
				case WrEvolSetMinTrayDims:
				case WrEvolSetMaxTrayDims:
				case WrEvolSetMinMaxTrayDims:
				{
					if (traceOn)
						Debug::trace("SessionWrapper", msgDebug + HobString("Msg for the evolWrapperTimingsManager_"));

					if ( !evolWrapperTimingsManager_ )
					{
						if (traceOn)
							Debug::trace("SessionWrapper", msgDebug + HobString("Creating a new evolWrapperTimingsManager_"));
						createEvolWrapperTimingsManager();
					}

					if ( evolWrapperTimingsManager_ )
					{
						evolWrapperTimingsManager_->handleMessage(msg);

						// If we leave the wrapper timings page, get rid of the timings manager
						if ( evolWrapperTimingsManager_->deleteMe() )
						{
							if (traceOn)
								Debug::trace("SessionWrapper", msgDebug + HobString("Deleting the evolWrapperTimingsManager_"));

							delete evolWrapperTimingsManager_;
							evolWrapperTimingsManager_ = 0;
						}
					}
					break;
				}

				case WrEvolPackageDesc:
				{
					if (traceOn)
						Debug::trace("SessionWrapper", msgDebug + HobString("WrEvolPackageDesc"));

					 HfcEvolWrapperPackageDesc *wrPackageDesc = dynamic_cast<HfcEvolWrapperPackageDesc *> (msg->getHfcObject());

					if (wrPackageDesc == 0)
						return;

					SessionMessage response(Wrapper, WrEvolPackageDesc);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(wrPackageDesc);

					send(&response, Messenger::DEST_UI);
					if ( currentMode == OpModeService && evolWrapperTimingsManager_ )
						evolWrapperTimingsManager_->handleMessage(msg);
					break;
				}

//				case WrCycleMode:
//				{
//					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrCycleMode"));
//
//					HfcWrapperCycleMode *wrapperCycleMode = dynamic_cast<HfcWrapperCycleMode *> (msg->getHfcObject());
//					if(wrapperCycleMode)
//					{
//						if(wrapperCycleMode->isValidMode())
//						{
//							WrapperCycle wrapperCycle;
//
//							if(wrapperCycleMode->isAutoDetectMode())
//								setCurrentModeOp(OP_WRAPPERAUTODETECT);
//							else if(wrapperCycleMode->isPackageSizeMode())
//							{
//								wrapperCycle.setWrPackCode(wrapperCycleMode->getPackCode());
//								setCurrentModeOp(OP_WRAPPERINTAKEPACKAGESIZES);
//							}
//							else if(wrapperCycleMode->isAutoConfigMode())
//							{
//								if(wrapperCycleMode->getCycleCount() == 0)
//									initAutoConfigSize();
//								setCurrentModeOp(OP_WRAPPERAUTOCONFIG);
//							}
//							currentPackCode = wrapperCycleMode->getPackCode();
//
//							//  Now Cycle the Wrapper
//							send(&wrapperCycle, Messenger::DEST_RTC);
//						}
//					}
//					break;
//				}
//
//				case WrReqErrorHistory:
//				{
//					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrReqErrorHistory"));
//
//					registerEvent(Wrapper, WrErrorHistory,false );
//
//					//  Not much to do here with a Wrapper Request Error History message except pass it on.
//					//  But first, change the format from Browser to Internal.
//					msg->setFormatType(InternalFormat);
//
//					send(msg, Messenger::DEST_RTC);
//					break;
//				}
//
//				case WrErrorHistoryTracking:
//				{
//					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrErrorHistoryTracking"));
//
//					HfcWrapperErrorHistoryTracking *errorHistoryTracking = dynamic_cast<HfcWrapperErrorHistoryTracking *> (msg->getHfcObject());
//					if(errorHistoryTracking)
//					{
//						if(errorHistoryTracking->isErrorHistoryTracking())
//						{
//							Debug::trace("SessionWrapper", getName() + HobString(" - ErrorHistoryTracking ON"));
//							registerEvent(Wrapper, WrErrorHistory);
//						}
//						else
//						{
//							Debug::trace("SessionWrapper", getName() + HobString(" - ErrorHistoryTracking OFF"));
//							deregisterEvent(Wrapper, WrErrorHistory);
//						}
//					}
//
//					//  Not much to do here with a WrErrorHistoryTracking message except pass it on.
//					//  But first, change the format from Browser to Internal.
//					msg->setFormatType(InternalFormat);
//
//					send(msg, Messenger::DEST_RTC);
//					break;
//				}
//
//				case WrErrorHistory:
//				{
//					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrErrorHistory"));
//
//					HfcWrapperErrorHistory *wrapperErrorHistory = dynamic_cast<HfcWrapperErrorHistory *> (msg->getHfcObject());
//
//					if (wrapperErrorHistory == (HfcWrapperErrorHistory *) 0)
//						return;
//
//					msg->setHfcObjectCreatedInternally(false);
//
//					SessionMessage errorHistory(Wrapper, WrErrorHistory);
//					errorHistory.setFormatType(BrowserFormat);
//					errorHistory.setHfcObject(wrapperErrorHistory);
//
//					send(&errorHistory, Messenger::DEST_UI);
//					break;
//				}
//
//				case WrSetGripperPWMTimings:
//				{
//					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrSetGripperPWMTimings"));
//
//					//  Not much to do here with a WrSetGripperPWMTimings message except pass it on.
//					//  But first, change the format from Browser to Internal.
//					msg->setFormatType(InternalFormat);
//
//					send(msg, Messenger::DEST_RTC);
//					break;
//				}

				case WrReqSensors:
				{
					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrReqSensors"));

					registerEvent(Wrapper, WrEvolSensors);
					//  Not much to do here with a Request Sensors message except pass it on.
					//  But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);

					send(msg, Messenger::DEST_RTC);
					break;
				}

				case WrReqPackageHistory:
				{
					Debug::trace("SessionWrapper", getName() + HobString(" - handling WrReqPackageHistory"));

					registerEvent(Wrapper, WrEvolPackageHistory, false);

					//  Not much to do here with a Wrapper Request Package History message except pass it on.
					//  But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);

					send(msg, Messenger::DEST_RTC);
					break;
				}

//				case WrPackageHistory:
//				{
//					if (traceOn)
//						Debug::trace("SessionWrapper", msgDebug + " - handling WrPackageHistory");
//
//					HfcWrapperPackageHistory *packageHistory = dynamic_cast<HfcWrapperPackageHistory *> (msg->getHfcObject());
//
//					if (!packageHistory)
//						return;
//
//					HfcWrapperConfig *wrapConfig = configurationManager->getCurrentWrapperConfig();
//					packageHistory->getPackCodeNames(wrapConfig);
//
//					msg->setFormatType(BrowserFormat);
//					send(msg, Messenger::DEST_UI);
//					break;
//				}

				case WrEvolPackageHistory:
				{
					if (traceOn)
						Debug::trace("SessionWrapper", msgDebug + HobString("WrEvolPackageHistory"));

					 HfcEvolWrapperPackageHistory *packageHistory = dynamic_cast<HfcEvolWrapperPackageHistory *> (msg->getHfcObject());

					if (!packageHistory)
						return;

					msg->setFormatType(BrowserFormat);
					send(msg, Messenger::DEST_UI);
					
					break;
				}

				case WrEvolCamerasTracking:
				{
					if (traceOn)
						Debug::trace("SessionWrapper", msgDebug + HobString("WrEvolCamerasTracking"));

					HfcEvolWrapperCamerasTracking *currentCamerasTracking = dynamic_cast<HfcEvolWrapperCamerasTracking *> (msg->getHfcObject());

					if(currentCamerasTracking)
					{
						if(currentCamerasTracking->isCamerasTracking())
						{
							if (traceOn)
								Debug::trace("SessionWrapper", getName() + HobString(" - EvolWrapperCamerasTracking ON"));
							registerEvent(Wrapper, WrCameraScan);
							//registerEvent(Wrapper, WrEvolTestComplete, msgQId);
						}
						else
						{
							if (traceOn)
								Debug::trace("SessionWrapper", getName() + HobString(" - EvolWrapperCamerasTracking OFF"));
							deregisterEvent(Wrapper, WrCameraScan);
							//deregisterEvent(Wrapper, WrEvolTestComplete, msgQId);
						}
					}

					//	Not much to do here with a Wrapper Sensors Tracking message except pass it on.
					//	But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);
					send(msg, Messenger::DEST_RTC);
					break;
				}

				case WrCameraScan:
				{
					if ( traceOn )
						Debug::trace("SessionWrapper", msgDebug + HobString("WrCameraScan"));

					HfcEvolWrapperCameraScan *ewcs( dynamic_cast<HfcEvolWrapperCameraScan*>( msg->getHfcObject() ) );

					if ( !ewcs )
					{
						if ( traceOn )
							Debug::trace("SessionWrapper", msgDebug + HobString("WrCameraScan - HfcEvolWrapperCameraScan in NULL!!"));

						return;
					}
					else if ( traceOn )
					{
						Debug::trace("SessionWrapper", msgDebug + HobString("WrCameraScan - **********************************************************"));

						Debug::trace("SessionWrapper", msgDebug + HobString("WrCameraScan - handle WrCameraScan, showing camera scan start"));

						ewcs->show();
						Debug::trace("SessionWrapper", msgDebug + HobString("WrCameraScan - handle WrCameraScan, showing camera scan end"));
					}

					//	Not much to do here with a WrCameraScan message except pass it on to the UI.
					//	But first, change the format from Internal to Browser.
					msg->setFormatType( BrowserFormat );
					send( msg, Messenger::DEST_UI );

					break;
				}

				case WrEvolCameraHeightScanTracking:
				{
					if (traceOn)
						Debug::trace("SessionWrapper", msgDebug + HobString("WrEvolCameraHeightScanTracking"));

					HfcEvolWrapperCamerasTracking *currentCamerasTracking = dynamic_cast<HfcEvolWrapperCamerasTracking *> (msg->getHfcObject());

					if(currentCamerasTracking)
					{
						curHeightScan.setScan(0);						//	Clear the scan so we can send only changes to the UI.
						if(currentCamerasTracking->isCamerasTracking())
						{
							if (traceOn)
								Debug::trace("SessionWrapper", msgDebug + HobString(" - EvolCameraHeightScanTracking ON"));
							//registerEvent(Wrapper, WrCameraHeightScan, false);
							registerEvent(Wrapper, WrCameraHeightScan);
						}
						else
						{
							if (traceOn)
								Debug::trace("SessionWrapper", msgDebug + HobString(" - EvolCameraHeightScanTracking OFF"));
							deregisterEvent(Wrapper, WrCameraHeightScan);

							//	Scanning is Off. Send the cleared scan to the browser, to clear the display.
							SessionMessage toUI(Wrapper, WrCameraHeightScan);
							toUI.setFormatType(BrowserFormat);
							HfcEvolWrapperCameraHeightScan newScan((UINT16) 0);
							toUI.setHfcObject(&newScan);
							send(&toUI, Messenger::DEST_UI);
						}
					}

					//	Not much to do here with a WrEvolCameraHeightScanTracking message except pass it on.
					//	But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);
					send(msg, Messenger::DEST_RTC);
					break;
				}

				case WrCameraHeightScan:
				{
					if (traceOn)
						Debug::trace("SessionWrapper", msgDebug + HobString("WrCameraHeightScan"));

					HfcEvolWrapperCameraHeightScan *wrScan = dynamic_cast<HfcEvolWrapperCameraHeightScan *> (msg->getHfcObject());
					if (!wrScan)
					{
						Debug::trace("SessionWrapper", msgDebug + "wrScan is NULL! Exiting!!!");
						return;
					}

					//	Pass the message on to the UI if it is different from the previous scan.
					//	Otherwise, the Browser will get bogged down processing them.
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append("WrCameraHeightScan:  curHeightScan = ").append(Hex::toString(curHeightScan.getScan()));
						msgDebug2.append(",  new wrScan = ").append(Hex::toString(wrScan->getScan()));
						Debug::trace("SessionWrapper", msgDebug2);
					}
					if (!wrScan->equals(curHeightScan))
					{
						if (traceOn)
							Debug::trace("SessionWrapper", msgDebug + "WrCameraHeightScan - height sensors have changed.");
						curHeightScan.setScan(wrScan->getScan());

						msg->setFormatType(BrowserFormat);
						send(msg, Messenger::DEST_UI);
					}
					break;
				}

				case WrCameraCalibration:
				{
					//if (traceOn)
					Debug::trace("SessionWrapper", msgDebug + "WrCameraCalibration");

					HfcEvolWrapperCameraCalibration *cameraCal = dynamic_cast<HfcEvolWrapperCameraCalibration *> (msg->getHfcObject());
					if (!cameraCal)
					{
						//Debug::trace("SessionWrapper", msgDebug + "cameraCal is NULL! Exiting!!!");
						return;
					}

					//	Not much to do here with a WrCameraCalibration message except pass it to the UI.
					SessionMessage response(Wrapper, WrCameraCalibration);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(cameraCal);

					send(&response, Messenger::DEST_UI);
					break;
				}
				case WrEvolRemoveUpgradeFiles:
				{
					if (traceOn)
						Debug::trace("SessionWrapper", msgDebug + "WrEvolRemoveUpgradeFiles");

					 HfcEvolWrapperRemoveUpgradeFiles *wrMsg = dynamic_cast<HfcEvolWrapperRemoveUpgradeFiles *> (msg->getHfcObject());

					if (wrMsg == 0)
					{
						if (traceOn)
							Debug::trace("SessionWrapper", msgDebug + "WrEvolRemoveUpgradeFiles:  wrMsg is 0!");
						return;
					}
					
					wrMsg->show();
					if (wrMsg->getState() == EvolWrapperRemoveUpgradeFiles::BeginRemoval)
					{
						if (responseMessageManager)
							responseMessageManager->addMessage(ErrWrapper, 9714, HfcResponse::RESPONSESTYLE_OK);

						SessionMessage removeMsg(VersionManagerDev, RemoveWrapperSWComponents);
						removeMsg.setFormatType(InternalFormat);
						if (traceOn)
							Debug::trace("SessionWrapper", msgDebug + "WrEvolRemoveUpgradeFiles:  Sending RemoveWrapperSWComponents to VersionManagerDev");
						send(&removeMsg, Messenger::DEST_RTC);
					}
					else if (wrMsg->getState() == EvolWrapperRemoveUpgradeFiles::RemovalComplete)
					{
						if (traceOn)
							Debug::trace("SessionWrapper", msgDebug + "WrEvolRemoveUpgradeFiles:  RemovalComplete.");
						if (responseMessageManager)
						{
							PAL_absclass::PAL_sleep(30);
							responseMessageManager->removeMessage(ErrWrapper, 9714, HfcResponse::RESPONSESTYLE_OK);
							responseMessageManager->addMessage(ErrWrapper, 9715, HfcResponse::RESPONSESTYLE_OK);
						}
					}
					break;
				}
				case WrEvolEventCapture:
				{
					Debug::trace("SessionWrapper", "WrEvolEventCapture");

					HfcEvolWrapperEventCapture *eventCaptureMsg( dynamic_cast<HfcEvolWrapperEventCapture*>( msg->getHfcObject() ) );
					if ( !eventCaptureMsg )
						return;

					SessionMessage response(Wrapper, WrEvolEventCapture);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(eventCaptureMsg);

					send(&response, Messenger::DEST_UI);

					if (( KnifeUpEvent == eventCaptureMsg->getEventCaptureType() ) || ( KnifeDownEvent == eventCaptureMsg->getEventCaptureType() ))
					{
						bool newTime = false;

						if ( !evolWrapperKnifeHistoryManager )
							evolWrapperKnifeHistoryManager = new EvolWrapperKnifeHistoryManager(this);

						if ( evolWrapperKnifeHistoryManager )
							newTime = evolWrapperKnifeHistoryManager->processWrapperKnifeTime(eventCaptureMsg);
				
						if (( newTime ) && ( isInService() ))
						{
							//	Refresh the knife time history page.
							HfcEvolWrapperKnifeTimeHistory *knifeTimeHistory( evolWrapperKnifeHistoryManager->getEvolWrapperKnifeTimeHistory() );

							if ( knifeTimeHistory )
							{
								SessionMessage response(Wrapper, WrKnifeTimeHistory);
								response.setFormatType(BrowserFormat);
								response.setHfcObject(knifeTimeHistory);

								send(&response, Messenger::DEST_UI);
								Debug::trace("SessionWrapper", "New time recorded. WrKnifeTimeHistory sent to UI.");
							}
						}
					}
					break;
				}
				case WrClearInstallKnifeTime:
				{
					Debug::trace("SessionWrapper", "WrClearInstallKnifeTime");

					if( !evolWrapperKnifeHistoryManager )
						evolWrapperKnifeHistoryManager = new EvolWrapperKnifeHistoryManager(this);

					if( evolWrapperKnifeHistoryManager )
					{
						evolWrapperKnifeHistoryManager->deleteAllEvolWrapperKnifeHistories();
						Debug::trace("SessionWrapper", "Cleared all knife time histories.");

						HfcEvolWrapperKnifeTimeHistory *knifeTimeHistory = evolWrapperKnifeHistoryManager->getEvolWrapperKnifeTimeHistory();
						if ( knifeTimeHistory )
						{
							SessionMessage response(Wrapper, WrKnifeTimeHistory);
							response.setFormatType(BrowserFormat);
							response.setHfcObject(knifeTimeHistory);

							send(&response, Messenger::DEST_UI);
							Debug::trace("SessionWrapper", "WrKnifeTimeHistory sent to UI");
						}
					}
					break;
				}
				case WrRequestKnifeTimeHistory:
				{
					Debug::trace("SessionWrapper", "WrRequestKnifeTimeHistory");

					if( !evolWrapperKnifeHistoryManager )
						evolWrapperKnifeHistoryManager = new EvolWrapperKnifeHistoryManager(this);

					if( evolWrapperKnifeHistoryManager )
					{
						HfcEvolWrapperKnifeTimeHistory *knifeTimeHistory = evolWrapperKnifeHistoryManager->getEvolWrapperKnifeTimeHistory();
						if ( knifeTimeHistory )
						{
							SessionMessage response(Wrapper, WrKnifeTimeHistory);
							response.setFormatType(BrowserFormat);
							response.setHfcObject(knifeTimeHistory);

							send(&response, Messenger::DEST_UI);
							Debug::trace("SessionWrapper", "WrKnifeTimeHistory sent to UI");
						}
					}
					break;
				}
				case WrEvolErrorHistory:
				case WrEvolReqErrorHistory:
				case WrEvolErrorHistoryClear:
				case WrEvolErrorHistoryListPrevious:
				case WrEvolErrorHistoryListNext:
				case WrEvolErrorHistoryListDone:
				{
					if (traceOn)
					{
						switch(msg->getEventType())
						{
							case WrEvolErrorHistory:
								Debug::trace("SessionWrapper", "WrEvolErrorHistory");
								break;
							case WrEvolReqErrorHistory:
								Debug::trace("SessionWrapper", "WrEvolReqErrorHistory");
								break;
							case WrEvolErrorHistoryClear:
								Debug::trace("SessionWrapper", "WrEvolErrorHistoryClear");
								break;
							case WrEvolErrorHistoryListPrevious:
								Debug::trace("SessionWrapper", "WrEvolErrorHistoryListPrevious");
								break;
							case WrEvolErrorHistoryListNext:
								Debug::trace("SessionWrapper", "WrEvolErrorHistoryListNext");
								break;
							case WrEvolErrorHistoryListDone:
								Debug::trace("SessionWrapper", "WrEvolErrorHistoryListDone");
								break;
						}
					}
					if ( !evolWrapperErrorHistoryManager )
						evolWrapperErrorHistoryManager = new EvolWrapperErrorHistoryManager(this);

					if (evolWrapperErrorHistoryManager)
						evolWrapperErrorHistoryManager->handleMessage(msg);
					break;
				}
				case WrEvolReqCycleCount:
				{
					msg->setFormatType(InternalFormat);
					send(msg, Messenger::DEST_RTC);
			
					registerEvent(Wrapper, WrEvolCycleCount);
					break;
				}
				case WrEvolStartMfgRunin:
				{
					msg->setFormatType(InternalFormat);

					send(msg, Messenger::DEST_RTC);
					break;
				}
				case WrEvolCycleCount:
				{
					HfcEvolWrapperCycleCount *pEvolWrapperCycleCount = dynamic_cast<HfcEvolWrapperCycleCount *> (msg->getHfcObject());

					SessionMessage response(Wrapper, WrEvolCycleCount);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(pEvolWrapperCycleCount);

					send(&response, Messenger::DEST_UI);
					break;
				}
				case WrEvolReqSealerPkgDetectorStatus:
				case WrEvolReqElevatorPkgDetectorStatus:
				{
					msg->setFormatType(InternalFormat);

					send(msg, Messenger::DEST_RTC);
					break;
				}
				case WrEvolPkgDetectorStatus:
				{
					HfcEvolWrapperPkgDetectorStatus *pPkgDetectorStatus = dynamic_cast<HfcEvolWrapperPkgDetectorStatus *> (msg->getHfcObject());

					SessionMessage response(Wrapper, WrEvolPkgDetectorStatus);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(pPkgDetectorStatus);

					send(&response, Messenger::DEST_UI);
					break;
				}
				case WrEvolRequestCameraThreshold:
				{
					Debug::trace("SessionWrapper.Camera", "WrEvolRequestCameraThreshold");
					SessionMessage request(Wrapper, WrEvolRequestCameraThreshold);
					request.setFormatType(InternalFormat);

					send(&request, Messenger::DEST_RTC);
					break;
				}
				case WrEvolCameraThreshold:
				{
					Debug::trace("SessionWrapper.Camera", "WrEvolCameraThreshold");
					HfcEvolWrapperCameraThreshold *pHfcEvolWrapperCameraThreshold = dynamic_cast<HfcEvolWrapperCameraThreshold *> (msg->getHfcObject());					

					SessionMessage response(Wrapper, WrEvolCameraThreshold);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(pHfcEvolWrapperCameraThreshold);

					send(&response, Messenger::DEST_UI);
					break;
				}
				case WrEvolWriteCameraThreshold:
				{
					Debug::trace("SessionWrapper.Camera", "WrEvolWriteCameraThreshold");
					
					//	Not much to do here with a Wrapper Write Camera Threshold message except pass it on.
					//	But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);
					send(msg, Messenger::DEST_RTC);

					if (responseMessageManager)
						responseMessageManager->addMessage(ErrWrapper, 9719, HfcResponse::RESPONSESTYLE_OK);
					break;
				}

				case WrReqEvolSystemInfo:
				{
					Debug::trace("SessionWrapper", "WrReqEvolSystemInfo");
					
					//	Not much to do here with a Wrapper Request System Info message except pass it on.
					//	But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);
					send(msg, Messenger::DEST_RTC);
					break;
				}

				case WrEvolSystemInfo:
				{
					Debug::trace("SessionWrapper", "WrEvolSystemInfo");
					HfcEvolWrapperSystemInfo *pHfcEvolWrapperSystemInfo = dynamic_cast<HfcEvolWrapperSystemInfo *> (msg->getHfcObject());					

					if ( wrapperIsTwoPrinter )
					{
						int newDischarge = pHfcEvolWrapperSystemInfo->isLeftToRight();
						if (newDischarge == -1)
						{
							Debug::trace("SessionWrapper", "NoModel returned from wrapper system info");
						}
						else
						{
							int isWrapperLR = FpmManager::getInstance()->isWrapperLeftToRight();
							if (isWrapperLR == -1)
							{
								Debug::trace("SessionWrapper", "Could not get system style from FPM Manager");
							}
							else
							{
								if ((isWrapperLR == 1 && newDischarge == RightDischarge) ||
									(isWrapperLR == 0 && newDischarge == LeftDischarge)
									)
								{
									Debug::trace("SessionWrapper", "SystemInfo discharge matches model style");
								}
								else
								{
									Debug::trace("SessionWrapper", "SystemInfo discharge does not march model style, throwing dialog");
									if (responseMessageManager)
									{
										responseMessageManager->addMessage(ErrWrapper, 9743, HfcResponse::RESPONSESTYLE_OK);
									}
								}
							}
						}
					}
					SessionMessage response(Wrapper, WrEvolSystemInfo);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(pHfcEvolWrapperSystemInfo);

					Debug::trace("SessionWrapper", "WrEvolSystemInfo getAwsSerialNumber()" + pHfcEvolWrapperSystemInfo->getAwsSerialNumber());
					Debug::trace("SessionWrapper", "WrEvolSystemInfo getAwsModelNumber()" + pHfcEvolWrapperSystemInfo->getAwsModelNumber());
					Debug::trace("SessionWrapper", "WrEvolSystemInfo getMotorBoardDateOfManufacture()" + pHfcEvolWrapperSystemInfo->getMotorBoardDateOfManufacture() );
					Debug::trace("SessionWrapper", "WrEvolSystemInfo getMotorBoardPartNumber()" + pHfcEvolWrapperSystemInfo->getMotorBoardPartNumber());
					Debug::trace("SessionWrapper", "WrEvolSystemInfo getAwsDateOfManufacture()" + pHfcEvolWrapperSystemInfo->getAwsDateOfManufacture());
					Debug::trace("SessionWrapper", "WrEvolSystemInfo getAwsCycleCount()" + Integer::toString(pHfcEvolWrapperSystemInfo->getAwsCycleCount()));

					send(&response, Messenger::DEST_UI);

					cancelWrSystemInfoSavedTimer();

					if (responseMessageManager)
					{
						responseMessageManager->removeMessage(ErrWrapper, 9705, HfcResponse::RESPONSESTYLE_NOINPUT);
						Debug::trace("SessionWrapper", "WrEvolSystemInfo:  Removing message 9705.");
					}
					else
					{
						Debug::trace("SessionWrapper", "WrEvolSystemInfo:  Error!!! responseMessageManager is NULL!");
					}
					break;
				}
				case WrEvolSystemInfoErase:
				{
					Debug::trace("SessionWrapper", "WrEvolSystemInfoErase");
					
					//	Not much to do here with a Wrapper System Info Erase message except pass it on.
					//	But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);
					send(msg, Messenger::DEST_RTC);
					break;
				}
				case WrEvolSystemInfoWrite:
				{
					Debug::trace("SessionWrapper", "WrEvolSystemInfoWrite");

					//	Not much to do here with a Wrapper Write System Info message except pass it on.
					//	But first, change the format from Browser to Internal.
					msg->setFormatType(InternalFormat);
					send(msg, Messenger::DEST_RTC);

					//	Now we need to wait for the system info message to be returned. Set the timer to do that.
					if (responseMessageManager)
					{
						Debug::trace("SessionWrapper", "WrEvolSystemInfoWrite:  Sending message 9705 to the ResponseMessageManager.");
						responseMessageManager->addMessage(ErrWrapper, 9705, HfcResponse::RESPONSESTYLE_NOINPUT);
					}
					else
					{
						Debug::trace("SessionWrapper", "WrEvolSystemInfoWrite:  Error!!! responseMessageManager is NULL!");
					}
					setWrSystemInfoSavedTimer();
					break;
				}
				case WrEvolManufacturingRunInNotice:
				{
					Debug::trace("SessionWrapper", "WrEvolManufacturingRunInNotice");

					SessionMessage response(Wrapper, WrEvolManufacturingRunInNotice);
					response.setFormatType(BrowserFormat);
				
					send(&response, Messenger::DEST_UI);
				
					//register for motor profiles
					registerEvent(Wrapper, WrEvolMotorProfile);
					break;
				}
				case WrEvolMotorProfile:
				{
					Debug::trace("SessionWrapper", "WrEvolMotorProfile");

					HfcEvolWrapperMotorProfile *pHfcEvolWrapperMotorProfile = dynamic_cast<HfcEvolWrapperMotorProfile *> (msg->getHfcObject());					

					SessionMessage response(Wrapper, WrEvolMotorProfile);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(pHfcEvolWrapperMotorProfile);

					send(&response, Messenger::DEST_UI);
					break;
				}
				case WrReqEvolHarwareAdditions:
				{
					HfcEvolWrapperConfig *wrapperConfig = configurationManager->getCurrentEvolWrapperConfig();
					if(wrapperConfig)
					{
						SessionMessage response(Wrapper, WrEvolInquire);
						response.setFormatType(BrowserFormat);
						response.setHfcObject(wrapperConfig);

						send(&response, Messenger::DEST_UI);
					}
					break;		
				}
				case WrReqUIWrapperModel:
				{
					Debug::trace("SessionWrapper", "WrReqUIWrapperModel");
					HfcEvolWrapperReqUIModel *pHfcEvolWrapperReqUIModel = dynamic_cast<HfcEvolWrapperReqUIModel *> (msg->getHfcObject());	
					
					SessionMessage response(Wrapper, WrReqUIWrapperModel);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(pHfcEvolWrapperReqUIModel);

					send(&response, Messenger::DEST_UI);
					break;
				}
				case WrUIWrapperModel:
				{
					Debug::trace("SessionWrapper", "WrUIWrapperModel");
					HfcEvolWrapperUIModel *pHfcEvolWrapperUIModel = dynamic_cast<HfcEvolWrapperUIModel *> (msg->getHfcObject());	
					
					SessionMessage response(Wrapper, WrUIWrapperModel);
					response.setFormatType(InternalFormat);
					response.setHfcObject(pHfcEvolWrapperUIModel);
					
					send(&response, Messenger::DEST_RTC);
					break;
				}
				case WrNGWWrapperDetached:
				{
					Debug::trace("SessionWrapper", "WrNGWWrapperDetached");
					//SOF-2564
					if ( evolWrapperTimingsManager_ ) {
						evolWrapperTimingsManager_->setFirstPass();
					} else {
						Debug::trace("SessionWrapper", "WrNGWWrapperDetached evolWrapperTimingsManager_ is NULL!");
					}

					break;
				}
				case WrNGWDimensionOverrides:
				{
					Debug::trace("SessionWrapper", "HfcNGWDimensionOverrides");
					HfcNGWDimensionOverrides *pOverrides = dynamic_cast<HfcNGWDimensionOverrides *> (msg->getHfcObject());	
					SessionMessage response(Wrapper, WrNGWDimensionOverrides);
					response.setFormatType(InternalFormat);
					response.setHfcObject( pOverrides );					
					send( &response, Messenger::DEST_RTC );
					break;
				}
				
#ifdef TRACK_EVOL_WRAPPER_ERRORS
				case WrEvolRequestPackageDesc:
				{
					if (traceOn)
						Debug::trace("SessionWrapper", msgDebug + HobString("WrEvolPackageDesc"));

					 HfcEvolWrapperPackageDesc *wrPackageDesc = dynamic_cast<HfcEvolWrapperPackageDesc *> (msg->getHfcObject());

					if (wrPackageDesc == 0)
						return;

					break;
				}
#endif
				case WrPPPDist: 
				{
					Debug::trace("SessionWrapper", "WrPPPDist");
					HfcWrPPPDist *pHfcWrPPPDist = dynamic_cast<HfcWrPPPDist *> (msg->getHfcObject());	
					
					SessionMessage response(Wrapper, WrPPPDist);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(pHfcWrPPPDist);

					send(&response, Messenger::DEST_UI);
					break;
				}
				case WrPPPCalibration:
				{
					Debug::trace("SessionWrapper", "WrPPPCalibration");
					HfcWrPPPCal *pHfcWrPPPCal = dynamic_cast<HfcWrPPPCal *> (msg->getHfcObject());	
					
					SessionMessage response(Wrapper, WrPPPCalibration);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(pHfcWrPPPCal);

					send(&response, Messenger::DEST_UI);
					break;
				}
				case WrPPPConfig:
				{
					Debug::trace("SessionWrapper", "WrPPPConfig");
					HfcWrPPPConfig *pHfcWrPPPConfig = dynamic_cast<HfcWrPPPConfig *> (msg->getHfcObject());	
					
					SessionMessage response(Wrapper, WrPPPConfig);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(pHfcWrPPPConfig);

					send(&response, Messenger::DEST_UI);
					break;
				}
				case WrPPPRequest:
				{
					Debug::trace("SessionWrapper", "WrPPPRequest");
					HfcWrPPPReq *pHfcWrPPPReq = dynamic_cast<HfcWrPPPReq *> (msg->getHfcObject());	
					SessionMessage response(Wrapper, WrPPPRequest);
					response.setFormatType(InternalFormat);
					response.setHfcObject(pHfcWrPPPReq);
					send(&response, Messenger::DEST_RTC);
					break;
				}
				case WrPPPVersion: 
				{
					Debug::trace("SessionWrapper", "WrPPPVersion");
					HfcWrPPPVersion *pHfcWrPPPVersion = dynamic_cast<HfcWrPPPVersion *> (msg->getHfcObject());	
					
					SessionMessage response(Wrapper, WrPPPVersion);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(pHfcWrPPPVersion);

					send(&response, Messenger::DEST_UI);
					break;
				}
				case WrPPPStatus:
				{
					// SOF-5463: Error and Status Dialogs for NEW 2P Project with single sliding 2nd Printer
					// All of these previous errors are no longer needed so we should not even be firing the WrPPPStatus, 
					// unless it is decided to be used with the new 2P Project.  In any case, the following messages
					// can be reclaimed and used again
					// 9729, 9730, 9731, 9732, 9733, 9734, 9735, 9736, 9737, 9738, 9739, 9740, 9741, 9746

					Debug::trace("SessionWrapper", "WrPPPStatus");
					HfcWrPPPStatus *pHfcWrPPPStatus = dynamic_cast<HfcWrPPPStatus *> (msg->getHfcObject());	
					
					//if( HfcWrPPPStatus::PPPStatus::PRI_SENSOR_CAL_NEEDED == pHfcWrPPPStatus->getSensorStatus() ) {
					//	if (responseMessageManager) {						
					//		responseMessageManager->addMessage(ErrWrapper, 9729, HfcResponse::RESPONSESTYLE_OK);
					//	}
					//} else if( HfcWrPPPStatus::PPPStatus::SEC_SENSOR_CAL_NEEDED == pHfcWrPPPStatus->getSensorStatus() ) {
					//	if (responseMessageManager) {						
					//		responseMessageManager->addMessage(ErrWrapper, 9730, HfcResponse::RESPONSESTYLE_OK);
					//	}
					//} else if( HfcWrPPPStatus::PPPStatus::PRI_SENSOR_INTERNAL_CAL_FAILED == pHfcWrPPPStatus->getSensorStatus() ) {
					//	if (responseMessageManager) {						
					//		responseMessageManager->addMessage(ErrWrapper, 9731, HfcResponse::RESPONSESTYLE_OK);
					//	}
					//} else if( HfcWrPPPStatus::PPPStatus::SEC_SENSOR_INTERNAL_CAL_FAILED == pHfcWrPPPStatus->getSensorStatus() ) {
					//	if (responseMessageManager) {						
					//		responseMessageManager->addMessage(ErrWrapper, 9732, HfcResponse::RESPONSESTYLE_OK);
					//	}					
					//} else if( HfcWrPPPStatus::PPPStatus::PRI_SENSOR_MEASUREMENT_START_FAILURE == pHfcWrPPPStatus->getSensorStatus() ) {
					//	if (responseMessageManager) {						
					//		responseMessageManager->addMessage(ErrWrapper, 9733, HfcResponse::RESPONSESTYLE_OK);
					//	}					
					//} else if( HfcWrPPPStatus::PPPStatus::SEC_SENSOR_MEASUREMENT_START_FAILURE == pHfcWrPPPStatus->getSensorStatus() ) {
					//	if (responseMessageManager) {						
					//		responseMessageManager->addMessage(ErrWrapper, 9734, HfcResponse::RESPONSESTYLE_OK);
					//	}					
					//} else if( HfcWrPPPStatus::PPPStatus::SEC_SENSOR_ADDRESS_FAILURE == pHfcWrPPPStatus->getSensorStatus() ) {
					//	if (responseMessageManager) {						
					//		responseMessageManager->addMessage(ErrWrapper, 9735, HfcResponse::RESPONSESTYLE_OK);
					//	}
					//} else if( HfcWrPPPStatus::PPPStatus::BOTH_SENSORS_CAL_NEEDED == pHfcWrPPPStatus->getSensorStatus() ) {
					//	if (responseMessageManager) {						
					//		responseMessageManager->addMessage(ErrWrapper, 9737, HfcResponse::RESPONSESTYLE_OK);
					//	}
					//} else if( HfcWrPPPStatus::PPPStatus::SENSOR_COMM_FAILURE == pHfcWrPPPStatus->getSensorStatus() ) {
					//	if (responseMessageManager) {						
					//		responseMessageManager->addMessage(ErrWrapper, 9736, HfcResponse::RESPONSESTYLE_OK);
					//	}
					//} else if( HfcWrPPPStatus::PPPStatus::PRI_TEMP_SENSOR_CONFIG_FAILURE == pHfcWrPPPStatus->getSensorStatus() ) {
					//	if (responseMessageManager) {						
					//		responseMessageManager->addMessage(ErrWrapper, 9738, HfcResponse::RESPONSESTYLE_OK);
					//	}
					//}  else if( HfcWrPPPStatus::PPPStatus::PRI_TEMP_SENSOR_ADDRESS_FAILURE == pHfcWrPPPStatus->getSensorStatus() ) {
					//	if (responseMessageManager) {						
					//		responseMessageManager->addMessage(ErrWrapper, 9739, HfcResponse::RESPONSESTYLE_OK);
					//	}
					//}  else if( HfcWrPPPStatus::PPPStatus::SEC_TEMP_SENSOR_CONFIG_FAILURE == pHfcWrPPPStatus->getSensorStatus() ) {
					//	if (responseMessageManager) {						
					//		responseMessageManager->addMessage(ErrWrapper, 9740, HfcResponse::RESPONSESTYLE_OK);
					//	}
					//}  else if( HfcWrPPPStatus::PPPStatus::SEC_TEMP_SENSOR_ADDRESS_FAILURE == pHfcWrPPPStatus->getSensorStatus() ) {
					//	if (responseMessageManager) {						
					//		responseMessageManager->addMessage(ErrWrapper, 9741, HfcResponse::RESPONSESTYLE_OK);
					//	}
					//}
					break;
				}
				case WrPPPBootError:	//SOF-4638
				{
					// SOF-5463: Error and Status Dialogs for NEW 2P Project - No longer triggering printer plater errors
					//if (responseMessageManager) {						
					//	responseMessageManager->addMessage(ErrWrapper, 9746, HfcResponse::RESPONSESTYLE_OK);
					//}
						break;
				}
				case WrWrapperPause: 
				{
					// SOF-5459 Printer Plate Adjust Message - Do not need this for a single sliding 2nd printer so can it all
					Debug::trace("WrapperManager.PPPMsg", "handleWrapperMessage() case WrWrapperPause: prolly should not do this anymore");
					/*
					Debug::trace("SessionWrapper", "WrWrapperPause");
					HfcNGWWrapperPause *pHfcNGWWrapperPause = dynamic_cast<HfcNGWWrapperPause *> (msg->getHfcObject());	
					
					SessionMessage response(Wrapper, WrWrapperPause);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(pHfcNGWWrapperPause);

					send(&response, Messenger::DEST_UI);
					*/
					break;
				}
				case WrPPPResume: 
				{
					Debug::trace("SessionWrapper", "WrPPPResume");
					HfcNGWWrapperResume *pHfcNGWWrapperResume = dynamic_cast<HfcNGWWrapperResume *> (msg->getHfcObject());	

					long weight = getCurrentWeight()->getWeight();
					Debug::trace("SessionWrapper", HobString("Weight: ") + HobString(Long::toString(weight)));

					if( weight > 0 )
					{
						//SOF-4475
        				wghtDialogSet = true;
						responseMessageManager->addMessage( ErrWrapper, 9744, HfcResponse::RESPONSESTYLE_OK );
					}

					SessionMessage response(Wrapper, WrPPPResume);
					response.setFormatType(InternalFormat);
					response.setHfcObject(pHfcNGWWrapperResume);
					
					send(&response, Messenger::DEST_RTC);
					break;
				}				
				case WrEvolReqSystemReset: {	//SOF_4216
					Debug::trace("SessionWrapper", "WrEvolReqSystemReset");
					SessionMessage response(Wrapper, WrEvolReqSystemReset);
					response.setFormatType(InternalFormat);					
					send(&response, Messenger::DEST_RTC);
					break;
				}
				default:
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append("handleWrapperMessage() - Don't recognize this event: ").append(Integer::toString(msg->getEventType()));
						Debug::trace("SessionWrapper",msgDebug2);
					}
					break;
				}
			break;
		}

		default:
			break;
	}

	Debug::trace("SessionWrapper", "Session has finished processing a wrapper message.");
}

void Session::handleNGWrapperMessage(SessionMessage *msg)
{
	switch(msg->getEventType())
	{
		case _StartUpgrade:
		{
			Debug::trace("SessionNGWrapper", "Received:  _StartUpgrade");
			HfcNGWStartUpgrade *pHfcNGWStartUpgrade = dynamic_cast<HfcNGWStartUpgrade *> (msg->getHfcObject());

			SessionMessage response(NGWrapper, _StartUpgrade);
			response.setFormatType(BrowserFormat);
			response.setHfcObject(pHfcNGWStartUpgrade);

			send(&response, Messenger::DEST_UI);
			break;
		}
		case _UpgradeStatus:
		{
			Debug::trace("SessionNGWrapper", "Received:  _UpgradeStatus");
			HfcNGWUpgradeStatus *pHfcNGWUpgradeStatus = dynamic_cast<HfcNGWUpgradeStatus *> (msg->getHfcObject());

			SessionMessage response(NGWrapper, _UpgradeStatus);
			response.setFormatType(BrowserFormat);
			response.setHfcObject(pHfcNGWUpgradeStatus);

			send(&response, Messenger::DEST_UI);
			break;
		}
		case _UpgradeProgress:
		{
			Debug::trace("SessionNGWrapper", "Received:  _UpgradeProgress");
			HfcNGWUpgradeProgress *pHfcNGWUpgradeProgress = dynamic_cast<HfcNGWUpgradeProgress *> (msg->getHfcObject());

			SessionMessage response(NGWrapper, _UpgradeProgress);
			response.setFormatType(BrowserFormat);
			response.setHfcObject(pHfcNGWUpgradeProgress);

			send(&response, Messenger::DEST_UI);
			break;
		}
		case _UpgradeFinished:
		{
			Debug::trace("SessionNGWrapper", "Received:  _UpgradeFinished");
			HfcNGWUpgradeFinished *pHfcNGWUpgradeFinished = dynamic_cast<HfcNGWUpgradeFinished *> (msg->getHfcObject());

			SessionMessage response(NGWrapper, _UpgradeFinished);
			response.setFormatType(BrowserFormat);
			response.setHfcObject(pHfcNGWUpgradeFinished);

			send(&response, Messenger::DEST_UI);
			break;
		}
		case _UpgradeRequired:
		{
			Debug::trace("SessionNGWrapper", "Received:  _UpgradeRequired");
			HfcNGWUpgradeRequired *pHfcNGWUpgradeRequired = dynamic_cast<HfcNGWUpgradeRequired *> (msg->getHfcObject());

			SessionMessage response(NGWrapper, _UpgradeRequired);
			response.setFormatType(BrowserFormat);
			response.setHfcObject(pHfcNGWUpgradeRequired);

			send(&response, Messenger::DEST_UI);

			break;
		}
		case _UpgradeUIReady:
		{
			Debug::trace("SessionNGWrapper", "Received:  _UpgradeUIReady");
			msg->setFormatType(InternalFormat);
			send(msg, Messenger::DEST_RTC);
			break;
		}
		case _UpgradeCancel:
		{
			Debug::trace("SessionNGWrapper", "Received:  _UpgradeCancel");
			msg->setFormatType(InternalFormat);
			send(msg, Messenger::DEST_RTC);
			break;
		}
		case _ReqUpgradeInfo:
		{
			Debug::trace("SessionNGWrapper", "Received:  _ReqUpgradeInfo");
			msg->setFormatType(InternalFormat);
			send(msg, Messenger::DEST_RTC);
			break;
		}
		case _UpgradeInfo:
		{
			Debug::trace("SessionNGWrapper", "Received:  _UpgradeInfo");
			HfcNGWUpgradeInfo *pHfcNGWUpgradeInfo = dynamic_cast<HfcNGWUpgradeInfo *> (msg->getHfcObject());

			SessionMessage response(NGWrapper, _UpgradeInfo);
			response.setFormatType(BrowserFormat);
			response.setHfcObject(pHfcNGWUpgradeInfo);

			send(&response, Messenger::DEST_UI);
			break;
		}
		case _WrNGWScan:
		{
        	Debug::trace("SessionNGWrapper", "_WrNGWScan: Starting the System Start Manager thread!");

        	// start the system start manager
        	SystemStartManager *pStartManager( SystemStartManager::getInstance() );

        	if ( pStartManager->setModeToScan( false ) )
        	{
            	// stop the probe timer
            	RealTimeController::getInstance()->stopWrapperProbeTimer();
        	}

        	break;
		}
		case _WrNGWScanFinished:
		{
			Debug::trace("SessionNGWrapper", "_WrNGWScanFinished: Starting probe timer!");
			RealTimeController::getInstance()->startWrapperProbeTimer();

			break;
		}
		default:
			Debug::trace("SessionNGWrapper", "Received:  Unknown Message!");
			break;
	}
}

// Private method
void Session::sendPackageSizeToUI(HfcWrapperIntakePkgSize *packageSize)
{
	Debug::trace("SessionWrapper", "Wrapper Status: Package is present - send length & width!");
	SessionMessage settings(Wrapper, WrIntakePkgSize);
	settings.setFormatType(BrowserFormat);
	settings.setHfcObject(packageSize);

	if (Debug::isTracing("SessionWrapper"))
	{
		const char *msgdata = settings.toString();
		int msglen = settings.getSerializedLength();
		Debug::trace("SessionWrapper",  HobString("wrapper WrIntakePkgSize: ") + HobString(Hex::toString(msgdata, msglen)));
	}

	send(&settings, Messenger::DEST_UI);
}

// Private method
void Session::initAutoConfigSize()
{
	autoConfigSize.setMinLength(0);
	autoConfigSize.setMaxLength(0);
	autoConfigSize.setMinWidth(0);
	autoConfigSize.setMaxWidth(0);
}

// Private method
//void Session::sendPackCodeToUI(WrPackCode thisPackCode)
//{
//	//  Do we have a valid PackCode?
//	if ((thisPackCode >= PackCode0) && (thisPackCode <= PackCode11))
//	{
//		HfcWrapperConfig *wrapperConfig = configurationManager->getCurrentWrapperConfig();
//		if (wrapperConfig)
//		{
//			HfcWrapperPackCodeInfo *wrapperPackCode = new HfcWrapperPackCodeInfo(thisPackCode, wrapperConfig);
//			if (wrapperPackCode)
//			{
//				currentPackCode = thisPackCode;		 //	  Session needs to keep track of this.
//
//				SessionMessage myMessage(Wrapper, WrWritePackCode);
//				myMessage.setFormatType(BrowserFormat);
//				myMessage.setHfcObject(wrapperPackCode);
//
//				if (Debug::isTracing("SessionWrapper"))
//				{
//					const char *msgdata = myMessage.toString();
//					int msglen = myMessage.getSerializedLength();
//					Debug::trace("SessionWrapper",	  HobString("wrapper pack code: ") + HobString(Hex::toString(msgdata, msglen)));
//				}
//
//				send(&myMessage, Messenger::DEST_UI);
//				delete wrapperPackCode;
//			}
//		}
//	}
//}

// Private method
void Session::sendWrapperWrReqStatusMsg()
{
	SessionMessage request(Wrapper, WrReqStatus);
	request.setFormatType(InternalFormat);

	send(&request, Messenger::DEST_RTC);
}


void Session::setWrapperSealerBeltPower(bool turnOn, WrapperKind wrKind)
{
	Debug::trace("SessionWrapper", "setWrapperSealerBeltPower()");
	if (turnOn)
	{
		wrapperSealerBeltOn = true;
		wrapperSealerBeltOff = false;
	}
	else
	{
		wrapperSealerBeltOn = false;
		wrapperSealerBeltOff = true;
	}

	if (wrKind == WrUltimaWrapper)
	{
		sendWrapperSealerBeltMessage();
	}
	else if (wrKind == WrEvolWrapper)
	{
		//	  Don't make any changes here for the Evolution Wrapper until we get the new Configuration code in.
		SessionMessage request(Wrapper, WrReqSettings);
		request.setFormatType(InternalFormat);

		send(&request, Messenger::DEST_RTC);
	}
}

void Session::sendWrapperSealerBeltMessage()
{
	bool turnOn;
	int errorCode = 0;

	HfcWrapperControl *wrControl = configurationManager->getCurrentWrapperControl();
	if (!wrControl)
	{
		//	  Send a WrReqSettings message
		SessionMessage response(Wrapper, WrReqSettings);
		response.setFormatType(InternalFormat);

		send(&response, Messenger::DEST_RTC);
		Debug::trace("SessionWrapper", "Sending WrReqSettings message to RTC");
	}
	else
	{
		if ((!wrapperSealerBeltOn) && (!wrapperSealerBeltOff))
			return;

		if (wrapperSealerBeltOn)
		{
			turnOn = true;
			wrapperSealerBeltOn = false;
			errorCode = SessionDevTimedWrSealerBeltOn;
		}
		else
		{
			turnOn = false;
			wrapperSealerBeltOff = false;
			errorCode = SessionDevTimedWrSealerBeltOff;
		}
		Debug::trace("SessionWrapper", "sendWrapperSealerBeltMessage():  sealerBeltOn = " + HobString(Bool::toString(turnOn)));

		wrControl->setHeaterEnable(turnOn);
		wrControl->show();

		SessionMessage response(Wrapper, WrControl);
		response.setFormatType(InternalFormat);
		response.setHfcObject(wrControl);

		send(&response, Messenger::DEST_RTC);
		Debug::trace("SessionWrapper", "Sending WrControl message to RTC");
		if (responseMessageManager && (errorCode > 0))
			responseMessageManager->addMessage(ErrSessionDev, errorCode, HfcResponse::RESPONSESTYLE_OK);
	}
}


void Session::sendEvolWrapperSealerBeltMessage(SessionMessage *UNUSED(msg))
{
	bool turnOn;
	int errorCode = 0;

	HfcEvolWrapperControl *wrControl = configurationManager->getCurrentEvolWrapperControl();
	if (!wrControl)
	{
		//	  Send a WrReqSettings message
		SessionMessage response(Wrapper, WrReqSettings);
		response.setFormatType(InternalFormat);

		send(&response, Messenger::DEST_RTC);
		Debug::trace("SessionWrapper", "Sending WrReqSettings message to RTC");
	}
	else
	{
		if ((!wrapperSealerBeltOn) && (!wrapperSealerBeltOff))
			return;

		if (wrapperSealerBeltOn)
		{
			turnOn = true;
			wrapperSealerBeltOn = false;
			errorCode = SessionDevTimedWrSealerBeltOn;
		}
		else
		{
			turnOn = false;
			wrapperSealerBeltOff = false;
			errorCode = SessionDevTimedWrSealerBeltOff;
		}
		Debug::trace("SessionWrapper", "sendEvolWrapperSealerBeltMessage():  sealerBeltOn = " + HobString(Bool::toString(turnOn)));

		wrControl->setSealerBeltHeaterEnabled(turnOn);
		wrControl->show();

		SessionMessage response(Wrapper, WrEvolControl);
		response.setFormatType(InternalFormat);
		response.setHfcObject(wrControl);

		send(&response, Messenger::DEST_RTC);
		Debug::trace("SessionWrapper", "Sending WrEvolControl message to RTC");

		if (responseMessageManager && (errorCode > 0))
			responseMessageManager->addMessage(ErrSessionDev, errorCode, HfcResponse::RESPONSESTYLE_OK);
	}
}

void Session::sendWrapperSaveCycleCount()
{
	SessionMessage response(Wrapper, WrEvolSaveCycleCount);
	response.setFormatType(InternalFormat);

	send(&response, Messenger::DEST_RTC);
	Debug::trace("SessionWrapper", "Sending WrEvolSaveCycleCount message to RTC");
}

void Session::createEvolWrapperTimingsManager()
{
	if (backupRestoreManager)
	{
		Debug::trace("SessionWrapper", "new EvolWrapperTimingsManager with a backupRestoreManager.");
		evolWrapperTimingsManager_ = new EvolWrapperTimingsManager(this, true);
	}
	else
	{
		Debug::trace("SessionWrapper", "new EvolWrapperTimingsManager.");
		evolWrapperTimingsManager_ = new EvolWrapperTimingsManager(this);
	}

}


#ifdef TRACK_EVOL_WRAPPER_ERRORS
void Session::createEvolWrapperErrorManager()
{
	Debug::trace("SessionWrapper", "new EvolWrapperTimingsManager.");
	evolWrapperErrorManager_ = new EvolWrapperErrorManager(this);
}
#endif


void Session::setWrTimingsCompleteTimer()
{
	// Create a timer to be notified when this streaming graphic should be replaced with the next one.
	TimerManager *timerManager = TimerManager::getInstance();
	if (timerManager)
		timerManager->addTimer(getId(), 30, Timer::RepeatNO, timedFuncWrTimingsComplete);
	else
		Debug::trace("SessionWrapper", 20, "Could not add the WrTimingsCompleteTimer!!!");
}


void Session::cancelWrTimingsCompleteTimer()
{
	TimerManager *timerManager = TimerManager::getInstance();
	if (timerManager)
	{
		Timer *wrTimer = timerManager->getTimer(getId(), timedFuncWrTimingsComplete);
		if (wrTimer)
		{
			timerManager->removeTimer(wrTimer->getTimerId());

			Debug::trace("SessionWrapper", 20, "Removed the WrTimingsCompleteTimer");
		}
	}
}
//	handleWrapperRestoreMessage - To enable us to process (or not) the WrapperTimings from a restore.
//	Since we need to wait for a response from the wrapper when the timings are sent to it, the Session
//	needs to handle this message
void Session::handleWrapperRestoreMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	bool traceOn = Debug::isTracing("SessionWrapper");
	HobString msgDebug;

		if (traceOn)
		{
		msgDebug = "handleWrapperRestoreMessage():  ";

			HobString msgDebug2 = msgDebug;
		msgDebug2.append("device = ");
		if (msg->getDeviceType() == WrapperSystem)
			msgDebug2.append("WrapperSystem");
		else
			msgDebug2.append("Wrapper");
		msgDebug2.append("  event = ").append(Integer::toString(msg->getEventType()));
				Debug::trace("SessionWrapper", msgDebug2);
			}
	if (msg->getDeviceType() == Wrapper && msg->getEventType() == WrEvolRestorePackageTimings)
		handleWrapperMessage(msg);
}

void Session::setWrSystemInfoSavedTimer()
{
	Debug::trace("SessionWrapper", 20, "setWrSystemInfoSavedTimer()");

	// Create a timer to notify us when the wrapper has not responded to a system info saved message.
	TimerManager *timerManager = TimerManager::getInstance();
	if (timerManager)
		timerManager->addTimer(getId(), 30, Timer::RepeatNO, timedFuncWrSystemInfoSaved);
	else
		Debug::trace("SessionWrapper", 20, "Could not add the WrSystemInfoSavedTimer!!!");
}


void Session::cancelWrSystemInfoSavedTimer()
{
	Debug::trace("SessionWrapper", 20, "cancelWrSystemInfoSavedTimer()");

	TimerManager *timerManager = TimerManager::getInstance();
	if (timerManager)
	{
		Timer *wrTimer = timerManager->getTimer(getId(), timedFuncWrSystemInfoSaved);
		if (wrTimer)
		{
			timerManager->removeTimer(wrTimer->getTimerId());

			Debug::trace("SessionWrapper", 20, "Removed the WrSystemInfoSavedTimer");
		}
	}
}


void Session::abortWrSystemInfoMsgWait()
{
	Debug::trace("SessionWrapper", 20, "abortWrSystemInfoMsgWait()");

	HfcEvolWrapperSystemInfo *blankEvolWrapperSystemInfo = new HfcEvolWrapperSystemInfo();					

	SessionMessage response(Wrapper, WrEvolSystemInfo);
	response.setFormatType(BrowserFormat);
	response.setHfcObject(blankEvolWrapperSystemInfo);

	send(&response, Messenger::DEST_UI);
}


void Session::sendWrapperDataToHook(HfcEvolWrapperControl* wrapperControl)
{
	if (!wrapperControl)
	{
		Debug::trace("SessionWrapper.sendWrapperDataToHook", "Received a null HfcEvolWrapperControl object. Aborting hook action.");
		return;
	}

	Debug::trace("SessionWrapper.sendWrapperDataToHook", "Processing hook actions for HfcEvolWrapperControl object.");

	std::string data(std::string("SendWrapperData-topRollWidth-").append(std::to_string(wrapperControl->getUpperRollWidth())));
	performHookAction(data.c_str());

	data.assign(std::string("SendWrapperData-bottomRollWidth-").append(std::to_string(wrapperControl->getLowerRollWidth())));
	performHookAction(data.c_str());

	Debug::trace("SessionWrapper.sendWrapperDataToHook", "Finished processing hook actions for HfcEvolWrapperControl object.");

/*
	performHookAction(topRollWidth.c_str());
	performHookAction(bottomRollWidth.c_str());
*/
}

void Session::sendWrapperDataToHook(HfcEvolWrapperStatus* wrapperStatus)
{
	if (!wrapperStatus)
	{
		Debug::trace("SessionWrapper.sendWrapperDataToHook", "Received a null HfcEvolWrapperStatus object. Aborting hook action.");
		return;
	}

	Debug::trace("SessionWrapper.sendWrapperDataToHook", "Processing hook actions for HfcEvolWrapperStatus object.");

	EvolWrapperTemperatureInfo temperatureInfo = wrapperStatus->getTemperatureInfo();
	EvolWrapperStatusInfo statusInfo = wrapperStatus->getStatusInfo();

	std::string data(std::string("SendWrapperData-wrapperState-").append(std::to_string(statusInfo.getWrapperState())));
	performHookAction(data.c_str());

	data.assign(std::string("SendWrapperData-beltTemperatureSetting-").append(std::to_string(temperatureInfo.getSealerBeltSetting())));
	performHookAction(data.c_str());

	data.assign(std::string("SendWrapperData-beltTemperatureActual-").append(std::to_string(temperatureInfo.getSealerBeltTemp())));
	performHookAction(data.c_str());

	data.assign(std::string("SendWrapperData-roomTemperature-").append(std::to_string(temperatureInfo.getAmbientTemp())));
	performHookAction(data.c_str());

	Debug::trace("SessionWrapper.sendWrapperDataToHook", "Finished processing hook actions for HfcEvolWrapperStatus object.");

/*
	performHookAction(wrapperState.c_str());
	performHookAction(beltTemperatureSetting.c_str());
	performHookAction(beltTemperatureActual.c_str());
	performHookAction(roomTemperature.c_str());
*/
}

void Session::sendWrapperDataToHook(MtdNGWWrapperData* wrapperFlashData)
{
	if (!wrapperFlashData)
	{
		Debug::trace("SessionWrapper.sendWrapperDataToHook", "Received a null MtdNGWWrapperData object. Aborting hook action.");
		return;
	}

	Debug::trace("SessionWrapper.sendWrapperDataToHook", "Processing hook actions for MtdNGWWrapperData object (cycle count data).");

	std::string data(std::string("SendWrapperData-cycleCounts-").append(std::to_string(wrapperFlashData->getCycleCounts())));
	performHookAction(data.c_str());

	Debug::trace("SessionWrapper.sendWrapperDataToHook", "Finished processing hook actions for MtdNGWWrapperData object (cycle count data).");
}
