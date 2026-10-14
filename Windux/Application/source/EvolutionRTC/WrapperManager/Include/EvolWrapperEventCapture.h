#ifndef EVOLWRAPPEREVENTCAPTURE
#define EVOLWRAPPEREVENTCAPTURE

#include <string>
#include "Wrapper.h"
#include "WrapperGeneric.h"


class EvolWrapperEventCapture : public WrapperGeneric
{
	public:
		
		EvolWrapperEventCapture(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		EvolWrapperEventCapture(const char *buffer);
		~EvolWrapperEventCapture();

		const char *serialize()const;
		
		WrEventCaptureType getEventCaptureType() const;
		void setEventCaptureType(WrEventCaptureType eventCaptureType);
		
		UINT16 getPackageId() const;
		void setPackageId(UINT16 id);
		
		UINT32 getValue() const;
		void setValue(UINT32 value);
		
		UINT16 getErrorNumber() const;
		void setErrorNumber(UINT16 num);
		
		INT16 getEventMonth() const;	
		void setEventMonth(INT16 month);

		INT16 getEventDay() const;	
		void setEventDay(INT16 day);

		INT16 getEventYear() const;	
		void setEventYear(INT16 year);

		INT16 getEventHour() const;	
		void setEventHour(INT16 hour);

		INT16 getEventMinutes() const;	
		void setEventMinutes(INT16 minutes);

		INT16 getEventSeconds() const;	
		void setEventSeconds(INT16 seconds);

		void show();
		
	private:
		WrEventCaptureType	captureType;
		UINT16				packageId;
		UINT32				value;
		UINT16				errorNum;
		
		//the following is when the event occured
		INT16				eventMonth;			
		INT16				eventDay;
		INT16				eventYear;
		INT16				eventHour;
		INT16				eventMinutes;
		INT16				eventSeconds;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif
