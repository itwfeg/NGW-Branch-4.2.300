#include "edb.h"
#include DBPRODUCTTEXTCFG_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBProductTextCfg::DBProductTextCfg(edb_adapter::DBProductTextCfgI* d)
		: decorated_(d)
{
}


DBProductTextCfg::DBProductTextCfg(const DBProductTextCfg& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBProductTextCfg::~DBProductTextCfg()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBProductTextCfg& rec)
{
	os << "DBProductTextCfg TextType " << rec.getTextType() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isLabelNumNull() )
		os << ", LabelNum: " << rec.getLabelNum();
	if ( !rec.isPrinterNumNull() )
		os << ", PrinterNum: " << rec.getPrinterNum();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
