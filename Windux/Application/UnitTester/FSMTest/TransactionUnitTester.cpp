#include "TransactionUnitTester.h"


#include "TransactionManager.h"
#include "Transaction.h"

#include "DBManager.h"
#include "DBTransactionsRM.h"
#include "DBTransaction.h"

#include "util/time/datetime.h"
#include "util/e_string/utilstring.h"



namespace hfc
{


    TransactionUnitTester::TransactionUnitTester()
    {
        db = edb::DBManager::getInstance();
    }


    TransactionUnitTester::~TransactionUnitTester()
    {
        edb::DBManager::destroyInstance(db);
    }

#define make_bool(a) ((a) ? true:false) 

    bool TransactionUnitTester::runTests()
    {
        int voidFlag = 0;
        bool result = true;
        util_time::DateTime date;
        std::string operatorId, operatorName;

        date -= 18;
        for ( int i = 1; i <= 10; ++i )
        {
            date += 2*18;
            operatorId.clear(); operatorId.append(util_string::inttostring(i));
            operatorName.assign("Operator ").append(util_string::inttostring(i));

            if ( result )
                result = runCreateTransaction1(--date, operatorId, operatorName, make_bool(++voidFlag % 2));

            if ( result )
                result = runCreateTransaction2(--date, operatorId, operatorName, make_bool(++voidFlag % 2));

            if ( result )
                result = runCreateTransaction3(--date, operatorId, operatorName, make_bool(++voidFlag % 2));

            if ( result )
                result = runCreateTransaction4(--date, operatorId, operatorName, make_bool(++voidFlag % 2));

            if ( result )
                result = runCreateTransaction5(--date, operatorId, operatorName, make_bool(++voidFlag % 2));

            if ( result )
                result = runCreateTransaction6(--date, operatorId, operatorName, make_bool(++voidFlag % 2));

            if ( result )
                result = runCreateTransaction7(--date, operatorId, operatorName, make_bool(++voidFlag % 2));

            if ( result )
                result = runCreateTransaction8(--date, operatorId, operatorName, make_bool(++voidFlag % 2));

            if ( result )
                result = runCreateTransaction9(--date, operatorId, operatorName, make_bool(++voidFlag % 2));

            if ( result )
                result = runCreateTransaction10(--date, operatorId, operatorName, make_bool(++voidFlag % 2));

            if ( result )
                result = runCreateTransaction11(--date, operatorId, operatorName, make_bool(++voidFlag % 2));

            if ( result )
                result = runCreateTransaction12(--date, operatorId, operatorName, make_bool(++voidFlag % 2));

            if ( result )
                result = runCreateTransaction14(--date, operatorId, operatorName, make_bool(++voidFlag % 2));

            if ( result )
                result = runCreateTransaction16(--date, operatorId, operatorName, make_bool(++voidFlag % 2));

            if ( result )
                result = runCreateTransaction17(--date, operatorId, operatorName, make_bool(++voidFlag % 2));

            if ( result )
                result = runCreateTransaction18(--date, operatorId, operatorName, make_bool(++voidFlag % 2));

            if ( result )
                result = runCreateTransaction19(--date, operatorId, operatorName, make_bool(++voidFlag % 2));

            if ( result )
                result = runCreateTransaction20(--date, operatorId, operatorName, make_bool(++voidFlag % 2));
        }

        if ( result )
        {
            runNavigateTransactions();
            voidableTransactions("Operator 1");
        }

        return result;
    }


    bool TransactionUnitTester::runCreateTransaction1(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag)
    {
        TransactionManager* transManager = getTransactionManager();
        Transaction* trans = new Transaction(1);

        trans->setVoid((Transaction::VoidFlag)voidFlag);
        trans->setAffectsProductionFlag(TransactionType::AffectsProductionPositively);
        trans->setNumberPackages(1);
        trans->setTotalValue(600);
        trans->setOldTotalValue(1200);

        trans->setProductNumber(11);
        trans->setReportingClass(999999);
        trans->setProductType(Product::PRODUCTTYPE_RANDOMWEIGHT);
        trans->setProductionPlanningType(Product::PRODUCTIONPLANNING_INGREDIENT);
        trans->setProductDescription("My Product iNGREDIENT");
        trans->setPriceModifier(11);
        trans->setBarcodePrefix(2);
        trans->setBarcodeNumber("111111");
        trans->setUnitPrice(300);
        trans->setExceptionPrice(2);
        trans->setFinalPrice(600);
        trans->setTare(10);
        trans->setProportionalTare(0);
        trans->setByCount(0);
        trans->setFinalNetWeight(200);
        trans->setShelfLifeDays(2);
        trans->setShelfLifeHours(0);
        trans->setProductLifeDays(4);
        trans->setProductLifeHours(0);
        trans->setLabel1(113);
        trans->setLabel2(249);
        trans->setLabel3(0);
        trans->setLabel4(0);
        trans->setGraphicName1("mygraphic1.bump");
        trans->setGraphicName2("YOURGRAPHIC2.BUMP");
        trans->setGraphicName3("Graphic Name 3");
        trans->setGraphicName4("Graphic Name 4");
        trans->setCOOLText("COOL Text");
        trans->setCOOLTrackingText("COOL Tracking Test");
        trans->setOperatorId(operatorId);
        trans->setOperatorName(operatorName);
        trans->setOldUnitPrice(400);
        trans->setOldFinalPrice(1200);

        trans->setCommittedDateTime(date);

        transManager->handleSaveTransaction(trans);

        return true;
    }


