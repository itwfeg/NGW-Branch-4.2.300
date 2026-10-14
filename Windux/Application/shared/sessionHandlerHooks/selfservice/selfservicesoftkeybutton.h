/**
 * @file selfservicesoftkeybutton.h
 * Softkey button widget for self service.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SELFSERVICESOFTKEYBUTTON_H
#define SELFSERVICESOFTKEYBUTTON_H

#include "softkeybutton.h"

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class selfServiceSoftkeyButton
 * @brief  Softkey button widget for self service.
 */
class selfServiceSoftkeyButton : public softkeyButton
{
    Q_OBJECT

    typedef selfServiceSoftkeyButton _Myt;
    typedef softkeyButton _Mybase;

public:

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit selfServiceSoftkeyButton(
        QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~selfServiceSoftkeyButton();

    // ************************************************************************
    // Properties
    // ************************************************************************

    /**
     * Set background image.
     *
     * @param[in] value  image
     */
    virtual void setBackgroundImage( const QString& value );

    /**
     * Set text alignment.
     *
     * @param[in] value  alignment
     */
    virtual void setTextAlignment( Qt::Alignment value );

protected:

    // ************************************************************************
    // Events
    // ************************************************************************

    /**
     * Paint event.
     *
     * @param[in,out] e  paint event
     */
    virtual void paintEvent( QPaintEvent *e );

    // ************************************************************************
    // Methods
    // ************************************************************************

    /**
     * Update button images.
     */
    virtual void updatePixmaps();

private:

    QString backgroundImageOrig_;
    double backgroundImageOrigSize_;

    static const Qt::Alignment buttonTextAlignment_;
    static const QColor buttonTextColor_;

    static const int buttonFontPointSize_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class selfServiceSoftkeyButtonFactory
 * @brief Factory class for softkey button widgets in self service mode.
 */
class selfServiceSoftkeyButtonFactory : public softkeyButtonFactory
{
    typedef selfServiceSoftkeyButtonFactory _Myt;
    typedef softkeyButtonFactory _Mybase;

public:

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * Constructor.
     */
    explicit selfServiceSoftkeyButtonFactory() : _Mybase() {}

    // ************************************************************************
    // Methods
    // ************************************************************************

    /**
     * Create new @c softkeyButton object.
     *
     * @param[in] parent  parent widget
     */
    virtual softkeyButton *create( QWidget *parent = 0 ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELFSERVICESOFTKEYBUTTON_H
