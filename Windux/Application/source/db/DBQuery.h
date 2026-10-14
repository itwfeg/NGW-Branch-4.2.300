#ifndef DBQUERY_H
#define DBQUERY_H


#include <iterator>
#include "util/e_string/utilstring.h"

#include "edb.h"
#include DBACCESSENTITY_INC
#include DBQUERYI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include DEBUG_INC


//	This fixes a bug where we have a pointer to a query being deleted and a smart pointer deleting that same query
//	pointer when the smart pointer goes out of scope. Case in point: go to Void Transactions in operate mode to view
//	the current transactions. Via comm, delete all transactions. When you go back to Void Transactions the Session
//	has an exception because we are deleting the pointer to the query twice. The problem is that when a query is
//	created, the appropriate db record manager is adding that query pointer to a container (qsRegistry_) via
//	registerQ(DBQuery< T >* query). After we do a deleteAll via Comm, the DBTransactionsRM destructor calls
//	disableAllQs() which clears out that container, deleting each query in it. Now we have a query pointer that
//	no longer exists in the smart pointer. Since this container (qsRegistry_) is used for the showAllQueries()
//	method, it is used for debugging purposes only. We have decided to not allow queries to be added to this
//	container to eliminate this problem. If the database code had been completely converted to use smart pointers,
//	this would not have been a problem. [CCD 6/06/11]
#define PTR_549


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;


	template <class DBEntity>
	class DBQuery : public DBAccessEntity
	{
			// DBProductsRM, DBOperatorsRM, etc., classes should be friends of
			// DBQuery template because they need access to the private constructor.
    #if defined( WIN32 )
			friend typename DBEntity::manager_type;
    #endif
			friend class DBBatchesRM;
			friend class DBBarcodeScannersRM;
			friend class DBCategoriesRM;
			friend class DBCategoryDetailedStatisticsRM;
			friend class DBCategoryStatisticsRM;
			friend class DBClassesRM;
			friend class DBConfigurationsRM;
			friend class DBCoolCategoriesRM;
			friend class DBCoolCategoryCountriesRM;
			friend class DBCoolCategoryPreTextsRM;
			friend class DBCoolCategoryProductionStepsRM;
			friend class DBCoolCountriesRM;
			friend class DBCoolListsRM;
			friend class DBCoolListTextsRM;
			friend class DBCoolPreTextsRM;
			friend class DBCoolProductionStepsRM;
			friend class DBCoolTextsRM;
			friend class DBCuttingTestsRM;        
			friend class DBDeptsRM;
			friend class DBDevicesRM;
			friend class DBEmailAlertsRM;
			friend class DBExpirationLabelsRM;
			friend class DBKnifeTimeHistoryRM;
			friend class DBFlashFilesRM;
			friend class DBLabelsRM;
			friend class DBLabelTextsRM;
			friend class DBNetworkErrorsRM;
			friend class DBNutritionsRM;
			friend class DBOperatorNotesRM;
			friend class DBOperatorsRM;
			friend class DBPresentationMethodDetailedStatisticsRM;
			friend class DBPresentationMethodsRM;
			friend class DBPresentationMethodStatisticsRM;
			friend class DBProductDefaultsRM;
			friend class DBProductDetailedStatisticsRM;
			friend class DBProductsRM;
			friend class DBProductStatisticsRM;
			friend class DBProductTextsRM;
			friend class DBProductTextCfgsRM;
			friend class DBRecallsRM;
			friend class DBROMLabelsRM;
			friend class DBScalesRM;
			friend class DBSoftKeyFeaturesRM;
			friend class DBSoftKeyGroupsRM;
			friend class DBSoftKeysRM;
			friend class DBSoftKeyStylesRM;
			friend class DBSpecialProgramsRM;
			friend class DBStreamingGraphicsRM;
			friend class DBSynchronizationChangesRM;
			friend class DBTransactionsRM;
			friend class DBTransactionTypesRM;
			friend class DBWrapperErrorHistoryRM;

		public:

			// --- Types and Subtypes ---
			class query_iterator;
			friend class query_iterator;
#ifdef VXWORKS
			typedef std::bidirectional_iterator<std::bidirectional_iterator_tag, DBEntity> DBEntityIterator;
#else
			typedef std::iterator<std::bidirectional_iterator_tag, DBEntity> DBEntityIterator;
#endif

			class query_iterator : public DBEntityIterator
			{
					friend class DBQuery<DBEntity>;

				public:

					query_iterator(DBQuery<DBEntity>* q, DBEntity* e)
							: enabled_(false), container_(q), entity_(e) 
							{ if ( container_->registerIter(this) ) enabled_ = true; }

					query_iterator(const query_iterator& rhs) 
							: enabled_(rhs.enabled_), container_(rhs.container_), entity_(rhs.entity_ ? DBQuery<DBEntity>::copyEntity(*rhs.entity_) : 0)
							{ if ( container_->registerIter(this) ) enabled_ = true; }

					~query_iterator() { if ( enabled_ ) container_->deregisterIter(this); if ( entity_ ) delete entity_; }


					bool isEnabled() const { return enabled_; }

					query_iterator& operator=(const query_iterator& rhs);
					DBEntity& operator*() { return *entity_; }
					const DBEntity& operator*() const { return *entity_; }
					DBEntity* operator->() { return entity_; }
					const DBEntity* operator->() const { return entity_; }
					query_iterator& operator++();
					const query_iterator& operator++() const;
					query_iterator operator++(int);
					const query_iterator operator++(int) const;
					query_iterator& operator--();
					const query_iterator& operator--() const;
					query_iterator operator--(int);
					const query_iterator operator--(int) const;
					bool operator==(const query_iterator& rhs) const;
					bool operator!=(const query_iterator& rhs) const { return !(*this == rhs); }

					void show();


				private:

					bool enabled_;               // If 'true' the Query (container_) is still alive
					DBQuery<DBEntity>* container_;
					mutable DBEntity* entity_;

					void disable() { enabled_ = false; }

					// Prevent default constructor
					query_iterator();
			};


			typedef DBEntity value_type;
			typedef typename DBEntity::interface_type decorated_type;
			typedef typename DBEntity::manager_type manager_type;
			typedef typename DBEntity::interface_type DBEntityI;
			typedef typename DBEntity::manager_type DBEntityRM;
			typedef size_t size_type;  /* (SJJC port 6/19/06) */

			typedef query_iterator iterator;
			typedef const query_iterator const_iterator;
			typedef std::reverse_iterator<iterator> reverse_iterator;
			typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

			typedef DBEntity* pointer;
			typedef const DBEntity* const_pointer;
			typedef DBEntity& reference;
			typedef const DBEntity& const_reference;


			// --- Constructors and destructor ---
			virtual ~DBQuery();


			// --- Accessor methods ---
			DBAccessEntityType getAccessEntityType() const { return DBAccessEntityQuery; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }


			// --- Processing methods ---
			// Processing methods for iterators
			virtual iterator begin();
			virtual const_iterator begin() const;
			virtual iterator end() { return iterator(this, 0); }
			virtual const_iterator end() const { return const_iterator(const_cast<DBQuery<DBEntity>*>(this), 0); }

			// Processing methods for reverse iterators that temporarily use regular iterators
			virtual iterator rbegin();
			virtual const_iterator rbegin() const;
			virtual iterator rend() { return iterator(this, 0); }
			virtual const_iterator rend() const { return const_iterator(const_cast<DBQuery<DBEntity>*>(this), 0); }

			virtual size_type size() const;
			virtual bool empty() const;


			// Processing methods for element access
//			virtual iterator operator[] (size_type n);    // unchecked access
//			virtual const_iterator operator[] (size_type n) const;

//			virtual iterator at(size_type n);             // checked access
//			virtual const_iterator at(size_type n) const;

//			virtual iterator front();                     // first element
//			virtual const_iterator front() const;
//			virtual iterator back();                      // last element
//			virtual const_iterator back() const;


			void show();
			void showAllIterators();


		protected:

			// The constructor is only available to friend class DBProductsRM, etc.
			DBQuery(DBManager* dbm, edb_adapter::DBQueryI<DBEntityI>* d, manager_type& rm);

			// The following two methods are only used by the query_iterator class and
			// overridden by any subclasses
			virtual DBEntity* first();
			virtual DBEntity* next(const DBEntity* e);
			virtual DBEntity* previous(const DBEntity* e);
			virtual DBEntity* last();


		private:

			DBManager* dbm_;
			bool enabled_;
			manager_type& recordManager_;
			DBRecordManager::DBRecordType recordType_;
			util_registry::Registry<iterator> iterRegistry_;
			edb_adapter::DBQueryI<typename DBEntity::interface_type>* decorated_;


			void disable();


			// Do not allow the default copy constructor.
			DBQuery(const DBQuery<DBEntity>& q);
			// Do not allow the default assignment operator.
			DBQuery operator=(const DBQuery<DBEntity>& rhs);


			bool registerIter(iterator* iter);
			bool deregisterIter(iterator* iter);
			void disableAllIterators();

			// This method is used here to avoid making iterator a friend of DBEntity
			// which would cause a problem with circular dependencies (this module
			// already includes dbentity.h and making iterator a friend of DBEntity
			// would cause that module to include this one).
			static DBEntity* copyEntity(const DBEntity& p) { return new DBEntity(p); }
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif



#include DBSTATUSRM_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



template <class DBEntity>
DBQuery<DBEntity>::DBQuery(DBManager* dbm, edb_adapter::DBQueryI<typename DBEntity::interface_type>* d,
		manager_type& rm) : dbm_(dbm), enabled_(false), recordManager_(rm), recordType_(rm.getRecordType()), decorated_(d)
{
#ifndef PTR_549
	if ( recordManager_.registerQ(this) )
#endif
		enabled_ = true;
}


template <class DBEntity>
DBQuery<DBEntity>::~DBQuery()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessQueryDispose);

	disable();

#ifndef PTR_549
	recordManager_.deregisterQ(this);
#endif

	if ( decorated_ )
	{
		delete decorated_;
		decorated_ = 0;
	}
}


template <class DBEntity>
typename DBQuery<DBEntity>::iterator DBQuery<DBEntity>::begin()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessQueryBegin);

		if (decorated_)
		{
			typename DBEntity::interface_type* e = decorated_->first();

			if ( e )
				return iterator(this, new DBEntity(e));
		}
	}

	return end();
}


template <class DBEntity>
const typename DBQuery<DBEntity>::iterator DBQuery<DBEntity>::begin() const
//typename DBQuery<DBEntity>::const_iterator DBQuery<DBEntity>::begin() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessQueryBegin);

		if (decorated_)
		{
			typename DBEntity::interface_type* e = decorated_->first();

			if ( e )
				return const_iterator(const_cast<DBQuery<DBEntity>*>(this), new DBEntity(e));
		}
	}

	return end();
}


template <class DBEntity>
typename DBQuery<DBEntity>::iterator DBQuery<DBEntity>::rbegin()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessQueryReverseBegin);

		if (decorated_)
		{
			typename DBEntity::interface_type* e = decorated_->last();

			if ( e )
				return iterator(this, new DBEntity(e));
		}
	}

	return rend();
}


