//#include "vxWorks.h"
#include "NumericStatus.h"

#include "NumericEvent.h"
#include "NumericWakeup.h"
#include "NumericInquire.h"

#include "Debug.h"
#include "Integer.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int NumericStatus::dataType = 0;
std::string NumericStatus::dataTypeName("NumericStatus"); 
#endif

int NumericStatus::COMM_CHANGED = 0x1;
int NumericStatus::VAL_CHANGED = 0x2;

NumericStatus::NumericStatus(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Numeric, NmStatus, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	disposition_ = NumericConfigValid;
	peripheralComm_ = false;
	managerDeleted_ = false;
	nmDisplayType_ = NumericInvalidDisplay;
}

NumericStatus::NumericStatus(const char *buffer) : EvolMessage(buffer)
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

	pChar = (char *) &nmDisplayType_;
	for (unsigned int i = 0; i < sizeof(nmDisplayType_); i++)
	{
		*pChar++ = buffer[j++];
	}


}

NumericStatus::~NumericStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *NumericStatus::serialize() const
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

	pChar = (char *) &nmDisplayType_;
	for (unsigned int i = 0; i < sizeof(nmDisplayType_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	return serializedData.data();
}

void NumericStatus::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("NumericStatus") );
#else
	Debug::addTraceCategory("NumericStatus");
#endif
	Debug::trace("NumericStatus", "NumericStatus: ");

	EvolMessage::show();

	Debug::trace("NumericStatus", HobString("peripheralComm: ") +
								  HobString(Bool::toString(peripheralComm_)));
	Debug::trace("NumericStatus", HobString("managerDeleted: ") +
								  HobString(Bool::toString(managerDeleted_)));

	switch (nmDisplayType_)
	{
	case NumericVacuumFlourescentDisplay:
		Debug::trace("NumericStatus", HobString("display type: vacuum flourescent") );
		break;
	case NumericGasPlasmaDisplay:
		Debug::trace("NumericStatus", HobString("display type: gas plasma") );
		break;
	case NumericInvalidDisplay:
		Debug::trace("NumericStatus", HobString("display type: invalid") );
		break;
	}



#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("NumericStatus") );
#else
	Debug::removeTraceCategory("NumericStatus");
#endif

}

NmCfgStatus NumericStatus::getDisposition()
{
	return disposition_;
}

void NumericStatus::setDisposition(NmCfgStatus value)
{
	disposition_ = value;
}

bool NumericStatus::isPeripheralComm() const
{
	return peripheralComm_;
}

void NumericStatus::setPeripheralComm(bool value)
{
	peripheralComm_ = value;
}

void NumericStatus::clearPeripheralComm()
{
	peripheralComm_ = false;
}

bool NumericStatus::isManagerDeleted() const
{
	return managerDeleted_;
}

void NumericStatus::setManagerDeleted(bool value)
{
	managerDeleted_ = value;
}

void NumericStatus::clearManagerDeleted()
{
	managerDeleted_ = false;
}
		
void NumericStatus::setDisplayType(NmDisplayType value)
{
	nmDisplayType_ = value;
}

NmDisplayType NumericStatus::getDisplayType()
{
	return nmDisplayType_;
}

int NumericStatus::updateStatus(NumericWakeup & receivedMsg)
{
	int changed = 0;

	if (!peripheralComm_)
	{
		peripheralComm_ = true;
		changed |= COMM_CHANGED;
	}

	if (disposition_ != receivedMsg.getNmCfgStatus())
	{
		changed |= VAL_CHANGED;

		disposition_ =	receivedMsg.getNmCfgStatus();

	}

	if (nmDisplayType_ != receivedMsg.getNmDisplayType())
	{
		changed |= VAL_CHANGED;

		nmDisplayType_ = receivedMsg.getNmDisplayType();
	}


	return changed;
}

int NumericStatus::updateStatus(NumericInquire & receivedMsg)
{
	int changed = 0;

	if (!peripheralComm_)
	{
		peripheralComm_ = true;
		changed |= COMM_CHANGED;
	}

	if (disposition_ !=	(NmCfgStatus)receivedMsg.getNmDisposition())
	{
		changed |= VAL_CHANGED;

		disposition_ =	(NmCfgStatus)receivedMsg.getNmDisposition();

	}

	return changed;
}
