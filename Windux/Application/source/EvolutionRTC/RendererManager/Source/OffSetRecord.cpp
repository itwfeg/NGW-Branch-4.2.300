#include "OffsetRecord.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OffsetRecord::dataType = 0;
std::string OffsetRecord::dataTypeName("OffsetRecord"); 
#endif

OffsetRecord::OffsetRecord(const OffsetRecord& obj):line(obj.line),offset(obj.offset) 
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

OffsetRecord::OffsetRecord(INT16 line, INT16 offset)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->line = line;
	this->offset = offset;
}


OffsetRecord::~OffsetRecord()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


INT16 OffsetRecord::getLine() const
{
	return line;
}


void OffsetRecord::setLine(INT16 labelLines)
{
	line = labelLines;
}


INT16 OffsetRecord::getOffSet() const
{
	return offset;
}


void OffsetRecord::setOffSet(INT16 fieldOffset)
{
	offset = fieldOffset; 
}
