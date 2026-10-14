/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Tare.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Tare::dataType = 0;
std::string Tare::dataTypeName("Tare"); 
#endif

Tare::Tare(const Tare& obj)
	: value(obj.value), forced(obj.forced)	
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

Tare::Tare()
	: value((HobWeight)0), forced(false)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

Tare::Tare(HobWeight val)
	: value(val), forced(false)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

Tare::~Tare()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


HobWeight Tare::getValue() const
{
	return value;
}
void Tare::setValue(HobWeight value)
{
	this->value = value;
}

bool Tare::isForced() const
{
	return forced;
}
void Tare::setForced(bool forced)
{
	this->forced = forced;
}

