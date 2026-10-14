#ifndef HOBBARCODEFORMATTER_H
#define HOBBARCODEFORMATTER_H

#include "stringformatter.h"

#include "HobString.h"

#include <list>
#include <map>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Hobart BarCode Formatter.
class HobBarCodeFormatter : public stringFormatter
{
    typedef HobBarCodeFormatter _Myt;
    typedef stringFormatter _Mybase;

public:

    /// Mapping of format string values.
	typedef std::map<char, HobString> formatMapping;

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
	/**
	 * @param[in] mapping  format mapping
	 */
    HobBarCodeFormatter( const formatMapping &mapping );

    /// Destructor.
    virtual ~HobBarCodeFormatter();

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
    virtual std::string value( char type, bool &isNumeric ) const;

private:

    std::list<char> numericValues_;

    formatMapping mapping_;

    // not implemented
    HobBarCodeFormatter( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOBBARCODEFORMATTER_H
