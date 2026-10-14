#include <logmanager.h>

#include "printerPlatePositioning.h"

#include "../widgets/labeledpluedit.h"

#include "../modes/operatemode.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStandardPaths>
#include <QPixmap>
#include <QDesktopWidget>
#include <QTimer>

static const double HALF_INCH_MM = 12.7;
static const int PLUS_ERROR_MM = 10;
static const int MINUS_ERROR_MM = 3;

PrinterPlatePositioning::PrinterPlatePositioning( SessionMainWindow *mainWindow, ModeWidget *parent )
    : _Mybase( mainWindow, parent )
    , operateMode { NULL }
    , printerDrawn { false }
    , isRightDischarge { true }
    , hasSecondLabel { false }
    , primaryPlateDistFromHome { 0 }
    , packageId { 0 }
    , currentNotchPrimary { 0 }
    , currentNotchSecondary { 0 }
    , requiredNotchPrimary { 0 }
    , requiredNotchSecondary { 0 }
    , graphicalNotchDistance { 0 }
{
    connect( mode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ) );

    operateMode = dynamic_cast<OperateMode*>( mode_ );

    // initialize
    initialize();
    createLayout( Qt::RightDockWidgetArea );

    session_->send( Wrapper, WrReqCalibration, QString::number( 13 ) );

    translate();
}

PrinterPlatePositioning::~PrinterPlatePositioning()
{
    delete primaryPlateImage;
}

void PrinterPlatePositioning::translate()
{
    acceptButton->setText( tr( "ACCEPT" ) );
    rejectButton->setText( tr( "REJECT" ) );

    _Mybase::translate();
}

bool PrinterPlatePositioning::eventFilter( QObject *obj, QEvent *e )
{
    return _Mybase::eventFilter( obj, e );
}

void PrinterPlatePositioning::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO("PrinterPlatePositioning", "onSessionMessage");
    int device = msg[0].toInt();
    int event = msg[1].toInt();

    if( device == Wrapper && event == WrPPPDist )
    {

        switch( msg[2].toInt() )
        {
            case primary:
            {
                primaryPlateDistFromHome = msg[3].toInt();
                //currentNotchPrimary = ceil( primaryPlateDistFromHome / HALF_INCH_MM );

                currentNotchPrimary = getNotch( primaryPlateDistFromHome );

                break;
            }
            case secondary:
            {
                secondaryPlateDistFromHome = msg[3].toInt();
                //currentNotchSecondary = ceil( secondaryPlateDistFromHome / HALF_INCH_MM );

                currentNotchSecondary = getNotch( secondaryPlateDistFromHome );

                break;
            }
            case unknown:
            {
                LOG_DEBUG( "WrPPPDist: Unknown received.", NULL );

                break;
            }
        }

        // Need to update the interface here
        updateInterface();
    }

    _Mybase::onSessionMessage( msg );
}

bool PrinterPlatePositioning::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    LOG_CLASS_INFO( "PrinterPlatePositioning", "isErrorMessageHandler" );

    // suppress "take label" error message when moving plates
    if (( Error == errorMsg[0].toInt() ) && ( ErrPrinterDevice == errorMsg[1].toInt() ))
    {
        if (( 7005 == errorMsg[2].toInt() ) || ( 7006 == errorMsg[2].toInt() ) ||
            ( 7105 == errorMsg[2].toInt() ) || ( 7106 == errorMsg[2].toInt() ) ||
            ( 7205 == errorMsg[2].toInt() ) || ( 7206 == errorMsg[2].toInt() ) ||
            ( 7505 == errorMsg[2].toInt() ) || ( 7506 == errorMsg[2].toInt() ))
        {
            // TODO
            // if we reject position we should probably show the error
            // if the wrapper times out and/or powers off we should probably show the error

            LOG_INFO( "suppressing take label message...", NULL );
            return true;
        }
    }

    return _Mybase::isErrorMessageHandler( errorMsg );
}

