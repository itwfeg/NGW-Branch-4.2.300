#ifndef PERIPHERALCODEUPDATE_H
#define PERIPHERALCODEUPDATE_H

#include "PAL_platformheader.h"
#include "evolution.h"
#include "EvolMessage.h"
#include "SoftwareSystem.h"
#include "CEvolMsgReceiver.h"

class HobVersion;
class HobComplexVersion;
namespace util_filesystem
{
class Directory;
}

class PeripheralCodeUpdate : public EvolMessage
{
	public:
		PeripheralCodeUpdate( USBD_NODE_ID nodeId,
							 SoftwareSystem::PeripheralVersionCheck pResponseType,
							 const HobComplexVersion &softwareVer,
							 Hobart::CEvolMsgReceiver *pHobThread = NULL);
		PeripheralCodeUpdate(const char *buffer);
		~PeripheralCodeUpdate();

		const char *serialize() const;
		void show();

		HobVersion::VersionType getVersionType();
		util_filesystem::Directory * getPath();

		void setComplexVersion(const HobComplexVersion &softwareVer); 
		HobComplexVersion *getComplexVersion(){return &softwareVersion_;}

		SoftwareSystem::PeripheralVersionCheck getResponseType();
		void setResponseType(SoftwareSystem::PeripheralVersionCheck pResponseType);

		USBD_NODE_ID getNodeId(){return nodeId_;};
        // intentionally disabled, violates UsbManager contract		
        //void setNodeId(USBD_NODE_ID nodeId) {nodeId_ = nodeId;}
		
		void setCurrentSoftwareVersion(const HobVersion &currVersion);
		HobVersion& getCurrentSoftwareVersion();
	private:
		USBD_NODE_ID nodeId_;
		HobComplexVersion softwareVersion_;
		SoftwareSystem::PeripheralVersionCheck responseType_;
		util_filesystem::Directory sourcePath_;
		HobVersion currentSoftwareVersion_;	

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//PERIPHERALCODEUPDATE_H
