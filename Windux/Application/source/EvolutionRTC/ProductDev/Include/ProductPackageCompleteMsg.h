#ifndef PRODUCTPACKAGECOMPLETEMSG_H
#define PRODUCTPACKAGECOMPLETEMSG_H



#include "EvolMessage.h"

#include "evolution.h"

class ProductTransaction;

namespace Hobart
{
	class CEvolMsgReceiver;
}



class ProductPackageCompleteMsg : public EvolMessage
{
	public:
		// --- Constructors and destructor --- //
		ProductPackageCompleteMsg(Hobart::CEvolMsgReceiver * pHobThread = 0, HobRecordNum recordNumber = -1,
				const ProductTransaction *pProductTransaction = 0);
		ProductPackageCompleteMsg(const char *buffer);
		~ProductPackageCompleteMsg();


		// --- Getters and setters --- //
		HobRecordNum getRecordNumber() const { return recordNumber_; }
		void setRecordNumber(HobRecordNum number) { recordNumber_ = number; }

		const ProductTransaction * getProductTransaction() const { return pProductTransaction_; }
		void setProductTransaction(const ProductTransaction * trans) { pProductTransaction_ = trans; }

		
		const char *serialize() const;


	private:
		HobRecordNum recordNumber_;
		const ProductTransaction * pProductTransaction_;


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif //PRODUCTPACKAGECOMPLETEMSG_H
