#ifndef STRINGFORMATTER_H
#define STRINGFORMATTER_H

#include <map>
#include <stack>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// String formatter.
class stringFormatter
{
    typedef stringFormatter _Myt;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    stringFormatter();

    /// Destructor.
    virtual ~stringFormatter();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Format string.
    /**
     * @param[in] f  string to format
     * @return  formatted string
     */
    virtual std::string format( const std::string &f ) const;

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Validation routines.
    static void validate();

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve value from formatted string.
    /**
     * @param[in] type  value type
     * @param[out] isNumeric  @c true if @p type is numeric, @c false otherwise
     * @return  value
     */
    virtual std::string value( char type, bool &isNumeric ) const = 0;

private:

    typedef std::pair<size_t, size_t> substr_t;
    typedef std::stack<substr_t> substr_stack_t;

    /// Parse format string into substrings.
    substr_stack_t parseFormatString( const std::string &f ) const;

    /// Format value accoring to rules.
    std::string formatValue( const std::string &f, const std::string &v, bool isNumeric ) const;

    // not implemented
    stringFormatter( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // STRINGFORMATTER_H
