/**
 * @file backgroundwidget.h
 * Widget that uses a custom background image.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef BACKGROUNDWIDGET_H
#define BACKGROUNDWIDGET_H

#include <palette.h>

#include <QApplication>
#include <QPainter>
#include <QPalette>
#include <QPen>
#include <QString>
#include <QWidget>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class backgroundWidget
 * @brief Widget that uses a custom background image.
 *
 * This widget will override the background to the image passed in. It will also resize the widget
 * to match the size of the image.
 *
 * @tparam _Widget  base widget class
 */
template <class _Widget>
class backgroundWidget : public _Widget
{
    typedef backgroundWidget _Myt;
    typedef _Widget _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] image  background image for the widget
     * @param[in] parent  parent widget
     */
    explicit backgroundWidget( const QPixmap& image, QWidget *parent = 0 );

    /**
     * Constructor.
     *
     * @param[in] image  background image for the widget
     * @param[in] text  widget text
     * @param[in] parent  parent widget
     */
    explicit backgroundWidget( const QPixmap& image, const QString& text, QWidget *parent = 0 );

    /**
     * Constructor.
     *
     * @param[in] image  background image for the widget
     * @param[in] text  widget icon
     * @param[in] text  widget text
     * @param[in] parent  parent widget
     */
    explicit backgroundWidget( const QPixmap& image, const QIcon& icon, const QString& text, QWidget *parent = 0 );

    /**
     * Constructor.
     *
     * @param[in] image  background image for the widget
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit backgroundWidget( const QPixmap& image, QWidget *parent, Qt::WindowFlags f );

    /**
     * Destructor.
     */
    virtual ~backgroundWidget();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve background.
     *
     * @return  background pixmap
     */
    virtual QPixmap background() const {return background_;}

    /**
     * Set the background.
     *
     * @param[in] value  background pixmap
     */
    virtual void setBackground( const QPixmap& value );

    /**
     * Retrieve button border pen.
     *
     * @return  border pen
     */
    virtual QPen borderPen() const {return borderPen_;}

    /**
     * Set the button border pen.
     *
     * @param[in] value  border pen
     */
    virtual void setBorderPen( const QPen& value );

    /**
     * Retrieve button border radius.
     *
     * @return  border radius in pixels
     */
    virtual qreal borderRadius() const {return borderRadius_;}

    /**
     * Set the button border radius.
     *
     * @param[in] value  border radius in pixels
     */
    virtual void setBorderRadius( qreal value );

    /**
     * Retrieve drop shadow pixmap.
     *
     * @return  pixmap of drop shadow
     */
    virtual QPixmap dropShadow() const {return dropShadow_->shadow_;}

    /**
     * Set drop shadow pixmap.
     *
     * @param[in] value  pixmap of drop shadow
     */
    virtual void setDropShadow( const QPixmap& value );

    /**
     * Set if drop shadow is visible.
     *
     * @param[in] value  @c true if visible, @c false otherwise
     */
    virtual void setDropShadowVisible( bool value );

protected:

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Sets the background palette.
     *
     * @param[in] pixmap  background pixmap to use
     */
    virtual void setBackgroundPalette( const QPixmap& pixmap );

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Draw background of control.
     *
     * @param[in,out] painter  painter object
     */
    virtual void drawControlBackground( QPainter *painter );

    /**
     * Hide event.
     *
     * @param[in,out] e  hide event
     */
    virtual void hideEvent( QHideEvent *e );

    /**
     * Move event.
     *
     * @param[in,out] e  move event
     */
    virtual void moveEvent( QMoveEvent *e );

    /**
     * Paint event.
     *
     * @param[in,out] e  paint event
     */
    virtual void paintEvent( QPaintEvent *e );

    /**
     * Resize event.
     *
     * @param[in,out] e  resize event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /**
     * Show event.
     *
     * @param[in,out] e  show event
     */
    virtual void showEvent( QShowEvent *e );

private:

    /**
     * @class dropShadowWidget
     * @brief Widget that handles drop shadow rendering.
     */
    class dropShadowWidget : public QWidget
    {
        friend class backgroundWidget;

        typedef dropShadowWidget _Myt;
        typedef QWidget _Mybase;

    public:

        /// Constructor.
        explicit dropShadowWidget( QWidget *parent = 0 ) :
            _Mybase( parent ) {
            setAttribute( Qt::WA_TransparentForMouseEvents );
        }

        /// Destructor.
        virtual ~dropShadowWidget() {}

    protected:

        /// Paint event.
        virtual void paintEvent( QPaintEvent *event ) {
            Q_UNUSED( event )
            if ( !shadow_.isNull() ) {
                QPainter painter( this );
                painter.drawPixmap( 0, 0, shadow_ );
            }
        }

    private:

        QPixmap shadow_;

    };

    QPixmap background_;

    dropShadowWidget *dropShadow_;
    bool dropShadowVisible_;

    QPen borderPen_;
    qreal borderRadius_;

    /**
     * Initialize.
     */
    void initialize( const QPixmap& image );

};

