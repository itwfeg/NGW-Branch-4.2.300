#ifndef DBTEXTSEARCHQUERY_H
#define DBTEXTSEARCHQUERY_H



#include <iterator>


#include "edb.h"

#include DBQUERY_INC
#include DBQUERYI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif

	template <class DBEntity>
	class DBTextSearchQuery : public DBQuery<DBEntity>
	{
		// DBProductsRM, DBOperatorsRM, etc., classes should be friends of
		// DBTextSearchQuery template because they need access to the private constructor.
    #if defined(PAL_WIN32_RUN)
		friend typename DBEntity::manager_type;
    #endif
    #if defined(LINUX_FIX)
        friend class DBClassesRM;
        friend class DBConfigurationsRM;
        friend class DBLabelTextsRM;
        friend class DBMarqueesRM;
        friend class DBMessagesRM;
        friend class DBNotesRM;
        friend class DBOperatorNotesRM;
        friend class DBOperatorsRM;
        friend class DBProductDefaultsRM;
        friend class DBTextsRM;
        friend class DBTransactionTypesRM;
    #endif

		public:
			// --- Types and Subtypes ---
			typedef DBEntity value_type;
			typedef typename DBEntity::interface_type decorated_type;
			typedef typename DBEntity::manager_type manager_type;
			typedef size_t size_type;  /* (SJJC port 6/19/06) */

			typedef typename DBQuery<DBEntity>::iterator iterator;
			typedef typename DBQuery<DBEntity>::const_iterator const_iterator;
			typedef typename DBQuery<DBEntity>::reverse_iterator reverse_iterator;
			typedef typename DBQuery<DBEntity>::const_reverse_iterator const_reverse_iterator;

			typedef DBEntity* pointer;
			typedef const DBEntity* const_pointer;
			typedef DBEntity& reference;
			typedef const DBEntity& const_reference;

			typedef std::string (DBEntity::* textMethod_type)() const ;
			typedef bool compare_function(const std::string&, const std::string&);


			// --- Constructors and destructor ---
			~DBTextSearchQuery() {}


			// --- Processing methods ---
			// Processing methods for iterators
			iterator begin();
#ifdef WIN32_OS
			const iterator begin() const;
#else
			const_iterator begin() const;
#endif

//			reverse_iterator rbegin();
//			const_reverse_iterator rbegin() const;

			size_type size() const;


			// Processing methods for element access
//			reference operator[] (size_type n);    // unchecked access
//			const_reference operator[] (size_type n) const;

//			reference at(size_type n);             // checked access
//			const_reference at(size_type n) const;

//			reference front();                      // first element
//			const_reference front() const;
//			reference back();                      // last element
//			const_reference back() const;


		protected:
			// The constructor is only available by friend class DBProductsRM
			DBTextSearchQuery(DBManager* dbm, textMethod_type method, const std::string& key,
					edb_adapter::DBQueryI<typename DBEntity::interface_type>* d, manager_type& rm,
					compare_function comp = util_string::isSubstringOfDBText)
					: DBQuery<DBEntity>(dbm, d, rm), keyword_(key), textMethod_(method), compare_(comp) {}

			// The following two methods are only used by the query_iterator class and
			// overridden by any subclasses
			DBEntity* next(const DBEntity* e);
			DBEntity* previous(const DBEntity* e);


		private:
			std::string keyword_;
			textMethod_type textMethod_;
			compare_function* compare_;

			// Do not allow the default copy constructor.
			DBTextSearchQuery(const DBTextSearchQuery<DBEntity>& q);
			// Do not allow the default assignment operator.
			DBTextSearchQuery operator=(const DBTextSearchQuery<DBEntity>& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif




#ifndef NO_NAMESPACES
namespace edb
{
#endif


template <class DBEntity>
typename DBTextSearchQuery<DBEntity>::iterator DBTextSearchQuery<DBEntity>::begin()
{
	iterator iter = DBQuery<DBEntity>::begin();

#if defined(LINUX_FIX)
	while ( iter != DBQuery<DBEntity>::end() )
#else
	while ( iter != end() )
#endif
	{
//		if ( util_string::isSubstringOfDBText(keyword_, ((*iter).*textMethod_)()) )
		if ( compare_(keyword_, ((*iter).*textMethod_)()) )
			break;

		++iter;
	}

	return iter;
}


template <class DBEntity>
#ifdef WIN32_OS
const typename DBTextSearchQuery<DBEntity>::iterator DBTextSearchQuery<DBEntity>::begin() const
#else
typename DBTextSearchQuery<DBEntity>::const_iterator DBTextSearchQuery<DBEntity>::begin() const
#endif
{
	iterator iter = DBQuery<DBEntity>::begin();

#if defined(LINUX_FIX)
	while ( iter != DBQuery<DBEntity>::end() )
#else
	while ( iter != end() )
#endif
	{
//		if ( util_string::isSubstringOfDBText(keyword_, ((*iter).*textMethod_)()) )
		if ( compare_(keyword_, ((*iter).*textMethod_)()) )
			break;

		++iter;
	}

	return iter;
}


template <class DBEntity>
typename DBTextSearchQuery<DBEntity>::size_type DBTextSearchQuery<DBEntity>::size() const
{
	size_type size = 0;
#if defined(LINUX_FIX)
	iterator iter = DBQuery<DBEntity>::begin();
#else
	iterator iter = begin();
#endif

#if defined(LINUX_FIX)
	while ( iter != DBQuery<DBEntity>::end() )
#else
	while ( iter != end() )
#endif
	{
		++size;
		++iter;
	}

	return size;
}


template <class DBEntity>
DBEntity* DBTextSearchQuery<DBEntity>::next(const DBEntity* r)
{
	DBEntity* entity = DBQuery<DBEntity>::next(r);

	while ( entity )
	{
//		if ( util_string::isSubstringOfDBText(keyword_, (entity->*textMethod_)()) )
		if ( compare_(keyword_, (entity->*textMethod_)()) )
			break;

		DBEntity* tmp = entity;
		entity = DBQuery<DBEntity>::next(entity);
		delete tmp;
	}

	return entity;
}


template <class DBEntity>
DBEntity* DBTextSearchQuery<DBEntity>::previous(const DBEntity* r)
{
	DBEntity* entity = DBQuery<DBEntity>::previous(r);

	while ( entity )
	{
//		if ( util_string::isSubstringOfDBText(keyword_, (entity->*textMethod_)()) )
		if ( compare_(keyword_, (entity->*textMethod_)()) )
			break;

		DBEntity* tmp = entity;
		entity = DBQuery<DBEntity>::previous(entity);
		delete tmp;
	}

	return entity;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif



#endif // DBTEXTSEARCHQUERY_H

