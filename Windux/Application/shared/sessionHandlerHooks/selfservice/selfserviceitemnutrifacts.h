/**
 * @file selfserviceitemnutrifacts.h
 * Self service widget for showing nutrifacts.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup selfServeCsu
 * @{
 */

#ifndef SELFSERVICEITEMNUTRIFACTS_H
#define SELFSERVICEITEMNUTRIFACTS_H

#include <QWidget>

class selfServiceHook;

class ScaleConfiguration;

class QLabel;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Self service widget for showing nutrifacts.
class selfServiceItemNutrifacts : public QWidget
{
    Q_OBJECT

    typedef selfServiceItemNutrifacts _Myt;
    typedef QWidget _Mybase;

public:

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * @brief  Constructor.
     * @param[in] parent  parent widget
     */
    explicit selfServiceItemNutrifacts( QWidget *parent = NULL );

    /// Destructor.
    virtual ~selfServiceItemNutrifacts();

    // ************************************************************************
    // Properties
    // ************************************************************************

    /**
     * @brief  Check if frequent shopper is displayed.
     * @return  @c true if displayed, @c false otherwise
     */
    virtual bool displayFreqShopper() const {return fs_;}

    /**
     * @brief  Set if frequent shopper is displayed or not.
     * @param[in]  @c true to display, @c false otherwise
     */
    virtual void setDisplayFreqShopper( bool value ) {fs_ = value;}

    /**
     * @brief  Set self service hook
     * @param[in] value  pointer to hook
     */
    virtual void setSelfServiceHook( selfServiceHook *value ) {hook_ = value;}

    // ************************************************************************
    // Methods
    // ************************************************************************

    /// Refresh widget.
    virtual void refresh();

    /// Translate the user interface to current language.
    virtual void translate();

protected:

    // ************************************************************************
    // Events
    // ************************************************************************

    /**
     * @brief  Resize event.
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

private:

    selfServiceHook *hook_;

    ScaleConfiguration *scaleConfig_;

    bool fs_;

    QLabel *fsItemUnitPriceLabel_;
    QLabel *fsItemUnitPrice_;

    QLabel *fsItemSavingsLabel_;
    QLabel *fsItemSavings_;

    QLabel *fsItemTotalPriceLabel_;
    QLabel *fsItemTotalPrice_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Update images.
    void updateImages();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SELFSERVICEITEMNUTRIFACTS_H
