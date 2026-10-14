#ifndef UPGRADELIST_H
#define UPGRADELIST_H

#include "EvolMessage.h"
#include "UpgradeVersions.h"

namespace Hobart
{
	class CEvolMsgReceiver;
}
class HobVersion;

class UpgradeList : public EvolMessage
{
	public:

		typedef std::list<UpgradeVersions> UpgradeVersionsContainer_type;

		UpgradeList(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		UpgradeList(const char *buffer);
		~UpgradeList();

		void appendUpgradeVersions(const HobVersion &curVer, const HobVersion *newVer);
		UpgradeVersionsContainer_type::size_type size();
		void clear();

		const char *serialize() const;
		void show();

	protected:
		mutable UpgradeVersionsContainer_type	versionData_;
//		UpgradeVersionsContainer_type::const_iterator *allUpgradeVersionsIter;


	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif	//UPGRADELIST_H
