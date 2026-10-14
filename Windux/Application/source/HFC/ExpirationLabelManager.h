/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef EXPIRATIONLABELMANAGER_H
#define EXPIRATIONLABELMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "ExpirationLabel.h"
#include "ProductDevEvent.h"

class SessionMessage;
class DBSession;
class Product;
class ProductManager;

#include "DBExpirationLabelsRM.h"
#include "DBExpirationLabel.h"
#include "DBQuery.h"

namespace hfc
{
    class HFCManagerContainer;
}


class ExpirationLabelManager : public DataManager
{
    public:

        ExpirationLabelManager();
        ExpirationLabelManager(hfc::HFCManagerContainer *container);
        virtual ~ExpirationLabelManager();

        DBSession *getMySession();
        void setMySession(DBSession *session);

        /* DataManager virtual methods. */
        ExpirationLabel* getFirstRecord() { return firstExpirationLabel(); }
        ExpirationLabel* getNextRecord()  { return nextExpirationLabel(); }
        EvolDeviceType getDevice() const { return ProductDev; }
        UINT8 getWriteEvent() const { return ProductWriteExpirationLabel; }
        UINT8 getDeleteAllEvent() const { return ProductDeleteAllExpirationLabels; }

        ExpirationLabel *firstExpirationLabel();
        ExpirationLabel *nextExpirationLabel();
        int countAllExpirationLabels();

        ExpirationLabel *getCurrentExpirationLabel();
        void setCurrentExpirationLabel(ExpirationLabel *newEL);

        void handleMessage(SessionMessage *msg);

        void handleProductPrintExpirationLabel(UINT8 newOrExisting);
        void handlePromptForExpirationDays();
        void handleProductPrintNewExpirationLabel(Product *currentProduct);
        void handleProductPrintExistingExpirationLabel(Product *currentProduct);
        void handleProductCompleteMessage(short whichExpirationLabel, HobRecordNum productNum, HobRecordNum deptNum, const HobString &productDesc, const HobDateTime &shelfLifeDateTime);
		void handleDeleteAll();
		void handleDeleteAll(HobRecordNum deptNum);

        void destroyQueriesAndIters();

		void purgeOldExpirationLabels();


    private:

        DBSession *mySession;
        hfc::HFCManagerContainer *mgrContainer_;

        ExpirationLabel *currentExpirationLabel;

        edb::DBQuery<edb::DBExpirationLabel> *allExpirationLabelsQuery;
        edb::DBQuery<edb::DBExpirationLabel>::const_iterator *allExpirationLabelsIter;

        void init();

        void handleRead(ExpirationLabel *el);
        void handleReadAll();
        void handleWrite(ExpirationLabel *el);
        void handleDelete(ExpirationLabel *el);
        void handleSave(ExpirationLabel *el);
        void handleRestore(ExpirationLabel *el);
        //void handleCreate(ExpirationLabel *el);
        
        bool oldProductExpirationDaysForced;
        int oldProductExpirationDays;


#ifdef USE_ACCOUNTANT
        static int _dataType_;
        static std::string _dataTypeName_;
#endif
};

#endif //EXPIRATIONLABELMANAGER_H
