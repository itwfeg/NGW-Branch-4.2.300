#ifndef EVOLWRAPPERWRITECAMERATHRESHOLD
#define EVOLWRAPPERWRITECAMERATHRESHOLD

#include "Wrapper.h"
#include "WrapperGeneric.h"
#include <string>

class HobThread;

class EvolWrapperWriteCameraThreshold : public WrapperGeneric
{
	public:
	
		EvolWrapperWriteCameraThreshold(Hobart::CEvolMsgReceiver *pHobThread = NULL, UCHAR threshold = 0);
		EvolWrapperWriteCameraThreshold(const char *buffer);
		~EvolWrapperWriteCameraThreshold();

		UCHAR getWrapperWriteCameraThreshold() const {return threshold_;}	
		void setWrapperWriteCameraThreshold(UCHAR wrapperThreshold){threshold_ = wrapperThreshold;}

		
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