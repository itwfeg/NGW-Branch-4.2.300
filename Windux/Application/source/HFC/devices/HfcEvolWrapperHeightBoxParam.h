#ifndef HFCEVOLWRAPPERHEIGHTBOXPARAM_H
#define HFCEVOLWRAPPERHEIGHTBOXPARAM_H


#include <string>
#include <list>

#include "HfcObject.h"


class HfcEvolWrapperHeightBoxParam : public HfcObject
{
	public:
		// --- Types and subtypes ---
		typedef enum HfcEvolWrapperHeightBoxParamFieldOffsets
		{
			fieldOffsetEvolWrHeightBoxParamPrecedence = 1,
			fieldOffsetEvolWrHeightBoxParamHeight,
			fieldOffsetEvolWrHeightBoxParamAttribute,
			fieldOffsetEvolWrHeightBoxParamAttributeValue,

			endHfcEvolWrHeightBoxParamFieldOffsets	//	MUST be last!
		} HfcEvolWrapperHeightBoxParamFieldOffsets_t;

		const static char DECIMAL_POINT = '.';


		// --- Constructors and destructor ---
		HfcEvolWrapperHeightBoxParam();
		HfcEvolWrapperHeightBoxParam(const char* buffer, int len, int formatType);
		~HfcEvolWrapperHeightBoxParam();


		// --- Accessor methods ---
		int getPrecedence() const { return precedence_; }
		void setPrecedence(int p) { precedence_ = p; }

		int getHeight() const { return height_; }
		void setHeight(int height) { height_ = height; }

		int getAttribute() const { return attribute_; }
		void setAttribute(int value);

		int getAttributeValue() const { return attributeValue_; }
		void setAttributeValue(int value);


		// --- Processing methods ---
		void init();
		void initFieldLists();
		void show() const;
		const char* serialize(int format)const;


	private:
		// --- Instance Members ---
		int precedence_;
		int height_;
		int attribute_;
		int attributeValue_;


		static std::list<HfcEvolWrapperHeightBoxParam::HfcEvolWrapperHeightBoxParamFieldOffsets> browserFieldList;

		// --- Class members ---
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif

};

#endif /* HFCEVOLWRAPPERHEIGHTBOXPARAM_H */
