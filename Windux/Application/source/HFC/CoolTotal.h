#ifndef COOLTOTAL_H
#define COOLTOTAL_H



#include "Total.h"


#include "tags/Tags.h"

#include "HobDateTime.h"



namespace hfc
{


	class COOLTotal : public Total
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum
			{
				fieldOffsetTotalProductNum = Total::endTotalFieldOffsets,
				fieldOffsetTotalTransactionTypeNum,
				fieldOffsetTotalTimeStamp,
				fieldOffsetTotalCOOLText,
				fieldOffsetTotalCOOLTrackingText,
				fieldOffsetTotalCOOLFirstLogDate,
				fieldOffsetTotalCOOLLastLogDate,
				fieldOffsetTotalCOOLFirstLogTime,
				fieldOffsetTotalCOOLLastLogTime,

				endCOOLTotalFieldOffsets
			} COOLTotalFieldOffsets;


			/* --- Constructors and destructor --- */
			COOLTotal(HobRecordNum pluNumber, HobRecordNum transTypeNumber, std::string trackingText, std::string coolText = "");
			~COOLTotal();


			/* --- Getter and setter methods --- */
			HobRecordNum getProductNumber() const { return productNumber_; }
			void setProductNumber(HobRecordNum number) { productNumber_ = number; }

			HobRecordNum getTransactionTypeNumber() const { return transTypeNumber_; }
			void setTransactionTypeNumber(HobRecordNum number) { transTypeNumber_ = number; }

			const std::string& getTrackingText() const { return trackingText_; }
			void setTrackingText(const std::string& text) { trackingText_.assign(text); }

			const HobDateTime& getFirstDateTime() const { return firstDateTime_; }
			void setFirstDateTime(const HobDateTime& dt) { firstDateTime_ = dt; }

			const HobDateTime& getLastDateTime() const { return lastDateTime_; }
			void setLastDateTime(const HobDateTime& dt) { lastDateTime_ = dt; }

			void setDateTime(const HobDateTime& dt) { if ( dt < firstDateTime_ ) firstDateTime_ = dt; if ( lastDateTime_ < dt ) lastDateTime_ = dt;  }

			const std::string& getCoolText() const { return coolText_; }
			void setCoolText(const std::string& text) { coolText_.assign(text); }

        #if defined(LINUX_FIX)
			bool isCoolTextVaries() const { return coolTextVaries_; }
        #else
			const bool isCoolTextVaries() const { return coolTextVaries_; }
        #endif
			void setCoolTextVaries(bool varies = true) { coolTextVaries_ = varies; }


			/* --- Instance function members --- */
			void getFieldAsString(int fieldCode, HobString &result, int format) const;
			const char *serialize(int format) const;

			bool isRecordValid() const { return true; }


		private:
			/* --- Instance data members --- */
			HobRecordNum    productNumber_;
			HobRecordNum    transTypeNumber_;
			std::string     trackingText_;
			HobDateTime     firstDateTime_;
			HobDateTime     lastDateTime_;
			std::string     coolText_;
			bool            coolTextVaries_;


			/* --- Instance function members --- */
			void init();
			void show();


			/* --- Class data members --- */
			static FieldOffsetsList _fieldList_, _ultimaFieldList_;

	#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
	#endif


			/* --- Class function members -- */
			static void initFieldList();
	};


}



#endif //COOLTOTAL_H
