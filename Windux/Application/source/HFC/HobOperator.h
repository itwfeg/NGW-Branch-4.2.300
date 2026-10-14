/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HOBOPERATOR_H
#define HOBOPERATOR_H

#include "evolution.h"
#include "HfcObject.h"
#include "OpModeEvent.h"

#include <list>

class DBAccess;
namespace edb 
{
class DBOperator;
}



typedef enum HobOperatorFieldOffsets
{
	fieldOffsetOpNum = 1,
	fieldOffsetOpRFID,
	fieldOffsetOpName,
	fieldOffsetAccessLevel,
	fieldOffsetOpLang,
	fieldOffsetSawNote,
	fieldOffsetMode,
	
//	fieldOffsetSelectedLookupMethod, // not yet implemented in Browser
	endHobOperatorFieldOffsets
}   HobOperatorFieldOffsets_t;


class HobOperator : public HfcObject
{
	public:

		static const short ACCESSLEVEL_SUPERVISOR = 9;

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		HobOperator();
		HobOperator(const HobOperator &source);
		HobOperator(const char *data, int len, int formatType);
		HobOperator(edb::DBOperator *dbOperator, DBAccess *access = (DBAccess *)NULL);

		~HobOperator();


		const HobString &getOpNum() const;
		void setOpNum(const HobString &num);

		const HobString &getOpRFID() const;
		void setOpRFID(const HobString &rfid);

		const HobString &getOpName() const;
		void setOpName(const HobString &name);
		
		short getAccessLevel() const;
		void setAccessLevel(short level);

		const HobString &getOpLang() const;
		void setOpLang(const HobString &language);

		bool hasSeenNote() const;
		void setSawNote(bool saw);

		short getSelectedLookupMethod() const;
		void setSelectedLookupMethod(short selectedLookupMethod);

		bool isLoggedIn() const;
		void setLoggedIn(bool loggedIn);

		void validate();

		void setWhatOperatorCanModify();

		/**
		 * Checks if the operator is allowed to perform the functionality specified by the parameter passed in.
		 * Returns true if they have access; false otherwise.
		 * The values for the parameter, 'function', currently supported by the Ultima,
		 * can be found in cb_uif.c
		 */
		bool isAllowed(int function) const;


		static const short CURRENTMODE_UNKNOWN		= 0;
		static const short CURRENTMODE_OPERATE		= 1;
		static const short CURRENTMODE_SUPERVISOR	= 2;
		static const short CURRENTMODE_SERVICE		= 3;

		short getCurrentMode() const;
		void setCurrentMode(short currentMode);
		void setCurrentMode(OpModeEvent mode);


		static const short LOGINMETHOD_OPNUM = 0;
		static const short LOGINMETHOD_RFID = 1;
		
		short getLoginMethod() const;
		void setLoginMethod(short method);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);


		int updateField(int fieldCode, const HobString &token, int formatType = BrowserFormat);
		void updateOperator(HobOperator *newOperator);

		void getFieldAsString(int fieldCode, HobString &result, int format) const;

		virtual const char *serialize(int format) const;


		void setDBAccess(DBAccess *dbAccess);

		edb::DBOperator *getMyDbRecord();

		bool isRecordValid();

		int deleteRecord();
		void saveRecord();

		static HobOperator *find(const HobString &opNum, DBAccess *dbAccess);
		static HobOperator *findRFID(const HobString &rfid, DBAccess *dbAccess);

		static void initFieldList();


	private:

		/** @supplierCardinality 0..*
		 * @clientCardinality 1
		 * @clientRole makes
		 * @supplierRole made by
		 * @directed */
		HobString opNum;

		HobString opRFID;

		HobString opName;

		HobString opPLang;
		
		
		/**
		 * The following business rule must be checked by this property's setter:
		 * The accessLevel must be between 0 and 9.  0 indicates the accessLevel was never set; it is uninitialized.
		 * The getter can/may check if this property is uninitialized, and return a default value.
		 *
		 * The access levels, and the functionality allowed at each level, are described in the
		 * Ultima source module, cb_uif.c.
		 */
		short accessLevel;
		static const int defaultAccessLevel = 9;

		/**
		 * Flag indicating the operator has seen the operator note.
		 */
		bool sawNote;

		/**
		 * This property indicates which plu lookup method the operator is using:
		 *
		 * 0 = Classic method, lookup by plu number or UPC number.
		 * 1 = Historical method.
		 * 2 = Frequency method.
		 * 3 = Favorites method.
		 * 4 = Key word search, based on item description.
		 *
		 * The initial value will be 0.
		 */
		short selectedLookupMethod;

		bool loggedIn;

		short loginMethod;

		/**
		 * The current mode (Operate, Supervisor, Service) this operator is logged into.
		 */
		short currentMode;

		HobErrorCode errorCode;


		DBAccess *dbAccess;

		edb::DBOperator *myDbRecord;

		static std::list<HobOperatorFieldOffsets> evolFieldList, dbindependentFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HOBOPERATOR_H
