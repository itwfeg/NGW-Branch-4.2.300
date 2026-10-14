/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGALL_H
#define CFGALL_H

#include "evolution.h"

#include "CfgBase.h"
#include "CfgScale.h"
#include "CfgFrequentShopper.h"
#include "CfgOperatorAccess.h"
#include "CfgPrintedFields.h"
#include "CfgSelfService.h"
#include "CfgNetwork.h"
#include "CfgEmail.h"
#include "CfgSynchronization.h"
#include "CfgFTP.h"
#include "CfgCommInterface.h"
#include "CfgPrepack.h"
#include "CfgMarquee.h"
#include "CfgCurrency.h"
#include "CfgGCD.h"
#include "CfgAccumulateItems.h"

#include "Integer.h"



class CfgAll : public CfgBase, public HfcObject
{
	public:

		CfgAll();
		CfgAll(const char *data, int len, int formatType);
		~CfgAll();


		void updateConfiguration();

		void save(DBAccess *dbAccess);
		void saveScaleOnlyFields(DBAccess *dbAccess);

		const CfgScale *getCfgScalePtr() const {return &cfgScale;}
//		const CfgFrequentShopper *getCfgFrequentShopperPtr() const {return &cfgFrequentShopper;}
//		const CfgOperatorAccess *getCfgOperatorAccessPtr() const {return &cfgOperatorAccess;}
//		const CfgPrintedFields *getCfgPrintedFieldsPtr() const {return &cfgPrintedFields;}
		const CfgSelfService *getCfgSelfServicePtr() const {return &cfgSelfService;}
		const CfgNetwork *getCfgNetworkPtr() const {return &cfgNetwork;}
		const CfgEmail *getCfgEmailPtr() const {return &cfgEmail;}
//		const CfgSynchronization *getCfgSynchronizationPtr() const {return &cfgSynchronization;}
//		const CfgFTP *getCfgFTPPtr() const {return &cfgFTP;}
		const CfgCommInterface *getCfgCommInterfacePtr() const {return &cfgCommInterface;}
//		const CfgPrepack *getCfgPrepackPtr() const {return &cfgPrepack;}
		const CfgMarquee *getCfgMarqueePtr() const {return &cfgMarquee;}
		const CfgCurrency * getCfgCurrencyPtr() const { return &cfgCurrency; }
		const CfgGCD *getCfgGCDPtr() const { return &cfgGCD; }
//		const CfgAccumulateItems *getCfgAccumulateItemsPtr() const {return &cfgAccumulateItems;}

		const HobString &getRwEanFormat() const {return rwEanFormat;}
		void setRwEanFormat(const HobString &val) {rwEanFormat = val;}
		const HobString &getFwEanFormat() const {return fwEanFormat;}
		void setFwEanFormat(const HobString &val) {fwEanFormat = val;}
		const HobString &getBcEanFormat() const {return bcEanFormat;}
		void setBcEanFormat(const HobString &val) {bcEanFormat = val;}

		// ALV
		const HobString &getRwEan128cFormat() const {return rwEan128cFormat;}
		void setRwEan128cFormat(const HobString &val) {rwEan128cFormat = val;}
		const HobString &getFwEan128cFormat() const {return fwEan128cFormat;}
		void setFwEan128cFormat(const HobString &val) {fwEan128cFormat = val;}
		const HobString &getBcEan128cFormat() const {return bcEan128cFormat;}
		void setBcEan128cFormat(const HobString &val) {bcEan128cFormat = val;}

		const HobString &getRwEanPrefix() const {return rwEanPrefix;}
		void setRwEanPrefix(const HobString &val) {rwEanPrefix = val;}
		const HobString &getFwEanPrefix() const {return fwEanPrefix;}
		void setFwEanPrefix(const HobString &val) {fwEanPrefix = val;}
		const HobString &getBcEanPrefix() const {return bcEanPrefix;}
		void setBcEanPrefix(const HobString &val) {bcEanPrefix = val;}

		virtual const char *serialize(int format) const;
		virtual void serializeUsingList(const list<CfgFieldOffsets> fieldOffsetList, int format) const;

		static void initFieldList();		 		 


	private:

		CfgScale                cfgScale;
		CfgFrequentShopper      cfgFrequentShopper;
		CfgOperatorAccess       cfgOperatorAccess;
		CfgPrintedFields        cfgPrintedFields;
		CfgSelfService          cfgSelfService;
		CfgNetwork              cfgNetwork;
		CfgEmail                cfgEmail;
		CfgSynchronization      cfgSynchronization;
		CfgFTP					cfgFTP;
		CfgCommInterface        cfgCommInterface;
		CfgPrepack              cfgPrepack;
		CfgMarquee              cfgMarquee;
		CfgCurrency             cfgCurrency;
		CfgGCD					cfgGCD;
		CfgAccumulateItems      cfgAccumulateItems;

		//	These 6 HobStrings are temporary holding places for EAN info from UltimaTaggedFormat.
		//	We will restore them to their respective ProductDefaults records after all the other Config classes have been restored.
		HobString	rwEanFormat;		//	From UltimaTaggedFormat
		HobString	fwEanFormat;		//	From UltimaTaggedFormat
		HobString	bcEanFormat;		//	From UltimaTaggedFormat

		HobString	rwEan128cFormat;		//	From UltimaTaggedFormat
		HobString	fwEan128cFormat;		//	From UltimaTaggedFormat
		HobString	bcEan128cFormat;		//	From UltimaTaggedFormat

		HobString	rwEanPrefix;		//	From UltimaTaggedFormat
		HobString	fwEanPrefix;		//	From UltimaTaggedFormat
		HobString	bcEanPrefix;		//	From UltimaTaggedFormat

		static std::list<CfgFieldOffsets> evolFieldList, ultimaFieldList, synchronizationList, dbIndependentList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CFGALL_H
