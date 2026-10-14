#include "SynchronizationManagerUnitTester.h"

#include "PrimarySynchronizationManager.h"
#include "SecondarySynchronizationManager.h"
#include "SynchronizationManagerProxy.h"

#include "SynchronizationChange.h"

#include "ProductDevEvent.h"


#include "Messages/ProcessUserFileMessage.h"
#include "Messages/ProcessUserFileProcessedMessage.h"
#include "Messages/CreateBatchMessage.h"
#include "Messages/CreateBatchProcessedMessage.h"
#include "Messages/AddRecordToBatchMessage.h"
#include "Messages/AbortBatchMessage.h"
#include "Messages/CompleteBatchMessage.h"
#include "Messages/ApplyBatchMessage.h"
#include "Messages/DeleteBatchMessage.h"
#include "Messages/DeleteBatchProcessedMessage.h"


#include "ProductDefault.h"
#include "Category.h"
#include "Class.h"
#include "Special.h"
#include "SoftkeyGroup.h"
#include "SoftkeyFeature.h"
#include "SoftkeyStyle.h"
#include "Softkey.h"
#include "CMReaderWriterEvolTag.h"


#include "util/e_string/utilstring.h"
#if defined( WIN32 )
    #include "winsock2.h"
#endif



namespace synchronization
{

    SynchronizationManagerUnitTester::SynchronizationManagerUnitTester()
    {
#if !defined ( __linux__ )
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;

        wVersionRequested = MAKEWORD( 2, 0 );

        err = WSAStartup( wVersionRequested, &wsaData );

        Assert(err == 0, Util::CNamedException( L"Socket Initialize error" ) );

        Assert(( LOBYTE( wsaData.wVersion ) == 2 ||HIBYTE( wsaData.wVersion ) == 0 ), Util::CNamedException( L"Socket Version Error" ) );
#endif      // !__linux__
    }


    SynchronizationManagerUnitTester::~SynchronizationManagerUnitTester()
    {
    }


    bool SynchronizationManagerUnitTester::runTestsPrimary()
    {
        bool status = true;
        //		int batchId1, batchId2;

        PrimarySynchronizationManager* ssm = new PrimarySynchronizationManager(true);

        ProductDefault::verifyReservedRecords(ssm);
        Category::verifyReservedRecords(ssm);
        Class::verifyReservedRecords(ssm);
        Special::verifyReservedRecords(ssm);
        SoftkeyGroup::verifyReservedRecords(ssm);
        SoftkeyFeature::verifyReservedRecords(ssm);
        SoftkeyStyle::verifyReservedRecords(ssm);
        Softkey::verifyReservedRecords(ssm);



        //		delete ssm;
#if defined(LINUX_FIX)
        PAL_absclass::PAL_sleep( 60*60*1000 );
#else
        Sleep(60*60*1000);
#endif

        return status;
    }


    bool SynchronizationManagerUnitTester::runTestsSecondary()
    {
        bool status = true;
        int batchId1, batchId2;

        SecondarySynchronizationManager* ssm = new SecondarySynchronizationManager(true, false, "JUANCSJ1XP");

        ProductDefault::verifyReservedRecords(ssm);
        Category::verifyReservedRecords(ssm);
        Class::verifyReservedRecords(ssm);
        Special::verifyReservedRecords(ssm);
        SoftkeyGroup::verifyReservedRecords(ssm);
        SoftkeyFeature::verifyReservedRecords(ssm);
        SoftkeyStyle::verifyReservedRecords(ssm);
        Softkey::verifyReservedRecords(ssm);

        if ( !runCreateUserInterfaceBatchTest(ssm, &batchId1) )
            status = false;

        //		if ( !runCompleteBatchTest(ssm, batchId1) )
        //			status = false;

        if ( !runProcessBatchTest(ssm, batchId1) )
            status = false;

        if ( !runCreateHQBatchTest(ssm, &batchId2) )
            status = false;

        if ( !runCompleteBatchTest(ssm, batchId2) )
            status = false;

        /*		if ( !runAbortBatchTest(ssm, batchId2) )
                        status = false;

                        if ( !runApplyBatchTest(ssm, batchId1) )
                        status = false;

                        if ( !runDeleteBatchTest(ssm, batchId1) )
                        status = false;
                        */
        //		delete ssm;
#if defined(LINUX_FIX)
        PAL_absclass::PAL_sleep( 5*60*1000 );
#else
        Sleep(5*60*1000);
#endif

        return status;
    }


#define US "\x1F"

    bool SynchronizationManagerUnitTester::runCreateUserInterfaceBatchTest(SecondarySynchronizationManager* ssm, int* batchId)
    {
        SynchronizationManagerProxy* proxy = ssm->getProxy();

        *batchId = proxy->createBatch(batchId, hfc::Batch::OriginLocal, hfc::Batch::TypeUser);

        //		Util::CSmartPtr<CreateBatchProcessedMessage> message =
        //			Util::CSmartPtr<CreateBatchProcessedMessage>::Cast(ssm->processCreateBatchMessage(Util::CSmartPtr<CreateBatchMessage>(new CreateBatchMessage(hfc::Batch::OriginLocal, hfc::Batch::TypeUser))));
        //		if ( message.notNull() )
        //			*batchId = message->getBatchId();


        Debug::trace("SynchronizationManager.unitTester", "After CreateBatch, batchId = " +
                util_string::inttostring(*batchId));

        CMReaderWriterEvolTag();


        proxy->addRecordToBatch(*batchId, Util::CSmartPtr<hfc::SynchronizationChange>(new 
                    hfc::SynchronizationChange(ProductDev, ProductWrite, "RT89" US "p#1" US "dtGROUND BEEF 1" US "l1103" US "rc999999" US "tf0" US "u#2" US "up12345" US "u$199")));
        proxy->addRecordToBatch(*batchId, Util::CSmartPtr<hfc::SynchronizationChange>(new 
                    hfc::SynchronizationChange(ProductDev, ProductWrite, "RT89" US "p#2" US "dtGROUND BEEF 2" US "l1103" US "rc999999" US "tf0" US "u#2" US "up12345" US "u$299")));
        proxy->addRecordToBatch(*batchId, Util::CSmartPtr<hfc::SynchronizationChange>(new 
                    hfc::SynchronizationChange(ProductDev, ProductWrite, "RT89" US "p#3" US "dtGROUND BEEF 3" US "l1103" US "rc999999" US "tf0" US "u#2" US "up12345" US "u$399")));
        proxy->addRecordToBatch(*batchId, Util::CSmartPtr<hfc::SynchronizationChange>(new 
                    hfc::SynchronizationChange(ProductDev, ProductWrite, "RT89" US "p#4" US "dtGROUND BEEF 4" US "l1103" US "rc999999" US "tf0" US "u#2" US "up12345" US "u$499")));
        proxy->addRecordToBatch(*batchId, Util::CSmartPtr<hfc::SynchronizationChange>(new 
                    hfc::SynchronizationChange(ProductDev, ProductWrite, "RT89" US "p#5" US "dtGROUND BEEF 5" US "l1103" US "rc999999" US "tf0" US "u#2" US "up12345" US "u$599")));
        proxy->addRecordToBatch(*batchId, Util::CSmartPtr<hfc::SynchronizationChange>(new 
                    hfc::SynchronizationChange(ProductDev, ProductWrite, "RT89" US "p#6" US "dtGROUND BEEF 6" US "l1103" US "rc999999" US "tf0" US "u#2" US "up12345" US "u$699")));


        return true;
    }