    bool TransactionUnitTester::runCreateTransaction2(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag)
    {
        TransactionManager* transManager = getTransactionManager();
        Transaction* trans = new Transaction(2);

        trans->setVoid((Transaction::VoidFlag)voidFlag);
        trans->setAffectsProductionFlag(TransactionType::AffectsProductionPositively);
        trans->setNumberPackages(1);
        trans->setTotalValue(600);
        trans->setOldTotalValue(900);

        trans->setProductNumber(12);
        trans->setReportingClass(999999);
        trans->setProductType(Product::PRODUCTTYPE_RANDOMWEIGHT);
        trans->setProductionPlanningType(Product::PRODUCTIONPLANNING_INGREDIENT);
        trans->setProductDescription("My Product iNGREDIENT");
        trans->setPriceModifier(11);
        trans->setBarcodePrefix(2);
        trans->setBarcodeNumber("121212");
        trans->setUnitPrice(300);
        trans->setExceptionPrice(2);
        trans->setFinalPrice(600);
        trans->setTare(1010);
        trans->setProportionalTare(0);
        trans->setByCount(0);
        trans->setFinalNetWeight(2000);
        trans->setShelfLifeDays(2);
        trans->setShelfLifeHours(0);
        trans->setProductLifeDays(4);
        trans->setProductLifeHours(0);
        trans->setLabel1(113);
        trans->setLabel2(249);
        trans->setLabel3(0);
        trans->setLabel4(0);
        trans->setGraphicName1("mygraphic1.bump");
        trans->setGraphicName2("YOURGRAPHIC2.BUMP");
        trans->setGraphicName3("Graphic Name 3");
        trans->setGraphicName4("Graphic Name 4");
        trans->setCOOLText("COOL Text");
        trans->setCOOLTrackingText("COOL Tracking Test");
        trans->setOperatorId(operatorId);
        trans->setOperatorName(operatorName);
        trans->setOldTare(10);
        trans->setOldFinalPrice(900);

        trans->setCommittedDateTime(date);

        transManager->saveTransaction(trans);

        return true;
    }


    bool TransactionUnitTester::runCreateTransaction3(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag)
    {
        TransactionManager* transManager = getTransactionManager();
        Transaction* trans = new Transaction(3);

        trans->setVoid((Transaction::VoidFlag)voidFlag);
        trans->setAffectsProductionFlag(TransactionType::AffectsProductionPositively);
        trans->setNumberPackages(1);
        trans->setTotalValue(150);
        trans->setOldTotalValue(150);

        trans->setProductNumber(13);
        trans->setReportingClass(999999);
        trans->setProductType(Product::PRODUCTTYPE_BYCOUNT);
        trans->setProductionPlanningType(Product::PRODUCTIONPLANNING_INGREDIENT);
        trans->setProductDescription("My Product iNGREDIENT");
        trans->setPriceModifier(11);
        trans->setBarcodePrefix(4);
        trans->setBarcodeNumber("131313");
        trans->setUnitPrice(300);
        trans->setExceptionPrice(150);
        trans->setFinalPrice(150);
        trans->setTare(0);
        trans->setProportionalTare(0);
        trans->setByCount(1);
        trans->setFinalNetWeight(2000);
        trans->setShelfLifeDays(2);
        trans->setShelfLifeHours(0);
        trans->setProductLifeDays(4);
        trans->setProductLifeHours(0);
        trans->setLabel1(113);
        trans->setLabel2(249);
        trans->setLabel3(0);
        trans->setLabel4(0);
        trans->setGraphicName1("mygraphic1.bump");
        trans->setGraphicName2("YOURGRAPHIC2.BUMP");
        trans->setGraphicName3("Graphic Name 3");
        trans->setGraphicName4("Graphic Name 4");
        trans->setCOOLText("COOL Text");
        trans->setCOOLTrackingText("COOL Tracking Test");
        trans->setOperatorId(operatorId);
        trans->setOperatorName(operatorName);
        trans->setOldByCount(3);

        trans->setCommittedDateTime(date);

        transManager->saveTransaction(trans);

        return true;
    }


