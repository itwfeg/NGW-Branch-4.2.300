/**
 * @file selfserviceweightbar.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "selfserviceicons.h"
#include "selfserviceweightbar.h"

#include <backgroundwidgettypes.h>
#include <capacitybarwidgets.h>
#include <selfservicehook.h>

#include <ScaleConfiguration.h>
#include <Session.h>
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>
#include <QDesktopWidget>
#include <QApplication>
#include <QMouseEvent>

const int selfServiceWeightBar::capacityWidgetHeight_( 25 );
QSize selfServiceWeightBar::weightWidgetSize_( 300, 90 );   // 180, 60     300, 90    350, 120
int selfServiceWeightBar::weightBarDataPointSize_(50);       // 30          50         60

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceWeightBar::selfServiceWeightBar( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    mode_( (mode) -1 ),
    hook_( h ),
    showTare_(false),
    scaleConfig_(NULL)
{
    setObjectName( "selfServiceWeightBar" );

    // initialize
    initialize();

    // create layouts
    createLayout();

    // translate
    translate();

    // set initial mode
    setCurrentMode( weightOnly );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceWeightBar::~selfServiceWeightBar()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWeightBar::networkConnectivity( const MadpNetworkConnectivity *m )
{
    wirelessSignalIcon_->updateState( m );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWeightBar::networkDeviceType( const MadpNetworkDeviceType *m )
{
    wirelessSignalIcon_->updateState( m );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWeightBar::refresh()
{
    // sanity check
    if ( !hook_ )
        return;

    if( !scaleConfig_ )
    {
        Session *s = hook_->session();
        scaleConfig_ = s->scaleConfig;
    }


    bool sizeUnitPriceToFit( false );

    weightLabel_->setText( hook_->fieldValue( sessionHooks::netWeightString ) );
    weight_->setText( hook_->fieldValue( sessionHooks::netWeight ) );

    // no need to populate by count when no total price... there would be no by count in that scenario!
    if ( hook_->fieldValue( sessionHooks::itemTotalPrice ).isEmpty() )
        byCount_->clear();
    else
        byCount_->setText( hook_->fieldValue( sessionHooks::itemByCount ) );

    tare_->setText( hook_->fieldValue( sessionHooks::itemTare ) );

    // by count pricing
    if ( byCountAndPrice == mode_ )
    {
        QString unitPrice( hook_->fieldValue( sessionHooks::itemUnitPrice ) );
        QString unitPriceCountFor( hook_->fieldValue( sessionHooks::itemUnitPriceCountFor ) );
        bool itemHasExceptionPricing( "1" == hook_->fieldValue( sessionHooks::itemHasExceptionPricing ) );

        // strip lbs or kgs pricing method
        unitPrice.replace( " /lb", "" );
        unitPrice.replace( " /kg", "" );

        if (( itemHasExceptionPricing ) && ( hook_->fieldValue( sessionHooks::itemByCount ).toInt() < unitPriceCountFor.toInt() ))
            unitPrice = hook_->fieldValue( sessionHooks::itemExceptionPrice );
        else if ( 1 < unitPriceCountFor.toInt() )
        {
            if( 1 == scaleConfig_->printSpecialFields.unitPriceFormat().toInt() )
            {
                QString prefix( "%1 %2 " );

                unitPrice.prepend( prefix.arg( unitPriceCountFor ).arg( tr( "for" ) ) );

                sizeUnitPriceToFit = true;
            }
        }

        if( unitPrice.contains(tr(" For ")) || checkScreenSize() == 10 )
            sizeUnitPriceToFit = true;

        unitPrice_->setText( unitPrice );
    }

    // random weight pricing
    else
    {
        QString unitPrice( hook_->fieldValue( sessionHooks::itemUnitPrice ) );
        QString unitPriceFractional( hook_->fieldValue( sessionHooks::itemUnitPriceFractional ) );
        QString unitPricePoundsFor( hook_->fieldValue( sessionHooks::itemUnitPricePoundsFor ) );

        unitPriceFractional = unitPriceFractional.trimmed();
        unitPricePoundsFor = unitPricePoundsFor.trimmed();

        if (( unitPriceFractional.length() ) && ( "1" != unitPriceFractional ))
        {
            unitPrice.append( " " );

            if ( "100" == unitPriceFractional )
                unitPrice.append( "100g" );
            else if ( "50" == unitPriceFractional )
                unitPrice.append( QString::fromUtf8( "½ lb" ) );
            else if ( "25" == unitPriceFractional )
                unitPrice.append( QString::fromUtf8( "¼ lb" ) );

            sizeUnitPriceToFit = true;
        }
        else if (( unitPricePoundsFor.length() ) && ( "1" != unitPricePoundsFor ))
        {
            if( 1 == scaleConfig_->printSpecialFields.unitPriceFormat().toInt() )
            {
                bool metricScale( "1" == hook_->fieldValue( sessionHooks::metricScale ) );
                QString prefix( "%1 %2 %3 " );

                unitPrice.prepend( prefix.arg( unitPricePoundsFor ).arg( metricScale ? "kgs" : "lbs" ).arg( tr( "for" ) ) );
            }

            sizeUnitPriceToFit = true;
        }

        unitPrice_->setText( unitPrice );
    }

    totalPrice_->setText( hook_->fieldValue( sessionHooks::itemTotalPrice ) );

/*
    weightWidgetSize_.setWidth( scaleConfig_->scaleConfig.selfServeWeightFieldWidth() );
    weightWidgetSize_.setHeight( scaleConfig_->scaleConfig.selfServeWeightFieldHeight() );
    weightBarDataPointSize_ = scaleConfig_->scaleConfig.selfServeWeightFieldFontSize();
    if (weightArea_)
        weightArea_->setFixedSize( weightWidgetSize_ );
    if (byCountArea_)
        byCountArea_->setFixedSize( weightWidgetSize_ );
    if (tareArea_)
        tareArea_->setFixedSize( weightWidgetSize_ );
    if (unitPriceArea_)
        unitPriceArea_->setFixedSize( weightWidgetSize_ );
    if (totalPriceArea_)
        totalPriceArea_->setFixedSize( weightWidgetSize_ );
*/

    capacityLabel_->setText( hook_->fieldValue( sessionHooks::capacityString ) );
    capacityHighRange_->setText( hook_->fieldValue( sessionHooks::capacityIndicatorString ) );
    capacityLowRange_->setText( hook_->fieldValue( sessionHooks::dualCapacityIndicatorString ) );
    zero_->setText( hook_->fieldValue( sessionHooks::zeroIndicatorString ) );

    capacityHighRange_->setIndicatorVisible( "1" == hook_->fieldValue( sessionHooks::capacityIndicator ) );
    capacityLowRange_->setIndicatorVisible( "1" == hook_->fieldValue( sessionHooks::dualCapacityIndicator ) );

    // hide/show zero
    zero_->setVisible( "1" == hook_->fieldValue( sessionHooks::zeroIndicator ) );

    // when unit price too big for field we need to compress it
    if ( sizeUnitPriceToFit || unitPrice_->text().length() > 8 )
        updateLabelFontPointSize( unitPrice_, 8 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWeightBar::translate()
{
    static const bool convertUpper( true );

    QString byCountLabelText( tr( "Quantity" ) );
    QString tareLabelText( tr( "Tare" ) );
    QString unitPriceLabelText( tr( "Unit Price" ) );
    QString totalPriceLabelText( tr( "Total Price" ) );
    QString weightLabelText( tr( "Weight" ) );
    QString capacityLabelText ( tr ( "Capacity" ) );
    QString zeroLabelText ( tr ("Zero" ) );

    if ( convertUpper )
    {
        byCountLabelText = byCountLabelText.toUpper();
        tareLabelText = tareLabelText.toUpper();
        unitPriceLabelText = unitPriceLabelText.toUpper();
        totalPriceLabelText = totalPriceLabelText.toUpper();
        weightLabelText = weightLabelText.toUpper();
        capacityLabelText = capacityLabelText.toUpper();
        zeroLabelText = zeroLabelText.toUpper();
    }

    byCountLabel_->setText( byCountLabelText );
    tareLabel_->setText( tareLabelText );
    unitPriceLabel_->setText( unitPriceLabelText );
    totalPriceLabel_->setText( totalPriceLabelText );
    weightLabel_->setText(weightLabelText);
    capacityLabel_->setText( capacityLabelText);
    zero_->setText(zeroLabelText);

    // update field values because they are used to change text of weightbar fields whenever the widget refreshes
    sessionHooks::instance()->updateFieldValue(sessionHooks::netWeightString, weightLabelText);
    sessionHooks::instance()->updateFieldValue( sessionHooks::capacityString, capacityLabelText);
    sessionHooks::instance()->updateFieldValue( sessionHooks::zeroIndicatorString, zeroLabelText);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWeightBar::setCurrentMode( mode value )
{
    // nothing to do
    if ( mode_ == value )
        return;

    showTare_ = false;

    // handle switch
    switch ( value )
    {
    case weightOnly:
        weightArea_->show();
        byCountArea_->hide();
        tareArea_->hide();
        unitPriceArea_->hide();
        totalPriceArea_->hide();
        background_ = &weightOnlyBrush_;
        break;

    case weightAndPrice:
        weightArea_->show();
        byCountArea_->hide();

        if( scaleConfig_->scaleConfig.selfServeDisplayTareOnRandomWeight() )
        {
            tareArea_->show();
            showTare_ = true;
        }
        else
            tareArea_->hide();

        unitPriceArea_->show();
        totalPriceArea_->show();
        background_ = &fieldAndPriceBrush_;
        break;

    case byCountAndPrice:
        weightArea_->hide();
        byCountArea_->show();
        tareArea_->hide();
        unitPriceArea_->show();
        totalPriceArea_->show();
        background_ = &fieldAndPriceBrush_;
        break;

    case capacityOnly:
        weightArea_->hide();
        byCountArea_->hide();
        tareArea_->hide();
        unitPriceArea_->hide();
        totalPriceArea_->hide();
        background_ = &weightOnlyBrush_;
        break;

    default:
        break;
    }

    // set background
    updateBackgroundPalette();

    // set mode
    mode_ = value;
    refresh();

    // emit!
    Q_EMIT currentModeChanged();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWeightBar::clearByCount()
{
    byCount_->clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWeightBar::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    //
    // create background for weight area
    //

    if( checkScreenSize() == 10 )
    {
        weightWidgetSize_.setHeight( 70 ); // 70
        weightWidgetSize_.setWidth( 200 );
    }

    QSize weightAreaSize( width(), height() - capacityArea_->height() );

    // generate pixmap
    QPixmap weightAreaPixmap( size() );
    weightAreaPixmap.fill( Qt::transparent );

    QPainter weightAreaPainter( &weightAreaPixmap );
    weightAreaPainter.setRenderHint( QPainter::Antialiasing, true );
    weightAreaPainter.setBrush( QColor( 255, 255, 255, 127 ) );

    QPen pen( weightAreaPainter.pen() );
    pen.setColor( hobart::colorPalette::gray );
    pen.setWidth( 2 );
    weightAreaPainter.setPen( pen );

    QPainterPath leftPath;
    QPainterPath middlePath;
    QPainterPath rightPath;
    QPainterPath middleLeftPath;
    QPainterPath middleRightPath;

    qreal margin( 3.0 );
    qreal slope( 10.0 );
    qreal spacing( 8.0 );

    qreal top( margin );
    qreal bottom( weightAreaSize.height() - margin );
    qreal left( margin );
    qreal right( weightAreaSize.width() - margin );

    qreal leftPath_bottom_right( weightWidgetSize_.width() );
    qreal leftPath_top_right( leftPath_bottom_right - slope );

    qreal rightPath_bottom_left( right - weightWidgetSize_.width() );
    qreal rightPath_top_left( rightPath_bottom_left + slope );

    // for resizing the weight, tare, and unit price fields as per James
    // if we're not on a 10" then this value should do nothing
    int extraMiddleSpace = 0;
    if( checkScreenSize() == 10 && showTare_ )
    {
        extraMiddleSpace = 10;
    }

    // draw path counter-clockwise
    leftPath.moveTo( left, top );
    leftPath.lineTo( left, bottom );
    leftPath.lineTo( leftPath_bottom_right + extraMiddleSpace, bottom );
    leftPath.lineTo( leftPath_top_right + extraMiddleSpace, top );
    leftPath.closeSubpath();

    // draw path counter-clockwise
    qreal middleleft_top(  leftPath_top_right + spacing );
    qreal middleleft_bot(  leftPath_bottom_right + spacing );
    qreal middleright_bot( rightPath_bottom_left - spacing );
    qreal middleright_top( rightPath_top_left - spacing );

    if( showTare_ )
    {
        int half_space( 4 );
        int halfwidth( ( middleright_top - middleleft_top ) / 2 );

        middleLeftPath.moveTo( middleleft_top + extraMiddleSpace, top );
        middleLeftPath.lineTo( middleleft_bot + extraMiddleSpace, bottom );
        middleLeftPath.lineTo( middleleft_bot + halfwidth - half_space - extraMiddleSpace, bottom );
        middleLeftPath.lineTo( middleleft_bot + halfwidth - half_space - extraMiddleSpace, top );
        middleLeftPath.closeSubpath();

        middleRightPath.moveTo( middleleft_bot + halfwidth + half_space - extraMiddleSpace, top );
        middleRightPath.lineTo( middleleft_bot + halfwidth + half_space - extraMiddleSpace, bottom );
        middleRightPath.lineTo( middleright_bot, bottom );
        middleRightPath.lineTo( middleright_top, top );
        middleRightPath.closeSubpath();
    }
    else
    {
        middlePath.moveTo( middleleft_top,  top );
        middlePath.lineTo( middleleft_bot,  bottom );
        middlePath.lineTo( middleright_bot, bottom );
        middlePath.lineTo( middleright_top, top );
        middlePath.closeSubpath();
    }

    // draw path counter-clockwise
    rightPath.moveTo( rightPath_top_left, top );
    rightPath.lineTo( rightPath_bottom_left, bottom );
    rightPath.lineTo( right, bottom );
    rightPath.lineTo( right, top );
    rightPath.closeSubpath();

    if( showTare_ )
    {
        weightAreaPainter.drawPath( leftPath );
        weightAreaPainter.drawPath( middleLeftPath );
        weightAreaPainter.drawPath( middleRightPath );
        weightAreaPainter.drawPath( rightPath );
    }
    else
    {
        weightAreaPainter.drawPath( leftPath );
        weightAreaPainter.drawPath( middlePath );
        weightAreaPainter.drawPath( rightPath );
    }

    fieldAndPriceBrush_ = QBrush( weightAreaPixmap );


    QSize weightOnlyAreaSize( width(), height() - capacityArea_->height() );

    // generate pixmap
    QPixmap weightOnlyAreaPixmap( size() );
    weightOnlyAreaPixmap.fill( Qt::transparent );

    QPainter weightOnlyAreaPainter( &weightOnlyAreaPixmap );
    weightOnlyAreaPainter.setRenderHint( QPainter::Antialiasing, true );
    weightOnlyAreaPainter.setBrush( QColor( 255, 255, 255, 127 ) );

    QPen woPen( weightOnlyAreaPainter.pen() );
    woPen.setColor( hobart::colorPalette::gray );
    woPen.setWidth( 2 );
    weightOnlyAreaPainter.setPen( woPen );

    weightOnlyAreaPainter.drawRoundedRect( QRect( QPoint( 0, 0 ), weightOnlyAreaSize ), 15, 15 );

    weightOnlyBrush_ = QBrush( weightOnlyAreaPixmap );


    // update
    updateBackgroundPalette();

    //
    // create background for capacity area
    //

    QSize capacityAreaSize( capacityArea_->size() );

    // generate pixmap
    QPixmap capacityAreaPixmap( capacityAreaSize );
    capacityAreaPixmap.fill( Qt::transparent );

    QPainter capacityAreaPainter( &capacityAreaPixmap );
    capacityAreaPainter.setRenderHint( QPainter::Antialiasing, true );
    capacityAreaPainter.setBrush( Qt::transparent );

    capacityAreaPainter.drawRoundedRect( QRect( QPoint( 0, 0 ), capacityAreaSize ), 4, 4 );

    // set background
    QPalette p( capacityArea_->palette() );
    p.setBrush( capacityArea_->backgroundRole(), capacityAreaPixmap );
    capacityArea_->setPalette( p );

    //
    // update unit price font point size
    //

    updateLabelFontPointSize( unitPrice_, 8 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool selfServiceWeightBar::eventFilter( QObject *obj, QEvent *e )
{
    // emit weight pressed signal
    /*
    if ( this == obj )
        if ( QEvent::MouseButtonRelease == e->type() )
            Q_EMIT weightPressed();
    */

    // SOF-5167 - Restrict W&M page to only open when the capacity string is clicked
    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>( e );
    if( mouseEvent )
    {
        if( mouseEvent->y() > capacityArea_->y() && 
            mouseEvent->x() < ( capacityLabel_->width() + capacityHighRange_->width() + capacityLowRange_->width() ) 
        )
        {
            Q_EMIT weightPressed();
        }
    }

    return _Mybase::eventFilter( obj, e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWeightBar::initialize()
{
    if( checkScreenSize() == 10 )
    {
        weightWidgetSize_.setHeight( 70 ); // 70
        weightWidgetSize_.setWidth( 200 );

        weightBarDataPointSize_ = 30;
    }
    else if( checkScreenSize() == 14 )
    {
        weightBarDataPointSize_ = 42;
    }

    QList<QLabel*> labelWidgets;
    QList<QLabel*> valueWidgets;

    setAutoFillBackground( true );

    // event filter to handle user pressing weight area
    installEventFilter( this );

    weightOnlyBrush_ = QBrush( Qt::transparent );
    fieldAndPriceBrush_ = QBrush( Qt::transparent );
    background_ = &weightOnlyBrush_;

    // ----- //
    weightArea_ = new QWidget( this );
    weightArea_->setFixedSize( weightWidgetSize_ );
    weightLabel_ = new QLabel( weightArea_ );
    weight_ = new QLabel( weightArea_ );

    labelWidgets.append( weightLabel_ );
    valueWidgets.append( weight_ );

    byCountArea_ = new QWidget( this );
    byCountArea_->setFixedSize( weightWidgetSize_ );
    byCountLabel_ = new QLabel( byCountArea_ );
    byCount_ = new QLabel( byCountArea_ );

    labelWidgets.append( byCountLabel_ );
    valueWidgets.append( byCount_ );

    tareArea_ = new QWidget( this );
    tareArea_->setFixedSize( weightWidgetSize_ );
    tareLabel_ = new QLabel( tareArea_ );
    tare_ = new QLabel( tareArea_ );

    labelWidgets.append( tareLabel_ );
    valueWidgets.append( tare_ );

    unitPriceArea_ = new QWidget( this );
    unitPriceArea_->setFixedSize( weightWidgetSize_ );
    unitPriceLabel_ = new QLabel( unitPriceArea_ );
    unitPrice_ = new QLabel( unitPriceArea_ );

    labelWidgets.append( unitPriceLabel_ );
    valueWidgets.append( unitPrice_ );

    totalPriceArea_ = new QWidget( this );
    totalPriceArea_->setFixedSize( weightWidgetSize_ );
    totalPriceLabel_ = new QLabel( totalPriceArea_ );
    totalPrice_ = new QLabel( totalPriceArea_ );

    labelWidgets.append( totalPriceLabel_ );
    valueWidgets.append( totalPrice_ );

    // ----- //

    capacityArea_ = new QWidget( this );
    capacityArea_->setAutoFillBackground( true );
    capacityArea_->setFixedWidth( 420 );

    capacityLabel_ = new hobart::indicatorLabel( capacityArea_ );
    capacityLabel_->setFixedHeight( capacityWidgetHeight_ );

    capacityHighRange_ = new hobart::indicatorLabel( capacityArea_ );
    capacityHighRange_->setFixedHeight( capacityWidgetHeight_ );

    capacityLowRange_ = new hobart::indicatorLabel( capacityArea_ );
    capacityLowRange_->setFixedHeight( capacityWidgetHeight_ );

    zero_ = new hobart::indicatorLabel( capacityArea_ );
    zero_->setFixedHeight( capacityWidgetHeight_ );
    zero_->setIndicatorDirection( hobart::indicatorLabel::Down );
    zero_->setIndicatorVisible( true );

    wirelessSignalIcon_ = new hobart::selfServiceWirelessSignalIcon( QSize( 0, capacityWidgetHeight_ ), capacityArea_ );

    labelWidgets.append( capacityLabel_ );
    labelWidgets.append( capacityHighRange_ );
    labelWidgets.append( capacityLowRange_ );
    labelWidgets.append( zero_ );

    // ----- //

    // process labels
    foreach ( QLabel *w, labelWidgets )
    {
        QFont f( w->font() );
        f.setPointSize( 9 );
        w->setFont( f );

        w->setAlignment( Qt::AlignCenter );
    }

    // process values
    foreach ( QLabel *w, valueWidgets )
    {
        QFont f( w->font() );
        f.setPointSize( weightBarDataPointSize_ );
        w->setFont( f );

        w->setAlignment( Qt::AlignCenter );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWeightBar::createLayout()
{
    QHBoxLayout *weightLayout( new QHBoxLayout() );
    weightLayout->setContentsMargins( QMargins() );

    weightLayout->addWidget( weightArea_ );
    weightLayout->addWidget( byCountArea_ );
    weightLayout->addWidget( tareArea_ );
    weightLayout->addWidget( unitPriceArea_ );
    weightLayout->addWidget( totalPriceArea_ );
    weightLayout->addStretch();

    QHBoxLayout *capacityLayout( new QHBoxLayout() );
    capacityLayout->setContentsMargins( QMargins() );   // default const:: constructs L,T,R,B margins all set to 0

    capacityLayout->addWidget( capacityArea_ );
    capacityLayout->addStretch();

    QVBoxLayout *screen( new QVBoxLayout( this ) );
    screen->setContentsMargins( QMargins( 1, 1, 1, 1 ) );
    screen->setSpacing( 1 );

    screen->addLayout( weightLayout );
    screen->addLayout( capacityLayout );

    // ----- //

    static const QMargins widgetMargins( 3, 2, 3, 2 );
    static const int widgetSpacing( 2 );

    // weight
    QVBoxLayout *weightAreaLayout( new QVBoxLayout( weightArea_ ) );
    weightAreaLayout->setContentsMargins( widgetMargins );
    weightAreaLayout->setSpacing( widgetSpacing );

    weightAreaLayout->addWidget( weightLabel_ );
    weightAreaLayout->addWidget( weight_ );

    // by count
    QVBoxLayout *byCountAreaLayout( new QVBoxLayout( byCountArea_ ) );
    byCountAreaLayout->setContentsMargins( widgetMargins );
    byCountAreaLayout->setSpacing( widgetSpacing );

    byCountAreaLayout->addWidget( byCountLabel_ );
    byCountAreaLayout->addWidget( byCount_ );

    // tare
    QVBoxLayout *tareAreaLayout( new QVBoxLayout( tareArea_ ) );
    tareAreaLayout->setContentsMargins( widgetMargins );
    tareAreaLayout->setSpacing( widgetSpacing );

    tareAreaLayout->addWidget( tareLabel_ );
    tareAreaLayout->addWidget( tare_ );

    // unit price
    QVBoxLayout *unitPriceAreaLayout( new QVBoxLayout( unitPriceArea_ ) );
    unitPriceAreaLayout->setContentsMargins( widgetMargins );
    unitPriceAreaLayout->setSpacing( widgetSpacing );

    unitPriceAreaLayout->addWidget( unitPriceLabel_ );
    unitPriceAreaLayout->addWidget( unitPrice_ );

    // total price
    QVBoxLayout *totalPriceAreaLayout( new QVBoxLayout( totalPriceArea_ ) );
    totalPriceAreaLayout->setContentsMargins( widgetMargins );
    totalPriceAreaLayout->setSpacing( widgetSpacing );

    totalPriceAreaLayout->addWidget( totalPriceLabel_ );
    totalPriceAreaLayout->addWidget( totalPrice_ );

    // capacity
    QHBoxLayout *capacityAreaLayout( new QHBoxLayout( capacityArea_ ) );
    capacityAreaLayout->setContentsMargins( QMargins( 1, 1, 1, 1 ) );

    capacityAreaLayout->addWidget( capacityLabel_ );
    capacityAreaLayout->addWidget( capacityHighRange_ );
    capacityAreaLayout->addWidget( capacityLowRange_ );
    capacityAreaLayout->addStretch();
    capacityAreaLayout->addWidget( zero_ );
    capacityAreaLayout->addStretch();
    capacityAreaLayout->addWidget( wirelessSignalIcon_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWeightBar::updateBackgroundPalette()
{
    // set background
    QPalette p( palette() );
    p.setBrush( backgroundRole(), *background_ );
    setPalette( p );

    // update
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWeightBar::updateLabelFontPointSize( QLabel *w, int margins )
{
    QString text( w->text() );

    if ( text.isEmpty() )
        return;

    QFont f( w->font() );
    int pointSize( weightBarDataPointSize_ );

    // reduce font size incrementally until we fit the field
    do
    {
        f.setPointSize( pointSize );

        // compute bounding rect
        QRect rect( 0, 0, INT_MAX, INT_MAX );

        QFontMetrics metrics( f );
        QRect boundingRect( metrics.boundingRect( rect, Qt::AlignLeft | Qt::AlignTop, text ) );

        // when text is smaller than widget we are done
        if ( boundingRect.width() <= (unitPrice_->width() - margins) )
            break;

    } while ( 8 < pointSize-- );

    // set new point size
    w->setFont( f );

    update();
}

int selfServiceWeightBar::checkScreenSize()
{
    // laptop (1920x1080)
    int screen_size = 10;

    QDesktopWidget *desktop = QApplication::desktop();

    int screen = desktop->primaryScreen();

    if (( 0 <= screen ) && ( screen < desktop->screenCount() ))
    {
        QRect g( desktop->screenGeometry( screen ) );

        QSize resolution_ = g.size();

        //QMessageBox::information(this, "Resolution", "Screen resolution is\n\nWidth: " + QString::number(resolution_.width()) + "\nHeight: " + QString::number(resolution_.height()), QMessageBox::Ok);

        if( resolution_.width() > 1034 && resolution_.height() > 610 )  // gave it 10 more but do I need to?
            screen_size = 14;
    }

    return screen_size;
}