    bool SynchronizationManagerUnitTester::runCreateHQBatchTest(SecondarySynchronizationManager* ssm, int* batchId)
    {
        //		Util::CSmartPtr<CreateBatchProcessedMessage> message =
        //			Util::CSmartPtr<CreateBatchProcessedMessage>::Cast(ssm->processCreateBatchMessage(Util::CSmartPtr<CreateBatchMessage>(new CreateBatchMessage(hfc::Batch::OriginLocal, hfc::Batch::TypeHQ))));

        SynchronizationManagerProxy* proxy = ssm->getProxy();

        proxy->createBatch(batchId, hfc::Batch::OriginLocal, hfc::Batch::TypeHQ);

        //		if ( message.notNull() )
        //		{
        //			*batchId = message->getBatchId();

        Debug::trace("SynchronizationManager.unitTester", "After CreateBatch, batchId = " +
                util_string::inttostring(*batchId));

        CMReaderWriterEvolTag();


        proxy->addRecordToBatch(*batchId, Util::CSmartPtr<hfc::SynchronizationChange>(new 
                    hfc::SynchronizationChange(ProductDev, ProductWrite, "RT89" US "p#11" US "dtGROUND BEEF 1" US "l1103" US "rc999999" US "tf0" US "u#2" US "up12345" US "u$199")));
        proxy->addRecordToBatch(*batchId, Util::CSmartPtr<hfc::SynchronizationChange>(new 
                    hfc::SynchronizationChange(ProductDev, ProductWrite, "RT89" US "p#12" US "dtGROUND BEEF 2" US "l1103" US "rc999999" US "tf0" US "u#2" US "up12345" US "u$299")));
        proxy->addRecordToBatch(*batchId, Util::CSmartPtr<hfc::SynchronizationChange>(new 
                    hfc::SynchronizationChange(ProductDev, ProductWrite, "RT89" US "p#13" US "dtGROUND BEEF 3" US "l1103" US "rc999999" US "tf0" US "u#2" US "up12345" US "u$399")));
        proxy->addRecordToBatch(*batchId, Util::CSmartPtr<hfc::SynchronizationChange>(new 
                    hfc::SynchronizationChange(ProductDev, ProductWrite, "RT89" US "p#14" US "dtGROUND BEEF 4" US "l1103" US "rc999999" US "tf0" US "u#2" US "up12345" US "u$499")));
        proxy->addRecordToBatch(*batchId, Util::CSmartPtr<hfc::SynchronizationChange>(new 
                    hfc::SynchronizationChange(ProductDev, ProductWrite, "RT89" US "p#15" US "dtGROUND BEEF 5" US "l1103" US "rc999999" US "tf0" US "u#2" US "up12345" US "u$599")));
        proxy->addRecordToBatch(*batchId, Util::CSmartPtr<hfc::SynchronizationChange>(new 
                    hfc::SynchronizationChange(ProductDev, ProductWrite, "RT89" US "p#16" US "dtGROUND BEEF 6" US "l1103" US "rc999999" US "tf0" US "u#2" US "up12345" US "u$699")));
        //			ssm->processAddRecordToBatchMessage(Util::CSmartPtr<AddRecordToBatchMessage>(new
        //					AddRecordToBatchMessage(*batchId, Util::CSmartPtr<hfc::SynchronizationChange>(new 
        //					hfc::SynchronizationChange(ProductDev, ProductWrite, "RT89" US "p#11" US "dtGROUND BEEF 1" US "l1103" US "rc999999" US "tf0" US "u#2" US "up12345" US "u$199")))));
        //			ssm->processAddRecordToBatchMessage(Util::CSmartPtr<AddRecordToBatchMessage>(new
        //					AddRecordToBatchMessage(*batchId, Util::CSmartPtr<hfc::SynchronizationChange>(new 
        //					hfc::SynchronizationChange(ProductDev, ProductWrite, "RT89" US "p#12" US "dtGROUND BEEF 2" US "l1103" US "rc999999" US "tf0" US "u#2" US "up12345" US "u$299")))));
        //			ssm->processAddRecordToBatchMessage(Util::CSmartPtr<AddRecordToBatchMessage>(new
        //					AddRecordToBatchMessage(*batchId, Util::CSmartPtr<hfc::SynchronizationChange>(new 
        //					hfc::SynchronizationChange(ProductDev, ProductWrite, "RT89" US "p#13" US "dtGROUND BEEF 3" US "l1103" US "rc999999" US "tf0" US "u#2" US "up12345" US "u$399")))));
        //			ssm->processAddRecordToBatchMessage(Util::CSmartPtr<AddRecordToBatchMessage>(new
        //					AddRecordToBatchMessage(*batchId, Util::CSmartPtr<hfc::SynchronizationChange>(new 
        //					hfc::SynchronizationChange(ProductDev, ProductWrite, "RT89" US "p#14" US "dtGROUND BEEF 4" US "l1103" US "rc999999" US "tf0" US "u#2" US "up12345" US "u$499")))));
        //			ssm->processAddRecordToBatchMessage(Util::CSmartPtr<AddRecordToBatchMessage>(new
        //					AddRecordToBatchMessage(*batchId, Util::CSmartPtr<hfc::SynchronizationChange>(new 
        //					hfc::SynchronizationChange(ProductDev, ProductWrite, "RT89" US "p#15" US "dtGROUND BEEF 5" US "l1103" US "rc999999" US "tf0" US "u#2" US "up12345" US "u$599")))));
        //			ssm->processAddRecordToBatchMessage(Util::CSmartPtr<AddRecordToBatchMessage>(new
        //					AddRecordToBatchMessage(*batchId, Util::CSmartPtr<hfc::SynchronizationChange>(new 
        //					hfc::SynchronizationChange(ProductDev, ProductWrite, "RT89" US "p#16" US "dtGROUND BEEF 6" US "l1103" US "rc999999" US "tf0" US "u#2" US "up12345" US "u$699")))));
        //		}

        return true;
    }


