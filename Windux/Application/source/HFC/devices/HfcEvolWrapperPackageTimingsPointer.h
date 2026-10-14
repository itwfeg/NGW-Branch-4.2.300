#ifndef HFCEVOLWRAPPERPACKAGETIMINGSPTR_H
#define HFCEVOLWRAPPERPACKAGETIMINGSPTR_H



#include <string>

#include "EvolWrapperPackageTimingsPointer.h"
#include "HfcObject.h"


class HfcEvolWrapperPackageTimingsPointer : public EvolWrapperPackageTimingsPointer, public HfcObject
{
	public:
		// --- Constructors and destructor ---
		HfcEvolWrapperPackageTimingsPointer(const char* buffer);
		HfcEvolWrapperPackageTimingsPointer(WrEvent wrEvent, EvolWrapperPackageTimings *ewpt);
		~HfcEvolWrapperPackageTimingsPointer();

		virtual const char *serialize(int format = InternalFormat)const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* HFCEVOLWRAPPERPACKAGETIMINGSPTR_H */
