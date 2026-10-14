/**
 * @file labelformtype.h
 * Label form types.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup labelViewerCsu
 * @{
 */

#ifndef LABELFORMTYPE_H
#define LABELFORMTYPE_H

#include <QString>
#include <QMimeData>
#include <QPixmap>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class labelFormType
 * @brief Label form types.
 *
 * Enumeration class of label form types.
 *
 * @sa labelForm
 */
class labelFormType
{
public:

    /**
     * Label form types.
     *
     * @note
     * When adding new types be sure to add the stringified form to the toString() method.
     */
    enum type
    {
        BARCODE                             = 0,    ///< This form displays the product's barcode. The data comes from the barcode field
                                                    ///< of the corresponding product record. It can display a UPC-A (subtype 0), UPC-D3
                                                    ///< (subtype 1), UPC-E (subtype 2), and EAN13 (subtype 3) barcodes. [POST_IMAGE,
                                                    ///< BARCODE_ENTITY]

        DESCRIPTION                         = 1,    ///< This form displays the item description. [PRE_IMAGE, TEXT_ENTITY]

        NUTRIFACTS                          = 2,    ///< This is the original @c NUTRIFACTS form type. It is not used anymore.

        EXPANDED_TEXT_1                     = 3,    ///< Form typically used for Ingredients, its data comes from the product's expanded
                                                    ///< text field. [PRE_IMAGE, TEXT_ENTITY]

        EXPANDED_TEXT_2                     = 4,    ///< NOT USED

        SPECIAL_MESSAGE                     = 5,    ///< Data comes from the product's special message field. [PRE_IMAGE, TEXT_ENTITY]

        COUPON_TEXT                         = 6,    ///< NOT USED

        PACKED_ON                           = 7,    ///< This form displays when the package is wrapped (its format is set in
                                                    ///< Configuration). [POST _IMAGE, TEXT_ENTITY]

        SELL_BY                             = 8,    ///< This form displays the product's Shelf Life. [POST _IMAGE, TEXT_ENTITY]

        BEST_BEFORE                         = 9,    ///< This form displays the product's Product Life. [POST _IMAGE, TEXT_ENTITY]

        NET_WEIGHT                          = 10,   ///< Gross weight minus any package/tare weight. [POST_IMAGE, TEXT_ENTITY]

        FIXED_WEIGHT                        = 11,   ///< Shows what the Fixed Weight product should weigh. (Data comes from the product's
                                                    ///< fixed weight field.) [POST_IMAGE, TEXT_ENTITY]

        TARE                                = 12,   ///< This is the packaging weight to be subtracted from the gross weight to get the
                                                    ///< net weight. [POST_IMAGE, TEXT_ENTITY]

        UNIT_PRICE                          = 13,   ///< Shows price per pound, price per by count or price per fixed weight depending on
                                                    ///< Item type. [POST_IMAGE, TEXT_ENTITY]

        TOTAL_PRICE                         = 14,   ///< Total price shows the customer what price they are paying for the package.
                                                    ///< [POST_IMAGE, TEXT_ENTITY]

        STORE_NAME                          = 15,   ///< [PRE_IMAGE, TEXT_ENTITY]

        STORE_NUMBER                        = 16,   ///< [PRE_IMAGE, TEXT_ENTITY]

        OPERATOR_NUMBER                     = 17,   ///< [PRE_IMAGE, TEXT_ENTITY]

        OPERATOR_NAME                       = 18,   ///< [PRE_IMAGE, TEXT_ENTITY]

        USER_LINE                           = 19,   ///< Lines printed with Generic Text. The subtype contains the thickness of the line.
                                                    ///< The Data is always 0. [PRE_IMAGE, LINE_ENTITY]

        USER_GRAPHIC                        = 20,   ///< Graphic Linked to the Product [PRE_IMAGE, GRAPHIC _ENTITY]

