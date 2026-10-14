#ifndef SYNCHRONIZATIONMANAGERUNITTESTER_H
#define SYNCHRONIZATIONMANAGERUNITTESTER_H



namespace synchronization
{


	class SecondarySynchronizationManager;


	class SynchronizationManagerUnitTester
	{
	public:
		/* --- Constructors and destructor --- */
		SynchronizationManagerUnitTester();
		virtual ~SynchronizationManagerUnitTester();


		bool runTestsPrimary();
		bool runTestsSecondary();

	private:
		bool runCreateUserInterfaceBatchTest(SecondarySynchronizationManager* ssm, int* batchId);
		bool runCreateHQBatchTest(SecondarySynchronizationManager* ssm, int* batchId);
		bool runCompleteBatchTest(SecondarySynchronizationManager* ssm, int batchId);
		bool runAbortBatchTest(SecondarySynchronizationManager* ssm, int batchId);
		bool runApplyBatchTest(SecondarySynchronizationManager* ssm, int batchId);
		bool runDeleteBatchTest(SecondarySynchronizationManager* ssm, int batchId);
		bool runProcessBatchTest(SecondarySynchronizationManager* ssm, int batchId);
	};


}



#endif /* SYNCHRONIZATIONMANAGERUNITTESTER_H */
