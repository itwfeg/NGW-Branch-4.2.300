
#ifndef NGWGENERIC_H_
#define NGWGENERIC_H_

#include "EvolMessage.h"
#include "NGWEvent.h"
#include "CEvolMsgReceiver.h"

class NGWGeneric : public EvolMessage
{
	public:
		NGWGeneric(NGWEvents ngwEvent, Hobart::CEvolMsgReceiver *pHobThread = NULL);
		NGWGeneric(const char *buffer);
		virtual ~NGWGeneric();

	private:
		NGWGeneric();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif
