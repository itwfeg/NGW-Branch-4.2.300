#include <string>


#include "sqlitedatabase.h"
#include "sqliteproductsrm.h"
#include "sqliteproduct.h"
#include "sqliteclass.h"
#include "sqliteproducttext.h"
#include "sqlitenutrition.h"
#include "sqlitecategory.h"
#include "sqlitecoolcategory.h"
#include "sqlitecoolpretext.h"
#include "sqlitecooltext.h"
#include "sqlitecoollist.h"
#include "sqliteproductstatistic.h"
#include "sqlitespecialprogram.h"
#include "sqlitesoftkey.h"
#include "sqliteproductsequencesrm.h"
#include "sqliteproductsequence.h"
#include "sqliteproductproductsequencequery.h"
#include "sqliteignoreword.h"
#include "sqlitequery.h"
#include "Bool.h"
#include "ConfigurationAccess.h"

#include ACCOUNTANTI_INC
#include UTILSTRING_INC
//#include DEBUG_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteProduct>::_dataType_ = 0;
template <> string SqliteQuery<SqliteProduct>::_dataTypeName_("SqliteQuery<SqliteProduct>");

//template <> int SqliteNearestQuery<SqliteProduct>::_dataType_ = 0;
//template <> string SqliteNearestQuery<SqliteProduct>::_dataTypeName_("SqliteNearestQuery<SqliteProduct>");

