/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "WeigherCalibration.h"
#include "SystemEvent.h"

#include "Session.h"
#include "SessionManager.h"		// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
#include "SessionMessage.h"
#include "ResponseMessageManager.h"
#include "HfcResponse.h"
#include "HfcWeigherStatus.h"
#include "HfcWeigherInquire.h"
#include "WeigherColdReset.h"
#include "WeigherReset.h"
#include "WeigherRequestInquire.h"
#include "HfcWeigherSystemCalibrationSwitch.h"
#include "WeigherMode.h"
#include "Weigher.h"
#include "WeigherCat3DateTime.h"
#include "HobDateTime.h"
#include "Configuration.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

#include "MessageLogger.h"

#include <vector>

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherCalibration::dataType = 0;
std::string WeigherCalibration::dataTypeName("WeigherCalibration"); 
#endif

// logging options
// debugTraceCategories+=WeigherCalibration
// debugTraceCategories+=WeigherCalibration.State
// debugTraceCategories+=WeigherCalibration.Step
// debugTraceCategories+=WeigherStatus

///////////////////////////////////////////////////////////////////////////////////////////////////
WeigherCalibration::WeigherCalibration() :
	mySession( (Session*) NULL )
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WeigherCalibration::WeigherCalibration( Session *session ) :
	mySession( session )
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeigherCalibration::init()
{
	Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationInitial");
	calibrationState = CalibrationInitial;

	centerOfMaintenanceZone = 0L;
	gainFactor = 0L;
	meanSquareError = 0L;

	userUpdatedGF = false;
	calibrationSucceeded = false;
	switchPressed = false;

	usingAvoirWeights = true;

	showingRemoveWeightMessage = false;

	savedCenterOfMaintenanceZone = 0L;
	savedGainFactor = 0L;

	// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
	WgKind = SessionManager::getInstance()->getFirstWeigher();

	if ( WgUsbAWS == WgKind ) // AWS Weighing Device (Atmel-based SAM-BA device)
	{
		counts_divisor = 1;

		steps.push_back( new WeigherCalibrationStep( 0L, 0L, 0L ) );
		steps.push_back( new WeigherCalibrationStep( 1L, 100000L, 76600L ) );
		steps.push_back( new WeigherCalibrationStep( 2L, 200000L, 200L ) );
		steps.push_back( new WeigherCalibrationStep( 3L, 300000L, 400L ) );
		steps.push_back( new WeigherCalibrationStep( 4L, 200000L, 200L ) );
		steps.push_back( new WeigherCalibrationStep( 5L, 100000L, 200L ) );
		steps.push_back( new WeigherCalibrationStep( 6L, 0L, 200L ) );
	}
	else if ( WgUsbEvol == WgKind ) // Ez-USB Weighing Device
	{
		counts_divisor = 10;

		steps.push_back( new WeigherCalibrationStep( 0L, 0L, 0L ) );
		steps.push_back( new WeigherCalibrationStep( 1L, 10000L, 7000L ) );
		steps.push_back( new WeigherCalibrationStep( 2L, 20000L, 20L ) );
		steps.push_back( new WeigherCalibrationStep( 3L, 30000L, 40L ) );
		steps.push_back( new WeigherCalibrationStep( 4L, 20000L, 20L ) );
		steps.push_back( new WeigherCalibrationStep( 5L, 10000L, 20L ) );
		steps.push_back( new WeigherCalibrationStep( 6L, 0L, 20L ) );
	}
	// SOF-2970
	else if ( WgCanKpc == WgKind )
	{
		counts_divisor = 10;

		// SOF-3309 adjusted by half
		steps.push_back( new WeigherCalibrationStep( 0L, 0L, 0L ) );
		steps.push_back( new WeigherCalibrationStep( 1L, 10000L, 55000L ) );
		steps.push_back( new WeigherCalibrationStep( 2L, 20000L, 150L ) );
		steps.push_back( new WeigherCalibrationStep( 3L, 30000L, 300L ) );
		steps.push_back( new WeigherCalibrationStep( 4L, 20000L, 150L ) );
		steps.push_back( new WeigherCalibrationStep( 5L, 10000L, 150L ) );
		steps.push_back( new WeigherCalibrationStep( 6L, 0L, 150L ) );
	}
	else // APC Weighing Device
	{
		counts_divisor = 10;

		steps.push_back( new WeigherCalibrationStep( 0L, 0L, 0L ) );
		steps.push_back( new WeigherCalibrationStep( 1L, 10000L, 7660L ) );
		steps.push_back( new WeigherCalibrationStep( 2L, 20000L, 20L ) );
		steps.push_back( new WeigherCalibrationStep( 3L, 30000L, 40L ) );
		steps.push_back( new WeigherCalibrationStep( 4L, 20000L, 20L ) );
		steps.push_back( new WeigherCalibrationStep( 5L, 10000L, 20L ) );
		steps.push_back( new WeigherCalibrationStep( 6L, 0L, 20L ) );
	}

	// Need an extra for sending the Browser the last step's delta.
	steps.push_back( new WeigherCalibrationStep( 7L, 0L, 0L ) );

	currentStep = NULL;

	weigherIsWithinZeroQuarterGrad = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WeigherCalibration::~WeigherCalibration()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	// Delete the memory allocated for each step. Then clear the steps vector.
	for ( WeigherCalibrationStep *doomed : steps )
		delete doomed;

	steps.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeigherCalibration::handleMessage( SessionMessage *msg )
{
	if ( !msg )
		return;

	const HobString hd( Integer::toString( msg->getDeviceType() ) );
	const HobString he( Integer::toString( msg->getEventType() ) );

	Debug::trace( "WeigherCalibration", HobString("received a message device: ") + hd + HobString(" event: ") + he );

	// The Browser sends the first step (Step 0) as the indication to start weigher calibration.
	// I need to internally keep track that I received this first step;
	// however, I don't want the user to continue with anything until they pressed
	// the calibrate switch on the weigher, and the weigher has gone through a scale
	// warmup.  Further restrictions will be handled within each 'handle...' method.
	switch ( msg->getDeviceType() )
	{
	case Error:

		handleError( msg );
		break;

	case Weigher:

		if ( WgStatus == msg->getEventType() )
			handleWeighStatus( dynamic_cast<HfcWeigherStatus*>( msg->getHfcObject() ) );
		else if ( WgCalibrationStep == msg->getEventType() )
			handleStep( dynamic_cast<WeigherCalibrationStep*>( msg->getHfcObject() ) );
		else if ( WgCalibrationEnd == msg->getEventType() )
			handleEnd( dynamic_cast<WeigherCalibrationEnd*>( msg->getHfcObject() ) );
		else if ( WgInquire == msg->getEventType() )
			handleWeighInquire( dynamic_cast<HfcWeigherInquire*>( msg->getHfcObject() ) );
		else
		{
			Debug::trace("WeigherCalibration", "CANNOT PROCESS THE MESSAGE LISTED ABOVE");
		}

		break;

	case WeigherSystem:

		if ( SysCalibSwitch == msg->getEventType() )
			handleWeighSystemCalibSwitch( dynamic_cast<HfcWeigherSystemCalibrationSwitch*>( msg->getHfcObject() ) );

		break;

	default:

		Debug::trace("WeigherCalibration", "CANNOT PROCESS THE MESSAGE LISTED ABOVE");
		break;
	}

	// process state
	processCurrentState();

	Debug::trace("WeigherCalibration", HobString("done processing message."));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeigherCalibration::processCurrentState()
{
	// Wait for the user to press the calibrate switch on the weigher.
	if (( CalibrationInitiated == calibrationState ) && ( mySession ))
	{
		Debug::trace("WeigherCalibration", HobString("===== initiated -- waiting for switch pressed ====="));

		// Send the message to the RTC to have the weigher go into dumb mode, then manual mode (as seen with Ultima monitor).
		WeigherMode wgMode(mySession, ScaleDumbMode);
		mySession->send( &wgMode, Messenger::DEST_RTC );

		WeigherMode wgMode2(mySession, ScaleManualMode);
		mySession->send( &wgMode2, Messenger::DEST_RTC );

		if ( switchPressed )
			calibrationState = CalibrationSwitchPressed;
		else
		{
			//	Request the Weigher switch.
			if ( mySession->getResponseMessageManager() )
				mySession->getResponseMessageManager()->addMessage(ErrWeigher, WeigherErrPressCalibrateSwitch, HfcResponse::RESPONSESTYLE_CANCEL);

			Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationWaitingForSwitch");
			calibrationState = CalibrationWaitingForSwitch;

			mySession->registerEvent( WeigherSystem, SysCalibSwitch );
		}
	}

	// Calibrate switch has been pressed.  Do a weigher reset (warmup).
	if ( CalibrationSwitchPressed == calibrationState )
	{
		Debug::trace("WeigherCalibration", HobString("===== config switch pressed -- waiting for weigher inquire ====="));

		// Commented out the register for the message, we are now always registered for the WgInquire.
		//mySession->registerEvent(Weigher, WgInquire, false ); 

		// Send the message to the RTC to get the weigher configuration.
		WeigherGeneric wri(WgReqConfig,mySession);
		mySession->send( &wri, Messenger::DEST_RTC );

		Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationWaitingForInquire1");
		calibrationState = CalibrationWaitingForInquire1;

		// Added log message when remote scale maintenance feature added, FHA 11/13
		MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Weigher calibration started");
	}

	// Weigher Configuration has been initialized.  Do a weigher reset (warmup).
	if ( CalibrationUpdatedConfig1 == calibrationState )
	{
		Debug::trace("WeigherCalibration", HobString("===== initial config updated -- will be waiting for weigher warmup ====="));

		resetTheWeigherAndTrack();

		Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationStartWarmingUp");
		calibrationState = CalibrationStartWarmingUp;
	}

	// Weigher Calibration is finished/cancelled.
	if ( CalibrationCompleted == calibrationState )
	{
		Debug::trace("WeigherCalibration", "===== calibration completed =====");

		WeigherMode wgMode(mySession, ScaleManualMode/*ScaleDumbMode*/);
		mySession->send(&wgMode, Messenger::DEST_RTC);

		if (calibrationSucceeded && mySession->getResponseMessageManager())
			mySession->getResponseMessageManager()->addMessage(ErrWeigher, WeigherErrCalibrationSuccessful, HfcResponse::RESPONSESTYLE_OK);

		SessionMessage msg(Weigher, WgCalibrationEnd);
		msg.setFormatType(BrowserFormat);

		WeigherCalibrationEnd state;
		
        if (calibrationSucceeded)
        {
            state.setEndState( WeigherCalibrationEnd::ENDSTATE_COMPLETE );
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Weigher calibration completed");
        }
        else
        {
            state.setEndState( WeigherCalibrationEnd::ENDSTATE_CANCEL );
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Weigher calibration cancelled");
        }
		
		msg.setHfcObject( &state );
		mySession->send( &msg, Messenger::DEST_UI );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeigherCalibration::handleError( SessionMessage *msg )
{
	if ( ErrRemoveAnyModalDialog == msg->getEventType() )
	{
		HfcResponse *errResponse( dynamic_cast<HfcResponse*>( msg->getHfcObject() ) );

		if (( errResponse ) && ( WeigherErrDeltaChangeTooBig == errResponse->getCode() ))
			;

		//calibrationState is currently CalibrationPromptingForTypeOfWeights
		else if (( errResponse ) && ( WeigherErrSelectWeightsType == errResponse->getCode() ))
		{
			// LB Weights was pressed
			if ( errResponse->getAnswer() == HfcResponse::RESPONSEANSWER_OK )
				usingAvoirWeights = true;
			// Metric Weights was pressed
			else
				usingAvoirWeights = false;

			Debug::trace("WeigherCalibration", HobString("handling response to WeigherErrSelectWeightsType - usingAvoirWeights: ") + HobString(Bool::toString(usingAvoirWeights)));

			// We need to tell the browser whether to set up the display for using avoir or metric weights.
			// We notify the browser of the type of weights the user chose for calibration by sending
			// step 99 to the browser.  The 'zeroedCounts' field of the step indicate which weights were
			// selected. ("0"=metric, "1"=avoir)
			WeigherCalibrationStep weightType( 99, 0, 0 );
			weightType.setZeroedCounts((usingAvoirWeights) ? 1 : 0);

			SessionMessage weightTypemsg( Weigher, WgCalibrationStep );
			weightTypemsg.setFormatType( BrowserFormat );
			weightTypemsg.setHfcObject( &weightType );
			mySession->send(&weightTypemsg, Messenger::DEST_UI);

			mySession->registerEvent(Weigher, WgStatus);

			currentStep = steps[0];

			// Automatically send the initial step back to the browser.
			SessionMessage retmsg( Weigher, WgCalibrationStep );
			retmsg.setFormatType( BrowserFormat );
			retmsg.setHfcObject( currentStep );
			mySession->send( &retmsg, Messenger::DEST_UI );

			Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationInitiated");
			calibrationState = CalibrationInitiated;
		}
		else if (( errResponse ) && ( WeigherErrRemoveWeight == errResponse->getCode() ))
		{
			// The user pressed CANCEL while waiting for the weigher to return to Zero during the
			// end of Calibration.  Normally the weigher will return to Zero when they take the weight
			// off the platter at the point of verifying the calculated gain factor.  But if it doesn't
			// return to Zero, we wanted to give them a way out of the calibration.

			//	Now, pressing Cancel returns the weigher to it's default values. [CCD Jan 2012]
			showingRemoveWeightMessage = false;

			// Remove the message window from the Browser.
			if (mySession->getResponseMessageManager())
				mySession->getResponseMessageManager()->removeMessage(ErrWeigher, WeigherErrRemoveWeight, HfcResponse::RESPONSESTYLE_CANCEL);

			calibrationSucceeded = false;

			resetTheWeigherAndTrack();

			if (mySession->getResponseMessageManager())
				mySession->getResponseMessageManager()->addMessage(ErrWeigher, WeigherErrScaleWarmup, HfcResponse::RESPONSESTYLE_NOINPUT);

			Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationStartFinalWarmingUp");
			calibrationState = CalibrationStartFinalWarmingUp;
		}
		else
		{
			WeigherCalibrationEnd cancel( WeigherCalibrationEnd::ENDSTATE_CANCEL );
			handleEnd( &cancel );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WeigherCalibration::isWeigherCalibrationDone()
{
	return ( CalibrationCompleted == calibrationState );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeigherCalibration::handleWeighSystemCalibSwitch( HfcWeigherSystemCalibrationSwitch *msg )
{
	if ( CalibrationWaitingForSwitch != calibrationState )
		return;

	Debug::trace("WeigherCalibration", HobString("received a Weigher System Calib Switch Message while waiting switch to be pressed."));

	if ( msg->isSystemCalibrationSwitch() )
	{
		Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationSwitchPressed");
		calibrationState = CalibrationSwitchPressed;
		mySession->deregisterEvent(WeigherSystem, SysCalibSwitch );

		// Remove the message window from the Browser.
		if (mySession->getResponseMessageManager())
			mySession->getResponseMessageManager()->removeMessage(ErrWeigher, WeigherErrPressCalibrateSwitch, HfcResponse::RESPONSESTYLE_CANCEL);

		Debug::trace("WeigherCalibration", HobString("done with switch pressed."));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeigherCalibration::handleWeighStatus( HfcWeigherStatus *status )
{
	if (( !status ) || ( !currentStep ))
		return;

	if ( Debug::isTracing("WeigherStatus") )
		status->show();

	// ================
	// Warming up start
	// ================

	if (( CalibrationStartWarmingUp == calibrationState ) ||
		( CalibrationStartWarmingUp2 == calibrationState ) ||
		( CalibrationStartFinalWarmingUp == calibrationState ))
	{
		Debug::trace("WeigherCalibration.State", "Weigher should be warming up ...");

		if ( status->isScaleWarmup() )
		{
			Debug::trace("WeigherCalibration.State", "Weigher IS warming up ...");

			if (mySession->getResponseMessageManager())
				mySession->getResponseMessageManager()->addMessage(ErrWeigher, WeigherErrScaleWarmup, HfcResponse::RESPONSESTYLE_CANCEL);

			if (calibrationState == CalibrationStartWarmingUp)
			{
				Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationWarmingUp");
				calibrationState = CalibrationWarmingUp;
			}
			else if (calibrationState == CalibrationStartWarmingUp2)
			{
				Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationWarmingUp2");
				calibrationState = CalibrationWarmingUp2;
			}
			else
			{
				Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationFinalWarmingUp");
				calibrationState = CalibrationFinalWarmingUp;
			}
		}
	}

	// ==========
	// Warming up
	// ==========
	else if (( CalibrationWarmingUp == calibrationState ) ||
		( CalibrationWarmingUp2 == calibrationState ) ||
		( CalibrationFinalWarmingUp == calibrationState ))
	{
		Debug::trace("WeigherCalibration.State", "Waiting for weigher to finish warming up ...");

		if ( !status->isScaleWarmup() )
		{
			Debug::trace("WeigherCalibration.State", "Weigher IS NOT warming up ...");
			// Send the message to the RTC to put the weigher in weight-tracking mode.
			//WeigherMode wgMode(mySession, (calibrationState == CalibrationWarmingUp) ? ScaleWeightTrackingMode : ScaleManualMode);
			WeigherMode wgMode(mySession, ScaleWeightTrackingMode);
			mySession->send(&wgMode, Messenger::DEST_RTC);

			// Remove the message window from the Browser.
			if (mySession->getResponseMessageManager())
				mySession->getResponseMessageManager()->removeMessage(ErrWeigher, WeigherErrScaleWarmup, HfcResponse::RESPONSESTYLE_CANCEL);

			if (calibrationState == CalibrationWarmingUp)
			{
				Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationStepping");
				calibrationState = CalibrationStepping;
			}
			else if (calibrationState == CalibrationWarmingUp2)
			{
				Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationUpdatingGainFactor");
				calibrationState = CalibrationUpdatingGainFactor;
			}
			else
			{
				Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationCompleted");
				calibrationState = CalibrationCompleted;
			}
		}
	}

	// ========
	// stepping
	// ========

	else if (( CalibrationStepping == calibrationState ) ||
		( CalibrationUpdatingGainFactor == calibrationState ))
	{
		// KPC uses raw counts to determine calibration
		if ( WgCanKpc == WgKind )
		{
			const long rcnts( status->getFilteredUncalibratedCounts() / counts_divisor );

			long zerocnts( 0 );

			if ( steps[0] == currentStep )
				zerocnts = rcnts;
			else
				zerocnts = steps[0]->getNonZeroedCounts();

			// update step
			currentStep->setZeroedCounts( rcnts - zerocnts );
			currentStep->setNonZeroedCounts( rcnts );

			// SOF-3136
			// as per Kraig, zeroed counts that float around "look bad"... so
			// lets shift counts down and remove some LSB bits...
			// SOF-3309 adjusted by half
			long display_zerocnts( currentStep->getZeroedCounts() / 8 );
			long display_zerocnts_remainder( currentStep->getZeroedCounts() % 8 );

			if ( display_zerocnts_remainder < -4 )
				--display_zerocnts;
			else if ( 4 <= display_zerocnts_remainder )
				++display_zerocnts;

			currentStep->setZeroedCountsForDisplay( display_zerocnts );
		}
		else
		{
			long cnts( status->getBaseCounts() / counts_divisor );
			long rcnts( status->getRawCounts() / counts_divisor );

			if ( WgUsbAWS == WgKind )
				rcnts = status->getFilteredUncalibratedCounts() / counts_divisor;

			Debug::trace("WeigherCalibration.Step", HobString("updating the current step: counts=") + Long::toString(cnts) +
				HobString("  rawcounts=") + Long::toString(rcnts) );

			// update step
			currentStep->setZeroedCounts( cnts );
			currentStep->setNonZeroedCounts( rcnts );

			if ( CalibrationUpdatingGainFactor == calibrationState )
				if (( !usingAvoirWeights ) && ( !MetSig::isScaleMetric() ))
					currentStep->setZeroedCounts( currentStep->getZeroedCounts() / 2 );
		}

		// Send the current step information to the UI (Browser), if mySession is set up.
		if ( mySession )
		{
			if ( CalibrationUpdatingGainFactor == calibrationState )	 // Partial fix for PTR#3760 (haberrc)
			{
				static long ZeroedCounts( -1 );

				if ( ZeroedCounts != currentStep->getZeroedCounts() )
				{
					// Now send the updated values to the Browser.
					SessionMessage msg( Weigher, WgCalibrationStep );
					msg.setFormatType( BrowserFormat );
					msg.setHfcObject( currentStep );

					Debug::trace("WeigherCalibration.State", "Sending updated step back to browser");
					mySession->send( &msg, Messenger::DEST_UI );

					ZeroedCounts = currentStep->getZeroedCounts();
				}

				Debug::trace( "WeigherCalibration.State", HobString("zeroed counts=") + Long::toString(ZeroedCounts) );

				// This will allow us to know whether or not there is weight on the platter before we send
				// the new Gain Factor to the weigher. In the case of the Atmel weigher, calibrating with
				// weights opposite of the weighing mode causes problems because the "Remove weight from platter"
				// message appears after we write the converted gain factor. The problem is remedied when we send a
				// Cold Reset, but we want to make sure there is no weight on the platter before we do the Cold Reset.
				// So, we will record this state while we are waiting for the final Inquire and that will tell us if there
				// truely is weight on the platter (PTRs 985 & 987).
				weigherIsWithinZeroQuarterGrad = status->isWithinZeroQuarterGrad();
				Debug::trace("WeigherCalibration.State", "CalibrationUpdatingGainFactor:   weigherIsWithinZeroQuarterGrad = " + HobString(Bool::toString(weigherIsWithinZeroQuarterGrad)));
			}
			else if ( CalibrationStepping == calibrationState )
			{
				// Now send the updated values to the Browser.
				SessionMessage msg( Weigher, WgCalibrationStep );
				msg.setFormatType( BrowserFormat );
				msg.setHfcObject( currentStep );

				Debug::trace( "WeigherCalibration", "Sending updated step back to browser" );
				mySession->send( &msg, Messenger::DEST_UI );
			}
		}
	}

	// ==============
	// weight removal
	// ==============
	else if ( CalibrationWaitingForWeightRemoval == calibrationState )
	{
		if ( status->isWithinZeroQuarterGrad() )
		{
			//	The weight has been removed from the platter. Move on.
			showingRemoveWeightMessage = false;

			// Remove the message window from the Browser.
			if (mySession->getResponseMessageManager())
				mySession->getResponseMessageManager()->removeMessage(ErrWeigher, WeigherErrRemoveWeight, HfcResponse::RESPONSESTYLE_CANCEL);

			finalizeWeigherCalibrationParameters();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeigherCalibration::handleStep( WeigherCalibrationStep *step )
{
	if ( !step )
		return;

	if (calibrationState == CalibrationInitial)
	{
		Debug::trace("WeigherCalibration", HobString("handling initial calibration step : ") + HobString(Integer::toString(step->getStepNum())));

		Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationPromptingForTypeOfWeights");
		calibrationState = CalibrationPromptingForTypeOfWeights;

		if (mySession && mySession->getResponseMessageManager())
			mySession->getResponseMessageManager()->addMessage(ErrWeigher, WeigherErrSelectWeightsType, HfcResponse::RESPONSESTYLE_CALIBRATIONWEIGHTS);

		if (step->getZeroedCounts() == 1)		//	The weigher switch was pressed already
		{
			switchPressed = true;
			Debug::trace("WeigherCalibration.State", "The weigher switch has been pressed.");
		}
	}

	else if (calibrationState == CalibrationStepping)
	{
		Debug::trace("WeigherCalibration", HobString("handling calibration step : ") + HobString(Integer::toString(step->getStepNum())));

		/* I received a new step.  Find the corresponding step in the steps vector and make it the current step.
		 * If not in the vector, well, then it's not in the vector.
		 * Sending a step with a new step number is how the Browser indicates the user
		 * has gone on to the next step.
		 */
		if (currentStep && currentStep->getStepNum() != step->getStepNum())
		{
			bool deltaErrorExists = false;
			WeigherCalibrationStep *nextStep = (WeigherCalibrationStep *)NULL;
			if ((short)steps.size() > step->getStepNum())
				nextStep = steps[step->getStepNum()];

			if (currentStep->getStepNum() > 0)	// There is no delta for the first step.
			{
				Debug::trace("WeigherCalibration", HobString("calculating delta: ") + HobString(nextStep->toString()));

				/* Calculate the delta between the previous two steps, when appropriate.
				 * The delta for the previous line is sent to the Browser with the new current line,
				 * So update this delta in the new current step.
				 */
				WeigherCalibrationStep *previousStep = steps[currentStep->getStepNum() - 1];

				nextStep->setZeroedCounts(currentStep->getZeroedCounts());
				nextStep->setNonZeroedCounts(currentStep->getNonZeroedCounts());

				long delta = currentStep->getZeroedCounts() - previousStep->getZeroedCounts();
				if (delta < 0L)
					delta *= -1;
				Debug::trace("WeigherCalibration", HobString("set delta: ") + HobString(Long::toString(delta)));
				nextStep->setDelta(delta);

				long deltaChange = nextStep->getDelta() - currentStep->getDelta();
				if (currentStep->getStepNum() == 1)
				{
					// For the first step, compare the delta with the scale's tolerance.
					// Bad if the delta is below the tolerance.
					if ( WgCanKpc == WgKind )
					{
						// SOF-3309 adjusted by half
						// expecting ~70000
						if ((delta < 60000) || (80000 < delta))
							deltaErrorExists = true;
					}
					// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
					else if ( WgUsbAWS == WgKind )
					{
						if (delta < 51067) // Lowered tolerance to support 30kg. (50lb.) load cells.
							deltaErrorExists = true;
					}
					else if ( WgUsbEvol == WgKind )
					{
						if (delta < 7000)
							deltaErrorExists = true;
					}
					else // WgUsbApc || WgCanPrepack
					{
						if (delta < 1000)
							deltaErrorExists = true;
					}
				}
				else
				{
					Debug::trace("WeigherCalibration", HobString("currentStep->getStepNum()= ") + HobString(Integer::toString(currentStep->getStepNum())));
					Debug::trace("WeigherCalibration", HobString("deltaChange= ") + HobString(Long::toString(deltaChange)));
					Debug::trace("WeigherCalibration", HobString("currentStep->getMaxError()= ") + HobString(Long::toString(currentStep->getMaxError())));

					if (deltaChange > currentStep->getMaxError() || deltaChange < -currentStep->getMaxError())
					{
						deltaErrorExists = true;
						Debug::trace("WeigherCalibration", HobString("deltaChange-FAILED"));
					}

					// Verify weigher steps coming back down to zero closely resemble those going up.
					if ( WgCanKpc == WgKind )
					{
						int diff( 0 );

						if ( 4 == currentStep->getStepNum() )
							diff = steps[4]->getNonZeroedCounts() - steps[2]->getNonZeroedCounts();
						else if ( 5 == currentStep->getStepNum() )
							diff = steps[5]->getNonZeroedCounts() - steps[1]->getNonZeroedCounts();
						else if ( 6 == currentStep->getStepNum() )
							diff = steps[6]->getNonZeroedCounts() - steps[0]->getNonZeroedCounts();

						if (( diff > (currentStep->getMaxError() / 2) ) || ( diff < (-currentStep->getMaxError() / 2) ))
						{
							deltaErrorExists = true;
							Debug::trace("WeigherCalibration", HobString("Step Variation - FAILED"));
						}
					}
				}

				if (deltaErrorExists)
				{
					Debug::trace("WeigherCalibration", HobString("delta is too big: ") + HobString(Long::toString(deltaChange)));
					// todo - SHOULD THIS WINDOW GO AWAY AUTOMATICALLY ??????????????????????

					/* Difference between previous Delta and current Delta is too big.
					 * Display an error message for the user.
					 */
					if (mySession->getResponseMessageManager())
						mySession->getResponseMessageManager()->addMessage(ErrWeigher, WeigherErrDeltaChangeTooBig, HfcResponse::RESPONSESTYLE_OK);

					SessionMessage retmsg(Weigher, WgCalibrationStep);
					retmsg.setFormatType(BrowserFormat);
					retmsg.setHfcObject(currentStep);
					mySession->send(&retmsg, Messenger::DEST_UI);
				}
				else
				{
					// Remove the message window from the Browser.
					if (mySession->getResponseMessageManager())
						mySession->getResponseMessageManager()->removeMessage(ErrWeigher, WeigherErrDeltaChangeTooBig, HfcResponse::RESPONSESTYLE_OK);

					SessionMessage retmsg(Weigher, WgCalibrationStep);
					retmsg.setFormatType(BrowserFormat);
					retmsg.setHfcObject(nextStep);
					mySession->send(&retmsg, Messenger::DEST_UI);
				}
			}

			if (!deltaErrorExists)
				currentStep = nextStep;
		}
	}

	else if (calibrationState == CalibrationUpdatingGainFactor)
	{
		/* The user chose to update/adjust the gain factor.
		 */

		Debug::trace("WeigherCalibration", "User updated gain factor");

		if (step->getDelta() != currentStep->getDelta())
		{
			// User changed the gain factor to a different value.
			userUpdatedGF = true;

			gainFactor = step->getDelta();
			currentStep->setDelta(gainFactor);
			// Commented out the register for the message, we are now always registered for the WgInquire.
			//mySession->registerEvent(Weigher, WgInquire, false ); 

			// Send the message to the RTC to get the weigher configuration.
			WeigherGeneric wri(WgReqConfig,mySession);
			mySession->send(&wri, Messenger::DEST_RTC);

			Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationWaitingForInquire3");
			calibrationState = CalibrationWaitingForInquire3;

			// Do not need to send this step back to the UI; the counts will be updated by the weighStatus message.
		}
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeigherCalibration::handleEnd(WeigherCalibrationEnd *state)
{
	Debug::trace("WeigherCalibration", "handling Calibration End Message");

	if (!state)
		return;

	Debug::trace("WeigherCalibration", "handleEnd():  EndState = " + Integer::toString(state->getEndState()));

	/* Do whatever we need to do when the Browser indicates that the user is done 
	 * with the calibration steps, and now wants to go on to validating the gain factor.
	 */
	if (state->getEndState() == WeigherCalibrationEnd::ENDSTATE_STEPSCOMPLETE)
	{
		Debug::trace("WeigherCalibration", "received the End Steps Message from the Browser");

		Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationSteppingDone");
		calibrationState = CalibrationSteppingDone;

		// Send the message to the RTC to put the weigher in dumb mode.
		WeigherMode wgMode(mySession, ScaleManualMode);
		mySession->send(&wgMode, Messenger::DEST_RTC);

		// Commented out the register for the message, we are now always registered for the WgInquire.
		//mySession->registerEvent(Weigher, WgInquire, false ); 

		// Send the message to the RTC to get the weigher configuration.
		WeigherGeneric wri(WgReqConfig,mySession);
		mySession->send(&wri, Messenger::DEST_RTC);

		/* Calculate the gain factor.  Create a new step (the next one in the sequence) to store
		 * it in, and send it to the Browser.  Also store it in the list of steps as the current
		 * step.  We are using a "step" message to send the gain factor to the Browser and to
		 * receive any updates to the gain factor from the Browser so that we wouldn't have to
		 * create a new message.
		 *
		 * The functions opt_gain_factor(...) and update_points(...) in cal_weigh.c (Ultima)
		 * were the basis of calculating the gain factor.
		 */
		WeigherCalibrationStep *gainFactorStep = new WeigherCalibrationStep(currentStep->getStepNum() + 1, 0L, 0L);

		if ( WgCanKpc == WgKind )
			calculateOptimalGainFactorLinearFit();
		else
			calculateOptimalGainFactor();

		Debug::trace("WeigherCalibration.State", "Calculated gain factor: " + Long::toString(gainFactor) );

		gainFactorStep->setDelta(gainFactor);

		currentStep = gainFactorStep;
		steps.push_back(gainFactorStep);

		SessionMessage msg(Weigher, WgCalibrationStep);
		msg.setFormatType(BrowserFormat);
		msg.setHfcObject(currentStep);
		if ( 0 <= mySession->send(&msg, Messenger::DEST_UI) )
		{
			Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationWaitingForInquire2");
			calibrationState = CalibrationWaitingForInquire2;
		}
	}

	// Do whatever we need to do when calibration has completed normally or was cancelled.
	else
	{
		/* Do whatever we need to do when the user has prematurely terminated
		 * calibration by pressing the Escape key.
		 */
		if (state->getEndState() == WeigherCalibrationEnd::ENDSTATE_CANCEL)
		{
			Debug::trace("WeigherCalibration", "received the CANCEL Calibration Message from the Browser");

			if (calibrationState >= CalibrationUpdatedConfig1)
			{
				userUpdatedGF = true;
				if (calibrationState >= CalibrationUpdatingGainFactor)
					calibrationSucceeded = true;
			}
		}
		else if (state->getEndState() == WeigherCalibrationEnd::ENDSTATE_COMPLETE)
		{
			userUpdatedGF = true;
			calibrationSucceeded = true;
		}

		if (userUpdatedGF)
		{
			if (!weigherIsWithinZeroQuarterGrad)
			{
				showingRemoveWeightMessage = true;

				if (mySession->getResponseMessageManager())
					mySession->getResponseMessageManager()->addMessage(ErrWeigher, WeigherErrRemoveWeight, HfcResponse::RESPONSESTYLE_CANCEL);

				Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationWaitingForWeightRemoval");
				calibrationState = CalibrationWaitingForWeightRemoval;
			}
			else if (calibrationState < CalibrationFinalWaitingForInquire)
				finalizeWeigherCalibrationParameters();
		}
		else
		{
			Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationCompleted");

			calibrationState = CalibrationCompleted;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeigherCalibration::handleWeighInquire(HfcWeigherInquire *hwi)
{
	//hwi->show();

	if (calibrationState == CalibrationWaitingForInquire1)
	{
		Debug::trace("WeigherCalibration", "received the weigher inquire response from the RTC (1)");

		savedCenterOfMaintenanceZone = hwi->getCenterOfMaintenanceZone();
		savedGainFactor = hwi->getGainFactor();

		if ( WgCanKpc != WgKind )
		{
			hwi->setCenterOfMaintenanceZone((UINT32)0);

			// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
			if (WgKind == WgUsbAWS)
				hwi->setGainFactor((UINT32)1000000);
			else
				hwi->setGainFactor((UINT32)10000);

			hwi->setConfigType(WgTemporary);
			hwi->setEventType(WgConfig);

			//hwi->show();
			mySession->send(hwi, Messenger::DEST_RTC);
		}

		Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationUpdatedConfig1");
		calibrationState = CalibrationUpdatedConfig1;
	}

	else if (calibrationState == CalibrationWaitingForInquire2)
	{
		// This intermediate step of the calibration process writes the gain factor for the type of weights
		// the service tech is using to calibrate the scale, so that the counts displayed on the
		// serviceWeigherCalibrationComplete screen shows correctly for the weights being used to calibrate.
		// Note: the final gain factor will be written to the eep for the mode the scale will operate in;
		// i.e. if they calibrate using metric weights, but use the scale in avoir mode, this code block will
		//      write the metric gain factor, but the final eep write will write the avoir gain factor.

		Debug::trace("WeigherCalibration", "received the weigher inquire response from the RTC (2,3)");

		if ( WgCanKpc == WgKind )
		{
			// KPC uses raw counts, average zero at first and last steps
			hwi->setCenterOfMaintenanceZone( counts_divisor * (steps[0]->getNonZeroedCounts() + steps[6]->getNonZeroedCounts()) / 2 );
		}
		// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
		else if (WgKind == WgUsbAWS)
		{
			unsigned long long int temp;	

			temp = (unsigned long long int)((unsigned long long int)steps[0]->getNonZeroedCounts() * gainFactor);
			hwi->setCenterOfMaintenanceZone((UINT32)(temp / ScalingConstant));
		}
		else
		{
			hwi->setCenterOfMaintenanceZone((UINT32)( steps[0]->getNonZeroedCounts() * gainFactor / 1000 ));
		}

		hwi->setGainFactor((UINT32)gainFactor);
		hwi->setConfigType((WgConfigType) (WgPermanent | WgCalibInProgress));
		hwi->setEventType(WgConfig);

		//hwi->show();
		mySession->send(hwi, Messenger::DEST_RTC);

		// Commented out the register for the message, we are now always registered for the WgInquire.
		//mySession->registerEvent(Weigher, WgInquire, false );

		// Send the message to the RTC to get the weigher configuration.
		WeigherGeneric wri(WgReqConfig,mySession);
		mySession->send(&wri, Messenger::DEST_RTC);

		Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationUpdatedConfig2");
		calibrationState = CalibrationUpdatedConfig2;
	}

	else if (calibrationState == CalibrationUpdatedConfig2)
	{
		Debug::trace("WeigherCalibration", "received the weigher inquire response after being configured from the RTC (2,3)");

		if (hwi->getConfigType() != 0)
		{
			Debug::trace("WeigherCalibration", "*************** Weigher EEP error - couldn't write weigher calibration data *****************");
		}

		resetTheWeigherAndTrack();

		if (mySession->getResponseMessageManager())
			mySession->getResponseMessageManager()->addMessage(ErrWeigher, WeigherErrScaleWarmup, HfcResponse::RESPONSESTYLE_CANCEL);

		Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationStartWarmingUp2");
		calibrationState = CalibrationStartWarmingUp2;
	}

	else if (calibrationState == CalibrationWaitingForInquire3)
	{
		// This intermediate step of the calibration process writes the gain factor for the type of weights
		// the service tech is using to calibrate the scale, so that the counts displayed on the
		// serviceWeigherCalibrationComplete screen shows correctly for the weights being used to calibrate.
		// Note: the final gain factor will be written to the eep for the mode the scale will operate in;
		// i.e. if they calibrate using metric weights, but use the scale in avoir mode, this code block will
		//      write the metric gain factor, but the final eep write will write the avoir gain factor.

		Debug::trace("WeigherCalibration", "received the weigher inquire response from the RTC (2,3)");

		if ( WgCanKpc == WgKind )
		{
			// KPC uses raw counts, average zero at first and last steps
			hwi->setCenterOfMaintenanceZone( counts_divisor * (steps[0]->getNonZeroedCounts() + steps[6]->getNonZeroedCounts()) / 2 );
		}
		// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
		else if (WgKind == WgUsbAWS)
		{
			unsigned long long int temp;	

			temp = (unsigned long long int)((unsigned long long int)steps[0]->getNonZeroedCounts() * gainFactor);
			hwi->setCenterOfMaintenanceZone((UINT32)(temp / ScalingConstant));
		}
		else
		{
			hwi->setCenterOfMaintenanceZone((UINT32)( steps[0]->getNonZeroedCounts() * gainFactor / 1000 ));
		}

		hwi->setGainFactor((UINT32)gainFactor);
		hwi->setConfigType(WgTemporary);
		hwi->setEventType(WgConfig);

		//hwi->show();
		mySession->send(hwi, Messenger::DEST_RTC);

		// Send the message to the RTC to put the weigher in weight tracking mode.
		WeigherMode wgMode(mySession, ScaleWeightTrackingMode);
		mySession->send(&wgMode, Messenger::DEST_RTC);

		Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationUpdatingGainFactor");
		calibrationState = CalibrationUpdatingGainFactor;
	}

	else if (calibrationState == CalibrationFinalWaitingForInquire)
	{
		Debug::trace("WeigherCalibration", "received the weigher inquire response from the RTC (finalizing)");

		// For Category III, have to send the current date/time to the Weigher Manager before permanently setting any configuration or calibration values.
		time_t now;
		time(&now);
		WeigherCat3DateTime wCat3DateTime( mySession, (UINT32) now );
		mySession->send( &wCat3DateTime, Messenger::DEST_RTC );

		if ( calibrationSucceeded )
		{
			Debug::trace("WeigherCalibration", "CalibrationFinalWaitingForInquire - before converting, centerOfMaintenanceZone is: " + HobString(Long::toString((UINT32)centerOfMaintenanceZone)));
			// Convert the gain factor for the current weigher mode, not for the weights with which it was calibrated.
			if (usingAvoirWeights && MetSig::isScaleMetric())
			{
				gainFactor = WeigherConfiguration::convertGainFactorToMetric((UINT32)gainFactor);
				Debug::trace("WeigherCalibration", "CalibrationFinalWaitingForInquire - converting avoir gain factor to metric: " + HobString(Long::toString((UINT32)gainFactor)));
			}
			else if (!usingAvoirWeights && MetSig::isScaleAvoir())
			{
				gainFactor = WeigherConfiguration::convertGainFactorToAvoir((UINT32)gainFactor);
				Debug::trace("WeigherCalibration", "CalibrationFinalWaitingForInquire - converting metric gain factor to avoir: " + HobString(Long::toString((UINT32)gainFactor)));
			}

			if ( WgCanKpc == WgKind )
			{
				// KPC uses raw counts, average zero at first and last steps
				centerOfMaintenanceZone = counts_divisor * (steps[0]->getNonZeroedCounts() + steps[6]->getNonZeroedCounts()) / 2;
			}
			// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
			else if (WgKind == WgUsbAWS)
			{
				unsigned long long int temp;	

				temp = (unsigned long long int)((unsigned long long int)steps[0]->getNonZeroedCounts() * gainFactor);
				centerOfMaintenanceZone = (long)(temp / ScalingConstant);
			}
			else
			{
				centerOfMaintenanceZone = steps[0]->getNonZeroedCounts() * gainFactor / 1000;
			}
			Debug::trace("WeigherCalibration", "CalibrationFinalWaitingForInquire - after converting, centerOfMaintenanceZone is: " + HobString(Long::toString((UINT32)centerOfMaintenanceZone)));
		}

		Debug::trace("WeigherCalibration", "CalibrationFinalWaitingForInquire - writing centerOfMaintenanceZone: " + HobString(Long::toString((UINT32)centerOfMaintenanceZone)));
		Debug::trace("WeigherCalibration", "CalibrationFinalWaitingForInquire - writing gain factor: " + HobString(Long::toString((UINT32)gainFactor)));
		hwi->setGainFactor((UINT32)gainFactor);
		hwi->setCenterOfMaintenanceZone((UINT32)centerOfMaintenanceZone);
		hwi->setConfigType((calibrationSucceeded) ? ((WgConfigType) (WgPermanent | WgCalibInProgress)) : WgPermanent);
		hwi->setEventType(WgConfig);

		/* If calibration was successful, update config params for last calibration date,
		 * and increment number of times calibrated.
		 */
		if (calibrationSucceeded)
			hwi->updateCalibrationDateAndTimes();

		//hwi->show();
		mySession->send(hwi, Messenger::DEST_RTC);

		// Commented out the register for the message, we are now always registered for the WgInquire.
		//mySession->registerEvent(Weigher, WgInquire ,false );

		// Send the message to the RTC to get the weigher configuration.
		WeigherGeneric wri(WgReqConfig, mySession);
		mySession->send(&wri, Messenger::DEST_RTC);

		Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationFinalUpdatedConfig");
		calibrationState = CalibrationFinalUpdatedConfig;
	}

	else if (calibrationState == CalibrationFinalUpdatedConfig)
	{
		
		Debug::trace("WeigherCalibration", "received the weigher inquire response after being configured from the RTC (final)");

		if (hwi->getConfigType() != 0)
		{
			Debug::trace("WeigherCalibration", "*************** Weigher EEP error - couldn't write weigher calibration data *****************");
		}

		resetTheWeigherAndTrack();

		if (mySession->getResponseMessageManager())
			mySession->getResponseMessageManager()->addMessage(ErrWeigher, WeigherErrScaleWarmup, HfcResponse::RESPONSESTYLE_NOINPUT);

		Debug::trace("WeigherCalibration.State", "Changing to calibrationState:  CalibrationStartFinalWarmingUp");
		calibrationState = CalibrationStartFinalWarmingUp;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeigherCalibration::calculateOptimalGainFactor()
{
	//gainFactor = 42L;

	long gf = 0;
	long optGF = 0;
	unsigned long optMSE;

	optMSE = 0X0FFFFFFFF;
/*
	if (max_weight == 50)  // This is read from the Weigher's Configuration (WgInquire)
	{
		gf = (steps[5]->getDesiredCounts() * 10000) /
				(steps[5]->getNonZeroedCounts() - steps[0]->getNonZeroedCounts());
	}
	else
*/
	{
		// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
		if (WgKind == WgUsbAWS)
		{
			/* Changed range that gain can be optimized from +/- 5 to +/- 500 of orig.
			   calculated gain. Both are 0.0005% of default gain. These comments are
			   per F. Albers who implemented new calibration schema during QX2. (haberrc) */
			unsigned long long int temp;	

			temp = (unsigned long long int)((unsigned long long int)steps[3]->getDesiredCounts() * ScalingConstant);
			gf = (long)(temp / (steps[3]->getNonZeroedCounts() - steps[0]->getNonZeroedCounts()));
		}
		else
			gf = (steps[3]->getDesiredCounts() * 10000) /
					(steps[3]->getNonZeroedCounts() - steps[0]->getNonZeroedCounts());
	}

	// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
	if (WgKind == WgUsbAWS)
		gf += 500; // to adjust gain by +/- 500 (0.0005%) to match old way
	else
		gf += 5;

	for (short i = 0; i < 10; ++i)
	{
		gainFactor = gf;
		calculateMeanSquareError();

		if (Debug::isTracing("WeigherCalibration"))
		{
			HobString h = Long::toString(gf);
			Debug::trace("WeigherCalibration", HobString("Calculating optimal gain factor - gf:") + h +
															HobString("  mse:") + HobString(Long::toString(meanSquareError)));
		}

		if ((unsigned long)meanSquareError < optMSE)
		{
			optMSE = (unsigned long)meanSquareError;
			optGF = gf;
		}
		// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
		if (WgKind == WgUsbAWS)
			gf -= 100;
		else
			--gf;
	}

	gainFactor = optGF;
	meanSquareError = optMSE;

	if (Debug::isTracing("WeigherCalibration"))
	{
		HobString h = Long::toString(gainFactor);
		Debug::trace("WeigherCalibration", HobString("***Calculated*** optimal gain factor - gf:") + h +
														HobString("  mse:") + HobString(Long::toString(meanSquareError)));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeigherCalibration::calculateOptimalGainFactorLinearFit()
{
	std::vector<double> x;
	std::vector<double> y;

	x.push_back( (double) steps[0]->getZeroedCounts() * counts_divisor );
	x.push_back( (double) steps[1]->getZeroedCounts() * counts_divisor );
	x.push_back( (double) steps[2]->getZeroedCounts() * counts_divisor );
	x.push_back( (double) steps[3]->getZeroedCounts() * counts_divisor );
	x.push_back( (double) steps[4]->getZeroedCounts() * counts_divisor );
	x.push_back( (double) steps[5]->getZeroedCounts() * counts_divisor );
	x.push_back( (double) steps[6]->getZeroedCounts() * counts_divisor );

	y.push_back( 0.0 );
	y.push_back( 100000.0 );
	y.push_back( 200000.0 );
	y.push_back( 300000.0 );
	y.push_back( 200000.0 );
	y.push_back( 100000.0 );
	y.push_back( 0.0 );

	for ( size_t i( 0 ); i < x.size(); ++i )
		Debug::trace("WeigherCalibration", HobString("step: ") + Long::toString(i) +
			HobString(" applied: ") + Double::toString(x[i], 8, 4) +
			HobString(" counts: ") + Double::toString(y[i], 10, 1) );

	// ---- //

	double x1;
	double x0;

	const size_t N( x.size() );

	double sumX( 0 );
	double sumY( 0 );
	double sumXY( 0 );
	double sumX2( 0 );

	for ( size_t i( 0 ); i < N; ++i )
	{
		sumX += x[i];
		sumY += y[i];
		sumXY += x[i] * y[i];
		sumX2 += pow( x[i], 2.0 );
	}

	const double xMean( sumX / N );
	const double yMean( sumY / N );
	const double denominator( sumX2 - sumX * xMean );

	x1 = (sumXY - sumX * yMean) / denominator;
	x0 = yMean - x1 * xMean;

	// ---- //

	// lbs/counts
	// should be ~0.0774622
	gainFactor = (long) std::round( x1 * 10000000.0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeigherCalibration::calculateMeanSquareError()
{
	meanSquareError = 0L;
	long baseCounts = steps[0]->getNonZeroedCounts();

	WeigherKind WgKind = SessionManager::getInstance()->getFirstWeigher();
	for (short i = 1; i < (short)steps.size() - 1; i++)
	{
		long stepError = steps[i]->calculateError(gainFactor, baseCounts, WgKind);
		meanSquareError += (stepError * stepError);
	}

	Debug::trace("WeigherCalibration", HobString("Calculated mean square error: ") + HobString(Long::toString(meanSquareError)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeigherCalibration::finalizeWeigherCalibrationParameters()
{
	Debug::trace("WeigherCalibration.State", "finalizeWeigherCalibrationParameters():  Changing to calibrationState:  CalibrationFinalWaitingForInquire");
	calibrationState = CalibrationFinalWaitingForInquire;

	/* In this case, we need to write the new configuration parameters back to the weigher's eep.
	 * The parameters are:  gain factor, center of maintenance zone, max weight (based on metric/avoir).
	 * The following parameters are set based on the weigher type (metric/avoir, scale factor):
	 *    auto zero maintenance limit, powerup maintenance zone limit, and operate maintenance zone limit.
	 *
	 * This was determined via the calibrate_hc11() function in cal_weigh.c (Ultima).
	 */

	if ( !calibrationSucceeded )
	{
		Debug::trace("WeigherCalibration.State", "CALIBRATION FAILED");

		centerOfMaintenanceZone = savedCenterOfMaintenanceZone;
		gainFactor = savedGainFactor;
	}
	else if ( WgCanKpc == WgKind )
	{
		// KPC uses raw counts, average zero at first and last steps
		centerOfMaintenanceZone = counts_divisor * (steps[0]->getNonZeroedCounts() + steps[6]->getNonZeroedCounts()) / 2;
	}
	else if ( WgUsbAWS == WgKind )
	{
		unsigned long long int temp;

		temp = (unsigned long long int)((unsigned long long int)steps[0]->getNonZeroedCounts() * gainFactor);
		centerOfMaintenanceZone = (long)(temp / ScalingConstant);
	}
	else
	{
		centerOfMaintenanceZone = steps[0]->getNonZeroedCounts() * gainFactor / 1000;
	}

	Debug::trace("WeigherCalibration", HobString("*** gain factor: ") + Long::toString(gainFactor) +
		HobString(" center of maint: ") + Long::toString(centerOfMaintenanceZone) );

	// Commented out the register for the message, we are now always registered for the WgInquire.
	//mySession->registerEvent(Weigher, WgInquire, false );

	// Send the message to the RTC to get the weigher configuration.
	WeigherGeneric wri(WgReqConfig, mySession );
	mySession->send( &wri, Messenger::DEST_RTC );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WeigherCalibration::resetTheWeigherAndTrack()
{
//	// Send the message to the RTC to put the weigher in weight tracking mode.
//	WeigherMode wgMode(mySession, ScaleWeightTrackingMode);
//	mySession->send(&wgMode, Messenger::DEST_RTC);

	// Send the message to the RTC to have the weigher do a scale warmup.
	if (WgKind == WgUsbAWS)
	{
		WeigherColdReset coldReset;		//	PTR 387, 388: Tom added a cold reset for the Atmel-based SAM-BA device.
		mySession->send(&coldReset, Messenger::DEST_RTC);
	}
	else
	{
		WeigherReset reset;
		mySession->send(&reset, Messenger::DEST_RTC);
	}

	// Send the message to the RTC to put the weigher in weight-tracking mode.
	WeigherMode wgMode(mySession, ScaleWeightTrackingMode);
	mySession->send(&wgMode, Messenger::DEST_RTC);
}

