#ifndef EVOLWRAPPERMOTORPROFILE
#define EVOLWRAPPERMOTORPROFILE

#include "Wrapper.h"
#include "WrapperGeneric.h"
#include <string>

class HobThread;

class EvolWrapperMotorProfile : public WrapperGeneric
{
	public:

		typedef enum BldcMotorStatus
		{
			MotorEnergyInRange,
			MotorEnergyWarning,
			MotorEnergyAlert	
		}BldcMotorStatus_t;
	

		EvolWrapperMotorProfile(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		EvolWrapperMotorProfile(const char *buffer);		
		~EvolWrapperMotorProfile();

		UINT16 getBldcMechanism() const {return bldcMechanism_;}	
		void setBldcMechanism(UINT16 bldcMechanism){bldcMechanism_ = bldcMechanism;}

		BldcMotorStatus getBldcMotorStatus() const {return bldcMotorStatus_;}	
		void setBldcMotorStatus(UINT16 status){bldcMotorStatus_ = (BldcMotorStatus)status;}

		UINT16 getBldcMotorCapacityUsage() const {return bldcMotorCapacityUsage_;}	
		void setBldcMotorCapacityUsage(UINT16 bldcMotorCapacityUsage){bldcMotorCapacityUsage_ = bldcMotorCapacityUsage;}

		const char *serialize()const;

	private:
		UINT16 bldcMechanism_;		
		BldcMotorStatus bldcMotorStatus_;		
		UINT16 bldcMotorCapacityUsage_;		

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif