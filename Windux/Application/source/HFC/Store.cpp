/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Store.h"
 
void Store::setNum(NumType & num)
{
	this->num = num;
}
NumType& Store::getNum()
{
	return num;
}
void Store::setName(StringType &  name)
{
	this->name = name;
}
StringType& Store::getName()
{
	return name;
}
void Store::setAddress(StringType & address)
{
	this->address = address;
}
StringType& Store::getAddress()
{
	return address;
}
void Store::setStoreGraphicNum(NumType & storeGraphicNum)
{
	this->storeGraphicNum = storeGraphicNum;
}
NumType& Store::getStoreGraphicNum()
{
	return storeGraphicNum;
}

Store * Store::getInstance()
{
	if (instance == 0)
	{
		instance = new Store();
	}
	return instance;
}

Store * Store::instance = 0;
Store::Store()
{
}

