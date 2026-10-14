/* Evolution Scale Project
 * COPYRIGHT (c) 2014.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "PrinterTags.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterTags::_dataType_ = 0;
std::string PrinterTags::_dataTypeName_("PrinterTags"); 
#endif

//	Primary printer settings
const std::string PrinterTags::horizontalPrintPosition1	= "1h";
const std::string PrinterTags::verticalPrintPosition1	= "1v";
const std::string PrinterTags::printContrast1			= "1c";

//	Second printer settings
const std::string PrinterTags::horizontalPrintPosition2	= "2h";
const std::string PrinterTags::verticalPrintPosition2	= "2v";
const std::string PrinterTags::printContrast2			= "2c";

//	Third printer settings
const std::string PrinterTags::horizontalPrintPosition3	= "3h";
const std::string PrinterTags::verticalPrintPosition3	= "3v";
const std::string PrinterTags::printContrast3			= "3c";

TagsList PrinterTags::printerTags(endPrinterTagFieldOffsets);
TagsMap  PrinterTags::fieldMap;	//example pair: r#,fieldOffsetTextNum


PrinterTags::PrinterTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	if (pFieldList)
	{
		//initialize the the table of tags for the Statistics class
		printerTags[0] 										= emptyTag;

//		First Printer settings
		printerTags[fieldOffsetHorizontalPrintPosition1]	= horizontalPrintPosition1;
		printerTags[fieldOffsetVerticalPrintPosition1]		= verticalPrintPosition1;
		printerTags[fieldOffsetPrintContrast1]				= printContrast1;

//		Second Printer settings
		printerTags[fieldOffsetHorizontalPrintPosition2]	= horizontalPrintPosition2;
		printerTags[fieldOffsetVerticalPrintPosition2]		= verticalPrintPosition2;
		printerTags[fieldOffsetPrintContrast2]				= printContrast2;

//		Third Printer settings
		printerTags[fieldOffsetHorizontalPrintPosition3]	= horizontalPrintPosition3;
		printerTags[fieldOffsetVerticalPrintPosition3]		= verticalPrintPosition3;
		printerTags[fieldOffsetPrintContrast3]				= printContrast3;

		putTagsInMap(pFieldList);
	}
}

PrinterTags::~PrinterTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

}

const TagsList &PrinterTags::getMyTagsList() const
{
	return printerTags;
}

TagsMap &PrinterTags::getMyTagsMap() const
{
	return fieldMap;
}
