#ifndef WRITEOPERATORTOTALSMESSAGE_H
#define WRITEOPERATORTOTALSMESSAGE_H



#include "HfcObject.h"


#include "HobList.h"
#include "CSmartPtr.h"


namespace hfc
{


	class OperatorTotal;


	class WriteOperatorTotalsMessage : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum WriteOperatorTotalsOffsets
			{
				fieldOffsetWriteOperatorTotalsNames = 1,
				fieldOffsetWriteOperatorTotalsWeights,
				fieldOffsetWriteOperatorTotalsPrices,
				fieldOffsetWriteOperatorTotalsItems,
				fieldOffsetWriteOperatorTotalsPackages,

				endWriteOperatorTotalsOffsets
			} WriteOperatorTotalsOffsets_t;


			/* --- Constructors and destructor --- */
			WriteOperatorTotalsMessage();
			//WriteOperatorTotalsMessage(const char * data, int len, int formatType);
			~WriteOperatorTotalsMessage();


			/* --- Getters and setters --- */


			/* --- Instance function members --- */
			void addOperatorTotal(Util::CSmartPtr<OperatorTotal> total, const std::string& name);

			const char * serialize(int format) const;


			/* --- Class function members --- */


		private:
			/* --- Data members --- */
			HobList names_;
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



#endif /* WRITEOPERATORTOTALSMESSAGE_H */
