#include "WrapperPackageHistory.h"

#include "utilstring.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperPackageHistory::dataType = 0;
std::string WrapperPackageHistory::dataTypeName("WrapperPackageHistory"); 
#endif

WrapperPackageHistory::WrapperPackageHistory(Hobart::CEvolMsgReceiver *pHobThread)
		: EvolMessage(Wrapper, WrPackageHistory, pHobThread),
		totalElements_(0), firstElement_(0), history_(_maxElements_)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}


WrapperPackageHistory::WrapperPackageHistory(const char *buffer)
		: EvolMessage(buffer), totalElements_(0), firstElement_(0), history_(_maxElements_)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
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
	Debug::trace("WrapperManager.PackageHistory", 20, "Unserializing totalElements_ = " + util_string::inttostring(totalElements_));

	pChar = (char *) &firstElement_;
	for ( unsigned int i = 0; i < sizeof(firstElement_); ++i )
	{
		*pChar++ = buffer[j++];
	}
	if ( firstElement_ >= totalElements_ )
		firstElement_ = 0;
	Debug::trace("WrapperManager.PackageHistory", 20, "Unserializing firstElement_ = " + util_string::inttostring(firstElement_));

	HistoryItem item;
	for ( int i = 0; i < totalElements_; ++i )
	{
		pChar = (char *) &item.packCode_;
		for ( unsigned int k = 0; k < sizeof(item.packCode_); ++k )
		{
			*pChar++ = buffer[j++];
		}
		Debug::trace("WrapperManager.PackageHistory", 20, "Unserializing item.packCode_ = " + util_string::inttostring(item.packCode_));

		pChar = (char *) &item.width_;
		for ( unsigned int k = 0; k < sizeof(item.width_); ++k )
		{
			*pChar++ = buffer[j++];
		}
		Debug::trace("WrapperManager.PackageHistory", 20, "Unserializing item.width_ = " + util_string::inttostring(item.width_));

		pChar = (char *) &item.length_;
		for ( unsigned int k = 0; k < sizeof(item.length_); ++k )
		{
			*pChar++ = buffer[j++];
		}
		Debug::trace("WrapperManager.PackageHistory", 20, "Unserializing item.length_ = " + util_string::inttostring(item.length_));

		history_[i] = item;
	}
}

WrapperPackageHistory::~WrapperPackageHistory()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

int WrapperPackageHistory::getNextIndex(int index)
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


int WrapperPackageHistory::getPreviousIndex(int index)
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


WrapperPackageHistory::iterator WrapperPackageHistory::begin()
{
	if ( totalElements_ )
		return iterator(this, firstElement_);

	return end();
}


WrapperPackageHistory::const_iterator WrapperPackageHistory::begin() const
{
	if ( totalElements_ )
		return const_iterator(const_cast<WrapperPackageHistory*>(this), firstElement_);

	return end();
}


void WrapperPackageHistory::addPackage(WrPackCode packCode, UINT16 width, UINT16 length)
{
	Debug::trace("WrapperManager.PackageHistory", 20, "Currently " + util_string::inttostring(totalElements_) +
			" packages in history list. First package is " + util_string::inttostring(firstElement_));

	if ( totalElements_ < _maxElements_ )
	{
		history_[totalElements_].packCode_  = packCode;
		history_[totalElements_].width_     = width;
		history_[totalElements_].length_    = length;

		++totalElements_;
	}
	else
	{
		history_[firstElement_].packCode_   = packCode;
		history_[firstElement_].width_      = width;
		history_[firstElement_].length_     = length;

		++firstElement_;
	}

	if ( firstElement_ == _maxElements_ )
		firstElement_ = 0;

	Debug::trace("WrapperManager.PackageHistory", 20, "After adding package, " + util_string::inttostring(totalElements_) +
			" packages in history list. First package is " + util_string::inttostring(firstElement_));
}


void WrapperPackageHistory::addPackage(const HistoryItem& item)
{
	Debug::trace("WrapperManager.PackageHistory", 20, "Currently " + util_string::inttostring(totalElements_) +
			" packages in history list. First package is " + util_string::inttostring(firstElement_));

	if ( totalElements_ < _maxElements_ )
		history_[totalElements_++] = item;
	else
		history_[firstElement_++] = item;

	if ( firstElement_ == _maxElements_ )
		firstElement_ = 0;

	Debug::trace("WrapperManager.PackageHistory", 20, "After adding package, " + util_string::inttostring(totalElements_) +
			" packages in history list. First package is " + util_string::inttostring(firstElement_));
}


