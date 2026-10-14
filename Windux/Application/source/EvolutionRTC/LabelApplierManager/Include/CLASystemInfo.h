#ifndef CLASYSTEMINFO_H
#define CLASYSTEMINFO_H

#include <string>
#include "EvolMessage.h"
#include "LabelApplierEvent.h"
#include "LabelApplier.h"
#include "System.h"



class CLASystemInfo : public EvolMessage
{
	public:
		CLASystemInfo(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		CLASystemInfo(const char *buffer);
		~CLASystemInfo();

		const char *serialize()const;

		typedef enum
		{
			NoModel,
			NoModel_1,
			CLAS,		//CLA with Wrapper Node Board Controller
			CLAS_B,	//CLAS without Wakeup and Weigher Entry Sensors. 
		}AWSModel;

		///*	SETUP CLASysInfo	*/
		//ULONG getConfigurationChecksum() const {return configurationChecksum_;}
		//void setConfigurationChecksum(ULONG configurationChecksum) {configurationChecksum_ = configurationChecksum;}

		//UINT16 getAppFlashMajor() const {return appFlashMajor_;}
		//void setAppFlashMajor(UINT16 appFlashMajor) {appFlashMajor_ = appFlashMajor;}
		//UINT16 getAppFlashMinor() const {return appFlashMinor_;}
		//void setAppFlashMinor(UINT16 appFlashMinor) {appFlashMinor_ = appFlashMinor;}

		//UINT16 getDataFlashMajor() const {return dataFlashMajor_;}
		//void setDataFlashMajor(UINT16 dataFlashMajor) {dataFlashMajor_ = dataFlashMajor;}
		//UINT16 getDataFlashMinor() const {return dataFlashMinor_;}
		//void setDataFlashMinor(UINT16 dataFlashMinor) {dataFlashMinor_ = dataFlashMinor;}

		//UINT16 getBootFlashMajor() const {return bootFlashMajor_;}
		//void setBootFlashMajor(UINT16 bootFlashMajor) {bootFlashMajor_ = bootFlashMajor;}
		//UINT16 getBootFlashMinor() const {return bootFlashMinor_;}
		//void setBootFlashMinor(UINT16 bootFlashMinor) {bootFlashMinor_ = bootFlashMinor;}

		/*	SETUP EPP93C56Contents	*/
		//Alex Zavala 8/21
		ULONG getBoardSectionChecksum()const {return boardSectionChecksum_;}
		void setBoardSectionChecksum(ULONG boardSectionChecksum) {boardSectionChecksum_ = boardSectionChecksum;}
		
		ULONG getAwsSectionChecksum() const {return awsSectionChecksum_;}
		void setAwsSectionChecksum(ULONG awsSectionChecksum)  {awsSectionChecksum_ = awsSectionChecksum;}

		/*	EEP93C56Board	*/

		//defined in cpp
		//const UINT8 *getPartNumber() const;
		//void setPartNumber(const UINT8 *partNumber);
		std::string getPartNumber() const {return partNumber_;}
		void setPartNumber(std::string partNumber) {partNumber_ = partNumber;}

		//defined in cpp
		//const UINT8 *getBoardRev() const;
		//void setBoardRev(const UINT8 *boardRev);
		std::string getBoardRev() const {return boardRev_;}
		void setBoardRev(std::string boardRev) {boardRev_ = boardRev;}


		//defined in cpp
		//const UINT8 *getDateOfManufacture() const;
		//void setDateOfManufacture(const UINT8 *dateOfManufacture);
		std::string getDateOfManufacture() const {return dateOfManufacture_;}
		void setDateOfManufacture(std::string dateOfManufacture) {dateOfManufacture_ = dateOfManufacture;}

		//defined in cpp
		//const UINT8 *getSerialNumber() const;
		//void setSerialNumber(const UINT8 *serialNumber);
		std::string getSerialNumber() const {return serialNumber_;}
		void setSerialNumber(std::string serialNumber) {serialNumber_ = serialNumber;}

		/*	EEP93C56AWS	*/

		//defined in cpp
		//const UINT8 *getAwsSerialNumber() const;
		//void setAwsSerialNumber(const UINT8 *serialNumber);
		std::string getAwsSerialNumber() const {return awsSerialNumber_;}
		void setAwsSerialNumber(std::string serialNumber) {awsSerialNumber_ = serialNumber;}

		//defined in cpp
		//const UINT8 *getAwsDateOfManufacture() const;
		//void setAwsDateOfManufacture(const UINT8 *date);
		std::string getAwsDateOfManufacture() const {return awsDateOfManufacture_;}
		void setAwsDateOfManufacture(std::string date) {awsDateOfManufacture_ = date;}

		//defined in cpp
		//const UINT8 *getAwsDateOfInstall() const;
		//void setAwsDateOfInstall(const UINT8 *date);
		std::string getAwsDateOfInstall() const {return awsDateOfInstall_;}
		void setAwsDateOfInstall(std::string date){awsDateOfInstall_ = date;}

		//defined in cpp
		void initSysInfoArray(UINT8 *var);

		AWSModel getModelNumber() const {return modelNumber_;}
		void setModelNumber(UINT16 model) {modelNumber_ = (AWSModel)model;}
		
		std::string getAwsModelNumber()const {return awsModelNumber_;}  		

		bool isModelNumberBlank() const {return (modelNumber_ == NoModel) ? true : false;}

		ULONG getCycleCount() const {return cycleCount_;}
		void setCycleCount(ULONG count) {cycleCount_ = count;}
		
		void show();
		
	private:
#if 0 //080321 CG removed. not applicable to CLA. PJR-175		
		void setAwsModelNumber(unsigned int model);	
#endif		
		///*	CLASysInfo	*/
		//ULONG			configurationChecksum_;
		//UINT16			appFlashMajor_;
		//UINT16			appFlashMinor_;
		//UINT16			dataFlashMajor_;
		//UINT16			dataFlashMinor_;
		//UINT16			bootFlashMajor_;
		//UINT16			bootFlashMinor_;

		/*	EEP93C56Board	*/
		//UINT8			partNumber_[PART_NUMBER_SIZE];
		//UINT8			boardRev_[REVISION_SIZE];
		//UINT8			dateOfManufacture_[DATE_SIZE];
		//UINT8			serialNumber_[SERIAL_NUMBER_SIZE];
		std::string		partNumber_;
		std::string		boardRev_;
		std::string		dateOfManufacture_;
		std::string		serialNumber_;
		ULONG			boardSectionChecksum_;
		
		/*	EEP93C56AWS	*/
		//UINT8			awsSerialNumber_[AWS_SERIAL_NUMBER_SIZE];
		//UINT8			awsDateOfManufacture_[DATE_SIZE];
		//UINT8			awsDateOfInstall_[DATE_SIZE];
		std::string		awsSerialNumber_;
		std::string		awsDateOfManufacture_;
		std::string		awsDateOfInstall_;
		std::string		awsModelNumber_;
		AWSModel		modelNumber_;		
		ULONG			cycleCount_;
		ULONG			awsSectionChecksum_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif