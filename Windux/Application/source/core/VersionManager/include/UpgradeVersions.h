#ifndef UPGRADEVERSIONS_H
#define UPGRADEVERSIONS_H

#include "EvolMessage.h"
#include "HobVersion.h"
#include "HobComplexVersion.h"
#include "Debug.h"
#include "Integer.h"


class UpgradeVersions
{
	public:

		UpgradeVersions();
		UpgradeVersions(const HobVersion &curVer, const HobVersion *newVer);
		UpgradeVersions(const char *buffer);
		UpgradeVersions(const UpgradeVersions &uv);
		~UpgradeVersions();

		HobVersion& getCurrentVersion();
		const HobVersion& getCurrentVersion() const; 
		
		HobVersion& getNewVersion();
		const HobVersion& getNewVersion()const;

		const char *serialize() const;
		UINT32 getSerializedLength(){return (UINT32)serializedData_.length();}
		void show();

	private:
		HobVersion	currentVersion_;
		HobVersion	newVersion_;
		mutable std::string serializedData_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//UPGRADEVERSIONS_H
