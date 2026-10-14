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

#ifndef SELFSERVICESOFTKEYBUTTONG2_H
#define SELFSERVICESOFTKEYBUTTONG2_H

#include "softkeybutton.h"
#include <QStringList>

class ScaleConfiguration;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class selfServiceSoftkeyButton
 * @brief  Softkey button widget for self service.
 */
class selfServiceSoftkeyButtonG2 : public softkeyButton
{
    Q_OBJECT

    typedef selfServiceSoftkeyButtonG2 _Myt;
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
    explicit selfServiceSoftkeyButtonG2(
        QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~selfServiceSoftkeyButtonG2();

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

    /**
     * set if we are using the new GEN2 User Interface
     *
     * @param[in] value  gen2Active
     */
    virtual void setGen2ActiveUI( bool );

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

    ScaleConfiguration *scaleConfig_;

    QStringList rando_bgs_;

    QString backgroundImageOrig_;
    double backgroundImageOrigSize_;
    bool isGen2UI_;

    static const Qt::Alignment buttonTextAlignment_;
    static const QColor buttonTextColor_;

    static const int buttonFontPointSize_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class selfServiceSoftkeyButtonFactoryG2
 * @brief Factory class for softkey button widgets in self service mode.
 */
class selfServiceSoftkeyButtonFactoryG2 : public softkeyButtonFactory
{
    typedef selfServiceSoftkeyButtonFactoryG2 _Myt;
    typedef softkeyButtonFactory _Mybase;

public:

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * Constructor.
     */
    explicit selfServiceSoftkeyButtonFactoryG2() : _Mybase() {}

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

#endif // SELFSERVICESOFTKEYBUTTONG2_H
