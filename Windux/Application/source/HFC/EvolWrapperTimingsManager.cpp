/* Evolution Scale Project
 * COPYRIGHT (c) 2003 - 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "EvolWrapperTimingsManager.h"

#include "Session.h"

#include "SessionMessage.h"
//#include "DispatchEvent.h"
#include "EvolWrapperRequestPackageTimings.h"
#include "EvolWrapperResetDefaultPackageTimings.h"
#include "EvolWrapperUnlockPackageTimings.h"
#include "HfcEvolWrapperTrayType.h"
#include "HfcEvolWrapperAllTrayTypes.h"
#include "HfcEvolWrapperChangeTrayTypePrecedence.h"
#include "HfcEvolWrapperWrapBox.h"
#include "HfcEvolWrapperHeightBox.h"
#include "HfcEvolWrapperRenameHeightBox.h"
#include "HfcEvolWrapperPackageDesc.h"
#include "HfcEvolWrapperPackageTimingsPointer.h"
#include "GenericHfcObject.h"
#include "ResponseMessageManager.h"
#include "HfcEvolWrapperTest.h"
#include "HfcEvolWrapperHeightBoxParam.h"
#include "HfcEvolWrapperAllHtBoxesOneParam.h"

#include "HfcPrinterPrintTestLabel.h"

#include "util/e_string/utilstring.h"
#include <string>

#include "HobString.h"
#include "Integer.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperTimingsManager::_dataType_ = 0;
std::string EvolWrapperTimingsManager::_dataTypeName_("EvolWrapperTimingsManager"); 
#endif


EvolWrapperTimingsManager::EvolWrapperTimingsManager(Session *session, bool forBackupRestore)
	: session_(session), currentViewMode_(ViewUnknown), cycleWrapperMode_(CycleWrapperUnknown),
	referencePrecedence_(_InvalidPrecedence_), updateTrayTypesToUI_(false), settingsChanged_(false),
	sendPermanentChanges_(false), wrapperIsFinished(true), reqCurrentWhenWrapperIsFinished_(false), reqDefaultWhenWrapperIsFinished_(false),
	exitWhenWrapperIsFinished(false), doDeleteMe(false), updateTrayTypesToWrMgr_(false), firstPass_(true), autoConfigFirstPass(true),
	lastWrapperTimings_(), saveWrapperTimings_(), backupRestore(forBackupRestore), eventWhenWrapperIsFinished(-1), lastSaveWasPermanent_(false),
	lastPackageDesc_()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	bool debugging = Debug::isTracing("EvolWrapperTimingsManager");
	if ( !_UsingWrapper_ )
	{
		if (debugging)
		{
			Debug::trace("EvolWrapperTimingsManager", "In EvolWrapperTimingsManager's constructor.");
			Debug::trace("EvolWrapperTimingsManager", "Going to generate dummy data.");
		}
		lastWrapperTimings_.generateDummyData();
		lastWrapperTimings_.show("");
	}
	if (debugging)
	{
		HobString msgDebug("Constructor:   ");
		msgDebug.append("this = ").append(util_string::ptrtostring(this));
		msgDebug.append("session_ = ").append(util_string::ptrtostring(session_));
		msgDebug.append("   forBackupRestore = ").append(Bool::toString(backupRestore)).append(" - Begin");
		Debug::trace("EvolWrapperTimingsManager", msgDebug);
	}
}


EvolWrapperTimingsManager::~EvolWrapperTimingsManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	bool debugging = Debug::isTracing("EvolWrapperTimingsManager");
	if (debugging)
	{
		HobString msgDebug("Destructor:  ");
		msgDebug.append("this = ").append(util_string::ptrtostring(this));
		msgDebug.append("session_ = ").append(util_string::ptrtostring(session_));
		msgDebug.append("   forBackupRestore = ").append(Bool::toString(backupRestore));
		Debug::trace("EvolWrapperTimingsManager", msgDebug);
	}
	clearWaitForWrapperMsg();

	// Update the wrapper if changes were made but the wrapper was not updated
	if (debugging)
		Debug::trace("EvolWrapperTimingsManager", "Destructor:  Calling sendPackageTimingsToWrapper(true)");
	sendPackageTimingsToWrapper(true);
	if (debugging)
		Debug::trace("EvolWrapperTimingsManager", "Destructor:  this = " + util_string::ptrtostring(this) + " - Done");
}


void EvolWrapperTimingsManager::handleMessage(SessionMessage *msg)
{
	bool debugging = Debug::isTracing("EvolWrapperTimingsManager");
	HobString msgDebug;
	if (debugging)
	{
		msgDebug = "handleMessage():   ";
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("device = ").append(Integer::toString(msg->getDeviceType()));
		msgDebug2.append("  event = ").append(Integer::toString(msg->getEventType()));
		msgDebug2.append("  cycleWrapperMode_ = ").append(Integer::toString(cycleWrapperMode_));
		Debug::trace("EvolWrapperTimingsManager", msgDebug2);
	}

	if ( msg && msg->getDeviceType() == Wrapper )
	{
		UINT8 event = msg->getEventType();

		switch ( event )
		{
			// -------------- PACKAGE TIMING MESSAGES --------------
			case WrEvolLoadPackageTimings:
			{
				if (debugging)
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolLoadPackageTimings from UI.");

				// The User Interface is getting ready to display the package timings info
				// on the screen, either because the service person entered the tray types
				// screen (Wrapper Package Timings screen) or because she changed between
				// the Manufacturing Default Timings and the Current Timings

				// Process the message
				// Determine whether to read current or default settings
				bool currentSettings = true;
				EvolWrapperRequestPackageTimings::TimingsType timingsType = EvolWrapperRequestPackageTimings::CurrentTimings;

				if (exitWhenWrapperIsFinished)
				{
					//	We thought we were done but we went right back into the page. Cancel the exit.
					exitWhenWrapperIsFinished = false;
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "WrEvolLoadPackageTimings:  exitWhenWrapperIsFinished = false");
				}

				GenericHfcObject* load = dynamic_cast<GenericHfcObject*>(msg->getHfcObject());
				if ( load && !load->getData().empty() )
					currentSettings = util_string::stringtobool(load->getData());

				if (!currentSettings)
				{
					timingsType = EvolWrapperRequestPackageTimings::DefaultTimings;
					if (debugging)
						Debug::trace("EvolWrapperTimingsManager", msgDebug + "timingsType = DefaultTimings");
				}

				// Set the View Mode: Current Settings vs. Manufacturing Default Settings
				currentViewMode_ = (timingsType == EvolWrapperRequestPackageTimings::DefaultTimings) ? ViewDefaultTimings : ViewCurrentTimings;
				if (debugging)
					Debug::trace("EvolWrapperTimingsManager", "WrEvolPackageTimings:  currentViewMode_ = " + Integer::toString(currentViewMode_) + ", firstPass_ = " + Bool::toString(firstPass_));

				if (!wrapperIsFinished)
				{
					if (currentViewMode_ == ViewDefaultTimings)
					{
						//	Wait for the wrapper to finish before we request this message.
						reqDefaultWhenWrapperIsFinished_ = true;
						if ( debugging )
							Debug::trace("EvolWrapperTimingsManager", "WrEvolPackageTimings:  reqDefaultWhenWrapperIsFinished_ = true.  Will request the Default PackageTimings when WrEvolSavePackageTimingsComplete is received.");
					}
				}

				//	Even if the wrapper is not finished saving the changes, we can still set up the information in ViewCurrentTimings mode.
				if ( !reqDefaultWhenWrapperIsFinished_ )
				{
					//	HA-191:  The only time we request timings from the wrapper is the first time we come to this page (firstPass_)
					//			 or when we need to display the Default timings. Any other time we can get the currently temporary or permanent
					//			 changes from saveWrapperTimings_
					if (!currentSettings || firstPass_)
					{
						// Now read the new package timings data from the wrapper
						requestPackageTimingsFromWrapper(timingsType);
						if (firstPass_)
							firstPass_ = false;
					}
					else
					{
						lastWrapperTimings_ = saveWrapperTimings_;
						if (debugging)
							Debug::trace("EvolWrapperTimingsManager", "WrEvolPackageTimings:  lastWrapperTimings_ = saveWrapperTimings_");
						sendAllTrayTypesToUI();
					}

					if ( !_UsingWrapper_ )
					{
						if (backupRestore)
							sendPackageTimingsToBackup();
						else
							sendAllTrayTypesToUI();
					}
				}
				break;
			}
			case WrEvolPackageTimings:
			{
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolPackageTimings from Wrapper");

				EvolWrapperPackageTimings *wrapperTimings = 0;

				HfcEvolWrapperPackageTimings *hfcWrPkgTimings = 0;
				HfcEvolWrapperPackageTimingsPointer* timingsPtrMsg = dynamic_cast<HfcEvolWrapperPackageTimingsPointer*>(msg->getHfcObject());
				if ( timingsPtrMsg )
				{
					wrapperTimings = timingsPtrMsg->getTimingsPointer();
					if (wrapperTimings)
					{
						hfcWrPkgTimings = new HfcEvolWrapperPackageTimings(wrapperTimings);
						/* removed -- race condition betwween Wrapper Manager and timings Manager.
						 * removed as per Randy Blankley
						delete wrapperTimings;				*/
					}
				}
				else
				{
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "timingsPtrMsg is null!");
				}

				if (hfcWrPkgTimings)
				{
#if 0
					if ( debugging )
						hfcWrPkgTimings->show("(((  EvolWrapperTimingsManager  )))");
#endif

					// Update our working copy of the wrapper timings
					lastWrapperTimings_ = *hfcWrPkgTimings;
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "WrEvolPackageTimings:  lastWrapperTimings_ = *hfcWrPkgTimings,  currentViewMode_ = " + Integer::toString(currentViewMode_) + ",  disposition = " + Integer::toString(lastWrapperTimings_.getDisposition()));
					/* removed -- race condition betwween Wrapper Manager and timings Manager.
					 * removed as per Randy Blankley.
					delete hfcWrPkgTimings; */			//	We no longer need this

#if 0
					if ( debugging )
						lastWrapperTimings_.show("+++  Our latest WrapperTimings  +++");
#endif

					//	HA-191:  The only time we save timings to saveWrapperTimings_ is when we receive the CURRENT wrapper timings
					//			 from the wrapper or after we save the timings to the wrapper. That way, we always have a current copy.
					if (currentViewMode_ == ViewCurrentTimings)
					{
						//	Save the current settings in case we need to restore
						//	them later to cancel changes
						saveWrapperTimings_ = lastWrapperTimings_;
						if (debugging)
							Debug::trace("EvolWrapperTimingsManager", "WrEvolPackageTimings - ViewCurrentTimings:  saveWrapperTimings_ = lastWrapperTimings_,  currentViewMode_ = " + Integer::toString(currentViewMode_));
					}

					// Update the User Interface or the BackupRestoreManager
					if (debugging)
					{
						HobString msgDebug;

						msgDebug.append("updateTrayTypesToWrMgr_ = ").append(Bool::toString(updateTrayTypesToWrMgr_));
						msgDebug.append("  backupRestore = ").append(Integer::toString(backupRestore));
						Debug::trace("EvolWrapperTimingsManager", msgDebug);
					}
					if (updateTrayTypesToWrMgr_)
					{
						updateTrayTypesToWrMgr_ = false;
						doDeleteMe = true;				//	Tray names got updated by requesting the timings again. We are done here.
					}
					else if (eventWhenWrapperIsFinished == WrEvolAutoConfigTrayType)
					{
						eventWhenWrapperIsFinished = -1;
						if ( debugging )
						{
							lastWrapperTimings_.show("+++  WrEvolPackageTimings:  Our latest WrapperTimings before AutoConfig  +++");
							Debug::trace("EvolWrapperTimingsManager", "Now we can finish processing AutoConfig.");
						}
						cycleWrapperAutoConfig();
					}
					else if (eventWhenWrapperIsFinished == WrEvolAutoDetectTrayType)
					{
						eventWhenWrapperIsFinished = -1;
						if ( debugging )
						{
							lastWrapperTimings_.show("+++  WrEvolPackageTimings:  Our latest WrapperTimings before AutoDetect  +++");
							Debug::trace("EvolWrapperTimingsManager", "Now we can finish processing AutoDetect.");
						}
						cycleWrapperAutoDetect();
					}
					else if (backupRestore)
					{
						sendPackageTimingsToBackup();
						doDeleteMe = true;				//	We are done here.
					}
					else
					{
						sendAllTrayTypesToUI();
					}
				}
				else
				{
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "hfcWrPkgTimings is null!");
				}
				break;
			}
			case WrEvolResetDefaultPackageTimings:
			{
				// The User Interface requests to reset wrapper timings to manufacturing defaults.
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolResetDefaultPackageTimings from UI.");


				// Send the message to the wrapper
				EvolWrapperResetDefaultPackageTimings resetMessage;

				int bytesSent = 0;
				if ( _UsingWrapper_ )
					bytesSent = session_->send(&resetMessage, Messenger::DEST_RTC);
				if ( bytesSent < 0 )
				{
					Debug::trace("EvolWrapperTimingsManager",
							"***** WrEvolResetDefaultPackageTimings MESSAGE COULD NOT BE SENT to RTC");
				}
				else
				{
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "Sending WrEvolResetDefaultPackageTimings to RTC");

					settingsChanged_ = false;
					sendPermanentChanges_ = false;
					
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "WrEvolResetDefaultPackageTimings: settingsChanged_ = false,  sendPermanentChanges_ = false");
					
					setupForWrapperIsFinished();
					reqCurrentWhenWrapperIsFinished_ = true;
					
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "WrEvolResetDefaultPackageTimings: reqCurrentWhenWrapperIsFinished_ = true");
				}
				break;
			}
			case WrEvolUnlockPackageTimings:
			{
				// The User Interface requests to reset wrapper timings to manufacturing defaults.
				if (debugging)
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolUnlockPackageTimings from UI.");

				// Send the message to the wrapper
				EvolWrapperUnlockPackageTimings unlockMessage;
				break;
			}
			case WrEvolDonePackageTimings:
			{
				// The User Interface reports to us that we are leaving the Wrapper Package Timings
				// screens. Update the wrapper if there have been changes since the last update.
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolDonePackageTimings from UI.");

				// Update the wrapper if changes were made but the wrapper was not updated
				sendPackageTimingsToWrapper(true);
				if (wrapperIsFinished)
				{
					doDeleteMe = true;
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "WrEvolDonePackageTimings:  doDeleteMe = true");
				}
				else
				{
					exitWhenWrapperIsFinished = true;
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "WrEvolDonePackageTimings:  exitWhenWrapperIsFinished = true");
				}
				break;
			}
			case WrEvolRestorePackageTimings:
			{
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolRestorePackageTimings from Backup/Restore.");

				EvolWrapperPackageTimings *wrapperTimings = 0;

				HfcEvolWrapperPackageTimings *hfcWrPkgTimings = 0;
				HfcEvolWrapperPackageTimingsPointer* timingsPtrMsg = dynamic_cast<HfcEvolWrapperPackageTimingsPointer*>(msg->getHfcObject());
				if ( timingsPtrMsg )
				{
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "timingsPtrMsg is NOT null");

					wrapperTimings = timingsPtrMsg->getTimingsPointer();
					if (wrapperTimings)
					{
						hfcWrPkgTimings = new HfcEvolWrapperPackageTimings(wrapperTimings);
						/* removed -- race condition betwween Wrapper Manager and timings Manager.
						 * removed as per Randy Blankley
						delete wrapperTimings; */
					}
				}
				else
				{
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "timingsPtrMsg is null!");
				}

				if (hfcWrPkgTimings)
				{
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "wrapperTimings is NOT null");

					// Update our working copy of the wrapper timings
					lastWrapperTimings_ = *hfcWrPkgTimings;
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "WrEvolRestorePackageTimings:  lastWrapperTimings_ = *hfcWrPkgTimings,  currentViewMode_ = " + Integer::toString(currentViewMode_));
					/* removed -- race condition betwween Wrapper Manager and timings Manager.
					 * removed as per Randy Blankley
					delete hfcWrPkgTimings; */			//	We no longer need this

#if 0
					if ( debugging )
						lastWrapperTimings_.show("+++  Our latest WrapperTimings after WrEvolRestorePackageTimings  +++");
