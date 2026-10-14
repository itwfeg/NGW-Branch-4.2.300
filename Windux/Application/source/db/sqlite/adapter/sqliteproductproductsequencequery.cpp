#include <string>


#include "sqliteproductproductsequencequery.h"
#include "sqlitedatabase.h"
#include "sqlitequery.h"
#include "sqliteproductsrm.h"
#include "sqliteproductsequencesrm.h"
#include "sqliteproductsequence.h"
#include "sqliteproduct.h"
#include "sqliteproductproductsequence.h"


#include UTILSTRING_INC
#include DEBUG_INC
#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
int SqliteProductProductSequenceQuery::_dataType_ = 0;
string SqliteProductProductSequenceQuery::_dataTypeName_("SqliteProductProductSequenceQuery");
#endif



SqliteProductProductSequenceQuery::SqliteProductProductSequenceQuery(const SqliteDatabase& db, DBQueryI<DBProductI>* d)
		: products_(db.getProductsRM()), productSequenceQuery_(0), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteProductProductSequenceQuery::~SqliteProductProductSequenceQuery()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( products_ )
		delete products_;

	if ( decorated_ ) 
		delete decorated_;

	if ( productSequenceQuery_ )
		delete productSequenceQuery_;
}


void SqliteProductProductSequenceQuery::selectSequenceLanguage(const string& sequence, const string& language)
{
	if ( products_ )
		productSequenceQuery_ = products_->createProductSequenceQuery(sequence, language);
}


DBProductI* SqliteProductProductSequenceQuery::first()
{
	if ( productSequenceQuery_ && products_ )
	{
		SqliteProductSequence* ps = dynamic_cast<SqliteProductSequence*>(productSequenceQuery_->first());
		if ( ps )
		{
			SqliteRecord* rec = products_->findKey(ps->getProductKey());
			if ( rec )
				return new SqliteProductProductSequence(rec, ps);
			else
				delete ps;
		}
	}

	return 0;
}


DBProductI* SqliteProductProductSequenceQuery::next(const DBProductI* current)
{
	const SqliteProductProductSequence* pps = dynamic_cast<const SqliteProductProductSequence*>(current);

	if ( pps )
	{
		if ( products_ )
		{
			SqliteProductSequence* ps = dynamic_cast<SqliteProductSequence*>(
					productSequenceQuery_->next(pps->getProductSequenceRecord()));
			if ( ps )
			{
				SqliteRecord* rec = products_->findKey(ps->getProductKey());
				if ( rec )
					return new SqliteProductProductSequence(rec, ps);
				else
					delete ps;
			}
		}

		return 0;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR,
				"Current record is not a SQLite layer record.");
}


DBProductI* SqliteProductProductSequenceQuery::previous(const DBProductI* current)
{
	const SqliteProductProductSequence* pps = dynamic_cast<const SqliteProductProductSequence*>(current);

	if ( pps )
	{
		if ( products_ )
		{
			SqliteProductSequence* ps = dynamic_cast<SqliteProductSequence*>(
					productSequenceQuery_->previous(pps->getProductSequenceRecord()));
			if ( ps )
			{
				SqliteRecord* rec = products_->findKey(ps->getProductKey());
				if ( rec )
					return new SqliteProductProductSequence(rec, ps);
				else
					delete ps;
			}
		}

		return 0;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR,
				"Current record is not a SQLite layer record.");
}


DBProductI* SqliteProductProductSequenceQuery::last()
{
	if ( productSequenceQuery_ && products_ )
	{
		SqliteProductSequence* ps = dynamic_cast<SqliteProductSequence*>(productSequenceQuery_->last());
		if ( ps )
		{
			SqliteRecord* rec = products_->findKey(ps->getProductKey());
			if ( rec )
				return new SqliteProductProductSequence(rec, ps);
			else
				delete ps;
		}
	}

	return 0;
}


DBProductI* SqliteProductProductSequenceQuery::find(const DBProductI* current)
{
	const SqliteProductProductSequence* pps = dynamic_cast<const SqliteProductProductSequence*>(current);

	if ( pps )
	{
		if ( products_ )
		{
			SqliteProductSequence* ps = dynamic_cast<SqliteProductSequence*>(
					productSequenceQuery_->find(pps->getProductSequenceRecord()));
			if ( ps )
			{
				SqliteRecord* rec = products_->findKey(ps->getProductKey());
				if ( rec )
					return new SqliteProductProductSequence(rec, ps);
				else
					delete ps;
			}
		}

		return 0;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR,
				"Current record is not a SQLite layer record.");
}


DBQueryI<DBProductI>::size_type SqliteProductProductSequenceQuery::size()
{
	if ( productSequenceQuery_ )
		return productSequenceQuery_->size();

	return 0;
}


bool SqliteProductProductSequenceQuery::empty()
{
	if ( productSequenceQuery_ )
		return productSequenceQuery_->empty();

	return true;
}


void SqliteProductProductSequenceQuery::show()
{
#ifndef EVOL_NDEBUG
	Debug::trace("database.show", 20, "SQLite query instance address: " + ptrtostring(this));
#endif
}



#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
