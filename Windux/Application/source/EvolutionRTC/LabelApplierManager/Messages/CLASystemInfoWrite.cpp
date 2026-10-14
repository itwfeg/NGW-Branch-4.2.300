#include "CLASystemInfoWrite.h"
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

int CLASystemInfoWrite::dataType = 0;
std::string CLASystemInfoWrite::dataTypeName("CLASystemInfoWrite"); 
#endif


CLASystemInfoWrite::CLASystemInfoWrite(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(LabelApplier, LaSysInfoWrite, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	awsSerialNumber_ = "";
	awsDateOfManufacture_ = "";
	awsDateOfInstall_ = "";
	awsModelNumber_ = "NoModel";
	cycleCount_ = 0;
	awsSectionChecksum_ = 0;
}

CLASystemInfoWrite::CLASystemInfoWrite(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();
	int j = getSerializedLength();

	// Now, for the derived class items

	/*	EEP93C56AWS	*/

	//Skip 2 bytes since we can ignore the function code and pad;

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

	char *pChar = (char *) &modelNumber_;
	for (unsigned int i = 0; i < sizeof(modelNumber_); i++)
	{
		*pChar++ = buffer[j++];
	}
	
	//CLASystemInfo::setModelNumber(modelNumber_);
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

CLASystemInfoWrite::~CLASystemInfoWrite()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


const char *CLASystemInfoWrite::serialize()const
{
	EvolMessage::serialize();

	// Now, for the derived class items

	/*	EEP93C56AWS	*/

	char *pChar = (char *) awsSerialNumber_.data();
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
void CLASystemInfoWrite::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("CLASystemInfoWrite") );
#else
	Debug::addTraceCategory("CLASystemInfoWrite");
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
	

	/*	EEP93C56AWS	*/
	Debug::trace("CLASystemInfoWrite", HobString("awsSerialNumber_: ") +
									 HobString((const char *) &awsSerialNumber_[0], AWS_SERIAL_NUMBER_SIZE));

	Debug::trace("CLASystemInfoWrite", HobString("awsDateOfManufacture_: ") +
									 HobString((const char *) &awsDateOfManufacture_[0], DATE_SIZE));

	Debug::trace("CLASystemInfoWrite", HobString("awsDateOfInstall_: ") +
									 HobString((const char *) &awsDateOfInstall_[0], DATE_SIZE));

	Debug::trace("CLASystemInfoWrite", HobString("awsModelNumber_: ") +
									 HobString(awsModelNumber_));

	Debug::trace("CLASystemInfoWrite", HobString("cycleCount_: ") +
		HobString(Long::toString(cycleCount_)));

	//Alex Zavala 8/21
	/*Debug::trace("CLASystemInfoWrite", HobString("awsSectionChecksum_: ") +
											  HobString(Bool::toString(awsSectionChecksum_)));
	*/
	Debug::trace("CLASystemInfoWrite", HobString("awsSectionChecksum_: ") +
		HobString(Long::toString(awsSectionChecksum_)));

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("CLASystemInfoWrite") );
#else
	Debug::removeTraceCategory("CLASystemInfoWrite");
#endif
}
