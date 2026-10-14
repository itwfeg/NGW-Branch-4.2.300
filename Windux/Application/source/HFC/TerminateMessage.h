/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef TERMINATEMESSAGE_H
#define TERMINATEMESSAGE_H

#include "EvolMessage.h"

//class HobThread;

class TerminateMessage : public EvolMessage {
	public:
		TerminateMessage(Hobart::CEvolMsgReceiver *pHobThread = NULL, bool notify = false);
		TerminateMessage(const char *buffer);
		~TerminateMessage();
		 
		const char *serialize() const;

		void show();	

		// This is set when the task (Session) should notify its UI when it is terminating.
		bool isnotifyUI() const;
		void setnotifyUI(bool notify);

	private:
		bool notifyUI;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //TERMINATEMESSAGE_H
