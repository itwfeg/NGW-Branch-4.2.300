#include <string>


#include "sqlitedatabase.h"
#include "sqlitetransactionsrm.h"
#include "sqlitetransaction.h"
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
template <> int SqliteQuery<SqliteTransaction>::_dataType_ = 0;
template <> string SqliteQuery<SqliteTransaction>::_dataTypeName_("SqliteQuery<SqliteTransaction>");

int SqliteTransactionsRM::_dataType_ = 0;
string SqliteTransactionsRM::_dataTypeName_("SqliteTransactionsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteTransactionsRM::DATA_FILENAME("Transactions");



SqliteTransactionsRM::SqliteTransactionsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBTransactionsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteTransaction::SCALEID_FIELD);
	columnNames_.insert(SqliteTransaction::TRANSACTIONID_FIELD);
	columnNames_.insert(SqliteTransaction::TRANSACTIONTYPE_FIELD);
	columnNames_.insert(SqliteTransaction::TRANSACTIONGROUP_FIELD);
	columnNames_.insert(SqliteTransaction::TRANSACTIONDATETIME_FIELD);
	columnNames_.insert(SqliteTransaction::TRANSACTIONVOID_FIELD);
	columnNames_.insert(SqliteTransaction::AFFECTSPRODUCTION_FIELD);
	columnNames_.insert(SqliteTransaction::READANDCLEAR_FIELD);
	columnNames_.insert(SqliteTransaction::NUMBERPACKAGES_FIELD);
	columnNames_.insert(SqliteTransaction::USEDSECURITYLABEL_FIELD);
	columnNames_.insert(SqliteTransaction::COOLFLAG_FIELD);
	columnNames_.insert(SqliteTransaction::TOTALVALUE_FIELD);
	columnNames_.insert(SqliteTransaction::OLDTOTALVALUE_FIELD);
	columnNames_.insert(SqliteTransaction::DEPTNUM_FIELD);
	columnNames_.insert(SqliteTransaction::PRODUCTNUMBER_FIELD);
	columnNames_.insert(SqliteTransaction::REPORTINGCLASS_FIELD);
	columnNames_.insert(SqliteTransaction::PRODUCTTYPE_FIELD);
	columnNames_.insert(SqliteTransaction::PRODUCTIONPLANNINGTYPE_FIELD);
	columnNames_.insert(SqliteTransaction::PRODUCTDESCRIPTION_FIELD);
	columnNames_.insert(SqliteTransaction::PRICEFACTOR_FIELD);
	columnNames_.insert(SqliteTransaction::BARCODEPREFIX_FIELD);
	columnNames_.insert(SqliteTransaction::BARCODENUMBER_FIELD);
	columnNames_.insert(SqliteTransaction::UNITPRICE_FIELD);
	columnNames_.insert(SqliteTransaction::EXCEPTIONPRICE_FIELD);
	columnNames_.insert(SqliteTransaction::FINALPRICE_FIELD);
	columnNames_.insert(SqliteTransaction::TARE_FIELD);
	columnNames_.insert(SqliteTransaction::PROPORTIONALTARE_FIELD);
	columnNames_.insert(SqliteTransaction::FINALTARE_FIELD);
	columnNames_.insert(SqliteTransaction::BYCOUNT_FIELD);
	columnNames_.insert(SqliteTransaction::FINALNETWEIGHT_FIELD);
	columnNames_.insert(SqliteTransaction::FIXEDWEIGHT_FIELD);
	columnNames_.insert(SqliteTransaction::SHELFLIFEDAYS_FIELD);
	columnNames_.insert(SqliteTransaction::SHELFLIFEHOURS_FIELD);
	columnNames_.insert(SqliteTransaction::SHELFLIFEMINUTES_FIELD);
	columnNames_.insert(SqliteTransaction::PRODUCTLIFEDAYS_FIELD);
	columnNames_.insert(SqliteTransaction::PRODUCTLIFEHOURS_FIELD);
	columnNames_.insert(SqliteTransaction::PRODUCTLIFEMINUTES_FIELD);
	columnNames_.insert(SqliteTransaction::LABEL1_FIELD);
	columnNames_.insert(SqliteTransaction::LABEL2_FIELD);
	columnNames_.insert(SqliteTransaction::LABEL3_FIELD);
	columnNames_.insert(SqliteTransaction::LABEL4_FIELD);
	columnNames_.insert(SqliteTransaction::OPERATORNUMBER_FIELD);
	columnNames_.insert(SqliteTransaction::OPERATORRFID_FIELD);
	columnNames_.insert(SqliteTransaction::OPERATORNAME_FIELD);
	columnNames_.insert(SqliteTransaction::OPERATORCHANGETYPE_FIELD);
	columnNames_.insert(SqliteTransaction::GRAPHIC1_FIELD);
	columnNames_.insert(SqliteTransaction::GRAPHIC2_FIELD);
	columnNames_.insert(SqliteTransaction::GRAPHIC3_FIELD);
	columnNames_.insert(SqliteTransaction::GRAPHIC4_FIELD);
	columnNames_.insert(SqliteTransaction::COOLTEXT_FIELD);
	columnNames_.insert(SqliteTransaction::COOLTRACKINGTEXT_FIELD);
	columnNames_.insert(SqliteTransaction::CUTTESTNUMBER_FIELD);
	columnNames_.insert(SqliteTransaction::PERCENTOFPRIMALWEIGHT_FIELD);
	columnNames_.insert(SqliteTransaction::PERCENTOFPRIMALCOST_FIELD);
	columnNames_.insert(SqliteTransaction::MODECHANGE_FIELD);

	columnNames_.insert(SqliteTransaction::OLDBARCODEDIGIT_FIELD);
	columnNames_.insert(SqliteTransaction::OLDUNITPRICE_FIELD);
	columnNames_.insert(SqliteTransaction::OLDFINALPRICE_FIELD);
	columnNames_.insert(SqliteTransaction::OLDTARE_FIELD);
	columnNames_.insert(SqliteTransaction::OLDBYCOUNT_FIELD);
	columnNames_.insert(SqliteTransaction::OLDFIXEDWEIGHT_FIELD);
	columnNames_.insert(SqliteTransaction::OLDSHELFLIFEDAYS_FIELD);
	columnNames_.insert(SqliteTransaction::OLDSHELFLIFEHOURS_FIELD);
	columnNames_.insert(SqliteTransaction::OLDSHELFLIFEMINUTES_FIELD);
	columnNames_.insert(SqliteTransaction::OLDPRODUCTLIFEDAYS_FIELD);
	columnNames_.insert(SqliteTransaction::OLDPRODUCTLIFEHOURS_FIELD);
	columnNames_.insert(SqliteTransaction::OLDPRODUCTLIFEMINUTES_FIELD );
	columnNames_.insert(SqliteTransaction::OLDLABEL1_FIELD);
	columnNames_.insert(SqliteTransaction::OLDLABEL2_FIELD);
	columnNames_.insert(SqliteTransaction::OLDLABEL3_FIELD);
	columnNames_.insert(SqliteTransaction::OLDLABEL4_FIELD);
	columnNames_.insert(SqliteTransaction::OLDGRAPHIC1_FIELD);
	columnNames_.insert(SqliteTransaction::OLDGRAPHIC2_FIELD);
	columnNames_.insert(SqliteTransaction::OLDGRAPHIC3_FIELD);
	columnNames_.insert(SqliteTransaction::OLDGRAPHIC4_FIELD);
	columnNames_.insert(SqliteTransaction::SWEETHEARTWEIGHT_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteTransaction::SCALEID_FIELD,                SqliteField(SqliteTransaction::SCALEID_FIELD,                SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::TRANSACTIONID_FIELD,          SqliteField(SqliteTransaction::TRANSACTIONID_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::TRANSACTIONTYPE_FIELD,        SqliteField(SqliteTransaction::TRANSACTIONTYPE_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::TRANSACTIONGROUP_FIELD,       SqliteField(SqliteTransaction::TRANSACTIONGROUP_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::TRANSACTIONDATETIME_FIELD,    SqliteField(SqliteTransaction::TRANSACTIONDATETIME_FIELD,    SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteTransaction::TRANSACTIONVOID_FIELD,        SqliteField(SqliteTransaction::TRANSACTIONVOID_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::AFFECTSPRODUCTION_FIELD,      SqliteField(SqliteTransaction::AFFECTSPRODUCTION_FIELD,      SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::READANDCLEAR_FIELD,           SqliteField(SqliteTransaction::READANDCLEAR_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::NUMBERPACKAGES_FIELD,         SqliteField(SqliteTransaction::NUMBERPACKAGES_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::USEDSECURITYLABEL_FIELD,      SqliteField(SqliteTransaction::USEDSECURITYLABEL_FIELD,      SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::COOLFLAG_FIELD,               SqliteField(SqliteTransaction::COOLFLAG_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::TOTALVALUE_FIELD,             SqliteField(SqliteTransaction::TOTALVALUE_FIELD,             SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::OLDTOTALVALUE_FIELD,          SqliteField(SqliteTransaction::OLDTOTALVALUE_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::DEPTNUM_FIELD,                SqliteField(SqliteTransaction::DEPTNUM_FIELD,                SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::PRODUCTNUMBER_FIELD,          SqliteField(SqliteTransaction::PRODUCTNUMBER_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::REPORTINGCLASS_FIELD,         SqliteField(SqliteTransaction::REPORTINGCLASS_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::PRODUCTTYPE_FIELD,            SqliteField(SqliteTransaction::PRODUCTTYPE_FIELD,            SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::PRODUCTIONPLANNINGTYPE_FIELD, SqliteField(SqliteTransaction::PRODUCTIONPLANNINGTYPE_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::PRODUCTDESCRIPTION_FIELD,     SqliteField(SqliteTransaction::PRODUCTDESCRIPTION_FIELD,     SqliteField::STRING)));
	columns_.insert(make_pair(SqliteTransaction::PRICEFACTOR_FIELD,            SqliteField(SqliteTransaction::PRICEFACTOR_FIELD,            SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::BARCODEPREFIX_FIELD,          SqliteField(SqliteTransaction::BARCODEPREFIX_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::BARCODENUMBER_FIELD,          SqliteField(SqliteTransaction::BARCODENUMBER_FIELD,          SqliteField::STRING)));
	columns_.insert(make_pair(SqliteTransaction::UNITPRICE_FIELD,              SqliteField(SqliteTransaction::UNITPRICE_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::EXCEPTIONPRICE_FIELD,         SqliteField(SqliteTransaction::EXCEPTIONPRICE_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::FINALPRICE_FIELD,             SqliteField(SqliteTransaction::FINALPRICE_FIELD,             SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::TARE_FIELD,                   SqliteField(SqliteTransaction::TARE_FIELD,                   SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::PROPORTIONALTARE_FIELD,       SqliteField(SqliteTransaction::PROPORTIONALTARE_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::FINALTARE_FIELD,              SqliteField(SqliteTransaction::FINALTARE_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::BYCOUNT_FIELD,                SqliteField(SqliteTransaction::BYCOUNT_FIELD,                SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::FINALNETWEIGHT_FIELD,         SqliteField(SqliteTransaction::FINALNETWEIGHT_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::FIXEDWEIGHT_FIELD,            SqliteField(SqliteTransaction::FIXEDWEIGHT_FIELD,            SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::SHELFLIFEDAYS_FIELD,          SqliteField(SqliteTransaction::SHELFLIFEDAYS_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::SHELFLIFEHOURS_FIELD,         SqliteField(SqliteTransaction::SHELFLIFEHOURS_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::SHELFLIFEMINUTES_FIELD,       SqliteField(SqliteTransaction::SHELFLIFEMINUTES_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::PRODUCTLIFEDAYS_FIELD,        SqliteField(SqliteTransaction::PRODUCTLIFEDAYS_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::PRODUCTLIFEHOURS_FIELD,       SqliteField(SqliteTransaction::PRODUCTLIFEHOURS_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::PRODUCTLIFEMINUTES_FIELD,     SqliteField(SqliteTransaction::PRODUCTLIFEMINUTES_FIELD,     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::LABEL1_FIELD,                 SqliteField(SqliteTransaction::LABEL1_FIELD,                 SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::LABEL2_FIELD,                 SqliteField(SqliteTransaction::LABEL2_FIELD,                 SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::LABEL3_FIELD,                 SqliteField(SqliteTransaction::LABEL3_FIELD,                 SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::LABEL4_FIELD,                 SqliteField(SqliteTransaction::LABEL4_FIELD,                 SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::OPERATORNUMBER_FIELD,         SqliteField(SqliteTransaction::OPERATORNUMBER_FIELD,         SqliteField::STRING)));
	columns_.insert(make_pair(SqliteTransaction::OPERATORRFID_FIELD,           SqliteField(SqliteTransaction::OPERATORRFID_FIELD,           SqliteField::STRING)));
	columns_.insert(make_pair(SqliteTransaction::OPERATORNAME_FIELD,           SqliteField(SqliteTransaction::OPERATORNAME_FIELD,           SqliteField::STRING)));
	columns_.insert(make_pair(SqliteTransaction::OPERATORCHANGETYPE_FIELD,     SqliteField(SqliteTransaction::OPERATORCHANGETYPE_FIELD,     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::GRAPHIC1_FIELD,               SqliteField(SqliteTransaction::GRAPHIC1_FIELD,               SqliteField::STRING)));
	columns_.insert(make_pair(SqliteTransaction::GRAPHIC2_FIELD,               SqliteField(SqliteTransaction::GRAPHIC2_FIELD,               SqliteField::STRING)));
	columns_.insert(make_pair(SqliteTransaction::GRAPHIC3_FIELD,               SqliteField(SqliteTransaction::GRAPHIC3_FIELD,               SqliteField::STRING)));
	columns_.insert(make_pair(SqliteTransaction::GRAPHIC4_FIELD,               SqliteField(SqliteTransaction::GRAPHIC4_FIELD,               SqliteField::STRING)));
	columns_.insert(make_pair(SqliteTransaction::COOLTEXT_FIELD,               SqliteField(SqliteTransaction::COOLTEXT_FIELD,               SqliteField::STRING)));
	columns_.insert(make_pair(SqliteTransaction::COOLTRACKINGTEXT_FIELD,       SqliteField(SqliteTransaction::COOLTRACKINGTEXT_FIELD,       SqliteField::STRING)));
	columns_.insert(make_pair(SqliteTransaction::CUTTESTNUMBER_FIELD,          SqliteField(SqliteTransaction::CUTTESTNUMBER_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::PERCENTOFPRIMALWEIGHT_FIELD,  SqliteField(SqliteTransaction::PERCENTOFPRIMALWEIGHT_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::PERCENTOFPRIMALCOST_FIELD,    SqliteField(SqliteTransaction::PERCENTOFPRIMALCOST_FIELD,    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::MODECHANGE_FIELD,             SqliteField(SqliteTransaction::MODECHANGE_FIELD,             SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::OLDBARCODEDIGIT_FIELD,        SqliteField(SqliteTransaction::OLDBARCODEDIGIT_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::OLDUNITPRICE_FIELD,           SqliteField(SqliteTransaction::OLDUNITPRICE_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::OLDFINALPRICE_FIELD,          SqliteField(SqliteTransaction::OLDFINALPRICE_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::OLDTARE_FIELD,                SqliteField(SqliteTransaction::OLDTARE_FIELD,                SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::OLDBYCOUNT_FIELD,             SqliteField(SqliteTransaction::OLDBYCOUNT_FIELD,             SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::OLDFIXEDWEIGHT_FIELD,         SqliteField(SqliteTransaction::OLDFIXEDWEIGHT_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::OLDSHELFLIFEDAYS_FIELD,       SqliteField(SqliteTransaction::OLDSHELFLIFEDAYS_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::OLDSHELFLIFEHOURS_FIELD,      SqliteField(SqliteTransaction::OLDSHELFLIFEHOURS_FIELD,      SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::OLDSHELFLIFEMINUTES_FIELD,    SqliteField(SqliteTransaction::OLDSHELFLIFEMINUTES_FIELD,    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::OLDPRODUCTLIFEDAYS_FIELD,     SqliteField(SqliteTransaction::OLDPRODUCTLIFEDAYS_FIELD,     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::OLDPRODUCTLIFEHOURS_FIELD,    SqliteField(SqliteTransaction::OLDPRODUCTLIFEHOURS_FIELD,    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::OLDPRODUCTLIFEMINUTES_FIELD,  SqliteField(SqliteTransaction::OLDPRODUCTLIFEMINUTES_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::OLDLABEL1_FIELD,              SqliteField(SqliteTransaction::OLDLABEL1_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::OLDLABEL2_FIELD,              SqliteField(SqliteTransaction::OLDLABEL2_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::OLDLABEL3_FIELD,              SqliteField(SqliteTransaction::OLDLABEL3_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::OLDLABEL4_FIELD,              SqliteField(SqliteTransaction::OLDLABEL4_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransaction::OLDGRAPHIC1_FIELD,            SqliteField(SqliteTransaction::OLDGRAPHIC1_FIELD,            SqliteField::STRING)));
	columns_.insert(make_pair(SqliteTransaction::OLDGRAPHIC2_FIELD,            SqliteField(SqliteTransaction::OLDGRAPHIC2_FIELD,            SqliteField::STRING)));
	columns_.insert(make_pair(SqliteTransaction::OLDGRAPHIC3_FIELD,            SqliteField(SqliteTransaction::OLDGRAPHIC3_FIELD,            SqliteField::STRING)));
	columns_.insert(make_pair(SqliteTransaction::OLDGRAPHIC4_FIELD,            SqliteField(SqliteTransaction::OLDGRAPHIC4_FIELD,            SqliteField::STRING)));
	columns_.insert(make_pair(SqliteTransaction::SWEETHEARTWEIGHT_FIELD,       SqliteField(SqliteTransaction::SWEETHEARTWEIGHT_FIELD,       SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteTransactionsRM::~SqliteTransactionsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBTransactionI>* SqliteTransactionsRM::retrieveAll()
{
	SqliteQuery<SqliteTransaction>* query = new SqliteQuery<SqliteTransaction>(*this);

	// Sort the transactions by their committed date/time
	query->selectAll(SqliteTransaction::TRANSACTIONDATETIME_FIELD);

	return query;
}


DBQueryI<DBTransactionI>* SqliteTransactionsRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteTransaction>* query = new SqliteQuery<SqliteTransaction>(*this);

	string filter = SqliteTransaction::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteTransaction::TRANSACTIONDATETIME_FIELD);

	return query;
}


DBQueryI<DBTransactionI>* SqliteTransactionsRM::retrieveReadAndClear(bool equal, int cleared, const std::string& deptNum)
{
	SqliteQuery<SqliteTransaction>* query = new SqliteQuery<SqliteTransaction>(*this);

	// Sort the transactions by their committed date/time
	// SOF-1865: dont filter by scale id, allow collected transactions to be seen
	//string filter(SqliteTransaction::SCALEID_FIELD + " = 0 and " + SqliteTransaction::READANDCLEAR_FIELD + (equal ? " = " : " != ") + util_string::inttostring(cleared));
	string filter(SqliteTransaction::READANDCLEAR_FIELD + (equal ? " = " : " != ") + util_string::inttostring(cleared));
	if (deptNum.size())
		filter.append(" and " + SqliteTransaction::DEPTNUM_FIELD + " = " + deptNum);
	query->selectSomeIndexed(filter, SqliteTransaction::TRANSACTIONDATETIME_FIELD);

	return query;
}


DBQueryI<DBTransactionI>* SqliteTransactionsRM::retrieveOperatorVoidable(const std::string& operatorId, int voidValue, const std::string& deptNum)
{
	SqliteQuery<SqliteTransaction>* query = new SqliteQuery<SqliteTransaction>(*this);

	string filter(SqliteTransaction::SCALEID_FIELD + " = 0");
	filter.append(" and " + SqliteTransaction::TRANSACTIONVOID_FIELD + " = " + util_string::inttostring(voidValue));
	if (deptNum.size())
		filter.append(" and " + SqliteTransaction::DEPTNUM_FIELD + " = " + deptNum);
	if ( operatorId.size() )
	{
		filter.append(" and " + SqliteTransaction::OPERATORNUMBER_FIELD + "= \'" + operatorId + "\'");
	}
	else
	{
		// FT-583 looking for a null or 0 (zero) operator id as per Larry
		filter.append(" and (");
		filter.append("(" + SqliteTransaction::OPERATORNUMBER_FIELD + " IS NULL)");
		filter.append(" or (" + SqliteTransaction::OPERATORNUMBER_FIELD + "= \'\')");
		filter.append(")");
	}

	// SOF - 4811: 0 items in transaction for by count after price change
	// Exclude Tracking Supervisor Changes - Changes
	filter.append(" and " + SqliteTransaction::MODECHANGE_FIELD + " = 0");

	// Sort the transactions by their reverse committed date/time
	query->selectSomeIndexed(filter, SqliteTransaction::TRANSACTIONDATETIME_FIELD, false);

	return query;
}


DBQueryI<DBTransactionI>* SqliteTransactionsRM::retrieveSupervisorVoidable(const std::string& operatorId, const std::string& deptNum, const std::string& pluNumber, const std::string& startTime, const std::string& endTime, int voidValue)
{
	SqliteQuery<SqliteTransaction>* query = new SqliteQuery<SqliteTransaction>(*this);

	string filter(SqliteTransaction::TRANSACTIONVOID_FIELD + " = " + util_string::inttostring(voidValue));
	filter.append(" and datetime('" + startTime + "') < " + SqliteTransaction::TRANSACTIONDATETIME_FIELD);  // starttime
	filter.append(" and " + SqliteTransaction::TRANSACTIONDATETIME_FIELD + " < datetime('" + endTime + "')");    // endtime
	if (deptNum.size())
		filter.append(" and " + SqliteTransaction::DEPTNUM_FIELD + " = " + deptNum);
	if ( pluNumber.size() )
		filter.append(" and " + SqliteTransaction::PRODUCTNUMBER_FIELD + " = " + pluNumber);
	if ( operatorId.size() )
	{
		filter.append(" and " + SqliteTransaction::OPERATORNUMBER_FIELD + " = \'" + operatorId + "\'");
	}
	
	// SOF - 4811: 0 items in transaction for by count after price change
	// Exclude Tracking Supervisor Changes - Changes
	filter.append(" and " + SqliteTransaction::MODECHANGE_FIELD + " = 0");

	// Sort the transactions by their committed date/time
	query->selectSomeIndexed(filter, SqliteTransaction::TRANSACTIONDATETIME_FIELD, false);

	return query;
}


DBQueryI<DBTransactionI>* SqliteTransactionsRM::retrieveFilteredTransactions(const std::string& startTime, const std::string& endTime, const std::string& transType, int voidValue,
																			const std::string& deptNum, const std::string& pluNumber, const std::string& cutTestNumber, const std::string& operatorId, bool defaultOrder)
{
	SqliteQuery<SqliteTransaction>* query = new SqliteQuery<SqliteTransaction>(*this);

	string filter("datetime('" + startTime + "') < " + SqliteTransaction::TRANSACTIONDATETIME_FIELD +
		" and " + SqliteTransaction::TRANSACTIONDATETIME_FIELD + " < datetime('" + endTime + "')"); //starttime endtime
	if ( transType.size() && util_string::stringtoint(transType) != 0 )
		filter.append(" and " + SqliteTransaction::TRANSACTIONTYPE_FIELD + " = " + transType);
	filter.append(" and " + SqliteTransaction::TRANSACTIONVOID_FIELD + " = " + util_string::inttostring(voidValue));
	if (deptNum.size())
		filter.append(" and " + SqliteTransaction::DEPTNUM_FIELD + " = " + deptNum);
	if ( pluNumber.size() )
		filter.append(" and " + SqliteTransaction::PRODUCTNUMBER_FIELD + " = " + pluNumber);
	if ( cutTestNumber.size() )
		filter.append(" and " + SqliteTransaction::CUTTESTNUMBER_FIELD + " = " + cutTestNumber);
	if ( operatorId.size() )
		filter.append(" and " + SqliteTransaction::OPERATORNUMBER_FIELD + " = \'" + operatorId + "\'");

	// Sort the transactions by their committed date/time
	query->selectSomeIndexed(filter, SqliteTransaction::TRANSACTIONDATETIME_FIELD, defaultOrder);

	return query;
}


DBQueryI<DBTransactionI>* SqliteTransactionsRM::retrieveAllCutTestTransactions()		//	AWL-135, HA-134
{
	SqliteQuery<SqliteTransaction>* query = new SqliteQuery<SqliteTransaction>(*this);

//	string filter( SqliteTransaction::CUTTESTNUMBER_FIELD + " > 0");
//	Doesn't work.	string filter( SqliteTransaction::CUTTESTNUMBER_FIELD + " IS NOTNULL");
	string filter( "(" + SqliteTransaction::CUTTESTNUMBER_FIELD + " IS NOT NULL) or " + SqliteTransaction::CUTTESTNUMBER_FIELD + " > 0"  );

	query->selectSomeIndexed(filter, SqliteTransaction::TRANSACTIONDATETIME_FIELD);

	return query;
}


DBQueryI<DBTransactionI>* SqliteTransactionsRM::retrieveCutTestTransactionsForCutTest(const std::string& cutTestNumber)
{
	SqliteQuery<SqliteTransaction>* query = new SqliteQuery<SqliteTransaction>(*this);

//	string filter( SqliteTransaction::CUTTESTNUMBER_FIELD + " > 0");
//	Doesn't work.	string filter( SqliteTransaction::CUTTESTNUMBER_FIELD + " IS NOTNULL");
	string filter( SqliteTransaction::CUTTESTNUMBER_FIELD + " = " + cutTestNumber );

	query->selectSomeIndexed(filter, SqliteTransaction::TRANSACTIONDATETIME_FIELD);

	return query;
}


DBTransactionI* SqliteTransactionsRM::findTransaction(const std::string& scaleId, const std::string& transId)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteTransaction::SCALEID_FIELD, SqliteField::INTEGER, scaleId));
	keys.push_back(SqliteField(SqliteTransaction::TRANSACTIONID_FIELD, SqliteField::INTEGER, transId));
		
	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
	if ( findrec )
		return new SqliteTransaction(findrec);

	return 0;
}


DBTransactionI* SqliteTransactionsRM::createRecord(const std::string& scaleId, const std::string& transId)
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteTransaction* createrec = new SqliteTransaction(dbrec);

		if ( createrec )
		{
			createrec->setScaleID(scaleId);
			if ( transId.size() )
				createrec->setTransactionID(transId);
			return createrec;
		}
		else
			delete dbrec;
	}

	return 0;
}


void SqliteTransactionsRM::saveRecord(const DBTransactionI* rec)
{
	const SqliteTransaction* inrec = dynamic_cast<const SqliteTransaction*>(rec);

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


int SqliteTransactionsRM::deleteRecord(const DBTransactionI* rec)
{
	const SqliteTransaction* inrec = dynamic_cast<const SqliteTransaction*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteTransactionsRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteTransaction::DEPTNUM_FIELD + " = " + deptNum);
}


std::string SqliteTransactionsRM::getNextTransactionId()
{
	if (Debug::isTracing("database.sql"))
		Debug::trace("database.sql", "SqliteTransactionsRM::getNextTransactionId - before calling getSemiautoIncrementedValue, autoIncField: " + autoIncrementField_);

	return getSemiautoIncrementedValue(autoIncrementField_);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
