
#ifndef WRAPPERGENERICPOINTER_H
#define WRAPPERGENERICPOINTER_H

#include "WrapperGeneric.h"
#include "WrapperEvent.h"

class HobThread;
class EvolWrapperPackageTimings;

class EvolWrapperPackageTimingsPointer : public WrapperGeneric
{
	public:
		EvolWrapperPackageTimingsPointer(WrEvent wrEvent, EvolWrapperPackageTimings *wptPointer = 0);
		EvolWrapperPackageTimingsPointer(const char *buffer);
		virtual ~EvolWrapperPackageTimingsPointer();

		void setTimingsPointer(EvolWrapperPackageTimings *wptPointer);
		EvolWrapperPackageTimings *getTimingsPointer();
		const EvolWrapperPackageTimings *getTimingsPointer()const;

		const char *serialize()const;
		UINT32 getSerializedLength()const;

	private:
		EvolWrapperPackageTimings	*timingsPointer;

		EvolWrapperPackageTimingsPointer();

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif //WRAPPERGENERICPOINTER_H