void PrinterPlatePositioning::updateInterface()
{
    LOG_CLASS_INFO( "PrinterPlatePositioning", "updateInterface" );

    // Primary printer is always on the inside, so we need to swap the printer graphics depending on if we are left or right discharge
    if( isRightDischarge )
    {
        // right discharge
        primaryArrowheadPoints[baseLeft].setX( preview->x() + printerImageSize.width() + ( 3 * edgeOffset ) - 10 );
        primaryArrowheadPoints[baseRight].setX( preview->x() + printerImageSize.width() + ( 3 * edgeOffset ) + arrowhead.baseWidth - 10 );
        primaryArrowheadPoints[top].setX( 0.5f * ( primaryArrowheadPoints[baseLeft].x() + primaryArrowheadPoints[baseRight].x() ) );

        secondaryArrowheadPoints[baseLeft].setX( preview->width() - printerImageSize.width() - ( 3 * edgeOffset ) - arrowhead.baseWidth );
        secondaryArrowheadPoints[baseRight].setX( preview->width() - printerImageSize.width() - ( 3 * edgeOffset ) );
        secondaryArrowheadPoints[top].setX( 0.5f * ( secondaryArrowheadPoints[baseLeft].x() + secondaryArrowheadPoints[baseRight].x() ) );
    }
    else
    {
        // left discharge
        primaryArrowheadPoints[baseLeft].setX( preview->width() - printerImageSize.width() - ( 3 * edgeOffset ) - arrowhead.baseWidth );
        primaryArrowheadPoints[baseRight].setX( preview->width() - printerImageSize.width() - ( 3 * edgeOffset ) );
        primaryArrowheadPoints[top].setX( 0.5f * ( primaryArrowheadPoints[baseLeft].x() + primaryArrowheadPoints[baseRight].x() ) );

        secondaryArrowheadPoints[baseLeft].setX( preview->x() + printerImageSize.width() + ( 3 * edgeOffset ) - 10 );
        secondaryArrowheadPoints[baseRight].setX( preview->x() + printerImageSize.width() + ( 3 * edgeOffset ) + arrowhead.baseWidth - 10 );
        secondaryArrowheadPoints[top].setX( 0.5f * ( secondaryArrowheadPoints[baseLeft].x() + secondaryArrowheadPoints[baseRight].x() ) );
    }

    // check if we already drew the printer to avoid drawing it everytime we update
    if( !printerDrawn )
    {
        // No need to swap printer graphic since its the same image
        LOG_DEBUG( "initializing printer images", NULL );

        primaryPlateImageRect.setX( preview->x() + ( /*2 **/ edgeOffset ) );
        primaryPlateImageRect.setY( ( preview->height() / 2 ) - printerImageSize.height() );
        primaryPlateImageRect.setWidth( printerImageSize.width() );
        primaryPlateImageRect.setHeight( printerImageSize.height() );

        secondaryPlateImageRect.setX( preview->width() - printerImageSize.width() - ( /*2 **/ edgeOffset ) );
        secondaryPlateImageRect.setY( ( preview->height() / 2 ) - printerImageSize.height() );
        secondaryPlateImageRect.setWidth( printerImageSize.width() );
        secondaryPlateImageRect.setHeight( printerImageSize.height() );

        printerDrawn = true;
    }

    generatePreview();
}

void PrinterPlatePositioning::onScreenChanged()
{
    if ( mode_->screen() != this )
        return;

    //session_->send( Wrapper, WrReqCalibration, QString::number( 13 ) );
    updateInterface();
}

void PrinterPlatePositioning::acceptButtonClicked()
{
    QString msg = QString::number( packageId );
    msg += "," + QString::number( resume );

    // send message to backend saying were done
    session_->send( Wrapper, WrPPPResume, msg );

    mainWindow_->navigateHome();
}

void PrinterPlatePositioning::rejectButtonClicked()
{
    QString msg = QString::number( packageId );
    msg += "," + QString::number( reject );

    // send message to backend we need to cancel this whole operation
    session_->send( Wrapper, WrPPPResume, msg );

    mainWindow_->navigateHome();
}

