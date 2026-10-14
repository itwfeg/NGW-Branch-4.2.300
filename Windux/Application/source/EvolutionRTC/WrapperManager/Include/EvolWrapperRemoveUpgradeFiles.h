#ifndef EVOLWRAPPERREMOVEUPGRADEFILES_H
#define EVOLWRAPPERREMOVEUPGRADEFILES_H

#include "EvolMessage.h"
#include "HobString.h"

class HobThread;

class EvolWrapperRemoveUpgradeFiles : public EvolMessage
{
	public:

		typedef enum WrEvolRemoveUpgradeState
		{
			BeginRemoval,
			RemovalComplete,
			NoFilesToRemove
		} WrEvolRemoveUpgradeState_t;

		EvolWrapperRemoveUpgradeFiles(Hobart::CEvolMsgReceiver *pHobThread = NULL, WrEvolRemoveUpgradeState state = BeginRemoval);
		EvolWrapperRemoveUpgradeFiles(const char *buffer);
		virtual ~EvolWrapperRemoveUpgradeFiles();
		 
		const char *serialize()const;

		void show();	

		WrEvolRemoveUpgradeState getState();
		void setState(WrEvolRemoveUpgradeState state);

	private:
		WrEvolRemoveUpgradeState	state_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif //EVOLWRAPPERREMOVEUPGRADEFILES_H
