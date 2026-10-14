#include "HfcEvolWrapperChangeTrayTypePrecedence.h"



#include "HobTokenizerCSV.h"

#include "util/e_string/utilstring.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperChangeTrayTypePrecedence::_dataType_ = 0;
std::string HfcEvolWrapperChangeTrayTypePrecedence::_dataTypeName_("HfcEvolWrapperChangeTrayTypePrecedence"); 
#endif


HfcEvolWrapperChangeTrayTypePrecedence::HfcEvolWrapperChangeTrayTypePrecedence(const char* buffer, int len, int formatType)
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
					case fieldOffsetEvolWrRenameHeightBoxPrecedence:
						setPrecedence(util_string::stringtoint(token));
						break;
					case fieldOffsetEvolWrRenameHeightBoxDelta:
						setDelta(util_string::stringtoint(token));
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


HfcEvolWrapperChangeTrayTypePrecedence::~HfcEvolWrapperChangeTrayTypePrecedence()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


void HfcEvolWrapperChangeTrayTypePrecedence::init()
{
	precedence_ = -1;
	delta_ = 0;
}


void HfcEvolWrapperChangeTrayTypePrecedence::show() const
{
	bool isTracing = Debug::isTracing("HfcEvolWrapperChangeTrayTypePrecedence");
	std::string debugString;

	if ( !isTracing )
		Debug::addTraceCategory("HfcEvolWrapperChangeTrayTypePrecedence");

	for ( int i = 1; i < endHfcEvolWrRenameHeightBoxFieldOffsets; ++i )
	{
		switch ( i )
		{
			case fieldOffsetEvolWrRenameHeightBoxPrecedence:
			{
				debugString.append("Precedence ").append(util_string::inttostring(precedence_));
				break;
			}
			case fieldOffsetEvolWrRenameHeightBoxDelta:
			{
				debugString.append(", Delta ").append(util_string::inttostring(delta_));
				break;
			}
		}
	}

	Debug::trace("HfcEvolWrapperChangeTrayTypePrecedence", debugString);


	if ( !isTracing )
		Debug::removeTraceCategory("HfcEvolWrapperChangeTrayTypePrecedence");
}
