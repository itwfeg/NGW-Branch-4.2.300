
#include "stringformatter.h"

#include <cassert>
#include <cctype>
#include <list>

///////////////////////////////////////////////////////////////////////////////////////////////////
stringFormatter::stringFormatter()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
stringFormatter::~stringFormatter()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string stringFormatter::format( const std::string &f ) const
{
    static const char PERCENT( -1 );

    std::string result( f );

    // replace all '%%' with temporary character
    size_t pos;

    while ( std::string::npos != (pos = result.find( "%%" )) )
        result.replace( pos, 2, 1, PERCENT );

    // parse format string
    substr_stack_t work( parseFormatString( result ) );

    while ( work.size() )
    {
        const substr_t w( work.top() );

        // retrieve format
        const std::string wf( result.substr( w.first, w.second ) );

        if ( 2 <= wf.length() )
        {
            bool isNumeric( false );

            // retrieve value
            const std::string wv( value( wf.back(), isNumeric ) );

            // retrieve and emplace formatted value
            result.replace( w.first, w.second, formatValue( wf, wv, isNumeric ) );
        }

        work.pop();
    }

    // replace all temporary characters
    while ( std::string::npos != (pos = result.find( PERCENT )) )
        result.replace( pos, 1, 1, '%' );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
stringFormatter::substr_stack_t stringFormatter::parseFormatString( const std::string &f ) const
{
    substr_stack_t result;

    bool evaluate( false );
    size_t pos;

    // parse string
    for ( size_t i( 0 ); i < f.length(); ++i )
    {
        const char c( f[i] );

        if ( !evaluate )
        {
            if ( '%' == c )
            {
                evaluate = true;
                pos = i;
            }
        }
        else if ( std::isalpha( c ) )
        {
            evaluate = false;
            result.push( substr_t( pos, (i - pos) + 1 ) );
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string stringFormatter::formatValue( const std::string &f, const std::string &v, bool isNumeric ) const
{
    bool leftAlign( false );
    bool usePrecision( false );
    bool useZeroPadding( false );

    unsigned int width( 0 );
    unsigned int precision( 0 );

    for ( std::string::const_iterator i( f.begin() ); i != f.end(); ++i )
    {
        const char c( *i );

        if ( '-' == c )
            leftAlign = true;
        else if (( '0' == c ) && ( !width ) && ( !usePrecision ))
            useZeroPadding = true;
        else if ( '.' == c )
            usePrecision = true;
        else if ( std::isdigit( c ) )
        {
            if ( usePrecision )
            {
                precision *= 10;
                precision += c - '0';
            }
            else
            {
                width *= 10;
                width += c - '0';
            }
        }
    }

    // ---- //

    std::string result( v );

    if ( usePrecision )
    {
        if ( isNumeric )
        {
            while ( result.length() < precision )
                result.insert( 0, 1, '0' );

            // special case, precision of 0 for the value 0 gives empty string
            if (( !precision ) && ( "0" == result ))
                result.clear();
        }
        else
        {
            while ( precision < result.length() )
                result.pop_back();
        }
    }

    while ( result.length() < width )
    {
        if ( leftAlign )
            result.push_back( ' ' );
        else if (( useZeroPadding ) && (( !isNumeric ) || ( !usePrecision )))
            result.insert( 0, 1, '0' );
        else
            result.insert( 0, 1, ' ' );
    }

    return result;
}

// ================================================================================================
// Validation Methods
// ================================================================================================

class myStringFormatter : public stringFormatter
{
public:
    myStringFormatter() : stringFormatter() {}
protected:
    std::string value( char type, bool &isNumeric ) const {
        static int i( 0 );

        isNumeric = ('d' == type);

        switch ( i++ % 4 )
        {
        case 0: return "0";
        case 1: return "42";
        case 2: return "6789";
        case 3: return "123456";
        default: break;
        }

        return "";
    }

};

void test_i()
{
    const int i_zero( 0 );
    const int i_small( 42 );
    const int i_med( 6789 );
    const int i_large( 123456 );

    std::list<std::string> i_list;
    i_list.push_back( "%d" );
    i_list.push_back( "%3d" );
    i_list.push_back( "%7d" );
    i_list.push_back( "%-d" );
    i_list.push_back( "%-3d" );
    i_list.push_back( "%-7d" );
    i_list.push_back( "%0d" );
    i_list.push_back( "%03d" );
    i_list.push_back( "%07d" );
    i_list.push_back( "%-0d" );
    i_list.push_back( "%-03d" );
    i_list.push_back( "%-07d" );
    i_list.push_back( "%3.0d" );
    i_list.push_back( "%7.0d" );
    i_list.push_back( "%3.3d" );
    i_list.push_back( "%7.3d" );
    i_list.push_back( "%3.7d" );
    i_list.push_back( "%7.7d" );
    i_list.push_back( "%-3.0d" );
    i_list.push_back( "%-7.0d" );
    i_list.push_back( "%-3.3d" );
    i_list.push_back( "%-7.3d" );
    i_list.push_back( "%-3.7d" );
    i_list.push_back( "%-7.7d" );
    i_list.push_back( "%03.0d" );
    i_list.push_back( "%07.0d" );
    i_list.push_back( "%03.3d" );
    i_list.push_back( "%07.3d" );
    i_list.push_back( "%03.7d" );
    i_list.push_back( "%07.7d" );
    i_list.push_back( "%-03.0d" );
    i_list.push_back( "%-07.0d" );
    i_list.push_back( "%-03.3d" );
    i_list.push_back( "%-07.3d" );
    i_list.push_back( "%-03.7d" );
    i_list.push_back( "%-07.7d" );
    i_list.push_back( "%.0d" );
    i_list.push_back( "%.3d" );
    i_list.push_back( "%.7d" );
    i_list.push_back( "%-.0d" );
    i_list.push_back( "%-.3d" );
    i_list.push_back( "%-.7d" );
    i_list.push_back( "%0.0d" );
    i_list.push_back( "%0.3d" );
    i_list.push_back( "%0.7d" );
    i_list.push_back( "%-0.0d" );
    i_list.push_back( "%-0.3d" );
    i_list.push_back( "%-0.7d" );

    for ( std::list<std::string>::const_iterator i( i_list.begin() ); i != i_list.end(); ++i )
    {
        const myStringFormatter formatter;
        char value[128];

        std::string fmt;
        fmt.append( "    '" );
        fmt.append( (*i) );
        fmt.append( "'\n" );

        //printf( "format '%s'\n", i->c_str() );

        //printf( fmt.c_str(), i_zero );
        sprintf( value, fmt.c_str(), i_zero );
        assert( value == formatter.format( fmt ) );

        //printf( fmt.c_str(), i_small );
        sprintf( value, fmt.c_str(), i_small );
        assert( value == formatter.format( fmt ) );

        //printf( fmt.c_str(), i_med );
        sprintf( value, fmt.c_str(), i_med );
        assert( value == formatter.format( fmt ) );

        //printf( fmt.c_str(), i_large );
        sprintf( value, fmt.c_str(), i_large );
        assert( value == formatter.format( fmt ) );
    }
}

void test_s()
{
    const char *s_zero( "0" );
    const char *s_small( "42" );
    const char *s_med( "6789" );
    const char *s_large( "123456" );

    std::list<std::string> s_list;
    s_list.push_back( "%s" );
    s_list.push_back( "%3s" );
    s_list.push_back( "%7s" );
    s_list.push_back( "%-s" );
    s_list.push_back( "%-3s" );
    s_list.push_back( "%-7s" );
    s_list.push_back( "%0s" );
    s_list.push_back( "%03s" );
    s_list.push_back( "%07s" );
    s_list.push_back( "%-0s" );
    s_list.push_back( "%-03s" );
    s_list.push_back( "%-07s" );
    s_list.push_back( "%3.0s" );
    s_list.push_back( "%7.0s" );
    s_list.push_back( "%3.3s" );
    s_list.push_back( "%7.3s" );
    s_list.push_back( "%3.7s" );
    s_list.push_back( "%7.7s" );
    s_list.push_back( "%-3.0s" );
    s_list.push_back( "%-7.0s" );
    s_list.push_back( "%-3.3s" );
    s_list.push_back( "%-7.3s" );
    s_list.push_back( "%-3.7s" );
    s_list.push_back( "%-7.7s" );
    s_list.push_back( "%03.0s" );
    s_list.push_back( "%07.0s" );
    s_list.push_back( "%03.3s" );
    s_list.push_back( "%07.3s" );
    s_list.push_back( "%03.7s" );
    s_list.push_back( "%07.7s" );
    s_list.push_back( "%-03.0s" );
    s_list.push_back( "%-07.0s" );
    s_list.push_back( "%-03.3s" );
    s_list.push_back( "%-07.3s" );
    s_list.push_back( "%-03.7s" );
    s_list.push_back( "%-07.7s" );
    s_list.push_back( "%.0s" );
    s_list.push_back( "%.3s" );
    s_list.push_back( "%.7s" );
    s_list.push_back( "%-.0s" );
    s_list.push_back( "%-.3s" );
    s_list.push_back( "%-.7s" );
    s_list.push_back( "%0.0s" );
    s_list.push_back( "%0.3s" );
    s_list.push_back( "%0.7s" );
    s_list.push_back( "%-0.0s" );
    s_list.push_back( "%-0.3s" );
    s_list.push_back( "%-0.7s" );

    for ( std::list<std::string>::const_iterator i( s_list.begin() ); i != s_list.end(); ++i )
    {
        const myStringFormatter formatter;
        char value[128];

        std::string fmt;
        fmt.append( "    '" );
        fmt.append( (*i) );
        fmt.append( "'\n" );

        //printf( "format '%s'\n", i->c_str() );

        //printf( fmt.c_str(), s_zero );
        sprintf( value, fmt.c_str(), s_zero );
        assert( value == formatter.format( fmt ) );

        //printf( fmt.c_str(), s_small );
        sprintf( value, fmt.c_str(), s_small );
        assert( value == formatter.format( fmt ) );

        //printf( fmt.c_str(), s_med );
        sprintf( value, fmt.c_str(), s_med );
        assert( value == formatter.format( fmt ) );

        //printf( fmt.c_str(), s_large );
        sprintf( value, fmt.c_str(), s_large );
        assert( value == formatter.format( fmt ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void stringFormatter::validate()
{
    test_i();
    test_s();

    const myStringFormatter formatter;
    const std::string f( "\t\t  &%%%07d \n%%\n%03.7s=() %-0.7s%.d%%%3.3s%-07.7d\"%%^%-7.3d\n\n\t%-08.3s" );

    char value[128];
    sprintf( value, f.c_str(), 123456, "6789", "42", 0, "123456", 6789, 42, "0" );
    assert( value == formatter.format( f ) );
}

