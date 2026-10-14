#ifndef PRODUCTSTARTPRINTFORTRANSACTIONMSG_H
#define PRODUCTSTARTPRINTFORTRANSACTIONMSG_H

#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"
//class HobThread;
class ProductTransaction;

class ProductStartPrintForTransactionMsg : public EvolMessage
{
	public:
		ProductStartPrintForTransactionMsg(Hobart::CEvolMsgReceiver *pHobThread = NULL, const ProductTransaction *pProductTransaction = NULL);
		ProductStartPrintForTransactionMsg(const char *buffer);
		~ProductStartPrintForTransactionMsg();

		const char *serialize() const;

		const ProductTransaction *getProductTransaction();
		void setProductTransaction(const ProductTransaction *pProductTransaction);

		bool isOkToPrintTransaction() const;
		void setOkToPrintTransaction(bool ok);

	private:
		ProductTransaction *pProductTransaction;
		bool okToPrintTransaction;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //PRODUCTSTARTPRINTFORTRANSACTIONMSG_H