const char* WrapperPackageHistory::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	const char* pChar = (const char*)&totalElements_;
	for ( unsigned int i = 0; i < sizeof(totalElements_); ++i )
	{
		serializedData.append(1, pChar[i]);
	}
	Debug::trace("WrapperManager.PackageHistory", 20, "Serializing totalElements_ = " + util_string::inttostring(totalElements_));

	pChar = (const char *) &firstElement_;
	for (unsigned int i = 0; i < sizeof(firstElement_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	Debug::trace("WrapperManager.PackageHistory", 20, "Serializing firstElement_ = " + util_string::inttostring(firstElement_));

	for ( int i = 0; i < totalElements_; ++i )
	{
		const HistoryItem& item = history_[i];

		pChar = (const char *) &item.packCode_;
		for ( unsigned int k = 0; k < sizeof(item.packCode_); ++k )
		{
			serializedData.append(1, pChar[k]);
		}
		Debug::trace("WrapperManager.PackageHistory", 20, "Serializing item.packCode_ = " + util_string::inttostring(item.packCode_));

		pChar = (const char *) &item.width_;
		for ( unsigned int k = 0; k < sizeof(item.width_); ++k )
		{
			serializedData.append(1, pChar[k]);
		}
		Debug::trace("WrapperManager.PackageHistory", 20, "Serializing item.width_ = " + util_string::inttostring(item.width_));

		pChar = (const char *) &item.length_;
		for ( unsigned int k = 0; k < sizeof(item.length_); ++k )
		{
			serializedData.append(1, pChar[k]);
		}
		Debug::trace("WrapperManager.PackageHistory", 20, "Serializing item.length_ = " + util_string::inttostring(item.length_));
	}

	return serializedData.data();
}


void WrapperPackageHistory::show()
{
	Debug::addTraceCategory("WrapperPackageHistory");


	Debug::trace("WrapperPackageHistory", "WrapperPackageHistory: ");
	EvolMessage::show();

	Debug::trace("WrapperPackageHistory", 20, "Total Elements: " + util_string::inttostring(totalElements_));
	Debug::trace("WrapperPackageHistory", 20, "First Element: " + util_string::inttostring(firstElement_));

	for ( int i = 0; i < totalElements_; ++i )
	{
		Debug::trace("WrapperPackageHistory", 20, "Package[" + util_string::inttostring(i) + "].packCode = " +
				util_string::inttostring(history_[i].packCode_));
		Debug::trace("WrapperPackageHistory", 20, "Package[" + util_string::inttostring(i) + "].width    = " +
				util_string::inttostring(history_[i].width_));
		Debug::trace("WrapperPackageHistory", 20, "Package[" + util_string::inttostring(i) + "].length   = " +
				util_string::inttostring(history_[i].length_));
	}


	Debug::removeTraceCategory("WrapperPackageHistory");
}

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperPackageHistory::HistoryIterator::dataType = 0;
std::string WrapperPackageHistory::HistoryIterator::dataTypeName("WrapperPackageHistory::HistoryIterator"); 
#endif

WrapperPackageHistory::HistoryIterator::HistoryIterator(const HistoryIterator& rhs)
			: container_(rhs.container_), index_(rhs.index_)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperPackageHistory::HistoryIterator::~HistoryIterator()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}



WrapperPackageHistory::HistoryIterator& WrapperPackageHistory::HistoryIterator::operator=(
		const WrapperPackageHistory::HistoryIterator& rhs)
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


bool WrapperPackageHistory::HistoryIterator::operator==(const HistoryIterator& rhs) const
{
	if ( rhs.container_ != container_ )
		return false;

	return (index_ == rhs.index_);
}


WrapperPackageHistory::HistoryIterator& WrapperPackageHistory::HistoryIterator::operator++()
{
	index_ = container_->getNextIndex(index_);

	return *this;
}


const WrapperPackageHistory::HistoryIterator& WrapperPackageHistory::HistoryIterator::operator++() const
{
	index_ = container_->getNextIndex(index_);

	return *this;
}


WrapperPackageHistory::HistoryIterator WrapperPackageHistory::HistoryIterator::operator++(int)
{
	iterator tmp = *this;

	++*this;

	return tmp;
}


const WrapperPackageHistory::HistoryIterator WrapperPackageHistory::HistoryIterator::operator++(int) const
{
	iterator tmp = *this;

	++*this;

	return tmp;
}


WrapperPackageHistory::HistoryIterator& WrapperPackageHistory::HistoryIterator::operator--()
{
	index_ = container_->getPreviousIndex(index_);

	return *this;
}


const WrapperPackageHistory::HistoryIterator& WrapperPackageHistory::HistoryIterator::operator--() const
{
	index_ = container_->getPreviousIndex(index_);

	return *this;
}


WrapperPackageHistory::HistoryIterator WrapperPackageHistory::HistoryIterator::operator--(int)
{
	HistoryIterator tmp = *this;

	--*this;

	return tmp;
}


const WrapperPackageHistory::HistoryIterator WrapperPackageHistory::HistoryIterator::operator--(int) const
{
	HistoryIterator tmp = *this;

	--*this;

	return tmp;
}


void WrapperPackageHistory::HistoryIterator::show() const
{
	const HistoryItem& item = container_->getElement(index_);

	Debug::trace("WrapperPackageHistory", 20, "Element at position: " + util_string::inttostring(index_));
	Debug::trace("WrapperPackageHistory", 20, "   PackCode Number: " + util_string::inttostring(item.packCode_));
	Debug::trace("WrapperPackageHistory", 20, "   Package Width: " + util_string::uinttostring(item.width_));
	Debug::trace("WrapperPackageHistory", 20, "   Package Length: " + util_string::uinttostring(item.length_));
}