        STORE_GRAPHIC                       = 21,   ///< Graphic Linked to the Store. Its name is set in Configuration. [PRE_IMAGE,
                                                    ///< GRAPHIC_ENTITY]

        GENERIC_TEXT_1                      = 22,   ///< Label String (usually for Packed On). [PRE_IMAGE, TEXT_ENTITY]

        GENERIC_TEXT_2                      = 23,   ///< Label String (usually for Best Before). [PRE_IMAGE, TEXT_ENTITY]

        GENERIC_TEXT_3                      = 24,   ///< Label String (usually for Sell By). [PRE_IMAGE, TEXT_ENTITY]

        GENERIC_TEXT_4                      = 25,   ///< Label String (usually for Net Weight). [PRE_IMAGE, TEXT_ENTITY]

        GENERIC_TEXT_5                      = 26,   ///< Label String (usually for Unit price). [PRE_IMAGE, TEXT_ENTITY]

        GENERIC_TEXT_6                      = 27,   ///< Label String (usually for Total Price). [PRE_IMAGE, TEXT_ENTITY]

        GENERIC_TEXT_7                      = 28,   ///< Label String [PRE_IMAGE, TEXT_ENTITY]

        GENERIC_TEXT_8                      = 29,   ///< Label String [PRE_IMAGE, TEXT_ENTITY]

        GENERIC_TEXT_9                      = 30,   ///< Label String [PRE_IMAGE, TEXT_ENTITY]

        GENERIC_TEXT_10                     = 31,   ///< Label String [PRE_IMAGE, TEXT_ENTITY]

        PRODUCT_NUMBER                      = 32,   ///< PLU number [PRE_IMAGE, TEXT_ENTITY]

        REPORTING_CLASS                     = 33,   ///< This is the Class number that the PLU reports to. This is set up in the PLU
                                                    ///< record. [PRE_IMAGE, TEXT_ENTITY]

        TOTAL_WEIGHT                        = 34,   ///< Used in itemized labels and used in totals reports [POST_IMAGE, TEXT_ENTITY]

        TOTAL_VALUE                         = 35,   ///< Used in itemized labels and used in totals reports [POST_IMAGE, TEXT_ENTITY]

        TOTAL_PACKAGES                      = 36,   ///< Used in itemized labels and used in totals reports [PRE_IMAGE, TEXT_ENTITY]

        TOTAL_RUNS                          = 37,   ///< Used in totals reports [PRE_IMAGE, TEXT_ENTITY]

        TOTAL_TYPE                          = 38,   ///< Used in totals reports. WAS USED to distinguish between A or B totals in the
                                                    ///< SP1500.   NOT USED

        TOTAL_TYPE_NAME                     = 39,   ///< Used in totals reports. Can also be used on regular labels as in the case of
                                                    ///< Rewraps. [PRE_IMAGE, TEXT_ENTITY]

        TOTAL_HOURS                         = 40,   ///< Used in totals reports [PRE_IMAGE, TEXT_ENTITY]

        UPC_NUMBER                          = 41,   ///< @c BARCODE is the barcode field. @c UPC_NUMBER specifies the digits that make up the
                                                    ///< barcode. [PRE_IMAGE, TEXT_ENTITY]

        OP_ACCESS_LEVEL                     = 42,   ///< Used mostly for Operator totals reports. [PRE_IMAGE, TEXT_ENTITY]

        PRICE_FORM                          = 43,   ///< ONLY USED in SP600 supervisor labels

        WEIGHT_TYPE_FORM                    = 44,   ///< Used on supervisor reports (fixed & random weight items) ONLY USED in SP600

        TEXT_FORM                           = 45,   ///< NOT USED

        SHELF_LIFE_FORM                     = 46,   ///< Used on supervisor reports ONLY USED in SP600

        LINE_FORM                           = 47,   ///< NOT USED

