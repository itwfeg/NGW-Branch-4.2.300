#ifndef EVOLWRAPPERTIMINGSMANAGER_H
#define EVOLWRAPPERTIMINGSMANAGER_H



#include "HfcEvolWrapperPackageTimings.h"
#include "HfcEvolWrapperPackageTimingsPointer.h"
#include "EvolWrapperRequestPackageTimings.h"
#include "HfcEvolWrapperPackageDesc.h"


class Session;
class SessionMessage;



class EvolWrapperTimingsManager
{
	public:

		static const int _absMinLength = 40;	//	 4.0
		static const int _absMinWidth = 40;		//	 4.0
		static const int _absMinLip = 5;		//	 0.5

		static const int _absMaxLength = 110;	//	11.0
		static const int _absMaxWidth = 160;	//	16.0
		static const int _absMaxLip = 60;		//	 6.0

		static const int _calcMinLength = 5;	//	 0.5
		static const int _calcMinWidth = 5;		//	 0.5

		static const int _calcMaxLength = 15;	//	 1.5
		static const int _calcMaxWidth = 15;	//	 1.5
		static const int _calcMaxLip = 2;		//	 0.2

		// --- Types and subtypes ---
		typedef enum
		{
			ViewUnknown,
			ViewCurrentTimings,
			ViewDefaultTimings
		} ViewTimingsMode;
		typedef enum
		{
			CycleWrapperUnknown,
			CycleWrapperAutoDetect,
			CycleWrapperAutoConfig,
			CycleWrapperVerify
		} CycleWrapperMode;

		// --- Constructors and destructor ---
		EvolWrapperTimingsManager(Session *session = 0, bool forBackupRestore = false);
		~EvolWrapperTimingsManager();

		void abortWaitForWrapperMsg();

		// --- Processing methods ---
		void handleMessage(SessionMessage *msg);
		HfcEvolWrapperPackageTimings *getEvolWrapperPackageTimings();

		bool deleteMe();

		void setFirstPass() { firstPass_ = true; }

	private:
		// --- Instance data members ---
		Session* session_;									// Session that instanciated us

		ViewTimingsMode currentViewMode_;					// Current vs. Default Settings mode

		CycleWrapperMode cycleWrapperMode_;					// Cycle wrapper operation in course
		int referencePrecedence_;							// The reference wrap box in the cycle operation
		int referenceHeight_;								// The reference height box in the cycle operation

		bool updateTrayTypesToUI_;							// UI needs updated tray types
		bool settingsChanged_;								// Wrapper needs updating (via TEMPORARY)
		bool sendPermanentChanges_;							// Wrapper needs updating (via PERMANENT)
		bool wrapperIsFinished;								// Wrapper is done updating Package Timings
		bool reqCurrentWhenWrapperIsFinished_;				//	Request Current timings after the wrapper is done saving timings. Used after a Reset to defaults.
		bool reqDefaultWhenWrapperIsFinished_;				//	Request Default timings after the wrapper is done saving timings.
		bool exitWhenWrapperIsFinished;
		bool doDeleteMe;
		bool updateTrayTypesToWrMgr_;						// Wrapper Manager needs updated tray types
		bool firstPass_;									// To save the Wrapper timings to Thumbdrive
		bool autoConfigFirstPass;							// On firstPass use the PackageDesc info to get the Min and Max values.
		HfcEvolWrapperPackageTimings lastWrapperTimings_;	// Working copy of wrapper settings
		HfcEvolWrapperPackageTimings saveWrapperTimings_;	// Settings saved in case we need to restore
		bool backupRestore;									// To save the Wrapper timings to Thumbdrive
		std::string newTrayName;
		int eventWhenWrapperIsFinished;
		bool lastSaveWasPermanent_;
		HfcEvolWrapperPackageDesc lastPackageDesc_;		// Copy of the last pkg description message so we can set min and max values.


		// --- Processing methods ---
		void init();

		void requestPackageTimingsFromWrapper(EvolWrapperRequestPackageTimings::TimingsType timingsType);
		void sendPackageTimingsToWrapper(bool makePermanent = false);

		void sendAllTrayTypesToUI();
		void sendWrapBoxToUI(int precedence, const WrapBox& wrapBox, unsigned int height = 0);
		void sendHeightBoxToUI(int precedence, const WrapBox::HeightBox& heightBox);
		void cycleWrapperAutoDetect();
		void cycleWrapperAutoConfig();
		void cycleWrapperVerify(int precedence, unsigned int height);
		bool timingsMatchViewMode(const HfcEvolWrapperPackageTimings& timings); // Need to finish
		bool validateDeletingTrayType(int precedence);
		bool validateDeletingHeightBox(const WrapBox& wrapBox, unsigned int height);

		void setupForWrapperIsFinished();
		void sendPackageTimingsToBackup();
		void clearWaitForWrapperMsg();

		bool setMinTrayValues(WrapBox &theBox);
		bool setMaxTrayValues(WrapBox &theBox);
		bool setMinAndMaxTrayValues(WrapBox &theBox);
		int getMinTrayLengthValue(int actualLength);
		int getMinTrayWidthValue(int actualWidth);
		int getMaxTrayLengthValue(int actualLength);
		int getMaxTrayWidthValue(int actualWidth);
		int getMaxTrayLipValue(int actualLip);


		static const int _InvalidPrecedence_ = -1;
		static const bool _UsingWrapper_ = true;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* EVOLWRAPPERTIMINGSMANAGER_H */
