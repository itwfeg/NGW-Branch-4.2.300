//#include "vxWorks.h"
#include "MarqueeStatus.h"

#include "MarqueeEvent.h"
#include "MarqueeWakeup.h"
#include "MarqueeInquire.h"

#include "Debug.h"
#include "Integer.h"
#include "Bool.h"
#include "HobString.h"

int MarqueeStatus::COMM_CHANGED = 0x1;
int MarqueeStatus::VAL_CHANGED = 0x2;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int MarqueeStatus::dataType = 0;
std::string MarqueeStatus::dataTypeName("MarqueeStatus"); 
#endif

MarqueeStatus::MarqueeStatus(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Marquee, MqStatus, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	disposition_ = MarqueeConfigValid;
	peripheralComm_ = false;
	managerDeleted_ = false;
	mqDisplayType_ = MarqueeInvalidDisplay;
}

MarqueeStatus::MarqueeStatus(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	char * pChar = (char *) &disposition_;
	for (unsigned int i = 0; i < sizeof(disposition_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &peripheralComm_;
	for (unsigned int i = 0; i < sizeof(peripheralComm_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &managerDeleted_;
	for (unsigned int i = 0; i < sizeof(managerDeleted_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &mqDisplayType_;
	for (unsigned int i = 0; i < sizeof(mqDisplayType_); i++)
	{
		*pChar++ = buffer[j++];
	}


}

MarqueeStatus::~MarqueeStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *MarqueeStatus::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items

	char * pChar = (char *) &disposition_;
	for (unsigned int i = 0; i < sizeof(disposition_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &peripheralComm_;
	for (unsigned int i = 0; i < sizeof(peripheralComm_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &managerDeleted_;
	for (unsigned int i = 0; i < sizeof(managerDeleted_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &mqDisplayType_;
	for (unsigned int i = 0; i < sizeof(mqDisplayType_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	return serializedData.data();
}

void MarqueeStatus::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("MarqueeStatus") );
#else
	Debug::addTraceCategory("MarqueeStatus");
#endif
	Debug::trace("MarqueeStatus", "MarqueeStatus: ");

	EvolMessage::show();

	Debug::trace("MarqueeStatus", HobString("peripheralComm: ") +
								  HobString(Bool::toString(peripheralComm_)));
	Debug::trace("MarqueeStatus", HobString("managerDeleted: ") +
								  HobString(Bool::toString(managerDeleted_)));

	switch (mqDisplayType_)
	{
	case MarqueeVacuumFlourescentDisplay:
		Debug::trace("MarqueeStatus", HobString("display type: vacuum flourescent") );
		break;
	case MarqueeGasPlasmaDisplay:
		Debug::trace("MarqueeStatus", HobString("display type: gas plasma") );
		break;
	case MarqueeInvalidDisplay:
		Debug::trace("MarqueeStatus", HobString("display type: invalid") );
		break;
	}

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("MarqueeStatus") );
#else
	Debug::removeTraceCategory("MarqueeStatus");
#endif

}

MqCfgStatus MarqueeStatus::getDisposition()
{
	return disposition_;
}

void MarqueeStatus::setDisposition(MqCfgStatus value)
{
	disposition_ = value;
}

bool MarqueeStatus::isPeripheralComm() const
{
	return peripheralComm_;
}

void MarqueeStatus::setPeripheralComm(bool value)
{
	peripheralComm_ = value;
}

void MarqueeStatus::clearPeripheralComm()
{
	peripheralComm_ = false;
}

bool MarqueeStatus::isManagerDeleted() const
{
	return managerDeleted_;
}

void MarqueeStatus::setManagerDeleted(bool value)
{
	managerDeleted_ = value;
}

void MarqueeStatus::clearManagerDeleted()
{
	managerDeleted_ = false;
}
		
void MarqueeStatus::setDisplayType(MqDisplayType value)
{
	mqDisplayType_ = value;
}

MqDisplayType MarqueeStatus::getDisplayType()
{
	return mqDisplayType_;
}

int MarqueeStatus::updateStatus(MarqueeWakeup & receivedMsg)
{
	int changed = 0;

	if (!peripheralComm_)
	{
		peripheralComm_ = true;
		changed |= COMM_CHANGED;
	}

	if (disposition_ != receivedMsg.getMqCfgStatus())
	{
		changed |= VAL_CHANGED;

		disposition_ =	receivedMsg.getMqCfgStatus();

	}

	if (mqDisplayType_ != receivedMsg.getMqDisplayType())
	{
		changed |= VAL_CHANGED;

		mqDisplayType_ = receivedMsg.getMqDisplayType();
	}


	return changed;
}

int MarqueeStatus::updateStatus(MarqueeInquire & receivedMsg)
{
	int changed = 0;

	if (!peripheralComm_)
	{
		peripheralComm_ = true;
		changed |= COMM_CHANGED;
	}

	if (disposition_ !=	(MqCfgStatus)receivedMsg.getMqDisposition())
	{
		changed |= VAL_CHANGED;

		disposition_ =	(MqCfgStatus)receivedMsg.getMqDisposition();

	}

	return changed;
}