        PRODUCT_LIST                        = 48,   ///< Used on supervisor report. Shows a list of PLU numbers associated with a text
                                                    ///< record (Expanded text, Special message, etc.) [PRE_IMAGE, TEXT_ENTITY]

        SPECIAL_TEXT                        = 49,   ///< Used on cutting test and configuration labels [TEXT_ENTITY]

        FIXED_TEXT                          = 50,   ///< NOT USED

        CURRENT_TIME                        = 51,   ///< Self explanatory [POST_IMAGE, TEXT_ENTITY]

        TOTAL_TYPE_NUMBER                   = 52,   ///< Used in totals reports [PRE_IMAGE, TEXT_ENTITY]

        INDIRECT_LABEL_REFERENCE            = 53,   ///< This is for a "nested label". The label number is imbedded in the form_number
                                                    ///< field (Flags are usually set to 0).

        NUTRIFACT_TEXT_1                    = 54,   ///< Nutrifact text with no values associated. This text has no changeable data
                                                    ///< associated with it. Not used in Evolution.

        NUTRIFACT_TEXT_2                    = 55,   ///< Nutrifact text with values following. This text has changeable data associated with
                                                    ///< it. Data contains a pointer to the text string. Not used in Evolution.

        NUTRIFACT_VALUE_1                   = 56,   ///< This is the V1 field. Nutrition info requires the number of mg; the V1 data is
                                                    ///< displayed in this location. Not used in Evolution.

        NUTRIFACT_VALUE_2                   = 57,   ///< This is the V2 field. Nutrition info requires the % DV; the V2 data is displayed
                                                    ///< in this location. Not used in Evolution.

        NUTRIFACT_VALUE_3                   = 58,   ///< This is the V3 field.  Additional info desired with the NUTRIFACT_TEXT_2 field;
                                                    ///< the V3 data is displayed in this location. Not used in Evolution.

        DIVIDING_LINE                       = 59,   ///< Always prints (unlike @c USER_LINE). The type contains the thickness of the line.
                                                    ///< The data is always NULL. [PRE_IMAGE, LINE_ENTITY]

        TOTAL_ITEMS                         = 60,   ///< Itemized Labels and used in totals reports [PRE_IMAGE, TEXT_ENTITY]

        CONSTANT_TEXT                       = 61,   ///< This differs from Generic Text in that it does not use any of the Label Strings.
                                                    ///< Constant text is always printed. The value is stored in DATA Property. The string
                                                    ///< is created with the label. [PRE_IMAGE, TEXT_ENTITY]

        GENERIC_TEXT_11                     = 62,   ///< Used from label string 11 Label Strings [PRE_IMAGE, TEXT_ENTITY]

        GENERIC_TEXT_12                     = 63,   ///< Used from label string 12 Label Strings [PRE_IMAGE, TEXT_ENTITY]

        GENERIC_TEXT_13                     = 64,   ///< Used from label string 13 Label Strings [PRE_IMAGE, TEXT_ENTITY]

        GENERIC_TEXT_14                     = 65,   ///< Used from label string 14 Label Strings [PRE_IMAGE, TEXT_ENTITY]

        GENERIC_TEXT_15                     = 66,   ///< Used from label string 15 Label Strings [PRE_IMAGE, TEXT_ENTITY]

        GENERIC_TEXT_16                     = 67,   ///< Used from label string 16 Label Strings [PRE_IMAGE, TEXT_ENTITY]

        GENERIC_TEXT_17                     = 68,   ///< Used from label string 17 Label Strings [PRE_IMAGE, TEXT_ENTITY]

        GENERIC_TEXT_18                     = 69,   ///< Used from label string 18 Label Strings [PRE_IMAGE, TEXT_ENTITY]

        GENERIC_TEXT_19                     = 70,   ///< Used from label string 19 Label Strings [PRE_IMAGE, TEXT_ENTITY]

        GENERIC_TEXT_20                     = 71,   ///< Used from label string 20 Label Strings [PRE_IMAGE, TEXT_ENTITY]

