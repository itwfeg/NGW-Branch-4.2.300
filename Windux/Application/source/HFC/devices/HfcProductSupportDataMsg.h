/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRODUCTSUPPORTDATAMSG_H
#define HFCPRODUCTSUPPORTDATAMSG_H

#include "evolution.h"
#include "HfcObject.h"
#include "Product.h"
#include "TextLinks.h"

class DBAccess;

namespace edb
{
class DBProductText;
class DBNutrition;
}

namespace hfc
{
	class HFCManagerContainer;
}


typedef enum ProductSupportDataFieldOffsets
{
	fieldOffsetPSDCurrentProductNum = 1,
	fieldOffsetPSDTextType,
	fieldOffsetPSDUsedByMany,
	fieldOffsetPSDFunction,
	fieldOffsetPSDProductNum,
	fieldOffsetPSDText,
	fieldOffsetPSDTextBlock2,	//	For Nutrifacts
	fieldOffsetPSDTextBlock3,	//	For Nutrifacts
	fieldOffsetPSDTextBlock4,	//	For Nutrifacts
	endProductSupportDataFieldOffsets
} ProductSupportDataFieldOffsets_t;


class HfcProductSupportDataMsg : public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		HfcProductSupportDataMsg(Product *pProduct = NULL);
		HfcProductSupportDataMsg(const char *data, int len, int formatType);

		~HfcProductSupportDataMsg();

		void init();

		HobRecordNum getCurrentProductNum();
		void setCurrentProductNum(HobRecordNum productNum);

		ProdSupportTextType getProdSupportTextType();
		void setProdSupportTextType(ProdSupportTextType type);

		HobString getText();
		void setText(HobString text);
		HobString getTextBlock2();
		void setTextBlock2(HobString text);
		HobString getTextBlock3();
		void setTextBlock3(HobString text);
		HobString getTextBlock4();
		void setTextBlock4(HobString text);

		ProdSupportFunction getProdSupportFunction();
		void setProdSupportFunction(ProdSupportFunction function);

		HobRecordNum getProductNum();
		void setProductNum(HobRecordNum productNum);

		bool isUsedByMany();
		void setUsedByMany(bool usedByMany);

		virtual const char *serialize(int format) const;
		
		void newText(DBAccess *dbAccess, Product *product);
		bool copyText(DBAccess *dbAccess, Product *product);
		void deleteText(DBAccess *dbAccess, Product *product);
		bool useText(DBAccess *dbAccess, Product *product);

		TextLinks *getTextLinks(DBAccess *dbAccess, Product *product);

		static HobRecordNum getNutrifactsNumFromProduct(DBAccess *dbAccess, HobRecordNum productNum, HobRecordNum deptNum);
		static HobRecordNum findNewNutrifactsNumber(DBAccess *dbAccess, HobRecordNum textNum, HobRecordNum deptNum);
		void removeNutrifactsFromAllProducts(DBAccess *dbAccess, Nutrifacts *nutrifacts);

		HobRecordNum findNewProductTextNumber(DBAccess *dbAccess, HobRecordNum textNum, HobRecordNum deptNum, ProdSupportTextType supportTextType);
		HobRecordNum getProductTextNumFromProduct(DBAccess *dbAccess, HobRecordNum productNum, HobRecordNum deptNum, ProdSupportTextType supportTextType);
		void removeProductTextFromAllProducts(DBAccess *dbAccess, ProductText *productText);

		void setMgrContainer(hfc::HFCManagerContainer *mgrContainer);


	private:

		Product				*myProduct;
		HobRecordNum		currentProductNum;
		ProdSupportTextType	supportTextType;
		HobString			text;
		HobString			textBlock2;
		HobString			textBlock3;
		HobString			textBlock4;
		ProdSupportFunction	editFunction;
		HobRecordNum		productNum;
		bool				usedByMany;

		//This class won't own this mgrContainer; it's only borrowing it (from ProductManager).
		hfc::HFCManagerContainer *mgrContainer;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCPRODUCTSUPPORTDATAMSG_H
