/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */



#include "HfcProductLabelFormSizeMsg.h"


#include "HobString.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcProductLabelFormSizeMsg::dataType = 0;
std::string HfcProductLabelFormSizeMsg::dataTypeName("HfcProductLabelFormSizeMsg"); 
#endif


HfcProductLabelFormSizeMsg::HfcProductLabelFormSizeMsg(const char *data, int len, 
		int formatType)
		: productNumber_(0), type_(0), width_(0), height_(0), defaultFontName_(""), defaultPointSize_(0)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	switch ( formatType )
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			for ( int field = 1; tok.next(&token) != -1; ++field )
			{
				setReceived(field, (token.length() == 0) ? false : true);
					
				switch ( field )
				{
					case fieldOffsetProductNumber:
					{
						productNumber_ = (HobRecordNum)Long::valueOf(token);
						break;
					}
					case fieldOffsetTextType:
					{
						type_ = Integer::valueOf(token);
						break;
					}
					case fieldOffsetVerticalSize:
					{
						token.unQuoteForCSV();
						height_ = Integer::valueOf(token);
						break;
					}
					case fieldOffsetHorizontalSize:
					{
						token.unQuoteForCSV();
						width_ = Integer::valueOf(token);
						break;
					}
					case fieldOffsetDefaultFontName:
					{
						defaultFontName_ = token;
						break;
					}
					case fieldOffsetDefaultPointSize:
					{
						defaultPointSize_ = Integer::valueOf(token);
						break;
					}
					default:
					{
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
}


HfcProductLabelFormSizeMsg::~HfcProductLabelFormSizeMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	Debug::trace("HfcProductLabelFormSizeMsg.Delete", 
			HobString("    HfcProductLabelFormSizeMsg: ") + 
			HobString(Hex::toString((long long) this)) + HobString("  Product number:") +
			HobString(Long::toString(productNumber_)));
}


const char *HfcProductLabelFormSizeMsg::serialize(int format) const
{
	serializedData = "";
	HobString stringForQuoting;

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case BrowserFormat:
		{
			for ( int field = 1; field < endProductLabelFormSizeFieldOffsets; ++field )
			{
				if ( field > 1 )
					serializedData.append(BrowserFormatSeparator);

				switch ( field )
				{
					case fieldOffsetProductNumber:
					{
						serializedData.append(Long::toString((long)productNumber_));

						break;
					}
					case fieldOffsetTextType:
					{
						serializedData.append(Integer::toString(type_));

						break;
					}
					case fieldOffsetVerticalSize:
					{
						stringForQuoting = Integer::toString(height_);
						stringForQuoting.quoteForCSV();
						serializedData.append(stringForQuoting);

						break;
					}
					case fieldOffsetHorizontalSize:
					{
						stringForQuoting = Integer::toString(width_);
						stringForQuoting.quoteForCSV();
						serializedData.append(stringForQuoting);

						break;
					}
					case fieldOffsetDefaultFontName:
					{
						serializedData.append(defaultFontName_);

						break;
					}
					case fieldOffsetDefaultPointSize:
					{
						serializedData.append(Integer::toString(defaultPointSize_));

						break;
					}
					default:
					{
						break;
					}
				}
			}

			break;
		}
		default:
		{
			serializedData = HobString("HfcProductLabelFormSizeMsg::serialize - unknown format:") + 
					HobString(Integer::toString(format));
			// todo - throw exception("invalid format");

			break;
		}
	}

	return serializedData.data();
}

