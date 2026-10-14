/**
 * @file mapdataobjectcontainer.h
 * Map container class for Hobart data objects.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup labelViewerCsu
 * @{
 */

#ifndef MAPDATAOBJECTCONTAINER_H
#define MAPDATAOBJECTCONTAINER_H

#include "dataobjectcontainer.h"
#include "dataobjecttraits.h"

#include <QMap>
#include <QReadWriteLock>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class mapDataObjectContainer
 * @brief Data object container implementation as a map.
 *
 * @tparam T  data object class
 */
template <class T>
class mapDataObjectContainer : public dataObjectContainer<T>
{
    typedef mapDataObjectContainer<T> _Myt;
    typedef dataObjectContainer<T> _Mybase;

public:

    /**
     * Key type.
     *
     * This is the data object key.
     */
    typedef typename _Mybase::key_type key_type;

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     */
    mapDataObjectContainer() : _Mybase() {}

    /**
     * Constructor.
     *
     * @param[in] rhs  object to copy
     */
    mapDataObjectContainer( const _Myt& rhs ) : _Mybase() {copy( rhs );}

    /**
     * Destructor.
     */
    virtual ~mapDataObjectContainer() {}

    ///////////////////////////////////////////////////////////////////////////
    // Operators

    /**
     * Assignment operator.
     *
     * @param[in] rhs  object to copy
     * @returns  reference to this
     */
    _Myt& operator = ( const _Myt& rhs );

protected:

    /**
     * Key list type.
     *
     * List of data object keys.
     */
    typedef typename _Mybase::key_list_type key_list_type;

    /**
     * Value list type.
     *
     * List of data objects.
     */
    typedef typename _Mybase::value_list_type value_list_type;

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Reads all keys from container.
     *
     * @returns  list of keys
     */
    virtual key_list_type readAllKeys() const;

    /**
     * Read data from container.
     *
     * @param[in] keys  list of key values to read
     * @returns  list of values
     */
    virtual value_list_type readData( const key_list_type& keys ) const;

    /**
     * Reads all data from container.
     *
     * @returns  list of values
     */
    virtual value_list_type readAllData() const;

    /**
     * Write data to container.
     *
     * @param[in] values  values to write
     */
    virtual void writeData( const value_list_type& values );

    /**
     * Delete data from container.
     *
     * @param[in] keys  list of key values to delete
     */
    virtual void deleteData( const key_list_type& keys );

    /**
     * Delete all data from container.
     */
    virtual void deleteAllData();

private:

    typedef QMap<key_type, T> map_type;

    map_type data_;
    QReadWriteLock mutex_;

    /**
     * Copy object.
     */
    void copy( const _Myt& rhs );

};

template <class T>
typename mapDataObjectContainer<T>::_Myt& mapDataObjectContainer<T>::operator = ( const _Myt& rhs )
{
    // copy the object
    copy( rhs );

    // emit changed signal
    Q_EMIT _Mybase::changed();

    return *this;
}

template <class T>
typename mapDataObjectContainer<T>::key_list_type mapDataObjectContainer<T>::readAllKeys() const
{
    // lock container
    QReadLocker lock( const_cast<QReadWriteLock*>( &mutex_ ) );

    // return key list
    return data_.keys();
}

template <class T>
typename mapDataObjectContainer<T>::value_list_type mapDataObjectContainer<T>::readData( const key_list_type& keys ) const
{
    typedef typename key_list_type::const_iterator key_list_const_iterator;

    value_list_type list;

    // lock container
    QReadLocker lock( const_cast<QReadWriteLock*>( &mutex_ ) );

    // iterate through keys, copy to list if found
    for ( key_list_const_iterator i = keys.constBegin(); i != keys.constEnd(); ++i )
        if ( data_.contains( *i ) )
            list.push_back( data_[ *i ] );

    return list;
}

template <class T>
typename mapDataObjectContainer<T>::value_list_type mapDataObjectContainer<T>::readAllData() const
{
    typedef typename map_type::const_iterator map_const_iterator;

    value_list_type list;

    // lock container
    QReadLocker lock( const_cast<QReadWriteLock*>( &mutex_ ) );

    // copy object to list
    for ( map_const_iterator i = data_.constBegin(); i != data_.constEnd(); ++i )
        list.push_back( i.value() );

    return list;
}

template <class T>
void mapDataObjectContainer<T>::writeData( const value_list_type& values )
{
    typedef typename value_list_type::const_iterator value_list_const_iterator;

    // lock container
    QWriteLocker lock( &mutex_ );

    bool shouldEmit = false;

    // update all values
    for ( value_list_const_iterator i = values.constBegin(); i != values.constEnd(); ++i )
    {
        if ( !data_.contains( i->objectKey() ) )
            shouldEmit = true;

        data_[ i->objectKey() ] = (*i);
    }

    // unlock before emitting signal
    lock.unlock();

    // emit changed signal
    if ( shouldEmit )
    {
        Q_EMIT _Mybase::changed();
    }
}

template <class T>
void mapDataObjectContainer<T>::deleteData( const key_list_type& keys )
{
    typedef typename key_list_type::const_iterator key_list_const_iterator;

    // lock container
    QWriteLocker lock( &mutex_ );

    bool shouldEmit = false;

    // iterate all keys, if found remove it
    for ( key_list_const_iterator i = keys.constBegin(); i != keys.constEnd(); ++i )
        if ( data_.contains( *i ) )
        {
            data_.remove( *i );
            shouldEmit = true;
        }

    // unlock before emitting signal
    lock.unlock();

    // emit changed signal
    if ( shouldEmit )
    {
        Q_EMIT _Mybase::changed();
    }
}

template <class T>
void mapDataObjectContainer<T>::deleteAllData()
{
    // lock container
    QWriteLocker lock( &mutex_ );

    if ( !data_.empty() )
    {
        // clear all data
        data_.clear();

        // unlock before emitting signal
        lock.unlock();

        // emit changed signal
        Q_EMIT _Mybase::changed();
    }
}

template <class T>
void mapDataObjectContainer<T>::copy( const _Myt& rhs )
{
    // lock containers
    QWriteLocker lock( &mutex_ );
    QReadLocker rhsLock( &rhs.mutex_ );

    // copy the data
    data_ = rhs.data_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // MAPDATAOBJECTCONTAINER_H
