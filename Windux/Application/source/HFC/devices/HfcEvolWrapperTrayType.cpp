#include "HfcEvolWrapperTrayType.h"

#include "HobTokenizerCSV.h"

#include "util/e_string/utilstring.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperTrayType::_dataType_ = 0;
std::string HfcEvolWrapperTrayType::_dataTypeName_("HfcEvolWrapperTrayType"); 
#endif


HfcEvolWrapperTrayType::HfcEvolWrapperTrayType(const char* buffer, int len, int formatType)
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
					case fieldOffsetEvolWrTrayTypePrecedence:
						setPrecedence(util_string::stringtoint(token));
						break;
					case fieldOffsetEvolWrTrayTypeName:
						token.unQuoteForCSV();
						setName(token);
						break;
					case fieldOffsetEvolWrTrayTypePLUException:
						setPLUException(util_string::stringtoint(token));
						break;
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


HfcEvolWrapperTrayType::~HfcEvolWrapperTrayType()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


void HfcEvolWrapperTrayType::init()
{
	precedence_ = -1;
	name_.erase();
	pluException_ = 0;
}


void HfcEvolWrapperTrayType::show() const
{
	bool isTracing = Debug::isTracing("HfcEvolWrapperTrayType");
	std::string debugString;

	if ( !isTracing )
		Debug::addTraceCategory("HfcEvolWrapperTrayType");

	
	for ( int i = 1; i < endHfcEvolWrTrayTypeFieldOffsets; ++i )
	{
		switch (i)
		{
			case fieldOffsetEvolWrTrayTypePrecedence:
			{
				debugString.append("Precedence ").append(util_string::inttostring(precedence_));
				break;
			}
			case fieldOffsetEvolWrTrayTypeName:
			{
				debugString.append(", Name '").append(name_);
				break;
			}
			case fieldOffsetEvolWrTrayTypePLUException:
			{
				debugString.append("', PLUException ").append(util_string::inttostring(pluException_));
				break;
			}
		}
	}

	Debug::trace("HfcEvolWrapperTrayType", debugString);


	if ( !isTracing )
		Debug::removeTraceCategory("HfcEvolWrapperTrayType");
}
