/**
 * @file selfserviceicons.h
 * Self service icon widgets.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup selfServeCsu
 * @{
 */

#ifndef SELFSERVICEICONS_H
#define SELFSERVICEICONS_H

#include <capacitybarwidgets.h>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class selfServiceWirelessSignalIcon
 * @brief Wireless signal icon for self service.
 */
class selfServiceWirelessSignalIcon : public wirelessSignalIcon
{
    Q_OBJECT

    typedef selfServiceWirelessSignalIcon _Myt;
    typedef wirelessSignalIcon _Mybase;

public:

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * @brief  Constructor.
     * @param[in] s  icon size
     * @param[in] parent  parent widget
     */
    explicit selfServiceWirelessSignalIcon( const QSize& s, QWidget *parent = 0 );

    /// Destructor.
    virtual ~selfServiceWirelessSignalIcon();

    // ************************************************************************
    // Properties
    // ************************************************************************

    /**
     * @brief  Size hint.
     * @return  size
     */
    virtual QSize sizeHint() const;

protected:

    // ************************************************************************
    // Properties
    // ************************************************************************

    /**
     * @brief  Set alert icon pixmap.
     * @param[in] value  pixmap
     */
    virtual void setAlertPixmap( const QPixmap& value );

    /**
     * @brief  Set icon pixmap.
     * @param[in] value  pixmap
     */
    virtual void setPixmap( const QPixmap& value );

private:

    QSize iconSize_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELFSERVICEICONS_H
