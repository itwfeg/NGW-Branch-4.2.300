/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef FLASHFILE_H
#define FLASHFILE_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>
#include "filesys/file.h"

class DBAccess;
namespace edb
{
class DBFlashFile;
}

typedef enum FlashFileFieldOffsets		//	Primarily used for dbIndependent format.
{
	fieldOffsetFlashFileName = 1,
	fieldOffsetFlashFileDate,
	endFlashFileFieldOffsets
}   FlashFileFieldOffsets_t;


class FlashFile : public HfcObject
{
	public:

		FlashFile(DBAccess *access = (DBAccess *)NULL);
		FlashFile(const char *data, int len, int formatType);
		FlashFile(edb::DBFlashFile *dbFlashFile, DBAccess *access = (DBAccess *)NULL);

		~FlashFile();


		const HobString &getFileName() const;
		void setFileName(const HobString &name);

		const HobString &getFileDate() const;
		void setFileDate(const HobString &date);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode code);


		void updateFlashFile(const FlashFile *newFlashFile);

		virtual const char *serialize(int format) const;


		void setDBAccess(DBAccess *dbAccess);

		edb::DBFlashFile *getMyDbRecord();

		bool isRecordValid();

		int deleteRecord();
		void saveRecord();

		static FlashFile *findFlashFile(const HobString &fileName, DBAccess *dbAccess);

		static void initFieldList();


	private:

		HobString		fileName;			//	File name on the server. From the db record.
		HobString		fileDate;			//	From the db record.
		HobErrorCode	errorCode;


		DBAccess *dbAccess;

		edb::DBFlashFile *myDbRecord; 

		static std::list<FlashFileFieldOffsets> fieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //FLASHFILE_H
