/**
 * @file datastore.h
 * Storage of Hobart data object containers.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
* @addtogroup labelViewerCsu
 * @{
 */

#ifndef DATASTORE_H
#define DATASTORE_H

#include "dataobjecttype.h"

#include "dataobjectcontainer.h"
#include "dataobjectcontainerfactory.h"
#include "processtagdatacontainer.h"

#include <QObject>
#include <QScopedArrayPointer>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class dataStoreBase
 * @brief Base class for all data stores.
 */
class dataStoreBase : public QObject
{
    Q_OBJECT

    typedef dataStoreBase _Myt;

Q_SIGNALS:

    /**
     * This signal is issued when a record is processed.
     *
     * @param[in] t  data object type processed
     */
    void processedRecord( dataObjectType::type t );

    /**
     * This signal is issued when the end of a stream has been reached.
     *
     * For example, a tag data footer record.
     */
    void complete();

public:

    ///////////////////////////////////////////////////////////////////////////
    // DTOR

    /**
     * Destructor.
     */
    virtual ~dataStoreBase();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Check if store is empty.
     *
     * @returns  @c true if the store is empty, @c false otherwise
     */
    virtual bool empty() const;

    /**
     * Retrieve pointer to store.
     *
     * Example usage:
     * @code
     *   // retrieve store of labels
     *   dataObjectContainer<label> *ptr = myDataStore.store<label>();
     *   // retrieve store of graphics
     *   dataObjectContainer<graphic> *ptr = myDataStore.store<graphic>();
     * @endcode
     *
     * @returns  pointer to store
     *
     * @tparam T  data object class
     */
    template <class T>
    dataObjectContainer<T> *store() {
        return dynamic_cast<dataObjectContainer<T>*>( containers_[ T::DATA_OBJECT_TYPE ] );
    }

    /**
     * This is an overloaded function.
     */
    template <class T>
    const dataObjectContainer<T> *store() const {
        return const_cast<_Myt*>( this )->store<T>();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Removes all items from the store.
     */
    virtual void clear();

    /**
     * Process a tag data command.
     *
     * This method provides generic access to the store via tag data commands.
     *
     * @param[in] command  command to process in tag data format
     * @param[out] os  output stream
     * @returns  @c true if command was processed, @c false otherwise
     */
    virtual bool processTagDataRecord( const tagDataRecord& command, tagDataStream& os );

protected:

    /**
     * Container pointer.
     */
    typedef dataObjectContainerBase* containerPtr;

    /**
     * Container pointer array.
     */
    typedef QScopedArrayPointer<containerPtr> containerPtrArray;

    ///////////////////////////////////////////////////////////////////////////
    // CTOR

    /**
     * Constructor.
     */
    dataStoreBase();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve reference to containers array.
     *
     * @returns  reference to containers array
     */
    containerPtrArray& containers();

    /**
     * Retrieve reference to container.
     *
     * @param[in] i  index of container to retrieve
     * @returns  reference to container pointer
     */
    containerPtr& container( int i );

private:

    containerPtrArray containers_;

};

/**
 * Tag data processing operator.
 *
 * Processes a tag data stream on a data store.
 *
 * @param[in,out] lhs  input stream
 * @param[out] rhs  object to parse into
 * @returns  reference to @p lhs
 */
tagDataStream& operator >> ( tagDataStream& lhs, dataStoreBase& rhs );

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class dataStore
 * @brief Data store for Hobart data objects.
 *
 * Containers implementation can be performed in many different ways, that is why you must
 * specify the container type. Example usage:
 * @code
 *   // create a map container store
 *   dataStore<mapDataObjectContainer> mapStore;
 *   // create a list container store
 *   dataStore<listDataObjectContainer> listStore;
 *   // create a database container store
 *   dataStore<databaseDataObjectContainer> databaseStore;
 * @endcode
 *
 * @tparam T  container implementation
 */
template <template <class> class T>
class dataStore : public dataStoreBase
{
    typedef dataStore<T> _Myt;
    typedef dataStoreBase _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     */
    dataStore();

    /**
     * Destructor.
     */
    virtual ~dataStore();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve pointer to store.
     *
     * Example usage:
     * @code
     *   // assuming the data store is of mapDataObjectContainer type...
     *   // retrieve store of labels
     *   mapDataObjectContainer<label> *ptr = myDataStore.store<label>();
     *   // retrieve store of graphics
     *   mapDataObjectContainer<graphic> *ptr = myDataStore.store<graphic>();
     * @endcode
     *
     * @returns  pointer to store
     *
     * @tparam I  data object class
     */
    template <class U>
    T<U> *store() {
        return dynamic_cast<T<U>*>( container( U::DATA_OBJECT_TYPE ) );
    }

    /**
     * This is an overloaded function.
     */
    template <class U>
    const T<U> *store() const {
        return const_cast<_Myt*>( this )->store<U>();
    }

};

template <template <class> class T>
dataStore<T>::dataStore() :
    _Mybase()
{
    // create a container for each object type
    for ( int i = 0; i < dataObjectType::_MAX_TYPES; ++i )
        container( i ) = dataObjectContainerFactory::create<T>( (dataObjectType::type) i );
}

template <template <class> class T>
dataStore<T>::~dataStore()
{
}

/**
 * Tag data processing operator.
 *
 * Processes a tag data stream on a data store.
 *
 * @param[in,out] lhs  input stream
 * @param[out] rhs  object to parse into
 * @returns  reference to @p lhs
 */
template <template <class> class T>
tagDataStream& operator >> ( tagDataStream& lhs, dataStore<T>& rhs )
{
    // invoke base class
    lhs >> *static_cast<dataStoreBase*>( &rhs );
    return lhs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // DATASTORE_H
