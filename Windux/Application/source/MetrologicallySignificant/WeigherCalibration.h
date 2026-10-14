/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef WEIGHERCALIBRATION_H
#define WEIGHERCALIBRATION_H

#include "evolution.h"
#include "DataManager.h"
#include "WeigherCalibrationStep.h"
#include "WeigherCalibrationEnd.h"

// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
#include "Weigher.h"

class Session;
class SessionMessage;
class HfcWeigherStatus;
class HfcWeigherInquire;
class HfcWeigherSystemCalibrationSwitch;


class WeigherCalibration : public DataManager
{
	public:

		// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
		// From: 14316593ULL -> To: 5377310ULL per T. Fink (haberrc, 04-26-2010)
		static const unsigned long long int ScalingConstant = 5377310ULL;

		WeigherCalibration();
		WeigherCalibration(Session *session);
		virtual ~WeigherCalibration();

		void handleMessage(SessionMessage *msg);

		bool isWeigherCalibrationDone();


	private:

		Session *mySession;

		WeigherCalibrationStep *currentStep;
		std::vector<WeigherCalibrationStep *> steps;

		long centerOfMaintenanceZone;
		long gainFactor;
		long meanSquareError;

		bool userUpdatedGF;

		bool showingRemoveWeightMessage;

		long savedCenterOfMaintenanceZone;
		long savedGainFactor;

		bool calibrationSucceeded;
		bool switchPressed;

		bool usingAvoirWeights;

		int inquireTimerId = 100;
		
		// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
	    long counts_divisor;

	    void handleError( SessionMessage *msg );
		void handleStep(WeigherCalibrationStep *step);
		void handleEnd(WeigherCalibrationEnd *state);
		void handleWeighSystemCalibSwitch(HfcWeigherSystemCalibrationSwitch *msg);
		void handleWeighStatus(HfcWeigherStatus *status);
		void handleWeighInquire(HfcWeigherInquire *hwi);

		void processCurrentState();

		void init();
		void calculateOptimalGainFactor();
		void calculateMeanSquareError();

		void calculateOptimalGainFactorLinearFit();

		void finalizeWeigherCalibrationParameters();
		void resetTheWeigherAndTrack();

		void InquireTimeoutCallback();

		typedef enum CalibrationState
		{
			CalibrationInitial,
			CalibrationPromptingForTypeOfWeights,
			CalibrationInitiated,
			CalibrationWaitingForSwitch,
			CalibrationSwitchPressed,
			CalibrationWaitingForInquire1,
			CalibrationUpdatedConfig1,
			CalibrationStartWarmingUp,
			CalibrationWarmingUp,
			CalibrationStepping,
			CalibrationSteppingDone,
			CalibrationWaitingForInquire2,
			CalibrationUpdatedConfig2,
			CalibrationStartWarmingUp2,
			CalibrationWarmingUp2,
			CalibrationWaitingForInquire3,
			CalibrationUpdatedConfig3,
			CalibrationUpdatingGainFactor,
			CalibrationWaitingForWeightRemoval,
			CalibrationFinalWaitingForInquire,
			CalibrationFinalUpdatedConfig,
//			CalibrationFinalWaitingForZero,
//			CalibrationFinalFinishedWaitingForZero,
			CalibrationStartFinalWarmingUp,
			CalibrationFinalWarmingUp,
			CalibrationCompleted
		}  CalibrationState_t;

		CalibrationState calibrationState;

		// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
		WeigherKind WgKind;

		bool weigherIsWithinZeroQuarterGrad;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif //WEIGHERCALIBRATION_H
