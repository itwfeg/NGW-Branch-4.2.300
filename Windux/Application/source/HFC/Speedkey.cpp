/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Speedkey.h"

void Speedkey::setNum(NumType& num)
{
	this->num = num;
}
NumType& Speedkey::getNum()
{
	return num;
}
void Speedkey::setFlags(int flags)
{
	this->flags = flags;
}
int Speedkey::getFlags()
{
	return flags;
}
void Speedkey::setMacro(StringType& macro)
{
	this->macro = macro;
}
StringType& Speedkey::getMacro()
{
	return macro;
}
void Speedkey::setDescription(StringType& description)
{
	this->description = description;
}
StringType& Speedkey::getDescription()
{
	return description;
}