    bool SynchronizationManagerUnitTester::runCompleteBatchTest(SecondarySynchronizationManager* ssm, int batchId)
    {
        //		ssm->processCompleteBatchMessage(Util::CSmartPtr<CompleteBatchMessage>(new CompleteBatchMessage(batchId)));

        SynchronizationManagerProxy* proxy = ssm->getProxy();

        proxy->completeBatch(batchId);

        return true;
    }


    bool SynchronizationManagerUnitTester::runProcessBatchTest(SecondarySynchronizationManager* ssm, int batchId)
    {
        Util::CSmartPtr<ProcessUserFileProcessedMessage> message =
            Util::CSmartPtr<ProcessUserFileProcessedMessage>::Cast(ssm->processProcessUserFileMessage(Util::CSmartPtr<ProcessUserFileMessage>(new ProcessUserFileMessage(batchId))));

        return message.notNull();
    }


    bool SynchronizationManagerUnitTester::runAbortBatchTest(SecondarySynchronizationManager* ssm, int batchId)
    {
        ssm->processAbortBatchMessage(Util::CSmartPtr<AbortBatchMessage>(new AbortBatchMessage(batchId)));

        return true;
    }


    bool SynchronizationManagerUnitTester::runApplyBatchTest(SecondarySynchronizationManager* ssm, int batchId)
    {
        ssm->processApplyBatchMessage(Util::CSmartPtr<ApplyBatchMessage>(new ApplyBatchMessage(batchId)));

        return true;
    }


    bool SynchronizationManagerUnitTester::runDeleteBatchTest(SecondarySynchronizationManager* ssm, int batchId)
    {
        ssm->processDeleteBatchMessage(Util::CSmartPtr<DeleteBatchMessage>(new DeleteBatchMessage(batchId, DeleteBatchProcessedMessage::DeleteChangesAndBatch)));

        return true;
    }


}