// UI Initialization
void PrinterPlatePositioning::initialize()
{
    LOG_CLASS_INFO( "PrinterPlatePositioning", "initialize()" );

    static const QMargins margins( 10, 0, 10, 0 );

    modifyFontPointSize setFontPointSize14( scaleFontPointSizeForScreen( 14 ) );

    const hobart::backgroundWidgetRenderer frameRenderer(
        new hobart::rectangleBackgroundWidget(),
        new hobart::darkOperatorBackgroundWidgetRendererAppearance()
    );

    const QSize messageFrameSize( mainWindow_->width() - 100, 50 );
    const QSize labelFrameSize( mainWindow_->width() - 45, scaleHeightForScreen( 475 ) );

    QString scaleModel = session_->scaleConfig->getScaleModelName();
    LOG_INFO("Scale model: %s", scaleModel.toStdString().c_str());

    if( scaleModel.contains("LR") )
    {
        isRightDischarge = true;
    }
    else
    {
        isRightDischarge = false;
    }
    //isRightDischarge = session_->scaleConfig->isWrapperRightDischarge();
    LOG_INFO( "isRightDischarge: %d", isRightDischarge );

    labelFrame = new frameWidget( frameRenderer.drawControl( labelFrameSize, hobart::ReadOnly ), this );
    labelFrame->setDropShadow( frameRenderer.drawDropShadow( labelFrameSize ) );
    labelFrame->setContentsMargins( margins );
    labelFrame->setObjectName( "labelFrame" );

    preview = new QLabel( labelFrame );
    preview->setAutoFillBackground( true );
    preview->setObjectName( "preview" );

    acceptButton = new QPushButton( labelFrame );
    acceptButton->setFixedSize( QSize( 250, 75 ) );
    acceptButton->setStyleSheet( "QPushButton {background-color: green; font: bold}" );
    acceptButton->setFocusPolicy( Qt::NoFocus );
    acceptButton->setObjectName( "acceptButton" );

    connect( acceptButton, SIGNAL( clicked() ), SLOT( acceptButtonClicked() ) );

    rejectButton = new QPushButton( labelFrame );
    rejectButton->setFixedSize( QSize( 250, 75 ) );
    rejectButton->setStyleSheet( "QPushButton {background-color: red; font: bold}" );
    rejectButton->setFocusPolicy( Qt::NoFocus );
    rejectButton->setObjectName( "rejectButton" );

    connect( rejectButton, SIGNAL( clicked() ), SLOT( rejectButtonClicked() ) );

    primaryPlateImage = new QImage();

    if( !primaryPlateImage->load( ":/images/screens/printer.png" ) )
    {
        LOG_INFO( "Failed to load printer image", NULL );
    }

    acceptButton->setEnabled(true);
    rejectButton->setEnabled(true);

    if( getScreenSize() == 14 )
    {
        edgeOffset = 40;

        printerImageSize.setWidth( 200 );
        printerImageSize.setHeight( 200 );

        arrowhead.baseWidth = 40;
        arrowhead.height = 40;
    }
    else
    {
        edgeOffset = 20;

        printerImageSize.setWidth( 100 );
        printerImageSize.setHeight( 100 );

        arrowhead.baseWidth = 25;
        arrowhead.height = 25;
    }

    graphicalNotchDistance = arrowhead.height - 13;

    homeLocation = preview->y() + 45;

    double requiredDistancePrimary = ( operateMode->getRequiredPrimaryPlatePosition() / 10.0f ) * 25.4; // in inches
    double requiredDistanceSecondary = ( operateMode->getRequiredSecondaryPlatePosition() / 10.0f ) * 25.4; // in inches

    LOG_INFO( "Distances - Primary: %d, Secondary: %d", requiredDistancePrimary, requiredDistanceSecondary );

    requiredNotchPrimary = ceil( requiredDistancePrimary / HALF_INCH_MM );
    requiredNotchSecondary = ceil( requiredDistanceSecondary / HALF_INCH_MM );

    tempPrimaryDist = (int) requiredDistancePrimary;
    tempSecondaryDist = (int) requiredDistanceSecondary;

    LOG_INFO( "Notches - Primary: %d, Secondary: %d", requiredNotchPrimary, requiredNotchSecondary );

    printerDrawn = false;

    primaryPlateImageRect.setX( preview->x() + ( 2 * edgeOffset ) );
    primaryPlateImageRect.setWidth( printerImageSize.width() );
    primaryPlateImageRect.setHeight( printerImageSize.height() );

    secondaryPlateImageRect.setX( preview->width() - printerImageSize.width() - ( 2 * edgeOffset ) );
    secondaryPlateImageRect.setWidth( printerImageSize.width() );
    secondaryPlateImageRect.setHeight( printerImageSize.height() );

    packageId = operateMode->getPackageId();

    if( session_->scaleConfig->currentPLU.secondLabelType() > 0 )
    {
        hasSecondLabel = true;
    }
    else
    {
        hasSecondLabel = false;
    }

    LOG_INFO( "hasSecondLabel = %d", hasSecondLabel );
}

