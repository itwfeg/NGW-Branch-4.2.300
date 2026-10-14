#ifndef EVOLWRAPPAERSYSTEMINFOWRITE_H
#define EVOLWRAPPAERSYSTEMINFOWRITE_H

#include <string>
#include "Wrapper.h"
#include "WrapperGeneric.h"

#include "EvolWrapperSystemInfo.h"

class EvolWrapperSystemInfoWrite : public WrapperGeneric
{
	public:
		EvolWrapperSystemInfoWrite(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		EvolWrapperSystemInfoWrite(const char *buffer);
		~EvolWrapperSystemInfoWrite();

		const char *serialize()const;

		std::string getAwsSerialNumber() const {return awsSerialNumber_;}
		void setAwsSerialNumber(std::string & serialNumber){awsSerialNumber_ = serialNumber;}

		std::string getAwsDateOfManufacture() const {return awsDateOfManufacture_;}
		void setAwsDateOfManufacture(std::string & date){awsDateOfManufacture_ = date;}

		std::string getAwsDateOfInstall() const {return awsDateOfInstall_;}
		void setAwsDateOfInstall(std::string & date){awsDateOfInstall_ = date;}

		EvolWrapperSystemInfo::EvolAWSModel getModelNumber() const {return modelNumber_;}
		void setModelNumber(unsigned int model) {modelNumber_ = (EvolWrapperSystemInfo::EvolAWSModel)model;}

		ULONG getAwsCycleCount() const {return wrapperCycleCount_;}
		void setAwsCycleCount(ULONG count) {wrapperCycleCount_ = count;}
		
		
	private:					
		std::string			awsSerialNumber_;
		std::string			awsDateOfManufacture_;
		std::string			awsDateOfInstall_;
		
		EvolWrapperSystemInfo::EvolAWSModel		modelNumber_;		
		
		ULONG				wrapperCycleCount_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif