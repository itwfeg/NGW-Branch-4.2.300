/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef STORE_H
#define STORE_H

#include "StringType.h"
#include "Data.h"
#include "NumType.h"

class Store : Data
{
	protected:

	Store();

	public:

	static Store * getInstance();

	NumType & getNum();

	void setNum(NumType & num);

	StringType & getName();

	void setName(StringType & name);

	StringType & getAddress();

	void setAddress(StringType & address);

	NumType & getStoreGraphicNum();

	void setStoreGraphicNum(NumType & storeGraphicNum);

	private:

	NumType num;
	StringType name;
	StringType address;
	NumType storeGraphicNum;

	/**
	 * @link
	 * @shapeType PatternLink
	 * @pattern <{Singleton}>
	 * @supplierRole Singleton factory
	 */

    /*# Store _store; */

	static Store * instance;
};

#endif //STORE_H


