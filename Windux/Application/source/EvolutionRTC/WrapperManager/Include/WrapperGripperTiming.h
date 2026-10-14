#ifndef WRAPPERGRIPPERTIMING_H
#define WRAPPERGRIPPERTIMING_H

#include <string>

#include "Wrapper.h"
#include "WrapperTime.h"

class WrapperGripperTiming
{
	public:
		WrapperGripperTiming(WrapperTime &setback, WrGripperPwm pwm);
		WrapperGripperTiming(const WrapperTime *setback = NULL, WrGripperPwm pwm = IgnoreGripperPwm);
		WrapperGripperTiming(const char *buffer);
		WrapperGripperTiming(WrapperGripperTiming &gripperSetback);
		virtual ~WrapperGripperTiming();

		const char *serialize() const;
		UINT32 getSerializedLength() const;

		void show();

		WrapperTime &getSetback();
		void setSetback(WrapperTime &setback);

		WrGripperPwm getPwm();
		void setPwm(WrGripperPwm pwm);

		void setPwm100();
		bool isPwm100();
		void setPwm074();
		bool isPwm074();
		void setPwm055();
		bool isPwm055();
		void setPwm038();
		bool isPwm038();
		void setIgnorePwm();
		bool isIgnorePwm();

	private:
		WrapperTime setback;
		WrGripperPwm pwm;

		mutable std::string serializedData;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};
#endif // WRAPPERGRIPPERTIMING_H
