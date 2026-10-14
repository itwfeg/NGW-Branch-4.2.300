#include "EvolWrapperSystemStatus.h"
#include "Hex.h"
#include "Debug.h"
#include "HobString.h"
#include "Integer.h"
#include "SystemEvent.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperSystemStatus::_dataType_ = 0;
std::string EvolWrapperSystemStatus::_dataTypeName_("EvolWrapperSystemStatus"); 
#endif

EvolWrapperSystemStatus::EvolWrapperSystemStatus(Hobart::CEvolMsgReceiver *pHobThread)
	: EvolMessage(WrapperSystem, SysUsbWrStatus, pHobThread), wrKind(WrEvolWrapper), reportSysError(0), node_(_maxBoards_)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

EvolWrapperSystemStatus::EvolWrapperSystemStatus(const char *buffer)
	: EvolMessage(buffer), wrKind(WrEvolWrapper), reportSysError(0), node_(_maxBoards_)
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
	char *pChar = (char *) &reportSysError;
	for (unsigned int i = 0; i < sizeof(reportSysError); i++)
	{
		*pChar++ = buffer[j++];
	}

	//	Read in all of the Nodes.
	for (unsigned int x = 0; x < _maxBoards_; ++x)
	{
		node_[x] = EvolWrapperNode(&buffer[j]);
		node_[x].serialize();
		j += node_[x].getSerializedLength();
	}
}


EvolWrapperSystemStatus::EvolWrapperSystemStatus(const EvolWrapperSystemStatus &ewss)
	: EvolMessage(WrapperSystem, SysUsbWrStatus), wrKind(WrEvolWrapper), reportSysError(0), node_(_maxBoards_)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	//Debug::trace("EvolWrapperSystemStatus", "EvolWrapperSystemStatus(const EvolWrapperSystemStatus &ewss) = 0x" + Hex::toString((long) this));

	reportSysError = (UINT16)ewss.getReportSysError();

	for (unsigned int x = 0; x < _maxBoards_; ++x)
	{
		setNode(const_cast<EvolWrapperSystemStatus&> (ewss).getNode(x), x);
	}
}


EvolWrapperSystemStatus::~EvolWrapperSystemStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *EvolWrapperSystemStatus::serialize()const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	const char *pCC = 0;
	int mLength = 0;

	char *pChar = (char *) &reportSysError;
	for (unsigned int i = 0; i < sizeof(reportSysError); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	for (unsigned int x = 0; x < _maxBoards_; ++x)
	{
		pCC = node_[x].serialize();
		mLength = node_[x].getSerializedLength();
		serializedData.append(pCC, mLength);
	}

	return serializedData.data();
}

void EvolWrapperSystemStatus::show() const
{
	bool isTracing = Debug::isTracing("EvolWrapperSystemStatus");

	if (isTracing == false)
		return;

	Debug::trace("EvolWrapperSystemStatus", "EvolWrapperSystemStatus: ");

	EvolWrapperNode node;
	for (unsigned int x = 0; x < _maxBoards_; ++x)
	{
		node_[x].show();
	}
}