void PrinterPlatePositioning::createLayout( Qt::DockWidgetArea numPadLocation )
{
    // delete previous layout
    if ( labelFrame->layout() )
    {
        delete labelFrame->layout();
    }

    QVBoxLayout *center( new QVBoxLayout() );
    center->addWidget( acceptButton );
    center->addWidget( rejectButton );

    QHBoxLayout *labelFrameLayout( new QHBoxLayout( labelFrame ) );

    labelFrameLayout->addWidget( preview, 1 );
    labelFrameLayout->addLayout( center );

    // delete previous layout
    if ( layout() )
    {
        delete layout();
    }

    QHBoxLayout *top( new QHBoxLayout() );
    top->setContentsMargins( QMargins() );
    top->setSpacing( 0 );

    QHBoxLayout *bottom( new QHBoxLayout() );
    bottom->setContentsMargins( QMargins() );
    bottom->setSpacing( 0 );

    bottom->addWidget( labelFrame );

    QHBoxLayout *bottomLayout( new QHBoxLayout() );
    bottomLayout->setContentsMargins( QMargins() );
    bottomLayout->setSpacing( 0 );

    if ( Qt::LeftDockWidgetArea == numPadLocation )
    {
        bottomLayout->addLayout( bottom, 1 );
        bottomLayout->addSpacing( 45 );
    }
    else if ( Qt::RightDockWidgetArea == numPadLocation )
    {
        bottomLayout->addSpacing( 45 );
        bottomLayout->addLayout( bottom, 1 );
    }

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins() );
    screenLayout->setSpacing( 0 );

    screenLayout->addLayout( top );
    screenLayout->addLayout( bottomLayout, 1 );
}

void PrinterPlatePositioning::generatePreview()
{
    QPalette p( preview->palette() );

    QPixmap pixmap( preview->size() );
    pixmap.fill( Qt::transparent );

    QPainter painter( &pixmap );
    painter.setRenderHint( QPainter::Antialiasing, true );

    drawDestinations( painter );
    if( hasSecondLabel )
    {
        painter.drawImage( primaryPlateImageRect, *primaryPlateImage );
    }

    if( isRightDischarge && !hasSecondLabel )
    {
        painter.drawImage( primaryPlateImageRect, *primaryPlateImage );
    }
    else
    {
        painter.drawImage( secondaryPlateImageRect, *primaryPlateImage );
    }

    drawAlignmentText( painter );

    QFont font;
    font.setPixelSize( 18 );
    font.setBold( true );
    font.setItalic( true );

    painter.setFont( font );

    QPen pen;

    // show pixmap
    p.setBrush( preview->backgroundRole(), pixmap );
    preview->setPalette( p );

    preview->update();
}

