#ifndef EVOLWRAPPERERRORHISTORY
#define EVOLWRAPPERERRORHISTORY

#include "Wrapper.h"
#include "WrapperGeneric.h"
#include <string>

class HobThread;

class EvolWrapperErrorHistory : public WrapperGeneric
{
	public:
	
		EvolWrapperErrorHistory(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		EvolWrapperErrorHistory(const char *buffer);
		~EvolWrapperErrorHistory();

		UINT16 getWrapperError() const {return wrapperError_;}	
		void setWrapperError(UINT16 wrapperError){wrapperError_ = wrapperError;}

		UINT16 getPackageLength() const {return packageLength_;}	
		void setPackageLength(UINT16 packageLength){packageLength_ = packageLength;}

		UINT16 getPackageWidth() const {return packageWidth_;}	
		void setPackageWidth(UINT16 packageWidth){packageWidth_ = packageWidth;}

		UINT16 getPackageHeight() const {return packageHeight_;}	
		void setPackageHeight(UINT16 packageHeight){packageHeight_ = packageHeight;}

		WrFilmWidthSelection getFilmUsed() const {return filmUsed_;}	
		void setFilmUsed(WrFilmWidthSelection filmUsed){filmUsed_ = filmUsed;}
		
		bool getUsedBestFit() const {return usedWrapBoxBestFit_;} 
		void setUsedBestFit(bool usedBestFit){usedWrapBoxBestFit_ = usedBestFit;}

		bool getStretchModified() const {return modifiedNetStretch_;}
		void setStretchModified( bool stretchModified){modifiedNetStretch_ = stretchModified;} 
		
		std::string getTrayName() const {return trayName_;}
		void setTrayName(std::string & name){trayName_ = name;}

		std::string getDateTime() const {return dateTime_;}
		void setDateTime(std::string & dateTime){dateTime_ = dateTime;}

		static const unsigned int _MaxNameLength_ = 10;
		
		const char *serialize()const;

		void show();
	private:
		UINT16 wrapperError_;		

		UINT16 packageLength_;	//  int decimal form, ie 100 is 10.0 inches
		UINT16 packageWidth_;	//  int decimal form, ie 100 is 10.0 inches
		UINT16 packageHeight_;	//  int decimal form, ie 100 is 10.0 inches
		
		WrFilmWidthSelection filmUsed_;
		
		bool usedWrapBoxBestFit_;	//	Used a best fit wrap box to wrap the pkg.
		bool modifiedNetStretch_;	//	Modified % net stretch to wrap the pkg.
		
		std::string trayName_;		
		std::string dateTime_;		

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif