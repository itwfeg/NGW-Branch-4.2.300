#ifndef VOIDTRANSACTIONSMESSAGE_H
#define VOIDTRANSACTIONSMESSAGE_H



#include "HfcObject.h"


#include "HobList.h"



namespace hfc
{


	class VoidTransactionsMessage : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum ReadTransactionGroupOffsets
			{
				fieldOffsetVoidTransactionsList = 1,

				endVoidTransactionsOffsets
			} ReadTransactionGroupOffsets_t;


			/* --- Constructors and destructor --- */
			VoidTransactionsMessage(const char * data, int len, int formatType);
			~VoidTransactionsMessage();


			/* --- Getters and setters --- */
			HobList& getTaskIdentifiers() { return ids_; }
			const HobList& getTaskIdentifiers() const { return ids_; }
			void setTaskIdentifiers(const HobList& ids) { ids_ = ids; }


			/* --- Instance function members --- */


			/* --- Class function members --- */


		private:
			/* --- Data members --- */
			HobList ids_;


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif

			/* --- Constructors --- */
			VoidTransactionsMessage();


			/* --- Instance function members --- */
			void init();
	};


}



#endif /* VOIDTRANSACTIONSMESSAGE_H */
