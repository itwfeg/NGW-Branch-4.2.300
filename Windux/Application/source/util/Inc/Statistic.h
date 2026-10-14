// Header file included by ANTLR UniLexer


#ifndef REM_LINUX_ERR
#include <tchar.h>
#endif

#ifndef Statistics_h
#define Statistics_h

#include <string>
#include <list>


//#define PROVIDE_STATISTICS


#ifdef PROVIDE_STATISTICS

namespace Util {
    class CStatisticsInterface
    {
    public:
		typedef std::list<std::wstring> string_container;

	    static std::wstring GetStatistics( const string_container &i_include, const string_container &i_exclude );

	    static bool Match( const std::wstring& name, const std::wstring& pattern );
    };

    class CStatisticsProvider;

    class CStatistic
    {
    public:
        virtual std::wstring Value( CStatisticsProvider *p ) = 0;
        static std::wstring IntegerValue( __int64 i );  // Translates an integer into a string
	    static std::wstring BooleanValue( bool b );

		// The purpose of the following two routines is to ensure that all pointers to
		// Statistics Provider classes will always show the same value; the one that
		// will appear as their "name" in the statistics text.
		static std::wstring PointerValue( CStatisticsProvider *p ) { return VoidPointerValue( (void*) p ); }
		static std::wstring PointerValue( void *p ) { return VoidPointerValue( (void*) p ); }

        const TCHAR *Name();

	private:
		static std::wstring VoidPointerValue( void *p );
    };

    template <class C>
		class TStatistic : public Util::CStatistic
    {
    public:
        virtual std::wstring Value( CStatisticsProvider *p )
		{
			return ProviderSpecificValue( (C*) p );
		}
        virtual std::wstring ProviderSpecificValue( C *p ) = 0;
    };

    class CStatisticsProvider
    {
    public:
	    void SetName( const std::wstring& i_name ) { m_name = i_name; }
        const std::wstring& Name();

    protected:
        CStatisticsProvider();

	    virtual ~CStatisticsProvider();

	    void Register( CStatistic *stat ) { m_statistics.push_back( stat ); }

    private:
	    friend class CStatisticsInterface;
        std::wstring m_name;

	    typedef std::list<CStatisticsProvider*> providers_container;
	    typedef std::list<CStatistic*> statistics_container;
	    
	    statistics_container m_statistics;		// Ownership of the objects is elsewhere.
		static providers_container& ProvidersList()
		{
			// This is a one-off memory leak.
			// A global (g_MonitoringInterface) creates something that uses this
			// container.  When it is destructed, this object must still exist.
		    static providers_container *sm_providers = new providers_container;
			return *sm_providers;
		}
    };
}

#define STATCLASS( spc, sc, v ) class sc : public Util::TStatistic<spc > \
{ public: virtual std::wstring ProviderSpecificValue( spc *p ) { v; } } stat_##sc;\
friend class sc;

#define STAT( sc ) Register( &stat_##sc );

#else

// New code has no effect.

namespace Util {
    class CStatisticsInterface
    {
    public:
		typedef std::list<std::wstring> string_container;

		static std::wstring GetStatistics( const string_container &/*i_include*/, const string_container &/*i_exclude*/ ) { return _T(""); }

		static bool Match( const std::wstring& /*name*/, const std::wstring& /*pattern*/ ) { return false; }
    };

    class CStatisticsProvider
    {
	public:
		virtual ~CStatisticsProvider(){};
		void SetName( const std::wstring& /*i_name*/ ) {}
        const std::wstring& Name(){return m_Name;};
	private:
		std::wstring m_Name;
	};

#define STATCLASS( spc, sc, v )
#define STATVAL( spc, sc, v )
#define STAT( sc )
}

#endif

#endif