    bool TransactionUnitTester::runCreateTransaction4(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag)
    {
        TransactionManager* transManager = getTransactionManager();
        Transaction* trans = new Transaction(3);

        trans->setVoid((Transaction::VoidFlag)voidFlag);
        trans->setAffectsProductionFlag(TransactionType::AffectsProductionPositively);
        trans->setNumberPackages(1);
        trans->setTotalValue(150);
        trans->setOldTotalValue(150);

        trans->setProductNumber(13);
        trans->setReportingClass(999999);
        trans->setProductType(Product::PRODUCTTYPE_BYCOUNT);
        trans->setProductionPlanningType(Product::PRODUCTIONPLANNING_INGREDIENT);
        trans->setProductDescription("My Product iNGREDIENT");
        trans->setPriceModifier(11);
        trans->setBarcodePrefix(4);
        trans->setBarcodeNumber("131313");
        trans->setUnitPrice(300);
        trans->setExceptionPrice(150);
        trans->setFinalPrice(150);
        trans->setTare(0);
        trans->setProportionalTare(0);
        trans->setByCount(1);
        trans->setFinalNetWeight(2000);
        trans->setShelfLifeDays(2);
        trans->setShelfLifeHours(0);
        trans->setProductLifeDays(4);
        trans->setProductLifeHours(0);
        trans->setLabel1(113);
        trans->setLabel2(249);
        trans->setLabel3(0);
        trans->setLabel4(0);
        trans->setGraphicName1("mygraphic1.bump");
        trans->setGraphicName2("YOURGRAPHIC2.BUMP");
        trans->setGraphicName3("Graphic Name 3");
        trans->setGraphicName4("Graphic Name 4");
        trans->setCOOLText("COOL Text");
        trans->setCOOLTrackingText("COOL Tracking Test");
        trans->setOperatorId(operatorId);
        trans->setOperatorName(operatorName);
        trans->setOldShelfLifeDays(3);

        trans->setCommittedDateTime(date);

        transManager->saveTransaction(trans);

        return true;
    }


    bool TransactionUnitTester::runCreateTransaction5(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag)
    {
        TransactionManager* transManager = getTransactionManager();
        Transaction* trans = new Transaction(3);

        trans->setVoid((Transaction::VoidFlag)voidFlag);
        trans->setAffectsProductionFlag(TransactionType::AffectsProductionPositively);
        trans->setNumberPackages(1);
        trans->setTotalValue(150);
        trans->setOldTotalValue(150);

        trans->setProductNumber(13);
        trans->setReportingClass(999999);
        trans->setProductType(Product::PRODUCTTYPE_BYCOUNT);
        trans->setProductionPlanningType(Product::PRODUCTIONPLANNING_INGREDIENT);
        trans->setProductDescription("My Product iNGREDIENT");
        trans->setPriceModifier(11);
        trans->setBarcodePrefix(4);
        trans->setBarcodeNumber("131313");
        trans->setUnitPrice(300);
        trans->setExceptionPrice(150);
        trans->setFinalPrice(150);
        trans->setTare(0);
        trans->setProportionalTare(0);
        trans->setByCount(1);
        trans->setFinalNetWeight(2000);
        trans->setShelfLifeDays(2);
        trans->setShelfLifeHours(0);
        trans->setProductLifeDays(4);
        trans->setProductLifeHours(0);
        trans->setLabel1(113);
        trans->setLabel2(249);
        trans->setLabel3(0);
        trans->setLabel4(0);
        trans->setGraphicName1("mygraphic1.bump");
        trans->setGraphicName2("YOURGRAPHIC2.BUMP");
        trans->setGraphicName3("Graphic Name 3");
        trans->setGraphicName4("Graphic Name 4");
        trans->setCOOLText("COOL Text");
        trans->setCOOLTrackingText("COOL Tracking Test");
        trans->setOperatorId(operatorId);
        trans->setOperatorName(operatorName);
        trans->setOldProductLifeDays(3);

        trans->setCommittedDateTime(date);

        transManager->saveTransaction(trans);

        return true;
    }


    bool TransactionUnitTester::runCreateTransaction6(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag)
    {
        TransactionManager* transManager = getTransactionManager();
        Transaction* trans = new Transaction(3);

        trans->setVoid((Transaction::VoidFlag)voidFlag);
        trans->setAffectsProductionFlag(TransactionType::AffectsProductionPositively);
        trans->setNumberPackages(1);
        trans->setTotalValue(150);
        trans->setOldTotalValue(150);

        trans->setProductNumber(13);
        trans->setReportingClass(999999);
        trans->setProductType(Product::PRODUCTTYPE_BYCOUNT);
        trans->setProductionPlanningType(Product::PRODUCTIONPLANNING_INGREDIENT);
        trans->setProductDescription("My Product iNGREDIENT");
        trans->setPriceModifier(11);
        trans->setBarcodePrefix(4);
        trans->setBarcodeNumber("131313");
        trans->setUnitPrice(300);
        trans->setExceptionPrice(150);
        trans->setFinalPrice(150);
        trans->setTare(0);
        trans->setProportionalTare(0);
        trans->setByCount(1);
        trans->setFinalNetWeight(2000);
        trans->setShelfLifeDays(2);
        trans->setShelfLifeHours(0);
        trans->setProductLifeDays(4);
        trans->setProductLifeHours(0);
        trans->setLabel1(113);
        trans->setLabel2(249);
        trans->setLabel3(0);
        trans->setLabel4(0);
        trans->setGraphicName1("mygraphic1.bump");
        trans->setGraphicName2("YOURGRAPHIC2.BUMP");
        trans->setGraphicName3("Graphic Name 3");
        trans->setGraphicName4("Graphic Name 4");
        trans->setCOOLText("COOL Text");
        trans->setCOOLTrackingText("COOL Tracking Test");
        trans->setOperatorId(operatorId);
        trans->setOperatorName(operatorName);
        trans->setOldLabel1(23);

        trans->setCommittedDateTime(date);

        transManager->saveTransaction(trans);

        return true;
    }


