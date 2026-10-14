#ifndef SQLITECUTTINGTEST_H
#define SQLITECUTTINGTEST_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBCUTTINGTESTI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteCuttingTest : public DBCuttingTestI
	{
		public:

			// Types and subtypes
			typedef DBCuttingTestI interface_type;

			
			// Constructors and destructor
			SqliteCuttingTest(SqliteRecord* r, DBCuttingTestI* d = 0);
			~SqliteCuttingTest();


			// Think about replacing this method with two friend functions from the
			// SqliteCuttingTestsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBCuttingTestI
			virtual DBCuttingTestI* clone() const { return new SqliteCuttingTest(new SqliteRecord(*rec_)); }
			virtual const DBCuttingTestI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getTestNumber() const { return rec_->getField(CUTTINGTESTID_FIELD); }
			virtual void setTestNumber(const std::string& num) { rec_->setField(CUTTINGTESTID_FIELD, num); }
			virtual bool isTestNumberNull() const { return rec_->isFieldNull(CUTTINGTESTID_FIELD); }
			virtual bool isTesNumberWritable() const { return true; }

			virtual std::string getOperatorNumber() const { return rec_->getField(OPERATORNUMBER_FIELD); }
			virtual void setOperatorNumber(const std::string& num) { rec_->setField(OPERATORNUMBER_FIELD, num); }
			virtual bool isOperatorNumberNull() const { return rec_->isFieldNull(OPERATORNUMBER_FIELD); }
			virtual void setOperatorNumberNull() { rec_->setFieldNull(OPERATORNUMBER_FIELD); }
			virtual bool isOperatorNumberWritable() const { return true; }

			virtual std::string getProductNumber() const { return rec_->getField(PRODUCTNUMBER_FIELD); }
			virtual void setProductNumber(const std::string& num) { rec_->setField(PRODUCTNUMBER_FIELD, num); }
			virtual bool isProductNumberNull() const { return rec_->isFieldNull(PRODUCTNUMBER_FIELD); }
			virtual void setProductNumberNull() { rec_->setFieldNull(PRODUCTNUMBER_FIELD); }
			virtual bool isProductNumberWritable() const { return true; }

			virtual std::string getVendorNumber() const { return rec_->getField(VENDORNUMBER_FIELD); }
			virtual void setVendorNumber(const std::string& num) { rec_->setField(VENDORNUMBER_FIELD, num); }
			virtual bool isVendorNumberNull() const { return rec_->isFieldNull(VENDORNUMBER_FIELD); }
			virtual void setVendorNumberNull() { rec_->setFieldNull(VENDORNUMBER_FIELD); }
			virtual bool isVendorNumberWritable() const { return true; }

			virtual int getPrimalWeight() const { return rec_->getField(PRIMALWEIGHT_FIELD); }
			virtual void setPrimalWeight(int value) { rec_->setField(PRIMALWEIGHT_FIELD, value); }
			virtual bool isPrimalWeightNull() const { return rec_->isFieldNull(PRIMALWEIGHT_FIELD); }
			virtual void setPrimalWeightNull() { rec_->setFieldNull(PRIMALWEIGHT_FIELD); }
			virtual bool isPrimalWeightWritable() const { return true; }

			virtual int getPrimalCost() const { return rec_->getField(PRIMALCOST_FIELD); }
			virtual void setPrimalCost(int value) { rec_->setField(PRIMALCOST_FIELD, value); }
			virtual bool isPrimalCostNull() const { return rec_->isFieldNull(PRIMALCOST_FIELD); }
			virtual void setPrimalCostNull() { rec_->setFieldNull(PRIMALCOST_FIELD); }
			virtual bool isPrimalCostWritable() const { return true; }

			virtual std::string getStartDate() const { return rec_->getField(STARTDATE_FIELD); }
			virtual void setStartDate(const std::string& date) { rec_->setField(STARTDATE_FIELD, date); }
			virtual bool isStartDateNull() const { return rec_->isFieldNull(STARTDATE_FIELD); }
			virtual void setStartDateNull() { rec_->setFieldNull(STARTDATE_FIELD); }
			virtual bool isStartDateWritable() const { return true; }

			virtual std::string getEndDate() const { return rec_->getField(ENDDATE_FIELD); }
			virtual void setEndDate(const std::string& date) { rec_->setField(ENDDATE_FIELD, date); }
			virtual bool isEndDateNull() const { return rec_->isFieldNull(ENDDATE_FIELD); }
			virtual void setEndDateNull() { rec_->setFieldNull(ENDDATE_FIELD); }
			virtual bool isEndDateWritable() const { return true; }

			virtual std::string getLotNumber() const { return rec_->getField(LOTNUMBER_FIELD); }
			virtual void setLotNumber(const std::string& num) { rec_->setField(LOTNUMBER_FIELD, num); }
			virtual bool isLotNumberNull() const { return rec_->isFieldNull(LOTNUMBER_FIELD); }
			virtual void setLotNumberNull() { rec_->setFieldNull(LOTNUMBER_FIELD); }
			virtual bool isLotNumberWritable() const { return true; }

			virtual std::string getPackingDate() const { return rec_->getField(PACKINGDATE_FIELD); }
			virtual void setPackingDate(const std::string& date) { rec_->setField(PACKINGDATE_FIELD, date); }
			virtual bool isPackingDateNull() const { return rec_->isFieldNull(PACKINGDATE_FIELD); }
			virtual void setPackingDateNull() { rec_->isFieldNull(PACKINGDATE_FIELD); }
			virtual bool isPackingDateWritable() const { return true; }

			virtual std::string getPackingHours() const { return rec_->getField(PACKINGHOURS_FIELD); }
			virtual void setPackingHours(const std::string& hours) { rec_->setField(PACKINGHOURS_FIELD, hours); }
			virtual bool isPackingHoursNull() const { return rec_->isFieldNull(PACKINGHOURS_FIELD); }
			virtual void setPackingHoursNull() { rec_->isFieldNull(PACKINGHOURS_FIELD); }
			virtual bool isPackingHoursWritable() const { return true; }

			virtual std::string getPackingMinutes() const { return rec_->getField(PACKINGMINUTES_FIELD); }
			virtual void setPackingMinutes(const std::string& minutes) { rec_->setField(PACKINGMINUTES_FIELD, minutes); }
			virtual bool isPackingMinutesNull() const { return rec_->isFieldNull(PACKINGMINUTES_FIELD); }
			virtual void setPackingMinutesNull() { rec_->isFieldNull(PACKINGMINUTES_FIELD); }
			virtual bool isPackingMinutesWritable() const { return true; }

			virtual std::string getComments() const { return rec_->getField(COMMENTS_FIELD); }
			virtual void setComments(const std::string& comments) { rec_->setField(COMMENTS_FIELD, comments); }
			virtual bool isCommentsNull() const { return rec_->isFieldNull(COMMENTS_FIELD); }
			virtual void setCommentsNull() { rec_->setFieldNull(COMMENTS_FIELD); }
			virtual bool isCommentsWritable() const { return true; }

			virtual std::string getFinalComments() const { return rec_->getField(FINALCOMMENTS_FIELD); }
			virtual void setFinalComments(const std::string& finalComments) { rec_->setField(FINALCOMMENTS_FIELD, finalComments); }
			virtual bool isFinalCommentsNull() const { return rec_->isFieldNull(FINALCOMMENTS_FIELD); }
			virtual void setFinalCommentsNull() { rec_->setFieldNull(FINALCOMMENTS_FIELD); }
			virtual bool isFinalCommentsWritable() const { return true; }

			virtual int getStatus() const { return rec_->getField(STATUS_FIELD); }
			virtual void setStatus(const int stat) { rec_->setField(STATUS_FIELD, stat); }
			virtual bool isStatusNull() const { return rec_->isFieldNull(STATUS_FIELD); }
			virtual void setStatusNull() { rec_->setFieldNull(STATUS_FIELD); }
			virtual bool isStatusWritable() const { return true; }

			virtual std::string getPercentTotalShrinkage() const { return rec_->getField(PERCENTTOTALSHRINKAGE_FIELD); }
			virtual void setPercentTotalShrinkage(const std::string& percentTotalShrinkage) { rec_->setField(PERCENTTOTALSHRINKAGE_FIELD, percentTotalShrinkage); }
			virtual bool isPercentTotalShrinkageNull() const { return rec_->isFieldNull(PERCENTTOTALSHRINKAGE_FIELD); }
			virtual void setPercentTotalShrinkageNull() { rec_->setFieldNull(PERCENTTOTALSHRINKAGE_FIELD); }
			virtual bool isPercentTotalShrinkageWritable() const { return true; }

			virtual std::string getTotalWeightOfCutProducts() const { return rec_->getField(TOTALWEIGHTOFCUTPRODUCTS_FIELD); }
			virtual void setTotalWeightOfCutProducts(const std::string& totalWeightOfCutProducts) { rec_->setField(TOTALWEIGHTOFCUTPRODUCTS_FIELD, totalWeightOfCutProducts); }
			virtual bool isTotalWeightOfCutProductsNull() const { return rec_->isFieldNull(TOTALWEIGHTOFCUTPRODUCTS_FIELD); }
			virtual void setTotalWeightOfCutProductsNull() { rec_->setFieldNull(TOTALWEIGHTOFCUTPRODUCTS_FIELD); }
			virtual bool isTotalWeightOfCutProductsWritable() const { return true; }

			virtual std::string getTotalPriceOfCutProducts() const { return rec_->getField(TOTALPRICEOFCUTPRODUCTS_FIELD); }
			virtual void setTotalPriceOfCutProducts(const std::string& totalPriceOfCutProducts) { rec_->setField(TOTALPRICEOFCUTPRODUCTS_FIELD, totalPriceOfCutProducts); }
			virtual bool isTotalPriceOfCutProductsNull() const { return rec_->isFieldNull(TOTALPRICEOFCUTPRODUCTS_FIELD); }
			virtual void setTotalPriceOfCutProductsNull() { rec_->setFieldNull(TOTALPRICEOFCUTPRODUCTS_FIELD); }
			virtual bool isTotalPriceOfCutProductsWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string CUTTINGTESTID_FIELD;
			static const std::string OPERATORNUMBER_FIELD;
			static const std::string STARTDATE_FIELD;
			static const std::string ENDDATE_FIELD;
			static const std::string PRODUCTNUMBER_FIELD;
			static const std::string VENDORNUMBER_FIELD;
			static const std::string PRIMALWEIGHT_FIELD;
			static const std::string PRIMALCOST_FIELD;
			static const std::string LOTNUMBER_FIELD;
			static const std::string PACKINGDATE_FIELD;
			static const std::string PACKINGHOURS_FIELD;
			static const std::string PACKINGMINUTES_FIELD;
			static const std::string COMMENTS_FIELD;
			static const std::string FINALCOMMENTS_FIELD;
			static const std::string STATUS_FIELD;
			static const std::string PERCENTTOTALSHRINKAGE_FIELD;
			static const std::string TOTALWEIGHTOFCUTPRODUCTS_FIELD;
			static const std::string TOTALPRICEOFCUTPRODUCTS_FIELD;


		private:

			SqliteRecord* rec_;
			DBCuttingTestI* decorated_;

			// Do not allow the default copy constructor.
			SqliteCuttingTest(const SqliteCuttingTest& rec);
			// Do not allow the default assignment operator.
			SqliteCuttingTest operator=(const SqliteCuttingTest& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECUTTINGTEST_H
