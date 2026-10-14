/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcEvolWrapperTrayNames.h"
#include "EvolWrapperPackageTimings.h"

#include "util/e_string/utilstring.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperTrayNames::_dataType_ = 0;
std::string HfcEvolWrapperTrayNames::_dataTypeName_("HfcEvolWrapperTrayNames"); 
#endif

HfcEvolWrapperTrayNames::HfcEvolWrapperTrayNames(EvolWrapperPackageTimings *timings)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	if (timings)
	{
		int wrapBoxes = timings->getNumberWrapBoxes();
		trayNames_.reserve(wrapBoxes);
		for ( int i = 0; i < wrapBoxes; ++i )
		{
			const WrapBox* wrapBox = timings->getWrapBox(i);

			if ( wrapBox )
			{
				trayNames_.push_back(wrapBox->getName());
			}
			else
			{
				trayNames_.push_back("");
			}
		}
	}
}

HfcEvolWrapperTrayNames::~HfcEvolWrapperTrayNames()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

void HfcEvolWrapperTrayNames::getTrayName(unsigned int index, std::string& result)
{
	if (index < trayNames_.size())
		result = trayNames_[index];
	else
		result = "";
}


void HfcEvolWrapperTrayNames::show(const std::string& title) const
{
	if (Debug::isTracing("HfcEvolWrapperTrayNames.show"))
	{
		Debug::trace("HfcEvolWrapperTrayNames.show", "START:  " + title);
		for (unsigned int i = 0; i < trayNames_.size(); i++)
		{
			/*removed -- to do ats
			Debug::trace("HfcEvolWrapperTrayNames.show", inttostring(i) + " : " + trayNames_[i]);
			*/
		}
		Debug::trace("HfcEvolWrapperTrayNames.show", " END:  " + title);
	}
}
