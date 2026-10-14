
#ifndef SYSTEMGCDPOWERON_H
#define SYSTEMGCDPOWERON_H

#include "SystemGeneric.h"

class SystemGCDPowerOn : public EvolMessage {
	public:
		SystemGCDPowerOn();
		virtual ~SystemGCDPowerOn();

		const char *serialize() const;
		int getMessageLength();

	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //SYSTEMGCDPOWERON_H
