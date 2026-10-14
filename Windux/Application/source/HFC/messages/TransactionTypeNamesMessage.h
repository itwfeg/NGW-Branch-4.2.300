#ifndef TRANSACTIONTYPENAMESMESSAGE_H
#define TRANSACTIONTYPENAMESMESSAGE_H



#include "HfcObject.h"

#include "HobList.h"
#include "CSmartPtr.h"


namespace hfc
{


	class TransactionTypeNamesMessage : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */


			/* --- Constructors and destructor --- */
			TransactionTypeNamesMessage();
			~TransactionTypeNamesMessage();

			/* --- Getters and setters --- */


			/* --- Instance function members --- */
			void addTransactionType(const std::string& number, const std::string& name1, const std::string& name2);

			const char * serialize(int format = BrowserFormat) const;


		private:
			/* --- Instance Data members --- */
			HobList typeNumbers_;
			HobList namesPrimary_;
			HobList namesSecondary_;


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


}



#endif //TRANSACTIONTYPENAMESMESSAGE_H
