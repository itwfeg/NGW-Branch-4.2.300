#ifndef HFCEVOLWRAPPERMOTORPROFILE_H
#define HFCEVOLWRAPPERMOTORPROFILE_H

#include "EvolWrapperMotorProfile.h"
#include "HfcObject.h"
#include "evolution.h"

typedef enum HfcEvolWrapperMotorProfileOffsets
{	
	fieldOffsetMotorProfileMechanism = 1,
	fieldOffsetMotorProfileStatus,
	fieldOffsetMotorProfileCapacity,		
	fieldOffsetMotorProfileEnd
} HfcEvolWrapperMotorProfileOffsets_t;


class HfcEvolWrapperMotorProfile : public EvolWrapperMotorProfile, public HfcObject
{
	public:

		HfcEvolWrapperMotorProfile(const char *data);

		~HfcEvolWrapperMotorProfile();

		virtual const char *serialize(int format = BrowserFormat)const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};


#endif