    bool TransactionUnitTester::runCreateTransaction7(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag)
    {
        TransactionManager* transManager = getTransactionManager();
        Transaction* trans = new Transaction(3);

        trans->setVoid((Transaction::VoidFlag)voidFlag);
        trans->setAffectsProductionFlag(TransactionType::AffectsProductionPositively);
        trans->setNumberPackages(1);
        trans->setTotalValue(150);
        trans->setOldTotalValue(150);

        trans->setProductNumber(13);
        trans->setReportingClass(999999);
        trans->setProductType(Product::PRODUCTTYPE_BYCOUNT);
        trans->setProductionPlanningType(Product::PRODUCTIONPLANNING_INGREDIENT);
        trans->setProductDescription("My Product iNGREDIENT");
        trans->setPriceModifier(11);
        trans->setBarcodePrefix(4);
        trans->setBarcodeNumber("131313");
        trans->setUnitPrice(300);
        trans->setExceptionPrice(150);
        trans->setFinalPrice(150);
        trans->setTare(0);
        trans->setProportionalTare(0);
        trans->setByCount(1);
        trans->setFinalNetWeight(2000);
        trans->setShelfLifeDays(2);
        trans->setShelfLifeHours(0);
        trans->setProductLifeDays(4);
        trans->setProductLifeHours(0);
        trans->setLabel1(113);
        trans->setLabel2(249);
        trans->setLabel3(0);
        trans->setLabel4(0);
        trans->setGraphicName1("mygraphic1.bump");
        trans->setGraphicName2("YOURGRAPHIC2.BUMP");
        trans->setGraphicName3("Graphic Name 3");
        trans->setGraphicName4("Graphic Name 4");
        trans->setCOOLText("COOL Text");
        trans->setCOOLTrackingText("COOL Tracking Test");
        trans->setOperatorId(operatorId);
        trans->setOperatorName(operatorName);
        trans->setOldLabel2(239);

        trans->setCommittedDateTime(date);

        transManager->saveTransaction(trans);

        return true;
    }


    bool TransactionUnitTester::runCreateTransaction8(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag)
    {
        TransactionManager* transManager = getTransactionManager();
        Transaction* trans = new Transaction(3);

        trans->setVoid((Transaction::VoidFlag)voidFlag);
        trans->setAffectsProductionFlag(TransactionType::AffectsProductionPositively);
        trans->setNumberPackages(1);
        trans->setTotalValue(150);
        trans->setOldTotalValue(150);

        trans->setProductNumber(13);
        trans->setReportingClass(999999);
        trans->setProductType(Product::PRODUCTTYPE_FIXEDWEIGHT);
        trans->setProductionPlanningType(Product::PRODUCTIONPLANNING_INGREDIENT);
        trans->setProductDescription("My Product iNGREDIENT");
        trans->setPriceModifier(11);
        trans->setBarcodePrefix(1);
        trans->setBarcodeNumber("131313");
        trans->setUnitPrice(300);
        trans->setExceptionPrice(0);
        trans->setFinalPrice(300);
        trans->setTare(0);
        trans->setProportionalTare(0);
        trans->setByCount(1);
        trans->setFinalNetWeight(2000);
        trans->setShelfLifeDays(2);
        trans->setShelfLifeHours(0);
        trans->setProductLifeDays(4);
        trans->setProductLifeHours(0);
        trans->setLabel1(113);
        trans->setLabel2(249);
        trans->setLabel3(0);
        trans->setLabel4(0);
        trans->setGraphicName1("mygraphic1.bump");
        trans->setGraphicName2("YOURGRAPHIC2.BUMP");
        trans->setGraphicName3("Graphic Name 3");
        trans->setGraphicName4("Graphic Name 4");
        trans->setCOOLText("COOL Text");
        trans->setCOOLTrackingText("COOL Tracking Test");
        trans->setOperatorId(operatorId);
        trans->setOperatorName(operatorName);
        trans->setOldFixedWeight(1500);

        trans->setCommittedDateTime(date);

        transManager->saveTransaction(trans);

        return true;
    }