        INDIRECT_GRAPHIC_REFERENCE          = 72,   ///< This form displays a graphic. The graphic name is imbedded in the Graphic Name
                                                    ///< field. It cannot be changed via Configuration or the PLU record. [PRE_IMAGE,
                                                    ///< GRAPHIC_ENTITY]

        MARQUEE_MESSAGE                     = 73,   ///< Used on supervisor report

        METRIC_RULE_WEIGHT                  = 74,   ///< Used for fixed weight and fluid ounce items. Displays the pounds and metric value
                                                    ///< together; the configured type is printed first. It does a standard conversion
                                                    ///< calculation. [TEXT_ENTITY]

        FORM_SEPARATOR                      = 75,   ///< Used on itemized labels to determine the number of items that can be printed on
                                                    ///< the label; a separator should follow each item.

        DISCOUNT_PRICE                      = 76,   ///< Frequent shopper total price: level 1 [POST_IMAGE, TEXT_ENTITY]

        DISCOUNT_SAVINGS                    = 77,   ///< Frequent shopper savings: level 1 [POST_IMAGE, TEXT_ENTITY]

        DISCOUNT_UNIT_PRICE                 = 78,   ///< Frequent shopper unit price: level 1 [POST_IMAGE, TEXT_ENTITY]

        PORTION_COUNT                       = 79,   ///< This is used to indicate the number of portions contained in the package (not to
                                                    ///< be confused with by count). [PRE_IMAGE, TEXT_ENTITY]

        AVG_PORTION_COST                    = 80,   ///< This is the Total price divided by the portion count to give the average portion
                                                    ///< cost. [POST_IMAGE, TEXT_ENTITY]

        AVG_PORTION_WEIGHT                  = 81,   ///< This is the Net weight or the fixed weight divided by the portion count to give
                                                    ///< the average portion cost. [POST_IMAGE, TEXT_ENTITY]

        AVG_FS_PORTION_COST                 = 82,   ///< Average Frequent Shopper portion cost. This is the Frequent Shopper total price
                                                    ///< (@c DISCOUNT_PRICE) divided by the portion count to give the average Frequent Shopper
                                                    ///< portion cost. [POST_IMAGE, TEXT_ENTITY]

        TOTAL_POINTS                        = 83,   ///< This is used to indicate points earned per package. [PRE_IMAGE, TEXT_ENTITY]

        SPEC_MSG_SHORT                      = 84,   ///< Use first 11 chars of special message. ONLY USED in SP600

        DATE_LINE_TEXT1                     = 85,   ///< Header Text for the date line. ONLY USED in SP600

        DATE_LINE_TEXT2                     = 86,   ///< Header Text for the date line. ONLY USED in SP600

        DATE_LINE_TEXT3                     = 87,   ///< Header Text for the date line. ONLY USED in SP600

        DATE_LINE_FORM1                     = 88,   ///< Field defined for the date line. ONLY USED in SP600

        DATE_LINE_FORM2                     = 89,   ///< Field defined for the date line. ONLY USED in SP600

        DATE_LINE_FORM3                     = 90,   ///< Field defined for the date line. ONLY USED in SP600

        NET_WEIGHT_HEADER                   = 91,   ///< Header prints Net Weight if item is not a by count and Quantity on by counts.
                                                    ///< ONLY USED in SP600

        TOTAL_ITEM_PKGS_FORM                = 92,   ///< Total n packages on an itemized label. ONLY USED in SP600

        ITEM_NUMBER_FORM                    = 93,   ///< Package m of n packages on itemized label. ONLY USED in SP600

        HEADER_TEXT                         = 94,   ///< Identical behavior to User Line. USED in SP600 like generic text

