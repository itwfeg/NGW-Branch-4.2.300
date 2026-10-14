#ifndef SQLITEPRODUCTSRM_H
#define SQLITEPRODUCTSRM_H


#include <string>
#include <vector>
#ifdef WIN32_OS
	#include <locale>
#endif


#include "edb.h"
#include "sqliterecordmanager.h"
#include "sqliteproductsequencesrm.h"
#include "sqliteignorewordsrm.h"

#include DBPRODUCTSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;
	class SqliteProduct;
	class SqliteProductSequence;


	class SqliteProductsRM : public DBProductsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteProductsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBProductsRMI* d = 0);
			~SqliteProductsRM();


			// --- Processing methods ---
			// Methods from DBProductsRMI
			DBQueryI<DBProductI>* retrieveAll();
			DBQueryI<DBProductI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBProductI>* retrieveRangePLU(const std::string& first, const std::string& last, const std::string& deptNum);
			DBQueryI<DBProductI>* retrieveNearestPLU(const DBProductI* product, int count);
			DBQueryI<DBProductI>* retrieveNearestPLU(const std::string& plu, const std::string& deptNum, int count);
			DBQueryI<DBProductI>* retrieveKeywordInName(const std::string& keyword);
			DBQueryI<DBProductI>* retrieveKeywordInDescription(const std::string& keyword, const std::string& deptNum);
			DBQueryI<DBProductI>* retrieveNameStartsWith(const std::string& keyword, LanguageSelection langNum, const std::string& deptNum);
			DBQueryI<DBProductI>* retrieveDescriptionStartsWith(const std::string& keyword, const std::string& deptNum);
			DBQueryI<DBProductI>* retrieveDescriptionOrNameStartsWith(const std::string& keyword, LanguageSelection langNum, const std::string& deptNum);
			DBQueryI<DBProductI>* retrieveLinkedtoClass(const std::string& classNumber, const std::string& deptNum);
			DBQueryI<DBProductI>* retrieveLinkedtoClass(const DBClassI* clas);
			DBQueryI<DBProductI>* retrieveLinkedtoText(const DBProductTextI* text);
			DBQueryI<DBProductI>* retrieveLinkedtoMessage(const DBProductTextI* message);
			DBQueryI<DBProductI>* retrieveLinkedtoNutrition(const DBNutritionI* nutrition);
			DBQueryI<DBProductI>* retrieveLinkedtoMarquee(const DBProductTextI* marquee);
			DBQueryI<DBProductI>* retrieveLinkedtoNote(const DBProductTextI* note);
			DBQueryI<DBProductI>* retrieveLinkedtoProductText5(const DBProductTextI* prodText);
			DBQueryI<DBProductI>* retrieveLinkedtoProductText6(const DBProductTextI* prodText);
			DBQueryI<DBProductI>* retrieveLinkedtoProductText7(const DBProductTextI* prodText);
			DBQueryI<DBProductI>* retrieveLinkedtoProductText8(const DBProductTextI* prodText);
			DBQueryI<DBProductI>* retrieveLinkedtoProductText9(const DBProductTextI* prodText);
			DBQueryI<DBProductI>* retrieveLinkedtoCategory(const DBCategoryI* category);
			DBQueryI<DBProductI>* retrieveLinkedtoCoolCategory(const DBCoolCategoryI* coolcategory);
			DBQueryI<DBProductI>* retrieveLinkedtoCoolPreText(const DBCoolPreTextI* coolpretext);
			DBQueryI<DBProductI>* retrieveLinkedtoDefaultCoolText(const DBCoolTextI* cooltext);
			DBQueryI<DBProductI>* retrieveLinkedtoCoolList(const DBCoolListI* coollist);
			DBQueryI<DBProductI>* retrieveSpecials(const DBSpecialProgramI* special);
			DBQueryI<DBProductI>* retrieveAllSpecials(const std::string& deptNum);
			DBQueryI<DBProductI>* retrieveLinkedtoRec(const std::string& column1Name, const std::string column1Value, const std::string& column2Name = "", const std::string& column2Value = "");

			bool isProductShortNameUsed(LanguageSelection langNum);

			DBProductI* findPLU(const std::string& plu, const std::string& deptNum);
			DBProductI* findLinkedRecord(const DBProductStatisticI* stat);
			DBProductI* findLinkedRecord(const DBSoftKeyI* skey);

			DBProductI* createRecord();
			void saveRecord(const DBProductI* rec);
			int deleteRecord(const DBProductI* rec);
			int deleteAll();
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace();
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show();


			// Overrides of SqliteRecordManager's virtual methods
			bool copy(bool from, const util_filesystem::Directory& path);
			bool backup(const std::string& dir);
			bool exists();
			bool compress();
			bool remove();
			bool check();


			// Other methods
			DBQueryI<DBProductSequenceI>* createProductSequenceQuery(const std::string& sequence, const std::string& language);


		private:

			const SqliteDatabase& sdb_;
			DBProductsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteProductsRM(const SqliteProductsRM& rm);
			// Do not allow the default assignment operator.
			SqliteProductsRM operator=(const SqliteProductsRM& rhs);


			const static std::string _longDescriptionLanguage_;

			int localeTransformedLength_;
			bool useShortDescriptionSequences_;
			bool useLongDescriptionSequences_;
			bool useSequences_;
			int minSequenceLength_, maxSequenceLength_;
			SqliteProductSequencesRM* productSequences_;

			bool validateWords_;
			bool allowDigitsInSequences_;
			bool useIgnoreWords_;
			SqliteIgnoreWordsRM* ignoreWords_;

			void retrieveProperties();
			void addSequences(const SqliteProduct* prod, const std::string& shortName, const std::vector<std::string>* sequences, const std::string& language, int sequenceLength, const std::locale& loc);
			void addASingleSeq(const std::string& seq, bool doNotTransform, const std::locale& loc, const std::string& language, int sequenceLength, const std::string& shortName, const SqliteProduct* prod);

			void removeSequences(const SqliteProduct* prod, const std::string& language);

			std::string cleanupDescription(const std::string& description, const std::locale& language);  /* (SJJC port 6/19/06) */
			bool isValidWord(const std::string& word, const std::locale& language);  /* (SJJC port 6/19/06) */

			DBQueryI<DBProductI>* retrieveTextColumnStartsWith(const std::string& keyword, const std::string& columnName, const std::string& columnName2, const std::string& deptNum);

			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};

	
#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRODUCTSRM_H
