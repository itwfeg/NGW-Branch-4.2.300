#ifndef TOTAL_H
#define TOTAL_H



#include "HfcObject.h"



namespace hfc
{


	class Total : public HfcObject
	{
		public:
			typedef enum
			{
				fieldOffsetTotalWeight = 1,
				fieldOffsetTotalPrice,
				fieldOffsetTotalPackages,
				fieldOffsetTotalRuns,
				fieldOffsetTotalItems,
				fieldOffsetTotalFlags,

				endTotalFieldOffsets
			} TotalFieldOffsets;


			/* --- Constructors and destructor --- */
			Total();
			~Total();


			/* --- Getters and setters --- */
			HobErrorCode getErrorCode() const { return errorCode_; }
			void setErrorCode(HobErrorCode errorCode) { errorCode_ = errorCode; }

			int getFlags() const { return flags_; }
			void setFlags(int flags) { flags_ = flags; flagsSet_ = true; }

			bool isFlagsSet() const { return flagsSet_; }

			HobWeight getWeight() const { return weight_; }
			void incrementWeight(HobWeight weight) { weight_ += weight; }
			void setWeight(HobWeight weight) { weight_ = weight; }

			HobPrice getPrice() const { return price_; }
			void incrementPrice(HobPrice price) { price_ += price; }
			void setPrice(HobPrice price) { price_ = price; }

			int getPackages() const { return packages_; }
			void incrementPackages(int number = 1) { packages_ += number; }
			void setPackages(int packages) { packages_ = packages; }

			int getRuns() const { return runs_; }
			void incrementRuns(int runs = 1) { runs_ += runs; }
			void setRuns(int runs) { runs_ = runs; }

			int getItems() const { return items_; }
			void incrementItems(int items) { items_ += items; }
			void setItems(int items) { items_ = items; }

			int getTransactions() const { return transactions_; }
			void incrementTransactions(int number = 1) { transactions_ += number; }
			void setTransactions(int number) { transactions_ = number; }


			/* --- Instance function members --- */
			void update(const Total *totalRec);
			void voidTotal(const Total *totalRec);

			virtual void updateField(int fieldCode, const HobString &token);

			virtual void getFieldAsString(int fieldCode, HobString &result, int format) const;

			const char *serialize(int format) const;

			void show();


		protected:
			/* --- Instance data members --- */
			HobErrorCode errorCode_;
			HobWeight weight_;
			HobPrice price_;
			int packages_;
			int runs_;
			int items_;
			int transactions_;

			/** from new_db.h:
			 *
			 *  These flags apply to appl_totaldata.totals_flags, but should
			 *  also be reserved for appl_pludata.plu_flags
			 *
			 * #define AUTO_CHANGE_UPC_DIGIT     0x08000
			 * #define MANUAL_CHANGE_UPC_DIGIT   0x04000
			 * #define ADD_TO_PRODUCTION_TOTALS  0x02000
			 * #define BACK_DATE_PACKAGES        0x01000
			 *
			 *  These flags apply to both appl_totaldata.total_flags and
			 *  appl_pludata.plu_flags
			 *
			 * #define PRICE_CHANGE        0x01
			 * #define BAD_RECORD          0x02
			 * #define FIXED_WEIGHT_PLU    0x04
			 * #define CHANGED_RECORD      0x08
			 * #define FLUID_OUNCE_PLU     0x10
			 * #define OP_TOTAL_OK         0x20
			 * #define PRNT_SECURITY_LABEL 0x40 */

			int flags_;
			mutable bool flagsSet_;


		private:
			void init();

#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
	};


}



#endif //TOTAL_H
