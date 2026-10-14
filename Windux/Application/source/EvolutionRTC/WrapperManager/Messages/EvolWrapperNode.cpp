#include "EvolWrapperNode.h"

#include "Debug.h"
#include "Integer.h"
#include "SystemEvent.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperNode::_dataType_ = 0;
std::string EvolWrapperNode::_dataTypeName_("EvolWrapperNode"); 
#endif

EvolWrapperNode::EvolWrapperNode()
	: appFlash_(), dataFlash_(), bootFlash_(), nodeId_(0), configVersion_(0), deviceInitFlags_(0), errorFlags_(0), wrapBoxChecksum_(0)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


EvolWrapperNode::EvolWrapperNode(const char *buffer)
	: appFlash_(), dataFlash_(), bootFlash_(), nodeId_(0), configVersion_(0), deviceInitFlags_(0), errorFlags_(0), wrapBoxChecksum_(0)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	// Now, for the derived class items
	int j = 0;
	char *pChar = (char *) &appFlash_;
	for (unsigned int i = 0; i < sizeof(appFlash_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &dataFlash_;
	for (unsigned int i = 0; i < sizeof(dataFlash_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &bootFlash_;
	for (unsigned int i = 0; i < sizeof(bootFlash_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &nodeId_;
	for (unsigned int i = 0; i < sizeof(nodeId_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &configVersion_;
	for (unsigned int i = 0; i < sizeof(configVersion_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &deviceInitFlags_;
	for (unsigned int i = 0; i < sizeof(deviceInitFlags_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &errorFlags_;
	for (unsigned int i = 0; i < sizeof(errorFlags_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &wrapBoxChecksum_;
	for (unsigned int i = 0; i < sizeof(wrapBoxChecksum_); i++)
	{
		*pChar++ = buffer[j++];
	}
}


EvolWrapperNode::EvolWrapperNode(const EvolWrapperVersions& appFlash, const EvolWrapperVersions& dataFlash, const EvolWrapperVersions& bootFlash,
									UINT16 nodeId, UINT16 configVersion, UINT16 deviceInitFlags, UINT32 errorFlags, UINT32 wrapBoxChecksum)
	: nodeId_(nodeId), configVersion_(configVersion), deviceInitFlags_(deviceInitFlags), errorFlags_(errorFlags), wrapBoxChecksum_(wrapBoxChecksum)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	appFlash_ = appFlash;
	dataFlash_ = dataFlash;
	bootFlash_ = bootFlash;
}

EvolWrapperNode::EvolWrapperNode(const EvolWrapperNode& node)
	: appFlash_(node.appFlash_), dataFlash_(node.dataFlash_), bootFlash_(node.bootFlash_),
		nodeId_(node.nodeId_), configVersion_(node.configVersion_), deviceInitFlags_(node.deviceInitFlags_), errorFlags_(node.errorFlags_), wrapBoxChecksum_(node.wrapBoxChecksum_)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

EvolWrapperNode::EvolWrapperNode(WrSysStatusInfo& node)
	: appFlash_(node.nodeVersion.appFlash), dataFlash_(node.nodeVersion.dataFlash), bootFlash_(node.nodeVersion.bootFlash),
		nodeId_(node.nodeid), configVersion_(node.config_version), deviceInitFlags_(node.device_init_flags), errorFlags_(node.error_flags), wrapBoxChecksum_(node.wrap_box_checksum)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

EvolWrapperNode::EvolWrapperNode(WrSysStatusInfo_Old& node)
	: appFlash_(node.nodeVersion.appFlash), dataFlash_(node.nodeVersion.dataFlash), bootFlash_(node.nodeVersion.bootFlash),
		nodeId_(node.nodeid), configVersion_(node.bit_error), deviceInitFlags_(node.device_init_flags), errorFlags_(node.error_flags), wrapBoxChecksum_(0)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

EvolWrapperNode::~EvolWrapperNode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


void EvolWrapperNode::operator=(const EvolWrapperNode& rhs)
{
	nodeId_ = rhs.getNodeId();
	configVersion_ = rhs.getConfigVersion();
	deviceInitFlags_ = rhs.getDeviceInitFlags();
	appFlash_ = rhs.getAppFlash();
	dataFlash_ = rhs.getDataFlash();
	bootFlash_ = rhs.getBootFlash();
	errorFlags_ = rhs.getErrorFlags();
	wrapBoxChecksum_ = rhs.getWrapBoxChecksum();
}


const char *EvolWrapperNode::serialize()const
{
	serializedData = "";

	// Now, for the derived class items
	char *pChar = (char *) &appFlash_;
	for (unsigned int i = 0; i < sizeof(appFlash_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &dataFlash_;
	for (unsigned int i = 0; i < sizeof(dataFlash_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &bootFlash_;
	for (unsigned int i = 0; i < sizeof(bootFlash_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &nodeId_;
	for (unsigned int i = 0; i < sizeof(nodeId_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &configVersion_;
	for (unsigned int i = 0; i < sizeof(configVersion_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &deviceInitFlags_;
	for (unsigned int i = 0; i < sizeof(deviceInitFlags_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &errorFlags_;
	for (unsigned int i = 0; i < sizeof(errorFlags_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &wrapBoxChecksum_;
	for (unsigned int i = 0; i < sizeof(wrapBoxChecksum_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}


UINT32 EvolWrapperNode::getSerializedLength()const
{
	return (UINT32)serializedData.length();
}


void EvolWrapperNode::show() const
{
	bool isTracing = Debug::isTracing("EvolWrapperNode");

	if (isTracing == false)
		return;

	HobString revision;

	Debug::trace("EvolWrapperNode", "******************************************");
	/*needs to be fixed -- ats
	Debug::trace("EvolWrapperNode", "           nodeId_: " + Integer::toString(nodeId_));
	Debug::trace("EvolWrapperNode", "         bitError_: " + Integer::toString(bitError_));
	Debug::trace("EvolWrapperNode", "  deviceInitFlags_: " + Integer::toString(deviceInitFlags_));
	*/
	revision = Integer::toString(appFlash_.majorRevision_);
	revision.append(".").append(Integer::toString(appFlash_.minorRevision_));
	Debug::trace("EvolWrapperNode", "         appFlash_: " + revision);

	revision = Integer::toString(dataFlash_.majorRevision_);
	revision.append(".").append(Integer::toString(dataFlash_.minorRevision_));
	Debug::trace("EvolWrapperNode", "        dataFlash_: " + revision);

	revision = Integer::toString(bootFlash_.majorRevision_);
	revision.append(".").append(Integer::toString(bootFlash_.minorRevision_));
	Debug::trace("EvolWrapperNode", "        bootFlash_: " + revision);
/*needs to be fixed -- ats
	Debug::trace("EvolWrapperNode", "       errorFlags_: " + Integer::toString(errorFlags_));
*/
	Debug::trace("EvolWrapperNode", "******************************************");
}
