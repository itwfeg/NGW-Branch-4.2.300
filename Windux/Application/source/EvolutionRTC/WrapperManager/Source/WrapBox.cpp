#include "WrapBox.h"
#include "FpmManager.h"

#include "util/e_string/utilstring.h"
//#include "HobString.h"
#include "Debug.h"



WrapBox::WrapBox()
	: name_(), pluException_(0), lengthLow_(0), lengthHigh_(0), widthLow_(0),
			widthHigh_(0), lip_(0), heightBoxes_()
{
}


WrapBox::WrapBox(const char* name, unsigned int lengthLow, unsigned int lengthHigh,
		unsigned int widthLow, unsigned int widthHigh, unsigned int lip) 
	: name_(name), pluException_(0), lengthLow_((UINT16)lengthLow), lengthHigh_((UINT16)lengthHigh),
			widthLow_((UINT16)widthLow), widthHigh_((UINT16)widthHigh), lip_((UINT16)lip), heightBoxes_()
{
	validateName();
}


WrapBox::WrapBox(const char* name, unsigned int plu, unsigned int lengthLow, unsigned int lengthHigh,
		unsigned int widthLow, unsigned int widthHigh, unsigned int lip) 
	: name_(name), pluException_(plu), lengthLow_((UINT16)lengthLow), lengthHigh_((UINT16)lengthHigh),
			widthLow_((UINT16)widthLow), widthHigh_((UINT16)widthHigh), lip_((UINT16)lip), heightBoxes_()
{
	validateName();
}


WrapBox::HeightBox* WrapBox::getHeightBox(unsigned int height)
{
	HeightBoxContainer_type::iterator iter =
			find_if(heightBoxes_.begin(), heightBoxes_.end(), bind2nd(isSameHeight(), height));

	return iter != heightBoxes_.end() ? &*iter : 0;
}


const WrapBox::HeightBox* WrapBox::getHeightBox(unsigned int height) const
{
	HeightBoxContainer_type::const_iterator iter =
			find_if(heightBoxes_.begin(), heightBoxes_.end(), bind2nd(isSameHeight(), height));

	return iter != heightBoxes_.end() ? &*iter : 0;
}

const WrapBox::HeightBox* WrapBox::getContainingHeightBox(unsigned int height) const
{
	HeightBoxContainer_type::const_iterator iter =
			find_if(heightBoxes_.begin(), heightBoxes_.end(), bind2nd(containsHeight(), height));

	return iter != heightBoxes_.end() ? &*iter : 0;
}


void WrapBox::initHeightBoxesDummy(unsigned int num, int baseNumber)
{
	FpmManager* pSerialFlash = FpmManager::getInstance();
	int paramListLen = WrapBox::_Use1P_LAST_PARAM_;				// 1P by default with 19 params, but we carry 21 
	if (pSerialFlash->isWrapperTwoPrinter())
		paramListLen = WrapBox::_Use2P_LAST_PARAM_;

	for ( unsigned int i = 1; i < num; ++i )
	{
		int base = baseNumber + i * 100;

		HeightBox hb = HeightBox(i * 5);
		for (int j = 0; j < paramListLen; ++j)
			hb.setParameter(j, base + lip_ + j);

		insertHeightBox(hb);
	}

	HeightBox hb = HeightBox(60);
	for ( int j = 0; j < paramListLen; ++j )
		hb.setParameter(j, baseNumber + lip_ + j);

	insertHeightBox(hb);
}


bool WrapBox::insertHeightBox(const HeightBox& newHeightBox)
{
	unsigned int height = newHeightBox.getHeight();
	HeightBoxContainer_type::const_iterator iter =
			find_if(heightBoxes_.begin(), heightBoxes_.end(), bind2nd(isSameHeight(), height));

	if ( iter == heightBoxes_.end() )
	{
		heightBoxes_.insert(
				find_if(heightBoxes_.begin(), heightBoxes_.end(), bind2nd(containsHeight(), height)),
				newHeightBox);

		return true;
	}

	return false;
}


bool WrapBox::renameHeightBox(unsigned int oldHeight, unsigned int newHeight)
{
	HeightBoxContainer_type::iterator iter =
			find_if(heightBoxes_.begin(), heightBoxes_.end(), bind2nd(isSameHeight(), oldHeight));

	if ( iter != heightBoxes_.end() )
	{
		HeightBoxContainer_type::const_iterator iter2 =
				find_if(heightBoxes_.begin(), heightBoxes_.end(), bind2nd(isSameHeight(), newHeight));

		if ( iter2 == heightBoxes_.end() )
		{
			HeightBox newHeightBox(newHeight, *iter);

			heightBoxes_.erase(iter);

			insertHeightBox(newHeightBox);

			return true;
		}
	}

	return false;
}


bool WrapBox::removeHeightBox(unsigned int height)
{
	HeightBoxContainer_type::iterator iter =
			find_if(heightBoxes_.begin(), heightBoxes_.end(), bind2nd(isSameHeight(), height));

	if ( iter != heightBoxes_.end() )
	{
		heightBoxes_.erase(iter);

		return true;
	}

	return false;
}


void WrapBox::show(const std::string& debugCategory) const
{

	Debug::trace("WrapBox", "****************        WrapBox        ****************");
	Debug::trace("WrapBox", "           Name: " + name_);
	Debug::trace("WrapBox", "  PLU Exception: " + util_string::uinttostring(pluException_));
	Debug::trace("WrapBox", "     Length low: " + util_string::uinttostring(lengthLow_));
	Debug::trace("WrapBox", "    Length high: " + util_string::uinttostring(lengthHigh_));
	Debug::trace("WrapBox", "      Width low: " + util_string::uinttostring(widthLow_));
	Debug::trace("WrapBox", "     Width high: " + util_string::uinttostring(widthHigh_));
	Debug::trace("WrapBox", "            Lip: " + util_string::uinttostring(lip_));
	Debug::trace("WrapBox", "*******************************************************");
}


void WrapBox::validateName()
{
	int erase = (int)name_.size() - _MaxNameLength_;
	if ( erase > 0 )
		name_.erase(_MaxNameLength_, (size_t)erase);
}


void WrapBox::HeightBox::show(const std::string& debugCategory) const
{
	Debug::trace("WrapBox", "****************        HeightBox        ****************");
	Debug::trace("WrapBox", "  Height Box: " + util_string::uinttostring(height_));
	Debug::trace("WrapBox", "   Parameter");

	FpmManager* pSerialFlash = FpmManager::getInstance();
	int paramListLen = WrapBox::_Use1P_LAST_PARAM_;				// 1P by default with 19 params, but we carry 21 
	if (pSerialFlash->isWrapperTwoPrinter())
		paramListLen = WrapBox::_Use2P_LAST_PARAM_;

	for ( int i = 0; i < paramListLen; ++i )
	{
		std::string spacer = "     ";
		if (i < 10)
			spacer.append(" ");
		Debug::trace("WrapBox", spacer + util_string::inttostring(i) + ": " + util_string::inttostring(getParameter(i)));
	}
	Debug::trace("WrapBox", "*******************************************************");
}

int WrapBox::getHeightBoxHeight(int n)
{
	int i = 0, height = 0;
	HeightBoxContainer_type::const_iterator iter = heightBoxes_.begin();
	while ( iter != heightBoxes_.end() )
	{
		if ( i++ == n)
		{
			height = (&*iter)->getHeight();
			break;
		}
		++iter;
	}
	return height;
}

const WrapBox::HeightBox* WrapBox::getHeightBoxNumber(int n) const
{
	int i = 0;

	HeightBoxContainer_type::const_iterator iter = heightBoxes_.begin();
	while ( iter != heightBoxes_.end() )
	{
		if ( i++ == n)
		{
			return &*iter;
		}
		++iter;
	}
	return 0;
}

WrapBox::HeightBox* WrapBox::getHeightBoxNumber(int n)
{
	int i = 0;

	HeightBoxContainer_type::iterator iter = heightBoxes_.begin();
	while ( iter != heightBoxes_.end() )
	{
		if ( i++ == n)
		{
			return &*iter;
		}
		++iter;
	}
	return 0;
}

