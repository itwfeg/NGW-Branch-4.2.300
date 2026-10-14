#include "HfcEvolWrapperWrapBox.h"

#include "WrapBox.h"
#include "HobTokenizerCSV.h"
#include "Configuration.h"

#include "util/e_string/utilstring.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

	int HfcEvolWrapperWrapBox::_dataType_ = 0;
	std::string HfcEvolWrapperWrapBox::_dataTypeName_("HfcEvolWrapperWrapBox"); 
#endif


HfcEvolWrapperWrapBox::HfcEvolWrapperWrapBox(int precedence, const WrapBox& wrapBox)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();

	precedence_ = precedence;
	name_.assign(wrapBox.getName());
	pluException_ = wrapBox.getPLUException();
	minLength_ = wrapBox.getLengthLow();
	maxLength_ = wrapBox.getLengthHigh();
	minWidth_ = wrapBox.getWidthLow();
	maxWidth_ = wrapBox.getWidthHigh();
	lipHeight_ = wrapBox.getLip();

	int heightBoxes = wrapBox.getNumberHeightBoxes();
	heights_.reserve(heightBoxes);
	WrapBox::const_iterator iter = wrapBox.begin();
	while ( iter != wrapBox.end() )
		heights_.push_back((iter++)->getHeight());
}


HfcEvolWrapperWrapBox::HfcEvolWrapperWrapBox(const char* buffer, int len, int formatType)
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
			short loopCnt = 1;
			HobString temp(buffer, len), token;
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);

			while ( tok.next(&token) != -1 )
			{
				setReceived(loopCnt, token.length() == 0 ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetEvolWrWrapBoxPrecedence:
						setPrecedence(stringtoint(token));
						break;
					case fieldOffsetEvolWrWrapBoxName:
					{
						token.unQuoteForCSV();
						setName(token);
						break;
					}
					case fieldOffsetEvolWrWrapBoxPLUException:
						setPLUException(stringtoint(token));
						break;
					case fieldOffsetEvolWrWrapBoxMinLength:
					{
						token.stripChar(DECIMAL_POINT);
						setMinLength(stringtoint(token));
						break;
					}
					case fieldOffsetEvolWrWrapBoxMaxLength:
					{
						token.stripChar(DECIMAL_POINT);
						setMaxLength(stringtoint(token));
						break;
					}
					case fieldOffsetEvolWrWrapBoxMinWidth:
					{
						token.stripChar(DECIMAL_POINT);
						setMinWidth(stringtoint(token));
						break;
					}
					case fieldOffsetEvolWrWrapBoxMaxWidth:
					{
						token.stripChar(DECIMAL_POINT);
						setMaxWidth(stringtoint(token));
						break;
					}
					case fieldOffsetEvolWrWrapBoxLipHeight:
					{
						token.stripChar(DECIMAL_POINT);
						setLipHeight(stringtoint(token));
						break;
					}
					case fieldOffsetEvolWrWrapBoxHeightBoxes:
					{
						int i = 0, numHeightBoxes = stringtoint(token);

						if ( tok.next(&token) != -1 )
						{
							// Prepare to read each of the heights for the height boxes
							token.unQuoteForCSV();

							HobString token2;
							HobTokenizerCSV tok2(&token, BrowserFormatSeparator);

							// We'll now read each of the heights
							heights_.reserve(numHeightBoxes);
							while ( tok2.next(&token2) != -1 )
							{
								if ( i >= numHeightBoxes )
									break;

								token2.stripChar(DECIMAL_POINT);
								heights_.push_back(stringtoint(token2));

								++i;
							}
						}
						break;
					}
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}
		default:
			// todo - throw exception("invalid format");
			break;
	}
}


HfcEvolWrapperWrapBox::~HfcEvolWrapperWrapBox()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


void HfcEvolWrapperWrapBox::init()
{
	precedence_ = -1;
	name_.erase();
	pluException_ = 0;
	minLength_ = 0;
	maxLength_ = 0;
	minWidth_ = 0;
	maxWidth_ = 0;
	lipHeight_ = 0;
	heights_.clear();
}


bool HfcEvolWrapperWrapBox::nameOrPLUChanged(const WrapBox& wrapBox) const
{
	if ( isReceived(fieldOffsetEvolWrWrapBoxName) && getName() != wrapBox.getName() )
		return true;

	if ( isReceived(fieldOffsetEvolWrWrapBoxPLUException) && getPLUException() != wrapBox.getPLUException() )
		return true;

	return false;
}


bool HfcEvolWrapperWrapBox::update(WrapBox& wrapBox) const
{
	bool hasChanged = false;
	for ( int i = 1; i < endHfcEvolWrWrapBoxFieldOffsets; ++i )
	{
		if ( isReceived(i) )
		{
			switch ( i )
			{
				case fieldOffsetEvolWrWrapBoxName:
					if (wrapBox.getName() != getName())
					{
						wrapBox.setName(getName());
						hasChanged = true;
					}
					break;
				case fieldOffsetEvolWrWrapBoxPLUException:
					if (wrapBox.getPLUException() != getPLUException())
					{
						wrapBox.setPLUException(getPLUException());
						hasChanged = true;
					}
					break;
				case fieldOffsetEvolWrWrapBoxMinLength:
					if (wrapBox.getLengthLow() != getMinLength())
					{
						wrapBox.setLengthLow(getMinLength());
						hasChanged = true;
					}
					break;
				case fieldOffsetEvolWrWrapBoxMaxLength:
					if (wrapBox.getLengthHigh() != getMaxLength())
					{
						wrapBox.setLengthHigh(getMaxLength());
						hasChanged = true;
					}
					break;
				case fieldOffsetEvolWrWrapBoxMinWidth:
					if (wrapBox.getWidthLow() != getMinWidth())
					{
						wrapBox.setWidthLow(getMinWidth());
						hasChanged = true;
					}
					break;
				case fieldOffsetEvolWrWrapBoxMaxWidth:
					if (wrapBox.getWidthHigh() != getMaxWidth())
					{
						wrapBox.setWidthHigh(getMaxWidth());
						hasChanged = true;
					}
					break;
				case fieldOffsetEvolWrWrapBoxLipHeight:
					if (wrapBox.getLip() != getLipHeight())
					{
						wrapBox.setLip(getLipHeight());
						hasChanged = true;
					}
					break;
				default:
					break;
			}
		}
	}
	return hasChanged;
}


void HfcEvolWrapperWrapBox::show(const std::string& cat) const
{
	bool isTracing = false;
	std::string category(cat);
	std::string debugString;

	if ( category.empty() )
		category.assign("HfcEvolWrapperWrapBox");
	isTracing = Debug::isTracing(category);

	if ( !isTracing )
		return;

	for ( int i = 1; i < endHfcEvolWrWrapBoxFieldOffsets; ++i )
	{
		switch (i)
		{
			case fieldOffsetEvolWrWrapBoxPrecedence:
			{
				debugString.append("Precedence ").append(inttostring(precedence_));
				break;
			}
			case fieldOffsetEvolWrWrapBoxName:
			{
				debugString.append(", Name '").append(name_);
				break;
			}
			case fieldOffsetEvolWrWrapBoxPLUException:
			{
				debugString.append("', PLUException ").append(inttostring(pluException_));
				break;
			}
			case fieldOffsetEvolWrWrapBoxMinLength:
			{
				debugString.append("', MinLength ").append(inttostring(minLength_));
				break;
			}
			case fieldOffsetEvolWrWrapBoxMaxLength:
			{
				debugString.append("', MaxLength ").append(inttostring(maxLength_));
				break;
			}
			case fieldOffsetEvolWrWrapBoxMinWidth:
			{
				debugString.append("', MinWidth ").append(inttostring(minWidth_));
				break;
			}
			case fieldOffsetEvolWrWrapBoxMaxWidth:
			{
				debugString.append("', MaxWidth ").append(inttostring(maxWidth_));
				break;
			}
			case fieldOffsetEvolWrWrapBoxLipHeight:
			{
				debugString.append("', LipHeight ").append(inttostring(lipHeight_));
				break;
			}
			case fieldOffsetEvolWrWrapBoxHeightBoxes:
			{
				int heights = getNumberHeights();
				debugString.append("', # HeightBoxes ").append(inttostring(heights));

				debugString.append(", Heights: ");
				for ( int i = 0; i < heights; ++i )
					debugString.append(inttostring(heights_[i])).append(", ");
				break;
			}
		}
	}
	Debug::trace(category, debugString);
}


const char* HfcEvolWrapperWrapBox::serialize(int format)const
{
	Debug::trace("HfcEvolWrapperWrapBox", "serialize()");
	HobString result;

	serializedData.erase();
	switch ( format )
	{
		case BrowserFormat:
		{
			for ( int i = 1; i < endHfcEvolWrWrapBoxFieldOffsets; ++i )
			{
				if ( i > 1 )
					serializedData.append(BrowserFormatSeparator);

				switch (i)
				{
					case fieldOffsetEvolWrWrapBoxPrecedence:
						serializedData.append(inttostring(precedence_));
						break;
					case fieldOffsetEvolWrWrapBoxName:
					{
						HobString temp = name_;

						temp.quoteForCSV();
						serializedData.append(temp);

						break;
					}
					case fieldOffsetEvolWrWrapBoxPLUException:
						serializedData.append(inttostring(pluException_));
						break;
					case fieldOffsetEvolWrWrapBoxMinLength:
					{
						Configuration::getInstance()->formatDecimalValue(minLength_, result, 1);
						serializedData.append(result);
						break;
					}
					case fieldOffsetEvolWrWrapBoxMaxLength:
					{
						Configuration::getInstance()->formatDecimalValue(maxLength_, result, 1);
						serializedData.append(result);
						break;
					}
					case fieldOffsetEvolWrWrapBoxMinWidth:
					{
						Configuration::getInstance()->formatDecimalValue(minWidth_, result, 1);
						serializedData.append(result);
						break;
					}
					case fieldOffsetEvolWrWrapBoxMaxWidth:
					{
						Configuration::getInstance()->formatDecimalValue(maxWidth_, result, 1);
						serializedData.append(result);
						break;
					}
					case fieldOffsetEvolWrWrapBoxLipHeight:
					{
						Configuration::getInstance()->formatDecimalValue(lipHeight_, result, 1);
						serializedData.append(result);
						break;
					}
					case fieldOffsetEvolWrWrapBoxHeightBoxes:
					{
						int numHeights = getNumberHeights();
						serializedData.append(inttostring(numHeights));
						HobString listContents;

						for ( int i = 0; i < numHeights; ++i )
						{
							if ( i )
								listContents.append(BrowserFormatSeparator);

							Configuration::getInstance()->formatDecimalValue(heights_[i], result, 1);
							listContents.append(result);
						}

						listContents.quoteForCSV();
						serializedData.append(BrowserFormatSeparator);
						serializedData.append(listContents);

						break;
					}
				}
			}

			break;
		}
		default:
			// todo - throw exception("invalid format");
			break;
	}

	Debug::trace("HfcEvolWrapperWrapBox", "serialize():  " + serializedData);

	return serializedData.c_str();
}

bool HfcEvolWrapperWrapBox::isValid() const
{
	if (name_.empty())
		return false;
	else
		return true;
}
