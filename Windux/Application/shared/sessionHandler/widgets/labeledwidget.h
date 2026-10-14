/**
 * @file labeledwidget.h
 * Base class generic container widget with custom background, highlighting, and embedded child
 * widget.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LABELEDWIDGET_H
#define LABELEDWIDGET_H

#include "../sessioninfo.h"

#include "../modes/modewidget.h"

#include <backgroundwidget.h>
#include <pixmap.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class labeledWidget
 * @brief Base class generic container widget with custom background, highlighting, and embedded
 * child widget.
 *
 * This class is a base class for many of the custom widgets found in operate mode.
 *
 * You will find below that this class contains a lot of layouts. This is to offer those who derive
 * from this class the most flexibility in where they put their widgets. Below is a diagram of
 * how the layouts are nested.
 *
 * @code
 * +-[ screenLayout_ (V) ]------------------------------------------------------------------------+
 * |+-[ screenLayoutInner_ (H) ]-----------------------------------------------------------------+|
 * ||+-[ widgetsLayout_ (V) ]-------------------------------------------------------------------+||
 * |||                                                                                          |||
 * ||| +-[ topLayout_ (H) ]-------------------------------------------------------------------+ |||
 * ||| |+-[ topLayoutInner_ (V) ]------------------------------------------------------------+| |||
 * ||| ||                                                                                    || |||
 * ||| || label_                                                                             || |||
 * ||| ||                                                                                    || |||
 * ||| |+------------------------------------------------------------------------------------+| |||
 * ||| +--------------------------------------------------------------------------------------+ |||
 * |||                                                                                          |||
 * ||| STRETCH                                                                                  |||
 * |||                                                                                          |||
 * ||| +-[ bottomLayout_ (H) ]----------------------------------------------------------------+ |||
 * ||| |+-[ bottomLayoutInner_ (V) ]---------------------------------------------------------+| |||
 * ||| ||                                                                                    || |||
 * ||| || child_                                                                             || |||
 * ||| ||                                                                                    || |||
 * ||| |+------------------------------------------------------------------------------------+| |||
 * ||| +--------------------------------------------------------------------------------------+ |||
 * |||                                                                                          |||
 * ||+------------------------------------------------------------------------------------------+||
 * |+--------------------------------------------------------------------------------------------+|
 * +----------------------------------------------------------------------------------------------+
 *
 * @tparam WIDGET  child widget type
 */
template <class WIDGET>
class labeledWidget : public backgroundWidget<QWidget>
{
    typedef labeledWidget _Myt;
    typedef backgroundWidget<QWidget> _Mybase;

