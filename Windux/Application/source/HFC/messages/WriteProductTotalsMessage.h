#ifndef WRITEPRODUCTTOTALSMESSAGE_H
#define WRITEPRODUCTTOTALSMESSAGE_H



#include "HfcObject.h"


#include "HobList.h"
#include "CSmartPtr.h"


namespace hfc
{


	class ProductTotal;


	class WriteProductTotalsMessage : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum WriteProductTotalsOffsets
			{
				fieldOffsetWriteProductTotalsTransactionTypes = 1,
				fieldOffsetWriteProductTotalsWeights,
				fieldOffsetWriteProductTotalsPrices,
				fieldOffsetWriteProductTotalsItems,
				fieldOffsetWriteProductTotalsPackages,

				endWriteProductTotalsOffsets
			} WriteProductTotalsOffsets_t;


			/* --- Constructors and destructor --- */
			WriteProductTotalsMessage();
			~WriteProductTotalsMessage();


			/* --- Getters and setters --- */


			/* --- Instance function members --- */
			void addProductTotal(Util::CSmartPtr<ProductTotal> total);

			const char * serialize(int format) const;


			/* --- Class function members --- */


		private:
			/* --- Data members --- */
			HobList transTypes_;
			HobList weights_;
			HobList prices_;
			HobList items_;
			HobList packages_;


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif

			/* --- Constructors --- */


			/* --- Instance function members --- */
	};


}



#endif /* WRITEPRODUCTTOTALSMESSAGE_H */
