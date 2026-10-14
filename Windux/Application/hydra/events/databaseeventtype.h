/**
 * @file databaseeventtype.h
 * @brief Database Event types.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DATABASEEVENTTYPE_H
#define DATABASEEVENTTYPE_H

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Database event types.
/**
 * @note Please update "eventfactory.cpp" with added events!
 */
enum class DatabaseEventType
{
    Invalid                             = 0,        ///< Invalid event.

    Read                                = 100,      ///< Read one record.
    ReadAll,                                        ///< Read all records.
    Write,                                          ///< Upsert record.
    Delete,                                         ///< Delete record.
    DeleteAll,                                      ///< Delete all records.
};

/// Product Text types.
enum class ProductTextType
{
    ExpandedText1                       = 1,
    SpecialMessage,
    ProductNote,
    Marquee,
    CookingInstructions,
    Recipe,
    ExpandedText2,
    SuggestiveSelling,
    Allergens
};


///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DATABASEEVENTTYPE_H
