/**
 * @file tagdatafunctioncode.h
 * Hobart tag data function codes.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup labelViewerCsu
 * @{
 */

#ifndef TAGDATAFUNCTIONCODE_H
#define TAGDATAFUNCTIONCODE_H

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class tagDataFunctionCode
 * @brief Hobart tag data function codes.
 */
class tagDataFunctionCode
{
public:

    /**
     * Hobart tag data function codes.
     *
     * @note
     * When compiling this header your compiler may complain about multi-character constants. You
     * can disable this with the "-Wno-multichar" option. For Qt projects, you can add the line
     * "QMAKE_CXXFLAGS += -Wno-multichar" to your .pro file.
     *
     * @note
     * It can be argued that using multi-character constants is bad programming because it is not
     * platform neutral (i.e. big versus small endianness). Endianness is a function of the data
     * stream, not the platform. So long as you read the stream correctly you should be fine. As
     * an FYI, the @c tagDataStream class is big endian, and it is enforced on reads and writes.
     */
    enum type
    {
        // protocol function codes
        DEPARTMENT_NUMBER                   = 'DN',     ///< This tag represents the Department Number that a given record is associated
                                                        ///< with. It is an optional tag for all record types that may support it
                                                        ///< (currently scales don't support multiple departments so this is intended for
                                                        ///< future development).

        DATA_TYPE                           = 'DT',     ///< This tag represents the Data Type of a given data stream and is a required
                                                        ///< field in a header record. When a stream is referred to as a Type 1 Data
                                                        ///< Stream it means a stream that has a header record where the DT tag has a value
                                                        ///< of 1 (indicating Hobart tag based data).

        ERROR_CODE                          = 'EC',     ///< This tag represents the Error Code that an error record is returning. The
                                                        ///< error code indicates what error the error record is describing to the
                                                        ///< controller. It is a required field in the error record.

        ERROR_RECORD                        = 'ER',     ///< This tag represents the Error Record type that had the error. Since error
                                                        ///< records may be returned from a scale at any time during a controller
                                                        ///< transmission there must be a way to indicate which record type triggered the
                                                        ///< record. This is an optional field in an error record.

        FAILED_TAG                          = 'FT',     ///< This tag represents the Failed Tag data. It is an optional field in the error
                                                        ///< record and indicates the field in a record that generated a specific error.

        SCALE_ID                            = 'ID',     ///< This tag designates the scale ID. It is an optional field in the header
                                                        ///< record.

        KEY_VALUE                           = 'KV',     ///< This tag represents the Key Value tag. It returns the value of the key of the
                                                        ///< record that generated the error. i.e. if PLU 17 had an error because of the
                                                        ///< value of the by count field the KV value would be 17. It is an optional field
                                                        ///< in the error record.

        // data function codes
        PLU_NUMBER                          = 'p#',     ///< PLU Number
        CLASS_NUMBER                        = 'c#',     ///< Class Number
        PRODUCT_TYPE                        = 'PT',     ///< Product type
        PICTURE_ID                          = 'g#',     ///< Picture id
        PICTURE_NAME1                       = 'G1',     ///< Picture Name 1
        PICTURE_NAME2                       = 'G2',     ///< Picture Name 2
        PICTURE_NAME3                       = 'G3',     ///< Picture Name 3
        PICTURE_NAME4                       = 'G4',     ///< Picture Name 4
        EXTENDED_TEXT_NUMBER                = 'r#',     ///< Extended text number
        SPECIAL_MESSAGE_NUMBER              = 's#',     ///< Special message number
        PRODUCT_NOTE_NUMBER                 = 'n#',     ///< Product note number (Evol)
        NUTRIFACT_NUMBER                    = 'N#',     ///< Nutrifact Number (Evol)
        CATEGORY_NUMBER                     = 'C#',     ///< Category Number (Evol)
        SPECIALS_NUMBER                     = 'S#',     ///< Specials Number (Evol)
        BY_COUNT                            = 'bc',     ///< By Count
        FORCED_BY_COUNT                     = 'fb',     ///< Forced By Cound
        DESCRIPTION                         = 'dt',     ///< Description
        NAME1                               = 'n1',     ///< Name 1
        NAME2                               = 'n2',     ///< Name 2
        FIRST_LABEL_TYPE                    = 'l1',     ///< First label type
        SECOND_LABEL_TYPE                   = 'l2',     ///< Second label type
        THIRD_LABEL_TYPE                    = 'l3',     ///< Third label type
        FIRST_PRINTER_CODE                  = 'p1',     ///< First printer code
        SECOND_PRINTER_CODE                 = 'p2',     ///< Second printer code
        THIRD_PRINTER_CODE                  = 'p3',     ///< Third printer code
        FIRST_LABEL_PLACEMENT               = 'L1',     ///< First label placement
        SECOND_LABEL_PLACEMENT              = 'L2',     ///< Second label placement
        THIRD_LABEL_PLACEMENT               = 'L3',     ///< Third label placement
        INTAKE_MODE                         = 'im',     ///< Intake mode
        NET_WEIGHT                          = 'nw',     ///< Net weight
        PLU_FLAGS                           = 'pf',     ///< PLU Flags
        PRICE_MODIFIER                      = 'pm',     ///< Price Modifier
        REPORTING_CLASS                     = 'rc',     ///< Reporting Class
        SHELF_LIFE                          = 'sl',     ///< Shelf life
        SHELF_LIFE_HOURS                    = 'SL',     ///< Shelf life hours
        PRINT_SHELF_LIFE                    = 'PS',     ///< Print shelf life
        UNIT_PRICE                          = 'u$',     ///< Unit price
        FORCED_PRICE                        = 'fp',     ///< Forced Price
        UPC_NUMBER_SYSTEM                   = 'u#',     ///< UPC number system / EAN Prefix
        UPC                                 = 'up',     ///< UPC / EAN13 number
        PRODUCT_NUMBER                      = 'pn',     ///< Product Number
        VENDOR_NUMBER                       = 'vn',     ///< Vendor Number
        EAN_PREFIX                          = 'ex',     ///< EAN Prefix
        EAN_CODE                            = 'Ec',     ///< EAN Code
        EAN_FORMAT_STRING                   = 'ef',     ///< EAN Format String
        PROPORTIONAL_TARE                   = 'pt',     ///< Proportional Tare
        PRODUCT_LIFE                        = 'pl',     ///< Product Life
        PRODUCT_LIFE_HOURS                  = 'PL',     ///< Product Life Hours
        PRINT_PRODUCT_LIFE                  = 'Pi',     ///< Print Product Life
        PRINT_PACKED_ON                     = 'PP',     ///< Print packed on
        TARE                                = 'ta',     ///< Tare
        FORCED_TARE                         = 'fT',     ///< Forced Tare
        EXCEPTION_PRICE                     = 'xp',     ///< Exception Price
        DISCOUNT_TYPE                       = 'Dt',     ///< Discount Type
        DISCOUNT_PRICE1                     = 'dp',     ///< Discount Price 1
        DISCOUNT_PRICE2                     = 'D2',     ///< Discount Price 2
        DISCOUNT_PRICE3                     = 'D3',     ///< Discount Price 3
        BONUS_POINTS                        = 'ps',     ///< Bonus Points
        USE_SECURITY_LABEL                  = 'us',     ///< Use security label
        SPECIAL_START_DATE                  = 'sS',     ///< Special start date
        IMAGE1                              = 'i1',     ///< Image 1
        IMAGE2                              = 'i2',     ///< Image 2
        NUTRIFACTS_DATA                     = 'ND',     ///< FDA Nutrifacts Data
        NUTRITION_COLUMN1                   = 'N1',     ///< Nutrition Column 1
        NUTRITION_COLUMN2                   = 'N2',     ///< Nutrition Column 2
        NUTRITION_COLUMN3                   = 'N3',     ///< Nutrition Column 3
        NUTRITION_COLUMN4                   = 'N4',     ///< Nutrition Column 4
        EXTENDED_TEXT                       = 'rt',     ///< Extended Text
        SPECIAL_MESSAGE                     = 'sm',     ///< Special Message
        OPERATOR_CLASS                      = 'oc',     ///< Operator Class (access level)
        OPERATOR_ID                         = 'oi',     ///< Operator ID
        OPERATOR_RFID                       = 'or',     ///< Operator RFID
        OPERATOR_NAME                       = 'on',     ///< Operator Name
        OPERATOR_SAW_NOTE_FLAG              = 'os',     ///< Operator Saw Note Flag
        OPERATOR_CHANGE_TYPE                = 'tc',     ///< Operator change type
        OPERATOR_CHANGE_OLD_VALUE           = 'ov',     ///< Operator change old value
        OPERATOR_CHANGE_NEW_VALUE           = 'nv',     ///< Operator change new value
        DATE                                = 'd_',     ///< Date
        TIME                                = 't_',     ///< Time
        DAY                                 = 'da',     ///< Day
        SPECIAL_NAME                        = 'sN',     ///< Special name
        SPECIAL_LENGTH                      = 'dl',     ///< Special length
        STREAMING_GRAPHIC_NUMBER            = 'G#',     ///< Streaming graphic number
        NEXT_STREAMING_GRAPHIC              = 'Gn',     ///< Next streaming graphic
        STREAMING_GRAPHIC_DURATION          = 'Gd',     ///< Streaming graphic duration
        STREAMING_GRAPHIC_ACTION_TYPE       = 'Gt',     ///< Streaming graphic action type
        STREAMING_GRAPHIC_ACTION_PARAM      = 'Gp',     ///< Streaming graphic action parameter
        SEQUENCE                            = 'sq',     ///< Sequence
        SEQUENCE_MAX                        = 'qm',     ///< Sequence Max
        RESOURCE_ID                         = 'ri',     ///< Resource ID
        RESOURCE_LENGTH                     = 'rl',     ///< Resource Length
        RESOURCE                            = 'rs',     ///< Resource
        IMAGE_TYPE                          = 'it',     ///< Image Type
        ALLOW_MANUAL_UPC_DIGIT_CHANGES      = 'mc',     ///< Allow manual UPC digit changes
        REPORTS_TO_PRODUCTION_TOTALS        = 'rp',     ///< Reports to production totals
        ALLOW_BACKDATING                    = 'bd',     ///< Allow backdating
        AUTO_UPC_DIGIT_CHANGES              = 'ac',     ///< Automatic UPC digit changes
        TOTALS_NAME_FLAGS                   = 'nf',     ///< Totals name flags
        TOTALS_NAME_KEY                     = 'tk',     ///< Totals name key
        TOTALS_NAME                         = 'tn',     ///< Totals name
        UPC_CHANGE_DIGIT                    = 'uc',     ///< UPC change digit
        SYSTEM_STRING_NUMBER                = 'y#',     ///< System string number
        STORE_ID                            = 'si',     ///< Store ID
        STORE_NAME                          = 'sn',     ///< Store Name
        CONFIG_VERSION                      = 'cv',     ///< Configuration Version
        SCALE_CONFIG                        = 'sf',     ///< Scale configuration
        SCALE_CONDITION                     = 'sc',     ///< Scale condition
        SCALE_ERROR                         = 'se',     ///< Scale error
        SCALE_STATUS                        = 'ss',     ///< Scale status
        SCALE_TYPE                          = 'st',     ///< Scale type
        TOTAL_SELECT_TIME                   = 'St',     ///< Total select time
        SYSTEM_STRING                       = 'sy',     ///< System String
        TOTAL_PRICE                         = 't$',     ///< Total Price
        TOTAL_FLAGS                         = 'tf',     ///< Total Flags
        TOTAL_PACKAGES                      = 'tp',     ///< Total packages
        TOTAL_ITEMS                         = 'ti',     ///< Total Items
        TOTAL_RUNS                          = 'tr',     ///< Total Runs
        TOTAL_TIMES_SELECTED                = 'tS',     ///< Total times selected
        TOTAL_TYPE                          = 'tt',     ///< Total type
        TOTAL_WEIGHT                        = 'tw',     ///< Total Weight
        TRANSACTION_HOUR                    = 'xh',     ///< Transaction Hour
        SELECTED_LANGUAGE_CODE              = 'ls',     ///< Selected Language Code
        FREE_MEMORY                         = 'fm',     ///< Free memory
        TOTAL_MEMORY                        = 'me',     ///< Total memory
        CLASS_COUNT                         = '#c',     ///< Class Count
        FONT_COUNT                          = '#f',     ///< Font Count
        PICTURE_COUNT                       = '#g',     ///< Picture count
        MARQUEE_COUNT                       = '#m',     ///< Marquee Count
        NUTRIFACTS_COUNT                    = '#n',     ///< Nutrifacts Count
        OPERATOR_COUNT                      = '#o',     ///< Operator Count,
        PLU_COUNT                           = '#p',     ///< PLU Count
        EXTENDED_TEXT_COUNT                 = '#r',     ///< Extended Text Count
        SPECIAL_MESSAGE_COUNT               = '#s',     ///< Special Message Count
        TOTALS_COUNT                        = '#t',     ///< Totals Count
        CHANGE_NUMBER                       = 'qq',     ///< Change Number
        IP_ADDRESS                          = 'ip',     ///< IP Address
        GATEWAY                             = 'gw',     ///< Gateway
        HOST_NAME                           = 'hn',     ///< Host Name
        DATABASE_DATE                       = 'tb',     ///< Database date
        FILENAME                            = 'FN',     ///< Filename
        PRODUCT_NOTE                        = 'an',     ///< Product Note
        OPERATOR_NOTE                       = 'oN',     ///< Operator Note
        PRESENTATION_METHOD_TYPE            = 'Pt',     ///< Presentation Method Type
        PRESENTATION_METHOD_DESC            = 'Pd',     ///< Presentation Method Description
        PARENT_CATEGORY_NUMBER              = 'rC',     ///< Parent category number
        CATEGORY_DISPLAY_ORDER              = 'do',     ///< Category display order
        COOL_TOTALS_TIME1                   = 'cp',     ///< COOL totals time 1
        COOL_TOTALS_TIME2                   = 'cn',     ///< COOL totals date 2
        COOL_TOTALS_DATE1                   = 'ck',     ///< COOL totals time 1
        COOL_TOTALS_DATE2                   = 'cl',     ///< COOL totals date 2
        PRE_TEXT_NUMBER                     = 'ci',     ///< Pre text number
        PRE_COOL_TEXT_PRIMARY_LANG          = 'ch',     ///< Pre COOL text primary language
        PRE_COOL_TEXT_SECONDARY_LANG        = 'cH',     ///< Pre COOL text secondary language
        TRACKING_TEXT_NUMBER_ASSOC          = 'cg',     ///< Tracking text number assoc
        FORCE_COOL                          = 'cf',     ///< Force COOL
        NEEDS_COOL_TEXT                     = 'co',     ///< Needs COOL text
        TRACKING_TEXT                       = 'cc',     ///< Tracking Text
        COOL_CATEGORY_NUMBER                = 'cb',     ///< COOL Category number
        NEXT_SHORTLIST_KEY                  = 'ca',     ///< Next shortlist key
        SHORTLIST_KEY                       = 'c9',     ///< Shortlist key
        SHORTLIST_NUMBER                    = 'cs',     ///< Shortlist number
        CURRENT_COOL_TEXT_KEY               = 'c8',     ///< Current COOL text key
        DEFAULT_COOL_TEXT_KEY               = 'c7',     ///< Default COOL text key
        CUSTOM_COOL_TEXT                    = 'c4',     ///< Custom COOL text
        CUSTOM_COOL_TEXT_KEY                = 'c3',     ///< Custom COOL text key
        COOL_TEXT                           = 'c2',     ///< COOL Text
        COOL_TEXT_KEY                       = 'c1',     ///< COOL Text Key
        COOL_TEXT_TYPE                      = 'cq',     ///< COOL Text Type
        COUNTRY_KEY                         = 'c5',     ///< Country Key
        COUNTRY_NAME_PRIMARY_LANG           = 'c6',     ///< Country Name Primary Language
        COUNTRY_NAME_SECONDARY_LANG         = 'C6',     ///< Country Name Secondary Language
        CATEGORY_NAME_PRIMARY_LANG          = 'c0',     ///< Category Name Primary Language
        CATEGORY_NAME_SECONDARY_LANG        = 'C0',     ///< Category Name Secondary Language
        BELONGS_TO_ALL_CATEGORIES_FLAG      = 'CA',     ///< Belongs to all categories flag
        PROD_STEP_NAME_SECONDARY_LANG       = 'cj',     ///< Production Step Name Secondary Language
        COOL_TEXT_ORDER_NUMBER              = 'CO',     ///< COOL text order number
        STYLE_NUMBER                        = 'Sn',     ///< Style number
        VERTICAL_ALIGNMENT                  = 'vA',     ///< Vertical Alignment
        TEXT_COLOR                          = 'tC',     ///< Text color
        DISABLED_TEXT_COLOR                 = 'tD',     ///< Disabled text color
        BACKGROUND_COLOR                    = 'bC',     ///< Background color
        BACKGROUND_IMAGE                    = 'bl',     ///< Background image
        IMAGE_DIRECTORY                     = 'fd',     ///< Image directory
        FOREGROUND_IMAGE_LOCATION           = 'fD',     ///< Foreground image location
        FEATURE_NUMBER                      = 'F#',     ///< Feature number
        HORIZONTAL_GRID_SIZE                = 'hG',     ///< Horizontal grid size
        VERTICAL_GRID_SIZE                  = 'vG',     ///< Vertical grid size
        HORIZONTAL_SPACING                  = 'hS',     ///< Horizontal spacing
        VERTICAL_SPACING                    = 'vS',     ///< Vertical spacing
        SOFTKEY_GROUP_NUMBER                = 'gn',     ///< Softkey group number
        SOFTKEY_NUMBER                      = 'f#',     ///< Softkey/Flashkey Number
        HORIZONTAL_POSITION                 = 'hp',     ///< Horizontal position
        VERTICAL_POSITION                   = 'vp',     ///< Vertical position
        HORIZONTAL_SIZE                     = 'fh',     ///< Horizontal size
        VERTICAL_SIZE                       = 'fv',     ///< Vertical size
        SOFTKEY_NAME1                       = 'kn',     ///< Softkey name 1
        SOFTKEY_NAME2                       = 'kN',     ///< Softkey name 2
        SOFTKEY_ACTION                      = 'ft',     ///< Softkey action
        SOFTKEY_ACTION_SUBTYPE              = 'as',     ///< Softkey action subtype
        SOFTKEY_ACTION_INT_PARAM            = 'fn',     ///< Softkey action int parameter
        SOFTKEY_ACTION_STRING_PARAM         = 'fS',     ///< Softkey action string parameter
        DAYS_TO_KEEP_OPERATOR_CHANGES       = 'od',     ///< Days to Keep Operator changes
        SHOW_DELETE_ALL_FILES               = 's1',     ///< Show delete all files
        SHOW_PRINTER_SETTINGS               = 's2',     ///< Show printer settings
        WEIGHT_PRECISION                    = 'Wp',     ///< Weight precision
        TARE_PRECISION                      = 'Tp',     ///< Tare precision
        EXPIRATION_NOTE                     = 'EN',     ///< Expiration note
        CHECK_FOR_NETWORK_CONNECTIVITY      = 'Cn',     ///< Check for network connectivity
        ALLOW_OPERATOR_DESKTOP_ACCESS       = 'AD',     ///< Allow operator access to desktop
        SYNCH_SCALE_TYPE                    = 'sT',     ///< Synch scale type
        CLEAR_PLU_AFTER_PRINTING            = 'cP',     ///< Clear PLU after printing
        COMMUNICATIONS_HISTORY              = 'CH',     ///< Communications History
        EMAIL_FROM_SETTING                  = 'Am',     ///< Email from setting
        PERMANENT_MARQUEE_TEXT              = 'mP',     ///< Permanent Marquee Text
        FTP_SERVER_IP                       = 'An',     ///< FTP Server IP address
        FTP_USER_NAME                       = 'AN',     ///< FTP User name
        FTP_SUBDIRECTORY                    = 'Ao',     ///< FTP Subdirectory
        FTP_PASSWORD                        = 'AO',     ///< FTP Password
        EXPIRATION_DATE_TIME                = 'el',     ///< Expiration Date and Time
        NUTRITIONAL_SCORE                   = 'ns',     ///< Nutritional Score
        PRINT_FIRST_LETTER_OF_TOTAL_TYPE    = 'fL',     ///< Print first letter of total type
        ALWAYS_PRINT_NET_WT_ON_FIXED_WT     = 'NW'      ///< Always print net weight on fixed weight
    };

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // TAGDATAFUNCTIONCODE_H
