#ifndef OPERATORCHANGESSUMMARYMESSAGE_H
#define OPERATORCHANGESSUMMARYMESSAGE_H



#include "HfcObject.h"

#include "HobList.h"
#include "CSmartPtr.h"


namespace hfc
{


	class OperatorChangeSummary;


	class OperatorChangesSummaryMessage : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */


			/* --- Constructors and destructor --- */
			OperatorChangesSummaryMessage();
			~OperatorChangesSummaryMessage();


			/* --- Getters and setters --- */


			/* --- Instance function members --- */
			void addOperatorSummary(Util::CSmartPtr<OperatorChangeSummary> summary);

			const char * serialize(int format = BrowserFormat) const;


		private:
			/* --- Instance Data members --- */
			HobList operatorIds_;
			HobList operatorNames_;
			HobList transQuantities_;
			HobList oldTotals_;
			HobList deltas_;


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


}



#endif //OPERATORCHANGESSUMMARYMESSAGE_H
