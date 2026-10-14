/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLCOUNTRYMANAGER_H
#define COOLCOUNTRYMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "COOLDevEvent.h"
#include "COOLCountry.h"

class SessionMessage;
class DBSession;

namespace hfc
{
	class HFCManagerContainer;
}


class COOLCountryManager : public DataManager
{
	public:

		COOLCountryManager();
		COOLCountryManager(hfc::HFCManagerContainer *container);
		virtual ~COOLCountryManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		COOLCountry* getFirstRecord();
		COOLCountry* getNextRecord();
		EvolDeviceType getDevice() const { return COOLDev; }
		virtual UINT8 getWriteEvent() const { return COOLDevWriteCountry; }
		virtual UINT8 getDeleteEvent() const { return COOLDevDeleteCountry; }
		virtual UINT8 getDeleteAllEvent() const { return COOLDevDeleteAllCountries; }

		void handleDeleteAll();
		void handleDeleteAll(HobRecordNum deptNum);


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //COOLCOUNTRYMANAGER_H
