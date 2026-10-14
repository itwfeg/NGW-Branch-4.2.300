/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */



#ifndef HFCPRODUCTLABELFORMSIZEMSG_H
#define HFCPRODUCTLABELFORMSIZEMSG_H



#include "HfcObject.h"



typedef enum ProductLabelFormSizeFieldOffsets
{
	fieldOffsetProductNumber = 1,
	fieldOffsetTextType,
	fieldOffsetVerticalSize,
	fieldOffsetHorizontalSize,
	fieldOffsetDefaultFontName,
	fieldOffsetDefaultPointSize,
	endProductLabelFormSizeFieldOffsets
} ProductLabelFormSizeFieldOffsets_t;


class HfcProductLabelFormSizeMsg : public HfcObject
{
	public:
		// --- Constructors and destructor ---
//		HfcProductLabelFormSizeMsg();
		HfcProductLabelFormSizeMsg(const char *data, int len, int formatType);

		~HfcProductLabelFormSizeMsg();


		// --- Accessor methods ---
		HobRecordNum getProductNumber() const { return productNumber_; }
		void setProductNumber(HobRecordNum number) { productNumber_ = number; }

		int getLabelFormType() const { return type_; }
		void setLabelFormType(int type) { type_ = type; }

		int getLabelFormWidth() const { return width_; }
		void setLabelFormWidth(int width) { width_ = width; }

		int getLabelFormHeight() const { return height_; }
		void setLabelFormHeight(int height) { height_ = height; }

		const std::string& getDefaultFontName() const { return defaultFontName_; }
		void setDefaultFontName(const std::string& fontName) { defaultFontName_ = fontName; }

		int getDefaultPointSize() const { return defaultPointSize_; }
		void setDefaultPointSize(int pointSize) { defaultPointSize_ = pointSize; }


		// --- Processing methods ---
		virtual const char *serialize(int format) const;


	private:
		// --- Data elements ---
		HobRecordNum productNumber_;
		int type_;
		int width_;
		int height_;
		std::string defaultFontName_;
		int defaultPointSize_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif //HFCPRODUCTLABELFORMSIZEMSG_H

