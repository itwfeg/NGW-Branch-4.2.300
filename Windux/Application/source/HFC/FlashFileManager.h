/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef FLASHFILEMANAGER_H
#define FLASHFILEMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "DataTypeEvent.h"
#include "HobDateTime.h"
#include "FlashFile.h"

class SessionMessage;
class DBSession;

namespace util_filesystem
{
	class Filename;
}

#include "DBFlashFilesRM.h"
#include "DBFlashFile.h"
#include "DBQuery.h"

namespace hfc
{
	class HFCManagerContainer;
}


class FlashFileManager : public DataManager
{
	public:

		FlashFileManager();
		FlashFileManager(hfc::HFCManagerContainer *container);
		virtual ~FlashFileManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		FlashFile* getFirstRecord() { return firstFlashFile(); }
		FlashFile* getNextRecord()  { return nextFlashFile(); }

		FlashFile *firstFlashFile();
		FlashFile *nextFlashFile();
		int countAllFlashFiles();

		FlashFile *getCurrentFlashFile();
		void setCurrentFlashFile(FlashFile *newFlashFile);

		void handleMessage(SessionMessage *msg);

		void handleDeleteAll();

		const HobString &getFileName() const;
		void setFileName(const HobString &name);
		const HobString &getLocalFlashFileName() const;
		bool doLoadTheFile() const;
		bool isCompleted() const;
		bool testIsGraphicFile(util_filesystem::File &srcFile);
		bool isGraphicFile();
		util_filesystem::Filename *getFileDestination();
		short getGraphicFileType();
		bool startRemoteFlashFileLoad();
		void checkRemoteFlashFile();
		bool getNextRemoteFlashFile();

		void destroyQueriesAndIters();


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		FlashFile *currentFlashFile;

		edb::DBQuery<edb::DBFlashFile> *allFlashFilesQuery;
		edb::DBQuery<edb::DBFlashFile>::const_iterator *allFlashFilesIter;

		util_filesystem::Filename		*fileDestination;
		HobDateTime		currentDate;
		HobString		fileName;				//	File name on the server. From the db record.
		HobString		localFlashFileName;		//	File name locally, copied from the server and excluding any paths from the server.
		HobString		fileDate;				//	From the db record.
		bool			loadFile;
		bool			completed;
		bool			fileIsGraphic;
		short			graphicFileType;

		void init();

#if 0
		void handleRead(FlashFile *msgFlashFile);
		void handleWrite(FlashFile *modifiedFlashFile);
		void handleDelete(FlashFile *msgFlashFile);
		void handleSave(FlashFile *modifiedFlashFile);
#endif

		void handleRestore(FlashFile *restoreFlashFile);

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //FLASHFILEMANAGER_H