    bool TransactionUnitTester::runCreateTransaction9(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag)
    {
        TransactionManager* transManager = getTransactionManager();
        Transaction* trans = new Transaction(3);

        trans->setVoid((Transaction::VoidFlag)voidFlag);
        trans->setAffectsProductionFlag(TransactionType::AffectsProductionPositively);
        trans->setNumberPackages(1);
        trans->setTotalValue(150);
        trans->setOldTotalValue(150);

        trans->setProductNumber(13);
        trans->setReportingClass(999999);
        trans->setProductType(Product::PRODUCTTYPE_BYCOUNT);
        trans->setProductionPlanningType(Product::PRODUCTIONPLANNING_INGREDIENT);
        trans->setProductDescription("My Product iNGREDIENT");
        trans->setPriceModifier(11);
        trans->setBarcodePrefix(4);
        trans->setBarcodeNumber("131313");
        trans->setUnitPrice(300);
        trans->setExceptionPrice(150);
        trans->setFinalPrice(150);
        trans->setTare(0);
        trans->setProportionalTare(0);
        trans->setByCount(1);
        trans->setFinalNetWeight(2000);
        trans->setShelfLifeDays(2);
        trans->setShelfLifeHours(0);
        trans->setProductLifeDays(4);
        trans->setProductLifeHours(0);
        trans->setLabel1(113);
        trans->setLabel2(249);
        trans->setLabel3(0);
        trans->setLabel4(0);
        trans->setGraphicName1("mygraphic1.bump");
        trans->setGraphicName2("YOURGRAPHIC2.BUMP");
        trans->setGraphicName3("Graphic Name 3");
        trans->setGraphicName4("Graphic Name 4");
        trans->setCOOLText("COOL Text");
        trans->setCOOLTrackingText("COOL Tracking Test");
        trans->setOperatorId(operatorId);
        trans->setOperatorName(operatorName);
        trans->setOldBarcodeDigit(3);

        trans->setCommittedDateTime(date);

        transManager->saveTransaction(trans);

        return true;
    }


    bool TransactionUnitTester::runCreateTransaction10(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag)
    {
        TransactionManager* transManager = getTransactionManager();
        Transaction* trans = new Transaction(3);

        trans->setVoid((Transaction::VoidFlag)voidFlag);
        trans->setAffectsProductionFlag(TransactionType::AffectsProductionPositively);
        trans->setNumberPackages(1);
        trans->setTotalValue(150);
        trans->setOldTotalValue(150);

        trans->setProductNumber(13);
        trans->setReportingClass(999999);
        trans->setProductType(Product::PRODUCTTYPE_BYCOUNT);
        trans->setProductionPlanningType(Product::PRODUCTIONPLANNING_INGREDIENT);
        trans->setProductDescription("My Product iNGREDIENT");
        trans->setPriceModifier(11);
        trans->setBarcodePrefix(4);
        trans->setBarcodeNumber("131313");
        trans->setUnitPrice(300);
        trans->setExceptionPrice(150);
        trans->setFinalPrice(100);
        trans->setTare(0);
        trans->setProportionalTare(0);
        trans->setByCount(1);
        trans->setFinalNetWeight(2000);
        trans->setShelfLifeDays(2);
        trans->setShelfLifeHours(0);
        trans->setProductLifeDays(4);
        trans->setProductLifeHours(0);
        trans->setLabel1(113);
        trans->setLabel2(249);
        trans->setLabel3(0);
        trans->setLabel4(0);
        trans->setGraphicName1("mygraphic1.bump");
        trans->setGraphicName2("YOURGRAPHIC2.BUMP");
        trans->setGraphicName3("Graphic Name 3");
        trans->setGraphicName4("Graphic Name 4");
        trans->setCOOLText("COOL Text");
        trans->setCOOLTrackingText("COOL Tracking Test");
        trans->setOperatorId(operatorId);
        trans->setOperatorName(operatorName);
        trans->setOldFinalPrice(150);

        trans->setCommittedDateTime(date);

        transManager->saveTransaction(trans);

        return true;
    }


    bool TransactionUnitTester::runCreateTransaction11(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag)
    {
        TransactionManager* transManager = getTransactionManager();
        Transaction* trans = new Transaction(1);

        trans->setVoid((Transaction::VoidFlag)voidFlag);
        trans->setAffectsProductionFlag(TransactionType::AffectsProductionPositively);
        trans->setNumberPackages(1);
        trans->setTotalValue(600);
        trans->setOldTotalValue(1200);

        trans->setProductNumber(11);
        trans->setReportingClass(999999);
        trans->setProductType(Product::PRODUCTTYPE_RANDOMWEIGHT);
        trans->setProductionPlanningType(Product::PRODUCTIONPLANNING_INGREDIENT);
        trans->setProductDescription("My Product iNGREDIENT");
        trans->setPriceModifier(11);
        trans->setBarcodePrefix(2);
        trans->setBarcodeNumber("111111");
        trans->setUnitPrice(300);
        trans->setExceptionPrice(2);
        trans->setFinalPrice(600);
        trans->setTare(10);
        trans->setProportionalTare(0);
        trans->setByCount(0);
        trans->setFinalNetWeight(200);
        trans->setShelfLifeDays(2);
        trans->setShelfLifeHours(0);
        trans->setProductLifeDays(4);
        trans->setProductLifeHours(0);
        trans->setLabel1(113);
        trans->setLabel2(249);
        trans->setLabel3(0);
        trans->setLabel4(0);
        trans->setGraphicName1("mygraphic1.bump");
        trans->setGraphicName2("YOURGRAPHIC2.BUMP");
        trans->setGraphicName3("Graphic Name 3");
        trans->setGraphicName4("Graphic Name 4");
        trans->setCOOLText("COOL Text");
        trans->setCOOLTrackingText("COOL Tracking Test");
        trans->setOperatorId(operatorId);
        trans->setOperatorName(operatorName);
        trans->setOldGraphicName1("my old graphic 1.bump");

        trans->setCommittedDateTime(date);

        transManager->saveTransaction(trans);

        return true;
    }


