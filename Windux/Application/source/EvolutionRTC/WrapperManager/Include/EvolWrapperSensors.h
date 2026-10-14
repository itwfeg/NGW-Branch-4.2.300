#ifndef EVOLWRAPPERSENSORS_H
#define EVOLWRAPPERSENSORS_H

#include <string>

#include "WrapperGeneric.h"
#include "Wrapper.h"
#include "EvolWrapperGeneralSensors.h"
#include "EvolWrapperInterlockSensors.h"


class EvolWrapperSensors : public WrapperGeneric
{
	public:
		EvolWrapperSensors(Hobart::CEvolMsgReceiver *pHobThread = 0);
		EvolWrapperSensors(const char *buffer);
		EvolWrapperSensors(const EvolWrapperSensors &ews);
		~EvolWrapperSensors();

		const char *serialize() const;
		UINT32 getSerializedLength() const;

		void show();


		EvolWrapperGeneralSensors &getGeneralSensors();
		const EvolWrapperGeneralSensors &getGeneralSensors()const;

		EvolWrapperInterlockSensors &getInterlockSensors();
		const EvolWrapperInterlockSensors &getInterlockSensors()const;

		EvolWrapperGeneralSensors_2 &getGeneralSensors_2();
		const EvolWrapperGeneralSensors_2 &getGeneralSensors_2()const;

		bool equals(const EvolWrapperSensors &wrSensors)const;
		void operator=(EvolWrapperSensors &ews);

	private:

		EvolWrapperGeneralSensors	genSensors;
		EvolWrapperInterlockSensors	interlockSensors;
		EvolWrapperGeneralSensors_2 genSensors_2;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif // EVOLWRAPPERSENSORS_H
