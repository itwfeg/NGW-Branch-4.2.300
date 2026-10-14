/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTDEFAULT_H
#define PRODUCTDEFAULT_H

#include "evolution.h"
#include "HfcObject.h"
#include "Product.h"

#include <list>

class DBAccess;

namespace edb
{
class DBProduct;
class DBProductDefault;
}

class ProductDefault : public Product
{
	public:

		ProductDefault();
		ProductDefault(const char *data, int len, int formatType);
		ProductDefault(edb::DBProductDefault *dbProductDefaultRec, DBAccess *access = (DBAccess *)NULL);

		~ProductDefault();


		HobPrice getFractionalPrice() const;
		void setFractionalPrice(HobPrice fractionalPrice);

		HobPrice getFractionalMode() const;
		void setFractionalMode(HobPrice fractionalPriceMode);

		bool isFractionalPriceForced() const;
		void setFractionalPriceForced(bool fractionalPriceForced);

		HobPrice getPoundsForPrice() const;
		void setPoundsForPrice(HobPrice poundsForPrice);

		HobPrice getPoundsFor() const;
		void setPoundsFor(HobPrice poundsFor);

		bool isPoundsForPriceForced() const;
		void setPoundsForPriceForced(bool poundsForPriceForced);

		HobString getEAN13PrefixString() const;
		void setEAN13PrefixString(HobString formatString);

		HobString getFormatString(BarcodeType bcType) const;
		void setFormatString(BarcodeType bcType, HobString formatString);


		bool updateField(int fieldOffset, const HobString &newValue, bool linksMustExist = false);
		void updateRecord(ProductDefault *newData, AbstractSession *session);

		void getFieldAsString(int fieldOffset, HobString &result, int format) const;
		virtual const char *serialize(int format) const;

		bool isRecordValid() const;
		int deleteRecord();
		void saveRecord();

		static ProductDefault *findRecord(short productType, HobRecordNum deptNum, DBAccess *dbAccess);

		static void verifyReservedRecords(DBAccess *dbAccess);

		static void initFieldLists();


	protected:

		HobString ean13PrefixString;


	private:

		edb::DBProductDefault *myDbRecord;

		static std::list<ProductFieldOffsets> evolFieldList, dbindependentFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //PRODUCTDEFAULT_H

