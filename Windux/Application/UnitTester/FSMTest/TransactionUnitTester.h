#ifndef TRANSACTIONUNITTESTER_H
#define TRANSACTIONUNITTESTER_H



#include "HFCManagerContainer.h"

#include "util/time/datetime.h"



namespace edb
{
    class DBManager;
}


namespace hfc
{


    class TransactionUnitTester : public HFCManagerContainer
    {
        public:
            /* --- Constructors and destructor --- */
            TransactionUnitTester();
            virtual ~TransactionUnitTester();


            bool runTests();


        private:
            edb::DBManager *db;


            bool runCreateTransaction1(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            bool runCreateTransaction2(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            bool runCreateTransaction3(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            bool runCreateTransaction4(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            bool runCreateTransaction5(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            bool runCreateTransaction6(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            bool runCreateTransaction7(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            bool runCreateTransaction8(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            bool runCreateTransaction9(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            bool runCreateTransaction10(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            bool runCreateTransaction11(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            bool runCreateTransaction12(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            bool runCreateTransaction13(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            bool runCreateTransaction14(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            bool runCreateTransaction15(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            bool runCreateTransaction16(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            bool runCreateTransaction17(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            bool runCreateTransaction18(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            bool runCreateTransaction19(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            bool runCreateTransaction20(const util_time::DateTime& date, const std::string& operatorId, const std::string& operatorName, bool voidFlag);
            void runNavigateTransactions();
            void voidableTransactions(const std::string& operatorName);
    };


}



#endif /* TRANSACTIONUNITTESTER_H */
