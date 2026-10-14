#include "UpgradeList.h"
#include "HobVersion.h"
#include "VersionManagerEvent.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"


#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int UpgradeList::_dataType_ = 0;
std::string UpgradeList::_dataTypeName_("UpgradeList"); 
#endif


UpgradeList::UpgradeList(Hobart::CEvolMsgReceiver *pHobThread)
	: EvolMessage(VersionManagerDev, PeripheralUpgradeList, pHobThread), versionData_()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

}

UpgradeList::UpgradeList(const char *buffer) : EvolMessage(buffer), versionData_()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	UINT16 listCount = 0;

	// index into buffer
	EvolMessage::serialize();
	int j = getSerializedLength();

	//	Now, for the derived class items
	//	Read the number of items in the list.
	char *pChar = (char *) &listCount;
	for (unsigned int i = 0; i < sizeof(listCount); i++)
	{
		*pChar++ = buffer[j++];
	}

	for ( int i = 0; i < listCount; ++i )
	{
		HobVersion currentVer = HobVersion(&buffer[j]);
		currentVer.serialize();
		j += currentVer.getSerializedLength();

		HobVersion newVer = HobVersion(&buffer[j]);
		newVer.serialize();
		j += newVer.getSerializedLength();

		versionData_.push_back(UpgradeVersions(currentVer, &newVer));
	}
}

UpgradeList::~UpgradeList()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char *UpgradeList::serialize() const
{
	EvolMessage::serialize();

	UINT16 listCount = (UINT16)versionData_.size();

	const char *pChar = (char *) & listCount;
	for (unsigned int i = 0; i < sizeof(listCount); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	std::list<UpgradeVersions>::iterator fieldIter =  versionData_.begin();
	while (fieldIter != versionData_.end())
	{
		const char *tempData = fieldIter->serialize();
		serializedData.append(tempData, fieldIter->getSerializedLength());
		++fieldIter;
	}
	return serializedData.data();
}

void UpgradeList::show()
{
	if (Debug::isTracing("UpgradeList"))
	{
		Debug::trace("UpgradeList", "UpgradeList: ");

		EvolMessage::show();

		std::list<UpgradeVersions>::iterator fieldIter = versionData_.begin();
		while (fieldIter != versionData_.end())
		{
			fieldIter->show();
			++fieldIter;
		}
	}
}

void UpgradeList::appendUpgradeVersions(const HobVersion &curVer, const HobVersion *newVer)
{
	UpgradeVersions trackVers = UpgradeVersions(curVer, newVer);
	versionData_.push_back(trackVers);
}


std::list<UpgradeVersions>::size_type UpgradeList::size()
{
	return versionData_.size();
}

void UpgradeList::clear()
{
	versionData_.clear();
}
