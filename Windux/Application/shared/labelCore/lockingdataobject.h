/**
 * @file lockingdataobject.h
 * Data object with read/write locking.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup labelViewerCsu
 * @{
 */

#ifndef LOCKINGDATAOBJECT_H
#define LOCKINGDATAOBJECT_H

#include <QReadWriteLock>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class lockingDataObject
 * @brief Data object wrapper class with read/write locking.
 *
 * Many databases offer record locking. This class is an attempt at offering record locking to
 * many of the container class implementations found within this library.
 *
 * @tparam T  data object class
 */
template <class T>
class lockingDataObject : public QReadWriteLock
{
    typedef lockingDataObject<T> _Myt;
    typedef QReadWriteLock _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     */
    lockingDataObject() : _Mybase(), dirtyFlag_( false ) {}

    /**
     * Constructor.
     *
     * @param[in] data  data object to copy
     */
    lockingDataObject( const T& data ) : _Mybase(), data_( data ), dirtyFlag_( false ) {}

    /**
     * Constructor.
     *
     * @param[in] rhs  object to copy
     */
    lockingDataObject( const _Myt& rhs ) : _Mybase(), data_( rhs.data() ), dirtyFlag_( false ) {}

    /**
     * Destructor.
     */
    virtual ~lockingDataObject() {}

    ///////////////////////////////////////////////////////////////////////////
    // Operators

    /**
     * Assignment operator.
     *
     * @param[in] data  data object to copy
     * @returns  reference to this
     */
    _Myt& operator = ( const T& data ) {setData( data );}

    /**
     * Assignment operator.
     *
     * @param[in] rhs  object to copy
     * @returns  reference to this
     */
    _Myt& operator = ( const _Myt& rhs ) {setData( rhs );}

    /**
     * Retrieve reference to data object.
     *
     * @warning
     * If you use this property you need to manage the lock manually! You can get into a lot of
     * trouble if you do not properly manage locking and unlocking... deadlocks and possible
     * crashes will occur.
     *
     * Example usage:
     * @code
     *   lockingDataObject<myClass> obj;
     *   obj.lockForWrite();
     *   myClass copy = obj;
     *   obj.unlock();
     * @endcode
     *
     * @returns  reference to value
     */
    T& operator * () {return data_;}

    /**
     * Retrieve pointer to data object.
     *
     * @warning
     * If you use this property you need to manage the lock manually! You can get into a lot of
     * trouble if you do not properly manage locking and unlocking... deadlocks and possible
     * crashes will occur.
     *
     * Example usage:
     * @code
     *   lockingDataObject<myClass> obj;
     *   obj.lockForWrite();
     *   obj->setSomeProperty( 42 );
     *   obj.unlock();
     * @endcode
     *
     * @returns  pointer to value
     */
    T *operator -> () {return &data_;}

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve object data.
     *
     * This property read locks before returning the value.
     *
     * @returns  object data
     */
    virtual const T data() const {
        QReadLocker lock( const_cast<_Myt*>( this ) );
        return T( data_ );
    }

    /**
     * Set object data.
     *
     * This property write locks before setting the value.
     *
     * @param[in] value  object value
     */
    virtual void setData( const T& value ) {
        QWriteLocker lock( this );
        data_ = value;
    }

    /**
     * Set object data.
     *
     * This property write locks before setting the value.
     *
     * @param[in] value  object value
     */
    virtual void setData( const _Myt& value ) {
        setData( value.data() );
    }

    /**
     * Check if object is dirty (i.e. has been written to).
     *
     * @returns  @c true if object is dirty, @c false otherwise.
     */
    virtual bool dirtyFlag() const {
        return dirtyFlag_;
    }

    /**
     * Set the dirty flag.
     *
     * @param[in] value  dirty flag
     */
    virtual void setDirtyFlag( bool value ) {
        dirtyFlag_ = value;
    }

private:

    T data_;

    bool dirtyFlag_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // LOCKINGDATAOBJECT_H
