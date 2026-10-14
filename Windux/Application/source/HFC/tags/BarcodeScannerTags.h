/**
 * @file BarcodeScannerTags.h
 * @brief BarcodeScannerTags tracks all barcodes that have been scanned by the usb scanner.  Currently the
 * Scanner is intended to be attached to an elevated self service device. The customer will have
 * the ability to walk up to a elevated self service scale with their produce item and hold it under
 * the scanner and the barcode sticker attached to the item will be used to look up a PLU which the
 * customer can then print a label for purchase.
 *
 * @copyright Copyright (c) 2015, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup BarcodeScannerTagsCsu
 * @{
*/
#ifndef BARCODESCANNERTAGS_H
#define BARCODESCANNERTAGS_H

#include "Tags.h"

class BarcodeScannerTags : public Tags
{
	public:
		BarcodeScannerTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~BarcodeScannerTags();

		static const std::string scannerCode;
		static const std::string scannerProductNum;

	protected:
		const TagsList &getMyTagsList() const {return barcodeScannerTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList barcodeScannerTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//BARCODESCANNERTAGS_H 
