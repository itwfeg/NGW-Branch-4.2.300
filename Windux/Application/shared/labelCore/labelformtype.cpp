/**
 * @file labelformtype.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "labelform.h"
#include "labelformtype.h"

#include <QPainter>

static const QString MIME_DATA_PREFIX( "[hobart.label.form.type]" );

///////////////////////////////////////////////////////////////////////////////////////////////////
QString labelFormType::toString( type value )
{
    switch ( value )
    {
    case BARCODE:
        return "Barcode";
    case DESCRIPTION:
        return "Description";
    case NUTRIFACTS:
        return "Nutrifacts";
    case EXPANDED_TEXT_1:
        return "Expanded Text";
    case EXPANDED_TEXT_2:
        return "Expanded Text 2";
    case SPECIAL_MESSAGE:
        return "Special Message";
    case COUPON_TEXT:
        return "Coupon Text";
    case PACKED_ON:
        return "Packed On";
    case SELL_BY:
        return "Sell By";
    case BEST_BEFORE:
        return "Best Before";
    case NET_WEIGHT:
        return "Net Weight";
    case FIXED_WEIGHT:
        return "Fixed Weight";
    case TARE:
        return "Tare";
    case UNIT_PRICE:
        return "Unit Price";
    case TOTAL_PRICE:
        return "Total Price";
    case STORE_NAME:
        return "Store Name";
    case STORE_NUMBER:
        return "Store Number";
    case OPERATOR_NUMBER:
        return "Operator Number";
    case OPERATOR_NAME:
        return "Operator Name";
    case USER_LINE:
        return "User Line";
    case USER_GRAPHIC:
        return "Product Graphic";
    case STORE_GRAPHIC:
        return "Store Graphic";
    case GENERIC_TEXT_1:
        return "Generic Text 1";
    case GENERIC_TEXT_2:
        return "Generic Text 2";
    case GENERIC_TEXT_3:
        return "Generic Text 3";
    case GENERIC_TEXT_4:
        return "Generic Text 4";
    case GENERIC_TEXT_5:
        return "Generic Text 5";
    case GENERIC_TEXT_6:
        return "Generic Text 6";
    case GENERIC_TEXT_7:
        return "Generic Text 7";
    case GENERIC_TEXT_8:
        return "Generic Text 8";
    case GENERIC_TEXT_9:
        return "Generic Text 9";
    case GENERIC_TEXT_10:
        return "Generic Text 10";
    case PRODUCT_NUMBER:
        return "Product Number";
    case REPORTING_CLASS:
        return "Reporting Class";
    case TOTAL_WEIGHT:
        return "Total Weight";
    case TOTAL_VALUE:
        return "Total Value";
    case TOTAL_PACKAGES:
        return "Total Packages";
    case TOTAL_RUNS:
        return "Total Runs";
    case TOTAL_TYPE:
        return "Total Type";
    case TOTAL_TYPE_NAME:
        return "Total Type Name";
    case TOTAL_HOURS:
        return "Total Hours";
    case UPC_NUMBER:
        return "UPC Number";
    case OP_ACCESS_LEVEL:
        return "Operator Access Level";
    case PRICE_FORM:
        return "Price Form";
    case WEIGHT_TYPE_FORM:
        return "Weight Type Form";
    case TEXT_FORM:
        return "Text Form";
    case SHELF_LIFE_FORM:
        return "Shelf Life Form";
    case LINE_FORM:
        return "Line Form";
    case PRODUCT_LIST:
        return "Product List";
    case SPECIAL_TEXT:
        return "Special Text";
    case FIXED_TEXT:
        return "Fixed Text";
    case CURRENT_TIME:
        return "Current Time";
    case TOTAL_TYPE_NUMBER:
        return "Total Type Number";
    case INDIRECT_LABEL_REFERENCE:
        return "Indirect Label Reference";
    case NUTRIFACT_TEXT_1:
        return "Nutrifact Text 1";
    case NUTRIFACT_TEXT_2:
        return "Nutrifact Text 2";
    case NUTRIFACT_VALUE_1:
        return "Nutrifact Value 1";
    case NUTRIFACT_VALUE_2:
        return "Nutrifact Value 2";
    case NUTRIFACT_VALUE_3:
        return "Nutrifact Value 3";
    case DIVIDING_LINE:
        return "Dividing Line";
    case TOTAL_ITEMS:
        return "Total Items";
    case CONSTANT_TEXT:
        return "Constant Text";
    case GENERIC_TEXT_11:
        return "Generic Text 11";
    case GENERIC_TEXT_12:
        return "Generic Text 12";
    case GENERIC_TEXT_13:
        return "Generic Text 13";
    case GENERIC_TEXT_14:
        return "Generic Text 14";
    case GENERIC_TEXT_15:
        return "Generic Text 15";
    case GENERIC_TEXT_16:
        return "Generic Text 16";
    case GENERIC_TEXT_17:
        return "Generic Text 17";
    case GENERIC_TEXT_18:
        return "Generic Text 18";
    case GENERIC_TEXT_19:
        return "Generic Text 19";
    case GENERIC_TEXT_20:
        return "Generic Text 20";
    case INDIRECT_GRAPHIC_REFERENCE:
        return "Indirect Graphic Reference";
    case MARQUEE_MESSAGE:
        return "Marquee Message";
    case METRIC_RULE_WEIGHT:
        return "Metric Rule Weight";
    case FORM_SEPARATOR:
        return "Form Separator";
    case DISCOUNT_PRICE:
        return "Discount Price";
    case DISCOUNT_SAVINGS:
        return "Discount Savings";
    case DISCOUNT_UNIT_PRICE:
        return "Discount Unit Price";
    case PORTION_COUNT:
        return "Portion Count";
    case AVG_PORTION_COST:
        return "Average Portion Cost";
    case AVG_PORTION_WEIGHT:
        return "Average Portion Weight";
    case AVG_FS_PORTION_COST:
        return "Average Frequent Shopper Portion Cost";
    case TOTAL_POINTS:
        return "Total Points";
    case SPEC_MSG_SHORT:
        return "Special Message Short";
    case DATE_LINE_TEXT1:
        return "Date Line Text 1";
    case DATE_LINE_TEXT2:
        return "Date Line Text 2";
    case DATE_LINE_TEXT3:
        return "Date Line Text 3";
    case DATE_LINE_FORM1:
        return "Date Line Form 1";
    case DATE_LINE_FORM2:
        return "Date Line Form 2";
    case DATE_LINE_FORM3:
        return "Date Line Form 3";
    case NET_WEIGHT_HEADER:
        return "Barcode";
    case TOTAL_ITEM_PKGS_FORM:
        return "Total Item Packages Form";
    case ITEM_NUMBER_FORM:
        return "Item Number Form";
    case HEADER_TEXT:
        return "Header Text";
    case PRICE_PER_UNIT_WT:
        return "Price Per Unit Weight";
    case SEC_UNIT_PRICE:
        return "Secondary Unit Price";
    case SEC_TOTAL_PRICE:
        return "Secondary Total Price";
    case EXCHANGE_RATE_FORM:
        return "Exchange Rate Form";
    case SEC_DISC_UNIT_PRICE:
        return "Secondary Discount Unit Price";
    case SEC_DISC_TOT_PRICE:
        return "Secondary Discount Total Price";
    case SEC_DISC_SAVINGS:
        return "Secondary Discount Savings";
    case FORCED_KB_WEIGHT:
        return "Keyboard Net Weight";
    case NUTRI_COLUMN_1:
        return "Nutridata Column 1";
    case NUTRI_COLUMN_2:
        return "Nutridata Column 2";
    case NUTRI_COLUMN_3:
        return "Nutridata Column 3";
    case NUTRI_COLUMN_4:
        return "Nutridata Column 4";
    case DISCOUNT_TOTAL_PRICE2:
        return "Discount Price Level 2";
    case DISCOUNT_SAVINGS2:
        return "Discount Savings Level 2";
    case DISCOUNT_UNIT_PRICE2:
        return "Discount Unit Price Level 2";
    case DISCOUNT_TOTAL_PRICE3:
        return "Discount Price Level 3";
    case DISCOUNT_SAVINGS3:
        return "Discount Savings Level 3";
    case DISCOUNT_UNIT_PRICE3:
        return "Discount Unit Price Level 3";
    case COOL_TEXT:
        return "COOL Text";
    case TRACKING_TEXT:
        return "COOL Tracking Text";
    case USER_GRAPHIC_2:
        return "Product Graphic 2";
    case USER_GRAPHIC_3:
        return "Product Graphic 3";
    case USER_GRAPHIC_4:
        return "Product Graphic 4";
    case REPORT_STREAM:
        return "Report Data";
    case REPORT_PAGE_NUMBER:
        return "Report Page #";
    case SELL_BY_TIME:
        return "Sell by time";
    case BEST_BEFORE_TIME:
        return "Best before time";
    case PACKED_ON_TIME:
        return "Packed on time";
    case SELL_BY_DAY_OF_WEEK:
        return "Sell by day of week";
    case TRANSACTION_STATUS:
        return "Transaction status";
    case CUSTOMER_LABEL_VERSION_NUM:
        return "Customer version number";
    case CUSTOMER_NAME:
        return "Customer name";
    case ORDER_NUMBER:
        return "Order number";
    case PICKUP_TIME:
        return "Pick-up time";
    default:
        break;
    }

    return "[Unknown]";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QMimeData *labelFormType::toMimeData( type value )
{
    QString text( MIME_DATA_PREFIX + "%1" );

    QMimeData *mimeData = new QMimeData;
    mimeData->setText( text.arg( value ) );

    return mimeData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPixmap labelFormType::toPixmap( type value )
{
    QSize size( labelForm::defaultSize() );
    QRect rect( QPoint( 0, 0 ), size );

    QPixmap pixmap( QSize( size.width() + 1, size.height() + 1 ) );
    pixmap.fill( Qt::white );

    QPainter painter( &pixmap );
    painter.setPen( QPen( Qt::black ) );
    painter.setBrush( QBrush( Qt::white ) );

    painter.drawRect( rect );
    painter.drawText( rect, Qt::AlignLeft | Qt::AlignTop, toString( value ) );

    return pixmap;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool labelFormType::isTypeOfMimeData( const QMimeData *value, type *valueType )
{
    if (( value ) && ( value->hasText() ))
    {
        QString data( value->text() );

        if ( data.contains( MIME_DATA_PREFIX ) )
        {
            type t = (type) data.replace( MIME_DATA_PREFIX, "" ).toInt();

            if (( 0 <= t ) && ( t < _MAX_TYPES ))
            {
                if ( valueType )
                    *valueType = t;

                return true;
            }
        }
    }

    return false;
}
