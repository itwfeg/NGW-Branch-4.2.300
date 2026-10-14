#ifndef HOURLYTOTAL_H
#define HOURLYTOTAL_H



#include "Total.h"


#include "tags/Tags.h"



namespace hfc
{


	class HourlyTotal : public Total
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum
			{
				fieldOffsetHourlyTotalNum = Total::endTotalFieldOffsets,

				endHourlyTotalFieldOffsets
			} HourlyTotalFieldOffsets;

			static const int _maxHour_ = 24;


			/* --- Constructors and destructor --- */
			HourlyTotal(short hour);
			~HourlyTotal();


			/* --- Getter and setter methods --- */
			short getHour() const { return hour_; }
			void setHour(short hour) { hour_ = hour; }


			/* --- Instance function members --- */
			void getFieldAsString(int fieldCode, HobString &result, int format) const;
			const char *serialize(int format) const;

			bool isRecordValid() const { return true; }


		private:
			/* --- Instance data members --- */
			short hour_;


			/* --- Instance function members --- */
			void init();


			/* --- Class data members --- */
			static FieldOffsetsList _fieldList_;

#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif


			/* --- Class function members -- */
			static void initFieldList();
	};


}



#endif //HOURLYTOTAL_H
