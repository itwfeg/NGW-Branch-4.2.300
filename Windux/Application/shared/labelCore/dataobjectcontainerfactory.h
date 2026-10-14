/**
 * @file dataobjectcontainerfactory.h
 * Factory method classes to produce containers.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
* @addtogroup labelViewerCsu
 * @{
 */

#ifndef DATAOBJECTCONTAINERFACTORY_H
#define DATAOBJECTCONTAINERFACTORY_H

#include "dataobjectcontainer.h"

#include "graphic.h"
#include "label.h"
#include "nutrifact.h"
#include "systemstring.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class dataObjectContainerFactory
 * @brief Factory methods to produce container objects.
 */
class dataObjectContainerFactory
{
    typedef dataObjectContainerFactory _Myt;

public:

    /**
     * Create a new container object.
     *
     * Containers implementation can be performed in many different ways, that is why you must
     * specify the container type. Here is some example usage of this factory method:
     * @code
     *   // create a map container of labels
     *   void *ptr = dataObjectContainerFactory::create<mapDataObjectContainer>( dataObjectType::LABEL );
     *   // create a list container of graphics
     *   void *ptr = dataObjectContainerFactory::create<listDataObjectContainer>( dataObjectType::GRAPHIC );
     *   // create a database container of nutrifacts
     *   void *ptr = dataObjectContainerFactory::create<databaseDataObjectContainer>( dataObjectType::NUTRIFACT );
     * @endcode
     *
     * @param[in] t  type of object container class to create
     * @returns  pointer to container class
     *
     * @tparam T  container implementation
     */
    template <template <class> class T>
    static dataObjectContainerBase *create( dataObjectType::type t )
    {
        switch ( t )
        {
        case dataObjectType::GRAPHIC:
            return new T<dataObjectType::toConcreteType<dataObjectType::GRAPHIC>::type>;
        case dataObjectType::LABEL:
            return new T<dataObjectType::toConcreteType<dataObjectType::LABEL>::type>;
        case dataObjectType::NUTRIFACT:
            return new T<dataObjectType::toConcreteType<dataObjectType::NUTRIFACT>::type>;
        case dataObjectType::SYSTEM_STRING:
            return new T<dataObjectType::toConcreteType<dataObjectType::SYSTEM_STRING>::type>;
        default:
            break;
        }
        return NULL;
    }

private:

    // not implemented
    dataObjectContainerFactory();

    // not implemented
    dataObjectContainerFactory( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // DATAOBJECTCONTAINERFACTORY_H
