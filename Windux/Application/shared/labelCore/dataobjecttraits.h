/**
 * @file dataobjecttraits.h
 * Hobart data object type traits.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
* @addtogroup labelViewerCsu
 * @{
 */

#ifndef DATAOBJECTTRAITS_H
#define DATAOBJECTTRAITS_H

#include <QList>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @struct dataObjectTraits
 * @brief Defines behavior and traits about a Hobart object.
 *
 * The below implementation is only the default implementation. You may override this behavior with
 * your own by specializing this template. For example:
 * @code
 *   template <>
 *   struct dataObjectTraits<myClass> {
 *      typedef int key_type; // my own key type
 *      typedef QList<key_type> key_list_type; // default implementation fine here
 *      typedef myClassList value_list_type; // my own list type
 *      // so on and so forth...
 *   }
 * @endcode
 *
 * @tparam T  data object class
 */
template <class T>
struct dataObjectTraits
{
    /**
     * Key type.
     *
     * Every Hobart object has a key since all Hobart objects can be put into a data store.
     */
    typedef typename T::key_type key_type;

    /**
     * Key list type.
     */
    typedef QList<key_type> key_list_type;

    /**
     * Value list type.
     */
    typedef QList<T> value_list_type;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // DATAOBJECTTRAITS_H
