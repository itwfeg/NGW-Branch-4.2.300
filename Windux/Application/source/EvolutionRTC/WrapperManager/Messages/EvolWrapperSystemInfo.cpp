#include "EvolWrapperSystemInfo.h"
#include "WrapperEvent.h"
#include "Debug.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperSystemInfo::_dataType_ = 0;
std::string EvolWrapperSystemInfo::_dataTypeName_("EvolWrapperSystemInfo"); 
#endif


EvolWrapperSystemInfo::EvolWrapperSystemInfo(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrEvolSystemInfo, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	boardChecksumStatus_  = false;
	awsChecksumStatus_ = false;
	
	motorBoardPartNumber_ ="";
	motorBoardRev_ = "";
	motorBoardDateOfManufacture_ = "";

	awsSerialNumber_ = "";
	awsDateOfManufacture_ = "";
	awsDateOfInstall_ = "";
	
	awsModelNumber_ = "NoModel";
	wrapperCycleCount_ = 0;
}

EvolWrapperSystemInfo::EvolWrapperSystemInfo(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &boardChecksumStatus_;
	for (unsigned int i = 0; i < sizeof(boardChecksumStatus_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &awsChecksumStatus_;
	for (unsigned int i = 0; i < sizeof(awsChecksumStatus_); i++)
	{
		*pChar++ = buffer[j++];
	}


	for (unsigned int i = 0; i < _PartNumbSize_; i++)
	{
		if ( buffer[j] )
			motorBoardPartNumber_.push_back( buffer[j] );

		++j;
	}
	
	for (unsigned int i = 0; i < _RevisionSize_; i++)
	{
		if ( buffer[j] )
			motorBoardRev_.push_back( buffer[j] );

		++j;
	}

	for (unsigned int i = 0; i < _DateSize_; i++)
	{
		if ( buffer[j] )
			motorBoardDateOfManufacture_.push_back( buffer[j] );

		++j;
	}

	for (unsigned int i = 0; i < _SerialNumbSize_; i++)
	{
		if ( buffer[j] )
			awsSerialNumber_.push_back( buffer[j] );

		++j;
	}
	
	for (unsigned int i = 0; i < _DateSize_; i++)
	{
		if ( buffer[j] )
			awsDateOfManufacture_.push_back( buffer[j] );

		++j;
	}

	for (unsigned int i = 0; i < _DateSize_; i++)
	{
		if ( buffer[j] )
			awsDateOfInstall_.push_back( buffer[j] );

		++j;
	}

	pChar = (char *) &modelNumber_;
	for (unsigned int i = 0; i < sizeof(modelNumber_); i++)
	{
		*pChar++ = buffer[j++];
	}
	
	setAwsModelNumber(modelNumber_);
	
	pChar = (char *) &wrapperCycleCount_;
	for (unsigned int i = 0; i < sizeof(wrapperCycleCount_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

EvolWrapperSystemInfo::~EvolWrapperSystemInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}



void EvolWrapperSystemInfo::setAwsModelNumber(unsigned int model)
{
	switch ( (EvolAWSModel) model )
	{
	case AWS1RL:
		awsModelNumber_ = "AWS1RL";
		break;

	case AWS1LR:
		awsModelNumber_ = "AWS1LR";
		break;

	case AWS1MLR:
		awsModelNumber_ = "AWS1MLR";
		break;

	case AWS1LR_I:
		awsModelNumber_ = "AWS1LR_I";
		break;

	case AWS1RL_I:
		awsModelNumber_ = "AWS1RL_I";
		break;

	case AWS1BWRL:
		awsModelNumber_ = "AWS1BWRL";
		break;

	case AWS1BWLR:
		awsModelNumber_ = "AWS1BWLR";
		break;

	case AWS2RL:
		awsModelNumber_ = "AWS-2RL";
		break;

	case AWS2LR:
		awsModelNumber_ = "AWS-2LR";
		break;

	case AWS3RL:
		awsModelNumber_ = "AWS-3RL";
		break;
	case AWS3LR:
		awsModelNumber_ = "AWS-3LR";
		break;
	case AWS3BRL:
		awsModelNumber_ = "EWS-3RL";
		break;
	case AWS3BLR:
		awsModelNumber_ = "EWS-3LR";
		break;

	case AWS4RL:
		awsModelNumber_ = "NGW2-RL";
		break;
	case AWS4LR:
		awsModelNumber_ = "NGW2-LR";
		break;
	case AWS4BRL:
		awsModelNumber_ = "NGW1-RL";
		break;
	case AWS4BLR:
		awsModelNumber_ = "NGW1-LR";
		break;

    // NGW Full CAN Wrapper models
	case NGW3BRL:
		awsModelNumber_ = "NGW2-I2RL";
		break;
	case NGW3BLR:
		awsModelNumber_ = "NGW2-I2LR";
		break;
	case NGW3RL:
		awsModelNumber_ = "NGW1-I2RL";
		break;
	case NGW3LR:
		awsModelNumber_ = "NGW1-I2LR";
		break;
	case NoModel:
	default:
		awsModelNumber_ = "Unknown";
		break;
	}
}



const char *EvolWrapperSystemInfo::serialize()const
{
	WrapperGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &boardChecksumStatus_;
	for (unsigned int i = 0; i < sizeof(boardChecksumStatus_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &awsChecksumStatus_;
	for (unsigned int i = 0; i < sizeof(awsChecksumStatus_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	for (unsigned int i = 0; i < _PartNumbSize_; i++)
	{
		serializedData.push_back( i < motorBoardPartNumber_.size() ? motorBoardPartNumber_[i] : '\0' );
	}

	for (unsigned int i = 0; i < _RevisionSize_; i++)
	{
		serializedData.push_back( i < motorBoardRev_.size() ? motorBoardRev_[i] : '\0' );
	}

	for (unsigned int i = 0; i < _DateSize_; i++)
	{
		serializedData.push_back( i < motorBoardDateOfManufacture_.size() ? motorBoardDateOfManufacture_[i] : '\0' );
	}

	for (unsigned int i = 0; i < _SerialNumbSize_; i++)
	{
		serializedData.push_back( i < awsSerialNumber_.size() ? awsSerialNumber_[i] : '\0' );
	}
	
	for (unsigned int i = 0; i < _DateSize_; i++)
	{
		serializedData.push_back( i < awsDateOfManufacture_.size() ? awsDateOfManufacture_[i] : '\0' );
	}

	for (unsigned int i = 0; i < _DateSize_; i++)
	{
		serializedData.push_back( i < awsDateOfInstall_.size() ? awsDateOfInstall_[i] : '\0' );
	}

	pChar = (char *) &modelNumber_;
	for (unsigned int i = 0; i < sizeof(modelNumber_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &wrapperCycleCount_;
	for (unsigned int i = 0; i < sizeof(wrapperCycleCount_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

int EvolWrapperSystemInfo::isLeftToRight()
{
	std::string modelNumber = getAwsModelNumber();
	Debug::trace("EvolWrapperSystemInfo", "Model Number: " + modelNumber);

	if (modelNumber.find("LR") == std::string::npos)
	{
		return 1;
	}
	else
	{
		return 0;
	}

	return -1;
}

void EvolWrapperSystemInfo::show()
{
	Debug::trace( "EvolWrapperSystemInfo", + "boardChecksumStatus_: " + Integer::toString( boardChecksumStatus_ ) );
	Debug::trace( "EvolWrapperSystemInfo", + "awsChecksumStatus_: " + Integer::toString( awsChecksumStatus_ ) );
	Debug::trace( "EvolWrapperSystemInfo", + "motorBoardPartNumber_: " + motorBoardPartNumber_ );
	Debug::trace( "EvolWrapperSystemInfo", + "motorBoardRev_: " + motorBoardRev_ );
	Debug::trace( "EvolWrapperSystemInfo", + "motorBoardDateOfManufacture_: " + motorBoardDateOfManufacture_ );
	Debug::trace( "EvolWrapperSystemInfo", + "awsSerialNumber_: " + awsSerialNumber_ );
	Debug::trace( "EvolWrapperSystemInfo", + "awsDateOfManufacture_: " + awsDateOfManufacture_ );
	Debug::trace( "EvolWrapperSystemInfo", + "awsDateOfInstall_: " + awsDateOfInstall_ );
	Debug::trace( "EvolWrapperSystemInfo", + "modelNumber_: " + Integer::toString( modelNumber_ ) );
	Debug::trace( "EvolWrapperSystemInfo", + "wrapperCycleCount_: " + Integer::toString( wrapperCycleCount_ ) );

}
