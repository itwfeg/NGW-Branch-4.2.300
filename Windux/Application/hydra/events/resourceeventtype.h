/**
 * @file resourceeventtype.h
 * @brief Resource Manager Event types.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef RESOURCEEVENTTYPE_H
#define RESOURCEEVENTTYPE_H

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Resource Manager event types.
/**
 * @note Please update "eventfactory.cpp" with added events!
 */
enum class ResourceEventType
{
    Invalid                             = 0,        ///< Invalid event.

    Read                                = 100,      ///< Read one resource.
    ReadAll,                                        ///< Read all resources.
    Write,                                          ///< Write resource.
    Delete,                                         ///< Delete resource.
    DeleteAll,                                      ///< Delete all resources.
};

/// Resource types.
enum class ResourceType
{
    Graphic,                                        ///< Graphics.
    Font,                                           ///< Fonts.
};

/// Graphic resource sub-types.
enum class GraphicSubtype
{
    ROM,                                            ///< ROM Label graphics.
    Product,                                        ///< Product graphics.
    Category,                                       ///< Category graphics.
    Specials,                                       ///< Specials.
    Streaming,                                      ///< Streaming graphics.
    User,                                           ///< User graphics.
    Softkey,                                        ///< Softkey graphics.
    Planogram,                                      ///< Planogram.
};

/// Font resource sub-types.
enum class FontSubtype
{
    User,                                           ///< User fonts.
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // RESOURCEEVENTTYPE_H