    bool TransactionUnitTester::runCreateTransaction12(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag)
    {
        TransactionManager* transManager = getTransactionManager();
        Transaction* trans = new Transaction(2);

        trans->setVoid((Transaction::VoidFlag)voidFlag);
        trans->setAffectsProductionFlag(TransactionType::AffectsProductionPositively);
        trans->setNumberPackages(1);
        trans->setTotalValue(600);
        trans->setOldTotalValue(900);

        trans->setProductNumber(12);
        trans->setReportingClass(999999);
        trans->setProductType(Product::PRODUCTTYPE_RANDOMWEIGHT);
        trans->setProductionPlanningType(Product::PRODUCTIONPLANNING_INGREDIENT);
        trans->setProductDescription("My Product iNGREDIENT");
        trans->setPriceModifier(11);
        trans->setBarcodePrefix(2);
        trans->setBarcodeNumber("121212");
        trans->setUnitPrice(300);
        trans->setExceptionPrice(2);
        trans->setFinalPrice(600);
        trans->setTare(1010);
        trans->setProportionalTare(0);
        trans->setByCount(0);
        trans->setFinalNetWeight(2000);
        trans->setShelfLifeDays(2);
        trans->setShelfLifeHours(0);
        trans->setProductLifeDays(4);
        trans->setProductLifeHours(0);
        trans->setLabel1(113);
        trans->setLabel2(249);
        trans->setLabel3(0);
        trans->setLabel4(0);
        trans->setGraphicName1("mygraphic1.bump");
        trans->setGraphicName2("YOURGRAPHIC2.BUMP");
        trans->setGraphicName3("Graphic Name 3");
        trans->setGraphicName4("Graphic Name 4");
        trans->setCOOLText("COOL Text");
        trans->setCOOLTrackingText("COOL Tracking Test");
        trans->setOperatorId(operatorId);
        trans->setOperatorName(operatorName);
        trans->setOldShelfLifeHours(5);

        trans->setCommittedDateTime(date);

        transManager->saveTransaction(trans);

        return true;
    }


    bool TransactionUnitTester::runCreateTransaction14(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag)
    {
        TransactionManager* transManager = getTransactionManager();
        Transaction* trans = new Transaction(3);

        trans->setVoid((Transaction::VoidFlag)voidFlag);
        trans->setAffectsProductionFlag(TransactionType::AffectsProductionPositively);
        trans->setNumberPackages(1);
        trans->setTotalValue(150);
        trans->setOldTotalValue(150);

        trans->setProductNumber(13);
        trans->setReportingClass(999999);
        trans->setProductType(Product::PRODUCTTYPE_BYCOUNT);
        trans->setProductionPlanningType(Product::PRODUCTIONPLANNING_INGREDIENT);
        trans->setProductDescription("My Product iNGREDIENT");
        trans->setPriceModifier(11);
        trans->setBarcodePrefix(4);
        trans->setBarcodeNumber("131313");
        trans->setUnitPrice(300);
        trans->setExceptionPrice(150);
        trans->setFinalPrice(150);
        trans->setTare(0);
        trans->setProportionalTare(0);
        trans->setByCount(1);
        trans->setFinalNetWeight(2000);
        trans->setShelfLifeDays(2);
        trans->setShelfLifeHours(0);
        trans->setProductLifeDays(4);
        trans->setProductLifeHours(0);
        trans->setLabel1(113);
        trans->setLabel2(249);
        trans->setLabel3(0);
        trans->setLabel4(0);
        trans->setGraphicName1("mygraphic1.bump");
        trans->setGraphicName2("YOURGRAPHIC2.BUMP");
        trans->setGraphicName3("Graphic Name 3");
        trans->setGraphicName4("Graphic Name 4");
        trans->setCOOLText("COOL Text");
        trans->setCOOLTrackingText("COOL Tracking Test");
        trans->setOperatorId(operatorId);
        trans->setOperatorName(operatorName);
        trans->setOldProductLifeHours(8);

        trans->setCommittedDateTime(date);

        transManager->saveTransaction(trans);

        return true;
    }