    typedef WIDGET _ChildWidgetType;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     */
    explicit labeledWidget(
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /**
     * Constructor.
     *
     * @param[in] text  label text
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     */
    explicit labeledWidget(
        const QString& text,
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /**
     * Destructor.
     */
    virtual ~labeledWidget();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve access level.
     *
     * @returns  operator access level
     */
    virtual int accessLevel() const;

    /**
     * Retrieve background.
     *
     * @param[in] forced  @c true to retrieve forced pixmap, @c false otherwise
     * @returns  background pixmap
     */
    virtual QPixmap background( bool forced = false ) const;

    /**
     * Retrieve background for focused state.
     *
     * @param[in] forced  @c true to retrieve forced pixmap, @c false otherwise
     * @returns  background pixmap
     */
    virtual QPixmap backgroundFocused( bool forced = false ) const;

    /**
     * Retrieve background for read only state.
     *
     * @param[in] forced  @c true to retrieve forced pixmap, @c false otherwise
     * @returns  background pixmap
     */
    virtual QPixmap backgroundReadOnly( bool forced = false ) const;

    /**
     * Retrieve contents margins.
     *
     * @returns  margins
     */
    virtual QMargins contentsMargins() const;

    /**
     * Check if widget has focus.
     *
     * @returns  @c true if focused, @c false otherwise
     */
    virtual bool hasFocus() const;

    /**
     * Retrieve forced state.
     *
     * @returns  @c true if forced, @c false otherwise
     */
    virtual bool isForced() const;

    /**
     * Retrieve label visibility.
     *
     * @returns  @c true if label is visible, @c false otherwise
     */
    virtual bool isLabelVisible() const;

    /**
     * Check if widget is read only.
     *
     * @returns  @c true if read only, @c false otherwise
     */
    virtual bool isReadOnly() const;

    /**
     * Retrieve text alignment of label.
     *
     * @returns  alignment
     */
    virtual Qt::Alignment labelAlignment() const;

    /**
     * Retrieve font of label.
     *
     * @returns  font
     */
    virtual QFont labelFont() const;

    /**
     * Retrieve label text.
     *
     * @returns  label text value
     */
    virtual QString labelText() const;

    /**
     * Set access level.
     *
     * @param[in]  operator access level
     */
    virtual void setAccessLevel( int value );

    /**
     * Set the background.
     *
     * @param[in] value  background pixmap
     * @param[in] forced  @c true to set forced pixmap, @c false otherwise
     */
    virtual void setBackground( const QPixmap& value, bool forced = false );

    /**
     * Set the background for focused state.
     *
     * @param[in] value  background pixmap
     * @param[in] forced  @c true to set forced pixmap, @c false otherwise
     */
    virtual void setBackgroundFocused( const QPixmap& value, bool forced = false );

    /**
     * Set the background for read only state.
     *
     * @param[in] value  background pixmap
     * @param[in] forced  @c true to set forced pixmap, @c false otherwise
     */
    virtual void setBackgroundReadOnly( const QPixmap& value, bool forced = false );

    /**
     * Set contents margins.
     *
     * @param[in] value  margins
     */
    virtual void setContentsMargins( const QMargins& value );

    /**
     * Set if control is forced.
     *
     * @param[in] value  @c true for forced, @c false for otherwise
     */
    virtual void setForced( bool value );

    /**
     * Set label alignment.
     *
     * @param[in] value  alignment
     */
    virtual void setLabelAlignment( Qt::Alignment value );

    /**
     * Set font of label.
     *
     * @param[in] value  font
     */
    virtual void setLabelFont( const QFont& value );

    /**
     * Set label text.
     *
     * @param[in] value  text value to set
     */
    virtual void setLabelText( const QString& value );

    /**
     * Set label object name.
     *
     * @param[in] value  text value to set
     */
    virtual void setLabelObjectName( const QString& value );

    /**
     * Set label visibility.
     *
     * @param[in] value  @c true to make label visible, @c false otherwise
     */
    virtual void setLabelVisible( bool value );

    /**
     * Set if control is read only.
     *
     * @param[in] value  @c true for read only, @c false for otherwise
     */
    virtual void setReadOnly( bool value );

    /**
     * Set toggle button is enabled for this control.
     *
     * @param[in]  @c true if available, @c false otherwise
     */
    virtual void setToggleButtonEnabled( bool value );

    /**
     * Set toggle button is visible for this control.
     *
     * @param[in]  @c true if available, @c false otherwise
     */
    virtual void setToggleButtonVisible( bool value );

    /**
     * Set if widget is available to operator.
     *
     * @param[in]  @c true if available, @c false otherwise
     */
    virtual void setWidgetAvailable( bool value );

    /**
     * Check if toggle button is enabled for this control.
     *
     * @returns  @c true if available, @c false otherwise
     */
    virtual bool toggleButtonEnabled() const;

    /**
     * Check if toggle button is visible for this control.
     *
     * @returns  @c true if available, @c false otherwise
     */
    virtual bool toggleButtonVisible() const;

    /**
     * Check if widget is available to operator.
     *
     * @returns  @c true if available, @c false otherwise
     */
    virtual bool widgetAvailable() const;

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Moves the label from the top of the control to the left side.
     */
    virtual void adjustLabelOrientationLeft();

    /**
     * Adds a stretch to bottom of control to center the child widget.
     */
    virtual void adjustChildCenter();

    /**
     * Check widget access level versus operator access level.
     *
     * @returns @c true if operator can access the widget, @c false otherwise
     */
    virtual bool checkOperatorAccessLevel() const;

    /**
     * Flips the control upside down.
     */
    virtual void flip();

    /**
     * Translate the widget fields.
     */
    virtual void translate();

protected:

    QPushButton *toggleButton_;                 ///< Toggle available button

    QHBoxLayout *screenLayoutInner_;            ///< Screen layout, contains @p widgetsLayout_

    QVBoxLayout *widgetsLayout_;                ///< Contains top and bottom layouts

    QHBoxLayout *topLayout_;                    ///< Top layout, contains @p topLayoutInner_
    QVBoxLayout *topLayoutInner_;               ///< Contains @p label_

    QHBoxLayout *bottomLayout_;                 ///< Bottom layout, contains @p bottomLayoutInner_
    QVBoxLayout *bottomLayoutInner_;            ///< Contains @p child_

    _ChildWidgetType *child_;                   ///< Child widget.

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Mouse press event.
     *
     * @param[in,out] e  mouse event
     */
    virtual void mousePressEvent( QMouseEvent *e );

    /**
     * Resize event.
     *
     * @param[in,out] e  resize event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /**
     * Update background palette for current state.
     */
    virtual void updateBackgroundPalette();

private:

    QVBoxLayout *screenLayout_;

    QLabel *label_;

    int accessLevel_;

    QPixmap backgroundReadOnly_;
    QPixmap backgroundFocused_;

    QPixmap forcedBackground_;
    QPixmap forcedBackgroundReadOnly_;
    QPixmap forcedBackgroundFocused_;
    bool forced_;

    bool toggleButtonEnabled_;
    bool toggleButtonVisible_;
    bool widgetAvailable_;

    /**
     * Initialize.
     */
    void initialize();

};

template <class WIDGET>
labeledWidget<WIDGET>::labeledWidget( QWidget *parent, const QPixmap& image ) :
    _Mybase( image, parent ),
    accessLevel_( 0 ),
    forced_( false ),
    toggleButtonEnabled_( false ),
    toggleButtonVisible_( false ),
    widgetAvailable_( true )
{
    // initialize
    initialize();
}

template <class WIDGET>
labeledWidget<WIDGET>::labeledWidget( const QString& text, QWidget *parent, const QPixmap& image ) :
    _Mybase( image, parent ),
    accessLevel_( 0 ),
    forced_( false ),
    toggleButtonEnabled_( false ),
    toggleButtonVisible_( false ),
    widgetAvailable_( true )
{
    // initialize
    initialize();

    // set label text
    setLabelText( text );
}

template <class WIDGET>
labeledWidget<WIDGET>::~labeledWidget()
{
}

template <class WIDGET>
int labeledWidget<WIDGET>::accessLevel() const
{
    return accessLevel_;
}

template <class WIDGET>
QPixmap labeledWidget<WIDGET>::background( bool forced ) const
{
    if ( forced )
        return forcedBackground_;

    return _Mybase::background();
}

template <class WIDGET>
QPixmap labeledWidget<WIDGET>::backgroundFocused( bool forced ) const
{
    if ( forced )
        return forcedBackgroundFocused_;

    return backgroundFocused_;
}

template <class WIDGET>
QPixmap labeledWidget<WIDGET>::backgroundReadOnly( bool forced ) const
{
    if ( forced )
        return forcedBackgroundReadOnly_;

    return backgroundReadOnly_;
}

template <class WIDGET>
QMargins labeledWidget<WIDGET>::contentsMargins() const
{
    return screenLayout_->contentsMargins();
}

template <class WIDGET>
bool labeledWidget<WIDGET>::isForced() const
{
    return forced_;
}

template <class WIDGET>
bool labeledWidget<WIDGET>::isLabelVisible() const
{
    return label_->isVisible();
}

template <class WIDGET>
Qt::Alignment labeledWidget<WIDGET>::labelAlignment() const
{
    return label_->alignment();
}

template <class WIDGET>
QFont labeledWidget<WIDGET>::labelFont() const
{
    return label_->font();
}

template <class WIDGET>
QString labeledWidget<WIDGET>::labelText() const
{
    return label_->text();
}

template <class WIDGET>
void labeledWidget<WIDGET>::setAccessLevel( int value )
{
    accessLevel_ = value;
}

template <class WIDGET>
void labeledWidget<WIDGET>::setBackground( const QPixmap& value, bool forced )
{
    if ( forced )
        forcedBackground_ = value;
    else
    {
        _Mybase::setBackground( value );
    }

    updateBackgroundPalette();
}

template <class WIDGET>
void labeledWidget<WIDGET>::setBackgroundFocused( const QPixmap& value, bool forced )
{
    if ( forced )
        forcedBackgroundFocused_ = value;
    else
        backgroundFocused_ = value;

    updateBackgroundPalette();
}

template <class WIDGET>
void labeledWidget<WIDGET>::setBackgroundReadOnly( const QPixmap& value, bool forced )
{
    if ( forced )
        forcedBackgroundReadOnly_ = value;
    else
        backgroundReadOnly_ = value;

    updateBackgroundPalette();
}

template <class WIDGET>
bool labeledWidget<WIDGET>::hasFocus() const
{
    return child_->hasFocus();
}

template <class WIDGET>
bool labeledWidget<WIDGET>::isReadOnly() const
{
    return child_->isReadOnly();
}

template <class WIDGET>
void labeledWidget<WIDGET>::setContentsMargins( const QMargins& value )
{
    screenLayout_->setContentsMargins( value );
    update();
}

template <class WIDGET>
void labeledWidget<WIDGET>::setForced( bool value )
{
    forced_ = value;

    updateBackgroundPalette();
}

template <class WIDGET>
void labeledWidget<WIDGET>::setLabelAlignment( Qt::Alignment value )
{
    label_->setAlignment( value );
}

template <class WIDGET>
void labeledWidget<WIDGET>::setLabelFont( const QFont& value )
{
    label_->setFont( value );
}

template <class WIDGET>
void labeledWidget<WIDGET>::setLabelText( const QString& value )
{
    label_->setText( value );
}

template <class WIDGET>
void labeledWidget<WIDGET>::setLabelObjectName( const QString& value )
{
    label_->setObjectName( value );
}

template <class WIDGET>
void labeledWidget<WIDGET>::setLabelVisible( bool value )
{
    label_->setVisible( value );
}

template <class WIDGET>
void labeledWidget<WIDGET>::setReadOnly( bool value )
{
    child_->setDisabled( value );
    child_->setReadOnly( value );

    updateBackgroundPalette();
}

template <class WIDGET>
void labeledWidget<WIDGET>::setToggleButtonEnabled( bool value )
{
    toggleButtonEnabled_ = value;
}

template <class WIDGET>
void labeledWidget<WIDGET>::setToggleButtonVisible( bool value )
{
    if ( toggleButtonEnabled() )
    {
        toggleButton_->setVisible( toggleButtonVisible_ = value );
        update();
    }
}

template <class WIDGET>
void labeledWidget<WIDGET>::setWidgetAvailable( bool value )
{
    if ( (widgetAvailable_ = value) )
        toggleButton_->setText( "" );
    else
        toggleButton_->setText( "X" );
}

template <class WIDGET>
bool labeledWidget<WIDGET>::toggleButtonEnabled() const
{
    return toggleButtonEnabled_;
}

template <class WIDGET>
bool labeledWidget<WIDGET>::toggleButtonVisible() const
{
    return toggleButtonVisible_;
}

template <class WIDGET>
bool labeledWidget<WIDGET>::widgetAvailable() const
{
    return widgetAvailable_;
}

template <class WIDGET>
void labeledWidget<WIDGET>::adjustLabelOrientationLeft()
{
    QVBoxLayout *layout = new QVBoxLayout();

    QLayoutItem *item = topLayoutInner_->takeAt( 0 );

    layout->addStretch();
    layout->addItem( item );
    layout->addStretch();

    screenLayoutInner_->insertLayout( 0, layout );
}

template <class WIDGET>
void labeledWidget<WIDGET>::adjustChildCenter()
{
    widgetsLayout_->addStretch();
}

template <class WIDGET>
bool labeledWidget<WIDGET>::checkOperatorAccessLevel() const
{
    // forced fields do not check access levels
    if ( isForced() )
        return true;

    // check for valid access level
    if ( 1 < accessLevel() )
    {
        sessionInfo info( this );

        SessionMainWindow *mainWindow = info();
        ModeWidget *mode = (mainWindow ? mainWindow->mode() : NULL);

        // check against operator access level
        if (( mode ) && ( mode->operatorsExist() ))
            return ( accessLevel() <= mode->operatorAccessLevel() );
    }

    return true;
}

template <class WIDGET>
void labeledWidget<WIDGET>::flip()
{
    flipPixmap verticalPixmapFlipper( false, true );

    // flip pixmaps
    setBackground( verticalPixmapFlipper( background() ) );
    setBackgroundFocused( verticalPixmapFlipper( backgroundFocused() ) );
    setBackgroundReadOnly( verticalPixmapFlipper( backgroundReadOnly() ) );
    setBackground( verticalPixmapFlipper( background( true ) ), true );
    setBackgroundFocused( verticalPixmapFlipper( backgroundFocused( true ) ), true );
    setBackgroundReadOnly( verticalPixmapFlipper( backgroundReadOnly( true ) ), true );
    setDropShadow( verticalPixmapFlipper( dropShadow() ) );

    // flip layout
    //
    // currently layout is at follows:
    //   0 label 1
    //   1 stretch
    //   2 label 2
    //
    // but we want the following:
    //   0 label 2
    //   1 stretch
    //   2 label 1
    QList<QLayoutItem*> topItems;
    QList<QLayoutItem*> bottomItems;
    QLayoutItem *child;

    while ( (child = topLayout_->takeAt( 0 )) )
        topItems.append( child );

    while ( (child = bottomLayout_->takeAt( 0 )) )
        bottomItems.append( child );

    topLayout_->invalidate();
    bottomLayout_->invalidate();

    foreach ( child, bottomItems )
    {
        topLayout_->addItem( child );

        child->invalidate();

        if ( child->layout() )
            child->layout()->setParent( topLayout_ );
    }

    foreach ( child, topItems )
    {
        bottomLayout_->addItem( child );

        child->invalidate();

        if ( child->layout() )
            child->layout()->setParent( bottomLayout_ );
    }

    updateBackgroundPalette();
}

template <class WIDGET>
void labeledWidget<WIDGET>::translate()
{
}

template <class WIDGET>
void labeledWidget<WIDGET>::mousePressEvent( QMouseEvent *e )
{
    // when the widget is clicked move focus to the input box
    if ( Qt::LeftButton == e->button() )
        if ( child_->isVisible() )
            child_->setFocus();

    _Mybase::mousePressEvent( e );
}

template <class WIDGET>
void labeledWidget<WIDGET>::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    // move toggle button to upper right corner
    QPoint pos( rect().topRight() );
    toggleButton_->move( QPoint( pos.x() - toggleButton_->width(), 0 ) );
}

template <class WIDGET>
void labeledWidget<WIDGET>::updateBackgroundPalette()
{
    bool showDropShadow = true;

    if ( hasFocus() )
        setBackgroundPalette( backgroundFocused( isForced() ) );
    else if ( isReadOnly() )
    {
        setBackgroundPalette( backgroundReadOnly( isForced() ) );
        showDropShadow = false;
    }
    else
        setBackgroundPalette( background( isForced() ) );

    setDropShadowVisible( showDropShadow );

    update();
}

template <class WIDGET>
void labeledWidget<WIDGET>::initialize()
{
    // create layouts
    screenLayout_ = new QVBoxLayout( this );
    screenLayout_->setContentsMargins( QMargins() );
    screenLayout_->setSpacing( 0 );

    screenLayoutInner_ = new QHBoxLayout();
    screenLayoutInner_->setContentsMargins( QMargins() );
    screenLayoutInner_->setSpacing( 0 );

    widgetsLayout_ = new QVBoxLayout();
    widgetsLayout_->setContentsMargins( QMargins() );
    widgetsLayout_->setSpacing( 0 );

    topLayout_ = new QHBoxLayout();
    topLayout_->setContentsMargins( QMargins() );
    topLayout_->setSpacing( 0 );

    topLayoutInner_ = new QVBoxLayout();
    topLayoutInner_->setContentsMargins( QMargins() );
    topLayoutInner_->setSpacing( 0 );

    bottomLayout_ = new QHBoxLayout();
    bottomLayout_->setContentsMargins( QMargins() );
    bottomLayout_->setSpacing( 0 );

    bottomLayoutInner_ = new QVBoxLayout();
    bottomLayoutInner_->setContentsMargins( QMargins() );
    bottomLayoutInner_->setSpacing( 0 );

    // nest layouts
    screenLayout_->addLayout( screenLayoutInner_ );
    screenLayoutInner_->addLayout( widgetsLayout_ );
    widgetsLayout_->addLayout( topLayout_ );
    widgetsLayout_->addStretch();
    widgetsLayout_->addLayout( bottomLayout_ );
    topLayout_->addLayout( topLayoutInner_ );
    bottomLayout_->addLayout( bottomLayoutInner_ );

    // create widgets
    topLayoutInner_->addWidget( label_ = new QLabel( this ) );
    bottomLayoutInner_->addWidget( child_ = new _ChildWidgetType( this ) );

    toggleButton_ = new QPushButton( this );
    toggleButton_->setFixedSize( QSize( 48, 48 ) ); // FT-1258 increased to 48 from 32
    toggleButton_->setFocusPolicy( Qt::NoFocus );
    toggleButton_->setFont( QApplication::font() );
    toggleButton_->setPalette( QApplication::palette() );
    toggleButton_->setVisible( toggleButtonVisible_ );

    // set object names
    label_->setObjectName( "label" );
    child_->setObjectName( "child" );
    toggleButton_->setObjectName( "toggleButton" );

    // set focus proxy
    setFocusProxy( child_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // LABELEDWIDGET_H
