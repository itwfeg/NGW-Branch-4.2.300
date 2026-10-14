#include <stdexcept>



#include "dblocale.h"
#include "dbcollate.h"
#include "collate_en.h"
#include "collate_es.h"
#include "dbctype.h"
#include "ctype_en.h"
#include "ctype_es.h"



#ifdef VXWORKS
#	include "..\thread\vxworkstasks.h"
#	include "..\thread\thread.h"
#endif



#ifndef NO_NAMESPACES
namespace std
{
#endif



#ifdef VXWORKS
class LocaleSingleton : public VxWorksTasks::DeleteTaskHook
#else
class LocaleSingleton
#endif
{
	public:
		// --- Types and subtypes ---
		typedef map<Thread<>::ThreadIDType, const locale*> glocaleContainerType;

		// --- Constructors and destructor ---
		static LocaleSingleton& getInstance();
		static void destroyInstance()
				{ Lock< Mutex<> > lock(_singletonMutex_); delete _locimp_; _locimp_ = 0; }

		// --- Accessor methods ---

		// --- Processing methods ---
		// Registry methods
		const locale* getRegisteredGLocale();
		bool registerGLocale(const locale* l);
		bool deregisterGLocale(const locale* l);

#ifdef VXWORKS
		void taskDeleted(VxWorksTasks::ThreadIDType id);
#endif

	private:
		// --- Data members ---
		glocaleContainerType registry_;
		Mutex<> registryMutex_;

		// Singleton data members
		static Mutex<> _singletonMutex_;
		static LocaleSingleton* _locimp_;
		static void* _dummy_;
		static void* privateInitializer() { LocaleSingleton::getInstance(); return 0; }

		// --- Constructors and destructor ---
		// The contents of the following methods are commented out because the Application's
		// Configuration Manager is created by the Main task, which opens the database for
		// it, but then is used by any other task long after the Main task ceased to exist.
		// If we register for task deletion and delete the corresponding DBManager object
		// there will probably be problems.
		// [Reenabled to test the code since the initial problem has apparently gone away.]
#ifdef VXWORKS
		LocaleSingleton() : registry_(), registryMutex_()
				{ if ( !VxWorksTasks::registerDeleteTaskHook(this) ) throw; }
		~LocaleSingleton() { VxWorksTasks::unregisterDeleteTaskHook(this); }
#else
		LocaleSingleton() : registry_(), registryMutex_() {}
		~LocaleSingleton() {}
#endif
};



// Here we initialize the identifiers for the known facet types, before
// they are used in initializeLocales
locale::id collate::id = locale::id();
locale::id ctype::id = locale::id();



int locale::id::_nextID_ = 0;
Mutex<> locale::id::_idMutex_ = Mutex<>();
locale::localeContainer* locale::_locales_;
Mutex<> locale::_localesMutex_ = Mutex<>();
locale* locale::_defaultLocale_ = initializeLocales();


class facetContainer
{
	public:
		facetContainer(const string& name) 
				: referenceCounter_(0), referenceMutex_(), name_(name), facets_(10) {}


		const std::string& getName() { return name_; }
		locale::facet* getFacet(int id) 
				{ if ( (int)facets_.size() > id ) return facets_[id]; return 0; }


		void add(int id, locale::facet* f) 
				{ if ( (int)facets_.size() < id + 1 ) facets_.resize(id + 1, 0); facets_[id] = f; }


		static facetContainer* addReference(facetContainer* fc) 
				{ Lock< Mutex<> > lock(fc->referenceMutex_); ++fc->referenceCounter_; return fc; }
		static void removeReference(facetContainer* fc) 
				{ Lock< Mutex<> > lock(fc->referenceMutex_); if ( --fc->referenceCounter_ == 0 ) delete fc; }