int SqliteProductsRM::_dataType_ = 0;
string SqliteProductsRM::_dataTypeName_("SqliteProductsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteProductsRM::DATA_FILENAME("Products");

const string SqliteProductsRM::_longDescriptionLanguage_("-----");



SqliteProductsRM::SqliteProductsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBProductsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d), 
		  localeTransformedLength_(0), useShortDescriptionSequences_(false),
		  useLongDescriptionSequences_(false), useSequences_(false), minSequenceLength_(2),
		  maxSequenceLength_(4), productSequences_(new SqliteProductSequencesRM(sdb, props, 0)),
		  validateWords_(false), allowDigitsInSequences_(false), useIgnoreWords_(false),
		  ignoreWords_(new SqliteIgnoreWordsRM(sdb, props, 0))
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	retrieveProperties();

	columnNames_.insert(SqliteProduct::PRODUCTID_FIELD);
	columnNames_.insert(SqliteProduct::DEPTNUM_FIELD);
	columnNames_.insert(SqliteProduct::REPORTINGCLASS_FIELD);
	columnNames_.insert(SqliteProduct::NUTRITION_FIELD);
	columnNames_.insert(SqliteProduct::TEXT_FIELD);
	columnNames_.insert(SqliteProduct::SPECIALMESSAGE_FIELD);
	columnNames_.insert(SqliteProduct::NOTE_FIELD);
	columnNames_.insert(SqliteProduct::MARQUEE_FIELD);
	columnNames_.insert(SqliteProduct::PRODUCTTEXT5_FIELD);
	columnNames_.insert(SqliteProduct::PRODUCTTEXT6_FIELD);
	columnNames_.insert(SqliteProduct::PRODUCTTEXT7_FIELD);
	columnNames_.insert(SqliteProduct::PRODUCTTEXT8_FIELD);
	columnNames_.insert(SqliteProduct::PRODUCTTEXT9_FIELD);
	columnNames_.insert(SqliteProduct::LABEL1_FIELD);
	columnNames_.insert(SqliteProduct::LABEL2_FIELD);
	columnNames_.insert(SqliteProduct::LABEL3_FIELD);
	columnNames_.insert(SqliteProduct::CATEGORY_FIELD);
	columnNames_.insert(SqliteProduct::SPECIALNUMBER_FIELD);
	columnNames_.insert(SqliteProduct::COOLCATEGORY_FIELD);
	columnNames_.insert(SqliteProduct::COOLPRETEXT_FIELD);
	columnNames_.insert(SqliteProduct::CURRENTCOOLTEXT_FIELD);
	columnNames_.insert(SqliteProduct::DEFAULTCOOLTEXT_FIELD);
	columnNames_.insert(SqliteProduct::COOLLISTKEY_FIELD);
	columnNames_.insert(SqliteProduct::PRICE_FIELD);
	columnNames_.insert(SqliteProduct::PRICEFACTOR_FIELD);
	columnNames_.insert(SqliteProduct::FORCEPRICE_FIELD);
	columnNames_.insert(SqliteProduct::EXCEPTIONPRICE_FIELD);
	columnNames_.insert(SqliteProduct::DISCOUNTMETHOD_FIELD);
	columnNames_.insert(SqliteProduct::DISCOUNTPRICE1_FIELD);
	columnNames_.insert(SqliteProduct::DISCOUNTPRICE2_FIELD);
	columnNames_.insert(SqliteProduct::DISCOUNTPRICE3_FIELD);
	columnNames_.insert(SqliteProduct::TARE_FIELD);
	columnNames_.insert(SqliteProduct::FORCETARE_FIELD);
	columnNames_.insert(SqliteProduct::PROPORTIONALTARE_FIELD);
	columnNames_.insert(SqliteProduct::NETWEIGHT_FIELD);
	columnNames_.insert(SqliteProduct::BYCOUNT_FIELD);
	columnNames_.insert(SqliteProduct::FORCEBYCOUNT_FIELD);
	columnNames_.insert(SqliteProduct::SHELFLIFEDAYS_FIELD);
	columnNames_.insert(SqliteProduct::SHELFLIFEHOURS_FIELD);			// Minutes are at the end
	columnNames_.insert(SqliteProduct::PRODUCTLIFEDAYS_FIELD);
	columnNames_.insert(SqliteProduct::PRODUCTLIFEHOURS_FIELD);			// Minutes are at the end
	columnNames_.insert(SqliteProduct::DONTPRINTSHELFLIFE_FIELD);
	columnNames_.insert(SqliteProduct::DONTPRINTPRODUCTLIFE_FIELD);
	columnNames_.insert(SqliteProduct::DONTPRINTPACKEDON_FIELD);
	columnNames_.insert(SqliteProduct::BONUSPOINTS_FIELD);
	columnNames_.insert(SqliteProduct::PORTIONS_FIELD);
	columnNames_.insert(SqliteProduct::APPLYSECURITYLABEL_FIELD);
	columnNames_.insert(SqliteProduct::INTAKEMODE_FIELD);
	columnNames_.insert(SqliteProduct::LABEL1ROTATION_FIELD);
	columnNames_.insert(SqliteProduct::LABEL1PLACEMENT_FIELD);
	columnNames_.insert(SqliteProduct::LABEL2ROTATION_FIELD);
	columnNames_.insert(SqliteProduct::LABEL2PLACEMENT_FIELD);
	columnNames_.insert(SqliteProduct::LABEL3ROTATION_FIELD);
	columnNames_.insert(SqliteProduct::LABEL3PLACEMENT_FIELD);
	columnNames_.insert(SqliteProduct::NAME1_FIELD);
	columnNames_.insert(SqliteProduct::NAME2_FIELD);
	columnNames_.insert(SqliteProduct::FORCECOOLREVIEW_FIELD);
	columnNames_.insert(SqliteProduct::COOLREQUIRED_FIELD);
	columnNames_.insert(SqliteProduct::PRODUCTIONPLANNINGTYPE_FIELD);
	columnNames_.insert(SqliteProduct::PLU_FIELD);
	columnNames_.insert(SqliteProduct::TYPE_FIELD);
	columnNames_.insert(SqliteProduct::BARCODEPREFIX_FIELD);
	columnNames_.insert(SqliteProduct::BARCODEPRODUCTNUMBER_FIELD);
	columnNames_.insert(SqliteProduct::BARCODEVENDORNUMBER_FIELD);
	columnNames_.insert(SqliteProduct::BARCODEEANCODE_FIELD);
	columnNames_.insert(SqliteProduct::RAWBARCODENUMBER_FIELD);
	columnNames_.insert(SqliteProduct::CODEDNAME1_FIELD);
	columnNames_.insert(SqliteProduct::CODEDNAME2_FIELD);
	columnNames_.insert(SqliteProduct::SPECIALSTARTDATE_FIELD);
	columnNames_.insert(SqliteProduct::DESCRIPTION_FIELD);
	columnNames_.insert(SqliteProduct::GRAPHIC1_FIELD);
	columnNames_.insert(SqliteProduct::GRAPHIC2_FIELD);
	columnNames_.insert(SqliteProduct::GRAPHIC3_FIELD);
	columnNames_.insert(SqliteProduct::GRAPHIC4_FIELD);
	columnNames_.insert(SqliteProduct::IMAGE_FIELD);
	columnNames_.insert(SqliteProduct::COOLTRACKINGTEXT_FIELD);
	columnNames_.insert(SqliteProduct::MEALSIDEITEMSGROUPNUM_FIELD);
	columnNames_.insert(SqliteProduct::MEALSIDEITEMSAREFREE_FIELD);
	columnNames_.insert(SqliteProduct::MEALNUMSIDESINCLUDED_FIELD);
	columnNames_.insert(SqliteProduct::CUTTESTMEATSPECSHEETGROUPNUM_FIELD);
	columnNames_.insert(SqliteProduct::TAREB_FIELD);
	// For Pick-5 project
	columnNames_.insert(SqliteProduct::WEIGHTMIN_FIELD);
	columnNames_.insert(SqliteProduct::WEIGHTMAX_FIELD);
	columnNames_.insert(SqliteProduct::TOTALPRICEMIN_FIELD);
	columnNames_.insert(SqliteProduct::TOTALPRICEMAX_FIELD);
	// For label types by mode project
	columnNames_.insert(SqliteProduct::MANUALLABELTYPENUM1_FIELD);
	columnNames_.insert(SqliteProduct::MANUALLABELTYPENUM2_FIELD);
	columnNames_.insert(SqliteProduct::MANUALLABELTYPENUM3_FIELD);
	columnNames_.insert(SqliteProduct::PREPACKLABELTYPENUM1_FIELD);
	columnNames_.insert(SqliteProduct::PREPACKLABELTYPENUM2_FIELD);
	columnNames_.insert(SqliteProduct::PREPACKLABELTYPENUM3_FIELD);
	columnNames_.insert(SqliteProduct::PRODENTRYLABELTYPENUM1_FIELD);
	columnNames_.insert(SqliteProduct::PRODENTRYLABELTYPENUM2_FIELD);
	columnNames_.insert(SqliteProduct::PRODENTRYLABELTYPENUM3_FIELD);
	columnNames_.insert(SqliteProduct::SELFSERVLABELTYPENUM1_FIELD);
	columnNames_.insert(SqliteProduct::SELFSERVLABELTYPENUM2_FIELD);
	columnNames_.insert(SqliteProduct::SELFSERVLABELTYPENUM3_FIELD);
	columnNames_.insert(SqliteProduct::PICK5LABELTYPENUM1_FIELD);
	columnNames_.insert(SqliteProduct::PICK5LABELTYPENUM2_FIELD);
	columnNames_.insert(SqliteProduct::PICK5LABELTYPENUM3_FIELD);
	columnNames_.insert(SqliteProduct::ONLINEORDERINGLABELTYPENUM1_FIELD);
	columnNames_.insert(SqliteProduct::ONLINEORDERINGLABELTYPENUM2_FIELD);
	columnNames_.insert(SqliteProduct::ONLINEORDERINGLABELTYPENUM3_FIELD);
	columnNames_.insert(SqliteProduct::SHELFLIFEMINUTES_FIELD);
	columnNames_.insert(SqliteProduct::PRODUCTLIFEMINUTES_FIELD);


	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteProduct::PRODUCTID_FIELD,                    SqliteField(SqliteProduct::PRODUCTID_FIELD,                    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::DEPTNUM_FIELD,                      SqliteField(SqliteProduct::DEPTNUM_FIELD,                      SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::REPORTINGCLASS_FIELD,               SqliteField(SqliteProduct::REPORTINGCLASS_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::NUTRITION_FIELD,                    SqliteField(SqliteProduct::NUTRITION_FIELD,                    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::TEXT_FIELD,                         SqliteField(SqliteProduct::TEXT_FIELD,                         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::SPECIALMESSAGE_FIELD,               SqliteField(SqliteProduct::SPECIALMESSAGE_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::NOTE_FIELD,                         SqliteField(SqliteProduct::NOTE_FIELD,                         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::MARQUEE_FIELD,                      SqliteField(SqliteProduct::MARQUEE_FIELD,                      SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PRODUCTTEXT5_FIELD,                 SqliteField(SqliteProduct::PRODUCTTEXT5_FIELD,                 SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PRODUCTTEXT6_FIELD,                 SqliteField(SqliteProduct::PRODUCTTEXT6_FIELD,                 SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PRODUCTTEXT7_FIELD,                 SqliteField(SqliteProduct::PRODUCTTEXT7_FIELD,                 SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PRODUCTTEXT8_FIELD,                 SqliteField(SqliteProduct::PRODUCTTEXT8_FIELD,                 SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PRODUCTTEXT9_FIELD,                 SqliteField(SqliteProduct::PRODUCTTEXT9_FIELD,                 SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::LABEL1_FIELD,                       SqliteField(SqliteProduct::LABEL1_FIELD,                       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::LABEL2_FIELD,                       SqliteField(SqliteProduct::LABEL2_FIELD,                       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::LABEL3_FIELD,                       SqliteField(SqliteProduct::LABEL3_FIELD,                       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::CATEGORY_FIELD,                     SqliteField(SqliteProduct::CATEGORY_FIELD,                     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::SPECIALNUMBER_FIELD,                SqliteField(SqliteProduct::SPECIALNUMBER_FIELD,                SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::COOLCATEGORY_FIELD,                 SqliteField(SqliteProduct::COOLCATEGORY_FIELD,                 SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::COOLPRETEXT_FIELD,                  SqliteField(SqliteProduct::COOLPRETEXT_FIELD,                  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::CURRENTCOOLTEXT_FIELD,              SqliteField(SqliteProduct::CURRENTCOOLTEXT_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::DEFAULTCOOLTEXT_FIELD,              SqliteField(SqliteProduct::DEFAULTCOOLTEXT_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::COOLLISTKEY_FIELD,                  SqliteField(SqliteProduct::COOLLISTKEY_FIELD,                  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PRICE_FIELD,                        SqliteField(SqliteProduct::PRICE_FIELD,                        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PRICEFACTOR_FIELD,                  SqliteField(SqliteProduct::PRICEFACTOR_FIELD,                  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::FORCEPRICE_FIELD,                   SqliteField(SqliteProduct::FORCEPRICE_FIELD,                   SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::EXCEPTIONPRICE_FIELD,               SqliteField(SqliteProduct::EXCEPTIONPRICE_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::DISCOUNTMETHOD_FIELD,               SqliteField(SqliteProduct::DISCOUNTMETHOD_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::DISCOUNTPRICE1_FIELD,               SqliteField(SqliteProduct::DISCOUNTPRICE1_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::DISCOUNTPRICE2_FIELD,               SqliteField(SqliteProduct::DISCOUNTPRICE2_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::DISCOUNTPRICE3_FIELD,               SqliteField(SqliteProduct::DISCOUNTPRICE3_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::TARE_FIELD,                         SqliteField(SqliteProduct::TARE_FIELD,                         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::FORCETARE_FIELD,                    SqliteField(SqliteProduct::FORCETARE_FIELD,                    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PROPORTIONALTARE_FIELD,             SqliteField(SqliteProduct::PROPORTIONALTARE_FIELD,             SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::NETWEIGHT_FIELD,                    SqliteField(SqliteProduct::NETWEIGHT_FIELD,                    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::BYCOUNT_FIELD,                      SqliteField(SqliteProduct::BYCOUNT_FIELD,                      SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::FORCEBYCOUNT_FIELD,                 SqliteField(SqliteProduct::FORCEBYCOUNT_FIELD,                 SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::SHELFLIFEDAYS_FIELD,                SqliteField(SqliteProduct::SHELFLIFEDAYS_FIELD,                SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::SHELFLIFEHOURS_FIELD,               SqliteField(SqliteProduct::SHELFLIFEHOURS_FIELD,               SqliteField::INTEGER)));	// Minutes at the end
	columns_.insert(make_pair(SqliteProduct::PRODUCTLIFEDAYS_FIELD,              SqliteField(SqliteProduct::PRODUCTLIFEDAYS_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PRODUCTLIFEHOURS_FIELD,             SqliteField(SqliteProduct::PRODUCTLIFEHOURS_FIELD,             SqliteField::INTEGER)));	// Minutes at the end
	columns_.insert(make_pair(SqliteProduct::DONTPRINTSHELFLIFE_FIELD,           SqliteField(SqliteProduct::DONTPRINTSHELFLIFE_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::DONTPRINTPRODUCTLIFE_FIELD,         SqliteField(SqliteProduct::DONTPRINTPRODUCTLIFE_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::DONTPRINTPACKEDON_FIELD,            SqliteField(SqliteProduct::DONTPRINTPACKEDON_FIELD,            SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::BONUSPOINTS_FIELD,                  SqliteField(SqliteProduct::BONUSPOINTS_FIELD,                  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PORTIONS_FIELD,                     SqliteField(SqliteProduct::PORTIONS_FIELD,                     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::APPLYSECURITYLABEL_FIELD,           SqliteField(SqliteProduct::APPLYSECURITYLABEL_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::INTAKEMODE_FIELD,                   SqliteField(SqliteProduct::INTAKEMODE_FIELD,                   SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::LABEL1ROTATION_FIELD,               SqliteField(SqliteProduct::LABEL1ROTATION_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::LABEL1PLACEMENT_FIELD,              SqliteField(SqliteProduct::LABEL1PLACEMENT_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::LABEL2ROTATION_FIELD,               SqliteField(SqliteProduct::LABEL2ROTATION_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::LABEL2PLACEMENT_FIELD,              SqliteField(SqliteProduct::LABEL2PLACEMENT_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::LABEL3ROTATION_FIELD,               SqliteField(SqliteProduct::LABEL3ROTATION_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::LABEL3PLACEMENT_FIELD,              SqliteField(SqliteProduct::LABEL3PLACEMENT_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::NAME1_FIELD,                        SqliteField(SqliteProduct::NAME1_FIELD,                        SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProduct::NAME2_FIELD,                        SqliteField(SqliteProduct::NAME2_FIELD,                        SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProduct::FORCECOOLREVIEW_FIELD,              SqliteField(SqliteProduct::FORCECOOLREVIEW_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::COOLREQUIRED_FIELD,                 SqliteField(SqliteProduct::COOLREQUIRED_FIELD,                 SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PRODUCTIONPLANNINGTYPE_FIELD,       SqliteField(SqliteProduct::PRODUCTIONPLANNINGTYPE_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PLU_FIELD,                          SqliteField(SqliteProduct::PLU_FIELD,                          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::TYPE_FIELD,                         SqliteField(SqliteProduct::TYPE_FIELD,                         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::BARCODEPREFIX_FIELD,                SqliteField(SqliteProduct::BARCODEPREFIX_FIELD,                SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::BARCODEPRODUCTNUMBER_FIELD,         SqliteField(SqliteProduct::BARCODEPRODUCTNUMBER_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::BARCODEVENDORNUMBER_FIELD,          SqliteField(SqliteProduct::BARCODEVENDORNUMBER_FIELD,          SqliteField::INTEGER)));

	// Allow 10 digits for EAN code. For HEB Mexico demo.
	columns_.insert(make_pair(SqliteProduct::BARCODEEANCODE_FIELD,               SqliteField(SqliteProduct::BARCODEEANCODE_FIELD,               SqliteField::STRING)));

	columns_.insert(make_pair(SqliteProduct::RAWBARCODENUMBER_FIELD,             SqliteField(SqliteProduct::RAWBARCODENUMBER_FIELD,             SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProduct::CODEDNAME1_FIELD,                   SqliteField(SqliteProduct::CODEDNAME1_FIELD,                   SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProduct::CODEDNAME2_FIELD,                   SqliteField(SqliteProduct::CODEDNAME2_FIELD,                   SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProduct::SPECIALSTARTDATE_FIELD,             SqliteField(SqliteProduct::SPECIALSTARTDATE_FIELD,             SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteProduct::DESCRIPTION_FIELD,                  SqliteField(SqliteProduct::DESCRIPTION_FIELD,                  SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProduct::GRAPHIC1_FIELD,                     SqliteField(SqliteProduct::GRAPHIC1_FIELD,                     SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProduct::GRAPHIC2_FIELD,                     SqliteField(SqliteProduct::GRAPHIC2_FIELD,                     SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProduct::GRAPHIC3_FIELD,                     SqliteField(SqliteProduct::GRAPHIC3_FIELD,                     SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProduct::GRAPHIC4_FIELD,                     SqliteField(SqliteProduct::GRAPHIC4_FIELD,                     SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProduct::IMAGE_FIELD,                        SqliteField(SqliteProduct::IMAGE_FIELD,                        SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProduct::COOLTRACKINGTEXT_FIELD,             SqliteField(SqliteProduct::COOLTRACKINGTEXT_FIELD,             SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProduct::MEALSIDEITEMSGROUPNUM_FIELD,        SqliteField(SqliteProduct::MEALSIDEITEMSGROUPNUM_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::MEALSIDEITEMSAREFREE_FIELD,         SqliteField(SqliteProduct::MEALSIDEITEMSAREFREE_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::MEALNUMSIDESINCLUDED_FIELD,         SqliteField(SqliteProduct::MEALNUMSIDESINCLUDED_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::CUTTESTMEATSPECSHEETGROUPNUM_FIELD, SqliteField(SqliteProduct::CUTTESTMEATSPECSHEETGROUPNUM_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::TAREB_FIELD,                        SqliteField(SqliteProduct::TAREB_FIELD,                        SqliteField::INTEGER)));
	// For Pick-5 project
	columns_.insert(make_pair(SqliteProduct::WEIGHTMIN_FIELD,                    SqliteField(SqliteProduct::WEIGHTMIN_FIELD,                    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::WEIGHTMAX_FIELD,                    SqliteField(SqliteProduct::WEIGHTMAX_FIELD,                    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::TOTALPRICEMIN_FIELD,                SqliteField(SqliteProduct::TOTALPRICEMIN_FIELD,                SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::TOTALPRICEMAX_FIELD,                SqliteField(SqliteProduct::TOTALPRICEMAX_FIELD,                SqliteField::INTEGER)));
	// For label types by mode project
	columns_.insert(make_pair(SqliteProduct::MANUALLABELTYPENUM1_FIELD,          SqliteField(SqliteProduct::MANUALLABELTYPENUM1_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::MANUALLABELTYPENUM2_FIELD,          SqliteField(SqliteProduct::MANUALLABELTYPENUM2_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::MANUALLABELTYPENUM3_FIELD,          SqliteField(SqliteProduct::MANUALLABELTYPENUM3_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PREPACKLABELTYPENUM1_FIELD,         SqliteField(SqliteProduct::PREPACKLABELTYPENUM1_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PREPACKLABELTYPENUM2_FIELD,         SqliteField(SqliteProduct::PREPACKLABELTYPENUM2_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PREPACKLABELTYPENUM3_FIELD,         SqliteField(SqliteProduct::PREPACKLABELTYPENUM3_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PRODENTRYLABELTYPENUM1_FIELD,       SqliteField(SqliteProduct::PRODENTRYLABELTYPENUM1_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PRODENTRYLABELTYPENUM2_FIELD,       SqliteField(SqliteProduct::PRODENTRYLABELTYPENUM2_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PRODENTRYLABELTYPENUM3_FIELD,       SqliteField(SqliteProduct::PRODENTRYLABELTYPENUM3_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::SELFSERVLABELTYPENUM1_FIELD,        SqliteField(SqliteProduct::SELFSERVLABELTYPENUM1_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::SELFSERVLABELTYPENUM2_FIELD,        SqliteField(SqliteProduct::SELFSERVLABELTYPENUM2_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::SELFSERVLABELTYPENUM3_FIELD,        SqliteField(SqliteProduct::SELFSERVLABELTYPENUM3_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PICK5LABELTYPENUM1_FIELD,           SqliteField(SqliteProduct::PICK5LABELTYPENUM1_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PICK5LABELTYPENUM2_FIELD,           SqliteField(SqliteProduct::PICK5LABELTYPENUM2_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PICK5LABELTYPENUM3_FIELD,           SqliteField(SqliteProduct::PICK5LABELTYPENUM3_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::ONLINEORDERINGLABELTYPENUM1_FIELD,  SqliteField(SqliteProduct::ONLINEORDERINGLABELTYPENUM1_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::ONLINEORDERINGLABELTYPENUM2_FIELD,  SqliteField(SqliteProduct::ONLINEORDERINGLABELTYPENUM2_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::ONLINEORDERINGLABELTYPENUM3_FIELD,  SqliteField(SqliteProduct::ONLINEORDERINGLABELTYPENUM3_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::SHELFLIFEMINUTES_FIELD,             SqliteField(SqliteProduct::SHELFLIFEMINUTES_FIELD,             SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProduct::PRODUCTLIFEMINUTES_FIELD,           SqliteField(SqliteProduct::PRODUCTLIFEMINUTES_FIELD,           SqliteField::INTEGER)));

	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteProductsRM::~SqliteProductsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
	{
		delete decorated_;
        decorated_ = NULL;
    }

	if ( productSequences_ )
	{
		delete productSequences_;
        productSequences_ = NULL;
    }

	if ( ignoreWords_ )
	{
		delete ignoreWords_;
        ignoreWords_ = NULL;
    }
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveAll()
{
	SqliteQuery<SqliteProduct>* query = new SqliteQuery<SqliteProduct>(*this);

	// Sort the products by PLU number
	query->selectAll(SqliteProduct::PLU_FIELD);

	return query;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteProduct>* query = new SqliteQuery<SqliteProduct>(*this);

	string filter = SqliteProduct::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteProduct::PLU_FIELD);

	return query;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveRangePLU(const std::string& first, const std::string& last, const std::string& deptNum)
{
	SqliteQuery<SqliteProduct>* query = new SqliteQuery<SqliteProduct>(*this);

	// Sort the products by PLU number
	string filter = SqliteProduct::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteProduct::PLU_FIELD + " >= " + first + " and " + SqliteProduct::PLU_FIELD + " <= " + last;
	query->selectSome(filter, SqliteProduct::PLU_FIELD);

	return query;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveNearestPLU(const DBProductI* prod, int count)
{
	const SqliteProduct* product = dynamic_cast<const SqliteProduct*>(prod);
	if ( product )
		return retrieveNearestPLU(product->getPLU(), product->getDeptNumber(), count);

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveNearestPLU(const std::string& plu, const std::string& deptNum, int count)
{
	SqliteQuery<SqliteProduct>* query = new SqliteQuery<SqliteProduct>(*this);
	if (query)
	{
		int halfRange = count;
		// Sort the products by PLU number
		string sql = "select " + SqliteProduct::PLU_FIELD +
						" from " + this->getTableName() +
						" where " + SqliteProduct::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteProduct::PLU_FIELD + " < " + plu +
						" order by " + SqliteProduct::PLU_FIELD + " desc " +
						" limit " + inttostring(halfRange);
		query->selectUsingQuery(sql);

		string firstPluNum = "1";

		DBProductI* p = query->first();
		while (p && halfRange)
		{
			firstPluNum = p->getPLU();
			halfRange--;
			p = query->next();
		}

		delete query;
		query = new SqliteQuery<SqliteProduct>(*this);

		// Determined the new beginning point for the real query to be returned to the caller.
		string filter = SqliteProduct::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteProduct::PLU_FIELD + " >= " + firstPluNum;
		query->selectSome(filter, SqliteProduct::PLU_FIELD, count*2 - halfRange + 1);
	}
	return query;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveKeywordInName(const std::string& keyword)
{
	if ( useSequences_ && productSequences_ )
	{
		SqliteProductProductSequenceQuery* query = new SqliteProductProductSequenceQuery(sdb_);

		if ( query )
		{
			std::string transformedKeyword;
			std::string reverseLocale("");
			locale currentLocale = locale();

#ifndef EVOL_NDEBUG
			Debug::trace("database.search", 20, "Going to retrieve products with keyword \'" + 
					keyword + "\' (\'" + util_string::stringtocodes(keyword) + 
					"\') in name field using locale " + currentLocale.name());
#endif
			util_platform::Platform::getInstance().reverseLocaleString(reverseLocale, currentLocale.name());

			util_string::localeTransform(transformedKeyword, keyword, currentLocale);

#if defined (__linux__)
			transformedKeyword.erase(keyword.size()*2);
			transformedKeyword.erase(keyword.size());
#else
			transformedKeyword.erase(keyword.size()*2);
#endif


#ifndef EVOL_NDEBUG
			Debug::trace("database.search", 30, "The keyword \'" + keyword +
					"\' was transformed by the current locale into \'" + transformedKeyword +
					"\' (\'" + util_string::stringtocodes(transformedKeyword) + "\')");
#endif

			query->selectSequenceLanguage(transformedKeyword, reverseLocale);

			return query;
		}
	}

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveKeywordInDescription(const std::string& keyword, const std::string& deptNum)
{
	if ( useSequences_ && productSequences_ )
	{
		SqliteProductProductSequenceQuery* query = new SqliteProductProductSequenceQuery(sdb_);
		if ( query )
		{
			std::string transformedKeyword;
			std::string reverseLocale("");
			locale currentLocale = locale();

			util_platform::Platform::getInstance().reverseLocaleString(reverseLocale, currentLocale.name());

			util_string::localeTransform(transformedKeyword, keyword, currentLocale);
			
#if defined (__linux__)
			transformedKeyword.erase(keyword.size()*2);
			transformedKeyword.erase(keyword.size());
#else
			transformedKeyword.erase(keyword.size()*2);
#endif


#ifndef EVOL_NDEBUG
			Debug::trace("database.search", 30, "The keyword \'" + keyword +
					"\' was transformed by the current locale into \'" + transformedKeyword +
					"\' (\'" + util_string::stringtocodes(transformedKeyword) + "\')");
#endif

			query->selectSequenceLanguage(transformedKeyword, reverseLocale);
		}

		return query;
	}
	else
	{
		SqliteQuery<SqliteProduct>* query = new SqliteQuery<SqliteProduct>(*this);
		string upperKeyword = keyword;
		//upperKeyword = util_string::toupper(upperKeyword);

		string sql = "select ";
		sql.append(SqliteProduct::PLU_FIELD + ", " + SqliteProduct::DESCRIPTION_FIELD + ", " + SqliteProduct::NAME1_FIELD + ", " + SqliteProduct::NAME2_FIELD + ", " + SqliteProduct::PRICE_FIELD + ", " + SqliteProduct::IMAGE_FIELD);
		sql.append(" from " + tableFilename_);
		//sql.append(" where upper(" + SqliteProduct::DESCRIPTION_FIELD + ") like '%" + upperKeyword + "%'");
		sql.append(" where " + SqliteProduct::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteProduct::DESCRIPTION_FIELD + " like '%" + upperKeyword + "%'");
		sql.append(" order by " + SqliteProduct::PLU_FIELD);

		query->selectUsingQuery(sql);

		return query;
	}
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveNameStartsWith(const std::string& keyword, LanguageSelection langNum, const std::string& deptNum)
{
	string whichName = ( langNum == PrimaryLanguage ) ? SqliteProduct::NAME1_FIELD : SqliteProduct::NAME2_FIELD;
	return retrieveTextColumnStartsWith(keyword, whichName, "", deptNum);
}

DBQueryI<DBProductI>* SqliteProductsRM::retrieveDescriptionStartsWith(const std::string& keyword, const std::string& deptNum)
{
	return retrieveTextColumnStartsWith(keyword, SqliteProduct::DESCRIPTION_FIELD, "", deptNum);
}

DBQueryI<DBProductI>* SqliteProductsRM::retrieveDescriptionOrNameStartsWith(const std::string& keyword, LanguageSelection langNum, const std::string& deptNum)
{
	string whichName = ( langNum == PrimaryLanguage ) ? SqliteProduct::NAME1_FIELD : SqliteProduct::NAME2_FIELD;
	return retrieveTextColumnStartsWith(keyword, SqliteProduct::DESCRIPTION_FIELD, whichName, deptNum);
}

DBQueryI<DBProductI>* SqliteProductsRM::retrieveTextColumnStartsWith(const std::string& keyword, const std::string& columnName, const std::string& columnName2, const std::string& deptNum)
{
	if ( useSequences_ && productSequences_ )
	{
		// TODO
		// Not implemented! Do we need this?
	}
	else
	{
		SqliteQuery<SqliteProduct>* query = new SqliteQuery<SqliteProduct>(*this);
		string upperKeyword = keyword;
		//upperKeyword = util_string::toupper(upperKeyword);

		string sql;
		/*
		try
		{
			sql = ConfigurationAccess::getInstance()->toStringCfgI(CfgInternal::SELFSERVEALPHABETICSEARCHQUERY)
		}
		catch (...)
		{
			continue;
		}
		*/

		if ( sql.empty() )
		{
			sql = "select ";
			sql.append(SqliteProduct::PLU_FIELD + ", " + SqliteProduct::DESCRIPTION_FIELD + ", " + SqliteProduct::NAME1_FIELD + ", " + SqliteProduct::NAME2_FIELD + ", " + SqliteProduct::PRICE_FIELD + ", " + SqliteProduct::IMAGE_FIELD);
			sql.append(" from " + tableFilename_);
			//sql.append(" where upper(" + columnName + ") like '%" + upperKeyword + "%'");
			sql.append(" where " + SqliteProduct::DEPTNUM_FIELD + " = " + deptNum + " and ( " + columnName + " like '" + upperKeyword + "%'");				// space in front of the character (search 'a' in 'Apple')
			sql.append(" or " + columnName + " like '% " + upperKeyword + "%'");				// space in front of the character (search 'a' in 'Gala Apple')
			sql.append(" or " + columnName + " like '%-" + upperKeyword + "%'");				// hyphen (dash) in front of the character (search 'a' in 'Gala-Apple')
			sql.append(" or " + columnName + " like '%'||X'09'||'" + upperKeyword + "%'");		// tab in front of the character (search 'a' in 'Gala	Apple')
			sql.append(" or " + columnName + " like '%'||X'0A'||'" + upperKeyword + "%'");		// linefeed in front of the character (search 'a' in 'Gala<nl>Apple')
			sql.append(" or " + columnName + " like '%'||X'0D'||'" + upperKeyword + "%'");		// carriage return in front of the character (search 'a' in 'Gala<cr>Apple')
			sql.append(" or " + columnName + " like '%'||X'1B'||'" + upperKeyword + "%'");		// Escape character - indicating the end of a font sequence - in front of the character

			if (!columnName2.empty())
			{
				sql.append(" or " + columnName2 + " like '" + upperKeyword + "%'");					// space in front of the character (search 'a' in 'Apple')
				sql.append(" or " + columnName2 + " like '% " + upperKeyword + "%'");				// space in front of the character (search 'a' in 'Gala Apple')
				sql.append(" or " + columnName2 + " like '%-" + upperKeyword + "%'");				// hyphen (dash) in front of the character (search 'a' in 'Gala-Apple')
				sql.append(" or " + columnName2 + " like '%'||X'09'||'" + upperKeyword + "%'");		// tab in front of the character (search 'a' in 'Gala	Apple')
				sql.append(" or " + columnName2 + " like '%'||X'0A'||'" + upperKeyword + "%'");		// linefeed in front of the character (search 'a' in 'Gala<nl>Apple')
				sql.append(" or " + columnName2 + " like '%'||X'0D'||'" + upperKeyword + "%'");		// carriage return in front of the character (search 'a' in 'Gala<cr>Apple')
				sql.append(" or " + columnName2 + " like '%'||X'1B'||'" + upperKeyword + "%'");		// Escape character - indicating the end of a font sequence - in front of the character
			}

			sql.append(" )");

			// We're not going to support the following for now, but we could uncomment these for additional possibilities,
			// or pass the entire query through an internal config parameter, selfServeAlphabeticSearchQuery
			//sql.append(" or " + columnName + " like '%," + upperKeyword + "%'");			// comma in front of the character (search 'a' in 'Gala,Apple')
			//sql.append(" or " + columnName + " like '%;" + upperKeyword + "%'");			// semi-colon in front of the character (search 'a' in 'Gala;Apple')
			//sql.append(" or " + columnName + " like '%." + upperKeyword + "%'");			// period in front of the character (search 'a' in 'Gala.Apple')
			//sql.append(" or " + columnName + " like '%_" + upperKeyword + "%'");			// underscore in front of the character (search 'a' in 'Gala_Apple')
			//sql.append(" or " + columnName + " like '%'||X'27'||'" + upperKeyword + "%'");	// single quote in front of the character (search 'a' in 'Gala 'Apple'')
			//sql.append(" or " + columnName + " like '%'||X'22'||'" + upperKeyword + "%'");	// double quote in front of the character (search 'a' in 'Gala "Apple"')

			sql.append(" order by " + columnName);
		}

		query->selectUsingQuery(sql);

		return query;
	}

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveLinkedtoClass(const string& classNumber, const string& deptNum)
{
	return retrieveLinkedtoRec(SqliteProduct::REPORTINGCLASS_FIELD, classNumber, SqliteProduct::DEPTNUM_FIELD, deptNum);
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveLinkedtoClass(const DBClassI* clas)
{
	const SqliteClass* inrec = dynamic_cast<const SqliteClass*>(clas);

	if ( inrec )
		return retrieveLinkedtoRec(SqliteProduct::REPORTINGCLASS_FIELD, inrec->getNumber(), SqliteProduct::DEPTNUM_FIELD, inrec->getDeptNumber());

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveLinkedtoNutrition(const DBNutritionI* nutrition)
{
	const SqliteNutrition* inrec = dynamic_cast<const SqliteNutrition*>(nutrition);

	if ( inrec )
		return retrieveLinkedtoRec(SqliteProduct::NUTRITION_FIELD, inrec->getNumber(), SqliteProduct::DEPTNUM_FIELD, inrec->getDeptNumber());

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveLinkedtoText(const DBProductTextI* text)
{
	const SqliteProductText* inrec = dynamic_cast<const SqliteProductText*>(text);

	if ( inrec )
		return retrieveLinkedtoRec(SqliteProduct::TEXT_FIELD, inrec->getNumber(), SqliteProduct::DEPTNUM_FIELD, inrec->getDeptNumber());

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveLinkedtoMessage(const DBProductTextI* message)
{
	const SqliteProductText* inrec = dynamic_cast<const SqliteProductText*>(message);

	if ( inrec )
		return retrieveLinkedtoRec(SqliteProduct::SPECIALMESSAGE_FIELD, inrec->getNumber(), SqliteProduct::DEPTNUM_FIELD, inrec->getDeptNumber());

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveLinkedtoNote(const DBProductTextI* note)
{
	const SqliteProductText* inrec = dynamic_cast<const SqliteProductText*>(note);

	if ( inrec )
		return retrieveLinkedtoRec(SqliteProduct::NOTE_FIELD, inrec->getNumber(), SqliteProduct::DEPTNUM_FIELD, inrec->getDeptNumber());

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveLinkedtoMarquee(const DBProductTextI* marquee)
{
	const SqliteProductText* inrec = dynamic_cast<const SqliteProductText*>(marquee);

	if ( inrec )
		return retrieveLinkedtoRec(SqliteProduct::MARQUEE_FIELD, inrec->getNumber(), SqliteProduct::DEPTNUM_FIELD, inrec->getDeptNumber());

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveLinkedtoProductText5(const DBProductTextI* prodText)
{
	const SqliteProductText* inrec = dynamic_cast<const SqliteProductText*>(prodText);

	if ( inrec )
		return retrieveLinkedtoRec(SqliteProduct::PRODUCTTEXT5_FIELD, inrec->getNumber(), SqliteProduct::DEPTNUM_FIELD, inrec->getDeptNumber());

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveLinkedtoProductText6(const DBProductTextI* prodText)
{
	const SqliteProductText* inrec = dynamic_cast<const SqliteProductText*>(prodText);

	if ( inrec )
		return retrieveLinkedtoRec(SqliteProduct::PRODUCTTEXT6_FIELD, inrec->getNumber(), SqliteProduct::DEPTNUM_FIELD, inrec->getDeptNumber());

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveLinkedtoProductText7(const DBProductTextI* prodText)
{
	const SqliteProductText* inrec = dynamic_cast<const SqliteProductText*>(prodText);

	if ( inrec )
		return retrieveLinkedtoRec(SqliteProduct::PRODUCTTEXT7_FIELD, inrec->getNumber(), SqliteProduct::DEPTNUM_FIELD, inrec->getDeptNumber());

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveLinkedtoProductText8(const DBProductTextI* prodText)
{
	const SqliteProductText* inrec = dynamic_cast<const SqliteProductText*>(prodText);

	if ( inrec )
		return retrieveLinkedtoRec(SqliteProduct::PRODUCTTEXT8_FIELD, inrec->getNumber(), SqliteProduct::DEPTNUM_FIELD, inrec->getDeptNumber());

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveLinkedtoProductText9(const DBProductTextI* prodText)
{
	const SqliteProductText* inrec = dynamic_cast<const SqliteProductText*>(prodText);

	if ( inrec )
		return retrieveLinkedtoRec(SqliteProduct::PRODUCTTEXT9_FIELD, inrec->getNumber(), SqliteProduct::DEPTNUM_FIELD, inrec->getDeptNumber());

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveLinkedtoCategory(const DBCategoryI* category)
{
	const SqliteCategory* inrec = dynamic_cast<const SqliteCategory*>(category);

	if ( inrec )
		return retrieveLinkedtoRec(SqliteProduct::CATEGORY_FIELD, inrec->getNumber(), SqliteProduct::DEPTNUM_FIELD, inrec->getDeptNumber());

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveLinkedtoCoolCategory(const DBCoolCategoryI* coolcategory)
{
	const SqliteCoolCategory* inrec = dynamic_cast<const SqliteCoolCategory*>(coolcategory);

	if ( inrec )
		return retrieveLinkedtoRec(SqliteProduct::COOLCATEGORY_FIELD, inrec->getNumber(), SqliteProduct::DEPTNUM_FIELD, inrec->getDeptNumber());

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveLinkedtoCoolPreText(const DBCoolPreTextI* coolpretext)
{
	const SqliteCoolPreText* inrec = dynamic_cast<const SqliteCoolPreText*>(coolpretext);

	if ( inrec )
		return retrieveLinkedtoRec(SqliteProduct::COOLPRETEXT_FIELD, inrec->getNumber(), SqliteProduct::DEPTNUM_FIELD, inrec->getDeptNumber());

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveLinkedtoDefaultCoolText(const DBCoolTextI* cooltext)
{
	const SqliteCoolText* inrec = dynamic_cast<const SqliteCoolText*>(cooltext);

	if ( inrec )
		return retrieveLinkedtoRec(SqliteProduct::DEFAULTCOOLTEXT_FIELD, inrec->getNumber(), SqliteProduct::DEPTNUM_FIELD, inrec->getDeptNumber());

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveLinkedtoCoolList(const DBCoolListI* coollist)
{
	const SqliteCoolList* inrec = dynamic_cast<const SqliteCoolList*>(coollist);

	if ( inrec )
		return retrieveLinkedtoRec(SqliteProduct::COOLLISTKEY_FIELD, inrec->getNumber(), SqliteProduct::DEPTNUM_FIELD, inrec->getDeptNumber());

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveSpecials(const DBSpecialProgramI* special)
{
	const SqliteSpecialProgram* inrec = dynamic_cast<const SqliteSpecialProgram*>(special);

	if ( inrec )
	{
		SqliteQuery<SqliteProduct>* query = new SqliteQuery<SqliteProduct>(*this);

		string filter = SqliteProduct::DEPTNUM_FIELD + " = " + inrec->getDeptNumber() + " and " + SqliteProduct::SPECIALNUMBER_FIELD + " = " + inrec->getNumber();
		if ( sdb_.getCurrentLocaleLanguageNumber() == 2 )
			query->selectSome(filter, SqliteProduct::NAME2_FIELD);
		else
			query->selectSome(filter, SqliteProduct::NAME1_FIELD);

		return query;
	}

	return 0;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveAllSpecials(const std::string& deptNum)
{
	SqliteQuery<SqliteProduct>* query = new SqliteQuery<SqliteProduct>(*this);

	string filter = SqliteProduct::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteProduct::SPECIALNUMBER_FIELD + " > 0";
	if ( sdb_.getCurrentLocaleLanguageNumber() == 2 )
		query->selectSome(filter, SqliteProduct::NAME2_FIELD);
	else
		query->selectSome(filter, SqliteProduct::NAME1_FIELD);

	return query;
}


DBQueryI<DBProductI>* SqliteProductsRM::retrieveLinkedtoRec(const std::string& column1Name, const std::string column1Value, const std::string& column2Name, const std::string& column2Value)
{
	if ( column1Name.size() && column1Value.size() )
	{
		SqliteQuery<SqliteProduct>* query = new SqliteQuery<SqliteProduct>(*this);

		string filter = column1Name + " = " + column1Value;
		if ( column2Name.size() && column2Value.size() )
			filter += " and " + column2Name + " = " + column2Value;
		query->selectSome(filter, SqliteProduct::PLU_FIELD);

		return query;
	}

	return 0;
}


bool SqliteProductsRM::isProductShortNameUsed(LanguageSelection langNum)
{
	bool result = false;
	string whichName = ( langNum == PrimaryLanguage ) ? SqliteProduct::NAME1_FIELD : SqliteProduct::NAME2_FIELD;
	SqliteQuery<SqliteProduct>* query = new SqliteQuery<SqliteProduct>(*this);
	if (query)
	{
		string sql = "select ";
		sql.append(SqliteProduct::PLU_FIELD + ", " + SqliteProduct::DESCRIPTION_FIELD + ", " + SqliteProduct::NAME1_FIELD + ", " + SqliteProduct::NAME2_FIELD);
		sql.append(" from " + tableFilename_);
		sql.append(" where " + whichName + " not null and " + whichName + " != '' limit 1");

		query->selectUsingQuery(sql);
		result = (query->empty()) ? false : true;
		delete query;
	}
	return result;
}


DBProductI* SqliteProductsRM::findPLU(const string& plu, const std::string& deptNum)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteProduct::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));
	keys.push_back(SqliteField(SqliteProduct::PLU_FIELD, SqliteField::INTEGER, plu));

	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
	if ( findrec )
		return new SqliteProduct(findrec);

	return 0;
}


DBProductI* SqliteProductsRM::findLinkedRecord(const DBProductStatisticI* stat)
{
	const SqliteProductStatistic* inrec = dynamic_cast<const SqliteProductStatistic*>(stat);

	if ( inrec )
	{
		SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteProduct::PRODUCTID_FIELD, SqliteField::INTEGER, inrec->getProductKey()));
		if ( findrec )
			return new SqliteProduct(findrec);
	}

	return 0;
}


DBProductI* SqliteProductsRM::findLinkedRecord(const DBSoftKeyI* skey)
{
	const SqliteSoftKey* inrec = dynamic_cast<const SqliteSoftKey*>(skey);

	if ( inrec )
		return findPLU(inrec->getActionProductNumber(), inrec->getDeptNumber());

	return 0;
}


DBProductI* SqliteProductsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteProduct* createrec = new SqliteProduct(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


// Saving a product is more complex than saving most of the other record types.
// This is because the product table has two fields that are hidden from the
// application, PrCdNm1 and PrCdNm2 (product coded name fields), which are
// therefore managed internally here. Additionally, there is a whole table that
// is also hidden from the application, PCSeqs (product character sequences),
// which is also managed internally here and throughout the methods of this
// class.
// This operation has now become very complex and not atomic, and as a consequence
// it cause problems if two tasks are modifying the same product at the same time.
void SqliteProductsRM::saveRecord(const DBProductI* rec)
{
	const SqliteProduct* inrec = dynamic_cast<const SqliteProduct*>(rec);

	if ( inrec )
	{
/*  Completely commented out when implementing the Customer Hook for Walmart;
    We are using the PrCdNm1 database column for the 'last time the PLU was received from the Walmart server',
	to determine if the record is expired or not.

		const string& newDescription = inrec->getDescription();
		const string& newName1 = inrec->getName1();
		const string& newName2 = inrec->getName2();
		string oldDescription, oldName1, oldName2;

		// Get the values of the Name1 and Name2 fields as they are currently 
		// stored in the database. They will help us decide whether to update
		// the CodedName1 and CodedName2 fields, as well as the character
		// sequences associated with this product record.
		SqliteRecord* temprec = SqliteRecordManager::findKey(SqliteField(SqliteProduct::PRODUCTID_FIELD, SqliteField::INTEGER, inrec->getProductKey()));
		if ( temprec )
		{
			oldDescription.assign(temprec->getField(SqliteProduct::DESCRIPTION_FIELD));
			oldName1.assign(temprec->getField(SqliteProduct::NAME1_FIELD));
			oldName2.assign(temprec->getField(SqliteProduct::NAME2_FIELD));

			delete temprec;
		}

#ifndef EVOL_NDEBUG
		Debug::trace("database.saveRecord.sequences", 28, "Saving product " + inrec->getPLU() + " with description='" +
				newDescription + "', name1='" + newName1 + "', and name2='" + newName2 + "' (old description='" +
				oldDescription + "', name1='" + oldName1 + "', and old name2='" + oldName2 + "')");
#endif

		// Deal with Name1 first
 		std::string nameToTransform1, localeName1;
		util_platform::Platform::getInstance().getLocaleString(localeName1, sdb_.getLanguageCode(1));

		// validate locale xx_XX.utf8 (Randy test)
		if (( !localeName1.length() ) || ( 10 == localeName1.length() ))
			Debug::trace("database.saveRecord.sequences", 28, "Primary locale: " + localeName1 );
		else
		{
			Debug::trace("database.saveRecord.sequences", 28, "Bad primary locale!" );
			localeName1.clear();
		}

		if ( newName1.empty() && (oldDescription.compare(newDescription) || oldName1.compare(newName1)) )
		{
			nameToTransform1 = cleanupDescription(newDescription, std::locale(localeName1.c_str()));
		}
		else if ( oldName1.compare(newName1) )
		{
			nameToTransform1 = newName1;
		}

		if ( !nameToTransform1.empty() )
		{
			std::string transformedName1;
			locale loc(localeName1.c_str());

			util_string::localeTransform(transformedName1, nameToTransform1, loc);
			int truncSize = (int)nameToTransform1.size() * 2;

			if (truncSize > localeTransformedLength_)
				truncSize = localeTransformedLength_;

			string shortTransformedName1(transformedName1.substr(0, truncSize));

#ifndef EVOL_NDEBUG
			Debug::trace("database.saveRecord.localeEncoded", 28, "Product's name1='" + nameToTransform1 + "' (localeTransformed is '" + 
					transformedName1 + "' and the short version is '" + shortTransformedName1 + "')");
#endif

			const_cast<SqliteProduct*>(inrec)->setCodedName1(shortTransformedName1);


			if ( useShortDescriptionSequences_ )
			{
				const string& language1 = sdb_.getLanguageCode(1);
				removeSequences(inrec, language1);

#ifndef EVOL_NDEBUG
				Debug::trace("database.saveRecord.sequences", 28, "Removed sequences for product's name1 field");
#endif

				// Instead of obtaining character sequences from the 'transformedName1' 
				// we get them from newName1 (we transform them right at the time of
				// storing them. The reason is to avoid some UI search problems: if
				// the user types 'CHE' (for 'cherimoya') or 'LEC' (for 'lechuga') in
				// 'Spanish search' we will fail because, in the former case, the 'CH'
				// becomes one single character and then a two-character sequence will
				// not match a three-character one, and in the latter, the 'LEC' sequence
				// the user typed will not match the 'LECH' sequence stored.
//				vector<string>* seqs = util_string::getCharacterSequences(*transformedName1, minSequenceLength_, maxSequenceLength_);
				vector<string>* seqs = util_string::getCharacterSequences(nameToTransform1, minSequenceLength_, maxSequenceLength_);
				if ( seqs )
				{
					addSequences(inrec, nameToTransform1.substr(0, localeTransformedLength_), seqs, language1, maxSequenceLength_*2, loc);

#ifndef EVOL_NDEBUG
					Debug::trace("database.saveRecord.sequences", 28, "Added new sequences for product's name1 field");
#endif

					delete seqs;
				}
			}
		}


		// Deal with Name2 next
 		std::string nameToTransform2, localeName2;
		util_platform::Platform::getInstance().getLocaleString(localeName2, sdb_.getLanguageCode(2));

		// validate locale xx_XX.utf8 (Randy test)
		if (( !localeName2.length() ) || ( 10 == localeName2.length() ))
			Debug::trace("database.saveRecord.sequences", 28, "Secondary locale: " + localeName2 );
		else
		{
			Debug::trace("database.saveRecord.sequences", 28, "Bad secondary locale!" );
			localeName2.clear();
		}

		if ( newName2.empty() && (oldDescription.compare(newDescription) || oldName2.compare(newName2)) )
		{
			nameToTransform2 = cleanupDescription(newDescription, std::locale(localeName2.c_str()));
		}
		else if ( oldName2.compare(newName2) )
		{
			nameToTransform2 = newName2;
		}

		if ( !nameToTransform2.empty() )
		{
			std::string transformedName2;
			locale loc(localeName2.c_str());

			util_string::localeTransform(transformedName2, nameToTransform2, loc);
			int truncSize = (int)nameToTransform2.size() * 2;

			if (truncSize > localeTransformedLength_)
				truncSize = localeTransformedLength_;

			string shortTransformedName2(transformedName2.substr(0, truncSize));

#ifndef EVOL_NDEBUG
			Debug::trace("database.saveRecord.localeEncoded", 28, "Product's name2='" + nameToTransform2 + "' (localeTransformed is '" + 
					transformedName2 + "' and the short version is '" + shortTransformedName2 + "')");
#endif

			const_cast<SqliteProduct*>(inrec)->setCodedName2(shortTransformedName2);


			if ( useShortDescriptionSequences_ )
			{
				const string& language2 = sdb_.getLanguageCode(2);
				removeSequences(inrec, language2);

#ifndef EVOL_NDEBUG
				Debug::trace("database.saveRecord.sequences", 28, "Removed sequences for product's name2 field");
#endif

				// Instead of obtaining character sequences from the 'transformedName1' 
				// we get them from newName1 (we transform them right at the time of
				// storing them. The reason is to avoid some UI search problems: if
				// the user types 'CHE' (for 'cherimoya') or 'LEC' (for 'lechuga') in
				// 'Spanish search' we will fail because, in the former case, the 'CH'
				// becomes one single character and then a two-character sequence will
				// not match a three-character one, and in the latter, the 'LEC' sequence
				// the user typed will not match the 'LECH' sequence stored.
//				vector<string>* seqs = util_string::getCharacterSequences(*transformedName2, minSequenceLength_, maxSequenceLength_);
				vector<string>* seqs = util_string::getCharacterSequences(nameToTransform2, minSequenceLength_, maxSequenceLength_);
				if ( seqs )
				{
					addSequences(inrec, nameToTransform2.substr(0, localeTransformedLength_), seqs, language2, maxSequenceLength_*2, loc);

#ifndef EVOL_NDEBUG
					Debug::trace("database.saveRecord.sequences", 28, "Added new sequences for product's name2 field");
#endif

					delete seqs;
				}
			}
		}
*/
		// Now save the product record
		if ( SqliteRecordManager::save(inrec->getRecord()) )
			modified();
		else
			throw edb::DBException(edb::DBException::SQLITE_SAVE_RECORD, "Unable to save record.");
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteProductsRM::deleteRecord(const DBProductI* rec)
{
	const SqliteProduct* inrec = dynamic_cast<const SqliteProduct*>(rec);

	if ( inrec )
	{
#ifndef EVOL_NDEBUG
		Debug::trace("database.delete", 28, "Going to delete product " + inrec->getPLU());
#endif

		if ( useSequences_ )
		{
#ifndef EVOL_NDEBUG
			Debug::trace("database.delete.sequences", 28, "Going to remove old sequences for description");
#endif
			removeSequences(inrec, _longDescriptionLanguage_);

#ifndef EVOL_NDEBUG
			Debug::trace("database.delete.sequences", 28, "Going to remove old sequences for name1");
#endif
			removeSequences(inrec, sdb_.getLanguageCode(1));

#ifndef EVOL_NDEBUG
			Debug::trace("database.delete.sequences", 28, "Going to remove old sequences for name2");
#endif

			removeSequences(inrec, sdb_.getLanguageCode(2));

#ifndef EVOL_NDEBUG
			Debug::trace("database.delete", 28, "Going to mark record as deleted");
#endif

		}
		
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteProductsRM::deleteAll()
{
	int deleted = 0;

#ifndef EVOL_NDEBUG
		Debug::trace("database.delete", 28, "Going to delete all products");
#endif

	SqliteQuery<SqliteProduct>* query = dynamic_cast<SqliteQuery<SqliteProduct>*>(retrieveAll());
	if ( query )
	{
		DBProductI* prod = query->first();

		while ( prod )
		{
			SqliteProduct* tmp = dynamic_cast<SqliteProduct*>(prod);
			prod = query->next(prod);

#ifndef EVOL_NDEBUG
			if ( prod )
				Debug::trace("database.delete", 28, "Navigated to next product " + prod->getPLU());
			else
				Debug::trace("database.delete", 28, "No more products to navigate");
#endif

			if ( SqliteRecordManager::markDeleted(tmp->getRecord()) )
			{
#ifndef EVOL_NDEBUG
				Debug::trace("database.delete", 28, "Deleted product " + tmp->getPLU() + " from the database");
#endif

				++ deleted;
			}

			delete tmp;
		}

		delete query;
	}

	if ( useSequences_ && productSequences_ )
	{
#ifndef EVOL_NDEBUG
		Debug::trace("database.delete.sequences", 28, "Going to delete ALL product sequences");
#endif

		productSequences_->deleteAll();
	}

#ifndef EVOL_NDEBUG
	Debug::trace("database.delete", 28, "Deleted all products");
#endif

	return deleted;
}


int SqliteProductsRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteProduct::DEPTNUM_FIELD + " = " + deptNum);
}


int SqliteProductsRM::calculateAverageSpace()
{
/*
	int totalRecords = table_->recCount();

	if ( totalRecords > 0 )
	{
		int totalSizeProducts = SqliteRecordManager::calculateTotalSpace(), totalSizeSequences = 0;

		if ( useSequences_ && productSequences_ )
			totalSizeSequences = productSequences_->calculateTotalSpace();

#ifndef EVOL_NDEBUG
		Debug::trace("database.averageSpace", 20, "Average Space taken by Products: ");
		Debug::trace("database.averageSpace", 20, "\tTotal Space by Products is " +
				inttostring(totalSizeProducts));
		Debug::trace("database.averageSpace", 20, "\tTotal Space by Character Sequences is " +
				inttostring(totalSizeSequences));
		Debug::trace("database.averageSpace", 20, "\tTotal Products records is " +
				inttostring(totalRecords));
		Debug::trace("database.averageSpace", 20, "\tAverage Space is " +
				inttostring((totalSizeProducts + totalSizeSequences) / totalRecords + 1));
#endif

		return (totalSizeProducts + totalSizeSequences) / totalRecords + 1;
	}

	return 0;
*/
	return 1;
}


bool SqliteProductsRM::copy(bool from, const util_filesystem::Directory& path)
{
	bool result = SqliteRecordManager::copy(from, path);
	if ( result && useSequences_ )
	{
		// Now copy the SqliteProductSequences table
		if ( productSequences_ )
			result = productSequences_->copy(from, path);
	}

	return result;
}


bool SqliteProductsRM::backup(const std::string& dir)
{
	bool result = SqliteRecordManager::backup(dir);
	if ( result && useSequences_ )
	{
		// Now backup the SqliteProductSequences table only if the backup of the
		// SqliteProducts table was successful
		if ( productSequences_ )
		{
			result = productSequences_->backup(dir);

#ifndef EVOL_NDEBUG
			if ( !result )
				Debug::trace("database.backup", 10, 
						"We were able to backup the products table but not the product sequences table");
#endif
		}
	}

	return result;
}


bool SqliteProductsRM::exists()
{
	bool result = SqliteRecordManager::exists();
	if ( result && useSequences_ )
	{
		// Now exists the SqliteProductSequences table
		if ( productSequences_ )
			result = productSequences_->exists();
	}

	return result;
}


bool SqliteProductsRM::compress()
{
	bool result = SqliteRecordManager::compress();
	if ( result && useSequences_ )
	{
		// Now compress the SqliteProductSequences table
		if ( productSequences_ )
			result = productSequences_->compress();
	}

	return result;
}


bool SqliteProductsRM::remove()
{
	bool result = SqliteRecordManager::remove();
	if ( result && useSequences_ )
	{
		// Now remove the SqliteProductSequences table
		if ( productSequences_ )
			result = productSequences_->remove();
	}

	return result;
}


bool SqliteProductsRM::check()
{
	bool result = SqliteRecordManager::check();
	if ( result && useSequences_ )
	{
		// Now check the SqliteProductSequences table
		if ( productSequences_ )
			result = productSequences_->check();
	}

	return result;
}


void SqliteProductsRM::show()
{
	SqliteRecordManager::show();
	if ( useSequences_ )
	{
		if ( productSequences_ )
			productSequences_->show();
	}
}


DBQueryI<DBProductSequenceI>* SqliteProductsRM::createProductSequenceQuery(const string& sequence, const string& language)
{
	return productSequences_->retrieveCharacterSequence(sequence, language);
}


void SqliteProductsRM::retrieveProperties()
{
	const string* property = getTableProperty("useCharacterSequencesForShortDescriptions");
	if ( property )
	{
		useShortDescriptionSequences_ = stringtobool(*property);
#ifndef EVOL_NDEBUG
		Debug::trace("database.products.properties", 28, "useCharacterSequencesForShortDescriptions='" +
				*property + "', value stored '" + Bool::toString(useShortDescriptionSequences_) + "'");
#endif
	}

	property = getTableProperty("useCharacterSequencesForLongDescriptions");
	if ( property )
	{
		useLongDescriptionSequences_ = stringtobool(*property);
#ifndef EVOL_NDEBUG
		Debug::trace("database.products.properties", 28, "useCharacterSequencesForLongDescriptions='" +
				*property + "', value stored '" + Bool::toString(useLongDescriptionSequences_) + "'");
#endif
	}

	useSequences_ = useShortDescriptionSequences_ || useLongDescriptionSequences_;

	property = getTableProperty("minCharacterSequenceLength");
	if ( property )
	{
		minSequenceLength_ = stringtoint(*property);
#ifndef EVOL_NDEBUG
		Debug::trace("database.products.properties", 28, "minCharacterSequenceLength='" +
				*property + "', value stored '" + inttostring(minSequenceLength_) + "'");
#endif
	}

	property = getTableProperty("maxCharacterSequenceLength");
	if ( property )
	{
		maxSequenceLength_ = stringtoint(*property);
#ifndef EVOL_NDEBUG
		Debug::trace("database.products.properties", 28, "maxCharacterSequenceLength='" +
				*property + "', value stored '" + inttostring(maxSequenceLength_) + "'");
#endif
	}

	property = getTableProperty("localeTransformedLength");
	if ( property )
	{
		localeTransformedLength_ = stringtoint(*property);
#ifndef EVOL_NDEBUG
		Debug::trace("database.products.properties", 28, "localeTransformedLength='" +
				*property + "', value stored '" + inttostring(localeTransformedLength_) + "'");
#endif
	}

	property = getTableProperty("validateWords");
	if ( property )
	{
		validateWords_ = stringtobool(*property);
#ifndef EVOL_NDEBUG
		Debug::trace("database.products.properties", 28, "validateWords='" +
				*property + "', value stored '" + Bool::toString(validateWords_) + "'");
#endif
	}

	property = getTableProperty("allowDigitsInSequences");
	if ( property )
	{
		allowDigitsInSequences_ = stringtobool(*property);
#ifndef EVOL_NDEBUG
		Debug::trace("database.products.properties", 28, "allowDigitsInSequences='" +
				*property + "', value stored '" + Bool::toString(allowDigitsInSequences_) + "'");
#endif
	}

	property = getTableProperty("useIgnoreWords");
	if ( property )
	{
		useIgnoreWords_ = stringtobool(*property);
#ifndef EVOL_NDEBUG
		Debug::trace("database.products.properties", 28, "useIgnoreWords='" +
				*property + "', value stored '" + Bool::toString(useIgnoreWords_) + "'");
#endif
	}
}


string SqliteProductsRM::cleanupDescription(const string& description, const std::locale& language)
{
	string::size_type pos = 0;
	string::size_type total = description.size();
	string cleanDescription, anotherWord;
	bool inWord = false, isAnotherWord = false;            // Only used if validating words
	string::size_type wordStarts = 0, spaceStarts = 0;     // Only used if validating words
	const ctype<char>& ctypeFacet = use_facet< ctype<char> >(language);  /* (SJJC port 6/19/06) */

	while ( pos < total )
	{
		bool skippedStuff;   // Need to define here in order to use in the while

		// Skip any font changes
		do
		{
			skippedStuff = false;

			if ( description[pos] == util_string::_ESC_ )
			{
				// If there is a font change sequence, skip it
				if ( pos + 1 < total && description[pos + 1] == _ESC_ )
				{
					string::size_type fontChangeStart = pos;

					// Find the end of the font change sequence
					pos += 2;
					while ( pos < total && description[pos] != _ESC_ )
						++pos;

					if ( pos < total )          // Pass over the last ESC
					{
						++pos;
						skippedStuff = true;   // Back to top of loop
					}
					else    // If there was no end, there was no font change.
						pos = fontChangeStart;
				}
			}
		} while ( pos < total && skippedStuff );

		if ( pos < total )
		{
			char character = description[pos++];

			if ( validateWords_ )
			{
				if ( !ctypeFacet.is(ctype_base::alnum, character) )
				{
					if ( inWord )
					{
						// We have just completed another word
						anotherWord = cleanDescription.substr(wordStarts);
						isAnotherWord = true;
						inWord = false;
					}
				}
				else
				{
					if ( !inWord )
					{
						wordStarts = cleanDescription.size();
						inWord = true;
					}
				}
			}

			// Add another character from original description
			cleanDescription.append(1, ctypeFacet.toupper(character));
		}

		if ( pos >= total )
		{
			if ( validateWords_ )
			{
				if ( inWord )
				{
					// We have just completed last word
					anotherWord = cleanDescription.substr(wordStarts);
					isAnotherWord = true;
					// Leave inWord set as true to help decide whether to delete the
					// last word through the end of the string or leave the last space
					// introduced
				}
			}
		}

		if ( isAnotherWord )
		{
#ifndef EVOL_NDEBUG
			Debug::trace("database.products.cleanupDescription", 28, "Completed another word: '" +
					anotherWord + "'");
#endif

			if ( !isValidWord(anotherWord, language) )
			{
#ifndef EVOL_NDEBUG
				Debug::trace("database.products.invalidWord", 28, "The word '" +
						anotherWord + "' is not valid and will be dropped from the description");
#endif
				
				if ( inWord )
					// There is no additional space at end
					cleanDescription.erase(spaceStarts);
				else
					// There is an additional space at end
					cleanDescription.erase(spaceStarts, cleanDescription.size() - spaceStarts - 1);
			}

			spaceStarts = cleanDescription.size() - 1;
			isAnotherWord = false;
		}
	}

	return cleanDescription;
}


bool SqliteProductsRM::isValidWord(const string& word, const std::locale& language)
{
	/* Words like the following will be invalid:
		On
		In
		The
		A
		E
		S
		W
		N
		Pure
		Fresh, Frsh
		Dried
		Raw
		Ripe
		Roasted
		Organic, Org
		Natural
		Seeded
		Seedless, Sdls
		Bunched, Bunch, Bnch
		Fruit
		Greens
		Nuts
		Root
		Tips
		Mix
		Reduced
	Sizes like:
		Baby
		Mini
		Small, SM, Sml
		Large, LG, LGE
		XLarge, XLGe
		Jumbo
		Long
		Bulk
		oz
		lb
		pt
		gal
		quart
		whole
		cut
		tote
		Cup
		Tray
		Pack
		Bowl
		Pieces
		Slices, Slice
		Sticks, Stick
	Colors like:
		Red
		Yellow, Ylw, Yel
		White, Wht
		Green, Grn
		Pink
		Brown, Brwn
		Black, Blk
		Golden, Gldn, Gold, Gld
		Purple
		Neon
	Also words that contain numbers in them since we cannot search for them. */

	string::size_type length = word.size();
	const ctype<char>& ctypeFacet = use_facet< ctype<char> >(language);  /* (SJJC port 6/19/06) */

	// If the word has only one character then it is not valid
	if ( length < 2 )
	{
#ifndef EVOL_NDEBUG
		Debug::trace("database.products.invalidWord", 28, "The word '" + word +
				"' is shorter than 2 characters and therefore deemeed invalid.");
#endif

		return false;
	}

	if ( !allowDigitsInSequences_ )
	{
		// If the word has digits, then it is not valid
		for ( string::size_type i = 0; i < length; ++i )
		{
			if ( ctypeFacet.is(ctype_base::digit, word[i]) )
			{
#ifndef EVOL_NDEBUG
				Debug::trace("database.products.invalidWord", 28, "The word '" +
						word + "' is not valid since it contains digits");
#endif
				return false;
			}
		}
	}

	// If the word has more than 20 characters, then it is assumed to be valid
	if ( length > 20 )
	{
#ifndef EVOL_NDEBUG
		Debug::trace("database.products.invalidWord", 28, "The word '" + word +
				"' is longer than 20 characters and therefore deemeed valid.");
#endif

		return true;
	}

	// If the word is in the list of "ignore words", then it is not valid
	if ( useIgnoreWords_ )
	{
		string localWord(word);
		DBIgnoreWordI* foundWord;

		ctypeFacet.toupper(&localWord[0], &localWord[localWord.size()]);
#ifndef EVOL_NDEBUG
		Debug::trace("database.products.invalidWord", 28, "Going to check the validity of '" +
				localWord + "' in language '" + language.name() + "'");
#endif
		if ( ignoreWords_ )
		{
			foundWord = ignoreWords_->find(language.name(), localWord);
			if ( foundWord != 0 )
			{
#ifndef EVOL_NDEBUG
				Debug::trace("database.products.invalidWord", 28, "The word '" +
						word + "' was found invalid for language '" + language.name() + "'");
#endif

				delete foundWord;

				return false;
			}
		}
	}

	// Otherwise the word is assumed to be valid.
	return true;
}


void SqliteProductsRM::addSequences(const SqliteProduct* prod, const string& shortName, 
		const vector<string>* sequences, const string& language, int sequenceLength,
		const locale& loc)
{
	if ( sequences )
	{

		bool doNotTransform = (language == _longDescriptionLanguage_);
		
		vector<string>::const_iterator iter = sequences->begin();
		while ( iter != sequences->end() )
		{

			addASingleSeq(*iter, doNotTransform,
					loc, language, sequenceLength, shortName, prod);


			++iter;
		}
	}

#ifndef EVOL_NDEBUG
	Debug::trace("database.addSequences", 28, 
			"Created and saved product sequences for product number " + prod->getPLU());
#endif
}

void SqliteProductsRM::addASingleSeq(const string& seq, bool doNotTransform,
								 const locale& loc, const string& language,
								 int sequenceLength, const string& shortName,
								 const SqliteProduct* prod)
{
	std::string transformedSequence;

	if ( doNotTransform )
		transformedSequence = seq;
	else
		util_string::localeTransform(transformedSequence, seq, loc);


	int transformSize = (int)seq.size() * 2;
	if (transformSize > sequenceLength)
		transformSize = sequenceLength;

	transformedSequence.erase(transformSize);


#ifndef EVOL_NDEBUG
	Debug::trace("database.addSequences", 28, "Going to add sequence '" + transformedSequence + "' for product with name '" + shortName + "' in language '" + language + "'.");
#endif

	SqliteProductSequence* ps = 
			dynamic_cast<SqliteProductSequence*>(productSequences_->findProductSequence(prod, transformedSequence, language));
	if ( !ps )
	{
		ps = dynamic_cast<SqliteProductSequence*>(productSequences_->createRecord(prod, transformedSequence, language));
		if ( ps )
		{
			ps->setProductName(shortName);
			productSequences_->saveRecord(ps);
		}
	}
	else
	{
		ps->setProductName(shortName);
		productSequences_->saveRecord(ps);

#ifndef EVOL_NDEBUG
		Debug::trace("database.addSequences", 28, "The sequence already existed. It was modified and saved.");
#endif
	}

	if ( ps )
		delete ps;

}

void SqliteProductsRM::removeSequences(const SqliteProduct* prod, const string& language)
{
	if ( prod )
	{
		DBQueryI<DBProductSequenceI>* query = productSequences_->retrieveLinkedToProduct(prod);
		if ( query )
		{
#ifndef EVOL_NDEBUG
			Debug::trace("database.removeSequences", 28, 
					"Retrieved product sequence query for product number " + prod->getPLU() + " and language " + language);
#endif

			DBProductSequenceI* ps = query->first();
			while ( ps )
			{
				DBProductSequenceI* next = query->next(ps);

				if ( ps->getCharacterSequenceLanguage() == language )
				{
					if ( productSequences_->deleteRecord(ps) )
					{
#ifndef EVOL_NDEBUG
						Debug::trace("database.removeSequences", 10, "Deleted product sequence");
#endif
					}
					else
					{
#ifndef EVOL_NDEBUG
						Debug::trace("database.removeSequences", 10, "Unable to delete product sequence");
#endif
					}
				}

				delete ps;
				ps = next;
			}

			delete query;
		}
	}

#ifndef EVOL_NDEBUG
	Debug::trace("database.removeSequences", 28, 
			"Deleted product sequences for product number " + prod->getPLU() + " and language " + language);
#endif
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif




#ifdef SQLITEPRODUCTSRM_DRIVER


#include <iostream>


int main(int argc, char * argv[])
{
	unsigned long int tot;
	SqliteProductsRM prods;

	tot = prods.countAll();
	if ( tot > 0 )
	{
		SqliteProductsRM::iterator it = prods.begin();
		SqliteProductsRM::iterator end = prods.end();

		while ( it != end )
			cout << *it;
	}
}


#endif // SQLITEPRODUCTSRM_DRIVER
