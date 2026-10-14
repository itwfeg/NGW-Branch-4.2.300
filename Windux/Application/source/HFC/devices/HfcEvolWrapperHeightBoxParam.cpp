#include "HfcEvolWrapperHeightBoxParam.h"

#include "HobTokenizerCSV.h"
#include "Configuration.h"

#include "util/e_string/utilstring.h"

#include "Integer.h"


std::list<HfcEvolWrapperHeightBoxParam::HfcEvolWrapperHeightBoxParamFieldOffsets> HfcEvolWrapperHeightBoxParam::browserFieldList;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

	int HfcEvolWrapperHeightBoxParam::_dataType_ = 0;
	std::string HfcEvolWrapperHeightBoxParam::_dataTypeName_("HfcEvolWrapperHeightBoxParam"); 
#endif

HfcEvolWrapperHeightBoxParam::HfcEvolWrapperHeightBoxParam()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();
}


HfcEvolWrapperHeightBoxParam::HfcEvolWrapperHeightBoxParam(const char* buffer, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcEvolWrapperHeightBoxParam", "Constructor passing in a char* buffer");

	init();

	switch (formatType)
	{
		case BrowserFormat:
		{
			Debug::trace("HfcEvolWrapperHeightBoxParam", "Constructor passing in a char* buffer:  BrowserFormat");

			HobString temp(buffer, len), token;
			std::list<HfcEvolWrapperHeightBoxParamFieldOffsets>::iterator fieldIter = browserFieldList.begin();
			int offset;

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);

			while ((tok.next(&token) != -1) && (fieldIter != browserFieldList.end()))
			{
				offset = *fieldIter;
				setReceived(offset, token.length() == 0 ? false : true);
					
				switch (offset)
				{
					case fieldOffsetEvolWrHeightBoxParamPrecedence:
						Debug::trace("HfcEvolWrapperHeightBoxParam", "Constructor passing in a char* buffer:  setting precedence_");
						setPrecedence(stringtoint(token));
						break;
					case fieldOffsetEvolWrHeightBoxParamHeight:
					{
						Debug::trace("HfcEvolWrapperHeightBoxParam", "Constructor passing in a char* buffer:  setting height_");
						token.stripChar(DECIMAL_POINT);
						setHeight(stringtoint(token));
						break;
					}
					case fieldOffsetEvolWrHeightBoxParamAttribute:
						Debug::trace("HfcEvolWrapperHeightBoxParam", "Constructor passing in a char* buffer:  setting attribute_");
						setAttribute(stringtoint(token));
						break;
					case fieldOffsetEvolWrHeightBoxParamAttributeValue:
					{
						Debug::trace("HfcEvolWrapperHeightBoxParam", "Constructor passing in a char* buffer:  setting attributeValue_");
						token.stripChar(DECIMAL_POINT);
						setAttributeValue(stringtoint(token));
						break;
					}
					default:
						break;
				}
				fieldIter++;
			}
			break;
		}
		default:
			// todo - throw exception("invalid format");
			break;
	}
}


HfcEvolWrapperHeightBoxParam::~HfcEvolWrapperHeightBoxParam()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


void HfcEvolWrapperHeightBoxParam::init()
{
	precedence_ = -1;
	height_ = 0;
	attribute_ = -1;
	attributeValue_ = 0;

	initFieldLists();
}


void HfcEvolWrapperHeightBoxParam::initFieldLists()
{
	if (browserFieldList.empty())
	{
		browserFieldList.push_back(fieldOffsetEvolWrHeightBoxParamPrecedence);
		browserFieldList.push_back(fieldOffsetEvolWrHeightBoxParamHeight);
		browserFieldList.push_back(fieldOffsetEvolWrHeightBoxParamAttribute);
		browserFieldList.push_back(fieldOffsetEvolWrHeightBoxParamAttributeValue);
	}
}

void HfcEvolWrapperHeightBoxParam::show() const
{
	if (Debug::isTracing("HfcEvolWrapperHeightBoxParam"))
	{
		std::string debugString;

		for ( int i = 1; i < endHfcEvolWrHeightBoxParamFieldOffsets; ++i )
		{
			switch (i)
			{
				case fieldOffsetEvolWrHeightBoxParamPrecedence:
				{
					debugString.append("Precedence: ").append(inttostring(precedence_));
					break;
				}
				case fieldOffsetEvolWrHeightBoxParamHeight:
				{
					debugString.append(", Height: ").append(inttostring(height_));
					break;
				}
				case fieldOffsetEvolWrHeightBoxParamAttribute:
				{
					debugString.append(", Attribute: ").append(inttostring(attribute_));
					break;
				}
				case fieldOffsetEvolWrHeightBoxParamAttributeValue:
				{
					debugString.append(", Attribute value: ").append(inttostring(attributeValue_));
					break;
				}
			}
		}
		Debug::trace("HfcEvolWrapperHeightBoxParam", debugString);
	}
}


const char* HfcEvolWrapperHeightBoxParam::serialize(int format)const
{
	bool traceOn = Debug::isTracing("HfcEvolWrapperHeightBoxParam");
	if (traceOn)
	{
		HobString msgDebug = "serialize():  format = ";
		msgDebug.append(Integer::toString(format));
		Debug::trace("HfcEvolWrapperHeightBoxParam", msgDebug);
	}
	HobString result;

	serializedData.erase();
	switch ( format )
	{
		case BrowserFormat:
		{
			Debug::trace("HfcEvolWrapperHeightBoxParam", "serialize():  BrowserFormat");
			bool firstTime = true;
			int offset;
			std::list<HfcEvolWrapperHeightBoxParamFieldOffsets>::iterator fieldIter;
			fieldIter = browserFieldList.begin();
			while (fieldIter != browserFieldList.end())
			{
				if (firstTime)
					firstTime = false;
				else
					serializedData.append(BrowserFormatSeparator);

				offset = *fieldIter;
				switch (offset)
				{
					case fieldOffsetEvolWrHeightBoxParamPrecedence:
						serializedData.append(inttostring(precedence_));
						break;
					case fieldOffsetEvolWrHeightBoxParamHeight:
					{
						Configuration::getInstance()->formatDecimalValue(height_, result, 1);
						serializedData.append(result);
						break;
					}
					case fieldOffsetEvolWrHeightBoxParamAttribute:
						serializedData.append(inttostring(attribute_));
						break;
					case fieldOffsetEvolWrHeightBoxParamAttributeValue:
						serializedData.append(inttostring(attributeValue_));
						break;
					default:
						break;
				}
				fieldIter++;
			}
			break;
		}
		default:
			// todo - throw exception("invalid format");
			break;
	}

	if (traceOn)
		Debug::trace("HfcEvolWrapperHeightBoxParam", "serialize():  " + serializedData);

	return serializedData.c_str();
}


void HfcEvolWrapperHeightBoxParam::setAttribute(int value)
{
	attribute_ = value;
}

void HfcEvolWrapperHeightBoxParam::setAttributeValue(int value)
{
	attributeValue_ = value;
}
