#ifndef DBLOCALE_H
#define DBLOCALE_H



#include <string>
#include <map>
#include <typeinfo>



//#ifdef VXWORKS
#if defined( __linux__ ) || defined( VXWORKS )
#include "../thread/mutex.h"
#include "../thread/lock.h"
#else
#include "..\thread\mutex.h"
#include "..\thread\lock.h"
#endif



#ifndef NO_NAMESPACES
namespace std
{
#endif



class facetContainer;



class locale
{
	public:
		class id
		{
			// This is a hack because the GNU compiler does not allow the instanciation of the
			// static facet id's unless using the copy constructor (which is private).
			friend class ctype;
			friend class collate;

			public:
				id () { Lock< Mutex<> > lock(_idMutex_); id_ = _nextID_++; }
				~id() {}

				int operator()() const { return id_; }
				bool operator==(const id& i) const { return id_ == i.id_; }
				bool operator!=(const id& i) const { return !(*this == i); }
				bool operator< (const id& i) const { return id_ < i.id_; }
				bool operator<=(const id& i) const { return (*this < i) || (*this == i); }
				bool operator> (const id& i) const { return i < *this; }
				bool operator>=(const id& i) const { return !(*this < i); }

			private:
				int id_;

				static int _nextID_;
				static Mutex<> _idMutex_;

				// The following copy constructor should not be defined to prevent duplications of
				// facet ids. However, this is currently part of the hack described above.
				id(const id& id) : id_(id.id_) {}                  // not defined
				void operator=(const id&);      // not defined
		};

		class facet
		{
			protected:
				explicit facet(size_t r = 0) : keepAlive_(r != 0) {}
				virtual ~facet() {}

			private:
				bool keepAlive_;


				facet(const facet& );            // not defined
				void operator=(const facet&);    // not defined
		};


		typedef int category;
		static const category
			none      = 0,
			collate   = 1,
			ctype     = 1 << 1,
			monetary  = 1 << 2,
			numeric   = 1 << 3,
			time      = 1 << 4,
			messages  = 1 << 5,
			all = collate | ctype | monetary | numeric | time | messages;

		typedef std::map<std::string, locale*> localeContainer;


		locale() throw();
		locale(const locale& l) throw();
		explicit locale(const char* p);

		~locale() throw();

//		locale(const locale& l, const char* p, category c);
//		locale(const locale& l, const locale& o, category c);

//		template<class Facet> loclale(const locale& l, Facet* f);
//		template<class Facet> locale combine(const locale& l);

//		const locale& operator=(const locale& l) throw();

//		bool operator==(const locale& l) const;
//		bool operator!=(const locale& l) const;

		string name() const;

		const facet* getFacet(int id) const;

		bool operator()(const string& str1, const string& str2) const;

		static locale global(const locale& l); 
//		static const locale& classic();



	private:
		facetContainer* facets_;

		locale(facetContainer* fc);

		template<class Facet>
			void add(Facet* f) { facets_->add(Facet::id(), f); }


		static localeContainer* _locales_;
		static Mutex<> _localesMutex_;
		static void addLocale(locale* l) 
				{ Lock< Mutex<> > lock(_localesMutex_); _locales_->insert(make_pair(l->name(), l)); }
		static locale* retrieveLocale(const std::string& name);


		static locale* _defaultLocale_;
		static locale* initializeLocales();
};


template<class Facet> bool has_facet(const locale& l) throw()
		{ return l.getFacet(Facet::id) ? true : false; }

template<class Facet> const Facet& use_facet(const locale& l)     // may throw bad_cast
		{ const locale::facet* f = l.getFacet(Facet::id()); if ( f ) return static_cast<const Facet&>(*f); throw std::bad_cast(); }



#ifndef NO_NAMESPACES
}
#endif



#endif // DBLOCALE_H

