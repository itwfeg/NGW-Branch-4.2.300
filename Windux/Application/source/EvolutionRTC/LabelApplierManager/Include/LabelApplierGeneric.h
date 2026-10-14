#ifndef LABELAPPLIERGENERIC_H
#define LABELAPPLIERGENERIC_H

#include "EvolMessage.h"
#include "LabelApplierEvent.h"

//class HobThread;

class LabelApplierGeneric : public EvolMessage
{
	public:
		LabelApplierGeneric(LaEvent laEvent, Hobart::CEvolMsgReceiver *pHobThread = NULL);
		LabelApplierGeneric(const char *buffer);
		virtual ~LabelApplierGeneric();
		 
	private:
		LabelApplierGeneric();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //LABELAPPLIERGENERIC_H
