#include "sqlitetransaction.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteTransaction::SCALEID_FIELD                 ("TrScID");
const string SqliteTransaction::TRANSACTIONID_FIELD           ("TrID");
const string SqliteTransaction::TRANSACTIONTYPE_FIELD         ("TTNum");
const string SqliteTransaction::TRANSACTIONGROUP_FIELD        ("TrGrp");
const string SqliteTransaction::TRANSACTIONDATETIME_FIELD     ("TrDtTm");
const string SqliteTransaction::TRANSACTIONVOID_FIELD         ("TrVoid");
const string SqliteTransaction::AFFECTSPRODUCTION_FIELD       ("TrAPrdt");
const string SqliteTransaction::READANDCLEAR_FIELD            ("TrRdClr");
const string SqliteTransaction::NUMBERPACKAGES_FIELD          ("TrNmPck");
const string SqliteTransaction::USEDSECURITYLABEL_FIELD       ("TrSecLb");
const string SqliteTransaction::COOLFLAG_FIELD                ("TrCOOLF");
const string SqliteTransaction::TOTALVALUE_FIELD              ("TrTtVal");
const string SqliteTransaction::OLDTOTALVALUE_FIELD           ("TrOTtVl");
const string SqliteTransaction::DEPTNUM_FIELD                 ("DeptNum");
const string SqliteTransaction::PRODUCTNUMBER_FIELD           ("TrPrNum");
const string SqliteTransaction::REPORTINGCLASS_FIELD          ("TrClNum");
const string SqliteTransaction::PRODUCTTYPE_FIELD             ("TrPrTyp");
const string SqliteTransaction::PRODUCTIONPLANNINGTYPE_FIELD  ("TrPrPlT");
const string SqliteTransaction::PRODUCTDESCRIPTION_FIELD      ("TrPrDes");
const string SqliteTransaction::PRICEFACTOR_FIELD             ("TrPrMod");
const string SqliteTransaction::BARCODEPREFIX_FIELD           ("TrBarPref");
const string SqliteTransaction::BARCODENUMBER_FIELD           ("TrBarNum");
const string SqliteTransaction::UNITPRICE_FIELD               ("TrUnPr");
const string SqliteTransaction::EXCEPTIONPRICE_FIELD          ("TrExPr");
const string SqliteTransaction::FINALPRICE_FIELD              ("TrFnlPr");
const string SqliteTransaction::TARE_FIELD                    ("TrTare");
const string SqliteTransaction::PROPORTIONALTARE_FIELD        ("TrPrTare");
const string SqliteTransaction::FINALTARE_FIELD               ("TrFnlTare");
const string SqliteTransaction::BYCOUNT_FIELD                 ("TrByCount");
const string SqliteTransaction::FINALNETWEIGHT_FIELD          ("TrFnlNtWt");
const string SqliteTransaction::FIXEDWEIGHT_FIELD             ("TrFxWt");
const string SqliteTransaction::SHELFLIFEDAYS_FIELD           ("TrSfLfD");
const string SqliteTransaction::SHELFLIFEHOURS_FIELD          ("TrSfLfH");
const string SqliteTransaction::SHELFLIFEMINUTES_FIELD        ("TrSfLfM");
const string SqliteTransaction::PRODUCTLIFEDAYS_FIELD         ("TrPrLfD");
const string SqliteTransaction::PRODUCTLIFEHOURS_FIELD        ("TrPrLfH");
const string SqliteTransaction::PRODUCTLIFEMINUTES_FIELD      ("TrPrLfM");
const string SqliteTransaction::LABEL1_FIELD                  ("TrLab1");
const string SqliteTransaction::LABEL2_FIELD                  ("TrLab2");
const string SqliteTransaction::LABEL3_FIELD                  ("TrLab3");
const string SqliteTransaction::LABEL4_FIELD                  ("TrLab4");
const string SqliteTransaction::OPERATORNUMBER_FIELD          ("TrOpNum");
const string SqliteTransaction::OPERATORRFID_FIELD            ("TrOpRFID");
const string SqliteTransaction::OPERATORNAME_FIELD            ("TrOpName");
const string SqliteTransaction::OPERATORCHANGETYPE_FIELD      ("TrOpChg");
const string SqliteTransaction::GRAPHIC1_FIELD                ("TrGrNm1");
const string SqliteTransaction::GRAPHIC2_FIELD                ("TrGrNm2");
const string SqliteTransaction::GRAPHIC3_FIELD                ("TrGrNm3");
const string SqliteTransaction::GRAPHIC4_FIELD                ("TrGrNm4");
const string SqliteTransaction::COOLTEXT_FIELD                ("TrCOTxt");
const string SqliteTransaction::COOLTRACKINGTEXT_FIELD        ("TrCOTrTxt");
const string SqliteTransaction::CUTTESTNUMBER_FIELD           ("TrUTNum");
const string SqliteTransaction::PERCENTOFPRIMALWEIGHT_FIELD   ("TrUTPercentOfPrimalWeight");
const string SqliteTransaction::PERCENTOFPRIMALCOST_FIELD     ("TrUTPercentOfPrimalCost");
const string SqliteTransaction::MODECHANGE_FIELD              ("TrModeChange");
const string SqliteTransaction::OLDBARCODEDIGIT_FIELD         ("TrOBarDig");
const string SqliteTransaction::OLDUNITPRICE_FIELD            ("TrOUnPr");
const string SqliteTransaction::OLDFINALPRICE_FIELD           ("TrOFnPr");
const string SqliteTransaction::OLDTARE_FIELD                 ("TrOTare");
const string SqliteTransaction::OLDBYCOUNT_FIELD              ("TrOByCount");
const string SqliteTransaction::OLDFIXEDWEIGHT_FIELD          ("TrOFxWt");
const string SqliteTransaction::OLDSHELFLIFEDAYS_FIELD        ("TrOSfLfD");
const string SqliteTransaction::OLDSHELFLIFEHOURS_FIELD       ("TrOSfLfH");
const string SqliteTransaction::OLDSHELFLIFEMINUTES_FIELD     ("TrOSfLfM");
const string SqliteTransaction::OLDPRODUCTLIFEDAYS_FIELD      ("TrOPrLfD");
const string SqliteTransaction::OLDPRODUCTLIFEHOURS_FIELD     ("TrOPrLfH");
const string SqliteTransaction::OLDPRODUCTLIFEMINUTES_FIELD   ("TrOPrLfM");
const string SqliteTransaction::OLDLABEL1_FIELD               ("TrOLab1");
const string SqliteTransaction::OLDLABEL2_FIELD               ("TrOLab2");
const string SqliteTransaction::OLDLABEL3_FIELD               ("TrOLab3");
const string SqliteTransaction::OLDLABEL4_FIELD               ("TrOLab4");
const string SqliteTransaction::OLDGRAPHIC1_FIELD             ("TrOGrNm1");
const string SqliteTransaction::OLDGRAPHIC2_FIELD             ("TrOGrNm2");
const string SqliteTransaction::OLDGRAPHIC3_FIELD             ("TrOGrNm3");
const string SqliteTransaction::OLDGRAPHIC4_FIELD             ("TrOGrNm4");
const string SqliteTransaction::SWEETHEARTWEIGHT_FIELD        ("TrSweetheartWeight");

const string::size_type SqliteTransaction::OPERATORNUMBER_SIZE = 16;


#ifdef USE_ACCOUNTANT
int SqliteTransaction::_dataType_ = 0;
string SqliteTransaction::_dataTypeName_("SqliteTransaction");
#endif



SqliteTransaction::SqliteTransaction(SqliteRecord* r, DBTransactionI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteTransaction::~SqliteTransaction()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
