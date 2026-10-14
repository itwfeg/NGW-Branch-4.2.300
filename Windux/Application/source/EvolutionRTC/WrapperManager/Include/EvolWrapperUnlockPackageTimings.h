#ifndef EVOLWRAPPERUNLOCKPACKAGETIMINGS_H
#define EVOLWRAPPERUNLOCKPACKAGETIMINGS_H



#include <string>

#include "WrapperGeneric.h"


class HobThread;



class EvolWrapperUnlockPackageTimings : public WrapperGeneric
{
	public:
		// --- Constructors and destructor ---
		EvolWrapperUnlockPackageTimings(Hobart::CEvolMsgReceiver *pHobThread = 0);
		EvolWrapperUnlockPackageTimings(const char* buffer);
		~EvolWrapperUnlockPackageTimings();


	private:
		// --- Class Members ---

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* EVOLWRAPPERUNLOCKPACKAGETIMINGS_H */
