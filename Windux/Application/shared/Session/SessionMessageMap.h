/**
 * @file SessionMessageMap.h
 * Capacity bar widget.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup thinClientMessagingCsu
 * @{
 */

#ifndef SESSIONMESSAGEMAP_H
#define SESSIONMESSAGEMAP_H

#include "Session.h"
#include "TokenParser.h"

#include <QDate>
#include <QString>
#include <QTextStream>
#include <QMap>

#include <QStringList>

extern int sessionDateStyle;

/**
 * List of integers.
 */
typedef QList<int> integerList;

/**
 * Vector of integers.
 */
typedef QVector<int> integerVector;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @macro DEFINE_FIELD
 * @brief This macro maps fields to types and defines accessors for them.
 */
#define DEFINE_FIELD( TYPE, GET, SET, RAW, NUMBER ) \
    TYPE GET() const {return SessionMessageMapBase::field<TYPE>( const_cast<_Myt*>( this )->RAW() );} \
    void SET( const TYPE& value ) {SessionMessageMapBase::setField( RAW(), value );} \
    QString& RAW() {return _Mybase::operator [] (NUMBER);}

#define DEFINE_RO_FIELD( NUMBER, TYPE, GET ) \
    TYPE GET() const {return SessionMessageMapBase::field<TYPE>( GET ## Raw() );} \
    QString& GET ## Raw () {return _Mybase::operator [] (NUMBER);} \
    const QString GET ## Raw () const {return _Mybase::operator [] (NUMBER);}

#define DEFINE_RW_FIELD( NUMBER, TYPE, GET, SET) \
    DEFINE_RO_FIELD( NUMBER, TYPE, GET ) \
    void SET( const TYPE& value ) {SessionMessageMapBase::setField( GET ## Raw(), value );}

#define BEGIN_SESSION_MESSAGE( NAME, DEVICE, EVENT ) \
    class NAME : public SessionMessageMap<DEVICE,EVENT> { \
        typedef NAME _Myt; \
        typedef SessionMessageMap<DEVICE,EVENT> _Mybase; \
    public: \
        NAME() : _Mybase() {} \
        NAME( const SessionMessage& rhs ) : _Mybase( rhs ) {} \
        NAME( const _Myt& rhs ) : _Mybase( rhs ) {} \
        virtual ~NAME() {}

#define END_SESSION_MESSAGE \
    };

/**
 * @class SessionMessageMapBase
 * @brief Session message wrapper base class.
 */
class SessionMessageMapBase : public QMap<int, QString>
{
    typedef SessionMessageMapBase _Myt;
    typedef QMap<int, QString> _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // DTOR

    /**
     * Destructor.
     */
    virtual ~SessionMessageMapBase() {}

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve field from message.
     *
     * Fields can be of any type.  For example:
     * @code
     *   int number = msg.field<int>( msg.sealerBeltTemp() );
     *   QString text = msg.field<QString>( msg.text() );
     *   float decimal = msg.field<float>( msg.weight() );
     * @endcode
     *
     * @param[in] value  field value
     * @returns  converted field value
     *
     * @tparam T  field type
     */
    template <class T>
    static T field( const QString& value );

    /**
     * Set field in message.
     *
     * Fields can be of any type.  For example:
     * @code
     *   int number = 42;
     *   msg.setField( msg.sealerBeltTemp(), number );
     *   QString text = "example";
     *   msg.setField( msg.text(), text );
     *   msg.setField( msg.weight(), 123.45f );
     * @endcode
     *
     * @param[in] f  field to set
     * @param[in] value  field value
     *
     * @tparam T  field type
     */
    template <class T>
    static void setField( QString& f, const T& value );

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Decorate string in preparation for sending as csv string.
     *
     * @param[in] value  string to decorate
     * @returns  decorated string
     */
    static QString decorate( const QString& value );

protected:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR

    /**
     * Constructor.
     */
    SessionMessageMapBase() : _Mybase() {}

};

template <class T>
T SessionMessageMapBase::field( const QString& value )
{
    QString temp = value;

    T f;
    QTextStream( &temp ) >> f;

    return f;
}

template <class T>
void SessionMessageMapBase::setField( QString& f, const T& value )
{
    QString field;
    QTextStream( &field ) << value;

    f = field;
}

inline QString SessionMessageMapBase::decorate( const QString& value )
{
    QString result = value;

    // change all " to ""
    result.replace( "\"", "\"\"" );

    return ( "\"" + result + "\"" );
}

/**
 * Specialization for string fields since there is nothing to do.
 */
template <>
inline QString SessionMessageMapBase::field<QString>( const QString& value )
{
    return value;
}

/**
 * Specialization for bool fields.
 */
template <>
inline bool SessionMessageMapBase::field<bool>( const QString& value )
{
    return (0 == value.toInt() ? false : true);
}

/**
 * Specialization for int fields.
 */
template <>
inline int SessionMessageMapBase::field<int>( const QString& value )
{
    return value.toInt();
}

/**
 * Specialization for a list of string fields.
 */
template <>
inline QStringList SessionMessageMapBase::field<QStringList>( const QString& value )
{
    QStringList result;

    // tokenize
    QVector<QString> tokens( TokenParser::splitStringTokens( value ) );

    // populate list
    foreach ( const QString& t, tokens )
        result.push_back( t );

    // strip leading and trailing double quotes from each field
    for ( QStringList::iterator i = result.begin(); i != result.end(); ++i )
        if ( 2 <= i->length() )
            if (( '\"' == i->at( 0 ) ) && ( '\"' == i->at( i->length() - 1 ) ))
                *i = i->mid( 1, i->length() - 2 );

    return result;
}

/**
 * Specialization for a list of int fields.
 */
template <>
inline integerList SessionMessageMapBase::field<integerList>( const QString& value )
{
    integerList result;

    // strip leading and trailing double quote
    QString s( value );

    if ( 2 <= s.length() )
        if (( '\"' == s[0] ) && ( '\"' == s[s.length() - 1] ))
            s = s.mid( 1, s.length() - 2 );

    // convert each string to an int
    const QStringList temp( s.split( "," ) );

    foreach ( const QString &t, temp )
        result.push_back( t.toInt() );

    return result;
}

/**
 * Specialization for a vector of int fields.
 */
template <>
inline integerVector SessionMessageMapBase::field<integerVector>( const QString& value )
{
    integerVector result;

    // strip leading and trailing double quote
    QString s( value );

    if ( 2 <= s.length() )
        if (( '\"' == s[0] ) && ( '\"' == s[s.length() - 1] ))
            s = s.mid( 1, s.length() - 2 );

    // convert each string to an int
    const QStringList temp( s.split( "," ) );

    foreach ( const QString &t, temp )
        result.push_back( t.toInt() );

    return result;
}

/**
 * Specialization for QDate fields.
 */
template <>
inline QDate SessionMessageMapBase::field<QDate>( const QString& value )
{
    QDate result;
    QString date( value );

    // retrieve locale
    QLocale locale;

    // fix short date
    // 0 = "FE 28, 13"
    // 6 = "13 FE 28"
    if (( 0 == sessionDateStyle ) || ( 6 == sessionDateStyle ))
    {
        QString oldName;
        QString newName;

        // replace abbreviated month name with month name
        if ( date.contains( (oldName = "JA") ) )
            newName = locale.monthName( 1, QLocale::ShortFormat );
        else if ( date.contains( (oldName = "FE") ) )
            newName = locale.monthName( 2, QLocale::ShortFormat );
        else if ( date.contains( (oldName = "MR") ) )
            newName = locale.monthName( 3, QLocale::ShortFormat );
        else if ( date.contains( (oldName = "AL") ) )
            newName = locale.monthName( 4, QLocale::ShortFormat );
        else if ( date.contains( (oldName = "MA") ) )
            newName = locale.monthName( 5, QLocale::ShortFormat );
        else if ( date.contains( (oldName = "JN") ) )
            newName = locale.monthName( 6, QLocale::ShortFormat );
        else if ( date.contains( (oldName = "JL") ) )
            newName = locale.monthName( 7, QLocale::ShortFormat );
        else if ( date.contains( (oldName = "AU") ) )
            newName = locale.monthName( 8, QLocale::ShortFormat );
        else if ( date.contains( (oldName = "SE") ) )
            newName = locale.monthName( 9, QLocale::ShortFormat );
        else if ( date.contains( (oldName = "OC") ) )
            newName = locale.monthName( 10, QLocale::ShortFormat );
        else if ( date.contains( (oldName = "NO") ) )
            newName = locale.monthName( 11, QLocale::ShortFormat );
        else if ( date.contains( (oldName = "DE") ) )
            newName = locale.monthName( 12, QLocale::ShortFormat );

        if ( newName.length() )
            date.replace( oldName, newName );
    }

    // handle date format
    switch ( sessionDateStyle )
    {
    // "FE 28, 13"
    case 0:
        // drop into next case

    // "Feb 28, 13"
    case 1:
        result = locale.toDate( date, "MMM dd, yy" );
        break;

    // "13/02/28"
    case 2:
        result = locale.toDate( date, "yy/MM/dd" );
        break;

    // "02/28/13"
    case 3:
        result = locale.toDate( date, "MM/dd/yy" );
        break;

    // "28/Feb/13"
    case 4:
        result = locale.toDate( date, "dd/MMM/yy" );
        break;

    // "28/02/13"
    case 5:
        result = locale.toDate( date, "dd/MM/yy" );
        break;

    // "13 FE 28"
    case 6:
        result = locale.toDate( date, "yy MMM dd" );
        break;

    // "28 FEB 13"
    case 7:
        result = locale.toDate( date, "dd MMM yy" );
        break;

    default:
        result = QDate::fromString( value, "MMM dd, yy" );
        break;
    }

    // fix year
    if ( result.year() < 1970 )
    {
        // Note:
        // Since the maximum amount I can adjust a date by is 999 days this code should
        // theoretically be okay until March of 2067.
        result = result.addYears( 100 );
    }

    return result;
}

/**
 * Specialization for string fields since they need to be decorated.
 */
template <>
inline void SessionMessageMapBase::setField<QString>( QString& f, const QString& value )
{
    f = decorate( value );
}

/**
 * Specialization for bool fields.
 */
template <>
inline void SessionMessageMapBase::setField<bool>( QString& f, const bool& value )
{
    f = (value ? "1" : "0");
}

/**
 * Specialization for int fields.
 */
template <>
inline void SessionMessageMapBase::setField<int>( QString& f, const int& value )
{
    f = QString::number( value );
}

/**
 * Specialization for a list of int fields.
 */
template <>
inline void SessionMessageMapBase::setField<integerList>( QString& f, const integerList& value )
{
    QString list;

    foreach ( int i, value )
    {
        if ( list.length() )
            list.append( "," );

        list.append( QString::number( i ) );
    }

    f = ("\"" + list + "\"");
}

/**
 * Specialization for a vector of int fields.
 */
template <>
inline void SessionMessageMapBase::setField<integerVector>( QString& f, const integerVector& value )
{
    QString list;

    foreach ( int i, value )
    {
        if ( list.length() )
            list.append( "," );

        list.append( QString::number( i ) );
    }

    f = ("\"" + list + "\"");
}

/**
 * Specialization for date fields.
 */
template <>
inline void SessionMessageMapBase::setField<QDate>( QString& f, const QDate& value )
{
    bool fixShortMonth = false;

    // retrieve locale
    QLocale locale;

    // handle date format
    switch ( sessionDateStyle )
    {
    // "FE 28, 13"
    case 0:
        fixShortMonth = true;
        // drop into next case

    // "Feb 28, 13"
    case 1:
        f = locale.toString( value, "MMM dd, yy" );
        break;

    // "13/02/28"
    case 2:
        f = locale.toString( value, "yy/MM/dd" );
        break;

    // "02/28/13"
    case 3:
        f = locale.toString( value, "MM/dd/yy" );
        break;

    // "28/Feb/13"
    case 4:
        f = locale.toString( value, "dd/MMM/yy" );
        break;

    // "28/02/13"
    case 5:
        f = locale.toString( value, "dd/MM/yy" );
        break;

    // "13 FE 28"
    case 6:
        fixShortMonth = true;
        f = locale.toString( value, "yy MMM dd" );
        break;

    // "28 FEB 13"
    case 7:
        f = locale.toString( value, "dd MMM yy" );
        break;

    default:
        f = value.toString( "MMM dd, yy" );
        break;
    }

    // replace month name with abbreviated month name
    if ( fixShortMonth )
    {
        QString oldName( locale.monthName( value.month(), QLocale::ShortFormat ) );

        if ( f.contains( oldName ) )
        {
            QString newName;

            // the abbreviated months are only in english locale
            switch ( value.month() )
            {
            case 1: newName = "JA"; break;
            case 2: newName = "FE"; break;
            case 3: newName = "MR"; break;
            case 4: newName = "AL"; break;
            case 5: newName = "MA"; break;
            case 6: newName = "JN"; break;
            case 7: newName = "JL"; break;
            case 8: newName = "AU"; break;
            case 9: newName = "SE"; break;
            case 10: newName = "OC"; break;
            case 11: newName = "NO"; break;
            case 12: newName = "DE"; break;
            default:
                break;
            }

            if ( newName.length() )
                f.replace( oldName, newName );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class SessionMessageMap
 * @brief Session message wrapper class to aid in parsing fields.
 *
 * @tparam _Device  device type
 * @tparam _Event  event type
 */
template <int _Device, int _Event>
class SessionMessageMap : public SessionMessageMapBase
{
    typedef SessionMessageMap<_Device, _Event> _Myt;
    typedef SessionMessageMapBase _Mybase;

public:

    /**
     * Device type.
     */
    enum {DEVICE = _Device};

    /**
     * Event type.
     */
    enum {EVENT = _Event};

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     */
    SessionMessageMap();

    /**
     * Constructor.
     *
     * @param[in] rhs  session message to copy
     */
    SessionMessageMap( const SessionMessage& rhs );

    /**
     * Constructor.
     *
     * @param[in] rhs  map to copy
     */
    SessionMessageMap( const _Mybase& rhs );

    /**
     * Constructor.
     *
     * @param[in] rhs  object to copy
     */
    SessionMessageMap( const _Myt& rhs );

    /**
     * Destructor.
     */
    virtual ~SessionMessageMap();

    ///////////////////////////////////////////////////////////////////////////
    // Operators

    /**
     * Cast object to @c SessionMessage.
     */
    operator SessionMessage () const;

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve device field.
     */
    QString& device() {return _Mybase::operator [] (0);}

    /**
     * Retrieve event field.
     */
    QString& event() {return _Mybase::operator [] (1);}

};

template <int _Device, int _Event>
SessionMessageMap<_Device, _Event>::SessionMessageMap() :
    _Mybase()
{
    _Mybase::insert( 0, QString::number( DEVICE ) );
    _Mybase::insert( 1, QString::number( EVENT ) );
}

template <int _Device, int _Event>
SessionMessageMap<_Device, _Event>::SessionMessageMap( const SessionMessage& rhs ) :
    _Mybase()
{
    for ( int i = 0; i < rhs.size(); ++i )
        _Mybase::insert( i, rhs.data()[i] );
}

template <int _Device, int _Event>
SessionMessageMap<_Device, _Event>::SessionMessageMap( const _Mybase& rhs ) :
    _Mybase( rhs )
{
}

template <int _Device, int _Event>
SessionMessageMap<_Device, _Event>::SessionMessageMap( const _Myt& rhs ) :
    _Mybase( rhs )
{
}

template <int _Device, int _Event>
SessionMessageMap<_Device, _Event>::~SessionMessageMap()
{
}

template <int _Device, int _Event>
SessionMessageMap<_Device, _Event>::operator SessionMessage () const
{
    SessionMessage m;

    for ( const_iterator i = begin(); i != end(); ++i )
    {
        m.resize( i.key() + 1 );
        m[ i.key() ] = i.value();
    }

    return m;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SESSIONMESSAGEMAP_H
