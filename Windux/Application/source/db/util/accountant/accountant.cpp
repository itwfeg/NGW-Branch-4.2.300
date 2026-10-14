#include "PAL_platformheader.h"

#include <map>



#include "accountant.h"
#include "accountanti.h"


#if	defined(LINUX_FIX)
#include "edb.h"
#include LOCK_INC
#include UTILSTRING_INC
#include DEBUG_INC
#else
#ifdef VXWORKS
	#include "util/thread/lock.h"
	#include "util/e_string/utilstring.h"
	#include "../../../util/Debug.h"
#else  /* (SJJC port 6/19/06) */
	#include "util/thread/lock.h"
	#include "util/e_string/utilstring.h"
	//#include "util/debug/debug.h"
#ifdef LINUX_FIX
#include "db_util_debug.h"
#endif      // LINUX_FIX
#endif
#endif


#ifndef NO_NAMESPACES
namespace util_acc
{
#endif



Accountant* Accountant::_instance_ = 0;
util_thread::Mutex<> Accountant::_singletonMutex_ = util_thread::Mutex<>();  /* (SJJC port 6/19/06) */



Accountant::Accountant()
		: capacity_(1000), currentCounters_(0)
{
	currentCounters_ = new Snapshot(capacity_);
}


Accountant& Accountant::getInstance()
{
	if ( !_instance_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_singletonMutex_);  /* (SJJC port 6/19/06) */

		if ( !_instance_ )
			_instance_ = new Accountant();
	}

	return *_instance_;
}


Accountant::~Accountant()
{
	delete currentCounters_;
}


int Accountant::registerDataType(const std::string& name)
{
	util_thread::Lock< util_thread::Mutex<> > lock(countersMutex_);  /* (SJJC port 6/19/06) */

	// Increment the number of types being tracked (don't use type 0)
	if ( currentCounters_->last_ + 1 >= currentCounters_->capacity_ )
	{
		// If there is not enough space to store it, increase the space
		Snapshot* oldCounters = currentCounters_;

		currentCounters_ = new Snapshot(oldCounters->capacity_ * 2);
		*currentCounters_ = *oldCounters;

		delete oldCounters;
	}

	return currentCounters_->newCounter(name);
}


void Accountant::addInstance(int type)
{
	util_thread::Lock< util_thread::Mutex<> > lock(countersMutex_);  /* (SJJC port 6/19/06) */

	++(currentCounters_->counters_[type]);
	++(currentCounters_->totals_[type]);
}


void Accountant::removeInstance(int type)
{
	util_thread::Lock< util_thread::Mutex<> > lock(countersMutex_);  /* (SJJC port 6/19/06) */

	--(currentCounters_->counters_[type]);
}


Accountant::Snapshot* Accountant::createSnapshotCopy()
{
	util_thread::Lock< util_thread::Mutex<> > lock(countersMutex_);  /* (SJJC port 6/19/06) */

	return new Snapshot(*currentCounters_);
}


void Accountant::showCounters(bool all)
{
	Snapshot* snapshot = createSnapshotCopy();
	std::multimap<int, int, std::greater<int> > container;  /* (SJJC port 6/19/06) */

	for ( int i = 1; i <= snapshot->last_; ++i )
	{
		if ( all || snapshot->counters_[i] )
			container.insert(std::make_pair(snapshot->counters_[i], i));  /* (SJJC port 6/19/06) */
	}

	showSortedCounters<std::multimap<int, int, std::greater<int> >::const_iterator>(snapshot, container.begin(), container.end());  /* (SJJC port 6/19/06) */

	delete snapshot;
}


void Accountant::showCounterDeltas(const Snapshot& snapshot, bool all)
{
	Snapshot* latest = createSnapshotCopy();
	std::multimap<int, int, std::greater<int> > container;  /* (SJJC port 6/19/06) */

	for ( int i = 1; i <= latest->last_; ++i )
	{
		int diff = latest->counters_[i] - (i <= snapshot.last_ ? snapshot.counters_[i] : 0);
		if ( all || diff )
			container.insert(std::make_pair(diff, i));  /* (SJJC port 6/19/06) */
	}

	showSortedCounters<std::multimap<int, int, std::greater<int> >::const_iterator>(latest, container.begin(), container.end());  /* (SJJC port 6/19/06) */

	delete latest;
}



template <class Iter>
void Accountant::showSortedCounters(const Snapshot* snapshot, Iter begin, Iter end)
{
	while ( begin != end )
	{
		int index = begin->second;

		Debug::trace("Accountant.showSortedCounters", 28, "Current Instances of " + *snapshot->names_[index] +
			" = " + util_string::inttostring(snapshot->counters_[index]) + " (of " +
				util_string::inttostring(snapshot->totals_[index]) + " total)");	/* (SJJC port 6/19/06) */

    #if defined(LINUX_FIX)
        PAL_absclass::PAL_sleep(1);
    #else
		Sleep(1);
    #endif

		++begin;
	}
}


int registerDataType(const std::string& name)
{
	return Accountant::getInstance().registerDataType(name);
}


void addInstance(int dataType)
{
	Accountant::getInstance().addInstance(dataType);
}


void removeInstance(int dataType)
{
	Accountant::getInstance().removeInstance(dataType);
}


void showInstanceCounters(bool all)
{
	Accountant::getInstance().showCounters(all);
}


Accountant::Snapshot* _savedSnapshot_ = 0;
void saveInstanceCounters()
{
	if ( _savedSnapshot_ )
		delete _savedSnapshot_;

	_savedSnapshot_ = Accountant::getInstance().createSnapshotCopy();
}


void showInstanceCounterDeltas(bool all)
{
	if ( _savedSnapshot_ )
		Accountant::getInstance().showCounterDeltas(*_savedSnapshot_, all);
}



#ifndef NO_NAMESPACES
} // namespace util_acc
#endif
