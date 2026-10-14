/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "PrintedLabelFields.h"
 
bool PrintedLabelFields::shouldPrint(int field)
{
}
void PrintedLabelFields::setPreprintTextNames(bool preprintTextNames)
{
	this->preprintTextNames = preprintTextNames;
}
bool PrintedLabelFields::getPreprintTextNames()
{
	return preprintTextNames;
}
void PrintedLabelFields::setSecondCheckDigit(bool secondCheckDigit)
{
	this->secondCheckDigit = secondCheckDigit;
}
bool PrintedLabelFields::getSecondCheckDigit()
{
	return secondCheckDigit;
}
void PrintedLabelFields::setBarCode(bool barCode)
{
	this->barCode = barCode;
}
bool PrintedLabelFields::getBarCode()
{
	return barCode;
}
void PrintedLabelFields::setWeightOnByCount(bool weightOnByCount)
{
	this->weightOnByCount = weightOnByCount;
}
bool PrintedLabelFields::getWeightOnByCount()
{
	return weightOnByCount;
}
void PrintedLabelFields::setByCount(bool byCount)
{
	this->byCount = byCount;
}
bool PrintedLabelFields::getByCount()
{
	return byCount;
}
void PrintedLabelFields::setUnitPrice(bool unitPrice)
{
	this->unitPrice = unitPrice;
}
bool PrintedLabelFields::getUnitPrice()
{
	return unitPrice;
}
void PrintedLabelFields::setProductLifeDateJulian(bool productLifeDateJulian)
{
	this->productLifeDateJulian = productLifeDateJulian;
}
bool PrintedLabelFields::getProductLifeDateJulian()
{
	return productLifeDateJulian;
}
void PrintedLabelFields::setProductLifeDate(bool productLifeDate)
{
	this->productLifeDate = productLifeDate;
}
bool PrintedLabelFields::getProductLifeDate()
{
	return productLifeDate;
}
void PrintedLabelFields::setPackedOnTime(bool packedOnTime)
{
	this->packedOnTime = packedOnTime;
}
bool PrintedLabelFields::getPackedOnTime()
{
	return packedOnTime;
}
void PrintedLabelFields::setPackedOnDateJulian(bool packedOnDateJulian)
{
	this->packedOnDateJulian = packedOnDateJulian;
}
bool PrintedLabelFields::getPackedOnDateJulian()
{
	return packedOnDateJulian;
}
void PrintedLabelFields::setPackedOnDate(bool packedOnDate)
{
	this->packedOnDate = packedOnDate;
}
bool PrintedLabelFields::getPackedOnDate()
{
	return packedOnDate;
}
void PrintedLabelFields::setSellByDateJulian(bool sellByDateJulian)
{
	this->sellByDateJulian = sellByDateJulian;
}
bool PrintedLabelFields::getSellByDateJulian()
{
	return sellByDateJulian;
}
void PrintedLabelFields::setSellByDate(bool sellByDate)
{
	this->sellByDate = sellByDate;
}
bool PrintedLabelFields::getSellByDate()
{
	return sellByDate;
}

PrintedLabelFields * PrintedLabelFields::getInstance()
{
	if (instance == 0)
	{
		instance = new PrintedLabelFields();
	}
	return instance;
}

PrintedLabelFields * PrintedLabelFields::instance = 0;
PrintedLabelFields::PrintedLabelFields()
{
}