	private:
		int referenceCounter_;
		Mutex<> referenceMutex_;
		string name_;
		vector<locale::facet*> facets_;
};



locale::locale() throw() : facets_(0) 
{
	const locale* gl = LocaleSingleton::getInstance().getRegisteredGLocale();
	if ( gl )
		facets_ = facetContainer::addReference(gl->facets_);
	else
		facets_ = facetContainer::addReference(_defaultLocale_->facets_);
}


locale::locale(const locale& l) throw() : facets_() 
{		 
	facets_ = facetContainer::addReference(l.facets_);
}


locale::locale(const char* p) : facets_(0)
{
	locale* l = retrieveLocale(string(p));
	if ( l )
		facets_ = facetContainer::addReference(l->facets_);
	else
		throw std::range_error(p);
}


locale::locale(facetContainer* fc) : facets_(0) 
{
	facets_ = facetContainer::addReference(fc);
}


locale::~locale() throw()
{
	facetContainer::removeReference(facets_);
}


string locale::name() const
{
	return facets_->getName(); 
}


const locale::facet* locale::getFacet(int id) const
{
	return facets_->getFacet(id);
}


bool locale::operator()(const string& str1, const string& str2) const
{
	const char* b1 = str1.data();
	const char* b2 = str2.data();
	std::collate* col = dynamic_cast<std::collate*>(facets_->getFacet(std::collate::id()));

	return (col->compare(b1, b1 + str1.size(), b2, b2 + str2.size()) == -1);
}


locale locale::global(const locale& l) 
{
	LocaleSingleton& singleton = LocaleSingleton::getInstance();
	const locale* previous = singleton.getRegisteredGLocale();
	if ( previous )
		singleton.deregisterGLocale(previous);
	else
		previous = _defaultLocale_;
	
	singleton.registerGLocale(retrieveLocale(l.name())); 
	
	return *previous; 
}


locale* locale::retrieveLocale(const std::string& name)
{
	{
		Lock< Mutex<> > lock(_localesMutex_); 
		localeContainer::iterator iter = _locales_->find(name); 
		if ( iter != _locales_->end() )
			return iter->second;
	}


	return 0;
}


// This static method initializes the default locales and returns one
// of them to become the default locale.
locale* locale::initializeLocales()
{
	_locales_ = new locale::localeContainer();

	// Create the English locale
	string en("en");
	locale* locale_en = new locale(new facetContainer(en.c_str()));
	locale_en->add(new ctype_en());
	locale_en->add(new collate_en());

	// Store English locale
	_locales_->insert(make_pair(en, locale_en));

	// Create the Spanish locale
	string es("es");
	locale* locale_es = new locale(new facetContainer(es.c_str()));
	locale_es->add(new ctype_es(0));
	locale_es->add(new collate_es());

	// Store English locale
	_locales_->insert(make_pair(es, locale_es));

	return locale_en;
}




// --- LocaleSingleton ---
// Definition/allocation of static members.
Mutex<> LocaleSingleton::_singletonMutex_ = Mutex<>();
LocaleSingleton* LocaleSingleton::_locimp_ = 0;
void* LocaleSingleton::_dummy_ = LocaleSingleton::privateInitializer();


// Singleton method
LocaleSingleton& LocaleSingleton::getInstance()
{
	if ( _locimp_ == 0 )
	{
		Lock< Mutex<> > lock(_singletonMutex_);
		if ( _locimp_ == 0 )
			_locimp_ = new LocaleSingleton();
	}

	return *_locimp_;
}


const locale* LocaleSingleton::getRegisteredGLocale()
{
	Thread<>::ThreadIDType tid = Thread<>::getCurrentThreadID();

	Lock< Mutex<> > lock(registryMutex_);

	glocaleContainerType::iterator iter = registry_.find(tid);
	if ( iter != registry_.end() )
		return iter->second;
	else
		return 0;
}


bool LocaleSingleton::registerGLocale(const locale* l)
{
	Thread<>::ThreadIDType tid = Thread<>::getCurrentThreadID();

	Lock< Mutex<> > lock(registryMutex_);

	pair<glocaleContainerType::iterator, bool> result = registry_.insert(make_pair(tid,l));

	return result.second;
}


bool LocaleSingleton::deregisterGLocale(const locale* l)
{
	bool result = false;
	Thread<>::ThreadIDType tid = Thread<>::getCurrentThreadID();

	Lock< Mutex<> > lock(registryMutex_);

	glocaleContainerType::iterator iter = registry_.find(tid);
	if ( iter != registry_.end() )
	{
		if ( iter->second == l )
		{
			registry_.erase(iter);
			result = true;
		}
	}

	if ( !result )
	{
		iter = registry_.begin();
		while ( iter != registry_.end() && !result )
		{
			if ( iter->second == l )
			{
				registry_.erase(iter);
				result = true;
			}

			++iter;
		}
	}

	return result;
}


#ifdef VXWORKS
void LocaleSingleton::taskDeleted(VxWorksTasks::ThreadIDType id)
{
	const locale* l = 0;

	{
		Lock< Mutex<> > lock(registryMutex_);

		glocaleContainerType::iterator iter = registry_.find(id);
		if ( iter != registry_.end() )
			l = iter->second;
	}

	if ( l )
		deregisterGLocale(l);

	// The locales are not deleted because
}
#endif



#ifndef NO_NAMESPACES
}
#endif