template <class DBEntity>
const typename DBQuery<DBEntity>::iterator DBQuery<DBEntity>::rbegin() const
//typename DBQuery<DBEntity>::const_iterator DBQuery<DBEntity>::begin() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessQueryReverseBegin);

		if (decorated_)
		{
			typename DBEntity::interface_type* e = decorated_->last();

			if ( e )
				return const_iterator(const_cast<DBQuery<DBEntity>*>(this), new DBEntity(e));
		}
	}

	return rend();
}


template <class DBEntity>
typename DBQuery<DBEntity>::size_type DBQuery<DBEntity>::size() const
{
	size_type count = 0;

	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessQuerySize);

		if (decorated_)
			count = decorated_->size() ;
	}
		
	return count; 
}


template <class DBEntity>
bool DBQuery<DBEntity>::empty() const
{
	bool isEmpty = true;

	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessQueryEmpty);

		if (decorated_)
			isEmpty = decorated_->empty();
	}
	
	return isEmpty;
}


template <class DBEntity>
void DBQuery<DBEntity>::show()
{
	Debug::trace("database.show", 20, DBEntityRM::DESCRIPTOR + " Query 0x " + util_string::ptrtostring(this) +
		(enabled_ ? ": ENABLED" : ": DISABLED"));  /* (SJJC port 6/19/06) */

	if ( enabled_ && decorated_ )
		decorated_->show();
}


template <class DBEntity>
void DBQuery<DBEntity>::showAllIterators()
{
	iterRegistry_.process_mem(&iterator::show);
}


template <class DBEntity>
DBEntity* DBQuery<DBEntity>::first()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessQueryFirst);

		if (decorated_)
		{
			typename DBEntity::interface_type* newop = decorated_->first();

			if ( newop )
				return new DBEntity(newop);
		}
	}

	return 0;
}


//template <class DBEntity>
//DBEntity* DBQuery<DBEntity>::next()
template <class DBEntity>
DBEntity* DBQuery<DBEntity>::next(const DBEntity* r)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessQueryNext);

		if (decorated_)
		{
			typename DBEntity::interface_type* newop = decorated_->next(r->decorated_);

			if ( newop )
				return new DBEntity(newop);
		}
	}

	return 0;
}


//template <class DBEntity>
//DBEntity* DBQuery<DBEntity>::previous()
template <class DBEntity>
DBEntity* DBQuery<DBEntity>::previous(const DBEntity* r)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessQueryPrevious);

		if (decorated_)
		{
			typename DBEntity::interface_type* newop = decorated_->previous(r->decorated_);

			if ( newop )
				return new DBEntity(newop);
		}
	}

	return 0;
}


template <class DBEntity>
DBEntity* DBQuery<DBEntity>::last()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessQueryLast);

		if (decorated_)
		{
			typename DBEntity::interface_type* newop = decorated_->last();

			if ( newop )
				return new DBEntity(newop);
		}
	}

	return 0;
}


template <class DBEntity>
void DBQuery<DBEntity>::disable()
{
	// Mark this query as not enabled so that it cannot be used
	enabled_ = false; 

	// Now disable all iterators associated with this query
	disableAllIterators();

	if ( decorated_ ) 
	{
		delete decorated_; 
		decorated_ = 0; 
	}
}


template <class DBEntity>
bool DBQuery<DBEntity>::registerIter(iterator* iter)
{
	return iterRegistry_.add(iter);
}


template <class DBEntity>
bool DBQuery<DBEntity>::deregisterIter(iterator* iter)
{
	return iterRegistry_.remove(iter);
}


template <class DBEntity>
void DBQuery<DBEntity>::disableAllIterators()
{
	iterRegistry_.process_mem(&iterator::disable);
	iterRegistry_.removeAll();
}




template <class DBEntity>
//#ifdef WIN32_OS
#if defined( WIN32 ) || defined( __linux__ )
typename DBQuery<DBEntity>::query_iterator& DBQuery<DBEntity>::query_iterator::operator=(
		const query_iterator& rhs)
#else
DBQuery<DBEntity>::query_iterator& DBQuery<DBEntity>::query_iterator::operator=(
		const query_iterator& rhs)
#endif
{
	// Check for self-assignment
	if ( *this != rhs )
	{
		// Delete the old DBEntity
		if (entity_)
		{
			delete entity_;
			entity_ = 0;
		}

		// Assign values to this iterator
		enabled_ = rhs.enabled_;
		container_ = rhs.container_;
		if ( rhs.entity_ )
			entity_ = DBQuery<DBEntity>::copyEntity(*rhs.entity_);
	}

	return *this;
}


template <class DBEntity>
bool DBQuery<DBEntity>::query_iterator::operator==(const query_iterator& rhs) const
{
	if ( rhs.container_ != container_ )
		return false;

	if ( rhs.entity_ )
		return entity_ ? (entity_ == rhs.entity_) : false;
		// It should've been: "return entity_ ? (*entity_ == rhs.entity_) : false;" but
		// this operator is only used to compare an iterator with the "end" iterator
		// for which the difference is irrelevant.
	else
		return entity_ ? false : true;
}


template <class DBEntity>
//#ifdef WIN32_OS
#if defined( WIN32 ) || defined( __linux__ )
typename DBQuery<DBEntity>::query_iterator& DBQuery<DBEntity>::query_iterator::operator++()
#else
DBQuery<DBEntity>::query_iterator& DBQuery<DBEntity>::query_iterator::operator++()
#endif
{
	if ( enabled_ )
	{
		if ( entity_ )
		{
			// Save the current DBEntity
			DBEntity* tmp = entity_;

			// Find the next DBEntity
			entity_ = container_->next(entity_);

			// Delete the old DBEntity
			delete tmp;
		}
	}
	else
	{
		// Delete the old DBEntity
		if (entity_)
		{
			delete entity_;
			entity_ = 0;
		}
	}

	return *this;
}


template <class DBEntity>
//#ifdef WIN32_OS
#if defined( WIN32 ) || defined( __linux__ )
const typename DBQuery<DBEntity>::query_iterator& DBQuery<DBEntity>::query_iterator::operator++() const
#else
const DBQuery<DBEntity>::query_iterator& DBQuery<DBEntity>::query_iterator::operator++() const
#endif
{
	if ( enabled_ )
	{
		if ( entity_ )
		{
			// Save the current DBEntity
			DBEntity* tmp = entity_;

			// Find the next DBEntity
			entity_ = container_->next(entity_);

			// Delete the old DBEntity
			delete tmp;
		}
	}
	else
	{
		// Delete the old DBEntity
		if (entity_)
		{
			delete entity_;
			entity_ = 0;
		}
	}

	return *this;
}


template <class DBEntity>
//#ifdef WIN32_OS
#if defined( WIN32 ) || defined( __linux__ )
typename DBQuery<DBEntity>::query_iterator DBQuery<DBEntity>::query_iterator::operator++(int)
#else
DBQuery<DBEntity>::query_iterator DBQuery<DBEntity>::query_iterator::operator++(int)
#endif
{
	iterator tmp = *this;

	++*this;

	return tmp;
}


template <class DBEntity>
//#ifdef WIN32_OS
#if defined( WIN32 ) || defined( __linux__ )
const typename DBQuery<DBEntity>::query_iterator DBQuery<DBEntity>::query_iterator::operator++(int) const
#else
const DBQuery<DBEntity>::query_iterator DBQuery<DBEntity>::query_iterator::operator++(int) const
#endif
{
	query_iterator tmp = *this;	

	++*this;

	return tmp;
}


template <class DBEntity>
//#ifdef WIN32_OS
#if defined( WIN32 ) || defined( __linux__ )
typename DBQuery<DBEntity>::query_iterator& DBQuery<DBEntity>::query_iterator::operator--()
#else
DBQuery<DBEntity>::query_iterator& DBQuery<DBEntity>::query_iterator::operator--()
#endif
{
	if ( enabled_ )
	{
		if ( entity_ )
		{
			// Save the current DBEntity
			DBEntity* tmp = entity_;

			// Find the previous DBEntity
			entity_ = container_->previous(entity_);

			// Delete the old DBEntity
			delete tmp;
		}
		else
			entity_ = container_->last();
	}
	else
	{
		// Delete the old DBEntity
		if (entity_)
		{
			delete entity_;
			entity_ = 0;
		}
	}

	return *this;
}


template <class DBEntity>
//#ifdef WIN32_OS
#if defined( WIN32 ) || defined( __linux__ )
const typename DBQuery<DBEntity>::query_iterator& DBQuery<DBEntity>::query_iterator::operator--() const
#else
const DBQuery<DBEntity>::query_iterator& DBQuery<DBEntity>::query_iterator::operator--() const
#endif
{
	if ( enabled_ )
	{
		if ( entity_ )
		{
			// Save the current DBEntity
			DBEntity* tmp = entity_;

			// Find the previous DBEntity
			entity_ = container_->previous(entity_);

			// Delete the old DBEntity
			delete tmp;
		}
		else
			entity_ = container_->last();
	}
	else
	{
		// Delete the old DBEntity
		if (entity_)
		{
			delete entity_;
			entity_ = 0;
		}
	}

	return *this;
}


template <class DBEntity>
//#ifdef WIN32_OS
#if defined( WIN32 ) || defined( __linux__ )
typename DBQuery<DBEntity>::query_iterator DBQuery<DBEntity>::query_iterator::operator--(int)
#else
DBQuery<DBEntity>::query_iterator DBQuery<DBEntity>::query_iterator::operator--(int)
#endif
{
	query_iterator tmp = *this;

	--*this;

	return tmp;
}


template <class DBEntity>
//#ifdef WIN32_OS
#if defined( WIN32 ) || defined( __linux__ )
const typename DBQuery<DBEntity>::query_iterator DBQuery<DBEntity>::query_iterator::operator--(int) const
#else
const DBQuery<DBEntity>::query_iterator DBQuery<DBEntity>::query_iterator::operator--(int) const
#endif
{
	query_iterator tmp = *this;

	--*this;

	return tmp;
}


template <class DBEntity>
void DBQuery<DBEntity>::query_iterator::show()
{
	Debug::trace("database.show", 20, DBEntityRM::DESCRIPTOR + " Iterator instance address: " +
			ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBQUERY_H