// This draws the notches and an arrow to guide the user to the required notch ( highlighted in green )
void PrinterPlatePositioning::drawDestinations(QPainter &painter)
{
    QPen primaryPen, secondaryPen;

    primaryPen.setWidth( 5 );
    secondaryPen.setWidth( 5 );

    primaryPen.setColor( hobart::colorPalette::red );
    secondaryPen.setColor( hobart::colorPalette::red );

    // draw the notches
    int currentLocation = homeLocation;
    for( int i = 0; i < NUM_NOTCHES; ++i )
    {
        if( i == requiredNotchPrimary )
        {
            primaryPen.setColor( hobart::colorPalette::green );
        }
        else
        {
            primaryPen.setColor( hobart::colorPalette::red );
        }

        if( i == requiredNotchSecondary && hasSecondLabel )
        {
            secondaryPen.setColor( hobart::colorPalette::green );
        }
        else
        {
            secondaryPen.setColor( hobart::colorPalette::red );
        }

        painter.setPen( primaryPen );
        painter.drawLine(
            primaryArrowheadPoints[baseLeft].x(),
            currentLocation,
            primaryArrowheadPoints[baseRight].x(),
            currentLocation
        );

        if( hasSecondLabel )
        {
            painter.setPen( secondaryPen );
            painter.drawLine(
                secondaryArrowheadPoints[baseLeft].x(),
                currentLocation,
                secondaryArrowheadPoints[baseRight].x(),
                currentLocation
            );
        }

        // draw arrows
        if( i == currentNotchPrimary )
        {
            if( requiredNotchPrimary <= currentNotchPrimary )
            {
                primaryArrowheadPoints[baseLeft].setY( currentLocation + arrowhead.height );
                primaryArrowheadPoints[baseRight].setY( currentLocation + arrowhead.height );
            }
            else
            {
                primaryArrowheadPoints[baseLeft].setY( currentLocation - arrowhead.height );
                primaryArrowheadPoints[baseRight].setY( currentLocation - arrowhead.height );
            }
            primaryArrowheadPoints[top].setY( currentLocation );
        }

        if( i == currentNotchSecondary )
        {
            if( requiredNotchSecondary <= currentNotchSecondary )
            {
                secondaryArrowheadPoints[baseLeft].setY( currentLocation + arrowhead.height );
                secondaryArrowheadPoints[baseRight].setY( currentLocation + arrowhead.height );
            }
            else
            {
                secondaryArrowheadPoints[baseLeft].setY( currentLocation - arrowhead.height );
                secondaryArrowheadPoints[baseRight].setY( currentLocation - arrowhead.height );
            }
            secondaryArrowheadPoints[top].setY( currentLocation );
        }

        currentLocation += graphicalNotchDistance;
    }

    QPolygon primaryArrowheadPolygon;
    primaryArrowheadPolygon << primaryArrowheadPoints[baseLeft] << primaryArrowheadPoints[baseRight] << primaryArrowheadPoints[top];

    if( currentNotchPrimary == requiredNotchPrimary )
    {
        primaryPen.setColor( hobart::colorPalette::green );
        painter.setBrush( hobart::colorPalette::green );
    }
    else
    {
        primaryPen.setColor( hobart::colorPalette::red );
        painter.setBrush( hobart::colorPalette::red );
    }

    painter.setPen( primaryPen );
    painter.drawConvexPolygon( primaryArrowheadPolygon );

    if( hasSecondLabel )
    {
        QPolygon secondaryArrowheadPolygon;
        secondaryArrowheadPolygon << secondaryArrowheadPoints[baseLeft] << secondaryArrowheadPoints[baseRight] << secondaryArrowheadPoints[top];

        if( currentNotchSecondary == requiredNotchSecondary )
        {
            secondaryPen.setColor( hobart::colorPalette::green );
            painter.setBrush( hobart::colorPalette::green );
        }
        else
        {
            secondaryPen.setColor( hobart::colorPalette::red );
            painter.setBrush( hobart::colorPalette::red );
        }

        painter.setPen( secondaryPen );
        painter.drawConvexPolygon( secondaryArrowheadPolygon );
    }
}