    bool TransactionUnitTester::runCreateTransaction16(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag)
    {
        TransactionManager* transManager = getTransactionManager();
        Transaction* trans = new Transaction(3);

        trans->setVoid((Transaction::VoidFlag)voidFlag);
        trans->setAffectsProductionFlag(TransactionType::AffectsProductionPositively);
        trans->setNumberPackages(1);
        trans->setTotalValue(150);
        trans->setOldTotalValue(150);

        trans->setProductNumber(13);
        trans->setReportingClass(999999);
        trans->setProductType(Product::PRODUCTTYPE_BYCOUNT);
        trans->setProductionPlanningType(Product::PRODUCTIONPLANNING_INGREDIENT);
        trans->setProductDescription("My Product iNGREDIENT");
        trans->setPriceModifier(11);
        trans->setBarcodePrefix(4);
        trans->setBarcodeNumber("131313");
        trans->setUnitPrice(300);
        trans->setExceptionPrice(150);
        trans->setFinalPrice(150);
        trans->setTare(0);
        trans->setProportionalTare(0);
        trans->setByCount(1);
        trans->setFinalNetWeight(2000);
        trans->setShelfLifeDays(2);
        trans->setShelfLifeHours(0);
        trans->setProductLifeDays(4);
        trans->setProductLifeHours(0);
        trans->setLabel1(113);
        trans->setLabel2(249);
        trans->setLabel3(0);
        trans->setLabel4(0);
        trans->setGraphicName1("mygraphic1.bump");
        trans->setGraphicName2("YOURGRAPHIC2.BUMP");
        trans->setGraphicName3("Graphic Name 3");
        trans->setGraphicName4("Graphic Name 4");
        trans->setCOOLText("COOL Text");
        trans->setCOOLTrackingText("COOL Tracking Test");
        trans->setOperatorId(operatorId);
        trans->setOperatorName(operatorName);
        trans->setOldLabel3(249);

        trans->setCommittedDateTime(date);

        transManager->saveTransaction(trans);

        return true;
    }


    bool TransactionUnitTester::runCreateTransaction17(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag)
    {
        TransactionManager* transManager = getTransactionManager();
        Transaction* trans = new Transaction(3);

        trans->setVoid((Transaction::VoidFlag)voidFlag);
        trans->setAffectsProductionFlag(TransactionType::AffectsProductionPositively);
        trans->setNumberPackages(1);
        trans->setTotalValue(150);
        trans->setOldTotalValue(150);

        trans->setProductNumber(13);
        trans->setReportingClass(999999);
        trans->setProductType(Product::PRODUCTTYPE_BYCOUNT);
        trans->setProductionPlanningType(Product::PRODUCTIONPLANNING_INGREDIENT);
        trans->setProductDescription("My Product iNGREDIENT");
        trans->setPriceModifier(11);
        trans->setBarcodePrefix(4);
        trans->setBarcodeNumber("131313");
        trans->setUnitPrice(300);
        trans->setExceptionPrice(150);
        trans->setFinalPrice(150);
        trans->setTare(0);
        trans->setProportionalTare(0);
        trans->setByCount(1);
        trans->setFinalNetWeight(2000);
        trans->setShelfLifeDays(2);
        trans->setShelfLifeHours(0);
        trans->setProductLifeDays(4);
        trans->setProductLifeHours(0);
        trans->setLabel1(113);
        trans->setLabel2(249);
        trans->setLabel3(0);
        trans->setLabel4(0);
        trans->setGraphicName1("mygraphic1.bump");
        trans->setGraphicName2("YOURGRAPHIC2.BUMP");
        trans->setGraphicName3("Graphic Name 3");
        trans->setGraphicName4("Graphic Name 4");
        trans->setCOOLText("COOL Text");
        trans->setCOOLTrackingText("COOL Tracking Test");
        trans->setOperatorId(operatorId);
        trans->setOperatorName(operatorName);
        trans->setOldLabel4(239);

        trans->setCommittedDateTime(date);

        transManager->saveTransaction(trans);

        return true;
    }


    bool TransactionUnitTester::runCreateTransaction18(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag)
    {
        TransactionManager* transManager = getTransactionManager();
        Transaction* trans = new Transaction(3);

        trans->setVoid((Transaction::VoidFlag)voidFlag);
        trans->setAffectsProductionFlag(TransactionType::AffectsProductionPositively);
        trans->setNumberPackages(1);
        trans->setTotalValue(150);
        trans->setOldTotalValue(150);

        trans->setProductNumber(13);
        trans->setReportingClass(999999);
        trans->setProductType(Product::PRODUCTTYPE_FIXEDWEIGHT);
        trans->setProductionPlanningType(Product::PRODUCTIONPLANNING_INGREDIENT);
        trans->setProductDescription("My Product iNGREDIENT");
        trans->setPriceModifier(11);
        trans->setBarcodePrefix(1);
        trans->setBarcodeNumber("131313");
        trans->setUnitPrice(300);
        trans->setExceptionPrice(0);
        trans->setFinalPrice(300);
        trans->setTare(0);
        trans->setProportionalTare(0);
        trans->setByCount(1);
        trans->setFinalNetWeight(2000);
        trans->setShelfLifeDays(2);
        trans->setShelfLifeHours(0);
        trans->setProductLifeDays(4);
        trans->setProductLifeHours(0);
        trans->setLabel1(113);
        trans->setLabel2(249);
        trans->setLabel3(0);
        trans->setLabel4(0);
        trans->setGraphicName1("mygraphic1.bump");
        trans->setGraphicName2("YOURGRAPHIC2.BUMP");
        trans->setGraphicName3("Graphic Name 3");
        trans->setGraphicName4("Graphic Name 4");
        trans->setCOOLText("COOL Text");
        trans->setCOOLTrackingText("COOL Tracking Test");
        trans->setOperatorId(operatorId);
        trans->setOperatorName(operatorName);
        trans->setOldGraphicName2("old grph 2.bump");

        trans->setCommittedDateTime(date);

        transManager->saveTransaction(trans);

        return true;
    }


