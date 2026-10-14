#include "edb.h"
#include DBBARCODESCANNER_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBBarcodeScanner::DBBarcodeScanner(edb_adapter::DBBarcodeScannerI* d)
		: decorated_(d)
{
}


DBBarcodeScanner::DBBarcodeScanner(const DBBarcodeScanner& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBBarcodeScanner::~DBBarcodeScanner()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBBarcodeScanner& rec)
{
	os << "DBBarcodeScanner ScannerCode " << rec.getScannerCode() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isScannerProductNumNull() )
		os << ", getScannerProductNum: " << rec.getScannerProductNum();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
