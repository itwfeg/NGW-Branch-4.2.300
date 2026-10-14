#include <string>


#include "sqlitedatabase.h"
#include "sqliteproductsequencesrm.h"
#include "sqliteproductsequence.h"
#include "sqliteproduct.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteProductSequence>::_dataType_ = 0;
template <> string SqliteQuery<SqliteProductSequence>::_dataTypeName_("SqliteQuery<SqliteProductSequence>");

int SqliteProductSequencesRM::_dataType_ = 0;
string SqliteProductSequencesRM::_dataTypeName_("SqliteProductSequencesRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteProductSequencesRM::DATA_FILENAME("ProductCharacterSequences");



SqliteProductSequencesRM::SqliteProductSequencesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBProductSequencesRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteProductSequence::PRODUCTKEY_FIELD);
	columnNames_.insert(SqliteProductSequence::CHARACTERSEQUENCELANGUAGE_FIELD);
	columnNames_.insert(SqliteProductSequence::CHARACTERSEQUENCE_FIELD);
	columnNames_.insert(SqliteProductSequence::PRODUCTNAME_FIELD);
	columnNames_.insert(SqliteProductSequence::PRODUCTPRICE_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteProductSequence::PRODUCTKEY_FIELD,                SqliteField(SqliteProductSequence::PRODUCTKEY_FIELD,                SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductSequence::CHARACTERSEQUENCELANGUAGE_FIELD, SqliteField(SqliteProductSequence::CHARACTERSEQUENCELANGUAGE_FIELD, SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductSequence::CHARACTERSEQUENCE_FIELD,         SqliteField(SqliteProductSequence::CHARACTERSEQUENCE_FIELD,         SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductSequence::PRODUCTNAME_FIELD,               SqliteField(SqliteProductSequence::PRODUCTNAME_FIELD,               SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductSequence::PRODUCTPRICE_FIELD,              SqliteField(SqliteProductSequence::PRODUCTPRICE_FIELD,              SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteProductSequencesRM::~SqliteProductSequencesRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBProductSequenceI>* SqliteProductSequencesRM::retrieveAll()
{
	SqliteQuery<SqliteProductSequence>* query = new SqliteQuery<SqliteProductSequence>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBProductSequenceI>* SqliteProductSequencesRM::retrieveLinkedToProduct(const DBProductI* prod)
{
	const SqliteProduct* inrec = dynamic_cast<const SqliteProduct*>(prod);

	if ( inrec )
	{
		SqliteQuery<SqliteProductSequence>* query = new SqliteQuery<SqliteProductSequence>(*this);

		string filter = SqliteProductSequence::PRODUCTKEY_FIELD + " = " + inrec->getProductKey();
		query->selectSome(filter, SqliteProductSequence::CHARACTERSEQUENCELANGUAGE_FIELD);

		return query;
	}

	return 0;
}


DBQueryI<DBProductSequenceI>* SqliteProductSequencesRM::retrieveCharacterSequence(const std::string& sequence, const string& language)
{
	SqliteQuery<SqliteProductSequence>* query = new SqliteQuery<SqliteProductSequence>(*this);

	string filter = SqliteProductSequence::CHARACTERSEQUENCE_FIELD + " = \'" + sequence + "\' and " + SqliteProductSequence::CHARACTERSEQUENCELANGUAGE_FIELD + " = \'" + language + "\'";
	query->selectSome(filter, SqliteProductSequence::PRODUCTNAME_FIELD);

	return query;
}


DBProductSequenceI* SqliteProductSequencesRM::findProductSequence(const DBProductI* prod, const string& sequence, const string& language)
{
	const SqliteProduct* inrec = dynamic_cast<const SqliteProduct*>(prod);

	if ( inrec )
	{
#ifndef EVOL_NDEBUG
		Debug::trace("database.findSequence", 28, "Going to find sequence '" + sequence + "' in language '" + language + "' for product '" + inrec->getProductKey() + "'");
#endif

		std::vector<SqliteField> keys;
		keys.push_back(SqliteField(SqliteProductSequence::PRODUCTKEY_FIELD, SqliteField::INTEGER, inrec->getProductKey()));
		keys.push_back(SqliteField(SqliteProductSequence::CHARACTERSEQUENCELANGUAGE_FIELD, SqliteField::STRING, language));
		keys.push_back(SqliteField(SqliteProductSequence::CHARACTERSEQUENCE_FIELD, SqliteField::STRING, sequence));
		
		SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
		if ( findrec )
		{
#ifndef EVOL_NDEBUG
			Debug::trace("database.findSequence", 28, "Sequence found.");
#endif

			return new SqliteProductSequence(findrec);
		}
#ifndef EVOL_NDEBUG
		else
			Debug::trace("database.findSequence", 28, "Sequence not found.");
#endif
	}

	return 0;
}


DBProductSequenceI* SqliteProductSequencesRM::createRecord(const DBProductI* prod, const std::string& sequence, const string& language)
{
	const SqliteProduct* inrec = dynamic_cast<const SqliteProduct*>(prod);
	if ( inrec )
	{
		SqliteRecord* dbrec = SqliteRecordManager::create();

		if ( dbrec )
		{
			SqliteProductSequence* createrec = new SqliteProductSequence(dbrec);

			if ( createrec )
			{
				createrec->setProductKey(inrec->getProductKey());
				createrec->setCharacterSequenceLanguage(language);
				createrec->setCharacterSequence(sequence);
				return createrec;
			}
			else
				delete dbrec;
		}
	}

	return 0;
}


void SqliteProductSequencesRM::saveRecord(const DBProductSequenceI* rec)
{
	const SqliteProductSequence* inrec = dynamic_cast<const SqliteProductSequence*>(rec);

	if ( inrec )
	{
		if ( SqliteRecordManager::save(inrec->getRecord()) )
			modified();
		else
			throw edb::DBException(edb::DBException::SQLITE_SAVE_RECORD, "Unable to save record.");
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteProductSequencesRM::deleteRecord(const DBProductSequenceI* rec)
{
	const SqliteProductSequence* inrec = dynamic_cast<const SqliteProductSequence*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif





#ifdef SQLITEPRODUCTSRM_DRIVER

#include <iostream>

int main(int argc, char * argv[])
{
	unsigned long int tot;
	SqliteProductSequencesRM prods;

	tot = prods.countAll();
	if ( tot > 0 )
	{
		SqliteProductSequencesRM::iterator it = prods.begin();
		SqliteProductSequencesRM::iterator end = prods.end();

		while ( it != end )
			cout << *it;
	}
}

#endif // SQLITEPRODUCTSRM_DRIVER
