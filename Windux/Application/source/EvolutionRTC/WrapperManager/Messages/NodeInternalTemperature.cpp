//#include "vxWorks.h"
#include "NodeInternalTemperature.h"

#include "Debug.h"
#include "UInteger.h"
#include "Hex.h"
#include "Bool.h"
#include "HobString.h"

using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int NodeInternalTemperature::_dataType_ = 0;
std::string NodeInternalTemperature::_dataTypeName_("NodeInternalTemperature"); 
#endif

NodeInternalTemperature::NodeInternalTemperature(UINT16 nodeId,	UINT16 temp, UINT16 statusBits) : nodeId(nodeId),
																								  temp(temp),
																								  statusBits(statusBits),
																								  serializedData("")
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	serialize();
}

NodeInternalTemperature::NodeInternalTemperature(const char *buffer) : serializedData("")
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	int j = 0;

	char *pChar = (char *) &nodeId;
	for (unsigned int i = 0; i < sizeof(nodeId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &temp;
	for (unsigned int i = 0; i < sizeof(temp); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &statusBits;
	for (unsigned int i = 0; i < sizeof(statusBits); i++)
	{
		*pChar++ = buffer[j++];
	}

	serialize();
}

NodeInternalTemperature::NodeInternalTemperature(const NodeInternalTemperature &nit) : nodeId(0), temp(0), statusBits(0)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("NodeInternalTemperature.Memory", "Copy constructor:  this = " + HobString(Hex::toString((long long) this)));

	nodeId = nit.getNodeId();
	temp = nit.getTemp();
	statusBits = nit.getStatusBits();
}

NodeInternalTemperature::~NodeInternalTemperature()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *NodeInternalTemperature::serialize() const
{
	serializedData = "";

	char *pChar = (char *) &nodeId;
	for (unsigned int i = 0; i < sizeof(nodeId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &temp;
	for (unsigned int i = 0; i < sizeof(temp); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &statusBits;
	for (unsigned int i = 0; i < sizeof(statusBits); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

UINT32 NodeInternalTemperature::getSerializedLength() const
{
	return (UINT32)serializedData.length();
}

void NodeInternalTemperature::show() const
{
	bool isTracing = Debug::isTracing("NodeInternalTemperature");

	if (isTracing == false)
	{
//		Debug::addTraceCategory("NodeInternalTemperature");
	}

	Debug::trace("NodeInternalTemperature", "NodeInternalTemperature: ");

	Debug::trace("NodeInternalTemperature", HobString("nodeId: ") + HobString(UInteger::toString(nodeId)));
	Debug::trace("NodeInternalTemperature", HobString("temp: ") + HobString(UInteger::toString(temp)));
	Debug::trace("NodeInternalTemperature", HobString("statusBits: ") + HobString(UInteger::toString(statusBits)));

	if (isTracing == false)
	{
		Debug::removeTraceCategory("NodeInternalTemperature");
	}
}

UINT16 NodeInternalTemperature::getNodeId() const
{
	return nodeId;
}

void NodeInternalTemperature::setNodeId(UINT16 nodeId)
{
	this->nodeId = nodeId;
}

UINT16 NodeInternalTemperature::getTemp() const
{
	return temp;
}

void NodeInternalTemperature::setTemp(UINT16 temp)
{
	this->temp = temp;
}

UINT16 NodeInternalTemperature::getStatusBits() const
{
	return statusBits;
}

void NodeInternalTemperature::setStatusBits(UINT16 statusBits)
{
	this->statusBits = statusBits;
}

void NodeInternalTemperature::operator=(const NodeInternalTemperature &nit)
{
//	Debug::trace("NodeInternalTemperature", "operator=(&):  this = " + HobString(Hex::toString((long) this)));

	nodeId = nit.getNodeId();
	temp = nit.getTemp();
	statusBits = nit.getStatusBits();
}

void NodeInternalTemperature::operator=(const NodeInternalTemperature *nit)
{
//	Debug::trace("NodeInternalTemperature", "operator=(*):  this = " + HobString(Hex::toString((long) this)));

	nodeId = nit->getNodeId();
	temp = nit->getTemp();
	statusBits = nit->getStatusBits();
}

bool NodeInternalTemperature::operator==(const NodeInternalTemperature &nit) const
{
//	Debug::trace("NodeInternalTemperature", "operator==(&):  this = " + HobString(Hex::toString((long) this)));

	bool rv = false;
	if ((getNodeId() == nit.getNodeId()) && (getTemp() == nit.getTemp()) && (getStatusBits() == nit.getStatusBits()))
		rv = true;
	return rv;
}

bool NodeInternalTemperature::operator!=(const NodeInternalTemperature &nit) const
{
//	Debug::trace("NodeInternalTemperature", "operator==(&):  this = " + HobString(Hex::toString((long) this)));

	bool rv = false;
	if ((getNodeId() != nit.getNodeId()) || (getTemp() != nit.getTemp()) || (getStatusBits() != nit.getStatusBits()))
		rv = true;
	return rv;
}