        PRICE_PER_UNIT_WT                   = 95,   ///< This is used to provide a unit price for a Fixed price item. For example, a store
                                                    ///< wants to sell all frozen turkeys for $10 each. They would weigh each turkey and
                                                    ///< the price per unit weight field would take the fixed price divided by the net
                                                    ///< weight to get a 'unit price' for that turkey. [POST_IMAGE, TEXT_ENTITY]

        SEC_UNIT_PRICE                      = 96,   ///< The unit price with the secondary currency symbol based on the exchange rate.
                                                    ///< ONLY USED in Nondomestic firmware [POST_IMAGE, TEXT_ENTITY]

        SEC_TOTAL_PRICE                     = 97,   ///< The total price with the secondary currency symbol based on the secondary unit
                                                    ///< price. ONLY USED in Nondomestic firmware [POST_IMAGE, TEXT_ENTITY]

        EXCHANGE_RATE_FORM                  = 98,   ///< The rate of exchange between currencies. ONLY USED in Nondomestic firmware

        SEC_DISC_UNIT_PRICE                 = 99,   ///< The secondary discounted unit price with the secondary currency symbol based on
                                                    ///< the exchange rate. ONLY USED in Nondomestic firmware [POST_IMAGE, TEXT_ENTITY]

        SEC_DISC_TOT_PRICE                  = 100,  ///< The secondary discounted total price with the secondary currency symbol based on
                                                    ///< the secondary discounted unit price. ONLY USED in Nondomestic firmware [POST_IMAGE,
                                                    ///< TEXT_ENTITY]

        SEC_DISC_SAVINGS                    = 101,  ///< The secondary frequent shopper savings, which when added to the secondary
                                                    ///< discounted total price produces the secondary total price. ONLY USED in
                                                    ///< Nondomestic firmware. [POST_IMAGE, TEXT_ENTITY]

        FORCED_KB_WEIGHT                    = 102,  ///< The net weight (NET_WEIGHT) that is forced by the scale operator. [POST_IMAGE,
                                                    ///< TEXT_ENTITY]

        NUTRI_COLUMN_1                      = 103,  ///< Nutrition Data field whose data comes from the first column of the nutrition
                                                    ///< data record linked to the current product. The tag associated with this form
                                                    ///< indicates which nutrition data element to use. (This field does not apply to
                                                    ///< scales prior to Evolution). [PRE_IMAGE, TEXT_ENTITY]

        NUTRI_COLUMN_2                      = 104,  ///< Nutrition Data field whose data comes from the second column of the nutrition
                                                    ///< data record linked to the current product. The tag associated with this form
                                                    ///< indicates which nutrition data element to use. (This field does not apply to
                                                    ///< scales prior to Evolution). [PRE_IMAGE, TEXT_ENTITY]

        NUTRI_COLUMN_3                      = 105,  ///< Nutrition Data field whose data comes from the third column of the nutrition
                                                    ///< data record linked to the current product. The tag associated with this form
                                                    ///< indicates which nutrition data element to use. (This field does not apply to
                                                    ///< scales prior to Evolution). [PRE_IMAGE, TEXT_ENTITY]

        NUTRI_COLUMN_4                      = 106,  ///< Nutrition Data field whose data comes from the fourth column of the nutrition
                                                    ///< data record linked to the current product. The tag associated with this form
                                                    ///< indicates which nutrition data element to use. (This field does not apply to
                                                    ///< scales prior to Evolution). [PRE_IMAGE, TEXT_ENTITY]

        DISCOUNT_TOTAL_PRICE2               = 107,  ///< Frequent shopper total price: level 2 [POST_IMAGE, TEXT_ENTITY]

        DISCOUNT_SAVINGS2                   = 108,  ///< Frequent shopper savings: level 2 [POST_IMAGE, TEXT_ENTITY]

        DISCOUNT_UNIT_PRICE2                = 109,  ///< Frequent shopper unit price: level 2 [POST_IMAGE, TEXT_ENTITY]

        DISCOUNT_TOTAL_PRICE3               = 110,  ///< Frequent shopper total price: level 3 [POST_IMAGE, TEXT_ENTITY]

