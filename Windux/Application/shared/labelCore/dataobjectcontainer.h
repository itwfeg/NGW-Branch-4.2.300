/**
 * @file dataobjectcontainer.h
 * Base class for all Hobart data object containers.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
* @addtogroup labelViewerCsu
 * @{
 */

#ifndef DATAOBJECTCONTAINER_H
#define DATAOBJECTCONTAINER_H

#include "dataobjecttraits.h"

#include <QObject>

class tagDataRecord;
class tagDataStream;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class dataObjectContainerBase
 * @brief Base class for all data object containers.
 */
class dataObjectContainerBase : public QObject
{
    Q_OBJECT

    typedef dataObjectContainerBase _Myt;
    typedef QObject _Mybase;

Q_SIGNALS:

    /**
     * This signal is emitted whenever the container data changes.
     *
     * @note
     * It is only emitted when the container changes, not if an individual object changes. This
     * means you will know when data is added or removed, but not when object 42's property
     * changes.
     */
    void changed();

public:

    ///////////////////////////////////////////////////////////////////////////
    // DTOR

    /**
     * Destructor.
     */
    virtual ~dataObjectContainerBase();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Check if container is empty.
     *
     * @returns  @c true if the list is empty, @c false otherwise
     */
    virtual bool empty() const;

    /**
     * Retrieve number of items in container.
     *
     * @returns  number of items in the container
     */
    virtual int size() const = 0;

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Removes all items from the container.
     */
    virtual void clear() = 0;

    /**
     * Process a tag data command.
     *
     * This method provides generic access to the container via tag data commands.
     *
     * Example usage:
     * @code
     *   // assuming you had a container class myClassContainer of myClass objects...
     *   myClassContainer c;
     *   tagDataRecord r;
     *   s >> r; // get record from tag data stream
     *   bool result = c.processTagDataRecord( r, s );
     * @endcode
     *
     * @param[in] command  command to process in tag data format
     * @param[out] os  output stream
     * @returns  @c true if command was processed, @c false otherwise
     */
    virtual bool processTagDataRecord( const tagDataRecord& command, tagDataStream& os );

protected:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR

    /**
     * Constructor.
     */
    dataObjectContainerBase();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class dataObjectContainer
 * @brief Data object container implementation and base class.
 *
 * In order to derive from this generic container class you must implement the following methods:
 * @arg readData()
 * @arg readAllData()
 * @arg writeData()
 * @arg deleteData()
 * @arg deleteAllData()
 *
 * Container implementation could be a list, a map, or perhaps a database.  Data may generically
 * written, read, deleted, etc. by use of the above functions.
 *
 * @tparam T  data object class
 */
template <class T>
class dataObjectContainer : public dataObjectContainerBase
{
    typedef dataObjectContainer<T> _Myt;
    typedef dataObjectContainerBase _Mybase;

public:

    /**
     * Key type.
     */
    typedef typename dataObjectTraits<T>::key_type key_type;

    /**
     * @class const_iterator
     * @brief STL style constant iterator for the container.
     *
     * Example usage:
     * @code
     *   dataObjectContainer<label> labels;
     *   for ( dataObjectContainer<label>::const_iterator i = labels.begin(); i != labels.end(); ++i ) {
     *       // do something with i...
     *   }
     * @endcode
     *
     * @warning
     * This iterator is very generic and slow. Each increment and decrement operation performs a
     * read of all the keys.
     */
    class const_iterator
    {
        typedef const_iterator _Myt;

        friend class dataObjectContainer<T>;

    public:

        ///////////////////////////////////////////////////////////////////////
        // CTOR / DTOR

        /// Constructor.
        const_iterator();

        /// Constructor.
        const_iterator( const _Myt& rhs );

        ///////////////////////////////////////////////////////////////////////
        // Properties

        /// Retrieve key value.
        const key_type key() const {return value_.objectKey();}

        /// Retrieve value.
        const T& value() const {return value_;}

        ///////////////////////////////////////////////////////////////////////
        // Operators

        /// Assignment operator.
        _Myt& operator = ( const _Myt& rhs ) {copy( rhs ); return *this;}

        /// Equality operator.
        bool operator == ( const _Myt& rhs ) const {return (0 == compare( rhs ));}

        /// Inequality operator.
        bool operator != ( const _Myt& rhs ) const {return (0 != compare( rhs ));}

        /// Less than operator.
        bool operator < ( const _Myt& rhs ) const {return (0 > compare( rhs ));}

        /// Less than equal to operator.
        bool operator <= ( const _Myt& rhs ) const {return (0 >= compare( rhs ));}

        /// Greater than equal to operator.
        bool operator >= ( const _Myt& rhs ) const {return (0 <= compare( rhs ));}

        /// Greater than operator.
        bool operator > ( const _Myt& rhs ) const {return (0 < compare( rhs ));}

        /// Indirection operator.
        const T& operator * () const {return value_;}

