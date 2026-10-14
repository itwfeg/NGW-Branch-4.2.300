/**
 * @file selfservicetoggledsoftkeybutton.h
 * Toggled softkey button widget for self service.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SELFSERVICETOGGLEDSOFTKEYBUTTON_H
#define SELFSERVICETOGGLEDSOFTKEYBUTTON_H

#include "softkeybutton.h"

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Toggled softkey button widget for self service.
class selfServiceToggledSoftkeyButton : public softkeyButton
{
    Q_OBJECT

    typedef selfServiceToggledSoftkeyButton _Myt;
    typedef softkeyButton _Mybase;

public:

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * @brief Constructor.
     * @param[in] parent  parent widget
     */
    explicit selfServiceToggledSoftkeyButton(
        QWidget *parent = 0 );

    /// Destructor.
    virtual ~selfServiceToggledSoftkeyButton();

    // ************************************************************************
    // Properties
    // ************************************************************************

    /**
     * @brief  Set text alignment.
     * @param[in] value  alignment
     */
    virtual void setTextAlignment( Qt::Alignment value );

protected:

    // ************************************************************************
    // Events
    // ************************************************************************

    /**
     * @brief  Paint event.
     * @param[in,out] e  paint event
     */
    virtual void paintEvent( QPaintEvent *e );

    // ************************************************************************
    // Methods
    // ************************************************************************

    /// Update button images.
    virtual void updatePixmaps();

private:

    static const Qt::Alignment buttonTextAlignment_;

    static const QColor buttonTextColor_;
    static const QColor buttonTextColorToggled_;

    static const int buttonFontPointSize_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Factory class for toggld softkey button widgets in self service mode.
class selfServiceToggledSoftkeyButtonFactory : public softkeyButtonFactory
{
    typedef selfServiceToggledSoftkeyButtonFactory _Myt;
    typedef softkeyButtonFactory _Mybase;

public:

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /// Constructor.
    explicit selfServiceToggledSoftkeyButtonFactory() : _Mybase() {}

    // ************************************************************************
    // Methods
    // ************************************************************************

    /**
     * @brief  Create new @c softkeyButton object.
     * @param[in] parent  parent widget
     */
    virtual softkeyButton *create( QWidget *parent = 0 ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELFSERVICETOGGLEDSOFTKEYBUTTON_H
