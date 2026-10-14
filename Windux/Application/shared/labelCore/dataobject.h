/**
 * @file dataobject.h
 * Hobart data object base class.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup labelViewerCsu
 * @{
 */

#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include "dataobjecttype.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class dataObjectBase
 * @brief Base class for Hobart data objects.
 *
 * If you derive from this class you must implement the objectType() and objectKey() functions. You
 * must also expose an enumeration named DATA_OBJECT_TYPE that contains the Hobart data type.
 *
 * @note
 * A base implementation for this class is provided by @c dataObject. Consider deriving from that
 * class instead of this.
 */
class dataObjectBase
{
public:

    ///////////////////////////////////////////////////////////////////////////
    // DTOR

    /**
     * Destructor.
     */
    virtual ~dataObjectBase();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve object type.
     *
     * @returns  type of object
     */
    virtual dataObjectType::type objectType() const = 0;

protected:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR

    /**
     * Constructor.
     */
    dataObjectBase();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class dataObject
 * @brief Base class implementation for Hobart data objects.
 *
 * If you derive from this class you must implement the objectKey() function. All Hobart data
 * objects may be inserted into a store.  When doing this it is inserted by using objectKey() as a
 * key.
 *
 * @tparam ObjectType  object type
 * @tparam Key  key class
 */
template <dataObjectType::type ObjectType, class Key>
class dataObject : public dataObjectBase
{
    typedef dataObject<ObjectType, Key> _Myt;
    typedef dataObjectBase _Mybase;

public:

    /**
     * Object type.
     */
    enum {DATA_OBJECT_TYPE = ObjectType};

    /**
     * Key type.
     */
    typedef Key key_type;

    ///////////////////////////////////////////////////////////////////////////
    // DTOR

    /**
     * Destructor.
     */
    virtual ~dataObject() {}

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve object type.
     *
     * @returns  type of object
     */
    virtual dataObjectType::type objectType() const {
        return (dataObjectType::type) DATA_OBJECT_TYPE;
    }

    /**
     * Retrieve the object key value.
     *
     * This key value is used for data store access.
     *
     * @returns  key value
     */
    virtual key_type objectKey() const = 0;

protected:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR

    /**
     * Constructor.
     */
    dataObject() : _Mybase() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // DATAOBJECT_H
