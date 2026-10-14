#ifndef SQLITEPRODUCTPRODUCTSEQUENCEQUERY_H
#define SQLITEPRODUCTPRODUCTSEQUENCEQUERY_H



#include "edb.h"

#include DBQUERYI_INC
#include DBPRODUCTI_INC
#include DBPRODUCTSEQUENCEI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif

	class SqliteDatabase;
	class SqliteProductsRM;
	class SqliteProductSequencesRM;
	class SqliteProductSequence;


	class SqliteProductProductSequenceQuery : public DBQueryI<DBProductI>
	{
		public:
			// --- Constructors and destructor ---
			SqliteProductProductSequenceQuery(const SqliteDatabase& db, DBQueryI<DBProductI>* d = 0);
			~SqliteProductProductSequenceQuery();


			// --- Processing methods ---
			// Methods from DBQueryI
			DBProductI* first();
			DBProductI* next(const DBProductI* current);
			DBProductI* previous(const DBProductI* current);
			DBProductI* last();
			DBProductI* find(const DBProductI* current);

			DBQueryI<DBProductI>::size_type size();
			bool empty();

			void show();


			void selectSequenceLanguage(const std::string& sequence, const std::string& language);


		protected:


		private:
			SqliteProductsRM* products_;
			DBQueryI<DBProductSequenceI>* productSequenceQuery_;
			DBQueryI<DBProductI>* decorated_;

			// Do not allow the default copy constructor.
			SqliteProductProductSequenceQuery(const SqliteProductProductSequenceQuery& rec);
			// Do not allow the default assignment operator.
			SqliteProductProductSequenceQuery operator=(const SqliteProductProductSequenceQuery& rhs);

			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif

	};

	
#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif



#endif // SQLITEPRODUCTPRODUCTSEQUENCEQUERY_H

