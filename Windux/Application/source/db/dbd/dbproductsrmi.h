#ifndef DBPRODUCTSRMI_H
#define DBPRODUCTSRMI_H


#include <string>
#include "evolution.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBProductI;
	class DBClassI;
	class DBProductTextI;
	class DBNutritionI;
	class DBCategoryI;
	class DBCoolCategoryI;
	class DBCoolPreTextI;
	class DBCoolTextI;
	class DBCoolListI;
	class DBProductStatisticI;
	class DBSpecialProgramI;
	class DBSoftKeyI;
	template <class DBEntityI> class DBQueryI;


	class DBProductsRMI
	{
		public:

			virtual ~DBProductsRMI() {}


			virtual DBQueryI<DBProductI>* retrieveAll() = 0;
			virtual DBQueryI<DBProductI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBProductI>* retrieveRangePLU(const std::string& first, const std::string& last, const std::string& deptNum) = 0;
			virtual DBQueryI<DBProductI>* retrieveNearestPLU(const DBProductI* product, int count) = 0;
			virtual DBQueryI<DBProductI>* retrieveNearestPLU(const std::string& plu, const std::string& deptNum, int count) = 0;
			virtual DBQueryI<DBProductI>* retrieveKeywordInName(const std::string& keyword) = 0;
			virtual DBQueryI<DBProductI>* retrieveKeywordInDescription(const std::string& keyword, const std::string& deptNum) = 0;
			virtual DBQueryI<DBProductI>* retrieveNameStartsWith(const std::string& keyword, LanguageSelection langNum, const std::string& deptNum) = 0;
			virtual DBQueryI<DBProductI>* retrieveDescriptionStartsWith(const std::string& keyword, const std::string& deptNum) = 0;
			virtual DBQueryI<DBProductI>* retrieveDescriptionOrNameStartsWith(const std::string& keyword, LanguageSelection langNum, const std::string& deptNum) = 0;
			virtual DBQueryI<DBProductI>* retrieveLinkedtoClass(const std::string& classNumber, const std::string& deptNum) = 0;
			virtual DBQueryI<DBProductI>* retrieveLinkedtoClass(const DBClassI* clas) = 0;
			virtual DBQueryI<DBProductI>* retrieveLinkedtoText(const DBProductTextI* text) = 0;
			virtual DBQueryI<DBProductI>* retrieveLinkedtoMessage(const DBProductTextI* message) = 0;
			virtual DBQueryI<DBProductI>* retrieveLinkedtoNutrition(const DBNutritionI* nutrition) = 0;
			virtual DBQueryI<DBProductI>* retrieveLinkedtoMarquee(const DBProductTextI* marquee) = 0;
			virtual DBQueryI<DBProductI>* retrieveLinkedtoNote(const DBProductTextI* note) = 0;
			virtual DBQueryI<DBProductI>* retrieveLinkedtoProductText5(const DBProductTextI* text) = 0;
			virtual DBQueryI<DBProductI>* retrieveLinkedtoProductText6(const DBProductTextI* text) = 0;
			virtual DBQueryI<DBProductI>* retrieveLinkedtoProductText7(const DBProductTextI* text) = 0;
			virtual DBQueryI<DBProductI>* retrieveLinkedtoProductText8(const DBProductTextI* text) = 0;
			virtual DBQueryI<DBProductI>* retrieveLinkedtoProductText9(const DBProductTextI* text) = 0;
			virtual DBQueryI<DBProductI>* retrieveLinkedtoCategory(const DBCategoryI* category) = 0;
			virtual DBQueryI<DBProductI>* retrieveLinkedtoCoolCategory(const DBCoolCategoryI* coolcategory) = 0;
			virtual DBQueryI<DBProductI>* retrieveLinkedtoCoolPreText(const DBCoolPreTextI* coolpretext) = 0;
			virtual DBQueryI<DBProductI>* retrieveLinkedtoDefaultCoolText(const DBCoolTextI* cooltext) = 0;
			virtual DBQueryI<DBProductI>* retrieveLinkedtoCoolList(const DBCoolListI* coollist) = 0;
			virtual DBQueryI<DBProductI>* retrieveSpecials(const DBSpecialProgramI* special) = 0;
			virtual DBQueryI<DBProductI>* retrieveAllSpecials(const std::string& deptNum) = 0;
			virtual DBQueryI<DBProductI>* retrieveLinkedtoRec(const std::string& column1Name, const std::string column1Value, const std::string& column2Name = "", const std::string& column2Value = "") = 0;

			virtual bool isProductShortNameUsed(LanguageSelection langNum) = 0;

			virtual DBProductI* findPLU(const std::string& plu, const std::string& deptNum) = 0;
			virtual DBProductI* findLinkedRecord(const DBProductStatisticI* stat) = 0;
			virtual DBProductI* findLinkedRecord(const DBSoftKeyI* skey) = 0;

			virtual DBProductI* createRecord() = 0;
			virtual void saveRecord(const DBProductI* rec) = 0;
			virtual int deleteRecord(const DBProductI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int deleteAllForDept(const std::string& deptNum) = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBPRODUCTSRMI_H
