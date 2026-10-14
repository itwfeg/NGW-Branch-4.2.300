/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTTEXTMANAGER_H
#define PRODUCTTEXTMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "ProductTextEvent.h"
#include "ProductText.h"
#include "Product.h"

class SessionMessage;
class DBSession;

#include "DBProductTextsRM.h"
#include "DBProductText.h"
#include "DBQuery.h"
#include "HFCManagerContainer.h"

using namespace hfc;



class ProductTextManager : public DataManager
{
	public:

		ProductTextManager(ProdSupportTextType type);
		ProductTextManager(ProdSupportTextType type, hfc::HFCManagerContainer *container);
		virtual ~ProductTextManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		EvolDeviceType getDevice() const {      if (type_ == psttExpandedText)   return ExpandedTextDev;
		                                   else if (type_ == psttSpecialMessage) return SpecialMessageDev;
		                                   else if (type_ == psttProductNote)    return ProductNotesDev;
		                                   else if (type_ == psttMarqueeText)    return MarqueeTextDev;
		                                   else if (type_ == psttProductText5)   return ProductText5Dev;
		                                   else if (type_ == psttProductText6)   return ProductText6Dev;
		                                   else if (type_ == psttProductText7)   return ProductText7Dev;
		                                   else if (type_ == psttProductText8)   return ProductText8Dev;
		                                   else if (type_ == psttProductText9)   return ProductText9Dev;
		                                   else return UnknownDevice;
		                                 }

		HFCManagerContainer::HFCRecordManagerType getRecordManagerType() const
		                                 {      if (type_ == psttExpandedText)   return HFCManagerContainer::Texts;
		                                   else if (type_ == psttSpecialMessage) return HFCManagerContainer::Messages;
		                                   else if (type_ == psttProductNote)    return HFCManagerContainer::Notes;
		                                   else if (type_ == psttMarqueeText)    return HFCManagerContainer::Marquees;
		                                   else if (type_ == psttProductText5)   return HFCManagerContainer::ProductText5;
		                                   else if (type_ == psttProductText6)   return HFCManagerContainer::ProductText6;
		                                   else if (type_ == psttProductText7)   return HFCManagerContainer::ProductText7;
		                                   else if (type_ == psttProductText8)   return HFCManagerContainer::ProductText8;
		                                   else if (type_ == psttProductText9)   return HFCManagerContainer::ProductText9;
		                                   else return HFCManagerContainer::InvalidType;
		                                 }


		/* DataManager virtual methods. */
		virtual ProductText* getFirstRecord() { return firstProductText(); }
		virtual ProductText* getNextRecord()  { return nextProductText(); }

		ProductText *firstProductText();
		ProductText *nextProductText();
		int countAllProductTexts();

		ProductText *getCurrentProductText();
		void setCurrentProductText(ProductText *newProductText);
		void setCurrentProductTextFromProduct(ProductText *newProductText);

		void handleMessage(SessionMessage *msg);

		void handleDeleteAll();
		void handleDeleteAll(HobRecordNum deptNum);
		void handleSaveRecord(ProductText *recordToSave);

		void destroyQueriesAndIters();


	protected:

		ProdSupportTextType type_;

		void handleDelete(SessionMessage *msg);
		void handleRestore(SessionMessage *msg);


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		ProductText *currentProductText;

		edb::DBQuery<edb::DBProductText> *allProductTextsQuery;
		edb::DBQuery<edb::DBProductText>::const_iterator *allProductTextsIter;


		void init();


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //PRODUCTTEXTMANAGER_H
