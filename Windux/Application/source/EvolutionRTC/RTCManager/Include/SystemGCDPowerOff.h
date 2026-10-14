
#ifndef SYSTEMGCDPOWEROFF_H
#define SYSTEMGCDPOWEROFF_H

#include "SystemGeneric.h"

class SystemGCDPowerOff : public EvolMessage {
	public:
		SystemGCDPowerOff();
		virtual ~SystemGCDPowerOff();

		const char *serialize() const;
		int getMessageLength();

	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //SYSTEMGCDPOWEROFF_H
