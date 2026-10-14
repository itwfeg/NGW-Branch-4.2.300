/**
 * @file evoltags.h
 * @brief Evolution Tags.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef EVOLTAGS_H
#define EVOLTAGS_H

///////////////////////////////////////////////////////////////////////////////////////////////////

//
// Comm Tags
//

#define EVOL_TAG_COMM_START                         "01"
#define EVOL_TAG_COMM_END                           "FF"

//
// Database Tags
//

#define EVOL_TAG_READ_DEPT_NAME						"75"
#define EVOL_TAG_WRITE_DEPT_NAME         			"9E"
#define EVOL_TAG_DEL_ALL_DEPT_NAMES					"48"

#define EVOL_TAG_READ_PRODUCT                       "53"
#define EVOL_TAG_WRITE_PRODUCT                      "89"
#define EVOL_TAG_DEL_PRODUCT                        "82"
#define EVOL_TAG_DEL_ALL_PRODUCT                    "35"

#define EVOL_TAG_READ_OPERATOR                      "62"
#define EVOL_TAG_WRITE_OPERATOR                     "96"
#define EVOL_TAG_DEL_OPERATOR                       "8E"
#define EVOL_TAG_DEL_ALL_OPERATOR                   "41"

#define EVOL_TAG_READ_OPERATOR_NOTES                "6L"
#define EVOL_TAG_WRITE_OPERATOR_NOTES               "9L"
#define EVOL_TAG_DEL_ALL_OPERATOR_NOTES             "3L"

#define EVOL_TAG_READ_CATEGORY                      "6M"
#define EVOL_TAG_WRITE_CATEGORY                     "9M"
#define EVOL_TAG_DEL_CATEGORY                       "AM"
#define EVOL_TAG_DEL_ALL_CATEGORY                   "3M"

#define EVOL_TAG_READ_EXP_TEXT1                     "64"    // 1
#define EVOL_TAG_WRITE_EXP_TEXT1                    "97"
#define EVOL_TAG_DEL_EXP_TEXT1                      "87"
#define EVOL_TAG_DEL_ALL_EXP_TEXT1                  "42"

#define EVOL_TAG_READ_EXP_TEXT2                     "5U"    // product text 7
#define EVOL_TAG_WRITE_EXP_TEXT2                    "EU"
#define EVOL_TAG_DEL_EXP_TEXT2                      "DU"
#define EVOL_TAG_DEL_ALL_EXP_TEXT2                  "2K"

#define EVOL_TAG_READ_SPECIAL_MSGS                  "66"    // 2
#define EVOL_TAG_WRITE_SPECIAL_MSGS                 "98"

#define EVOL_TAG_READ_PRODUCT_NOTES                 "6K"    // 3
#define EVOL_TAG_WRITE_PRODUCT_NOTES                "9K"
#define EVOL_TAG_DEL_PRODUCT_NOTE                   "AK"
#define EVOL_TAG_DEL_ALL_PRODUCT_NOTES              "3K"

#define EVOL_TAG_READ_MARQUEES                      "59"    // 4
#define EVOL_TAG_WRITE_MARQUEES                     "93"
#define EVOL_TAG_DEL_MARQUEE                        "84"
#define EVOL_TAG_DEL_ALL_MARQUEES                   "37"

#define EVOL_TAG_READ_COOKING_INST                  "5S"    // product text 5
#define EVOL_TAG_WRITE_COOKING_INST                 "ES"
#define EVOL_TAG_DEL_COOKING_INST                   "DS"
#define EVOL_TAG_DEL_ALL_COOKING_INST               "2I"

#define EVOL_TAG_READ_RECIPE                        "5T"    // product text 6
#define EVOL_TAG_WRITE_RECIPE                       "ET"
#define EVOL_TAG_DEL_RECIPE                         "DT"
#define EVOL_TAG_DEL_ALL_RECIPE                     "2J"

#define EVOL_TAG_READ_SUGGEST_SELL                  "5V"    // product text 8
#define EVOL_TAG_WRITE_SUGGEST_SELL                 "EV"
#define EVOL_TAG_DEL_SUGGEST_SELL                   "DV"
#define EVOL_TAG_DEL_ALL_SUGGEST_SELL               "2L"

#define EVOL_TAG_READ_ALLERGENS                     "5W"    // product text 9
#define EVOL_TAG_WRITE_ALLERGENS                    "EW"
#define EVOL_TAG_DEL_ALLERGENS                      "DW"
#define EVOL_TAG_DEL_ALL_ALLERGENS                  "2M"

#define EVOL_TAG_READ_NU_DATA                       "60"
#define EVOL_TAG_WRITE_NU_DATA                      "94"
#define EVOL_TAG_DEL_NU_DATA                        "85"
#define EVOL_TAG_DEL_ALL_NU_DATA                    "38"

//
// Filesystem Tags
//

#define EVOL_TAG_READ_ROM_GRAPHICS                  "6R"
#define EVOL_TAG_WRITE_ROM_GRAPHICS                 "9R"

#define EVOL_TAG_READ_PRODUCT_GRAPHICS              "6S"
#define EVOL_TAG_WRITE_PRODUCT_GRAPHICS             "9S"
#define EVOL_TAG_DEL_PRODUCT_GRAPHIC                "AS"
#define EVOL_TAG_DEL_ALL_PRODUCT_GRAPHICS           "3S"

#define EVOL_TAG_READ_CATEGORY_GRAPHICS             "6T"
#define EVOL_TAG_WRITE_CATEGORY_GRAPHICS            "9T"
#define EVOL_TAG_DEL_CATEGORY_GRAPHIC               "AT"
#define EVOL_TAG_DEL_ALL_CATEGORY_GRAPHICS          "3T"

#define EVOL_TAG_READ_SPECIALS_GRAPHICS             "6U"
#define EVOL_TAG_WRITE_SPECIALS_GRAPHICS            "9U"
#define EVOL_TAG_DEL_SPECIALS_GRAPHIC               "AU"
#define EVOL_TAG_DEL_ALL_SPECIALS_GRAPHICS          "3U"

#define EVOL_TAG_READ_STREAMING_GRAPHICS            "6V"
#define EVOL_TAG_WRITE_STREAMING_GRAPHICS           "9V"
#define EVOL_TAG_DEL_STREAMING_GRAPHIC              "AV"
#define EVOL_TAG_DEL_ALL_STREAMING_GRAPHICS         "3V"

#define EVOL_TAG_READ_USER_GRAPHICS                 "6W"
#define EVOL_TAG_WRITE_USER_GRAPHICS                "9W"
#define EVOL_TAG_DEL_USER_GRAPHIC                   "AW"
#define EVOL_TAG_DEL_ALL_USER_GRAPHICS              "3W"

#define EVOL_TAG_READ_USER_FONTS                    "6Y"
#define EVOL_TAG_WRITE_USER_FONTS                   "9Y"
#define EVOL_TAG_DEL_USER_FONT                      "AY"
#define EVOL_TAG_DEL_ALL_USER_FONTS                 "3Y"

#define EVOL_TAG_READ_SOFTKEY_GRAPHICS              "6Z"
#define EVOL_TAG_WRITE_SOFTKEY_GRAPHICS             "9Z"
#define EVOL_TAG_DEL_SOFTKEY_GRAPHIC                "AZ"
#define EVOL_TAG_DEL_ALL_SOFTKEY_GRAPHICS           "3Z"

#define EVOL_TAG_READ_PLANOGRAM_GRAPHICS            "6H"
#define EVOL_TAG_WRITE_PLANOGRAM_GRAPHICS           "9H"
#define EVOL_TAG_DEL_PLANOGRAM_GRAPHICS             "DH"
#define EVOL_TAG_DEL_ALL_PLANOGRAM_GRAPHICS         "2H"



///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // EVOLTAGS_H
