#include "sqlitescale.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteScale::IDENTIFIER_FIELD                     ("ScID");
const string SqliteScale::HOSTNAME_FIELD                       ("ScHstNm");
const string SqliteScale::IPADDRESS_FIELD                      ("ScIPAddr");
const string SqliteScale::TYPE_FIELD                           ("ScType");
const string SqliteScale::CREATIONDATETIME_FIELD               ("ScCreatDT");
const string SqliteScale::STATE_FIELD                          ("ScState");
const string SqliteScale::STATECHANGEDDATETIME_FIELD           ("ScStateDT");
const string SqliteScale::REGISTERING_FIELD                    ("ScRegis");
const string SqliteScale::DEREGISTERING_FIELD                  ("ScDReg");
const string SqliteScale::NOTIFIEDBATCHES_FIELD                ("ScNBatch");
const string SqliteScale::NOTIFIEDBATCHESDATETIME_FIELD        ("ScNBtchDT");
const string SqliteScale::NOTIFIEDONLINE_FIELD                 ("ScNOnLn");
const string SqliteScale::NOTIFIEDONLINEDATETIME_FIELD         ("ScNOnLnDT");
const string SqliteScale::RETRYCOUNT_FIELD                     ("ScRetCt");
const string SqliteScale::VERSION_FIELD                        ("ScVer");
const string SqliteScale::VERSIONRECEIVED_FIELD                ("ScVerRec");
const string SqliteScale::VERSIONRECEIVEDDATETIME_FIELD        ("ScVerRecDT");
const string SqliteScale::LASTSUCCESSFULCOMMDATETIME_FIELD     ("ScLSuccCm");
const string SqliteScale::FIRSTFAILEDCOMMDATETIME_FIELD        ("ScFFailCm");
const string SqliteScale::TOTALSUCCESSFULCOMMUNICATIONS_FIELD  ("ScTSuccCm");
const string SqliteScale::TOTALFAILEDCOMMUNICATIONS_FIELD      ("ScTFailCm");
const string SqliteScale::TOTALBUSYCOMMUNICATIONS_FIELD        ("ScTBusyCm");


#ifdef USE_ACCOUNTANT
int SqliteScale::_dataType_ = 0;
string SqliteScale::_dataTypeName_("SqliteScale");
#endif



SqliteScale::SqliteScale(SqliteRecord* r, DBScaleI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteScale::~SqliteScale()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( rec_ )
		delete rec_;

	if ( decorated_ )
		delete decorated_;
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
