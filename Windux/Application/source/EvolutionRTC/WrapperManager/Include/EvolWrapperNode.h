
#ifndef EVOLWRAPPERNODE_H
#define EVOLWRAPPERNODE_H

//#include <string>
#include "EvolWrapperMessages.h"
#include "HobString.h"

class EvolWrapperNode
{
	public:

		struct EvolWrapperVersions
		{
			UINT16 majorRevision_;
			UINT16 minorRevision_;

			EvolWrapperVersions(UINT16 major = 0, UINT16 minor = 0)
					: majorRevision_(major), minorRevision_(minor) {}
			EvolWrapperVersions(const EvolWrapperVersions& version)
					: majorRevision_(version.majorRevision_), minorRevision_(version.minorRevision_) {}
			EvolWrapperVersions(const ComponentRevision& version)
					: majorRevision_(version.major), minorRevision_(version.minor) {}
			~EvolWrapperVersions() {}

			UINT16 getMajorRevision() const { return majorRevision_; }
			void setMajorRevision(UINT16 value) { majorRevision_ = value; }

			UINT16 getMinorRevision() const { return minorRevision_; }
			void setMinorRevision(UINT16 value) { minorRevision_ = value; }
		};

		EvolWrapperNode();
		EvolWrapperNode(const char *buffer);

		EvolWrapperNode(const EvolWrapperVersions& appFlash, const EvolWrapperVersions& dataFlash, const EvolWrapperVersions& bootFlash,
			UINT16 nodeId = 0, UINT16 configVersion = 0, UINT16 deviceInitFlags = 0, UINT32 errorFlags = 0, UINT32 wrapBoxChecksum = 0);

		EvolWrapperNode(const EvolWrapperNode& node);

		EvolWrapperNode(WrSysStatusInfo& node);
		EvolWrapperNode(WrSysStatusInfo_Old& node);

		~EvolWrapperNode();

		void operator=(const EvolWrapperNode& rhs);

		const char *serialize() const;

		UINT32 getSerializedLength()const;

		void show() const;

		UINT16 getNodeId() const { return nodeId_; }
		void setNodeId(UINT16 value) { nodeId_ = value; }

		UINT16 getConfigVersion() const { return configVersion_; }
		void setConfigVersion(UINT16 value) { configVersion_ = value; }

		UINT16 getDeviceInitFlags() const { return deviceInitFlags_; }
		void setDeviceInitFlags(UINT16 value) { deviceInitFlags_ = value; }

		EvolWrapperVersions getAppFlash() const { return appFlash_; }
		void setAppFlash(EvolWrapperVersions& version) { appFlash_ = version; }
		void setAppFlash(UINT16 major = 0, UINT16 minor = 0) { appFlash_.majorRevision_ = major; appFlash_.minorRevision_ = minor; }

		EvolWrapperVersions getDataFlash() const { return dataFlash_; }
		void setDataFlash(EvolWrapperVersions& version) { dataFlash_ = version; }
		void setDataFlash(UINT16 major = 0, UINT16 minor = 0) { dataFlash_.majorRevision_ = major; dataFlash_.minorRevision_ = minor; }

		EvolWrapperVersions getBootFlash() const { return bootFlash_; }
		void setBootFlash(EvolWrapperVersions& version) { bootFlash_ = version; }
		void setBootFlash(UINT16 major = 0, UINT16 minor = 0) { bootFlash_.majorRevision_ = major; bootFlash_.minorRevision_ = minor; }

		UINT32 getErrorFlags() const { return errorFlags_; }
		void setErrorFlags(UINT32 value) { errorFlags_ = value; }

		UINT32 getWrapBoxChecksum() const { return wrapBoxChecksum_; }
		void setWrapBoxChecksum(UINT32 value) { wrapBoxChecksum_ = value; }

	private:
		EvolWrapperVersions	appFlash_;
		EvolWrapperVersions	dataFlash_;
		EvolWrapperVersions	bootFlash_;
		UINT16		nodeId_;
		UINT16		configVersion_;			//	Was bitError_
		UINT16		deviceInitFlags_;
		UINT32		errorFlags_;	
		UINT32		wrapBoxChecksum_;		//	New

		mutable std::string serializedData;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif //EVOLWRAPPERNODE_H