    bool TransactionUnitTester::runCreateTransaction19(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag)
    {
        TransactionManager* transManager = getTransactionManager();
        Transaction* trans = new Transaction(3);

        trans->setVoid((Transaction::VoidFlag)voidFlag);
        trans->setAffectsProductionFlag(TransactionType::AffectsProductionPositively);
        trans->setNumberPackages(1);
        trans->setTotalValue(150);
        trans->setOldTotalValue(150);

        trans->setProductNumber(13);
        trans->setReportingClass(999999);
        trans->setProductType(Product::PRODUCTTYPE_BYCOUNT);
        trans->setProductionPlanningType(Product::PRODUCTIONPLANNING_INGREDIENT);
        trans->setProductDescription("My Product iNGREDIENT");
        trans->setPriceModifier(11);
        trans->setBarcodePrefix(4);
        trans->setBarcodeNumber("131313");
        trans->setUnitPrice(300);
        trans->setExceptionPrice(150);
        trans->setFinalPrice(150);
        trans->setTare(0);
        trans->setProportionalTare(0);
        trans->setByCount(1);
        trans->setFinalNetWeight(2000);
        trans->setShelfLifeDays(2);
        trans->setShelfLifeHours(0);
        trans->setProductLifeDays(4);
        trans->setProductLifeHours(0);
        trans->setLabel1(113);
        trans->setLabel2(249);
        trans->setLabel3(0);
        trans->setLabel4(0);
        trans->setGraphicName1("mygraphic1.bump");
        trans->setGraphicName2("YOURGRAPHIC2.BUMP");
        trans->setGraphicName3("Graphic Name 3");
        trans->setGraphicName4("Graphic Name 4");
        trans->setCOOLText("COOL Text");
        trans->setCOOLTrackingText("COOL Tracking Test");
        trans->setOperatorId(operatorId);
        trans->setOperatorName(operatorName);
        trans->setOldGraphicName3("old grph 3.bump");

        trans->setCommittedDateTime(date);

        transManager->saveTransaction(trans);

        return true;
    }


    bool TransactionUnitTester::runCreateTransaction20(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag)
    {
        TransactionManager* transManager = getTransactionManager();
        Transaction* trans = new Transaction(3);

        trans->setVoid((Transaction::VoidFlag)voidFlag);
        trans->setAffectsProductionFlag(TransactionType::AffectsProductionPositively);
        trans->setNumberPackages(1);
        trans->setTotalValue(150);
        trans->setOldTotalValue(150);

        trans->setProductNumber(13);
        trans->setReportingClass(999999);
        trans->setProductType(Product::PRODUCTTYPE_BYCOUNT);
        trans->setProductionPlanningType(Product::PRODUCTIONPLANNING_INGREDIENT);
        trans->setProductDescription("My Product iNGREDIENT");
        trans->setPriceModifier(11);
        trans->setBarcodePrefix(4);
        trans->setBarcodeNumber("131313");
        trans->setUnitPrice(300);
        trans->setExceptionPrice(150);
        trans->setFinalPrice(100);
        trans->setTare(0);
        trans->setProportionalTare(0);
        trans->setByCount(1);
        trans->setFinalNetWeight(2000);
        trans->setShelfLifeDays(2);
        trans->setShelfLifeHours(0);
        trans->setProductLifeDays(4);
        trans->setProductLifeHours(0);
        trans->setLabel1(113);
        trans->setLabel2(249);
        trans->setLabel3(0);
        trans->setLabel4(0);
        trans->setGraphicName1("mygraphic1.bump");
        trans->setGraphicName2("YOURGRAPHIC2.BUMP");
        trans->setGraphicName3("Graphic Name 3");
        trans->setGraphicName4("Graphic Name 4");
        trans->setCOOLText("COOL Text");
        trans->setCOOLTrackingText("COOL Tracking Test");
        trans->setOperatorId(operatorId);
        trans->setOperatorName(operatorName);
        trans->setOldGraphicName4("old grph 4.bump");

        trans->setCommittedDateTime(date);

        transManager->saveTransaction(trans);

        return true;
    }


    void TransactionUnitTester::runNavigateTransactions()
    {
        TransactionManager* transManager = getTransactionManager();

        if ( transManager->initializeAllTransactionsQuery() )
        {
            Transaction* trans = transManager->getFirstTransaction();

            while ( trans )
            {
                trans->show();

                trans = transManager->getNextTransaction();
            }
        }
    }


    void TransactionUnitTester::voidableTransactions(const std::string& UNUSED(operatorName))
    {
        UNREFERENCED_PARAMETER( operatorName );
        /*              TransactionManager* transManager = getTransactionManager();

                        if ( transManager->initializeOperatorVoidableQuery(operatorName) )
                        {
                        Transaction* trans = transManager->getFirstOperatorVoidableTransaction(operatorName);

                        while ( trans )
                        {
                        trans->show();

                        trans = transManager->getNextOperatorVoidableTransaction(operatorName);
                        }
                        } */
    }



}
