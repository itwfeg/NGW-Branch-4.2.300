/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRINTERTAGS_H
#define PRINTERTAGS_H

#include "Tags.h"


class PrinterTags : public Tags
{
	public:

		typedef enum PrinterTagFieldOffsets
		{
		//	FirstPrinter
			fieldOffsetHorizontalPrintPosition1 = 1,
			fieldOffsetVerticalPrintPosition1,
			fieldOffsetPrintContrast1,
		//	SecondPrinter
			fieldOffsetHorizontalPrintPosition2,
			fieldOffsetVerticalPrintPosition2,
			fieldOffsetPrintContrast2,
		//	ThirdPrinter
			fieldOffsetHorizontalPrintPosition3,
			fieldOffsetVerticalPrintPosition3,
			fieldOffsetPrintContrast3,
			endPrinterTagFieldOffsets
		} PrinterTagFieldOffsets_t;

		PrinterTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~PrinterTags();

	protected:
		const TagsList &getMyTagsList() const;
		TagsMap &getMyTagsMap() const;

	private:
//		Primary Printer settings
		static const std::string horizontalPrintPosition1;	
		static const std::string verticalPrintPosition1;	
		static const std::string printContrast1;	

//		Second Printer settings
		static const std::string horizontalPrintPosition2;	
		static const std::string verticalPrintPosition2;	
		static const std::string printContrast2;	

//		Third Printer settings
		static const std::string horizontalPrintPosition3;	
		static const std::string verticalPrintPosition3;	
		static const std::string printContrast3;	

		static TagsList printerTags;
		static TagsMap  fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//PRINTERTAGS_H
