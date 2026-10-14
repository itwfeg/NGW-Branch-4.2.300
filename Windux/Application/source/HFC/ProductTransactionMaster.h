#ifndef PRODUCTTRANSACTIONMASTER_H
#define PRODUCTTRANSACTIONMASTER_H



#include "ProductTransaction.h"


#include "BarCodeNumber.h"
#include "FrequentShopper.h"

class Product;
class HobOperator;
class HfcWeigherStatus;

#include "HobDateTime.h"
#include <deque>



class ProductTransactionMaster : public ProductTransaction
{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		ProductTransactionMaster(Product * product = 0);
		~ProductTransactionMaster();

		/* --- Getters and setters --- */
		//std::deque<ProductTransaction *> getTransactionList();
		void getTransactionList();
		ProductTransaction *findTransaction(const ProductTransaction *transactionToFind);
		void addProductTransaction(ProductTransaction *trans);
		ProductTransaction *removeItemFromList(int indexInList);
		void clearTransactionList();
		void removeAllIndividualTransactionsFromTransactionList();

		ProductTransaction *getFirstTransaction();
		ProductTransaction *getNextTransaction();
		ProductTransaction *getTransactionPositionNum(int position);
		const ProductTransaction *getTransactionPositionNum(int position) const;
		int getTransactionCount() const;

		Product *getMasterProduct() const;
		ProductTransaction *getMasterProductTransaction() const;


	private:

		std::deque<ProductTransaction *> productTransactionList;
		int currentproductTransactionListOffset;
		//std::deque<ProductTransaction *>::const_iterator *productTransactionListIterator;

		/* --- Instance function members --- */

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif //PRODUCTTRANSACTIONMASTER_H
