/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef IMPORTMANAGER_H
#define IMPORTMANAGER_H

#include "HobString.h"
#include "CEvolMsgReceiver.h"
class GraphicManager;
namespace util_filesystem
{
class Directory;
}
class SessionMessage;
class Session;

class ImportManager : public Util::CRunnableClass, Hobart::CEvolMsgReceiver
{
	public:

		ImportManager(Session *session, SessionMessage *pMsg, bool popDialogs = true);
		ImportManager(const std::string & path, bool popDialogs = true);

		virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );

		virtual ~ImportManager();

		DWORD Run();

		void handleMessage(SessionMessage *msg);

		void init();


	private:
	
		void handleImportAllGraphics();

		GraphicManager * graphicManager_;
		Session* pMySession_;
		Util::CSmartPtr<Util::CTLSThread>	checkingThread;

		HobString directoryPath_;

		bool popDialogs_;
		bool moveFile_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};
#endif //IMPORTMANAGER_H
