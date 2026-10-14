/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef BACKUPRESTOREMANAGER_H
#define BACKUPRESTOREMANAGER_H

#include "CEvolMsgReceiver.h"
#include "DBSession.h"
//#include "DataManager.h"

#include "BackupRestore.h"
#include "BackupRestoreEvent.h"
#include "BackupRestoreSelections.h"
#include "HfcResponse.h"
#include "UltimaDiskEvent.h"
#include "Printer.h"
#include "AllDepts.h"

#include <list>
#include <cstdio>

#include "HobString.h"

class Session;
class SessionMessage;
class CMReaderWriter;
class CMFile;

class BackupRestoreProgress;
class BackupRestoreErrors;

class Dept;
class Product;
class Class;
class Nutrifacts;
class ProductText;
class LabelType;
//class DBLabelsRM;
class Graphic;
class Category;
class Special;
class Dept;

class COOLCategory;
class COOLCountry;
class COOLProductionStep;
class COOLPretext;
class COOLText;
class COOLShortlist;
class COOLCategoryCountry;
class COOLCategoryProdStep;
class COOLCategoryPretext;

class SoftkeyGroup;
class SoftkeyStyle;
class SoftkeyFeature;
class Softkey;

class ExpirationLabel;

class HobOperator;

class StreamingGraphic;

class GenericLabelText;

class Configuration;

class HobEmailAlert;

class CuttingTest;
class CutTestTotal;

namespace hfc
{
	class Batch;
	class ClassTotal;
	class COOLTotal;
	class HourlyTotal;
	class OperatorChange;
	class OperatorTotal;
	class ProductTotal;
	class Scale;
	class SynchronizationChange;
	class Transaction;
	class TransactionType;
}

class PresentationMethodManager;
class PresentationMethod;

class ProductStatistic;

class HobList;
class HfcEvolWrapperPackageTimingsPointer;
class HfcEvolWrapperPackageTimings;
class EvolWrapperTimingsManager;

class Recall;

class Device;
class ProductTextCfg;
class HfcScannerProductRead;



class BackupRestoreManager : public DBSession
{
	public:

		BackupRestoreManager(Session *session, SessionMessage *msg = 0);
		~BackupRestoreManager();

		static char *autoGenerateName(Session *session);

		int getEvent() const;
		const BackupRestoreSelections &getSelections() const;

		short getSessionType() const;


	protected:

		virtual void noMessageFromMessengerProcessing();  // is virtual override
		virtual void sessionMessageProcess( SessionMessage * msg );  // is virtual override
		virtual void initializationBeforeRun();  // override from AbstractSession
		virtual void finalizationAfterRun();  // override from AbstractSession
		virtual void quitGracefully();  // override from AbstractSession


	private:

		void handleMessage(SessionMessage *msg);

		Session *mySession;
		int event;

		BackupRestoreSelections selections; 				//hfcObject
		std::list<BRSubStates> *subStateList;				//list of subStates to be completed
		bool doneWithSubState;

		BRState state; 										//current state of the task
		BRSubStates subState;								//what is currently happening within a state

		BRMode mode;										//mode of the current task
		BRFormat format;									//the format to use for backup

		bool startingNewFile;							// indicates that we are starting to process a new file (done with the previous one)

		HobString fileName;									//name of file to be used for the backup or restore

		CMFile *fpIn; 
		CMReaderWriter *frwIn;
		CMFile *fpOut; 
		CMReaderWriter *frwOut;

		bool fileWriteErrorOccurred;

		bool displayStats;

							 								//counters for the User Interface
		int currentRecordCount;								//the current record being processed
		int totalRecords;									//the total amount of records for the current record manager		
		int currentRecordsIgnored;							//the current record being ignored

		int totalTaskRecords;								//the total amount of records for the entire backup or restore
		int progressTotal;									//current count of all records processed 
		int progressTotalIgnored;							//current count of all records ignored 

		int totalRecordErrors;								//keep track of the number of bad record in the bad record scan

		HobList *hobListFI;								   	//this list contains the BRFileIndicator of the offending record
		HobList *hobList;									//this list contains the record number and description of the offending record
															//both lists are a one for one match BRFIProducts -- 100 BEEF 
		BackupRestoreProgress *progress;
		SessionMessage *progMsg;

		BackupRestoreErrors *errorsLists;
		SessionMessage *errorsMsg;

		int myBackupID_in;
		int myBackupID_out;

		bool writeUserTar;
		bool writeSystemTar;

		bool verifySoftkeyGroupReservedRecords;                  // We take note (true) that we need to verify SoftkeyGroup reserved records

		bool pausedThreads;

		//SessionMessage *restoreRecordMsg;

		bool finishedWithClasses;
		bool finishedWithNutrifacts;
		bool finishedWithExpandedText;
		bool finishedWithSpecialMessages;
		bool finishedWithProductNotes;
		bool finishedWithProductText5;
		bool finishedWithProductText6;
		bool finishedWithProductText7;
		bool finishedWithProductText8;
		bool finishedWithProductText9;
		bool finishedWithTransactions;
		bool finishedWithProducts;
		bool finishedWithDepts;

		Dept *dept;
		Product *product;
		Class *classes;
		Nutrifacts *nutriFact;
		ProductText *productText;
		LabelType *labelType;
		Graphic *graphic;
		Category *category;
		Special *special;

		COOLCategory *coolCategory;
		COOLCountry *coolCountry;
		COOLProductionStep *coolProductionStep;
		COOLPretext *coolPretext;
		COOLText *coolText;
		COOLShortlist *coolShortlist;
		COOLCategoryCountry *coolCategoryCountry;
		COOLCategoryProdStep *coolCategoryProdStep;
		COOLCategoryPretext *coolCategoryPretext;

		SoftkeyGroup *softkeyGroup;
		SoftkeyStyle *softkeyStyle;
		SoftkeyFeature *softkeyFeature;
		Softkey *softkey;

		ExpirationLabel *expirationLabel;

		HobOperator *hobOperator;		
		Util::CSmartPtr<hfc::OperatorChange> operatorChange_;

		StreamingGraphic *streamingGraphic;

		GenericLabelText *genericLabelText;
		bool genericLabelTextUpdated;

		Configuration *configuration;							//scaleConfiguration

		Util::CSmartPtr<hfc::Transaction> transaction_;
		Util::CSmartPtr<hfc::TransactionType> transactionType_;
		Util::CSmartPtr<hfc::ProductTotal> productTotal_;
		Util::CSmartPtr<hfc::ClassTotal> classTotal_;
		Util::CSmartPtr<hfc::OperatorTotal> operatorTotal_;
		Util::CSmartPtr<hfc::HourlyTotal> hourlyTotal_;
		Util::CSmartPtr<hfc::COOLTotal> coolTotal_;
		Util::CSmartPtr<CutTestTotal> cutTestTotal_;
		Util::CSmartPtr<CuttingTest> cuttingTest_;

		HobEmailAlert *emailAlert;

		hfc::Scale *scale;
		hfc::Batch *batch;
		hfc::SynchronizationChange *synchChange;

		PresentationMethodManager *presentationMethodManager;
		PresentationMethod *presentationMethod;

		ProductStatistic *productStatistic;

		Recall *recall;

		Device *device;
		ProductTextCfg *productTextCfg;
		HfcScannerProductRead *hfcScannerProductRead;

		BRFileIndicator previousFileIndicator;

		bool isTracing;

		// For synchronization, this is the batch id of the batch created for restoring or merging data, when a batch is created.
		int synchBatchId;


		EvolWrapperTimingsManager *wrapperTimingsManager;
		HfcEvolWrapperPackageTimingsPointer *wrPackageTimingsPtr;
		HfcEvolWrapperPackageTimings *pMyWrapperTimings;
		bool wrapperTimingsReady;

		unsigned int _printerType_;
		unsigned int _printerCfgRetries_;

		unsigned int _weigherCfgRetries_;

		AllDepts allDepts;
		HobList deptNumList;

		void init();
		void initWithSelections();
		void initiateReadAll(BRSubStates whichData, BRSubStates whichDataNext = BRIdleSubState);
		void initiateReadAll(EvolDeviceType device, UINT8 event);
		void initiateVersionIndependentBackup();
		void initialBackup();
		void handleBackup();								//handlers for the different states
		void processBackupSubState();
		void backupARecord(SessionMessage *msg);

		void backupProducts();
		void backupProductDefaults();
		void backupProductDefault(short productType, HobRecordNum deptNum);
		void backupClasses();
		void backupNutriData();
		void backupProductTexts(BRSubStates subState);
		void backupOneProductText(ProductTextManager *prodTextMgr, EvolDeviceType dev, BRFileIndicator fileInd);
		void backupTransactionTypes();
		void backupLabelTypes();
		void backupLabelTextNames();
		void backupStoreData();
		void backupScaleConfig();
		void backupScaleStatus();
		void backupScaleStatistics();
		void backupOperatorData();
		void backupOperatorNote();
		void backupOperatorChanges();
		void backupDepts();

		void backupWrapperRequestSettings();
		void backupWrapperSettings();

		void backupPrinterSettings();
		void backupWeigherSettings();
				
		void backupProductTotals();
		void backupClassTotals();
		void backupOperatorTotals();
		void backupHourlyTotals();
		void backupCutTestTotals();
		void backupCuttingTests();
		void backupTransactions();
		void backupNotClearedTransactions();

		void backupCategories();
		void backupPresentationMethods();
		void backupSpecials();
		void backupStreamingGraphics();
		void backupProductStatistics();

		void backupEmailAlerts();

		void backupCOOLCategories();
		void backupCOOLCountries();
		void backupCOOLProductionSteps();
		void backupCOOLPretexts(bool forUltima = false);
		void backupCOOLTexts(bool forUltima = false);
		void backupCOOLShortlists(bool forUltima = false);
		void backupCOOLTotals(bool forUltima = false);

		void backupCOOLCategoryCountries();
		void backupCOOLCategoryCountriesForUltima();
		void backupCOOLCategoryProductionSteps();
		void backupCOOLCategoryProductionStepsForUltima();
		void backupCOOLCategoryPretexts();

		void backupSoftkeyGroups();
		void backupSoftkeyStyles();
		void backupSoftkeyFeatures();
		void backupSoftkeys();

		void backupExpirationLabels();

		void backupLabelGraphics();
		void backupProductGraphics();
		void backupCategoryGraphics();
		void backupSpecialsGraphics();
		void backupSoftkeyGraphics();
		void backupStreamingGraphicsGraphics();

		void backupSynchronizationConfig();

		void backupScales();
		void backupBatches();
		void backupSynchronizationChanges();

		void backupRecalls();

		void backupDevices();
		void backupProductTextCfgs();
		void backupBarcodeScanners();

		void backupUltimaHeader();
		void backupUltimaFooter();


		void handleProductTextMessage(SessionMessage *msg, ProductTextManager *productTextManager, ProdSupportTextType type, BRFileIndicator fileInd);


		void handleRestore();
		void handleProcessFile();
		void clearSelections();
		void requestDBCompression();

		void initialBadRecord();
		void handleBadRecordScan();

		void handleIntranet();

		void updateUI(BRGroupIndicator group, BRFileIndicator file, int resetTo = 1, int resetIgnoredTo = 0);
		void updateUI();
		void updateUIDialog(bool displayDialog, ErrorEvent errorDevice, int errorCode, short responseStyle = HfcResponse::RESPONSESTYLE_OK);

		void requestWrapperTimings();
		void requestPrinterConfig(PrinterType printerType);

		void requestWeigherConfig();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif // BACKUPRESTOREMANAGER_H
