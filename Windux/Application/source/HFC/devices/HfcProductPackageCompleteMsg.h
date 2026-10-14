#ifndef HFCPRODUCTPACKAGECOMPLETEMSG_H
#define HFCPRODUCTPACKAGECOMPLETEMSG_H



#include "ProductPackageCompleteMsg.h"
#include "HfcObject.h"

namespace Hobart
{
	class CEvolMsgReceiver;
}



class HfcProductPackageCompleteMsg : public ProductPackageCompleteMsg, public HfcObject
{
	public:
		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		HfcProductPackageCompleteMsg(const char *data, int len, int formatType);
		~HfcProductPackageCompleteMsg();

//		virtual const char *serialize(int format) const;


	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};



#endif //HFCPRODUCTPACKAGECOMPLETEMSG_H