// text indicating to the user if the printer is in the correct position (aligned) or not
void PrinterPlatePositioning::drawAlignmentText( QPainter &painter )
{
    QPen primaryPen, secondaryPen;
    primaryPen.setWidth( 3 );
    secondaryPen.setWidth( 3 );

    QFont font;
    font.setPixelSize( 15 );
    font.setBold( true );
    font.setItalic( true );

    QRect primaryTextPosition(
        primaryPlateImageRect.x() - 25,
        primaryPlateImageRect.y() - 75,
        350,
        50
    );

    QRect secondaryTextPosition(
        secondaryPlateImageRect.x() - 25,
        secondaryPlateImageRect.y() - 75,
        375,
        50
    );

    int numberOfNotchesAwayPrimary = abs( requiredNotchPrimary - currentNotchPrimary );
    int numberOfNotchesAwaySecondary = abs( requiredNotchSecondary - currentNotchSecondary );

    QString primaryAlignmentText = "";
    QString secondaryAlignmentText = "";

    if( currentNotchPrimary == requiredNotchPrimary )
    {
        primaryAlignmentText = "Ready";// + QString::number(primaryPlateDistFromHome); // debug
        primaryPen.setColor( hobart::colorPalette::green );

        //primaryTextPosition = QRect( primaryPlateImageRect.x(), primaryPlateImageRect.y() + primaryPlateImageRect.height(), 100, 50 );
    }
    else
    {
        primaryAlignmentText = "Move plate " + QString::number( numberOfNotchesAwayPrimary ) + " notches";
        primaryPen.setColor( hobart::colorPalette::red );
    }

    if( currentNotchSecondary == requiredNotchSecondary )
    {
        if( hasSecondLabel )
        {
            secondaryAlignmentText = "Ready";// + QString::number(secondaryPlateDistFromHome); // debug
            secondaryPen.setColor( hobart::colorPalette::green );
        }

        //secondaryTextPosition = QRect( secondaryPlateImageRect.x(), secondaryPlateImageRect.y() + secondaryPlateImageRect.height(), 100, 50 );
    }
    else
    {
        if( hasSecondLabel )
        {
            secondaryAlignmentText = "Move plate " + QString::number( numberOfNotchesAwaySecondary ) + " notches";
            secondaryPen.setColor( hobart::colorPalette::red );
        }
    }

    if( !isRightDischarge )
    {
        // swap the text since the printers will be reversed
        QString temp = secondaryAlignmentText;
        secondaryAlignmentText = primaryAlignmentText;
        primaryAlignmentText = temp;
    }

    painter.setFont( font );

    if( isRightDischarge )
    {
        painter.setPen( primaryPen );
        painter.drawText( primaryTextPosition, primaryAlignmentText );

        if( hasSecondLabel )
        {
            painter.setPen( secondaryPen );
            painter.drawText( secondaryTextPosition, secondaryAlignmentText );
        }
    }
    else
    {
        painter.setPen( primaryPen );
        //painter.drawText( primaryTextPosition, primaryAlignmentText );
        painter.drawText( secondaryTextPosition, secondaryAlignmentText );

        //painter.drawText( secondaryTextPosition, secondaryAlignmentText );
        if( hasSecondLabel )
        {
            painter.setPen( secondaryPen );
            painter.drawText( primaryTextPosition, primaryAlignmentText );
        }
    }
}

int PrinterPlatePositioning::getScreenSize()
{
    int screen_size = 10;

    QDesktopWidget *desktop = QApplication::desktop();

    int screen = desktop->primaryScreen();

    if ( ( 0 <= screen ) && ( screen < desktop->screenCount() ) )
    {
        QRect g( desktop->screenGeometry( screen ) );

        QSize resolution_ = g.size();

        if( resolution_.width() > 1034 && resolution_.height() > 610 )  // gave it 10 more but do I need to?
            screen_size = 14;
    }

    return screen_size;
}

// gets the notch correlating to a distance in mm
int PrinterPlatePositioning::getNotch( int distance )
{
    int notch = 0;
    for( int i = 0; i < NUM_NOTCHES; ++i)
    {
        // just park the arrow at the last notch if we go over the total number of notches
        if( distance > actualNotchDistances[NUM_NOTCHES - 1] )
        {
            notch = NUM_NOTCHES;
            break;
        }

        if( distance <= ( actualNotchDistances[i] + PLUS_ERROR_MM ) && distance >= ( actualNotchDistances[i] - MINUS_ERROR_MM ) )
        {
            notch = i + 1;
        }
    }

    return notch;
}
