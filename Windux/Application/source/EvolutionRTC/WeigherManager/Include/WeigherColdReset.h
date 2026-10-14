#ifndef WEIGHERCOLDRESET_H
#define WEIGHERCOLDRESET_H

#include "WeigherGeneric.h"


class WeigherColdReset : public WeigherGeneric
{
	public:
		WeigherColdReset(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		WeigherColdReset(const char *buffer);
		~WeigherColdReset();

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //WEIGHERCOLDRESET_H