#endif
					// Update the Wrapper
					currentViewMode_ = ViewCurrentTimings;
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "WrEvolRestorePackageTimings:  currentViewMode_ = ViewCurrentTimings");
					exitWhenWrapperIsFinished = true;
					sendPermanentChanges_ = true;
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "WrEvolRestorePackageTimings: settingsChanged_ = " + util_string::booltostring(settingsChanged_) + ",  sendPermanentChanges_ = true");
					sendPackageTimingsToWrapper(true);
				}
				break;
			}
			case WrEvolSavePackageTimingsComplete:
			{
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + " START Received WrEvolSavePackageTimingsComplete from Wrapper.");

				clearWaitForWrapperMsg();
		
				//	Were we waiting for the wrapper to be finished before exiting?
				if (exitWhenWrapperIsFinished)
				{
					doDeleteMe = true;
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "I can be deleted after processing this message.");
				}
				else if (reqCurrentWhenWrapperIsFinished_)
				{
					reqCurrentWhenWrapperIsFinished_ = false;

					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "Now calling requestPackageTimingsFromWrapper(CurrentTimings)");

					requestPackageTimingsFromWrapper(EvolWrapperRequestPackageTimings::CurrentTimings);
					if ( !_UsingWrapper_ )
						sendAllTrayTypesToUI();
				}
				else if (reqDefaultWhenWrapperIsFinished_)
				{
					reqDefaultWhenWrapperIsFinished_ = false;

					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "Now calling requestPackageTimingsFromWrapper(DefaultTimings)");

					requestPackageTimingsFromWrapper(EvolWrapperRequestPackageTimings::DefaultTimings);
					if ( !_UsingWrapper_ )
						sendAllTrayTypesToUI();
				}
				else if (eventWhenWrapperIsFinished == WrEvolAutoConfigTrayType)
				{
					eventWhenWrapperIsFinished = -1;
					if ( debugging )
					{
						lastWrapperTimings_.show("+++  WrEvolSavePackageTimingsComplete:  Our latest WrapperTimings before AutoConfig  +++");
						Debug::trace("EvolWrapperTimingsManager", "Now we can finish processing AutoConfig.");
					}
					cycleWrapperAutoConfig();
				}
				else if (eventWhenWrapperIsFinished == WrEvolAutoDetectTrayType)
				{
					eventWhenWrapperIsFinished = -1;
					if ( debugging )
					{
						lastWrapperTimings_.show("+++  WrEvolSavePackageTimingsComplete:  Our latest WrapperTimings before AutoDetect  +++");
						Debug::trace("EvolWrapperTimingsManager", "Now we can finish processing AutoDetect.");
					}
					cycleWrapperAutoDetect();
				}

				Debug::trace("EvolWrapperTimingsManager", msgDebug + "DONE Received WrEvolSavePackageTimingsComplete from Wrapper.");

				break;
			}


			// ---------------- TRAY TYPE MESSAGES ----------------
			case WrEvolAutoDetectTrayType:
			{
				// The User Interface requests the auto detection of a specific tray's tray type
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolAutoDetectTrayType from UI");

				// First update the wrapper if any changes have happened. Make them permanent in case a tray was deleted or added already.
				sendPackageTimingsToWrapper(true);

				if (wrapperIsFinished)
				{
					// Now tell the wrapper to take in package (tray) and report back its size
					// The function cycleWrapper will update the wrapper if there are changes
					cycleWrapperAutoDetect();
				}
				else
				{
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "Wait until wrapperIsFinished before starting AutoDetect.");
					eventWhenWrapperIsFinished = event;
				}

				break;
			}
			case WrEvolAutoConfigTrayType:
			{
				// The User Interface requests the auto configuration of a new tray type
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolAutoConfigTrayType from UI");

				// Process the message
				HfcEvolWrapperTrayType* trayType = dynamic_cast<HfcEvolWrapperTrayType*>(msg->getHfcObject());
				if ( trayType )
				{
					if ( debugging )
					{
						Debug::trace("EvolWrapperTimingsManager", "Going to auto config a new tray type with name [" + trayType->getName() + "]");
					}

					if ( cycleWrapperMode_ != CycleWrapperAutoConfig )
					{
						//	If this is the first time the button is pressed, first
						//	update the wrapper if any changes have happened.
						//	Make them permanent in case a tray was deleted or added already.
						sendPackageTimingsToWrapper(true);

						// Save the new tray name until we get a wrap box to copy from
						newTrayName = trayType->getName();
					}
					if (wrapperIsFinished)
						cycleWrapperAutoConfig();
					else
					{
						if ( debugging )
							Debug::trace("EvolWrapperTimingsManager", "Wait until wrapperIsFinished before starting AutoConfig.");
						eventWhenWrapperIsFinished = event;
					}
				}
				break;
			}
			case WrEvolCreateTrayType:
			{
				// The User Interface requests the creation of a new tray type
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolCreateTrayType from UI");


				// Process the message
				HfcEvolWrapperTrayType* trayType = dynamic_cast<HfcEvolWrapperTrayType*>(msg->getHfcObject());
				if ( trayType )
				{
					// First update the wrapper in case any changes have happened.
					sendPackageTimingsToWrapper();

					int precedence = trayType->getPrecedence();
					const string& name = trayType->getName();
					int exception = trayType->getPLUException();
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "WrEvolCreateTrayType:  precedence = " + util_string::inttostring(precedence));

					// Find the reference tray type
					const WrapBox* referenceWrapBox = lastWrapperTimings_.getWrapBox(precedence);
					if ( referenceWrapBox )
					{
						// Save current settings in case we need to restore them later to cancel changes
						saveWrapperTimings_ = lastWrapperTimings_;


						// Create new tray type
						WrapBox newWrapBox(*referenceWrapBox);

						newWrapBox.setName(name);
						newWrapBox.setPLUException(exception);

						// Insert the new tray type before its reference wrapBox
						settingsChanged_ |= lastWrapperTimings_.insertWrapBox(newWrapBox, precedence);
						if ( debugging )
							Debug::trace("EvolWrapperTimingsManager", "WrEvolCreateTrayType: settingsChanged_ = " + util_string::booltostring(settingsChanged_) + ",  sendPermanentChanges_ = " + util_string::booltostring(sendPermanentChanges_));

						// Update User Interface: the new wrap box will have the precedence of its
						// reference wrap box since it was inserted right before (also, we are
						// implicitly calling for the shortest height box)
						sendWrapBoxToUI(precedence, newWrapBox);

						// Also update the tray types for when we return to that screen
						updateTrayTypesToUI_ = true;
					}
				}

				break;
			}
			case WrEvolChangeTrayTypePrecedence:
			{
				// The User Interface requests a change in the precedence of a tray type
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolChangeTrayTypePrecedence from UI");

				// Process the message
				HfcEvolWrapperChangeTrayTypePrecedence* changePrecedenceMessage =
						dynamic_cast<HfcEvolWrapperChangeTrayTypePrecedence*>(msg->getHfcObject());
				if ( changePrecedenceMessage )
				{
					int precedence = changePrecedenceMessage->getPrecedence();
					int delta = changePrecedenceMessage->getDelta();
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "WrEvolChangeTrayTypePrecedence:  precedence = " + util_string::inttostring(precedence));

					if ( lastWrapperTimings_.changeTrayTypePrecedence(precedence, delta) )
					{
						settingsChanged_ = true;
						if ( debugging )
							Debug::trace("EvolWrapperTimingsManager", "WrEvolChangeTrayTypePrecedence: settingsChanged_ = true,  sendPermanentChanges_ = " + util_string::booltostring(sendPermanentChanges_));

						// We are not changing screens so update tray types
						sendAllTrayTypesToUI();
					}
					else
						Debug::trace("EvolWrapperTimingsManager", "***** Unable to change precedence");
				}

				break;
			}
			case WrEvolWriteTrayType:
			{
				// The User Interface requests the update of a tray type
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolWriteTrayType from UI");


				// Process the message
				HfcEvolWrapperTrayType* trayType = dynamic_cast<HfcEvolWrapperTrayType*>(msg->getHfcObject());
				if ( trayType )
				{
					int precedence = trayType->getPrecedence();
					const string& name = trayType->getName();
					int exception = trayType->getPLUException();
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "WrEvolWriteTrayType:  precedence = " + util_string::inttostring(precedence));

					settingsChanged_ |= lastWrapperTimings_.updateTrayType(precedence, name, exception);
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "WrEvolWriteTrayType: settingsChanged_ = " + util_string::booltostring(settingsChanged_) + ",  sendPermanentChanges_ = " + util_string::booltostring(sendPermanentChanges_));

					// We are not changing screens so update tray types
					sendAllTrayTypesToUI();
				}

				break;
			}
			case WrEvolDeleteTrayType:
			{
				// The User Interface requests the deletion of a tray type
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolDeleteTrayType from UI");

				// Process the message
				GenericHfcObject* deleteMessage = dynamic_cast<GenericHfcObject*>(msg->getHfcObject());
				if ( deleteMessage && !deleteMessage->getData().empty() )
				{
					int precedence = util_string::stringtoint(deleteMessage->getData());
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "WrEvolDeleteTrayType:  precedence = " + util_string::inttostring(precedence));

					if ( validateDeletingTrayType(precedence) )
					{
						lastWrapperTimings_.deleteTrayType(precedence);

						settingsChanged_ = true;
						if ( debugging )
							Debug::trace("EvolWrapperTimingsManager", "WrEvolDeleteTrayType: settingsChanged_ = true,  sendPermanentChanges_ = " + util_string::booltostring(sendPermanentChanges_));

						// We are not changing screens so update tray types
						sendAllTrayTypesToUI();
					}
					else
					{
						Debug::trace("EvolWrapperTimingsManager",
								"***** Not allowed to delete tray type " + util_string::inttostring(precedence));
					}
				}

				break;
			}

			// ---------------- WRAP BOX MESSAGES -----------------
			case WrEvolReadWrapBox:
			{
				// The User Interface requests the edition of a wrap box
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "case WrEvolReadWrapBox(240): Received WrEvolReadWrapBox from UI");


				// Process the message
				GenericHfcObject* editMessage = dynamic_cast<GenericHfcObject*>(msg->getHfcObject());
				if ( editMessage && !editMessage->getData().empty() )
				{
					int precedence = util_string::stringtoint(editMessage->getData());
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "case WrEvolReadWrapBox(240):  precedence = " + util_string::inttostring(precedence));

					// Retrieve Wrap Box and send it to the UI
					const WrapBox* wrapBox = lastWrapperTimings_.getWrapBox(precedence);
					if ( wrapBox )
					{
 						// we are implicitly calling for the shortest height box
 						sendWrapBoxToUI(precedence, *wrapBox);

                        // me either, so I deleted it
					}
					else
						Debug::trace("EvolWrapperTimingsManager", "case WrEvolReadWrapBox(240): Could not find wrap box " +
								util_string::inttostring(precedence));
				}
				break;
			}
			case WrEvolWriteWrapBox:
			{
				// The User Interface requests the update of a wrap box
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "handleMessage() case WrEvolWriteWrapBox(241): Received WrEvolWriteWrapBox from UI");

				// Process the message
				HfcEvolWrapperWrapBox* wrapBoxMessage = dynamic_cast<HfcEvolWrapperWrapBox*>(msg->getHfcObject());
				if ( wrapBoxMessage && wrapBoxMessage->isValid() )
				{
					bool nameOrPLUChanged = false;
					const WrapBox* wrapBox = lastWrapperTimings_.getWrapBox(wrapBoxMessage->getPrecedence());
					if ( wrapBox )
						nameOrPLUChanged = wrapBoxMessage->nameOrPLUChanged(*wrapBox);

					if ( debugging )
					{
						Debug::trace("EvolWrapperTimingsManager", "handleMessage() case WrEvolWriteWrapBox(241): Updating tray type " +
								util_string::inttostring(wrapBoxMessage->getPrecedence()) + "'s wrap box with:");
						wrapBoxMessage->show("EvolWrapperTimingsManager");
						if ( nameOrPLUChanged )
							Debug::trace("EvolWrapperTimingsManager", "Name or PLU changed");
					}

					settingsChanged_ |= lastWrapperTimings_.updateWrapBox(*wrapBoxMessage);
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "handleMessage() case WrEvolWriteWrapBox(241): settingsChanged_ = " + util_string::booltostring(settingsChanged_) + ",  sendPermanentChanges_ = " + util_string::booltostring(sendPermanentChanges_));

					if ( nameOrPLUChanged )
					{
						// Also update the tray types for when we return to that screen
						updateTrayTypesToUI_ = true;
					}
				}

				break;
			}
			case WrEvolPrintWrapBox:
			{
				// The User Interface requests the print wrap box parameters
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolPrintWrapBox from UI");


				// Process the message
				GenericHfcObject* printMessage = dynamic_cast<GenericHfcObject*>(msg->getHfcObject());
				if ( printMessage && !printMessage->getData().empty() )
				{
					if ( debugging )
					{
						int precedence = util_string::stringtoint(printMessage->getData());
						Debug::trace("EvolWrapperTimingsManager", "WrEvolPrintWrapBox:  precedence = " + util_string::inttostring(precedence));
						Debug::trace("EvolWrapperTimingsManager", "Going to print tray type " +
								util_string::inttostring(precedence) + "'s wrap box parameters");
					}
//					print wrap box
				}

				break;
			}
			case WrEvolCancelWrapBoxChanges:
			{
				// The User Interface requests the cancel any wrap box changes made
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolCancelWrapBoxChanges from UI");

				// Process the message
				GenericHfcObject* cancelMessage = dynamic_cast<GenericHfcObject*>(msg->getHfcObject());
				if ( cancelMessage && !cancelMessage->getData().empty() )
				{
					if ( debugging )
					{
						int precedence = util_string::stringtoint(cancelMessage->getData());
						Debug::trace("EvolWrapperTimingsManager", "WrEvolCancelWrapBoxChanges:  precedence = " + util_string::inttostring(precedence));
						Debug::trace("EvolWrapperTimingsManager", "Going to cancel changes made to tray type " +
								util_string::inttostring(precedence) + "'s wrap box");
						Debug::trace("EvolWrapperTimingsManager", "currentViewMode_ = " + util_string::inttostring(currentViewMode_));
					}

					// Restored settings saved when we started editing timings
					if ( currentViewMode_ == ViewCurrentTimings )
					{
						lastWrapperTimings_ = saveWrapperTimings_;
						if ( debugging )
							Debug::trace("EvolWrapperTimingsManager", "WrEvolWriteWrapBox:  lastWrapperTimings_ = saveWrapperTimings_,  currentViewMode_ = " + Integer::toString(currentViewMode_));
						settingsChanged_ = false;
						if ( debugging )
							Debug::trace("EvolWrapperTimingsManager", "WrEvolWriteWrapBox: settingsChanged_ = false,  sendPermanentChanges_ = " + util_string::booltostring(sendPermanentChanges_));
					}
					else
					{
						if ( debugging )
							Debug::trace("EvolWrapperTimingsManager", "WrEvolCancelWrapBoxChanges:  currentViewMode_ != ViewCurrentTimings,  settingsChanged_ = true???");
					}
				}

				if ( updateTrayTypesToUI_ )
				{
					sendAllTrayTypesToUI();

					updateTrayTypesToUI_ = false;
				}

				// Ensure that we are not in any cycle wrapper mode
				cycleWrapperMode_ = CycleWrapperUnknown;

				autoConfigFirstPass = true;

				break;
			}
			case WrEvolDoneWrapBox:
			{
				// The User Interface informs the completion of changes to a wrap box
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "handleMessage() case WrEvolDoneWrapBox(244): Received WrEvolDoneWrapBox from UI.  cycleWrapperMode_ = " + util_string::inttostring(cycleWrapperMode_));

				if ( cycleWrapperMode_ == CycleWrapperAutoConfig )
				{
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "handleMessage() case WrEvolDoneWrapBox(244): Exiting CycleWrapperAutoConfig.");
					sendPackageTimingsToWrapper();

					updateTrayTypesToUI_ = true;
					cycleWrapperMode_ = CycleWrapperUnknown;
				}
				else
				if ( currentViewMode_ == ViewCurrentTimings )
				{
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "handleMessage() case WrEvolDoneWrapBox(244): currentViewMode_ == ViewCurrentTimings");
					sendPackageTimingsToWrapper();
				}

				if ( updateTrayTypesToUI_ )
				{
					sendAllTrayTypesToUI();

					updateTrayTypesToUI_ = false;
				}

				// Ensure that we are not in any cycle wrapper mode
			   //	cycleWrapperMode_ = CycleWrapperUnknown;

				autoConfigFirstPass = true;

				break;
			}

			// --------------- HEIGHT BOX MESSAGES ----------------
			case WrEvolReadHeightBox:
			{
				// The User Interface requests to read a height box
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "handleMessage() case WrEvolReadHeightBox(246): from UI");


				// Process the message
				HfcEvolWrapperHeightBox* heightBox = dynamic_cast<HfcEvolWrapperHeightBox*>(msg->getHfcObject());
				if ( heightBox )
				{
					int precedence = heightBox->getPrecedence();
					int height = heightBox->getHeight();
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "handleMessage() case WrEvolReadHeightBox(246):  precedence = " + util_string::inttostring(precedence));

					// Find the wrap box
					const WrapBox* wrapBox = lastWrapperTimings_.getWrapBox(precedence);
					if ( wrapBox )
					{
						// Find the height box
						const WrapBox::HeightBox* heightBox = wrapBox->getHeightBox(height);
						if ( heightBox )
						{
							sendHeightBoxToUI(precedence, *heightBox);
						}
						else
							Debug::trace("EvolWrapperTimingsManager", "handleMessage() case WrEvolReadHeightBox(246): ***** Unable to find heightBox " +
									util_string::inttostring(height) + " of wrapBox " + util_string::inttostring(precedence));
					}
					else
						Debug::trace("EvolWrapperTimingsManager", "handleMessage() case WrEvolReadHeightBox(246): Could not find wrap box " +
								util_string::inttostring(precedence));
				}

				break;
			}
			case WrEvolWriteHeightBox:
			{
				// The User Interface requests to change a height box
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "handleMessage() case WrEvolWriteHeightBox(247): from UI");

				// Process the message
				HfcEvolWrapperHeightBox* heightBox = dynamic_cast<HfcEvolWrapperHeightBox*>(msg->getHfcObject());
				if ( heightBox )
				{
					int precedence = heightBox->getPrecedence();
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "handleMessage() case WrEvolWriteHeightBox(247):  precedence = " + util_string::inttostring(precedence));
					
					const WrapBox* wrapBox = lastWrapperTimings_.getWrapBox(precedence);
					if ( wrapBox )
					{
						HobString myDebug;
						if ( debugging )
						{
							myDebug.append("Tray type ").append(util_string::inttostring(precedence));
							myDebug.append("'s height box ").append(util_string::inttostring(heightBox->getHeight()));
							Debug::trace("EvolWrapperTimingsManager", "Updating " + myDebug);
						}

						bool hbChanged = lastWrapperTimings_.updateHeightBox(*heightBox);
						if ( debugging )
						{
							myDebug.append(" did ");
							if ( !hbChanged )
								myDebug.append(" NOT ");
							myDebug.append("change.");
							Debug::trace("EvolWrapperTimingsManager", myDebug);
						}
						settingsChanged_ |= hbChanged;
						if ( debugging )
							Debug::trace("EvolWrapperTimingsManager", "WrEvolWriteHeightBox:  settingsChanged_ = " + util_string::booltostring(settingsChanged_) + ",  sendPermanentChanges_ = " + util_string::booltostring(sendPermanentChanges_));
					}
					else
						Debug::trace("EvolWrapperTimingsManager", " case WrEvolWriteHeightBox(247): Could not find wrap box " +
								util_string::inttostring(precedence));
				}

				break;
			}
			case WrEvolRenameHeightBox:
			{
				// The User Interface requests to change the height associated with a particular
				// height box
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "case WrEvolRenameHeightBox(248): Received WrEvolRenameHeightBox from UI");


				// Process the message
				HfcEvolWrapperRenameHeightBox* renameHeightBox = dynamic_cast<HfcEvolWrapperRenameHeightBox*>(msg->getHfcObject());
				if ( renameHeightBox )
				{
					int precedence = renameHeightBox->getPrecedence();
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "case WrEvolRenameHeightBox(248): precedence = " + util_string::inttostring(precedence));
					unsigned int oldHeight = renameHeightBox->getOldHeight();
					unsigned int newHeight = renameHeightBox->getNewHeight();
					WrapBox* wrapBox = lastWrapperTimings_.getWrapBox(precedence);
					if ( wrapBox )
					{

						const WrapBox::HeightBox* oldHeightBox = wrapBox->getHeightBox(oldHeight);
						const WrapBox::HeightBox* newHeightBox = wrapBox->getHeightBox(newHeight);
						if ( oldHeightBox )
						{
							if ( !newHeightBox )
							{
								if ( wrapBox->renameHeightBox(oldHeight, newHeight) )
								{
									settingsChanged_ = true;
									if ( debugging )
										Debug::trace("EvolWrapperTimingsManager", "case WrEvolRenameHeightBox(248): settingsChanged_ = true,  sendPermanentChanges_ = " + util_string::booltostring(sendPermanentChanges_));

			 						sendWrapBoxToUI(precedence, *wrapBox, newHeight);
								}
							}
							else
							{
								Debug::trace("EvolWrapperTimingsManager", "case WrEvolRenameHeightBox(248): There is already a height box with that height " +
										util_string::inttostring(newHeight));

//								send pop up error
							}
						}
						else
							Debug::trace("EvolWrapperTimingsManager", "case WrEvolRenameHeightBox(248): Could not find reference height box " +
									util_string::inttostring(oldHeight));
					}
					else
						Debug::trace("EvolWrapperTimingsManager", "case WrEvolRenameHeightBox(248): Could not find wrap box " +
								util_string::inttostring(precedence));
				}

				break;
			}
			case WrEvolCreateHeightBox:
			{
				// The User Interface requests to create a height box
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolCreateHeightBox from UI");


				// Process the message
				HfcEvolWrapperHeightBox* heightBox = dynamic_cast<HfcEvolWrapperHeightBox*>(msg->getHfcObject());
				if ( heightBox )
				{
					int precedence = heightBox->getPrecedence();
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "case WrEvolCreateHeightBox(249): precedence = " + util_string::inttostring(precedence));
					unsigned int height = heightBox->getHeight();
					unsigned int newHeight = heightBox->getNewHeight();
					WrapBox* wrapBox = lastWrapperTimings_.getWrapBox(precedence);
					if ( wrapBox )
					{
						if ( debugging )
						{
							Debug::trace("EvolWrapperTimingsManager", "case WrEvolCreateHeightBox(249): Creating new height box " +
									util_string::inttostring(newHeight) + " based on height box " +
									util_string::inttostring(height) + " for tray type " + util_string::inttostring(precedence));
						}

						// Determine the next height box and create a new one with the same timings
						// but the new height. Add it to the wrap box
						const WrapBox::HeightBox* referenceBox = wrapBox->getContainingHeightBox(height);
						if ( referenceBox )
						{
							if ( referenceBox->getHeight() != newHeight )
							{
								WrapBox::HeightBox newHeightBox(newHeight, *referenceBox);
								if ( debugging )
								{
									Debug::trace("EvolWrapperTimingsManager", "*******  Here is the new height box   **********");
									newHeightBox.show();
									Debug::trace("EvolWrapperTimingsManager", "************************************************");
								}

								settingsChanged_ |= wrapBox->insertHeightBox(newHeightBox);
								if ( debugging )
									Debug::trace("EvolWrapperTimingsManager", "case WrEvolCreateHeightBox(249):  settingsChanged_ = " + util_string::booltostring(settingsChanged_) + ",  sendPermanentChanges_ = " + util_string::booltostring(sendPermanentChanges_));

								// Now send the newly created height box to the UI
		 						sendWrapBoxToUI(precedence, *wrapBox, newHeight);
							}
							else
								sendHeightBoxToUI(precedence, *referenceBox);
						}
						else
							Debug::trace("EvolWrapperTimingsManager", "case WrEvolCreateHeightBox(249): Couldn't find height box to contain height " +
									util_string::inttostring(height) + " in wrap box " + util_string::inttostring(precedence));
					}
					else
						Debug::trace("EvolWrapperTimingsManager", "case WrEvolCreateHeightBox(249): Could not find wrap box " +
								util_string::inttostring(precedence));
				}

				break;
			}
			case WrEvolDeleteHeightBox:
			{
				// The User Interface requests to create a height box
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "case WrEvolDeleteHeightBox(251): Received WrEvolDeleteHeightBox from UI");


				// Process the message
				HfcEvolWrapperHeightBox* heightBox = dynamic_cast<HfcEvolWrapperHeightBox*>(msg->getHfcObject());
				if ( heightBox )
				{
					int precedence = heightBox->getPrecedence();
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "case WrEvolDeleteHeightBox(251):  precedence = " + util_string::inttostring(precedence));
					unsigned int height = heightBox->getHeight();

					WrapBox* wrapBox = lastWrapperTimings_.getWrapBox(precedence);
					if ( wrapBox )
					{
						if ( validateDeletingHeightBox(*wrapBox, height) )
						{
							settingsChanged_ |= wrapBox->removeHeightBox(height);
							if ( debugging )
								Debug::trace("EvolWrapperTimingsManager", "case WrEvolDeleteHeightBox(251): settingsChanged_ = " + util_string::booltostring(settingsChanged_) + ",  sendPermanentChanges_ = " + util_string::booltostring(sendPermanentChanges_));

	 						sendWrapBoxToUI(precedence, *wrapBox, height);
						}
						else
						{
							Debug::trace("EvolWrapperTimingsManager", "***** Not allowed to delete tray type " +
									util_string::inttostring(precedence) + "'s height box " + util_string::inttostring(height));

//							send popup eror
						}
					}
					else
						Debug::trace("EvolWrapperTimingsManager", "case WrEvolDeleteHeightBox(251): Could not find wrap box " +
								util_string::inttostring(precedence));
				}

				break;
			}
			case WrEvolVerifyInHeightBox:
			{
				// The User Interface requests the verify whether a specific tray is wrapped
				// as a specific tray type and with a specific height box
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "case WrEvolVerifyInHeightBox(250): Received WrEvolVerifyInHeightBox from UI");


				// Process the message
				HfcEvolWrapperHeightBox* verifyMessage = dynamic_cast<HfcEvolWrapperHeightBox*>(msg->getHfcObject());
				if ( verifyMessage )
				{
					int precedence = verifyMessage->getPrecedence();
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "case WrEvolVerifyInHeightBox(250):  precedence = " + util_string::inttostring(precedence));
					unsigned int height = verifyMessage->getHeight();
					// Verify that we have a tray type for the given precedence
					const WrapBox* wrapBox = lastWrapperTimings_.getWrapBox(precedence);
					if ( wrapBox )
					{
						// Now tell the wrapper to take in package (tray) and report back its size
						// The function cycleWrapper will update the wrapper if there are changes
						cycleWrapperVerify(precedence, height);
					}
					else
						Debug::trace("EvolWrapperTimingsManager", "case WrEvolVerifyInHeightBox(250): Could not find wrap box " +
								util_string::inttostring(precedence));
				}

				break;
			}

			// -------------- PACKAGE SIZE MESSAGES ---------------
			case WrEvolPackageDesc:
			{
				// The wrapper is sending us the package size data
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolPackageDesc from Wrapper");


				// Process message
				HfcEvolWrapperPackageDesc* packageSize = dynamic_cast<HfcEvolWrapperPackageDesc*>(msg->getHfcObject());
				if ( packageSize )
				{
					// Update our working copy of the wrapper timings
					lastPackageDesc_ = *packageSize;

					if ( debugging )
					{
						Debug::trace("EvolWrapperTimingsManager", "Show HfcEvolWrapperPackageDesc START");
						packageSize->show();
						Debug::trace("EvolWrapperTimingsManager", "Show HfcEvolWrapperPackageDesc  END");
					}
					int precedence = packageSize->getWrapBoxIndex();
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "WrEvolPackageDesc:  precedence = " + util_string::inttostring(precedence));
					int height = packageSize->getHeightBoxIndex();

					if ( debugging )
					{
						Debug::trace("EvolWrapperTimingsManager", "The tray 'wrapped' was identified as type " +
								util_string::inttostring(precedence) + " and height " + util_string::inttostring(height) +
								". The reference tray type was " + util_string::inttostring(referencePrecedence_));
					}

					switch ( cycleWrapperMode_ )
					{
						case CycleWrapperAutoDetect:
						{
							if (debugging)
								Debug::trace("EvolWrapperTimingsManager", "WrEvolPackageDesc / CycleWrapperAutoDetect");

							WrapBox* wrapBox = lastWrapperTimings_.getWrapBox(precedence);
							if ( wrapBox )
							{
								sendWrapBoxToUI(precedence, *wrapBox, wrapBox->getHeightBoxHeight(height));

								// Save current settings in case we need to restore them later to cancel changes
								saveWrapperTimings_ = lastWrapperTimings_;
								if ( debugging )
									Debug::trace("EvolWrapperTimingsManager", "CycleWrapperAutoDetect:  saveWrapperTimings_ = lastWrapperTimings_,  currentViewMode_ = " + Integer::toString(currentViewMode_));
							}

							cycleWrapperMode_ = CycleWrapperUnknown;

							break;
						}
						case CycleWrapperAutoConfig:
						{
							WrapBox* wrapBox = 0;
							int setting;
							HobString msgDebug2("");
							if (autoConfigFirstPass)
							{
								if ( debugging )
								{
									msgDebug2 = "WrEvolPackageDesc / CycleWrapperAutoConfig / First pass";
									Debug::trace("EvolWrapperTimingsManager", msgDebug2);
									msgDebug2.append(":  ");
								}

								//	Create the new Wrap box from the wrap box it closely matches.
								const WrapBox* refWrapBox = lastWrapperTimings_.getWrapBox(precedence);
								if (refWrapBox)
								{
									WrapBox newWrapBox(*refWrapBox);

									newWrapBox.setName(newTrayName);

									setMinAndMaxTrayValues(newWrapBox);

									settingsChanged_ |= lastWrapperTimings_.insertWrapBox(newWrapBox, precedence);
									if ( debugging )
										Debug::trace("EvolWrapperTimingsManager", "CycleWrapperAutoConfig: settingsChanged_ = " + util_string::booltostring(settingsChanged_) + ",  sendPermanentChanges_ = " + util_string::booltostring(sendPermanentChanges_));

									wrapBox = lastWrapperTimings_.getWrapBox(precedence);
									if (debugging)
									{
										Debug::trace("EvolWrapperTimingsManager", "New tray type [" +
												newWrapBox.getName() + "], based on tray " + refWrapBox->getName() +
												".  Inserted at precedence = " + util_string::inttostring(precedence) + ".");
									}

									if (wrapBox)
										wrapBox->show("EvolWrapperTimingsManager.Clare");

									referencePrecedence_ = precedence;
									if ( debugging )
										Debug::trace("EvolWrapperTimingsManager", "CycleWrapperAutoConfig:  referencePrecedence_ = " + util_string::inttostring(referencePrecedence_));

									autoConfigFirstPass = false;
								}
								else
								{
									//	We have a problem here!
									if ( debugging )
										Debug::trace("EvolWrapperTimingsManager", "CycleWrapperAutoConfig, first pass:  no wrapBox found based on precedence!");
								}
							}
							else
							{
								if ( debugging )
									msgDebug2 = "WrEvolPackageDesc - CycleWrapperAutoConfig - next pass:  ";

								wrapBox = lastWrapperTimings_.getWrapBox(referencePrecedence_);
								if ( wrapBox )
								{
									//	check MinTrayLength
									setting = getMinTrayLengthValue(packageSize->getLength());
									if (setting < wrapBox->getLengthLow())
									{
										wrapBox->setLengthLow((UINT16) setting);
										if (debugging)
										{
											HobString msgDebug3 = msgDebug2;
											msgDebug3.append("Changing MinTrayLength to ").append(Integer::toString(wrapBox->getLengthLow()));
											Debug::trace("EvolWrapperTimingsManager", msgDebug3);
										}
									}

									//	check MaxTrayLength
									setting = getMaxTrayLengthValue(packageSize->getLength());
									if (setting > wrapBox->getLengthHigh())
									{
										wrapBox->setLengthHigh((UINT16) setting);
										if (debugging)
										{
											HobString msgDebug3 = msgDebug2;
											msgDebug3.append("Changing MaxTrayLength to ").append(Integer::toString(wrapBox->getLengthHigh()));
											Debug::trace("EvolWrapperTimingsManager", msgDebug3);
										}
									}

									//	check MinTrayWidth
									setting = getMinTrayWidthValue(packageSize->getWidth());
									if (setting < wrapBox->getWidthLow())
									{
										wrapBox->setWidthLow((UINT16) setting);
										if (debugging)
										{
											HobString msgDebug3 = msgDebug2;
											msgDebug3.append("Changing MinTrayWidth to ").append(Integer::toString(wrapBox->getWidthLow()));
											Debug::trace("EvolWrapperTimingsManager", msgDebug3);
										}
									}

									//	check MaxTrayWidth
									setting = getMaxTrayWidthValue(packageSize->getWidth());
									if (setting > wrapBox->getWidthHigh())
									{
										wrapBox->setWidthHigh((UINT16) setting);
										if (debugging)
										{
											HobString msgDebug3 = msgDebug2;
											msgDebug3.append("Changing MaxTrayWidth to ").append(Integer::toString(wrapBox->getWidthHigh()));
											Debug::trace("EvolWrapperTimingsManager", msgDebug3);
										}
									}
#if 0
//	Per Gary Cornish and Kraig Wooddell (Jan. 2010). They no longer want this for the tray lip.
									//	set MaxTrayLip
									setting = getMaxTrayLipValue(packageSize->getLip());
									if (setting > wrapBox->getLip())
										wrapBox->setLip((UINT16) setting);

#endif
								}
							}
							if (wrapBox)
							{
								if (debugging)
								{
									HobString msgDebug3 = msgDebug2;
									msgDebug3.append("Length:   ").append(Integer::toString(wrapBox->getLengthLow()));
									msgDebug3.append(" <= ").append(Integer::toString(packageSize->getLength()));
									msgDebug3.append(" <= ").append(Integer::toString(wrapBox->getLengthHigh()));
									Debug::trace("EvolWrapperTimingsManager", msgDebug3);

									msgDebug3 = msgDebug2;
									msgDebug3.append("Width :   ").append(Integer::toString(wrapBox->getWidthLow()));
									msgDebug3.append(" <= ").append(Integer::toString(packageSize->getWidth()));
									msgDebug3.append(" <= ").append(Integer::toString(wrapBox->getWidthHigh()));
									Debug::trace("EvolWrapperTimingsManager", msgDebug3);

									msgDebug3 = msgDebug2;
									msgDebug3.append("Lip :   ").append(Integer::toString(packageSize->getLip()));
									msgDebug3.append(" <= ").append(Integer::toString(wrapBox->getLip()));
									Debug::trace("EvolWrapperTimingsManager", msgDebug3);
								}
								sendWrapBoxToUI(referencePrecedence_, *wrapBox, wrapBox->getHeightBoxHeight(height));
							}
							else
							{
								Debug::trace("EvolWrapperTimingsManager", "CycleWrapperAutoConfig:  no wrapBox found!");
							}
							break;
						}
						case CycleWrapperVerify:
						{
							// If the identified tray is different from the reference tray, report error
							if (debugging)
								Debug::trace("EvolWrapperTimingsManager", "WrEvolPackageDesc / CycleWrapperVerify:  precedence = " + util_string::inttostring(precedence) + ", referencePrecedence_ = " + util_string::inttostring(referencePrecedence_));
							if (precedence != referencePrecedence_)
							{
								ResponseMessageManager *myResponseMessageManager = session_ ? session_->getResponseMessageManager() : 0;
								if (myResponseMessageManager)
								{
									Debug::trace("EvolWrapperTimingsManager", "CycleWrapperVerify:  Sending message 9716 to the ResponseMessageManager.");
									myResponseMessageManager->addMessage(ErrWrapper, 9716, HfcResponse::RESPONSESTYLE_OK);
								}
								else
								{
									Debug::trace("EvolWrapperTimingsManager", "CycleWrapperVerify:  Error!!! myResponseMessageManager is NULL!");
								}
							}
							break;
						}
						case CycleWrapperUnknown:
						{
							if (debugging)
								Debug::trace("EvolWrapperTimingsManager", "cycleWrapperMode_ = CycleWrapperUnknown, what do we do now?");

							break;
						}
						default:
						{

							if (debugging)
								Debug::trace("EvolWrapperTimingsManager", "cycleWrapperMode_ = default, what do we do now?");
							break;
						}
					}
				}

				break;
			}

			// -------------- CHANGE ONE ATTRIBUTE FOR ALL HEIGHT BOXES ---------------
			case WrEvolReadAllForAttribute:
			{
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolReadAllForAttribute from UI");

				// Process the message
				HfcEvolWrapperHeightBoxParam* attributeMessage = dynamic_cast<HfcEvolWrapperHeightBoxParam*>(msg->getHfcObject());
				if ( attributeMessage )
				{
					HfcEvolWrapperAllHtBoxesOneParam allHtBoxesOneParam(lastWrapperTimings_, attributeMessage->getPrecedence(), attributeMessage->getAttribute());

					Debug::trace("EvolWrapperTimingsManager", "*********    Show HfcEvolWrapperAllHtBoxesOneParam    ********");
					allHtBoxesOneParam.show();
					Debug::trace("EvolWrapperTimingsManager", "*********     End HfcEvolWrapperAllHtBoxesOneParam    ********");

					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "Sending WrEvolWriteAllForAttribute to the UI");
					SessionMessage msgToSend(Wrapper, WrEvolWriteAllForAttribute);
					msgToSend.setFormatType(BrowserFormat);
					msgToSend.setHfcObject(&allHtBoxesOneParam);

					session_->send(&msgToSend, Messenger::DEST_UI);
				}
				else
				{
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "attributeMessage is NULL!");
				}
				break;
			}

			case WrEvolSaveAttributeChanges:
			{
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolSaveAttribute from UI");

				// Process the message
				HfcEvolWrapperHeightBoxParam* attributeMessage = dynamic_cast<HfcEvolWrapperHeightBoxParam*>(msg->getHfcObject());
				if ( attributeMessage )
				{
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "Updating tray type " + util_string::inttostring(attributeMessage->getPrecedence()) +
							", height box " + util_string::inttostring(attributeMessage->getHeight()) +
							", attribute " + util_string::inttostring(attributeMessage->getAttribute()) +
							" with " + util_string::inttostring(attributeMessage->getAttributeValue()));

					settingsChanged_ |= lastWrapperTimings_.updateHeightBoxParam(*attributeMessage);
					if ( debugging )
						Debug::trace("EvolWrapperTimingsManager", "WrEvolSaveAttributeChanges: settingsChanged_ = " + util_string::booltostring(settingsChanged_) + ",  sendPermanentChanges_ = " + util_string::booltostring(sendPermanentChanges_));
				}
				break;
			}

			case WrEvolCancelAttributeChanges:
			{
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolCancelAttributeChanges from UI");

				lastWrapperTimings_ = saveWrapperTimings_;
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", "WrEvolCancelAttributeChanges:  lastWrapperTimings_ = saveWrapperTimings_,  currentViewMode_ = " + Integer::toString(currentViewMode_));
				settingsChanged_ = false;
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", "WrEvolCancelAttributeChanges:  Latest changes not recorded.  settingsChanged_ = false,  sendPermanentChanges_ = " + util_string::booltostring(sendPermanentChanges_));
				break;
			}

			case WrEvolDoneAttributeChanges:
			{
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolDoneAttributeChanges from UI");

				if ( currentViewMode_ == ViewCurrentTimings )
				{
					Debug::trace("EvolWrapperTimingsManager", "WrEvolDoneAttributeChanges: currentViewMode_ == ViewCurrentTimings.  sendPackageTimingsToWrapper()");
					sendPackageTimingsToWrapper();
				}
				break;
			}

			case WrEvolSetMinTrayDims:
			{
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolSetMinTrayDims from UI");

				HfcEvolWrapperWrapBox* wbm = dynamic_cast<HfcEvolWrapperWrapBox*>(msg->getHfcObject());
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "WrEvolSetMinTrayDims:  wbm = " + util_string::ptrtostring(wbm));
				if (wbm)
				{
					WrapBox* refWrapBox = lastWrapperTimings_.getWrapBox(wbm->getPrecedence());
					if ( debugging )
					{
						Debug::trace("EvolWrapperTimingsManager", msgDebug + "WrEvolSetMinTrayDims:  precedence = " + util_string::inttostring(wbm->getPrecedence()) + ",  refWrapBox = " + util_string::ptrtostring(refWrapBox));
						if (refWrapBox)
							refWrapBox->show("EvolWrapperTimingsManager");
					}
					if (refWrapBox)
					{
						settingsChanged_ |= setMinTrayValues(*refWrapBox);
						if ( debugging )
							Debug::trace("EvolWrapperTimingsManager", msgDebug + "WrEvolSetMinTrayDims:  settingsChanged_ = " + util_string::booltostring(settingsChanged_) + ",  sendPermanentChanges_ = " + util_string::booltostring(sendPermanentChanges_));
						sendWrapBoxToUI(wbm->getPrecedence(), *refWrapBox);
					}
				}
				break;
			}

			case WrEvolSetMaxTrayDims:
			{
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolSetMaxTrayDims from UI");

				HfcEvolWrapperWrapBox* wbm = dynamic_cast<HfcEvolWrapperWrapBox*>(msg->getHfcObject());
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "WrEvolSetMaxTrayDims:  wbm = " + util_string::ptrtostring(wbm));
				if (wbm)
				{
					WrapBox* refWrapBox = lastWrapperTimings_.getWrapBox(wbm->getPrecedence());
					if ( debugging )
					{
						Debug::trace("EvolWrapperTimingsManager", msgDebug + "WrEvolSetMaxTrayDims:  precedence = " + util_string::inttostring(wbm->getPrecedence()) + ",  refWrapBox = " + util_string::ptrtostring(refWrapBox));
						if (refWrapBox)
							refWrapBox->show("EvolWrapperTimingsManager");
					}
					if (refWrapBox)
					{
						settingsChanged_ |= setMaxTrayValues(*refWrapBox);
						if ( debugging )
							Debug::trace("EvolWrapperTimingsManager", msgDebug + "WrEvolSetMaxTrayDims:  settingsChanged_ = " + util_string::booltostring(settingsChanged_) + ",  sendPermanentChanges_ = " + util_string::booltostring(sendPermanentChanges_));
						sendWrapBoxToUI(wbm->getPrecedence(), *refWrapBox);
					}
				}
				break;
			}

			case WrEvolSetMinMaxTrayDims:
			{
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "Received WrEvolSetMinMaxTrayDims from UI");

				HfcEvolWrapperWrapBox* wbm = dynamic_cast<HfcEvolWrapperWrapBox*>(msg->getHfcObject());
				if ( debugging )
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "WrEvolSetMinMaxTrayDims:  wbm = " + util_string::ptrtostring(wbm));
				if (wbm)
				{
					WrapBox* refWrapBox = lastWrapperTimings_.getWrapBox(wbm->getPrecedence());
					if ( debugging )
					{
						Debug::trace("EvolWrapperTimingsManager", msgDebug + "WrEvolSetMinMaxTrayDims:  precedence = " + util_string::inttostring(wbm->getPrecedence()) + ",  refWrapBox = " + util_string::ptrtostring(refWrapBox));
						if (refWrapBox)
							refWrapBox->show("EvolWrapperTimingsManager");
					}
					if (refWrapBox)
					{
						settingsChanged_ |= setMinAndMaxTrayValues(*refWrapBox);
						if ( debugging )
							Debug::trace("EvolWrapperTimingsManager", msgDebug + "WrEvolSetMinMaxTrayDims:  settingsChanged_ = " + util_string::booltostring(settingsChanged_) + ",  sendPermanentChanges_ = " + util_string::booltostring(sendPermanentChanges_));
						sendWrapBoxToUI(wbm->getPrecedence(), *refWrapBox);
					}
				}
				break;
			}

			default:
			{
				Debug::trace("EvolWrapperTimingsManager", msgDebug + "***** Received a Wrapper message, event " +
						util_string::inttostring(event) + ", that is not being processed.");

				break;
			}
		}
	}
	else
	{
		Debug::trace("EvolWrapperTimingsManager", msgDebug + "***** Received a message, device " +
				util_string::inttostring(msg->getDeviceType()) + ", event " + util_string::inttostring(msg->getEventType()) +
				", that is not being processed.");
	}
}


void EvolWrapperTimingsManager::requestPackageTimingsFromWrapper(EvolWrapperRequestPackageTimings::TimingsType timingsType)
{
	bool traceOn = Debug::isTracing("EvolWrapperTimingsManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "requestPackageTimingsFromWrapper(";
		msgDebug.append(Integer::toString(timingsType)).append(")");
		Debug::trace("EvolWrapperTimingsManager", msgDebug);
		msgDebug.append(":  ");
	}

	if ( _UsingWrapper_ )
	{
		// Register with the Session to receive the reply to our request
		session_->registerEvent(Wrapper, WrEvolPackageTimings, false);
		if (traceOn)
			Debug::trace("EvolWrapperTimingsManager", msgDebug + "Registered for WrEvolPackageTimings msg.");
	}
	

	// Setup request message and send it to RTC
	EvolWrapperRequestPackageTimings reqTimingsMsg(timingsType);

	if ( _UsingWrapper_ )
		session_->send(&reqTimingsMsg, Messenger::DEST_RTC);
}


void EvolWrapperTimingsManager::sendAllTrayTypesToUI()
{
	Debug::trace("EvolWrapperTimingsManager", "sendAllTrayTypesToUI()");

	HfcEvolWrapperAllTrayTypes trayTypes(lastWrapperTimings_);

	SessionMessage trayTypesMessage(Wrapper, WrEvolWriteAllTrayTypes);
	trayTypesMessage.setFormatType(BrowserFormat);
	trayTypesMessage.setHfcObject(&trayTypes);

	int bytesSent = session_->send(&trayTypesMessage, Messenger::DEST_UI);
	if ( bytesSent < 0 )
		Debug::trace("EvolWrapperTimingsManager", "***** Wrapper, WrEvolWriteAllTrayTypes COULD NOT BE SENT");
}


void EvolWrapperTimingsManager::sendPackageTimingsToWrapper(bool makePermanent)
{
	if (!_UsingWrapper_)
		return;

	bool traceOn = Debug::isTracing("EvolWrapperTimingsManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "sendPackageTimingsToWrapper():   ";
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("currentViewMode_ = ").append(Integer::toString(currentViewMode_));
		msgDebug2.append("   settingsChanged_ = ").append(util_string::booltostring(settingsChanged_));
		msgDebug2.append("   sendPermanentChanges_ = ").append(Bool::toString(sendPermanentChanges_));
		msgDebug2.append("   makePermanent = ").append(Bool::toString(makePermanent));
		msgDebug2.append("   wrapperIsFinished = ").append(Bool::toString(wrapperIsFinished));
		Debug::trace("EvolWrapperTimingsManager", msgDebug2);
	}
	lastSaveWasPermanent_ = false;

	if ( currentViewMode_ == ViewCurrentTimings && wrapperIsFinished && (settingsChanged_ || (makePermanent && sendPermanentChanges_)))
	{
		EvolWrapperPackageTimings *ewpt = new EvolWrapperPackageTimings((EvolWrapperPackageTimings*) &lastWrapperTimings_);
		if (ewpt)
		{
			ewpt->setDisposition((makePermanent && (settingsChanged_ || sendPermanentChanges_)) ? WRITE_PERMANENT : WRITE_TEMPORARY);
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Disposition = ");
				if (ewpt->getDisposition() == WRITE_PERMANENT)
					msgDebug2.append("WRITE_PERMANENT");
				else
					msgDebug2.append("WRITE_TEMPORARY");
				Debug::trace("EvolWrapperTimingsManager", msgDebug2);
			}
			HfcEvolWrapperPackageTimingsPointer wtPtr(WrEvolSavePackageTimings, (EvolWrapperPackageTimings *) ewpt);

			SessionMessage timingsMessage(Wrapper, WrEvolSavePackageTimings);
			timingsMessage.setFormatType(InternalFormat);
			timingsMessage.setHfcObject(&wtPtr);

			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Sending updated package timings to RTC. # of Wrap boxes = ").append(Integer::toString(ewpt->getNumberWrapBoxes()));
				Debug::trace("EvolWrapperTimingsManager", msgDebug2);
			}

			int bytesSent = 0;
			if ( _UsingWrapper_ )
				bytesSent = session_->send(&timingsMessage, Messenger::DEST_RTC);

			if ( bytesSent < 0 )
			{
				if (traceOn)
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "***** WrEvolPackageTimings MESSAGE COULD NOT BE SENT to RTC");
			}
			else
			{
				if (makePermanent)
				{
					lastSaveWasPermanent_ = true;
					sendPermanentChanges_ = false;
					if (traceOn)
						Debug::trace("EvolWrapperTimingsManager", msgDebug + "  lastSaveWasPermanent_ = true,  sendPermanentChanges_ = false");
				}
				else
				{
					sendPermanentChanges_ |= settingsChanged_;
					if (traceOn)
						Debug::trace("EvolWrapperTimingsManager", msgDebug + "  1) settingsChanged_ = " + util_string::booltostring(settingsChanged_) + ",  sendPermanentChanges_ = " + util_string::booltostring(sendPermanentChanges_));
				}
				//	HA-191:  Any time we save timings, whether Temporary or Permanent, we copy those timings to saveWrapperTimings_
				// Save current settings in case we need to restore
				// them later to cancel changes
				saveWrapperTimings_ = lastWrapperTimings_;
				if (traceOn)
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "saveWrapperTimings_ = lastWrapperTimings_,  currentViewMode_ = " + Integer::toString(currentViewMode_));

				settingsChanged_ = false;
				if (traceOn)
					Debug::trace("EvolWrapperTimingsManager", msgDebug + "  2) settingsChanged_ = " + util_string::booltostring(settingsChanged_) + ",  sendPermanentChanges_ = " + util_string::booltostring(sendPermanentChanges_));

				setupForWrapperIsFinished();
			}
		}
	}
	else if (traceOn)
	{
		Debug::trace("EvolWrapperTimingsManager", msgDebug + "NOT sending updated package timings to RTC!");
	}
}