        DISCOUNT_SAVINGS3                   = 111,  ///< Frequent shopper savings: level 3 [POST_IMAGE, TEXT_ENTITY]

        DISCOUNT_UNIT_PRICE3                = 112,  ///< Frequent shopper unit price: level 3 [POST_IMAGE, TEXT_ENTITY]

        COOL_TEXT                           = 113,  ///< Country of Origin text field [PRE_IMAGE, TEXT_ENTITY]

        TRACKING_TEXT                       = 114,  ///< Country of Origin Tracking # field [PRE_IMAGE, TEXT_ENTITY]

        USER_GRAPHIC_2                      = 115,  ///< Graphic 2 Linked to the Product (only on Evolution) [PRE_IMAGE, GRAPHIC _ENTITY]

        USER_GRAPHIC_3                      = 116,  ///< Graphic 3 Linked to the Product (only on Evolution) [PRE_IMAGE, GRAPHIC _ENTITY]

        USER_GRAPHIC_4                      = 117,  ///< Graphic 4 Linked to the Product (only on Evolution) [PRE_IMAGE, GRAPHIC _ENTITY]

        REPORT_STREAM                       = 118,  ///< Report stream with contents of report (only in Evolution) [POST_IMAGE,
                                                    ///< TEXT_ENTITY]

        REPORT_PAGE_NUMBER                  = 119,  ///< Page number (label number) in a report printed on multiple labels (only on
                                                    ///< Evolution) [POST_IMAGE, TEXT_ENTITY]

        SELL_BY_TIME          		        = 120,  ///< This field displays the products shelf life  time. For machines modified to
                                                    ///< display shelf life using date and time. [POST_IMAGE,TEXT_ENTITY]

        BEST_BEFORE_TIME       		        = 121,  ///< This field displays the product life time. For machines modified to display
                                                    ///< product life using date and time. [POST_IMAGE,TEXT_ENTITY]

        PACKED_ON_TIME              	    = 122,  ///< This field displays the products packed on time. For machines modified to
                                                    ///< display packed on using date and time [POST_IMAGE,TYEXT_ENTITY]

        SELL_BY_DAY_OF_WEEK                 = 123,  ///< This field displays the day of the week for the shelf life date.  This field is
                                                    ///< new for the shelf life expiration label feature of the HLX. [POST_IMAGE,
                                                    ///< TEXT_ENTITY]

        TRANSACTION_STATUS                  = 124,   ///< This field displays the transaction status on a label. Print R on the label for Rewrap
                                                    ///< transaction type, prints M when the price changes for this PLU

        CUSTOMER_LABEL_VERSION_NUM          = 125,   ///< Customer version number.

        CUSTOMER_NAME                       = 157,

        ORDER_NUMBER                        = 158,

        PICKUP_TIME                         = 159,

        // add new types above this placeholder
        _MAX_TYPES                          = 160


    };

    /**
     * Convert to string.
     *
     * @param[in] value  label form type
     * @returns  label form type in string form
     */
    static QString toString( type value );

    /**
     * Convert to mime data.
     *
     * Mime data is used for drag and drop.
     *
     * @param[in] value  label form type
     * @returns  label form type in mime data form
     */
    static QMimeData *toMimeData( type value );

    /**
     * Convert to pixmap.
     *
     * This pixmap is best suited for use in drag and drop operations.
     *
     * @param[in] value  label form type
     * @returns  label form type in pixmap form
     */
    static QPixmap toPixmap( type value );

    /**
     * Checks if @p value is a type of @c labelFormType::type.
     *
     * @param[in] value  object to check
     * @param[out] valueType  type @p value is
     * @returns  @c true if @p value is a @c labelFormType::type, @c false otherwise
     */
    static bool isTypeOfMimeData( const QMimeData *value, type *valueType = NULL );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // LABELFORMTYPE_H