template <class _Widget>
backgroundWidget<_Widget>::backgroundWidget( const QPixmap& image, QWidget *parent ) :
    _Mybase( parent ),
    dropShadow_( new dropShadowWidget( parent ) ),
    dropShadowVisible_( false ),
    borderPen_( Qt::transparent ),
    borderRadius_( 0.0 )
{
    // initialize
    initialize( image );
}

template <class _Widget>
backgroundWidget<_Widget>::backgroundWidget( const QPixmap& image, const QString& text, QWidget *parent ) :
    _Mybase( text, parent ),
    dropShadow_( new dropShadowWidget( parent ) ),
    dropShadowVisible_( false ),
    borderPen_( Qt::transparent ),
    borderRadius_( 0.0 )
{
    // initialize
    initialize( image );
}

template <class _Widget>
backgroundWidget<_Widget>::backgroundWidget( const QPixmap& image, const QIcon& icon, const QString& text, QWidget *parent ) :
    _Mybase( icon, text, parent ),
    dropShadow_( new dropShadowWidget( parent ) ),
    dropShadowVisible_( false ),
    borderPen_( Qt::transparent ),
    borderRadius_( 0.0 )
{
    // initialize
    initialize( image );
}

template <class _Widget>
backgroundWidget<_Widget>::backgroundWidget( const QPixmap& image, QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( parent, f ),
    dropShadow_( new dropShadowWidget( parent ) ),
    dropShadowVisible_( false ),
    borderPen_( Qt::transparent ),
    borderRadius_( 0.0 )
{
    // initialize
    initialize( image );
}

template <class _Widget>
backgroundWidget<_Widget>::~backgroundWidget()
{
}

template <class _Widget>
void backgroundWidget<_Widget>::setBackground( const QPixmap& value )
{
    // set background
    initialize( value );
    _Mybase::update();
}

template <class _Widget>
void backgroundWidget<_Widget>::setBorderPen( const QPen& value )
{
    Q_UNUSED( value );

    // set pen
    //borderPen_ = value;
    borderPen_.setColor( Qt::black );

    _Mybase::update();
}

template <class _Widget>
void backgroundWidget<_Widget>::setBorderRadius( qreal value )
{
    // set radius
    borderRadius_ = value;
    _Mybase::update();
}

template <class _Widget>
void backgroundWidget<_Widget>::setDropShadow( const QPixmap& value )
{
    // set pixmap
    dropShadow_->shadow_ = value;
    dropShadow_->update();
}

template <class _Widget>
void backgroundWidget<_Widget>::setDropShadowVisible( bool value )
{
    dropShadowVisible_ = value;

    // set visibility
    dropShadow_->setVisible( dropShadowVisible_ && _Mybase::isVisible() );
    dropShadow_->update();
}

template <class _Widget>
void backgroundWidget<_Widget>::setBackgroundPalette( const QPixmap& pixmap )
{
    // set background palette
    if ( !pixmap.isNull() )
    {
        modifyPaletteBackground setBackground( pixmap );
        setBackground( this );

        _Mybase::update();
    }
}

template <class _Widget>
void backgroundWidget<_Widget>::drawControlBackground( QPainter *painter )
{
    // draw control
    painter->setBrush( painter->background() );
    painter->setPen( borderPen_ );
    painter->drawRoundedRect( _Mybase::rect(), borderRadius_, borderRadius_ );
}

template <class _Widget>
void backgroundWidget<_Widget>::hideEvent( QHideEvent *e )
{
    dropShadow_->hide();
    _Mybase::hideEvent( e );
}

template <class _Widget>
void backgroundWidget<_Widget>::moveEvent( QMoveEvent *e )
{
    QPoint pos( _Mybase::geometry().topLeft() );

    dropShadow_->move( pos.x() + 5, pos.y() + 5 );
    _Mybase::moveEvent( e );
}

template <class _Widget>
void backgroundWidget<_Widget>::paintEvent( QPaintEvent *e )
{
    // draw control
    if ( background_.isNull() )
        _Mybase::paintEvent( e );
    else
    {
        QPainter p( this );

        drawControlBackground( &p );
    }
}

template <class _Widget>
void backgroundWidget<_Widget>::resizeEvent( QResizeEvent *e )
{
    dropShadow_->setFixedSize( _Mybase::size() );
    _Mybase::resizeEvent( e );
}

template <class _Widget>
void backgroundWidget<_Widget>::showEvent( QShowEvent *e )
{
    if ( dropShadowVisible_ )
        dropShadow_->show();
    else
        dropShadow_->hide();                    // FS UI - hides border around blank images

    _Mybase::showEvent( e );
}

template <class _Widget>
void backgroundWidget<_Widget>::initialize( const QPixmap& image )
{
    // set background
    background_ = image;

    // set palette
    if ( !background_.isNull() )
    {
        // set size
        _Mybase::setFixedSize( background_.size() );

        // set palette
        setBackgroundPalette( background_ );

        // we draw our own background
        _Mybase::setAutoFillBackground( false );
    }

    // make sure we are above the drop shadow
    dropShadow_->stackUnder( this );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // BACKGROUNDWIDGET_H
