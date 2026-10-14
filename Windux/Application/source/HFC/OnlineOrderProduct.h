/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef ONLINEORDERPRODUCT_H
#define ONLINEORDERPRODUCT_H


#include <vector>
#include <list>

#include "evolution.h"
#include "HfcObject.h"
#include "ProductDevEvent.h"
#include "Product.h"

#include "CCriticalSection.h"



class OnlineOrderProduct : public Product
{
	public:

		OnlineOrderProduct();
		OnlineOrderProduct(const char *data, int len, int formatType);
		OnlineOrderProduct(edb::DBProduct *dbProductRec, DBAccess *access = (DBAccess *)NULL, bool loadLinkedData = true);

		~OnlineOrderProduct();

		void updateOnlineOrderingData(OnlineOrderProduct *ooSourceProduct);

		HobRecordNum getOrderNum() const { return orderNum; }
		void setOrderNum(HobRecordNum oNum) { orderNum = oNum; }

		HobRecordNum getOrderProductId() const { return orderProductId; }
		void setOrderProductId(HobRecordNum oProductId) { orderProductId = oProductId; }

		HobString getCustomerName() const { return customerName; }
		void setCustomerName(HobString name) { customerName = name; }

		HobString getPickupTime() const { return pickupTime; }
		void setPickupTime(HobString time) { pickupTime = time; }

		static void initFieldLists();			//initializes a list of field offset in either ultima, Evolution or Browser format

		bool updateField(int fieldOffset, const HobString &newValue, bool linksMustExist = false, int formatType = BrowserFormat);


	protected:

		HobRecordNum orderNum;

		HobRecordNum orderProductId;

		HobString customerName;

		HobString pickupTime;

		// For totalPrice and totalWeight, get these from the ProductTranscation object within theProductTransaction *
		// HobValue64 totalPrice;
		// HobWeight totalWeight;
		
		static std::list<ProductFieldOffsets> uiFieldList;


	private:

		void init();

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //ONLINEORDERPRODUCT_H




