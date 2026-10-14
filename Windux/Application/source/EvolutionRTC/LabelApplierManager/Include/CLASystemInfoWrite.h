#ifndef CLASYSTEMINFOWRITE_H
#define CLASYSTEMINFOWRITE_H

#include <string>
#include "EvolMessage.h"
#include "LabelApplierEvent.h"
#include "LabelApplier.h"
#include "System.h"
#include "CLASystemInfo.h"


class CLASystemInfoWrite : public EvolMessage
{
	public:
		CLASystemInfoWrite(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		CLASystemInfoWrite(const char *buffer);
		~CLASystemInfoWrite();

		const char *serialize()const;

		/*	EEP93C56AWS	*/

		std::string getAwsSerialNumber() const {return awsSerialNumber_;}
		void setAwsSerialNumber(std::string serialNumber) {awsSerialNumber_ = serialNumber;}

		std::string getAwsDateOfManufacture() const {return awsDateOfManufacture_;}
		void setAwsDateOfManufacture(std::string date) {awsDateOfManufacture_ = date;}

		std::string getAwsDateOfInstall() const {return awsDateOfInstall_;}
		void setAwsDateOfInstall(std::string date){awsDateOfInstall_ = date;}

		CLASystemInfo::AWSModel getModelNumber() const {return modelNumber_;}
		void setModelNumber(UINT16 model) {modelNumber_ = (CLASystemInfo::AWSModel)model;}		

		ULONG getCycleCount() const {return cycleCount_;}
		void setCycleCount(ULONG count) {cycleCount_ = count;}

		ULONG getAwsSectionChecksum() const {return awsSectionChecksum_;}
		void setAwsSectionChecksum(ULONG awsSectionChecksum)  {awsSectionChecksum_ = awsSectionChecksum;}
		
		void show();

		//Sizes for the format of the webpage
		static const unsigned int _serial_number_size_	= 9;
		
	private:
		
		/*	EEP93C56AWS	*/
		std::string					awsSerialNumber_;
		std::string					awsDateOfManufacture_;
		std::string					awsDateOfInstall_;
		std::string					awsModelNumber_;
		CLASystemInfo::AWSModel		modelNumber_;	
		ULONG						cycleCount_;
		ULONG						awsSectionChecksum_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif
