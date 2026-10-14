#ifndef WRITEHOURLYTOTALSMESSAGE_H
#define WRITEHOURLYTOTALSMESSAGE_H



#include "HfcObject.h"


#include "HobList.h"
#include "CSmartPtr.h"


namespace hfc
{


	class HourlyTotal;


	class WriteHourlyTotalsMessage : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum WriteHourlyTotalsOffsets
			{
				fieldOffsetWriteHourlyTotalsHour = 1,
				fieldOffsetWriteHourlyTotalsWeights,
				fieldOffsetWriteHourlyTotalsPrices,
				fieldOffsetWriteHourlyTotalsItems,
				fieldOffsetWriteHourlyTotalsPackages,

				endWriteHourlyTotalsOffsets
			} WriteHourlyTotalsOffsets_t;


			/* --- Constructors and destructor --- */
			WriteHourlyTotalsMessage();
			//WriteHourlyTotalsMessage(const char * data, int len, int formatType);
			~WriteHourlyTotalsMessage();


			/* --- Getters and setters --- */


			/* --- Instance function members --- */
			void addHourlyTotal(Util::CSmartPtr<HourlyTotal> total);

			const char * serialize(int format) const;


			/* --- Class function members --- */


		private:
			/* --- Data members --- */
			HobList hours_;
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



#endif /* WRITEHOURLYTOTALSMESSAGE_H */
