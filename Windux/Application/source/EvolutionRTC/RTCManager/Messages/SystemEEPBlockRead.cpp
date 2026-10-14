//#include "vxWorks.h"
#include "SystemEvent.h"
#include "SystemEEPBlockRead.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int SystemEEPBlockRead::_dataType_ = 0;
std::string SystemEEPBlockRead::_dataTypeName_("SystemEEPBlockRead"); 
#endif

SystemEEPBlockRead::SystemEEPBlockRead(EvolDeviceType evolDevType, Hobart::CEvolMsgReceiver *pHobThread, UINT16 eepBlockSize, const UINT8 *pBlock) : EvolMessage(evolDevType, SysEEPBlockRead, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	this->eepBlockSize = eepBlockSize;

	if ((eepBlockSize == 0) || (pBlock == NULL))
	{
		this->eepBlockSize = 0;
		pEEPBlock = NULL;
	}
	else
	{
		this->eepBlockSize = eepBlockSize;

		pEEPBlock = new UINT8[eepBlockSize];
		for (int i = 0; i < eepBlockSize; i++)
		{
			pEEPBlock[i] = (UINT8) pBlock[i];
		}
	}
}

SystemEEPBlockRead::SystemEEPBlockRead(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &eepBlockSize;
	for (unsigned int i = 0; i < sizeof(eepBlockSize); i++)
	{
		*pChar++ = buffer[j++];
	}

	if (eepBlockSize == 0)
	{
		pEEPBlock = NULL;
	}
	else
	{
		pEEPBlock = new UINT8[eepBlockSize];
		for (int i = 0; i < eepBlockSize; i++)
		{
			pEEPBlock[i] = (UINT8) buffer[j++];
		}
	}
}

SystemEEPBlockRead::~SystemEEPBlockRead()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
	if (pEEPBlock != NULL)
	{
		delete [] pEEPBlock;
	}
}
 
const char *SystemEEPBlockRead::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &eepBlockSize;
	for (unsigned int i = 0; i < sizeof(eepBlockSize); i++)
	{
		serializedData.append(1, pChar[i]);
	}

/*	if (eepBlockSize != 0)
	{
		pEEPBlock = new UINT8[eepBlockSize];
	}
sjjc- this is probably unnecessary and if not it should not be here*/

	for (int i = 0; i < eepBlockSize; i++)
	{
		serializedData.append(1, pEEPBlock[i]);
	}

	return serializedData.data();
}

void SystemEEPBlockRead::show()
{
	Debug::addTraceCategory( std::string("SystemEEPBlockRead") );
	Debug::trace("SystemEEPBlockRead", "SystemEEPBlockRead: ");

	EvolMessage::show();

	Debug::trace("SystemEEPBlockRead", HobString("eepBlockSize: ") + HobString(Integer::toString(eepBlockSize)));
	Debug::trace("SystemEEPBlockRead", Hex::toString((const char *) &pEEPBlock[0], eepBlockSize));

	Debug::removeTraceCategory( std::string("SystemEEPBlockRead") );
}

void SystemEEPBlockRead::setEEPBlock(UINT16 eepBlockSize, const UINT8 *pBlock)
{
	if ((eepBlockSize > 0) && (pBlock != NULL))
	{
		if (pEEPBlock != NULL)
		{
			delete [] pEEPBlock;
		}

		this->eepBlockSize = eepBlockSize;

		pEEPBlock = new UINT8[eepBlockSize];
		for (int i = 0; i < eepBlockSize; i++)
		{
			pEEPBlock[i] = (UINT8) pBlock[i];
		}
	}
}

const UINT8 *SystemEEPBlockRead::getEEPBlock()
{
	return (const UINT8 *) pEEPBlock;
}
