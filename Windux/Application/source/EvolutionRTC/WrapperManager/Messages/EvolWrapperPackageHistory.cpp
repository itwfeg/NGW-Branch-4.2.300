#include "EvolWrapperPackageHistory.h"

#include "HobString.h"
#include "util/e_string/utilstring.h"
#include "Debug.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperPackageHistory::_dataType_ = 0;
std::string EvolWrapperPackageHistory::_dataTypeName_("EvolWrapperPackageHistory"); 
#endif

EvolWrapperPackageHistory::EvolWrapperPackageHistory(Hobart::CEvolMsgReceiver *pHobThread)
		: EvolMessage(Wrapper, WrEvolPackageHistory, pHobThread),
		totalElements_(0), firstElement_(0), history_(_maxElements_)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


EvolWrapperPackageHistory::EvolWrapperPackageHistory(const char *buffer)
		: EvolMessage(buffer), totalElements_(0), firstElement_(0), history_(_maxElements_)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	EvolMessage::serialize();
	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &totalElements_;
	for ( unsigned int i = 0; i < sizeof(totalElements_); ++i )
	{
		*pChar++ = buffer[j++];
	}
	if ( totalElements_ > _maxElements_ )
		totalElements_ = _maxElements_;
	Debug::trace("WrapperManager.EvolPackageHistory", 20, "Unserializing totalElements_ = " + util_string::inttostring(totalElements_));

	pChar = (char *) &firstElement_;
	for ( unsigned int i = 0; i < sizeof(firstElement_); ++i )
	{
		*pChar++ = buffer[j++];
	}
	if ( firstElement_ >= totalElements_ )
		firstElement_ = 0;
	Debug::trace("WrapperManager.EvolPackageHistory", 20, "Unserializing firstElement_ = " + util_string::inttostring(firstElement_));

	HistoryItem item;
	for ( int i = 0; i < totalElements_; ++i )
	{
		pChar = (char *) &item.packId_;
		for ( unsigned int k = 0; k < sizeof(item.packId_); ++k )
		{
			*pChar++ = buffer[j++];
		}
		Debug::trace("WrapperManager.EvolPackageHistory", 20, "Unserializing item.packId_ = " + util_string::inttostring(item.packId_));

		pChar = (char *) &item.width_;
		for ( unsigned int k = 0; k < sizeof(item.width_); ++k )
		{
			*pChar++ = buffer[j++];
		}
		Debug::trace("WrapperManager.EvolPackageHistory", 20, "Unserializing item.width_ = " + util_string::inttostring(item.width_));

		pChar = (char *) &item.length_;
		for ( unsigned int k = 0; k < sizeof(item.length_); ++k )
		{
			*pChar++ = buffer[j++];
		}
		Debug::trace("WrapperManager.EvolPackageHistory", 20, "Unserializing item.length_ = " + util_string::inttostring(item.length_));

		pChar = (char *) &item.height_;
		for ( unsigned int k = 0; k < sizeof(item.height_); ++k )
		{
			*pChar++ = buffer[j++];
		}
		Debug::trace("WrapperManager.EvolPackageHistory", 20, "Unserializing item.height_ = " + util_string::inttostring(item.height_));

		pChar = (char *) &item.lip_;
		for ( unsigned int k = 0; k < sizeof(item.lip_); ++k )
		{
			*pChar++ = buffer[j++];
		}
		Debug::trace("WrapperManager.EvolPackageHistory", 20, "Unserializing item.lip_ = " + util_string::inttostring(item.lip_));

		pChar = (char *) &item.filmSelection_;
		for ( unsigned int k = 0; k < sizeof(item.filmSelection_); ++k )
		{
			*pChar++ = buffer[j++];
		}
		Debug::trace("WrapperManager.EvolPackageHistory", 20, "Unserializing item.filmSelection_ = " + util_string::inttostring(item.filmSelection_));

		pChar = (char *) &item.usedWrapBoxBestFit_;
		for ( unsigned int k = 0; k < sizeof(item.usedWrapBoxBestFit_); ++k )
		{
			*pChar++ = buffer[j++];
		}
		Debug::trace("WrapperManager.EvolPackageHistory", 20, "Unserializing item.usedWrapBoxBestFit_ = " + util_string::inttostring(item.usedWrapBoxBestFit_));

		pChar = (char *) &item.modifiedNetStretch_;
		for ( unsigned int k = 0; k < sizeof(item.modifiedNetStretch_); ++k )
		{
			*pChar++ = buffer[j++];
		}
		Debug::trace("WrapperManager.EvolPackageHistory", 20, "Unserializing item.modifiedNetStretch_ = " + util_string::inttostring(item.modifiedNetStretch_));

		char name[_MaxNameLength_ + 1];
		pChar = name;
		for (unsigned int k = 0; k < _MaxNameLength_ + 1; ++k)
		{
			*pChar++ = buffer[j++];
		}
		item.trayName_ = name;
		Debug::trace("WrapperManager.EvolPackageHistory", 20, "Unserializing item.trayName_ = [" + item.trayName_ + "]");

		history_[i] = item;
	}
}

EvolWrapperPackageHistory::~EvolWrapperPackageHistory()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


int EvolWrapperPackageHistory::getNextIndex(int index)
{
	// If the current index is within range ...
	if ( 0 <= index && index < totalElements_ )
	{
		// If we are overrunning the array, return end
		if ( (firstElement_ && index == firstElement_ - 1) ||
			(!firstElement_ && index == totalElements_ - 1) )
			index = -1;
		else
		{
			// ... increment the index and wrap around if necessary
			if ( ++index == _maxElements_ )
				index = 0;
		}

		return index;
	}

	return -1;
}


int EvolWrapperPackageHistory::getPreviousIndex(int index)
{
	// If the current index is within range ...
	if ( 0 <= index && index < totalElements_ )
	{
		// If we are overrunning the array, return end
		if ( index == firstElement_ )
			index = -1;
		else
		{
			// ... decrement the index and wrap around if necessary
			if ( --index == 0 )
				index = totalElements_ - 1;
		}

		return index;
	}
	else if ( index == -1 && totalElements_ )
	{
		// or if index is end go to last element
		return firstElement_ ? firstElement_ - 1 : totalElements_ - 1;
	}

	return -1;
}


EvolWrapperPackageHistory::iterator EvolWrapperPackageHistory::begin()
{
	if ( totalElements_ )
		return iterator(this, firstElement_);

	return end();
}


EvolWrapperPackageHistory::const_iterator EvolWrapperPackageHistory::begin() const
{
	if ( totalElements_ )
		return const_iterator(const_cast<EvolWrapperPackageHistory*>(this), firstElement_);

	return end();
}


void EvolWrapperPackageHistory::addPackage(UINT16 packId, UINT16 width, UINT16 length, UINT16 height, UINT16 lip,
										   WrFilmWidthSelection filmSelection, std::string &trayName, bool bestFit, bool modifiedStretch)
{
	Debug::trace("WrapperManager.EvolPackageHistory", 20, "Currently " + util_string::inttostring(totalElements_) +
			" packages in history list. First package is " + util_string::inttostring(firstElement_));

	if ( totalElements_ < _maxElements_ )
	{
		history_[totalElements_].packId_				= packId;
		history_[totalElements_].width_					= width;
		history_[totalElements_].length_				= length;
		history_[totalElements_].height_				= height;
		history_[totalElements_].lip_					= lip;
		history_[totalElements_].filmSelection_			= filmSelection;
		history_[totalElements_].usedWrapBoxBestFit_	= bestFit;
		history_[totalElements_].modifiedNetStretch_	= modifiedStretch;
		history_[totalElements_].trayName_ 				= trayName;

		++totalElements_;
	}
	else
	{
		history_[firstElement_].packId_					= packId;
		history_[firstElement_].width_					= width;
		history_[firstElement_].length_					= length;
		history_[firstElement_].height_					= height;
		history_[firstElement_].lip_					= lip;
		history_[firstElement_].filmSelection_			= filmSelection;
		history_[firstElement_].usedWrapBoxBestFit_		= bestFit;
		history_[firstElement_].modifiedNetStretch_		= modifiedStretch;
		history_[firstElement_].trayName_ 				= trayName;

		++firstElement_;
	}

	if ( firstElement_ == _maxElements_ )
		firstElement_ = 0;

	Debug::trace("WrapperManager.EvolPackageHistory", 20, "After adding package, " + util_string::inttostring(totalElements_) +
			" packages in history list. First package is " + util_string::inttostring(firstElement_));
}


void EvolWrapperPackageHistory::addPackage(const HistoryItem& item)
{
	Debug::trace("WrapperManager.EvolPackageHistory", 20, "Currently " + util_string::inttostring(totalElements_) +
			" packages in history list. First package is " + util_string::inttostring(firstElement_));

	if ( totalElements_ < _maxElements_ )
		history_[totalElements_++] = item;
	else
		history_[firstElement_++] = item;

	if ( firstElement_ == _maxElements_ )
		firstElement_ = 0;

	Debug::trace("WrapperManager.EvolPackageHistory", 20, "After adding package, " + util_string::inttostring(totalElements_) +
			" packages in history list. First package is " + util_string::inttostring(firstElement_));
}


const char* EvolWrapperPackageHistory::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	const char* pChar = (const char*)&totalElements_;
	for ( unsigned int i = 0; i < sizeof(totalElements_); ++i )
	{
		serializedData.append(1, pChar[i]);
	}
	Debug::trace("WrapperManager.EvolPackageHistory", 20, "Serializing totalElements_ = " + util_string::inttostring(totalElements_));

	pChar = (const char *) &firstElement_;
	for (unsigned int i = 0; i < sizeof(firstElement_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	Debug::trace("WrapperManager.EvolPackageHistory", 20, "Serializing firstElement_ = " + util_string::inttostring(firstElement_));

	for ( int i = 0; i < totalElements_; ++i )
	{
		const HistoryItem& item = history_[i];

		pChar = (const char *) &item.packId_;
		for ( unsigned int k = 0; k < sizeof(item.packId_); ++k )
		{
			serializedData.append(1, pChar[k]);
		}
		Debug::trace("WrapperManager.EvolPackageHistory", 20, "Serializing item.packId_ = " + util_string::inttostring(item.packId_));

		pChar = (const char *) &item.width_;
		for ( unsigned int k = 0; k < sizeof(item.width_); ++k )
		{
			serializedData.append(1, pChar[k]);
		}
		Debug::trace("WrapperManager.EvolPackageHistory", 20, "Serializing item.width_ = " + util_string::inttostring(item.width_));

		pChar = (const char *) &item.length_;
		for ( unsigned int k = 0; k < sizeof(item.length_); ++k )
		{
			serializedData.append(1, pChar[k]);
		}
		Debug::trace("WrapperManager.EvolPackageHistory", 20, "Serializing item.length_ = " + util_string::inttostring(item.length_));

		pChar = (const char *) &item.height_;
		for ( unsigned int k = 0; k < sizeof(item.height_); ++k )
		{
			serializedData.append(1, pChar[k]);
		}
		Debug::trace("WrapperManager.EvolPackageHistory", 20, "Serializing item.height_ = " + util_string::inttostring(item.height_));

		pChar = (const char *) &item.lip_;
		for ( unsigned int k = 0; k < sizeof(item.lip_); ++k )
		{
			serializedData.append(1, pChar[k]);
		}
		Debug::trace("WrapperManager.EvolPackageHistory", 20, "Serializing item.lip_ = " + util_string::inttostring(item.lip_));

		pChar = (const char *) &item.filmSelection_;
		for ( unsigned int k = 0; k < sizeof(item.filmSelection_); ++k )
		{
			serializedData.append(1, pChar[k]);
		}
		Debug::trace("WrapperManager.EvolPackageHistory", 20, "Serializing item.filmSelection_ = " + util_string::inttostring(item.filmSelection_));

		pChar = (const char *) &item.usedWrapBoxBestFit_;
		for ( unsigned int k = 0; k < sizeof(item.usedWrapBoxBestFit_); ++k )
		{
			serializedData.append(1, pChar[k]);
		}
		Debug::trace("WrapperManager.EvolPackageHistory", 20, "Serializing item.usedWrapBoxBestFit_ = " + util_string::inttostring(item.usedWrapBoxBestFit_));

		pChar = (const char *) &item.modifiedNetStretch_;
		for ( unsigned int k = 0; k < sizeof(item.modifiedNetStretch_); ++k )
		{
			serializedData.append(1, pChar[k]);
		}
		Debug::trace("WrapperManager.EvolPackageHistory", 20, "Serializing item.modifiedNetStretch_ = " + util_string::inttostring(item.modifiedNetStretch_));

		char name[_MaxNameLength_ + 1];
		strcpy(name, item.trayName_.c_str());
		pChar = name;
		for ( unsigned int k = 0; k < _MaxNameLength_ + 1; ++k )
		{
			serializedData.append(1, pChar[k]);
		}
		Debug::trace("WrapperManager.EvolPackageHistory", 20, "Serializing item.trayName_ = " + item.trayName_);
	}

	return serializedData.data();
}


void EvolWrapperPackageHistory::show()
{
	if (Debug::isTracing("EvolWrapperPackageHistory"))
	{
		Debug::trace("EvolWrapperPackageHistory", "EvolWrapperPackageHistory: ");
		EvolMessage::show();

		Debug::trace("EvolWrapperPackageHistory", 20, "Total Elements: " + util_string::inttostring(totalElements_));
		Debug::trace("EvolWrapperPackageHistory", 20, "First Element: " + util_string::inttostring(firstElement_));

		for ( int i = 0; i < totalElements_; ++i )
		{
			Debug::trace("EvolWrapperPackageHistory", 20, "Package[" + util_string::inttostring(i) + "].packId = " +
					util_string::inttostring(history_[i].packId_));
			Debug::trace("EvolWrapperPackageHistory", 20, "Package[" + util_string::inttostring(i) + "].width    = " +
					util_string::inttostring(history_[i].width_));
			Debug::trace("EvolWrapperPackageHistory", 20, "Package[" + util_string::inttostring(i) + "].length   = " +
					util_string::inttostring(history_[i].length_));
			Debug::trace("EvolWrapperPackageHistory", 20, "Package[" + util_string::inttostring(i) + "].height = " +
					util_string::inttostring(history_[i].height_));
			Debug::trace("EvolWrapperPackageHistory", 20, "Package[" + util_string::inttostring(i) + "].lip    = " +
					util_string::inttostring(history_[i].lip_));
			Debug::trace("EvolWrapperPackageHistory", 20, "Package[" + util_string::inttostring(i) + "].filmSelection   = " +
					util_string::inttostring(history_[i].filmSelection_));
			Debug::trace("EvolWrapperPackageHistory", 20, "Package[" + util_string::inttostring(i) + "].trayName_   = " +
					history_[i].trayName_);
		}
	}
}


EvolWrapperPackageHistory::HistoryIterator& EvolWrapperPackageHistory::HistoryIterator::operator=(
		const EvolWrapperPackageHistory::HistoryIterator& rhs)
{
	// Check for self-assignment
	if ( *this != rhs )
	{
		// Assign values to this iterator
		container_ = rhs.container_;
		index_ = rhs.index_;
	}

	return *this;
}


bool EvolWrapperPackageHistory::HistoryIterator::operator==(const HistoryIterator& rhs) const
{
	if ( rhs.container_ != container_ )
		return false;

	return (index_ == rhs.index_);
}


EvolWrapperPackageHistory::HistoryIterator& EvolWrapperPackageHistory::HistoryIterator::operator++()
{
	index_ = container_->getNextIndex(index_);

	return *this;
}


const EvolWrapperPackageHistory::HistoryIterator& EvolWrapperPackageHistory::HistoryIterator::operator++() const
{
	index_ = container_->getNextIndex(index_);

	return *this;
}


EvolWrapperPackageHistory::HistoryIterator EvolWrapperPackageHistory::HistoryIterator::operator++(int)
{
	iterator tmp = *this;

	++*this;

	return tmp;
}


const EvolWrapperPackageHistory::HistoryIterator EvolWrapperPackageHistory::HistoryIterator::operator++(int) const
{
	iterator tmp = *this;

	++*this;

	return tmp;
}


EvolWrapperPackageHistory::HistoryIterator& EvolWrapperPackageHistory::HistoryIterator::operator--()
{
	index_ = container_->getPreviousIndex(index_);

	return *this;
}


const EvolWrapperPackageHistory::HistoryIterator& EvolWrapperPackageHistory::HistoryIterator::operator--() const
{
	index_ = container_->getPreviousIndex(index_);

	return *this;
}


EvolWrapperPackageHistory::HistoryIterator EvolWrapperPackageHistory::HistoryIterator::operator--(int)
{
	HistoryIterator tmp = *this;

	--*this;

	return tmp;
}


const EvolWrapperPackageHistory::HistoryIterator EvolWrapperPackageHistory::HistoryIterator::operator--(int) const
{
	HistoryIterator tmp = *this;

	--*this;

	return tmp;
}


void EvolWrapperPackageHistory::HistoryIterator::show() const
{
	if (Debug::isTracing("EvolWrapperPackageHistory"))
	{
		const HistoryItem& item = container_->getElement(index_);

		Debug::trace("EvolWrapperPackageHistory", 20, "Element at position: " + util_string::inttostring(index_));
		Debug::trace("EvolWrapperPackageHistory", 20, "        PackCode Number: " + util_string::inttostring(item.packId_));
		Debug::trace("EvolWrapperPackageHistory", 20, "          Package Width: " + util_string::uinttostring(item.width_));
		Debug::trace("EvolWrapperPackageHistory", 20, "         Package Length: " + util_string::uinttostring(item.length_));
		Debug::trace("EvolWrapperPackageHistory", 20, "         Package Height: " + util_string::uinttostring(item.height_));
		Debug::trace("EvolWrapperPackageHistory", 20, "            Package Lip: " + util_string::uinttostring(item.lip_));
		Debug::trace("EvolWrapperPackageHistory", 20, "  Package FilmSelection: " + util_string::uinttostring(item.filmSelection_));
		Debug::trace("EvolWrapperPackageHistory", 20, "              Tray name: " + item.trayName_);
	}
}
