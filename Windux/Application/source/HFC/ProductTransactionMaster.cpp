#include "ProductTransactionMaster.h"
#include "Product.h"

#include "Integer.h"
#include "Long.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



#ifdef USE_ACCOUNTANT
	int ProductTransactionMaster::dataType = 0;
	std::string ProductTransactionMaster::dataTypeName("ProductTransactionMaster"); 
#endif



ProductTransactionMaster::ProductTransactionMaster(Product * product)
	: ProductTransaction(product)
{
	currentproductTransactionListOffset = 0;

#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}


ProductTransactionMaster::~ProductTransactionMaster()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	// clear the transaction list kept by this ProductTransactionMaster
	ProductTransaction *masterTrans = getMasterProductTransaction();
	if (masterTrans)
		masterTrans->setProductTransactionMaster(0);
	clearTransactionList();
	Product *myProd = getProduct();
	if (myProd)
		delete myProd;
}

void getTransactionList()
//std::deque<ProductTransaction *> getTransactionList()
{
	//return productTransactionList;
}

ProductTransaction *ProductTransactionMaster::findTransaction(const ProductTransaction *transactionToFind)
{
	ProductTransaction *foundTransaction = 0;

	if (!productTransactionList.empty())
	{
		std::deque<ProductTransaction *>::iterator pos;
		pos = productTransactionList.begin();
		while (pos != productTransactionList.end())
		{
			if (*pos == transactionToFind)
			{
				foundTransaction = *pos;
				break;
			}
			pos++;
		}
	}
	return foundTransaction;
}

void ProductTransactionMaster::addProductTransaction( ProductTransaction *trans )
{
	// sanity check
	if ( !trans )
		return;

	// First item in list is master record (rollup of items within).
	if ( productTransactionList.size() )
	{
		ProductTransaction *masterTrans;

		// The master record has already been added to the list.  Thus, this transaction being added
		// is one item in the list.  I need to update the master record's counter of the number of items
		// in the list, and the total amount.
		if ( (masterTrans = getMasterProductTransaction()) )
		{
			masterTrans->incrementTotalItems();
			masterTrans->setTotalPrice( masterTrans->getTotalPrice() + trans->getTotalPrice() );
		}
	}

	// FT-1695: update all items to point to the master
	trans->setProductTransactionMaster( this );

	// Only add it to the list if it's not already in the list.
	if ( !findTransaction( trans ) )
		productTransactionList.push_back( trans );
}

// Return the transaction removed from the list, so the mode can return it to the Session within
// a transaction failed message.
ProductTransaction *ProductTransactionMaster::removeItemFromList(int indexInList)
{
	ProductTransaction *transactionToReturn = 0;

	if (productTransactionList.size() > 1)
	{
		// indexInList == 0 means remove the last one added
		if (indexInList == 0 || indexInList == productTransactionList.size() - 1)
		{
			transactionToReturn = productTransactionList[productTransactionList.size()-1];
			productTransactionList.pop_back();
		}
		else if (indexInList > 0 && indexInList < (int) productTransactionList.size())
		{
			transactionToReturn = productTransactionList[indexInList];

			std::deque<ProductTransaction *>::iterator pos = productTransactionList.begin();
			int i = 0;
			while (pos != productTransactionList.end() && i < indexInList)
			{
				pos++;
				i++;
			}
			if (pos != productTransactionList.end() && i == indexInList)
				productTransactionList.erase(pos);
		}
	}

	if (transactionToReturn)
	{
		// We've removed a transaction.  Update the master record for the counter of the number of items
		// in the list, and the total amount.
		ProductTransaction *masterTrans = getMasterProductTransaction();
		if (masterTrans)
		{
			masterTrans->decrementTotalItems();
			masterTrans->setTotalPrice(masterTrans->getTotalPrice() - transactionToReturn->getTotalPrice());
		}
	}
	return transactionToReturn;
}

// Note: This also removes the master transaction kept in the first position, element 0.
void ProductTransactionMaster::clearTransactionList()
{
	productTransactionList.clear();
}

void ProductTransactionMaster::removeAllIndividualTransactionsFromTransactionList()
{
	if (productTransactionList.size() > 1)
	{
		for (size_t i = productTransactionList.size(); i > 1; i--)
			productTransactionList.pop_back();
	}
}


ProductTransaction *ProductTransactionMaster::getFirstTransaction()
{
	/*
	if (!productTransactionList.empty())
	{
		std::deque<ProductTransaction *>::const_iterator tempIter = productTransactionList.begin();
		if (tempIter == productTransactionList.end())
			return 0;

		/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
		 * the iterator for the entire use of the query (which extends beyond this function call).
		 * /
		productTransactionListIterator = new std::deque<ProductTransaction *>::const_iterator(tempIter);

		++(*productTransactionListIterator);
		if (*productTransactionListIterator == productTransactionList.end())
			return 0;

		return *(*productTransactionListIterator);
	}
*/
	currentproductTransactionListOffset = 1;
	if (productTransactionList.size() > (size_t) currentproductTransactionListOffset)
		return productTransactionList[currentproductTransactionListOffset];
	return 0;
}

ProductTransaction *ProductTransactionMaster::getNextTransaction()
{
	currentproductTransactionListOffset++;
	if (productTransactionList.size() > (size_t) currentproductTransactionListOffset)
		return productTransactionList[currentproductTransactionListOffset];
	return 0;
}

// This returns item transactions in the list, not the master transaction.
// Thus, when a 1 is passed in for 'position', we will return the second transaction in the list,
// since the master transaction is the first in the list.
ProductTransaction *ProductTransactionMaster::getTransactionPositionNum(int position)
{
	if (productTransactionList.size() > (size_t) position)
		return productTransactionList[position];
	return 0;
}

const ProductTransaction *ProductTransactionMaster::getTransactionPositionNum(int position) const
{
	if (productTransactionList.size() > (size_t) position)
		return productTransactionList[position];
	return 0;
}

// This returns the count of the item transactions in the list, not counting the master transaction.
int ProductTransactionMaster::getTransactionCount() const
{
	int size = 0;
	if (productTransactionList.size() > 1)
		size = (int)productTransactionList.size() - 1;
	return size;
}

Product *ProductTransactionMaster::getMasterProduct() const
{
	if (!productTransactionList.empty())
	{
		ProductTransaction *trans = *(productTransactionList.begin());
		if (trans)
			return trans->getProduct();
	}
	return 0;
}
ProductTransaction *ProductTransactionMaster::getMasterProductTransaction() const
{
	if (!productTransactionList.empty())
		return *(productTransactionList.begin());
	return 0;
}
