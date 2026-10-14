#include "HfcEvolWrapperHeightBox.h"
#include "FpmManager.h"

#include "HobTokenizerCSV.h"
#include "Configuration.h"

#include "util/e_string/utilstring.h"

#include "Bool.h"


std::list<HfcEvolWrapperHeightBox::HfcEvolWrapperHeightBoxFieldOffsets> HfcEvolWrapperHeightBox::browserFieldList;
std::list<HfcEvolWrapperHeightBox::HfcEvolWrapperHeightBoxFieldOffsets> HfcEvolWrapperHeightBox::createFieldList;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

	int HfcEvolWrapperHeightBox::_dataType_ = 0;
	std::string HfcEvolWrapperHeightBox::_dataTypeName_("HfcEvolWrapperHeightBox"); 
#endif

HfcEvolWrapperHeightBox::HfcEvolWrapperHeightBox(int precedence, const WrapBox::HeightBox& heightBox)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();

	precedence_ = precedence;
	height_ = heightBox.getHeight();
	if ((unsigned int) height_ == WrapBox::_MaxHeight_)
		canDelete_ = false;
	else
		canDelete_ = true;

	FpmManager* pSerialFlash = FpmManager::getInstance();
	int paramListLen = WrapBox::_Use1P_LAST_PARAM_;				// 1P by default with 19 params, but we carry 21 
	if (pSerialFlash->isWrapperTwoPrinter())
		paramListLen = WrapBox::_Use2P_LAST_PARAM_;

	attributes_.reserve(paramListLen);
	for ( int i = 0; i < paramListLen; ++i )					// had to drop because we can now have either 21 or 19 parameters
		attributes_.push_back(heightBox.getParameter(i));
}


HfcEvolWrapperHeightBox::HfcEvolWrapperHeightBox(const char* buffer, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();

	switch (formatType)
	{
		case BrowserFormat:
		{
			Debug::trace("HfcEvolWrapperHeightBox", "Constructor passing in a char* buffer:  BrowserFormat");

			HobString temp(buffer, len), token;
			std::list<HfcEvolWrapperHeightBoxFieldOffsets>::iterator fieldIter = browserFieldList.begin();
			int offset;

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);

			while ((tok.next(&token) != -1) && (fieldIter != browserFieldList.end()))
			{
				offset = *fieldIter;
				setReceived(offset, token.length() == 0 ? false : true);
					
				switch (offset)
				{
					case fieldOffsetEvolWrHeightBoxPrecedence:
						setPrecedence(stringtoint(token));
						break;
					case fieldOffsetEvolWrHeightBoxHeight:
					{
						token.stripChar(DECIMAL_POINT);
						setHeight(stringtoint(token));
						break;
					}
					case fieldOffsetEvolWrHeightBoxAttributes:
					{
						int i = 0, numAttributes = stringtoint(token);

						// We'll now read each of the wrapping attributes
						if ( tok.next(&token) != -1 )
						{
							// Prepare to read each of the attributes for the height boxes
							token.unQuoteForCSV();

							HobString token2;
							HobTokenizerCSV tok2(&token, BrowserFormatSeparator);

							// We'll now read each of the attributes
							attributes_.reserve(numAttributes);
							while ( tok2.next(&token2) != -1 )
							{
								if ( i >= numAttributes )
									break;

								attributes_.push_back(stringtoint(token2));

								++i;
							}
						}
						break;
					}
					default:
						break;
				}
				fieldIter++;
			}
			break;
		}

		case CreateFormat:
		{
			Debug::trace("HfcEvolWrapperHeightBox", "Constructor passing in a char* buffer:  CreateFormat");

			HobString temp(buffer, len), token;
			std::list<HfcEvolWrapperHeightBoxFieldOffsets>::iterator fieldIter = createFieldList.begin();
			int offset;

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);

			while ((tok.next(&token) != -1) && (fieldIter != createFieldList.end()))
			{
				offset = *fieldIter;
				setReceived(offset, token.length() == 0 ? false : true);
					
				switch (offset)
				{
					case fieldOffsetEvolWrHeightBoxPrecedence:
						setPrecedence(stringtoint(token));
						break;
					case fieldOffsetEvolWrHeightBoxHeight:
					{
						token.stripChar(DECIMAL_POINT);
						setHeight(stringtoint(token));
						break;
					}
					case fieldOffsetEvolWrHeightBoxNewHeight:
					{
						token.stripChar(DECIMAL_POINT);
						setNewHeight(stringtoint(token));
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


HfcEvolWrapperHeightBox::~HfcEvolWrapperHeightBox()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


void HfcEvolWrapperHeightBox::init()
{
	precedence_ = -1;
	height_ = 0;
	newHeight_ = 0;
	attributes_.clear();
	canDelete_ = false;

	initFieldLists();
}


void HfcEvolWrapperHeightBox::initFieldLists()
{
	if (browserFieldList.empty())
	{
		browserFieldList.push_back(fieldOffsetEvolWrHeightBoxPrecedence);
		browserFieldList.push_back(fieldOffsetEvolWrHeightBoxHeight);
		browserFieldList.push_back(fieldOffsetEvolWrHeightBoxAttributes);
		browserFieldList.push_back(fieldOffsetEvolWrHeightBoxCanDelete);
	}
	if (createFieldList.empty())
	{
		createFieldList.push_back(fieldOffsetEvolWrHeightBoxPrecedence);
		createFieldList.push_back(fieldOffsetEvolWrHeightBoxNewHeight);
		createFieldList.push_back(fieldOffsetEvolWrHeightBoxHeight);
	}
}


bool HfcEvolWrapperHeightBox::update(WrapBox::HeightBox& heightBox) const
{
	bool hasChanged = false;

	FpmManager* pSerialFlash = FpmManager::getInstance();
 
	int paramListLen = WrapBox::_Use1P_LAST_PARAM_;				// 1P by default with 19 params, but we carry 21 
	if (pSerialFlash->isWrapperTwoPrinter())
		paramListLen = WrapBox::_Use2P_LAST_PARAM_;

	Debug::trace("WrapBox", "WrapBox::show() number of Params: " + util_string::inttostring(paramListLen));

	if ( isReceived(fieldOffsetEvolWrHeightBoxAttributes) )
	{
		for ( int i = 0; i < paramListLen; ++i )
		{
			if (heightBox.getParameter(i) != attributes_[i])
			{
				heightBox.setParameter(i, attributes_[i]);
				hasChanged = true;
			}
		}
	}
	return hasChanged;
}


void HfcEvolWrapperHeightBox::show() const
{
	bool isTracing = Debug::isTracing("HfcEvolWrapperHeightBox");
	std::string debugString;

	if ( !isTracing )
		Debug::addTraceCategory("HfcEvolWrapperHeightBox");

	
	for ( int i = 1; i < endHfcEvolWrHeightBoxFieldOffsets; ++i )
	{
		switch (i)
		{
			case fieldOffsetEvolWrHeightBoxPrecedence:
			{
				debugString.append("Precedence ").append(inttostring(precedence_));
				break;
			}
			case fieldOffsetEvolWrHeightBoxHeight:
			{
				debugString.append(", Height '").append(inttostring(height_));
				break;
			}
			case fieldOffsetEvolWrHeightBoxAttributes:
			{
				int attributes = getNumberAttributes();
				debugString.append("', # Attributes ").append(inttostring(attributes));

				debugString.append(", Attributes: ");
				for ( int i = 0; i < attributes; ++i )
					debugString.append(inttostring(attributes_[i])).append(", ");

				break;
			}
			case fieldOffsetEvolWrHeightBoxNewHeight:
			{
				debugString.append(", New Height '").append(inttostring(newHeight_));
				break;
			}
			case fieldOffsetEvolWrHeightBoxCanDelete:
			{
				debugString.append(", Can delete '").append(booltostring(canDelete_));
				break;
			}
		}
	}

	Debug::trace("HfcEvolWrapperHeightBox", debugString);


	if ( !isTracing )
		Debug::removeTraceCategory("HfcEvolWrapperHeightBox");
}


const char* HfcEvolWrapperHeightBox::serialize(int format)const
{
	bool traceOn = Debug::isTracing("HfcEvolWrapperHeightBox");
	if (traceOn)
	{
		HobString msgDebug = "serialize():  format = ";
		msgDebug.append(Integer::toString(format));
		Debug::trace("HfcEvolWrapperHeightBox", msgDebug);
	}
	HobString result;

	serializedData.erase();
	switch ( format )
	{
		case BrowserFormat:
		{
			Debug::trace("HfcEvolWrapperHeightBox", "serialize():  BrowserFormat");
			bool firstTime = true;
			int offset;
			std::list<HfcEvolWrapperHeightBoxFieldOffsets>::iterator fieldIter;
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
					case fieldOffsetEvolWrHeightBoxPrecedence:
						serializedData.append(inttostring(precedence_));
						break;
					case fieldOffsetEvolWrHeightBoxHeight:
					{
						Configuration::getInstance()->formatDecimalValue(height_, result, 1);
						serializedData.append(result);
						break;
					}
					case fieldOffsetEvolWrHeightBoxAttributes:
					{
						int attributes = getNumberAttributes();
						serializedData.append(inttostring(attributes));
						HobString listContents;

						for ( int i = 0; i < attributes; ++i )
						{
							if ( i )
								listContents.append(BrowserFormatSeparator);

							if ( i == WrapBox::GRIPPER_ADJUST_PARAMETER || i == WrapBox::FUF_ADJUST_PARAMETER || i == WrapBox::SUF_ADJUST_PARAMETER )
								listContents.append(inttostring(attributes_[i]));
							else
								listContents.append(uinttostring(attributes_[i]));
						}

						listContents.quoteForCSV();
						serializedData.append(BrowserFormatSeparator);
						serializedData.append(listContents);

						break;
					}
					case fieldOffsetEvolWrHeightBoxCanDelete:
					{
						serializedData.append(Bool::toString(canDelete_));
						break;
					}
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
		Debug::trace("HfcEvolWrapperHeightBox", "serialize():  " + serializedData);

	return serializedData.c_str();
}
