/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRESENTATIONMETHOD_H
#define PRESENTATIONMETHOD_H

#include "evolution.h"
#include "HfcObject.h"
#include "Softkey.h"
#include <list>

class DBAccess;
namespace edb 
{
class DBPresentationMethod;
}

typedef enum PresentationMethodFieldOffsets
{
	fieldOffsetPMType = 1,
	fieldOffsetPMDescription,
	endPresentationMethodFieldOffsets
} PresentationMethodFieldOffsets_t;


class PresentationMethod : public HfcObject
{
	public:

		static const short Type_Unknown     = 0;
		static const short Type_MostCommon  = 1;
		static const short Type_Specials    = 2;
		static const short Type_Keyboard    = 3;
		static const short Type_PluNumber   = 4;
		static const short Type_Category    = 5;
		static const short Type_StartScreen = 6;
		static const short Type_Previous    = Softkey::DataType_Previous;
		static const short Type_Next        = Softkey::DataType_Next;


		PresentationMethod();
		PresentationMethod(const char *data, int len, int formatType);
		PresentationMethod(edb::DBPresentationMethod *dbPresentationMethod, DBAccess *access = (DBAccess *)NULL);

		~PresentationMethod();


		short getType() const;
		void setType(short type);

		const HobString &getDescription() const;
		void setDescription(const HobString &desc);


		void updatePresentationMethod(const PresentationMethod *newPresentationMethod);

		virtual const char *serialize(int format) const;


		void setDBAccess(DBAccess *dbAccess);

		const edb::DBPresentationMethod *getMyDbRecord() const;

		bool isRecordValid();
		
		int deleteRecord();
		void saveRecord(); 

		static PresentationMethod *findMethod(short type, DBAccess *dbAccess);

		static void verifyReservedRecords(DBAccess *dbAccess);

		static void initFieldList();


	private:

		int type;

		HobString description;

		DBAccess *dbAccess;

		edb::DBPresentationMethod *myDbRecord; 

		static std::list<PresentationMethodFieldOffsets> fieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //PRESENTATIONMETHOD_H