        /// Structure dereference operator.
        const T *operator -> () const {return &value_;}

        /// Pre-increment operator.
        _Myt& operator ++ ();

        /// Post-increment operator.
        _Myt operator ++ ( int );

        /// Pre-decrement operator.
        _Myt& operator -- ();

        /// Post-decrement operator.
        _Myt operator -- ( int );

    private:

        typedef dataObjectContainer<T> container_type;
        const container_type *container_;

        T value_;
        bool end_;

        ///////////////////////////////////////////////////////////////////////
        // CTOR / DTOR

        /// Constructor.
        const_iterator( const container_type *container );

        /// Constructor.
        const_iterator( const container_type *container, const T& value );

        ///////////////////////////////////////////////////////////////////////
        // Methods

        /// Copy object.
        void copy( const _Myt& rhs );

        /// Compare objects.
        int compare( const _Myt& rhs ) const;

    };

    ///////////////////////////////////////////////////////////////////////////
    // DTOR

    /**
     * Destructor.
     */
    virtual ~dataObjectContainer() {}

    ///////////////////////////////////////////////////////////////////////////
    // Operators

    /**
     * Retrieve the value associated with the key @p key.
     *
     * If the map contains no item with key @p key, the function returns a default-constructed
     * value instead.
     *
     * @param[in] key  key of value to retrieve
     * @returns  the value associated with the key @p key
     */
    const T operator [] ( const key_type& key ) const {return value( key );}

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve a STL-style constant iterator pointing to the first item in the container.
     *
     * @returns  constant iterator pointing to the first item in the container
     */
    virtual const_iterator begin() const;

    /**
     * Check if container contains key @p key.
     *
     * @param[in] key  key to check
     * @returns  @c true if the container contains an item with key @p key; otherwise returns false
     */
    virtual bool contains( const key_type& key ) const;

    /**
     * Retrieve an STL-style constant iterator pointing to the imaginary item after the last item in
     * the container.
     *
     * @returns  constant iterator
     */
    virtual const_iterator end() const;

    /**
     * Retrieve number of items in container.
     *
     * @returns  number of items in the container
     */
    virtual int size() const;

    /**
     * Retrieve the value associated with the key @p key.
     *
     * If the map contains no item with key @p key, the function returns a default-constructed
     * value instead.
     *
     * @param[in] key  key of value to retrieve
     * @returns  the value associated with the key @p key
     */
    virtual const T value( const key_type& key ) const;

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Removes all items from the container.
     */
    virtual void clear() {deleteAllData();}

    /**
     * Removes the (key, value) pair pointed to by the iterator @p pos from the container, and
     * returns an iterator to the next item in the container.
     *
     * @param[in] pos  iterator to remove
     * @returns  iterator to next item
     */
    virtual const_iterator erase( const_iterator pos  );

    /**
     * Inserts an item into the container.
     *
     * @param[in] value  item to insert
     * @returns  iterator to newly inserted item
     */
    virtual const_iterator insert( const T& value );

    /**
     * Process a tag data command.
     *
     * This method provides generic access to the container via tag data commands.
     *
     * Example usage:
     * @code
     *   // assuming you had a container class myClassContainer of myClass objects...
     *   myClassContainer c;
     *   tagDataRecord r;
     *   s >> r; // get record from tag data stream
     *   bool result = c.processTagDataRecord( r, s );
     * @endcode
     *
     * @param[in] command  command to process in tag data format
     * @param[out] os  output stream
     * @returns  @c true if command was processed, @c false otherwise
     */
    virtual bool processTagDataRecord( const tagDataRecord& command, tagDataStream& os );

protected:

    /**
     * Key list type.
     */
    typedef typename dataObjectTraits<T>::key_list_type key_list_type;

    /**
     * Value list type.
     */
    typedef typename dataObjectTraits<T>::value_list_type value_list_type;

    ///////////////////////////////////////////////////////////////////////////
    // CTOR

    /**
     * Constructor.
     */
    dataObjectContainer() : _Mybase() {}

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Reads all keys from container.
     *
     * @returns  list of keys
     */
    virtual key_list_type readAllKeys() const = 0;

    /**
     * Read data from container.
     *
     * @param[in] keys  list of key values to read
     * @returns  list of values
     */
    virtual value_list_type readData( const key_list_type& keys ) const = 0;

    /**
     * Reads all data from container.
     *
     * @returns  list of values
     */
    virtual value_list_type readAllData() const = 0;

    /**
     * Write data to container.
     *
     * @param[in] values  values to write
     */
    virtual void writeData( const value_list_type& values ) = 0;

    /**
     * Delete data from container.
     *
     * @param[in] keys  list of key values to delete
     */
    virtual void deleteData( const key_list_type& keys ) = 0;

    /**
     * Delete all data from container.
     */
    virtual void deleteAllData() = 0;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// dataObjectContainer::const_iterator Implementation
//

template <class T>
dataObjectContainer<T>::const_iterator::const_iterator() :
    container_( NULL ),
    end_( true )
{
}

template <class T>
dataObjectContainer<T>::const_iterator::const_iterator( const container_type *container ) :
    container_( container ),
    end_( true )
{
}

template <class T>
dataObjectContainer<T>::const_iterator::const_iterator( const container_type *container, const T& value ) :
    container_( container ),
    value_( value ),
    end_( false )
{
}

template <class T>
dataObjectContainer<T>::const_iterator::const_iterator( const _Myt& rhs )
{
    copy( rhs );
}

template <class T>
typename dataObjectContainer<T>::const_iterator& dataObjectContainer<T>::const_iterator::operator ++ ()
{
    // at end, nothing to do
    if ( end_ )
        return *this;

    // get the keys
    key_list_type keys( container_->readAllKeys() );

    // find this iterator in the list of keys
    int i = keys.indexOf( key() );

    // found, get next item
    if ( 0 <= i )
    {
        if ( ++i < keys.size() )
            value_ = container_->value( keys[i] );
        else
            end_ = true;

        return *this;
    }

    // not found, get the item after this one...
    for ( i = 0; i < keys.size(); ++i )
        if ( key() < keys[i] )
        {
            value_ = container_->value( keys[i] );
            return *this;
        }

    end_ = true;
    return *this;
}

template <class T>
typename dataObjectContainer<T>::const_iterator dataObjectContainer<T>::const_iterator::operator ++ ( int )
{
    _Myt i( *this );
    ++(*this);
    return i;
}

template <class T>
typename dataObjectContainer<T>::const_iterator& dataObjectContainer<T>::const_iterator::operator -- ()
{
    // no container, nothing to do
    if ( !container_ )
        return *this;

    // get the keys
    key_list_type keys( container_->readAllKeys() );

    // at end, just get last item
    if ( end_ )
    {
        if ( keys.size() )
        {
            value_ = container_->value( keys.last() );
            end_ = false;
        }

        return *this;
    }

    // find this iterator in the list of keys
    int i = keys.indexOf( key() );

    // found, get previous item
    if ( 0 < i )
    {
        value_ = container_->value( keys[--i] );
        return *this;
    }
    // found, but we are already at the beginning
    else if ( 0 == i )
    {
        return *this;
    }

    // save off key for comparison
    key_type k = key();

    // not found, get the item previous this one
    for ( i = 0; i < keys.size(); ++i )
    {
        if ( keys[i] < k )
            value_ = container_->value( keys[i] );
        else
            break;
    }

    return *this;
}

template <class T>
typename dataObjectContainer<T>::const_iterator dataObjectContainer<T>::const_iterator::operator -- ( int )
{
    _Myt i( *this );
    --(*this);
    return i;
}

template <class T>
void dataObjectContainer<T>::const_iterator::copy( const _Myt& rhs )
{
    container_ = rhs.container_;
    value_ = rhs.value_;
    end_ = rhs.end_;
}

template <class T>
int dataObjectContainer<T>::const_iterator::compare( const _Myt& rhs ) const
{
    if ( end_ )
        return (rhs.end_ ? 0 : 1);
    else if ( rhs.end_ )
        return -1;
    else if ( key() < rhs.key() )
        return -1;
    else if ( key() == rhs.key() )
        return 0;
    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// dataObjectContainer Implementation
//

template <class T>
typename dataObjectContainer<T>::const_iterator dataObjectContainer<T>::begin() const
{
    key_list_type keys( readAllKeys() );

    if ( keys.empty() )
        return end();

    return const_iterator( this, value( keys[0] ) );
}

template <class T>
bool dataObjectContainer<T>::contains( const key_type& key ) const
{
    key_list_type keys( readAllKeys() );

    return (0 <= keys.indexOf( key ));
}

template <class T>
typename dataObjectContainer<T>::const_iterator dataObjectContainer<T>::end() const
{
    return const_iterator( this );
}

template <class T>
int dataObjectContainer<T>::size() const
{
    return readAllKeys().size();
}

template <class T>
const T dataObjectContainer<T>::value( const key_type& key ) const
{
    key_list_type keys;

    // push the key onto the list
    keys.push_back( key );

    // read the data
    value_list_type values( readData( keys ) );

    // not found!
    if ( !values.size() )
        return T();

    // return the value
    return values.first();
}

template <class T>
typename dataObjectContainer<T>::const_iterator dataObjectContainer<T>::erase( const_iterator pos  )
{
    key_list_type keys;

    // push the key onto the list
    keys.push_back( pos.key() );

    // get the next item before we delete...
    ++pos;

    // delete!
    deleteData( keys );

    return pos;
}

template <class T>
typename dataObjectContainer<T>::const_iterator dataObjectContainer<T>::insert( const T& value )
{
    value_list_type values;

    // push the value onto the list
    values.push_back( value );

    // write!
    writeData( values );

    // return an iterator to it
    return const_iterator( this, value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // DATAOBJECTCONTAINER_H
