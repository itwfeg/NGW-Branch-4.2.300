
#ifndef SYSTEMCONFIGUREGCD_H
#define SYSTEMCONFIGUREGCD_H

#include "SystemGeneric.h"

class SystemConfigureGCD : public EvolMessage {
	public:
		SystemConfigureGCD();
		virtual ~SystemConfigureGCD();

		const char *serialize() const;
		int getMessageLength();

	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //SYSTEMCONFIGUREGCD_H
