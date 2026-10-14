#include "CLASystemInfo.h"
#include "LabelApplierEvent.h"
//Alex Zavala 8/21
#include "Debug.h"
#include "Hex.h"
#include "Bool.h"
#include "Integer.h"
#include "Long.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CLASystemInfo::_dataType_ = 0;
std::string CLASystemInfo::_dataTypeName_("CLASystemInfo"); 
#endif


CLASystemInfo::CLASystemInfo(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(LabelApplier, LaSysInfo, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	//boardSectionChecksum_  = false;
	//awsSectionChecksum_ = false;
	//Alex Zavala 8/21
	boardSectionChecksum_  = 0;
	awsSectionChecksum_ = 0;

	//configurationChecksum_ = 0;

	//appFlashMajor_ = 0;
	//appFlashMinor_ = 0;

	//dataFlashMajor_ = 0;
	//dataFlashMinor_ = 0;

	//bootFlashMajor_ = 0;
	//bootFlashMinor_ = 0;
	/*
	partNumber_ =NULL;
	boardRev_ = NULL;
	dateOfManufacture_ = NULL;
	serialNumber_ = NULL;

	awsSerialNumber_ = NULL;
	awsDateOfManufacture_ = NULL;
	awsDateOfInstall_ = NULL;
	*/
/*	initSysInfoArray(partNumber_);
	initSysInfoArray(boardRev_);
	initSysInfoArray(dateOfManufacture_);
	initSysInfoArray(serialNumber_);
	initSysInfoArray(awsSerialNumber_);
	initSysInfoArray(awsDateOfManufacture_);
	initSysInfoArray(awsDateOfInstall_);
*/
	partNumber_ = "";
	boardRev_ = "";
	dateOfManufacture_ = "";
	serialNumber_ = "";
	awsSerialNumber_ = "";
	awsDateOfManufacture_ = "";
	awsDateOfInstall_ = "";
	awsModelNumber_ = "NoModel";
	cycleCount_ = 0;
}

CLASystemInfo::CLASystemInfo(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	EvolMessage::serialize();
	int j = getSerializedLength();

	// Now, for the derived class items

	///*	CLASysInfo	*/

	//char *pChar = (char *) &configurationChecksum_;
	//for (unsigned int i = 0; i < sizeof(configurationChecksum_); i++)
	//{
	//	*pChar++ = buffer[j++];
	//}

	//pChar = (char *) &appFlashMajor_;
	//for (unsigned int i = 0; i < sizeof(appFlashMajor_); i++)
	//{
	//	*pChar++ = buffer[j++];
	//}
	//pChar = (char *) &appFlashMinor_;
	//for (unsigned int i = 0; i < sizeof(appFlashMinor_); i++)
	//{
	//	*pChar++ = buffer[j++];
	//}

	//pChar = (char *) &dataFlashMajor_;
	//for (unsigned int i = 0; i < sizeof(dataFlashMajor_); i++)
	//{
	//	*pChar++ = buffer[j++];
	//}
	//pChar = (char *) &dataFlashMinor_;
	//for (unsigned int i = 0; i < sizeof(dataFlashMinor_); i++)
	//{
	//	*pChar++ = buffer[j++];
	//}

	//pChar = (char *) &bootFlashMajor_;
	//for (unsigned int i = 0; i < sizeof(bootFlashMajor_); i++)
	//{
	//	*pChar++ = buffer[j++];
	//}
	//pChar = (char *) &bootFlashMinor_;
	//for (unsigned int i = 0; i < sizeof(bootFlashMinor_); i++)
	//{
	//	*pChar++ = buffer[j++];
	//}

	/*	Serialize the EEP93C56Contents structure	*/

	/*	EEP93C56Board	*/

	//pChar = (char *) &partNumber_;
	//char *pChar = (char *) &partNumber_;
	//for (unsigned int i = 0; i < PART_NUMBER_SIZE; i++)
	//{
	//	*pChar++ = buffer[j++];
	//}

	//pChar = (char *) &boardRev_;
	//for (unsigned int i = 0; i < REVISION_SIZE; i++)
	//{
	//	*pChar++ = buffer[j++];
	//}

	//pChar = (char *) &dateOfManufacture_;
	//for (unsigned int i = 0; i < DATE_SIZE; i++)
	//{
	//	*pChar++ = buffer[j++];
	//}

	//pChar = (char *) &serialNumber_;
	//for (unsigned int i = 0; i < SERIAL_NUMBER_SIZE; i++)
	//{
	//	*pChar++ = buffer[j++];
	//}

	//pChar = (char *) &boardSectionChecksum_;
	//for (unsigned int i = 0; i < sizeof(boardSectionChecksum_); i++)
	//{
	//	*pChar++ = buffer[j++];
	//}

	///*	EEP93C56AWS	*/

	//pChar = (char *) &awsSerialNumber_;
	//for (unsigned int i = 0; i < AWS_SERIAL_NUMBER_SIZE; i++)
	//{
	//	*pChar++ = buffer[j++];
	//}

	//pChar = (char *) &awsDateOfManufacture_;
	//for (unsigned int i = 0; i < DATE_SIZE; i++)
	//{
	//	*pChar++ = buffer[j++];
	//}

	//pChar = (char *) &awsDateOfInstall_;
	//for (unsigned int i = 0; i < DATE_SIZE; i++)
	//{
	//	*pChar++ = buffer[j++];
	//}

	//pChar = (char *) &modelNumber_;
	//for (unsigned int i = 0; i < sizeof(modelNumber_); i++)
	//{
	//	*pChar++ = buffer[j++];
	//}
	//
	//setAwsModelNumber(modelNumber_);
	//
	//pChar = (char *) &cycleCount_;
	//for (unsigned int i = 0; i < sizeof(cycleCount_); i++)
	//{
	//	*pChar++ = buffer[j++];
	//}

	//pChar = (char *) &awsSectionChecksum_;
	//for (unsigned int i = 0; i < sizeof(awsSectionChecksum_); i++)
	//{
	//	*pChar++ = buffer[j++];
	//}
	for (unsigned int i = 0; i < PART_NUMBER_SIZE; i++)
	{
		partNumber_.append(1, buffer[j++]);
	}

	for (unsigned int i = 0; i < REVISION_SIZE; i++)
	{
		boardRev_.append(1, buffer[j++]);
	}

	for (unsigned int i = 0; i < DATE_SIZE; i++)
	{
		dateOfManufacture_.append(1, buffer[j++]);
	}

	for (unsigned int i = 0; i < SERIAL_NUMBER_SIZE; i++)
	{
		serialNumber_.append(1, buffer[j++]);
	}

	char * pChar = (char *) &boardSectionChecksum_;
	for (unsigned int i = 0; i < sizeof(boardSectionChecksum_); i++)
	{
		*pChar++ = buffer[j++];
	}

	/*	EEP93C56AWS	*/

	for (unsigned int i = 0; i < AWS_SERIAL_NUMBER_SIZE; i++)
	{
		awsSerialNumber_.append(1, buffer[j++]);
	}

	for (unsigned int i = 0; i < DATE_SIZE; i++)
	{
		awsDateOfManufacture_.append(1, buffer[j++]);
	}

	for (unsigned int i = 0; i < DATE_SIZE; i++)
	{
		awsDateOfInstall_.append(1, buffer[j++]);
	}

	pChar = (char *) &modelNumber_;
	for (unsigned int i = 0; i < sizeof(modelNumber_); i++)
	{
		*pChar++ = buffer[j++];
	}

#if 0 //080321 CG removed. not applicable to CLA. PJR-175	
	setAwsModelNumber(modelNumber_);
#endif	

	//Might need to add something here for the pad0
	pChar = (char *) &cycleCount_;
	for (unsigned int i = 0; i < sizeof(cycleCount_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &awsSectionChecksum_;
	for (unsigned int i = 0; i < sizeof(awsSectionChecksum_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

CLASystemInfo::~CLASystemInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


#if 0 //080321 CG removed. not applicable to CLA. PJR-175
void CLASystemInfo::setAwsModelNumber(unsigned int model)
{
	AWSModel myModel = (AWSModel)model;
	switch(myModel)
	{
		case AWS1RL:
		{
			std::string thisModel = "AWS1RL";
			awsModelNumber_.replace(0,thisModel.size(), thisModel);
			break;
		}
		case AWS1LR:
		{
			std::string thisModel = "AWS1LR";
			awsModelNumber_.replace(0,thisModel.size(), thisModel);
			break;
		}
		case AWS1MLR:
		{
			std::string thisModel = "AWS1MLR";
			awsModelNumber_.replace(0,thisModel.size(), thisModel);
			break;
		}
		case AWS1LR_I:
		{
			std::string thisModel = "AWS1LR_I";
			awsModelNumber_.replace(0,thisModel.size(), thisModel);
			break;
		}
		case AWS1RL_I:
		{
			std::string thisModel = "AWS1RL_I";
			awsModelNumber_.replace(0,thisModel.size(), thisModel);
			break;
		}
		case AWS1BWRL:
		{
			std::string thisModel = "AWS1BWRL";
			awsModelNumber_.replace(0,thisModel.size(), thisModel);
			break;
		}
		case AWS1BWLR:
		{
			std::string thisModel = "AWS1BWLR";
			awsModelNumber_.replace(0,thisModel.size(), thisModel);
			break;
		}
		case AWS2RL:
		{
			std::string thisModel = "AWS2RL";
			awsModelNumber_.replace(0,thisModel.size(), thisModel);
			break;
		}
		case AWS2LR:
		{
			std::string thisModel = "AWS2LR";
			awsModelNumber_.replace(0,thisModel.size(), thisModel);
			break;
		}
		case NoModel:
		default:
		{
			break;
		}
	}
}
#endif


const char *CLASystemInfo::serialize()const
{
	EvolMessage::serialize();

	// Now, for the derived class items

	/*	CLASysInfo	*/

	//char *pChar = (char *) &configurationChecksum_;
	//for (unsigned int i = 0; i < sizeof(configurationChecksum_); i++)
	//{
	//	serializedData.append(1, pChar[i]);
	//}

	//pChar = (char *) &appFlashMajor_;
	//for (unsigned int i = 0; i < sizeof(appFlashMajor_); i++)
	//{
	//	serializedData.append(1, pChar[i]);
	//}
	//pChar = (char *) &appFlashMinor_;
	//for (unsigned int i = 0; i < sizeof(appFlashMinor_); i++)
	//{
	//	serializedData.append(1, pChar[i]);
	//}

	//pChar = (char *) &dataFlashMajor_;
	//for (unsigned int i = 0; i < sizeof(dataFlashMajor_); i++)
	//{
	//	serializedData.append(1, pChar[i]);
	//}
	//pChar = (char *) &dataFlashMinor_;
	//for (unsigned int i = 0; i < sizeof(dataFlashMinor_); i++)
	//{
	//	serializedData.append(1, pChar[i]);
	//}

	//pChar = (char *) &bootFlashMajor_;
	//for (unsigned int i = 0; i < sizeof(bootFlashMajor_); i++)
	//{
	//	serializedData.append(1, pChar[i]);
	//}
	//pChar = (char *) &bootFlashMinor_;
	//for (unsigned int i = 0; i < sizeof(bootFlashMinor_); i++)
	//{
	//	serializedData.append(1, pChar[i]);
	//}

	/*	Serialize the EEP93C56Contents structure	*/

	/*	EEP93C56Board	*/

	//pChar = (char *) &partNumber_;
//	char *pChar = (char *) &partNumber_;
//	for (unsigned int i = 0; i < sizeof(partNumber_); i++)
//	{
//		serializedData.append(1, pChar[i]);
//	}
//
//	pChar = (char *) &boardRev_;
//	for (unsigned int i = 0; i < sizeof(boardRev_); i++)
//	{
//		serializedData.append(1, pChar[i]);
//	}
//
//	pChar = (char *) &dateOfManufacture_;
//	for (unsigned int i = 0; i < sizeof(dateOfManufacture_); i++)
//	{
//		serializedData.append(1, pChar[i]);
//	}
//
//	pChar = (char *) &serialNumber_;
//	for (unsigned int i = 0; i < sizeof(serialNumber_); i++)
//	{
//		serializedData.append(1, pChar[i]);
//	}
//
//	pChar = (char *) &boardSectionChecksum_;
//	for (unsigned int i = 0; i < sizeof(boardSectionChecksum_); i++)
//	{
//		serializedData.append(1, pChar[i]);
//	}
//	
//	/*	EEP93C56AWS	*/
//
//	pChar = (char *) &awsSerialNumber_;
//	for (unsigned int i = 0; i < sizeof(awsSerialNumber_); i++)
//	{
//		serializedData.append(1, pChar[i]);
//	}
//	
//	pChar = (char *) &awsDateOfManufacture_;
//	for (unsigned int i = 0; i < sizeof(awsDateOfManufacture_); i++)
//	{
//		serializedData.append(1, pChar[i]);
//	}
//
//	pChar = (char *) &awsDateOfInstall_;
//	for (unsigned int i = 0; i < sizeof(awsDateOfInstall_); i++)
//	{
//		serializedData.append(1, pChar[i]);
//	}
//
//	pChar = (char *) &modelNumber_;
//	for (unsigned int i = 0; i < sizeof(modelNumber_); i++)
//	{
//		serializedData.append(1, pChar[i]);
//	}
//
//	pChar = (char *) &cycleCount_;
//	for (unsigned int i = 0; i < sizeof(cycleCount_); i++)
//	{
//		serializedData.append(1, pChar[i]);
//	}
//
//	pChar = (char *) &awsSectionChecksum_;
//	for (unsigned int i = 0; i < sizeof(awsSectionChecksum_); i++)
//	{
//		serializedData.append(1, pChar[i]);
//	}
//
	//Trying sumtin
	char *pChar = (char *) partNumber_.data();
	for (unsigned int i = 0; i < partNumber_.size(); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) boardRev_.data();
	for (unsigned int i = 0; i < boardRev_.size(); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) dateOfManufacture_.data();
	for (unsigned int i = 0; i < dateOfManufacture_.size(); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) serialNumber_.data();
	for (unsigned int i = 0; i < serialNumber_.size(); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &boardSectionChecksum_;
	for (unsigned int i = 0; i < sizeof(boardSectionChecksum_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	/*	EEP93C56AWS	*/

	pChar = (char *) awsSerialNumber_.data();
	for (unsigned int i = 0; i < awsSerialNumber_.size(); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	pChar = (char *) awsDateOfManufacture_.data();
	for (unsigned int i = 0; i < awsDateOfManufacture_.size(); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) awsDateOfInstall_.data();
	for (unsigned int i = 0; i < awsDateOfInstall_.size(); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &modelNumber_;
	for (unsigned int i = 0; i < sizeof(modelNumber_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &cycleCount_;
	for (unsigned int i = 0; i < sizeof(cycleCount_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &awsSectionChecksum_;
	for (unsigned int i = 0; i < sizeof(awsSectionChecksum_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}
void CLASystemInfo::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("CLASystemInfo") );
#else
	Debug::addTraceCategory("CLASystemInfo");
#endif
	
		///*	CLASysInfo	*/
		//ULONG					configurationChecksum_;
		//UINT16				appFlashMajor_;
		//UINT16				appFlashMinor_;
		//UINT16				dataFlashMajor_;
		//UINT16				dataFlashMinor_;
		//UINT16				bootFlashMajor_;
		//UINT16				bootFlashMinor_;

		///*	EEP93C56Board	*/
		//std::string			partNumber_;
		//std::string			boardRev_;
		//std::string			dateOfManufacture_;
		//std::string			serialNumber_;
		//std::string			boardSectionChecksum_;
		//
		///*	EEP93C56AWS	*/
		//std::string			awsSerialNumber_;
		//std::string			awsDateOfManufacture_;
		//std::string			awsDateOfInstall_;
		//std::string			awsModelNumber_;
		//AWSModel			modelNumber_;		
		//ULONG				cycleCount_;
		//std::string			awsSectionChecksum_;
	
	//Debug::trace("CLASystemInfo", "CLASystemInfo: ");

	///*	CLASysInfo	*/
	//Debug::trace("CLASystemInfo", HobString("configurationChecksum_: ") +
	//	HobString(Long::toString(configurationChecksum_)));

	//Debug::trace("CLASystemInfo", HobString("appFlashMajor_: ") +
	//	HobString(Integer::toString(appFlashMajor_)));
	//Debug::trace("CLASystemInfo", HobString("appFlashMinor_: ") +
	//	HobString(Integer::toString(appFlashMinor_)));

	//Debug::trace("CLASystemInfo", HobString("dataFlashMajor_: ") +
	//	HobString(Integer::toString(dataFlashMajor_)));
	//Debug::trace("CLASystemInfo", HobString("dataFlashMinor_: ") +
	//	HobString(Integer::toString(dataFlashMinor_)));

	//Debug::trace("CLASystemInfo", HobString("bootFlashMajor_: ") +
	//	HobString(Integer::toString(bootFlashMajor_)));
	//Debug::trace("CLASystemInfo", HobString("bootFlashMinor_: ") +
	//	HobString(Integer::toString(bootFlashMinor_)));
	
	/*	Output the EEP93C56Contents structure	*/

	/*	EEP93C56Board	*/
	Debug::trace("CLASystemInfo", HobString("partNumber_: ") +
									 HobString((const char *) &partNumber_[0], PART_NUMBER_SIZE));

	Debug::trace("CLASystemInfo", HobString("boardRev_: ") +
									 HobString((const char *) &boardRev_[0], REVISION_SIZE));

	Debug::trace("CLASystemInfo", HobString("dateOfManufacture_: ") +
									 HobString((const char *) &dateOfManufacture_[0], DATE_SIZE));

	Debug::trace("CLASystemInfo", HobString("serialNumber_: ") +
									 HobString((const char *) &serialNumber_[0], SERIAL_NUMBER_SIZE));

	//Alex Zavala 8/21
/*	Debug::trace("CLASystemInfo", HobString("boardSectionChecksum_: ") +
											  HobString(Bool::toString(boardSectionChecksum_)));
											  */
	Debug::trace("CLASystemInfo", HobString("boardSectionChecksum_: ") +
		HobString(Long::toString(boardSectionChecksum_)));

	/*	EEP93C56AWS	*/
	Debug::trace("CLASystemInfo", HobString("awsSerialNumber_: ") +
									 HobString((const char *) &awsSerialNumber_[0], AWS_SERIAL_NUMBER_SIZE));

	Debug::trace("CLASystemInfo", HobString("awsDateOfManufacture_: ") +
									 HobString((const char *) &awsDateOfManufacture_[0], DATE_SIZE));

	Debug::trace("CLASystemInfo", HobString("awsDateOfInstall_: ") +
									 HobString((const char *) &awsDateOfInstall_[0], DATE_SIZE));

	Debug::trace("CLASystemInfo", HobString("modelNumber_: ") +
									 HobString(Long::toString(modelNumber_)));

	Debug::trace("CLASystemInfo", HobString("cycleCount_: ") +
		HobString(Long::toString(cycleCount_)));

	//Alex Zavala 8/21
	/*Debug::trace("CLASystemInfo", HobString("awsSectionChecksum_: ") +
											  HobString(Bool::toString(awsSectionChecksum_)));
	*/
	Debug::trace("CLASystemInfo", HobString("awsSectionChecksum_: ") +
		HobString(Long::toString(awsSectionChecksum_)));

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("CLASystemInfo") );
#else
	Debug::removeTraceCategory("CLASystemInfo");
#endif
}

///* Setters and getters */
//
///*	EEP93C56Board	*/
//const UINT8 *CLASystemInfo::getPartNumber() const
//{
//	return (const UINT8 *) partNumber_;
//}
//
//void CLASystemInfo::setPartNumber(const UINT8 *partNumber)
//{
//	for (int i = 0; i < PART_NUMBER_SIZE; i++)
//	{
//		if (partNumber == NULL)
//		{
//			partNumber_[i] = '\0';
//		}
//		else
//		{
//			partNumber_[i] = partNumber[i];
//		}
//	}
//}
//
//const UINT8 *CLASystemInfo::getBoardRev() const
//{
//	return (const UINT8 *) boardRev_;
//}
//
//void CLASystemInfo::setBoardRev(const UINT8 *boardRev)
//{
//	for (int i = 0; i < REVISION_SIZE; i++)
//	{
//		if (boardRev == NULL)
//		{
//			boardRev_[i] = '\0';
//		}
//		else
//		{
//			boardRev_[i] = boardRev[i];
//		}
//	}
//}
//
//const UINT8 *CLASystemInfo::getDateOfManufacture() const
//{
//	return (const UINT8 *) dateOfManufacture_;
//}
//
//void CLASystemInfo::setDateOfManufacture(const UINT8 *date)
//{
//	for (int i = 0; i < DATE_SIZE; i++)
//	{
//		if (date == NULL)
//		{
//			dateOfManufacture_[i] = '\0';
//		}
//		else
//		{
//			dateOfManufacture_[i] = date[i];
//		}
//	}
//}
//
//const UINT8 *CLASystemInfo::getSerialNumber() const
//{
//	return (const UINT8 *) serialNumber_;
//}
//
//void CLASystemInfo::setSerialNumber(const UINT8 *serialNumber)
//{
//	for (int i = 0; i < SERIAL_NUMBER_SIZE; i++)
//	{
//		if (serialNumber == NULL)
//		{
//			serialNumber_[i] = '\0';
//		}
//		else
//		{
//			serialNumber_[i] = serialNumber[i];
//		}
//	}
//}
//
///*	EEP93C56AWS	*/
//
//const UINT8 *CLASystemInfo::getAwsSerialNumber() const
//{
//	return (const UINT8 *) awsSerialNumber_;
//}
//
//void CLASystemInfo::setAwsSerialNumber(const UINT8 *serialNumber)
//{
//	for (int i = 0; i < AWS_SERIAL_NUMBER_SIZE; i++)
//	{
//		if (serialNumber == NULL)
//		{
//			awsSerialNumber_[i] = '\0';
//		}
//		else
//		{
//			awsSerialNumber_[i] = serialNumber[i];
//		}
//	}
//}
//
//const UINT8 *CLASystemInfo::getAwsDateOfManufacture() const
//{
//	return (const UINT8 *) awsDateOfManufacture_;
//}
//
//void CLASystemInfo::setAwsDateOfManufacture(const UINT8 *date)
//{
//	for (int i = 0; i < DATE_SIZE; i++)
//	{
//		if (date == NULL)
//		{
//			awsDateOfManufacture_[i] = '\0';
//		}
//		else
//		{
//			awsDateOfManufacture_[i] = date[i];
//		}
//	}
//}
//
//const UINT8 *CLASystemInfo::getAwsDateOfInstall() const
//{
//	return (const UINT8 *) awsDateOfInstall_;
//}
//
//void CLASystemInfo::setAwsDateOfInstall(const UINT8 *date)
//{
//	for (int i = 0; i < DATE_SIZE; i++)
//	{
//		if (date == NULL)
//		{
//			awsDateOfInstall_[i] = '\0';
//		}
//		else
//		{
//			awsDateOfInstall_[i] = date[i];
//		}
//	}
//}
//
//void CLASystemInfo::initSysInfoArray(UINT8 *var)
//{
//	for(int i = 0; i < sizeof(var); i++)
//	{
//		var[i] = 0;
//	}
//}