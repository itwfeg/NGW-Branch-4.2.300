/**
 * @file dataobjecttype.h
 * Hobart data object types.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
* @addtogroup labelViewerCsu
 * @{
 */

#ifndef DATAOBJECTTYPE_H
#define DATAOBJECTTYPE_H

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class dataObjectType
 * @brief Hobart data object types.
 */
class dataObjectType
{
public:

    /**
     * Hobart data object types.
     *
     * This is an enumeration of all data object types supported.
     *
     * @note
     * When adding a new type into the system your implementation needs to derive from the
     * @c dataObject base class.
     *
     * @note
     * In addition to the above you will also need to declare your object type using the
     * DECLARE_DATA_OBJECT() macro. This macro adds your class implementation into the type
     * system.
     *
     * @note
     * If you plan on using containers update the factory create method in the
     * @c dataObjectContainerFactory class.
     *
     * @note
     * Here is an example implementation header:
     * @code
     *   DECLARE_DATA_OBJECT( myLabelClass, dataObjectType::LABEL );
     *
     *   class myLabelClass : public dataObject< dataObjectType::LABEL, int > {
     *   public:
     *       // implementation here
     *   };
     * @endcode
     */
    enum type
    {
        GRAPHIC = 0,                        ///< Graphic object that is printed on a label
        LABEL,                              ///< Label data object that can be printed
        NUTRIFACT,                          ///< Nutrifact information which is printed on a label
        SYSTEM_STRING,                      ///< Global string that is printed on a label

        // insert additional types above this line in alphabetical order
        _MAX_TYPES
    };

    /**
     * @struct toConcreteType
     * @brief Maps a @c type to a concrete class.
     *
     * This structure provides mapping from type @c T to a concrete class.
     *
     * Example usage:
     * @code
     *   // create a label
     *   dataObjectType::toConcreteType<dataObjectType::LABEL>::type label;
     * @endcode
     *
     * @note
     * This is just a forward declaration. Use the DECLARE_DATA_OBJECT() macro below in your
     * class source header file.
     *
     * @tparam T  type to map
     */
    template <type T>
    struct toConcreteType {};

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * This macro maps a data object type to a concrete class.
 *
 * Example usage:
 * @code
 *   // declare data object implementations
 *   DECLARE_DATA_OBJECT( label, dataObjectType::LABEL );
 *   DECLARE_DATA_OBJECT( graphic, dataObjectType::GRAPHIC );
 * @endcode
 */
#define DECLARE_DATA_OBJECT( CLASS, TYPE ) \
    class CLASS; \
    template <> struct dataObjectType::toConcreteType<TYPE> { \
        typedef CLASS type; \
    }

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // DATAOBJECTTYPE_H
