/**
 * @file messagefactory.h
 * HFPD Message Factory.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPD_MESSAGEFACTORY_H
#define HFPD_MESSAGEFACTORY_H

class message;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Message Factory.
class messageFactory
{
    typedef messageFactory _Myt;

public:

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Message Factory Method.
    /**
     * @param[in] s  pointer to array of characters
     * @param[in] n  number of characters in array
     * @param[out] len  characters used to create message
     * @return  pointer to message or @c NULL on failure
     */
    static message *create( const char *s, size_t n, size_t *len );

private:

    // not implemented
    messageFactory();

    // not implemented
    messageFactory( const _Myt & );

    // not implemented
    _Myt & operator = ( const _Myt & );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPD_MESSAGEFACTORY_H

