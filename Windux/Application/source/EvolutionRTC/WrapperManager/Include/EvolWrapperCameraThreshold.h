#ifndef EVOLWRAPPERCAMERATHRESHOLD
#define EVOLWRAPPERCAMERATHRESHOLD

#include "Wrapper.h"
#include "WrapperGeneric.h"
#include <string>

class HobThread;

class EvolWrapperCameraThreshold : public WrapperGeneric
{
	public:
	
		EvolWrapperCameraThreshold(Hobart::CEvolMsgReceiver *pHobThread = NULL, UCHAR threshold = 0);
		EvolWrapperCameraThreshold(const char *buffer);		
		~EvolWrapperCameraThreshold();

		UCHAR getWrapperCameraThreshold() const {return threshold_;}	
		void setWrapperCameraThreshold(const char *pWrapperThreshold){threshold_ = *pWrapperThreshold;}

		
		const char *serialize()const;
		void show();

	private:
		UCHAR threshold_;		

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif