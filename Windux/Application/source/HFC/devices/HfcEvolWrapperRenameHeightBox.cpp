#include "HfcEvolWrapperRenameHeightBox.h"

#include "HobTokenizerCSV.h"

#include "util/e_string/utilstring.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperRenameHeightBox::_dataType_ = 0;
std::string HfcEvolWrapperRenameHeightBox::_dataTypeName_("HfcEvolWrapperRenameHeightBox"); 
#endif


HfcEvolWrapperRenameHeightBox::HfcEvolWrapperRenameHeightBox(const char* buffer, int len, int formatType)
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
					case fieldOffsetEvolWrRenameHeightBoxOldHeight:
						setOldHeight(util_string::stringtoint(token));
						break;
					case fieldOffsetEvolWrRenameHeightBoxNewHeight:
						setNewHeight(util_string::stringtoint(token));
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


HfcEvolWrapperRenameHeightBox::~HfcEvolWrapperRenameHeightBox()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


void HfcEvolWrapperRenameHeightBox::init()
{
	precedence_ = -1;
	oldHeight_ = 0;
	newHeight_ = 0;
}


void HfcEvolWrapperRenameHeightBox::show() const
{
	bool isTracing = Debug::isTracing("HfcEvolWrapperRenameHeightBox");
	std::string debugString;

	if ( !isTracing )
		Debug::addTraceCategory("HfcEvolWrapperRenameHeightBox");

	for ( int i = 1; i < endHfcEvolWrRenameHeightBoxFieldOffsets; ++i )
	{
		switch (i)
		{
			case fieldOffsetEvolWrRenameHeightBoxPrecedence:
			{
				debugString.append("Precedence ").append(util_string::inttostring(precedence_));
				break;
			}
			case fieldOffsetEvolWrRenameHeightBoxOldHeight:
			{
				debugString.append(", OldHeight '").append(util_string::inttostring(oldHeight_));
				break;
			}
			case fieldOffsetEvolWrRenameHeightBoxNewHeight:
			{
				debugString.append(", NewHeight '").append(util_string::inttostring(newHeight_));
				break;
			}
		}
	}

	Debug::trace("HfcEvolWrapperRenameHeightBox", debugString);


	if ( !isTracing )
		Debug::removeTraceCategory("HfcEvolWrapperRenameHeightBox");
}
