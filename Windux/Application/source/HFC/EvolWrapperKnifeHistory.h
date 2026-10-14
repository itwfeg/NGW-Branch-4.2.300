#ifndef EVOLWRAPPERKNIFEHISTORY
#define EVOLWRAPPERKNIFEHISTORY

#include <string>

#include "HfcObject.h"
#include "HobDateTime.h"


	class EvolWrapperKnifeHistory : public HfcObject
	{
		public:
			
			typedef enum 
			{
				KnifeTimeNone,
				KnifeTimeInstall,
				KnifeTimeMin,		
				KnifeTimeMax
			}KnifeHistoryType;

			EvolWrapperKnifeHistory();
			~EvolWrapperKnifeHistory();
			
			HobErrorCode getErrorCode() const { return errorCode_; }
			void setErrorCode(HobErrorCode errorCode){errorCode_ = errorCode;}
			bool isRecordValid() const { return errorCode_ == RECORD_CORRUPT; }


			KnifeHistoryType getType() const { return knifeHistoryType_; }
			void setType(KnifeHistoryType knifeHistoryType) { knifeHistoryType_ = knifeHistoryType; }

			int getKnifeTime() const { return knifeTime_; }
			void setKnifeTime(int time) { knifeTime_ = time; }

			HobDateTime getHobTimeStampDateTime() const { return HobDateTime::createDateTimeFromDBFormat(knifeDateTime_); }
			const std::string& getTimeStampDateTime() const { return knifeDateTime_; }
			void setTimeStampDateTime(const std::string& dt) { knifeDateTime_ = dt; }
			void setTimeStampHobDateTime(const HobDateTime& dt = HobDateTime()) { knifeDateTime_ = dt.convertToDBFormat(); }

			short getHobDateDay() const;
			short getHobDateMonth() const;
			short getHobDateYear() const;
			
			const char *serialize(int format = BrowserFormat)const;

		private:
			
			KnifeHistoryType knifeHistoryType_;
			int knifeTime_;
			std::string knifeDateTime_;
			HobErrorCode errorCode_;

#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};



#endif