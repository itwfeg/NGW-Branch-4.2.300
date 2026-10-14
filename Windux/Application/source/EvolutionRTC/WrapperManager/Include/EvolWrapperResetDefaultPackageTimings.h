#ifndef EVOLWRAPPERRESETDEFAULTPACKAGETIMINGS_H
#define EVOLWRAPPERRESETDEFAULTPACKAGETIMINGS_H



#include <string>

#include "WrapperGeneric.h"


class HobThread;



class EvolWrapperResetDefaultPackageTimings : public WrapperGeneric
{
	public:
		// --- Constructors and destructor ---
		EvolWrapperResetDefaultPackageTimings(Hobart::CEvolMsgReceiver *pHobThread = 0);
		EvolWrapperResetDefaultPackageTimings(const char* buffer);
		~EvolWrapperResetDefaultPackageTimings();


	private:
		// --- Class Members ---

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* EVOLWRAPPERRESETDEFAULTPACKAGETIMINGS_H */
