#ifndef DATABASESYNCHRONIZER_H
#define DATABASESYNCHRONIZER_H



#include "Synchronizer.h"
#include "CTLSThread.h"

//#include "EvolMessageWrapper.h"
#include "Commands/Command.h"

#include "CSmartPtr.h"

#include "ProductText.h"

class EvolMessageWrapper;


namespace hfc
{
	class SynchronizationChange;
}



namespace synchronization
{


	class QueueManager;


	class DatabaseSynchronizer: public Synchronizer, public Util::CRunnableClass
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		DatabaseSynchronizer(Hobart::CEvolMsgReceiver* syncManager);
		~DatabaseSynchronizer();


		/* --- Getter and setter methods --- */


		/* --- Instance Function members --- */
		DWORD Run();
		void waitUntilRunThreadEnds();

		void processEvent(Util::CSmartPtr<EvolMessageWrapper> & msg);
		void processCommand(Util::CSmartPtr<Command> command);

		/* Use cases */
		/* "Use Case 5.54: Executing command". */
		Util::CSmartPtr<EvolMessage> processExecuteCommandMessage(Util::CSmartPtr<ExecuteCommandMessage> msg);

		static int applyChangeToDB(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change, Util::CSmartPtr<hfc::Batch> batch);

		// virtual from HfcManagerContainer
		void dispatchMessage(EvolMessage *message);



	protected:
		/* Use cases */
		/* "Use Case 7.33: Purging batches". */
		int usecasePurgingBatches();
		/* "Use Case 7.42: Aborting batch". */
		int usecaseAbortingBatch(int batchId);
		/* "Use Case 7.45: Applying batch". */
		int usecaseApplyingBatch(int batchId);
		/* "Use Case 7.46: Deleting batch". */
		int usecaseDeletingBatch(int batchId);
		/* "Use Case 7.47: Applying and deleting batch". */
		int usecaseApplyingDeletingBatch(int batchId);
		/* "Use Case 7.54: Executing command". */
		int usecaseExecutingCommand(Util::CSmartPtr<Command> command);
		/* "Use Case 7.58: Resuming processing". */
		int usecaseResumingProcessing();
		/* "Use Case 7.59: Pausing processing". */
		int usecasePausingProcessing(int reason);


	private:
		/* --- Instance Data members --- */
		Util::CSmartPtr<Util::CTLSThread> thread_;
		Util::CSmartPtr<Command> command_;


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		DatabaseSynchronizer(const DatabaseSynchronizer& sm);


		/* --- Instance function members --- */
		/* Operators */
		// Operator asignment is not available
		DatabaseSynchronizer& operator = (const DatabaseSynchronizer& rhs);

		static int applyCategoryChange         (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applyClassChange            (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applyConfigurationChange    (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applyCOOLChange             (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applyEmailAlertsChange      (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applyExpandedTextChange     (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applyGenericTextChange      (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applyGraphicChange          (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applyLabelTypeChange        (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applyMarqueeChange          (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applyNutrifactsChange       (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applyOperatorChange         (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applyOperatorNotesChange    (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applyProductChange          (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applyProductDefaultsChange  (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applyProductNotesChange     (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applySoftkeyChange          (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applySpecialMessageChange   (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applySpecialsChange         (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applyTransactionTypeChange  (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);
		static int applyTransactionChange      (HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change, int scaleID);
		static int applyProductTextChange      (ProdSupportTextType type, HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change);

		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static const std::string _dataTypeName_;
#endif
	};


}



#endif /* DATABASESYNCHRONIZER_H */