void EvolWrapperTimingsManager::sendWrapBoxToUI(int precedence, const WrapBox& wrapBox, unsigned int height)
{

	Debug::trace("EvolWrapperTimingsManager",
			"sendWrapBoxToUI():   precedence = " + util_string::inttostring(precedence) + "   height = " + util_string::uinttostring(height));

	HfcEvolWrapperWrapBox hfcWrapBox(precedence, wrapBox);

	SessionMessage wrapBoxMessage(Wrapper, WrEvolWriteWrapBox);
	wrapBoxMessage.setFormatType(BrowserFormat);
	wrapBoxMessage.setHfcObject(&hfcWrapBox);

	Debug::trace("EvolWrapperTimingsManager", "Sending Wrap Box to UI");
	hfcWrapBox.show();

	int bytesSent = session_->send(&wrapBoxMessage, Messenger::DEST_UI);
	if ( bytesSent < 0 )
		Debug::trace("EvolWrapperTimingsManager", "Wrapper, WrEvolWriteWrapBox COULD NOT BE SENT");

	if (cycleWrapperMode_ != CycleWrapperAutoConfig)
	{
		//	If we are not in AutoConfig mode, Retrieve the Height Box requested and send it to the UI
		const WrapBox::HeightBox* heightBox = wrapBox.getContainingHeightBox(height);
		if ( heightBox )
			sendHeightBoxToUI(precedence, *heightBox);
		else
			Debug::trace("EvolWrapperTimingsManager",
					"Could not find shortest height box for wrap box " + util_string::inttostring(precedence));
	}
}


void EvolWrapperTimingsManager::sendHeightBoxToUI(int precedence, const WrapBox::HeightBox& heightBox)
{
	HfcEvolWrapperHeightBox hfcHeightBox(precedence, heightBox);

	SessionMessage heightBoxMessage(Wrapper, WrEvolWriteHeightBox);
	heightBoxMessage.setFormatType(BrowserFormat);
	heightBoxMessage.setHfcObject(&hfcHeightBox);

	Debug::trace("EvolWrapperTimingsManager", "Sending Height Box to UI");
	hfcHeightBox.show();

	int bytesSent = session_->send(&heightBoxMessage, Messenger::DEST_UI);
	if ( bytesSent < 0 )
	{
		Debug::trace("EvolWrapperTimingsManager",
				"Wrapper, WrEvolWriteHeightBox COULD NOT BE SENT");
	}
}


void EvolWrapperTimingsManager::cycleWrapperAutoDetect()
{
	// The UI requested that we cycle the wrapper in order to determine the tray
	// type and size of the tray placed on the wrapper's intake
	
	// Register the Session to receive the reply to our request
	if ( _UsingWrapper_ ) {
		Debug::trace("EvolWrapperTimingsManager", " registerEvent PrTestLabel" );
		session_->registerEvent(Printer, PrTestLabel);
		session_->registerEvent(Wrapper, WrEvolPackageDesc, false);

	}
	// Prepare and send the wrap message
	SessionMessage cycleMessage(Wrapper, WrEvolTest);
	cycleMessage.setFormatType(InternalFormat);

	HfcEvolWrapperTest wrap(0, EvolWrapperTest::WrapperCycle);
	wrap.setUseLongParam();

	cycleMessage.setHfcObject(&wrap);

	if ( _UsingWrapper_ )
		session_->send(&cycleMessage, Messenger::DEST_RTC);

	cycleWrapperMode_ = CycleWrapperAutoDetect;
}


