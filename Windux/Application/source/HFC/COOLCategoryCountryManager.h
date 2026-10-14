/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLCATEGORYCOUNTRYMANAGER_H
#define COOLCATEGORYCOUNTRYMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "COOLDevEvent.h"
#include "COOLCategoryCountry.h"

class SessionMessage;
class DBSession;

namespace hfc
{
	class HFCManagerContainer;
}


class COOLCategoryCountryManager : public DataManager
{
	public:

		COOLCategoryCountryManager();
		COOLCategoryCountryManager(hfc::HFCManagerContainer *container);
		virtual ~COOLCategoryCountryManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		COOLCategoryCountry* getFirstRecord();
		COOLCategoryCountry* getNextRecord();
		EvolDeviceType getDevice() const { return COOLDev; }
		virtual UINT8 getWriteEvent() const { return COOLDevWriteCategoryCountry; }
		virtual UINT8 getDeleteEvent() const { return COOLDevDeleteCategoryCountry; }
		virtual UINT8 getDeleteAllEvent() const { return COOLDevDeleteAllCategoryCountries; }

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

#endif //COOLCATEGORYCOUNTRYMANAGER_H
