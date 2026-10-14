#ifndef OPERATORCHANGESUMMARY_H
#define OPERATORCHANGESUMMARY_H



#include <string>


#include "evolution.h"



namespace hfc
{


	class OperatorChangeSummary
	{
		public:
			/* --- Types and subtypes --- */


			/* --- Constructors and destructor --- */
			OperatorChangeSummary(const std::string& operatorId, const std::string& operatorName);
			~OperatorChangeSummary();


			/* --- Getter and setter methods --- */
			const std::string& getOperatorId() const { return operatorId_; }
			void setOperatorId(const std::string& id) { operatorId_.assign(id); }

			const std::string& getOperatorName() const { return operatorName_; }
			void setOperatorName(const std::string& name) { operatorName_.assign(name); }

			int getTransactionQuantity() const { return transQuantity_; }
			void incrementTransactionQuantity(int inc = 1) { transQuantity_ += inc; }
			void setTransactionQuantity(int qty) { transQuantity_ = qty; }

			HobPrice getOldTotal() const { return oldTotal_; }
			void incrementOldTotal(HobPrice inc) { oldTotal_ += inc; }
			void setOldTotal(HobPrice total) { oldTotal_ = total; }

			HobPrice getDelta() const { return delta_; }
			void incrementDelta(HobPrice inc) { delta_ += inc; }
			void setDelta(HobPrice delta) { delta_ = delta; }


			/* --- Instance function members --- */


		private:
			/* --- Instance data members --- */
			std::string operatorId_;
			std::string operatorName_;
			int transQuantity_;
			HobPrice oldTotal_;
			HobPrice delta_;


			/* --- Instance function members --- */
			void init();


			/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif


			/* --- Class function members -- */
	};


}



#endif //OPERATORCHANGESUMMARY_H
