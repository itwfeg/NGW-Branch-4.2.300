/*	Created by Alex Zavala 10/3
*	based off wrapper
*/
#ifndef CLAREMOVEUPGRADEFILES_H
#define CLAREMOVEUPGRADEFILES_H

#include "EvolMessage.h"
#include "HobString.h"

class HobThread;

class CLARemoveUpgradeFiles : public EvolMessage
{
	public:

		typedef enum CLARemoveUpgradeState
		{
			BeginRemoval,
			RemovalComplete,
			NoFilesToRemove
		} CLARemoveUpgradeState_t;

		CLARemoveUpgradeFiles(Hobart::CEvolMsgReceiver *pHobThread = NULL, CLARemoveUpgradeState state = BeginRemoval);
		CLARemoveUpgradeFiles(const char *buffer);
		virtual ~CLARemoveUpgradeFiles();
		 
		const char *serialize()const;

		void show();	

		CLARemoveUpgradeState getState();
		void setState(CLARemoveUpgradeState state);

	private:
		CLARemoveUpgradeState	state_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif //CLAREMOVEUPGRADEFILES_H
