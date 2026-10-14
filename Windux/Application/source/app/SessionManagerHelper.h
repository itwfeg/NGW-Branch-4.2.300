#ifndef SESSIONMANAGERHELPER_H
#define SESSIONMANAGERHELPER_H

#include <string>

class SessionManagerHelper
{
	public:
		void addDialog(int errorDevice, int errorCode, const std::string msgData = std::string() );
		void removeDialog(int errorDevice, int errorCode);
		void panicShutdown();
		bool hasPrimarySession();
		bool isWeigherAttached();
		bool isModeCorrectForApplyingTareSetback();
		static SessionManagerHelper *getInstance();

	private:
		SessionManagerHelper();
		~SessionManagerHelper();

		static SessionManagerHelper *instance;
};

#endif //SESSIONMANAGERHELPER_H
