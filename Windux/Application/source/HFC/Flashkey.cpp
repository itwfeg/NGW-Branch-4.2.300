/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Flashkey.h"
 
void Flashkey::setNum(NumType& num)
{
	this->num = num;
}
NumType& Flashkey::getNum()
{
	return num;
}
void Flashkey::setFkeyType(int fkeyType)
{
	this->fkeyType = fkeyType;
}
int Flashkey::getFkeyType()
{
	return fkeyType;
}
void Flashkey::setVerticalSize(short verticalSize)
{
	this->verticalSize = verticalSize;
}
short Flashkey::getVerticalSize()
{
	return verticalSize;
}
void Flashkey::setHorizontalSize(short horizontalSize)
{
	this->horizontalSize = horizontalSize;
}
short Flashkey::getHorizontalSize()
{
	return horizontalSize;
}
void Flashkey::setRecordNum(NumType& recordNum)
{
	this->recordNum = recordNum;
}
NumType& Flashkey::getRecordNum()
{
	return recordNum;
}
void Flashkey::setDescription(StringType& description)
{
	this->description = description;
}
StringType& Flashkey::getDescription()
{
	return description;
}

