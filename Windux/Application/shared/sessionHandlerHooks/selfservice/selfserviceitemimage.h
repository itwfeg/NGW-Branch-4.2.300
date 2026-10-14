/**
 * @file selfserviceitemimage.h
 * Self service widget for showing the item image and other details.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup selfServeCsu
 * @{
 */

#ifndef SELFSERVICEITEMIMAGE_H
#define SELFSERVICEITEMIMAGE_H

#include <QWidget>

class QLabel;

class ScaleConfiguration;

class selfServiceHook;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Self service widget for showing the item image and other details.
class selfServiceItemImage : public QWidget
{
    Q_OBJECT

    typedef selfServiceItemImage _Myt;
    typedef QWidget _Mybase;

public:

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * @brief  Constructor.
     * @param[in] parent  parent widget
     */
    explicit selfServiceItemImage( QWidget *parent = NULL );

    /// Destructor.
    virtual ~selfServiceItemImage();

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

    QLabel *itemExpandedText_;

    QLabel *fsItemUnitPriceLabel_;
    QLabel *fsItemUnitPrice_;

    QLabel *fsItemSavingsLabel_;
    QLabel *fsItemSavings_;

    QLabel *fsItemTotalPriceLabel_;
    QLabel *fsItemTotalPrice_;

    QStringList rando_bgs_;
    QStringList systemFolders_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Update images.
    void updateImages();

    QString makeImage( const QString& filename ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SELFSERVICEITEMIMAGE_H
