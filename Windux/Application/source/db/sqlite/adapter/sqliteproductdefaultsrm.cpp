#include <string>


#include "sqlitedatabase.h"
#include "sqliteproductdefaultsrm.h"
#include "sqliteproductdefault.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteProductDefault>::_dataType_ = 0;
template <> string SqliteQuery<SqliteProductDefault>::_dataTypeName_("SqliteQuery<SqliteProductDefault>");

int SqliteProductDefaultsRM::_dataType_ = 0;
string SqliteProductDefaultsRM::_dataTypeName_("SqliteProductDefaultsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteProductDefaultsRM::DATA_FILENAME("ProductDefaults");



SqliteProductDefaultsRM::SqliteProductDefaultsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBProductDefaultsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteProductDefault::DEPTNUM_FIELD);
	columnNames_.insert(SqliteProductDefault::TYPE_FIELD);
	columnNames_.insert(SqliteProductDefault::PRICE_FIELD);
	columnNames_.insert(SqliteProductDefault::PRICEFACTOR_FIELD);
	columnNames_.insert(SqliteProductDefault::FORCEPRICE_FIELD);
	columnNames_.insert(SqliteProductDefault::EXCEPTIONPRICE_FIELD);
	columnNames_.insert(SqliteProductDefault::DISCOUNTMETHOD_FIELD);
	columnNames_.insert(SqliteProductDefault::DISCOUNTPRICE1_FIELD);
	columnNames_.insert(SqliteProductDefault::DISCOUNTPRICE2_FIELD);
	columnNames_.insert(SqliteProductDefault::DISCOUNTPRICE3_FIELD);
	columnNames_.insert(SqliteProductDefault::TARE_FIELD);
	columnNames_.insert(SqliteProductDefault::FORCETARE_FIELD);
	columnNames_.insert(SqliteProductDefault::PROPORTIONALTARE_FIELD);
	columnNames_.insert(SqliteProductDefault::NETWEIGHT_FIELD);
	columnNames_.insert(SqliteProductDefault::BYCOUNT_FIELD);
	columnNames_.insert(SqliteProductDefault::FORCEBYCOUNT_FIELD);
	columnNames_.insert(SqliteProductDefault::SHELFLIFEDAYS_FIELD);
	columnNames_.insert(SqliteProductDefault::SHELFLIFEHOURS_FIELD);		// Minutes at the end
	columnNames_.insert(SqliteProductDefault::PRODUCTLIFEDAYS_FIELD);
	columnNames_.insert(SqliteProductDefault::PRODUCTLIFEHOURS_FIELD);		// Minutes at the end
	columnNames_.insert(SqliteProductDefault::DONTPRINTSHELFLIFE_FIELD);
	columnNames_.insert(SqliteProductDefault::DONTPRINTPRODUCTLIFE_FIELD);
	columnNames_.insert(SqliteProductDefault::DONTPRINTPACKEDON_FIELD);
	columnNames_.insert(SqliteProductDefault::BONUSPOINTS_FIELD);
	columnNames_.insert(SqliteProductDefault::PORTIONS_FIELD);
	columnNames_.insert(SqliteProductDefault::APPLYSECURITYLABEL_FIELD);
	columnNames_.insert(SqliteProductDefault::INTAKEMODE_FIELD);
	columnNames_.insert(SqliteProductDefault::LABEL1ROTATION_FIELD);
	columnNames_.insert(SqliteProductDefault::LABEL1PLACEMENT_FIELD);
	columnNames_.insert(SqliteProductDefault::LABEL2ROTATION_FIELD);
	columnNames_.insert(SqliteProductDefault::LABEL2PLACEMENT_FIELD);
	columnNames_.insert(SqliteProductDefault::LABEL3ROTATION_FIELD);
	columnNames_.insert(SqliteProductDefault::LABEL3PLACEMENT_FIELD);
	columnNames_.insert(SqliteProductDefault::NAME1_FIELD);
	columnNames_.insert(SqliteProductDefault::NAME2_FIELD);
	columnNames_.insert(SqliteProductDefault::VERIFYCOOL_FIELD);
	columnNames_.insert(SqliteProductDefault::COOLREQUIRED_FIELD);
	columnNames_.insert(SqliteProductDefault::PRODUCTIONPLANNINGTYPE_FIELD);
	columnNames_.insert(SqliteProductDefault::REPORTINGCLASS_FIELD);
	columnNames_.insert(SqliteProductDefault::TEXT_FIELD);
	columnNames_.insert(SqliteProductDefault::SPECIALMESSAGE_FIELD);
	columnNames_.insert(SqliteProductDefault::NUTRITION_FIELD);
	columnNames_.insert(SqliteProductDefault::MARQUEE_FIELD);
	columnNames_.insert(SqliteProductDefault::NOTE_FIELD);
	columnNames_.insert(SqliteProductDefault::CATEGORY_FIELD);
	columnNames_.insert(SqliteProductDefault::LABEL1_FIELD);
	columnNames_.insert(SqliteProductDefault::LABEL2_FIELD);
	columnNames_.insert(SqliteProductDefault::LABEL3_FIELD);
	columnNames_.insert(SqliteProductDefault::SPECIALTYPE_FIELD);
	columnNames_.insert(SqliteProductDefault::COOLCATEGORYNUMBER_FIELD);
	columnNames_.insert(SqliteProductDefault::DEFAULTCOOLTEXTNUMBER_FIELD);
	columnNames_.insert(SqliteProductDefault::COOLPRETEXTNUMBER_FIELD);
	columnNames_.insert(SqliteProductDefault::COOLLISTNUMBER_FIELD);
	columnNames_.insert(SqliteProductDefault::UPCNUMBERSYSTEM_FIELD);
	columnNames_.insert(SqliteProductDefault::EANFORMATSTRING_FIELD);
	columnNames_.insert(SqliteProductDefault::EAN128CFORMATSTRING_FIELD);
	columnNames_.insert(SqliteProductDefault::GS1GTINFORMATSTRING_FIELD);
	columnNames_.insert(SqliteProductDefault::GS1AIFORMATSTRING_FIELD);
	columnNames_.insert(SqliteProductDefault::QRCODEFORMATSTRING_FIELD);
	columnNames_.insert(SqliteProductDefault::DATAMATRIXFORMATSTRING_FIELD);
	columnNames_.insert(SqliteProductDefault::EANPREFIX_FIELD);
	columnNames_.insert(SqliteProductDefault::DESCRIPTION_FIELD);
	columnNames_.insert(SqliteProductDefault::GRAPHIC1_FIELD);
	columnNames_.insert(SqliteProductDefault::GRAPHIC2_FIELD);
	columnNames_.insert(SqliteProductDefault::GRAPHIC3_FIELD);
	columnNames_.insert(SqliteProductDefault::GRAPHIC4_FIELD);
	columnNames_.insert(SqliteProductDefault::IMAGE_FIELD);
	columnNames_.insert(SqliteProductDefault::COOLTRACKINGNUMBER_FIELD);
	columnNames_.insert(SqliteProductDefault::MEALSIDEITEMSGROUPNUM_FIELD);
	columnNames_.insert(SqliteProductDefault::MEALSIDEITEMSAREFREE_FIELD);
	columnNames_.insert(SqliteProductDefault::MEALNUMSIDESINCLUDED_FIELD);
	columnNames_.insert(SqliteProductDefault::CUTTESTMEATSPECSHEETGROUPNUM_FIELD);
	columnNames_.insert(SqliteProductDefault::TAREB_FIELD);
	// For label types by mode project
	columnNames_.insert(SqliteProductDefault::MANUALLABELTYPENUM1_FIELD);
	columnNames_.insert(SqliteProductDefault::MANUALLABELTYPENUM2_FIELD);
	columnNames_.insert(SqliteProductDefault::MANUALLABELTYPENUM3_FIELD);
	columnNames_.insert(SqliteProductDefault::PREPACKLABELTYPENUM1_FIELD);
	columnNames_.insert(SqliteProductDefault::PREPACKLABELTYPENUM2_FIELD);
	columnNames_.insert(SqliteProductDefault::PREPACKLABELTYPENUM3_FIELD);
	columnNames_.insert(SqliteProductDefault::PRODENTRYLABELTYPENUM1_FIELD);
	columnNames_.insert(SqliteProductDefault::PRODENTRYLABELTYPENUM2_FIELD);
	columnNames_.insert(SqliteProductDefault::PRODENTRYLABELTYPENUM3_FIELD);
	columnNames_.insert(SqliteProductDefault::SELFSERVLABELTYPENUM1_FIELD);
	columnNames_.insert(SqliteProductDefault::SELFSERVLABELTYPENUM2_FIELD);
	columnNames_.insert(SqliteProductDefault::SELFSERVLABELTYPENUM3_FIELD);
	columnNames_.insert(SqliteProductDefault::PICK5LABELTYPENUM1_FIELD);
	columnNames_.insert(SqliteProductDefault::PICK5LABELTYPENUM2_FIELD);
	columnNames_.insert(SqliteProductDefault::PICK5LABELTYPENUM3_FIELD);
	columnNames_.insert(SqliteProductDefault::ONLINEORDERINGLABELTYPENUM1_FIELD);
	columnNames_.insert(SqliteProductDefault::ONLINEORDERINGLABELTYPENUM2_FIELD);
	columnNames_.insert(SqliteProductDefault::ONLINEORDERINGLABELTYPENUM3_FIELD);
	columnNames_.insert(SqliteProductDefault::SHELFLIFEMINUTES_FIELD);
	columnNames_.insert(SqliteProductDefault::PRODUCTLIFEMINUTES_FIELD);

	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteProductDefault::DEPTNUM_FIELD,                      SqliteField(SqliteProductDefault::DEPTNUM_FIELD,                      SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::TYPE_FIELD,                         SqliteField(SqliteProductDefault::TYPE_FIELD,                         SqliteField::INTEGER, 1)));  // random weight by default
	columns_.insert(make_pair(SqliteProductDefault::PRICE_FIELD,                        SqliteField(SqliteProductDefault::PRICE_FIELD,                        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::PRICEFACTOR_FIELD,                  SqliteField(SqliteProductDefault::PRICEFACTOR_FIELD,                  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::FORCEPRICE_FIELD,                   SqliteField(SqliteProductDefault::FORCEPRICE_FIELD,                   SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::EXCEPTIONPRICE_FIELD,               SqliteField(SqliteProductDefault::EXCEPTIONPRICE_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::DISCOUNTMETHOD_FIELD,               SqliteField(SqliteProductDefault::DISCOUNTMETHOD_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::DISCOUNTPRICE1_FIELD,               SqliteField(SqliteProductDefault::DISCOUNTPRICE1_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::DISCOUNTPRICE2_FIELD,               SqliteField(SqliteProductDefault::DISCOUNTPRICE2_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::DISCOUNTPRICE3_FIELD,               SqliteField(SqliteProductDefault::DISCOUNTPRICE3_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::TARE_FIELD,                         SqliteField(SqliteProductDefault::TARE_FIELD,                         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::FORCETARE_FIELD,                    SqliteField(SqliteProductDefault::FORCETARE_FIELD,                    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::PROPORTIONALTARE_FIELD,             SqliteField(SqliteProductDefault::PROPORTIONALTARE_FIELD,             SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::NETWEIGHT_FIELD,                    SqliteField(SqliteProductDefault::NETWEIGHT_FIELD,                    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::BYCOUNT_FIELD,                      SqliteField(SqliteProductDefault::BYCOUNT_FIELD,                      SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::FORCEBYCOUNT_FIELD,                 SqliteField(SqliteProductDefault::FORCEBYCOUNT_FIELD,                 SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::SHELFLIFEDAYS_FIELD,                SqliteField(SqliteProductDefault::SHELFLIFEDAYS_FIELD,                SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::SHELFLIFEHOURS_FIELD,               SqliteField(SqliteProductDefault::SHELFLIFEHOURS_FIELD,               SqliteField::INTEGER)));		// minutes at the end
	columns_.insert(make_pair(SqliteProductDefault::PRODUCTLIFEDAYS_FIELD,              SqliteField(SqliteProductDefault::PRODUCTLIFEDAYS_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::PRODUCTLIFEHOURS_FIELD,             SqliteField(SqliteProductDefault::PRODUCTLIFEHOURS_FIELD,             SqliteField::INTEGER)));		// minutes at the end
	columns_.insert(make_pair(SqliteProductDefault::DONTPRINTSHELFLIFE_FIELD,           SqliteField(SqliteProductDefault::DONTPRINTSHELFLIFE_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::DONTPRINTPRODUCTLIFE_FIELD,         SqliteField(SqliteProductDefault::DONTPRINTPRODUCTLIFE_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::DONTPRINTPACKEDON_FIELD,            SqliteField(SqliteProductDefault::DONTPRINTPACKEDON_FIELD,            SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::BONUSPOINTS_FIELD,                  SqliteField(SqliteProductDefault::BONUSPOINTS_FIELD,                  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::PORTIONS_FIELD,                     SqliteField(SqliteProductDefault::PORTIONS_FIELD,                     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::APPLYSECURITYLABEL_FIELD,           SqliteField(SqliteProductDefault::APPLYSECURITYLABEL_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::INTAKEMODE_FIELD,                   SqliteField(SqliteProductDefault::INTAKEMODE_FIELD,                   SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::LABEL1ROTATION_FIELD,               SqliteField(SqliteProductDefault::LABEL1ROTATION_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::LABEL1PLACEMENT_FIELD,              SqliteField(SqliteProductDefault::LABEL1PLACEMENT_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::LABEL2ROTATION_FIELD,               SqliteField(SqliteProductDefault::LABEL2ROTATION_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::LABEL2PLACEMENT_FIELD,              SqliteField(SqliteProductDefault::LABEL2PLACEMENT_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::LABEL3ROTATION_FIELD,               SqliteField(SqliteProductDefault::LABEL3ROTATION_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::LABEL3PLACEMENT_FIELD,              SqliteField(SqliteProductDefault::LABEL3PLACEMENT_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::NAME1_FIELD,                        SqliteField(SqliteProductDefault::NAME1_FIELD,                        SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductDefault::NAME2_FIELD,                        SqliteField(SqliteProductDefault::NAME2_FIELD,                        SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductDefault::VERIFYCOOL_FIELD,                   SqliteField(SqliteProductDefault::VERIFYCOOL_FIELD,                   SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::COOLREQUIRED_FIELD,                 SqliteField(SqliteProductDefault::COOLREQUIRED_FIELD,                 SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::PRODUCTIONPLANNINGTYPE_FIELD,       SqliteField(SqliteProductDefault::PRODUCTIONPLANNINGTYPE_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::REPORTINGCLASS_FIELD,               SqliteField(SqliteProductDefault::REPORTINGCLASS_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::TEXT_FIELD,                         SqliteField(SqliteProductDefault::TEXT_FIELD,                         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::SPECIALMESSAGE_FIELD,               SqliteField(SqliteProductDefault::SPECIALMESSAGE_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::NUTRITION_FIELD,                    SqliteField(SqliteProductDefault::NUTRITION_FIELD,                    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::MARQUEE_FIELD,                      SqliteField(SqliteProductDefault::MARQUEE_FIELD,                      SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::NOTE_FIELD,                         SqliteField(SqliteProductDefault::NOTE_FIELD,                         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::CATEGORY_FIELD,                     SqliteField(SqliteProductDefault::CATEGORY_FIELD,                     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::LABEL1_FIELD,                       SqliteField(SqliteProductDefault::LABEL1_FIELD,                       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::LABEL2_FIELD,                       SqliteField(SqliteProductDefault::LABEL2_FIELD,                       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::LABEL3_FIELD,                       SqliteField(SqliteProductDefault::LABEL3_FIELD,                       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::SPECIALTYPE_FIELD,                  SqliteField(SqliteProductDefault::SPECIALTYPE_FIELD,                  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::COOLCATEGORYNUMBER_FIELD,           SqliteField(SqliteProductDefault::COOLCATEGORYNUMBER_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::DEFAULTCOOLTEXTNUMBER_FIELD,        SqliteField(SqliteProductDefault::DEFAULTCOOLTEXTNUMBER_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::COOLPRETEXTNUMBER_FIELD,            SqliteField(SqliteProductDefault::COOLPRETEXTNUMBER_FIELD,            SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::COOLLISTNUMBER_FIELD,               SqliteField(SqliteProductDefault::COOLLISTNUMBER_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::UPCNUMBERSYSTEM_FIELD,              SqliteField(SqliteProductDefault::UPCNUMBERSYSTEM_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::EANFORMATSTRING_FIELD,              SqliteField(SqliteProductDefault::EANFORMATSTRING_FIELD,              SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductDefault::EAN128CFORMATSTRING_FIELD,          SqliteField(SqliteProductDefault::EAN128CFORMATSTRING_FIELD,          SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductDefault::GS1GTINFORMATSTRING_FIELD,          SqliteField(SqliteProductDefault::GS1GTINFORMATSTRING_FIELD,          SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductDefault::GS1AIFORMATSTRING_FIELD,            SqliteField(SqliteProductDefault::GS1AIFORMATSTRING_FIELD,            SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductDefault::QRCODEFORMATSTRING_FIELD,           SqliteField(SqliteProductDefault::QRCODEFORMATSTRING_FIELD,           SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductDefault::DATAMATRIXFORMATSTRING_FIELD,       SqliteField(SqliteProductDefault::DATAMATRIXFORMATSTRING_FIELD,       SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductDefault::EANPREFIX_FIELD,                    SqliteField(SqliteProductDefault::EANPREFIX_FIELD,                    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::DESCRIPTION_FIELD,                  SqliteField(SqliteProductDefault::DESCRIPTION_FIELD,                  SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductDefault::GRAPHIC1_FIELD,                     SqliteField(SqliteProductDefault::GRAPHIC1_FIELD,                     SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductDefault::GRAPHIC2_FIELD,                     SqliteField(SqliteProductDefault::GRAPHIC2_FIELD,                     SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductDefault::GRAPHIC3_FIELD,                     SqliteField(SqliteProductDefault::GRAPHIC3_FIELD,                     SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductDefault::GRAPHIC4_FIELD,                     SqliteField(SqliteProductDefault::GRAPHIC4_FIELD,                     SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductDefault::IMAGE_FIELD,                        SqliteField(SqliteProductDefault::IMAGE_FIELD,                        SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductDefault::COOLTRACKINGNUMBER_FIELD,           SqliteField(SqliteProductDefault::COOLTRACKINGNUMBER_FIELD,           SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductDefault::MEALSIDEITEMSGROUPNUM_FIELD,        SqliteField(SqliteProductDefault::MEALSIDEITEMSGROUPNUM_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::MEALSIDEITEMSAREFREE_FIELD,         SqliteField(SqliteProductDefault::MEALSIDEITEMSAREFREE_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::MEALNUMSIDESINCLUDED_FIELD,         SqliteField(SqliteProductDefault::MEALNUMSIDESINCLUDED_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::CUTTESTMEATSPECSHEETGROUPNUM_FIELD, SqliteField(SqliteProductDefault::CUTTESTMEATSPECSHEETGROUPNUM_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::TAREB_FIELD,                        SqliteField(SqliteProductDefault::TAREB_FIELD,                        SqliteField::INTEGER)));
	// For label types by mode project
	columns_.insert(make_pair(SqliteProductDefault::MANUALLABELTYPENUM1_FIELD,          SqliteField(SqliteProductDefault::MANUALLABELTYPENUM1_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::MANUALLABELTYPENUM2_FIELD,          SqliteField(SqliteProductDefault::MANUALLABELTYPENUM2_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::MANUALLABELTYPENUM3_FIELD,          SqliteField(SqliteProductDefault::MANUALLABELTYPENUM3_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::PREPACKLABELTYPENUM1_FIELD,         SqliteField(SqliteProductDefault::PREPACKLABELTYPENUM1_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::PREPACKLABELTYPENUM2_FIELD,         SqliteField(SqliteProductDefault::PREPACKLABELTYPENUM2_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::PREPACKLABELTYPENUM3_FIELD,         SqliteField(SqliteProductDefault::PREPACKLABELTYPENUM3_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::PRODENTRYLABELTYPENUM1_FIELD,       SqliteField(SqliteProductDefault::PRODENTRYLABELTYPENUM1_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::PRODENTRYLABELTYPENUM2_FIELD,       SqliteField(SqliteProductDefault::PRODENTRYLABELTYPENUM2_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::PRODENTRYLABELTYPENUM3_FIELD,       SqliteField(SqliteProductDefault::PRODENTRYLABELTYPENUM3_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::SELFSERVLABELTYPENUM1_FIELD,        SqliteField(SqliteProductDefault::SELFSERVLABELTYPENUM1_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::SELFSERVLABELTYPENUM2_FIELD,        SqliteField(SqliteProductDefault::SELFSERVLABELTYPENUM2_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::SELFSERVLABELTYPENUM3_FIELD,        SqliteField(SqliteProductDefault::SELFSERVLABELTYPENUM3_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::PICK5LABELTYPENUM1_FIELD,           SqliteField(SqliteProductDefault::PICK5LABELTYPENUM1_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::PICK5LABELTYPENUM2_FIELD,           SqliteField(SqliteProductDefault::PICK5LABELTYPENUM2_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::PICK5LABELTYPENUM3_FIELD,           SqliteField(SqliteProductDefault::PICK5LABELTYPENUM3_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::ONLINEORDERINGLABELTYPENUM1_FIELD,  SqliteField(SqliteProductDefault::ONLINEORDERINGLABELTYPENUM1_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::ONLINEORDERINGLABELTYPENUM2_FIELD,  SqliteField(SqliteProductDefault::ONLINEORDERINGLABELTYPENUM2_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::ONLINEORDERINGLABELTYPENUM3_FIELD,  SqliteField(SqliteProductDefault::ONLINEORDERINGLABELTYPENUM3_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::SHELFLIFEMINUTES_FIELD,             SqliteField(SqliteProductDefault::SHELFLIFEMINUTES_FIELD,             SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDefault::PRODUCTLIFEMINUTES_FIELD,           SqliteField(SqliteProductDefault::PRODUCTLIFEMINUTES_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(checkField_,                                              SqliteField(checkField_,                                              SqliteField::INTEGER, 1)));
}


SqliteProductDefaultsRM::~SqliteProductDefaultsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBProductDefaultI>* SqliteProductDefaultsRM::retrieveAll()
{
	SqliteQuery<SqliteProductDefault>* query = new SqliteQuery<SqliteProductDefault>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBProductDefaultI>* SqliteProductDefaultsRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteProductDefault>* query = new SqliteQuery<SqliteProductDefault>(*this);

	string filter = SqliteProductDefault::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteProductDefault::TYPE_FIELD);

	return query;
}


DBProductDefaultI* SqliteProductDefaultsRM::findType(const string& type, const std::string& deptNum)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteProductDefault::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));
	keys.push_back(SqliteField(SqliteProductDefault::TYPE_FIELD, SqliteField::INTEGER, type));

	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
	if ( findrec )
		return new SqliteProductDefault(findrec);

	return 0;
}


DBProductDefaultI* SqliteProductDefaultsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteProductDefault* createrec = new SqliteProductDefault(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteProductDefaultsRM::saveRecord(const DBProductDefaultI* rec)
{
	const SqliteProductDefault* inrec = dynamic_cast<const SqliteProductDefault*>(rec);

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


int SqliteProductDefaultsRM::deleteRecord(const DBProductDefaultI* rec)
{
	const SqliteProductDefault* inrec = dynamic_cast<const SqliteProductDefault*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteProductDefaultsRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteProductDefault::DEPTNUM_FIELD + " = " + deptNum);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif





#ifdef SQLITEPRODUCTSRM_DRIVER

#include <iostream>

int main(int argc, char * argv[])
{
	unsigned long int tot;
	SqliteProductDefaultsRM pdefs;

	tot = pdefs.countAll();
	if ( tot > 0 )
	{
		SqliteProductDefaultsRM::iterator it = pdefs.begin();
		SqliteProductDefaultsRM::iterator end = pdefs.end();

		while ( it != end )
			cout << *it;
	}
}

#endif // SQLITEPRODUCTSRM_DRIVER
