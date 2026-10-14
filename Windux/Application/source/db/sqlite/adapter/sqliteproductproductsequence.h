#ifndef SQLITEPRODUCTPRODUCTSEQUENCE_H
#define SQLITEPRODUCTPRODUCTSEQUENCE_H


#include "edb.h"
#include "sqliteproduct.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteProductSequence;
	class DBProductI;


	class SqliteProductProductSequence : public SqliteProduct
	{
		public:

			// Types and subtypes
			typedef SqliteProduct::interface_type interface_type;

			
			// Constructors and destructor
			SqliteProductProductSequence(SqliteRecord* r, SqliteProductSequence* ps, DBProductI* d = 0) : SqliteProduct(r, d), ps_(ps) { }
			~SqliteProductProductSequence() { if ( ps_ ) delete ps_; }

			virtual DBProductI* clone() const { return new SqliteProductProductSequence(new SqliteRecord(*getRecord()), dynamic_cast<SqliteProductSequence*>(ps_->clone())); }

			const SqliteProductSequence* getProductSequenceRecord() const { return ps_; }


		private:

			SqliteProductSequence* ps_;

			// Do not allow the default copy constructor.
			SqliteProductProductSequence(const SqliteProductProductSequence& rec);
			// Do not allow the default assignment operator.
			SqliteProductProductSequence operator=(const SqliteProductProductSequence& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRODUCTPRODUCTSEQUENCE_H
