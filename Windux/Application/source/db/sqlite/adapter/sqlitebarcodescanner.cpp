#include "sqlitebarcodescanner.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteBarcodeScanner::SCANNERCODE_FIELD  ("ScanCode");
const string SqliteBarcodeScanner::SCANNERPLU_FIELD   ("ScanPlu");


#ifdef USE_ACCOUNTANT
int SqliteBarcodeScanner::_dataType_ = 0;
string SqliteBarcodeScanner::_dataTypeName_("SqliteBarcodeScanner");
#endif



SqliteBarcodeScanner::SqliteBarcodeScanner(SqliteRecord* r, DBBarcodeScannerI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteBarcodeScanner::~SqliteBarcodeScanner()
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