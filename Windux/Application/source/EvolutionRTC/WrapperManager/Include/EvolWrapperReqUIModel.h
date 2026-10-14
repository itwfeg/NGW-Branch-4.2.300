#ifndef EvolWrapperReqUIModel_H
#define EvolWrapperReqUIModel_H

#include "WrapperGeneric.h"

class EvolWrapperReqUIModel : public WrapperGeneric
{
	public:
		EvolWrapperReqUIModel(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		EvolWrapperReqUIModel(const char *buffer);
		const char *serialize()const;
		
		~EvolWrapperReqUIModel();

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};


#endif