void EvolWrapperTimingsManager::cycleWrapperAutoConfig()
{
	// The UI requested that we cycle the wrapper in order to size 
	// the tray placed on the wrapper's intake with the purpose of
	// configuring a new tray type

	// We will not update the wrapper with any changes for the new wrap box


	// Register the Session to receive the reply to our request
	if ( _UsingWrapper_ )
		session_->registerEvent(Wrapper, WrEvolPackageDesc, false);
	// Prepare and send the wrap message
	SessionMessage cycleMessage(Wrapper, WrEvolTest);
	cycleMessage.setFormatType(InternalFormat);

	HfcEvolWrapperTest wrap(0, EvolWrapperTest::WrapperCycle);
	wrap.setUseLongParam();

	cycleMessage.setHfcObject(&wrap);

	if ( _UsingWrapper_ )
		session_->send(&cycleMessage, Messenger::DEST_RTC);

	cycleWrapperMode_ = CycleWrapperAutoConfig;
}


void EvolWrapperTimingsManager::cycleWrapperVerify(int precedence, unsigned int height)
{
	// Register the Session to receive the reply to our request
	if ( _UsingWrapper_ )
		session_->registerEvent(Wrapper, WrEvolPackageDesc, false);

	// Prepare and send the wrap message
	SessionMessage cycleMessage(Wrapper, WrEvolWrap);
	cycleMessage.setFormatType(InternalFormat);

	if ( _UsingWrapper_ )
		session_->send(&cycleMessage, Messenger::DEST_RTC);
	referencePrecedence_ = precedence;
	Debug::trace("EvolWrapperTimingsManager", "cycleWrapperVerify():  referencePrecedence_ = precedence = " + util_string::inttostring(referencePrecedence_));
	referenceHeight_ = height;
	cycleWrapperMode_ = CycleWrapperVerify;
}


bool EvolWrapperTimingsManager::timingsMatchViewMode(const HfcEvolWrapperPackageTimings& UNUSED(timings))
{
	bool result = false;

	switch ( currentViewMode_ )
	{
		case ViewCurrentTimings:
		{
//			if ( timings.getDisposition() == 0)		//	WrPermanent
				result = true;

			break;
		}
		case ViewDefaultTimings:
		{
//			if ( timings.getDisposition() == 2)		//	WrDefault
				result = true;

			break;
		}
		default:
			break;
	}
	if (Debug::isTracing("EvolWrapperTimingsManager"))
	{
		HobString msgDebug("timingsMatchViewMode() -   ");

		msgDebug.append("currentViewMode_ = ").append(Integer::toString(currentViewMode_));
#if 0
		msgDebug.append("  timings.getDisposition() = ").append(Integer::toString(timings.getDisposition()));
#endif
		msgDebug.append("  result = ").append(Bool::toString(result));
		Debug::trace("EvolWrapperTimingsManager", msgDebug);
	}
	return result;
}


bool EvolWrapperTimingsManager::validateDeletingTrayType(int precedence)
{
	Debug::trace("EvolWrapperTimingsManager", "validateDeletingTrayType(" + util_string::inttostring(precedence) + ")");
	int wrapBoxes = lastWrapperTimings_.getNumberWrapBoxes();

	// Only prevent the deletion of the last two wrap boxes, which are "Dflt Wide" and "Dflt Narr".
	return (0 <= precedence && precedence < wrapBoxes - 2);
}


bool EvolWrapperTimingsManager::validateDeletingHeightBox(const WrapBox& wrapBox, unsigned int height)
{
	Debug::trace("EvolWrapperTimingsManager", "validateDeletingHeightBox: " + util_string::uinttostring(height) + "  for WrapBox " + wrapBox.getName() + ".");
	const WrapBox::HeightBox* heightBox = wrapBox.getHeightBox(height);

	if ( heightBox )
		return height < WrapBox::_MaxHeight_;

	return false;
}


void EvolWrapperTimingsManager::setupForWrapperIsFinished()
{
	Debug::trace("EvolWrapperTimingsManager", "setupForWrapperIsFinished()");

	wrapperIsFinished = false;
	ResponseMessageManager *myResponseMessageManager = session_ ? session_->getResponseMessageManager() : 0;
	if (myResponseMessageManager)
	{
		Debug::trace("EvolWrapperTimingsManager", "setupForWrapperIsFinished():  Sending message 9705 to the ResponseMessageManager.");
		myResponseMessageManager->addMessage(ErrWrapper, 9705, HfcResponse::RESPONSESTYLE_NOINPUT);
	}
	else
	{
		Debug::trace("EvolWrapperTimingsManager", "setupForWrapperIsFinished():  Error!!! myResponseMessageManager is NULL!");
	}
	session_->registerEvent(Wrapper, WrEvolSavePackageTimingsComplete, false);
	session_->setWrTimingsCompleteTimer();
}


void EvolWrapperTimingsManager::clearWaitForWrapperMsg()
{
	if (session_)
	{
		session_->cancelWrTimingsCompleteTimer();	//	HA-107:  Cancel the timer regardless. If it's not set - no worries.

		if (!wrapperIsFinished)
		{
			ResponseMessageManager *myResponseMessageManager = session_->getResponseMessageManager();
			if (myResponseMessageManager)
			{
				myResponseMessageManager->removeMessage(ErrWrapper, 9705, HfcResponse::RESPONSESTYLE_OK);
				Debug::trace("EvolWrapperTimingsManager", "clearWaitForWrapperMsg():  Removing message 9705.");
				wrapperIsFinished = true;
			}
			else
			{
				Debug::trace("EvolWrapperTimingsManager", "clearWaitForWrapperMsg():  Error!!! myResponseMessageManager is NULL!");
			}
		}
	}
}


bool EvolWrapperTimingsManager::deleteMe()
{
	return doDeleteMe;
}

HfcEvolWrapperPackageTimings *EvolWrapperTimingsManager::getEvolWrapperPackageTimings()
{
	return(&lastWrapperTimings_);
}
void EvolWrapperTimingsManager::sendPackageTimingsToBackup()
{
	bool debugging = Debug::isTracing("EvolWrapperTimingsManager");
	HobString msgDebug;

	if (session_)
	{
		if ( debugging )
		{
			msgDebug = "sendPackageTimingsToBackup()";
			Debug::trace("EvolWrapperTimingsManager", msgDebug);
			msgDebug.append(":  ");
		}
		EvolWrapperPackageTimings *ewpt = 0;

		const char * newBuff = lastWrapperTimings_.serialize(InternalFormat);
		if (newBuff)
			 ewpt = new EvolWrapperPackageTimings(newBuff);

		if (ewpt)
		{
			if ( debugging )
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("ewpt = ").append(Hex::toString((long long) ewpt));
				Debug::trace("EvolWrapperTimingsManager", msgDebug2);
			}
			EvolWrapperPackageTimingsPointer wtPtrMsg(WrEvolBackupPackageTimings, (EvolWrapperPackageTimings *) ewpt);
			int bytesSent = session_->send(&wtPtrMsg, Messenger::DEST_RTC);

			if ( debugging )
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("***** Wrapper, WrEvolPackageTimings To Backup ");
				if ( bytesSent < 0 )
					Debug::trace("EvolWrapperTimingsManager", msgDebug2 + "COULD NOT BE SENT.");
				else
					Debug::trace("EvolWrapperTimingsManager", msgDebug2 + "was sent.");
			}
		}
		else
		{
			if ( debugging )
				Debug::trace("EvolWrapperTimingsManager", msgDebug + "Could not allocate memory for EvolWrapperPackageTimings!");
		}
	}
}

void EvolWrapperTimingsManager::abortWaitForWrapperMsg()
{
	clearWaitForWrapperMsg();
	exitWhenWrapperIsFinished = false;
	Debug::trace("EvolWrapperTimingsManager", "abortWaitForWrapperMsg:  exitWhenWrapperIsFinished = false");
}

bool EvolWrapperTimingsManager::setMinTrayValues(WrapBox &theBox)
{
	Debug::trace("EvolWrapperTimingsManager", "setMinTrayValues()");
	int initialVal, setting;
	bool hasChanged = false;

	//	set MinTrayLength
	initialVal = theBox.getLengthLow();
	setting = getMinTrayLengthValue(lastPackageDesc_.getLength());
	theBox.setLengthLow((UINT16) setting);
	hasChanged |= (setting != initialVal);
	Debug::trace("EvolWrapperTimingsManager", "setMinTrayValues():  minLength = " + util_string::inttostring(theBox.getLengthLow()) + ",  hasChanged = " + util_string::booltostring(hasChanged));

	//	set MinTrayWidth
	initialVal = theBox.getWidthLow();
	setting = getMinTrayWidthValue(lastPackageDesc_.getWidth());
	theBox.setWidthLow((UINT16) setting);
	hasChanged |= (setting != initialVal);
	Debug::trace("EvolWrapperTimingsManager", "setMinTrayValues():  minWidth = " + util_string::inttostring(theBox.getWidthLow()) + ",  hasChanged = " + util_string::booltostring(hasChanged));

	return hasChanged;
}

bool EvolWrapperTimingsManager::setMaxTrayValues(WrapBox &theBox)
{
	Debug::trace("EvolWrapperTimingsManager", "setMaxTrayValues()");
	int initialVal, setting;
	bool hasChanged = false;

	//	set MaxTrayLength
	initialVal = theBox.getLengthHigh();
	setting = getMaxTrayLengthValue(lastPackageDesc_.getLength());
	theBox.setLengthHigh((UINT16) setting);
	hasChanged |= (setting != initialVal);
	Debug::trace("EvolWrapperTimingsManager", "setMaxTrayValues():  maxLength = " + util_string::inttostring(theBox.getLengthHigh()) + ",  hasChanged = " + util_string::booltostring(hasChanged));

	//	set MaxTrayWidth
	initialVal = theBox.getWidthHigh();
	setting = getMaxTrayWidthValue(lastPackageDesc_.getWidth());
	theBox.setWidthHigh((UINT16) setting);
	hasChanged |= (setting != initialVal);
	Debug::trace("EvolWrapperTimingsManager", "setMaxTrayValues():  maxWidth = " + util_string::inttostring(theBox.getWidthHigh()) + ",  hasChanged = " + util_string::booltostring(hasChanged));
#if 0
//	Per Gary Cornish and Kraig Wooddell (Jan. 2010). They no longer want this for the tray lip.
	//	set MaxTrayLip
	initialVal = theBox.getLip();
	setting = getMaxTrayLipValue(lastPackageDesc_.getLip());
	theBox.setLip((UINT16) setting);
	hasChanged |= (setting != initialVal);
	Debug::trace("EvolWrapperTimingsManager", "setMaxTrayValues():  lip = " + inttostring(theBox.getLip()) + ",  hasChanged = " + booltostring(hasChanged));
#endif

	return hasChanged;
}

bool EvolWrapperTimingsManager::setMinAndMaxTrayValues(WrapBox &theBox)
{
	Debug::trace("EvolWrapperTimingsManager", "setMinAndMaxTrayValues()");
	bool hasChanged = false;

	hasChanged |= setMinTrayValues(theBox);
	hasChanged |= setMaxTrayValues(theBox);
	Debug::trace("EvolWrapperTimingsManager", "setMinAndMaxTrayValues():  hasChanged = " + util_string::booltostring(hasChanged));

	return hasChanged;
}

int EvolWrapperTimingsManager::getMinTrayLengthValue(int actualLength)
{
	Debug::trace("EvolWrapperTimingsManager", "getMinTrayLengthValue()");
	int setting = actualLength - _calcMinLength;
	if (setting < _absMinLength)
		setting = _absMinLength;
	Debug::trace("EvolWrapperTimingsManager", "getMinTrayLengthValue(" + util_string::inttostring(actualLength) + ") = " + util_string::inttostring(setting));
	return setting;
}

int EvolWrapperTimingsManager::getMinTrayWidthValue(int actualWidth)
{
	Debug::trace("EvolWrapperTimingsManager", "getMinTrayWidthValue()");
	int setting = actualWidth - _calcMinWidth;
	if (setting < _absMinWidth)
		setting = _absMinWidth;
	Debug::trace("EvolWrapperTimingsManager", "getMinTrayWidthValue(" + util_string::inttostring(actualWidth) + ") = " + util_string::inttostring(setting));
	return setting;
}

int EvolWrapperTimingsManager::getMaxTrayLengthValue(int actualLength)
{
	Debug::trace("EvolWrapperTimingsManager", "getMaxTrayLengthValue()");
	int setting = actualLength + _calcMaxLength;
	if (setting > _absMaxLength)
		setting = _absMaxLength;
	Debug::trace("EvolWrapperTimingsManager", "getMaxTrayLengthValue(" + util_string::inttostring(actualLength) + ") = " + util_string::inttostring(setting));
	return setting;
}

int EvolWrapperTimingsManager::getMaxTrayWidthValue(int actualWidth)
{
	Debug::trace("EvolWrapperTimingsManager", "getMaxTrayWidthValue()");
	int setting = actualWidth + _calcMaxWidth;
	if (setting > _absMaxWidth)
		setting = _absMaxWidth;
	Debug::trace("EvolWrapperTimingsManager", "getMaxTrayWidthValue(" + util_string::inttostring(actualWidth) + ") = " + util_string::inttostring(setting));
	return setting;
}

int EvolWrapperTimingsManager::getMaxTrayLipValue(int actualLip)
{
	Debug::trace("EvolWrapperTimingsManager", "getMaxTrayLipValue()");
	int setting = actualLip + _calcMaxLip;
	if (setting > _absMaxLip)
		setting = _absMaxLip;
	Debug::trace("EvolWrapperTimingsManager", "getMaxTrayLipValue(" + util_string::inttostring(actualLip) + ") = " + util_string::inttostring(setting));
	return setting;
}
