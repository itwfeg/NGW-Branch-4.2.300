/**
 * @file capacitybarwidget.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <filesystem.h>
#include <logmanager.h>

#include "capacitybarwidget.h"

#include "../sessionmainwindow.h"

#include "../dialogs/graphicdialog.h"
#include "../dialogs/networkinfodialog.h"
#include "../dialogs/operatemodedialog.h"
#include "../dialogs/planogramdialog.h"
#include "../dialogs/recalldialog.h"

#include "../modes/operatemode.h"

#include "../screens/labelplacement.h"
#include "../screens/pluinfoscreen.h"
#include "../screens/plumainscreen.h"
#include "../screens/webbrowserscreen.h"
#include "../screens/wrappersettings.h"

#include <backgroundwidgettypes.h>
#include <capacitybarwidgets.h>
#include <palette.h>
#include <pixmap.h>
#include <sessionhooks.h>
#include <QDesktopWidget>

#include <Session.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <Messages.h>

#include <QApplication>
#include <QBitmap>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QSignalMapper>
#include <QVBoxLayout>

const int CapacityBarWidget::pixmapOffset_ = 25;
const bool CapacityBarWidget::convertUppercase_ = true;

///////////////////////////////////////////////////////////////////////////////////////////////////
CapacityBarWidget::CapacityBarWidget( SessionMainWindow *mainWindow, ModeWidget *parent, CapacityBarArea area ) :
    _Mybase( mainWindow, parent ),
    area_( area ),
    moving_( false )
{
    operateMode_ = dynamic_cast<OperateMode*>( mode_ );

    setObjectName( "CapacityBarWidget" );

    // load pixmap
    const QSize backgroundSize( scaleSizeForScreen( QSize( 1024, 44 ) ) );

    const hobart::backgroundWidgetRenderer renderBackground(
        new hobart::capacityBarBackgroundWidget(),
        new hobart::capacityBarBackgroundWidgetRendererAppearance() );

    QPixmap background( renderBackground.drawControl( backgroundSize, hobart::Normal ) );

    if ( BottomCapacityBarArea == area )
    {
        flipPixmap pixmapFlipper( false, true );
        background = pixmapFlipper( background );
    }

    // set palette
    modifyPaletteBackground setBackground( background );
    setBackground( this );

    setAutoFillBackground( true );
    setFixedSize( background.size() );

    // setup mask
    QBitmap bitmap( background.size() );
    bitmap.fill( Qt::color0 );

    QPainter p( &bitmap );

    if ( BottomCapacityBarArea == area )
    {
        p.translate( bitmap.width() / 2, bitmap.height() / 2 );
        p.rotate( 180 );
        p.translate( -bitmap.width() / 2, -bitmap.height() / 2 );
    }

    hobart::capacityBarBackgroundWidget c;
    p.fillPath( c.painterPath( backgroundSize ), Qt::color1 );
    p.fillRect( QRect( backgroundSize.width() / 2, 0, backgroundSize.width() / 4, backgroundSize.height() - 1 ), Qt::color1 );

    setMask( bitmap );

    // setup font
    QFont scaledFont( font() );
    scaledFont.setPointSize( scaleFontPointSizeForScreen( scaledFont.pointSize() ) );

    setFont( scaledFont );

    // initialize
    initialize();
    createLayout();
    translate();

    // populate fields
    setZeroIndicator( "", "", false );
    setScreenIndicator();

    // connect slots
    connect( parent, SIGNAL( operatorLoggedIn() ), SLOT( onOperatorLoggedIn() ) );
    connect( parent, SIGNAL( operatorLoggedOut() ), SLOT( onOperatorLoggedOut() ) );
    connect( parent, SIGNAL( screenChanged() ), SLOT( setScreenIndicator() ) );

    if ( operateMode_ )
    {
        connect( operateMode_, SIGNAL( currentProductTotalsChanged() ), SLOT( onCurrentProductTotalsChanged() ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CapacityBarWidget::~CapacityBarWidget()
{
    // stop continuous network updates
    //session_->send( NetworkConnectivityDev, StopContinuousNetworkStatus );

    // stop continuous date and time
    //session_->send( SessionDev, SnStopDateTime );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QMargins CapacityBarWidget::margins() const
{
    const int pixmapOffset( scaleHeightForScreen( pixmapOffset_ ) );

    QMargins value;

    if ( BottomCapacityBarArea == area_ )
        value.setBottom( pixmapOffset );
    else if ( TopCapacityBarArea == area_ )
        value.setTop( pixmapOffset );

    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::translate()
{
    setCapacityText();
    zeroIndicator_->setText( tr( "ZERO" ) );

    setWeighMode();
    setDateTimeText();

    setDeptName();

    // update hooks
    sessionHooks::instance()->updateFieldValue( sessionHooks::zeroIndicatorString, zeroIndicator_->text() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CapacityBarWidget::eventFilter( QObject *obj, QEvent *e )
{
    // pop operate mode dialog when clicking the label
    if ( scaleMode_ == obj )
        if ( QEvent::MouseButtonRelease == e->type() )
            if ( scaleModeIcon_->isVisible() )
            {
                onOperateModeClicked();
                return true;
            }

    return _Mybase::eventFilter( obj, e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::mousePressEvent( QMouseEvent *e )
{
    LOG_CLASS_INFO( "CapacityBarWidget", "mousePressEvent" );

    if ( Qt::LeftButton == e->button() )
    {
        LOG_DEBUG( "Cap bar mouse press", NULL );
        position_ = mapToParent( e->pos() );
    }

    _Mybase::mousePressEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::mouseMoveEvent( QMouseEvent *e )
{
    LOG_CLASS_INFO( "CapacityBarWidget", "mouseMoveEvent" );

    // allow drag only when left button pressed and dragging enabled in operate mode
    if ( e->buttons().testFlag( Qt::LeftButton ) )
        if (( operateMode_ ) && ( !operateMode_->widgetDragLock() ))
        {
            QPoint mousePos = mapToParent( e->pos() );

            // check for drag
            if ( !moving_ )
            {
                int distance = (mousePos - position_).manhattanLength();

                // drag initiated
                if ( distance >= QApplication::startDragDistance() )
                {
                    LOG_INFO( "Cap bar drag initiated", NULL );
                    moving_ = true;
                }
            }

            // move window
            if ( moving_ )
            {
                int x = pos().x();
                int y = pos().y() + (mousePos.y() - position_.y());

                // do not allow outside parent
                int min = 0;
                int max = mode_->height() - height();

                if ( y < min )
                    y = min;
                else if ( y > max )
                    y = max;

                move( x, y );
                position_ = mousePos;
            }
        }

    _Mybase::mouseMoveEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::mouseReleaseEvent( QMouseEvent *e )
{
    LOG_CLASS_INFO( "CapacityBarWidget", "mouseReleaseEvent" );

    // dropping the capacity bar
    if ( moving_ )
    {
        LOG_INFO( "Cap bar drag complete", NULL );
        moving_ = false;

        // snap window to edge
        int min = 0;
        int max = mode_->height() - height();

        CapacityBarArea where;

        // top
        if ( pos().y() < (min + ((max - min) / 2)) )
        {
            move( pos().x(), min );
            where = TopCapacityBarArea;
        }

        // bottom
        else
        {
            move( pos().x(), max );
            where = BottomCapacityBarArea;
        }

        // emit signal
        Q_EMIT moved( where );
    }

    // clicking (near) scale mode icon
    if (( scaleModeIcon_ ) && ( scaleModeIcon_->isVisible() ))
    {
        QRect r;

        // check icon label
        r = scaleMode_->geometry();

        if (( r.left() <= e->pos().x() ) && ( e->pos().x() < r.right() ))
            onOperateModeClicked();

        // check icon
        r = scaleModeIcon_->geometry();

        if (( r.left() <= e->pos().x() ) && ( e->pos().x() < r.right() ))
        {
            // icon will emit its own signal, so only look for click outside icon
            if (( e->pos().y() < r.top() ) || ( r.bottom() <= e->pos().y() ))
                onOperateModeClicked();
        }
    }

    _Mybase::mouseReleaseEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "CapacityBarWidget", "onSessionMessage" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // weigher config
    // scale config
    if ((( Weigher == device ) && ( WgInquire == event )) ||
        (( ConfigurationDev == device ) && ( CfgScaleWrite == event )))
    {
        setCapacityText();
        setScaleModeIconVisible();
    }
    else if (( NetworkConnectivityDev == device ) && ( NetworkDeviceType == event ))
    {
        MadpNetworkDeviceType m( msg );

        // update
        wirelessIcon_->updateState( &m );
    }

    // weigher status (i.e. weight)
    else if (( Weigher == device ) && ( WgStatus == event ))
    {
        MadpWeightData *weightData = &session_->scaleConfig->weightData;

        setZeroIndicator( weightData->netWeight(), weightData->dualRangeIndicator(), weightData->zeroFlag() );
    }

    // weigh mode configuration
    else if ((( ConfigurationDev == device ) && ( CfgPrepackWrite == event )) ||
        (( ConfigurationDev == device ) && ( CfgAccumulateItemsWrite == event )))
    {
        setScaleModeIconVisible();
    }

    // weigh mode
    else if (( SessionDev == device ) && ( SnWriteWeighModeTransactionType == event ))
    {
        setWeighMode();
    }

    // date and time
    else if (( SessionDev == device ) && ( SnWriteDateTime == event ))
    {
        setDateTimeText();
    }

    // printer status errors are queued up
    else if (( Printer == device ) && ( PrStatusErrors == event ))
    {
        LOG_INFO( "printer in error state", NULL );

        // blink the printer
        printerIcon_->setAlertState( true );
    }

    // no printer status errors are queued
    else if (( Printer == device ) && ( PrStatusClear == event ))
    {
        LOG_INFO( "printer error state cleared", NULL );

        // quit blinking the printer
        printerIcon_->setAlertState( false );
    }

    // planograms are queued up
    else if (( GraphicDev == device ) && ( WriteMostRecentPlanogramGraphic == event ))
    {
        DomGraphicNameWriteMostRecentPlanogramGraphic m( msg );

        LOG_INFO( "planogram in error state", NULL );

        mostRecentPlanogram_ = m.graphicName();

        // blink the planogram
        planogramIcon_->setAlertState( mostRecentPlanogram_.length() ? true : false );
    }

    // network status
    else if (( NetworkConnectivityDev == device ) && ( CurrentStatus == event ))
    {
        MadpNetworkConnectivity m( msg );

        // update
        wirelessIcon_->updateState( &m );
    }

    // recall
    else if (( RecallsDev == device ) && ( RecallsDevNewRecallsExist == event ))
    {
        DomRecallsExist m( msg );

        // alert state
        recallIcon_->setAlertState( m.existFlag() );
    }

    // show User the dept
    else if (( DeptDev == device ) && ( DeptDevDtWrite == event ))
    {
        DomCurrentDept m( msg );

        // SOF-924
		if(m.deptNum() == 0)
        {
            deptNamePrimary = "";
            deptNameSecondary = "";
        }
        else
        {
            deptNamePrimary = m.name1();
            deptNameSecondary = m.name2();
        }

        setDeptName();

        getSizes();
    }

    // register session
    else if (( SessionDev == device ) && ( SnRegister == event ))
    {
        LOG_INFO( "session has been registered!", NULL );

        // request current network status to quickly update the NIC type
        session_->send( NetworkConnectivityDev, RequestNetworkDeviceType );

        //LOG_INFO( "network current status requested", NULL );
        // clear printer errors
        // Walmart remote print - One click printing
        // In Rev 1297 the printer status icon was created and set up. During the setup the printer status
        // flag in the Application is cleared containing any PrStatus flags.  This clear, however, adversely
        // affects walmart remote print. So if there is a Label Missing status at boot, and after boot we send
        // a printRemoteLabel command, the status doesnt get reported and it appears the label prints.  It doesnt
        // start working properly until a PLU is requested on the scale and the status is reported. I could not
        // think of a good reason not to report the status at boot, so I am commenting this line out and will
        // keep it commented out until someone can find a good reason not to.

        //session_->send( Printer, PrClearCurrentStatus );

        LOG_INFO( "pinter errors cleared", NULL );
        // request weigher info
        session_->send( Weigher, WgReqInquire );

        LOG_INFO( "weigher info requested", NULL );
        // request continuous date and time
        session_->send( SessionDev, SnContinuousDateTime );

        LOG_INFO( "continuous date+time requested", NULL );
        // request scale mode
        session_->send( SessionDev, SnReadWeighModeTransactionType );

        LOG_INFO( "scale mode requested", NULL );
        // request continuous network updates
        session_->send( NetworkConnectivityDev, StartContinuousNetworkStatus );

        LOG_INFO( "dept requested", NULL );
        session_->send(DeptDev, DeptDevDtReadCurrentDept);

        LOG_INFO( "continuous network updates requested", NULL );
    }

    // database restore
    else if (( BackupRestoreDev == device ) && ( Terminate  == event ))
    {
        // FT-1115 request configuration after db restore
        session_->send( Weigher, WgReqInquire );
        session_->send( SessionDev, SnReadWeighModeTransactionType );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::onIconClicked( QWidget *w )
{
    LOG_CLASS_INFO( "CapacityBarWidget", "onIconClicked" );

    if ( !w )
        return;

    if ( recallIcon_ == w )
    {
        LOG_INFO( "showing recalls dialog", NULL );

        RecallDialog *dialog = new RecallDialog( mainWindow_ );

        // show dialog!
        dialog->open();
    }
    else if ( printerIcon_ == w )
    {
        // request errors
        session_->send( Printer, PrReqCurrentStatus );
    }
    else if ( wirelessIcon_ == w )
    {
        const MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

        // FT-1197
        bool enableNetworkDetails( scaleConfig->enableDisplayNetworkDetails() );

        // show dialog when enabled
        if ( !enableNetworkDetails )
            LOG_TRACE( "network info dialog is disabled", NULL );
        else
        {
            LOG_INFO( "showing network info dialog", NULL );

            NetworkInfoDialog *dialog = new NetworkInfoDialog( mainWindow_ );

            // show dialog!
            dialog->open();
        }
    }
    else if ( planogramIcon_ == w )
    {
        // when planogram is blinking we are to only show the most recent planogram image
        if ( planogramIcon_->alertState() )
        {
            QString filename( filesystem::absoluteFilePath( mostRecentPlanogram_, folderType::PlanogramImages ) );

            LOG_INFO( "have most recent planogram '%s'", qPrintable( filename ) );

            if ( filename.length() )
            {
                 // create dialog
                 GraphicDialog *dialog = new GraphicDialog( filename, (QWidget*) mainWindow_ );

                 // validate graphic
                 if ( !dialog->isValid() )
                 {
                     LOG_WARN( "graphic '%s' was not valid", qPrintable( filename ) );
                     dialog->deleteLater();
                 }

                 // show
                 else
                 {
                     LOG_DEBUG( "showing dialog for graphic '%s'", qPrintable( filename ) );

                     // acknowledge the graphic
                     session_->send( GraphicDev, AcknowledgeMostRecentPlanogramGraphic );

                     // stop alerting
                     planogramIcon_->setAlertState( false );

                     // show dialog!
                     dialog->open();
                 }
            }
        }

        // otherwise bring up list
        else
        {
            LOG_INFO( "showing planograms dialog", NULL );

            PlanogramDialog *dialog = new PlanogramDialog( mainWindow_ );

            // show dialog!
            dialog->open();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::onCurrentProductTotalsChanged()
{
    if ( !operateMode_ )
        return;

    DomCurrentPLU *product( operateMode_->currentProduct() );

    // no product
    if ( product->itemNumber().isEmpty() )
        return;

    MadpUpdateCurrentPLUTotals *productTotals( operateMode_->currentProductTotals() );

    setZeroIndicator( productTotals->totalNetWeight(), productTotals->dualRangeIndicator(), productTotals->zeroFlag() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::onOperateModeClicked()
{
    OperateModeDialog *dialog = new OperateModeDialog( mainWindow_ );

    // open dialog
    dialog->open();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::onOperatorLoggedIn()
{
    getSizes();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::onOperatorLoggedOut()
{
    operatorName_->setText( "" );

    setDeptName();

    getSizes();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::onSwipeScreenLeft()
{
    ModeWidget *mode = qobject_cast<ModeWidget*>( parent() );
    ScreenWidget *screen = (mode ? mode->screen() : NULL);

    if ( screen )
        screen->handleSwipe( ScreenWidget::Left );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::onSwipeScreenRight()
{
    ModeWidget *mode = qobject_cast<ModeWidget*>( parent() );
    ScreenWidget *screen = (mode ? mode->screen() : NULL);

    if ( screen )
        screen->handleSwipe( ScreenWidget::Right );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::setCapacityText()
{
    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

    QString range;
    QString dualRange;
    QString capacity( tr( "Capacity" ) );

    // weigher attached
    if ( scaleConfig->weigherAttached().toInt() )
        if( !( session_->uiUtilities->isWrapOnly() ) )
            capacity_->show();

    // no weigher
    else
    {
        capacity_->hide();
        scaleMax_->hide();
        scaleDualMax_->hide();
        return;
    }

    // convert case if required
    if ( convertUppercase_ )
        capacity = capacity.toUpper();

    // retrieve strings
    QString capacityString( session_->uiUtilities->setupCapacityString( capacity, range, dualRange ) );

    // set strings
    capacity_->setText( capacity );
    scaleMax_->setText( range );
    scaleDualMax_->setText( dualRange );

    // handle dual range
    scaleMax_->setVisible( !range.isEmpty() );
    scaleDualMax_->setVisible( !dualRange.isEmpty() );

    // update hooks
    sessionHooks::instance()->updateFieldValue( sessionHooks::capacityString, capacity_->text() );
    sessionHooks::instance()->updateFieldValue( sessionHooks::capacityIndicatorString, scaleMax_->text() );
    sessionHooks::instance()->updateFieldValue( sessionHooks::dualCapacityIndicatorString, scaleDualMax_->text() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::setZeroIndicator( const QString& netWeight, const QString& dualRangeIndicator, bool zeroFlag )
{
    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

    // no weigher
    if ( !scaleConfig->weigherAttached().toInt() )
    {
        zeroIndicator_->hide();

        // update hooks
        sessionHooks::instance()->updateFieldValue( sessionHooks::zeroIndicator, false );

        return;
    }

    // Note:
    // Dual range indicator does not work at the moment!!
    if ( dualRangeIndicator.length() )
    {
        // dual range indicator is '2g' or '5g'
        bool lowRange = ("2g" == dualRangeIndicator);
        // range indicator
        if ( scaleDualMax_->isVisible() )
        {
            scaleMax_->setIndicatorVisible( !lowRange );
            scaleDualMax_->setIndicatorVisible( lowRange );
        }
        else
        {
            // for non-dual range scale do not set an indicator as nothing needs to be indicated
            scaleMax_->setIndicatorVisible( false );
            scaleDualMax_->setIndicatorVisible( false );
        }

        // update hooks
        sessionHooks::instance()->updateFieldValue( sessionHooks::capacityIndicator, !lowRange );
        sessionHooks::instance()->updateFieldValue( sessionHooks::dualCapacityIndicator, lowRange );
    }

    else
    {
        int netWeightLength = netWeight.length();
        int netWeightIndexOfDecimal;

        DomCurrentPLU *product( operateMode_->currentProduct() );

        if(product->keyboardNetWeight() == "")
        {
            if ( netWeightLength)
            if ( 0 < (netWeightIndexOfDecimal = netWeight.indexOf( "." )) )
            {
                // number of digits starting at the decimal (i.e. ".000")
                bool lowRange = (4 == (netWeightLength - netWeightIndexOfDecimal));

                // range indicator
                if ( scaleDualMax_->isVisible() )
                {
                    scaleMax_->setIndicatorVisible( !lowRange );
                    scaleDualMax_->setIndicatorVisible( lowRange );
                }
                else
                {
                    // for non-dual range scale do not set an indicator as nothing needs to be indicated
                    scaleMax_->setIndicatorVisible( false );
                    scaleDualMax_->setIndicatorVisible( false );
                }

                // update hooks
                sessionHooks::instance()->updateFieldValue( sessionHooks::capacityIndicator, !lowRange );
                sessionHooks::instance()->updateFieldValue( sessionHooks::dualCapacityIndicator, lowRange );
            }
        }
    }

    // zero indicator
    if( !( session_->uiUtilities->isWrapOnly() ) )
        zeroIndicator_->setVisible( zeroFlag );

    // update hooks
    sessionHooks::instance()->updateFieldValue( sessionHooks::zeroIndicator, zeroFlag );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::setScreenIndicator()
{
    ScreenWidget *screen = NULL;

    if ( operateMode_ )
        screen = operateMode_->screen();

    if ( !screen )
        return;

    int currentScreen( -1 );

    //
    // what screen is currently showing
    //

    LabelPlacementScreen *labelPlacementScreen = NULL;
    PluInfoScreen *pluInfoScreen = NULL;
    PluMainScreen *pluMainScreen = NULL;
    WebBrowserScreen *webBrowserScreen = NULL;
    WrapperSettingsScreen *wrapperSettingsScreen = NULL;

    if ( !(pluMainScreen = dynamic_cast<PluMainScreen*>( screen )) )
    {
        labelPlacementScreen = dynamic_cast<LabelPlacementScreen*>( screen );
        pluInfoScreen = dynamic_cast<PluInfoScreen*>( screen );
        webBrowserScreen = dynamic_cast<WebBrowserScreen*>( screen );
        wrapperSettingsScreen = dynamic_cast<WrapperSettingsScreen*>( screen );
    }

    //
    // compute screen index
    //

    // Access
    if ( session_->isPrepack() )
    {
        // check what screen is the right screen...
        // 0 = PluInfoScreen
        // 1 = WrapperSettingsScreen
        // 2 = LabelPlacementScreen
        int rightScreen( session_->scaleConfig->scaleConfig.rightScreenType() );

        if ( pluMainScreen )
            currentScreen = 1;
        else if ( webBrowserScreen )
            currentScreen = 0;
        else if (( 0 == rightScreen ) && ( pluInfoScreen ))
            currentScreen = 2;
        else if (( 1 == rightScreen ) && ( wrapperSettingsScreen ))
            currentScreen = 2;
        else if (( 2 == rightScreen ) && ( labelPlacementScreen ))
            currentScreen = 2;
    }

    // HTs (no left panel)
    else if ( !operateMode_->isCached<WebBrowserScreen>() )
    {
        if ( pluMainScreen )
            currentScreen = 0;
        else if ( pluInfoScreen )
            currentScreen = 1;
    }

    // HTi/x
    else
    {
        if ( pluMainScreen )
            currentScreen = 1;
        else if ( webBrowserScreen )
            currentScreen = 0;
        else if ( pluInfoScreen )
            currentScreen = 2;
    }

    //
    // update indicator
    //

    if ( currentScreen < 0 )
        screenIndicatorIcon_->setVisible( false );
    else
    {
        screenIndicatorIcon_->setScreen( currentScreen );
        screenIndicatorIcon_->setFixedSize( screenIndicatorIcon_->calculateSize() );

        if( !( session_->uiUtilities->isWrapOnly() ) )
            screenIndicatorIcon_->setVisible( true );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::setWeighMode()
{
    DomWeighModeTotalType *msg = &session_->scaleConfig->weighModeTotalType;
    QString mode;

    // handle weight mode
    switch ( msg->weighMode() )
    {
    case OpModeIdle:
        mode = tr( "Logged Out" );
        break;
    case OpModeOperate:
        mode = tr( "Operate" );
        break;
    case OpModePluEntry:
        mode = tr( "Plu Entry" );
        break;
    case OpModeAutoWeigh:
        mode = tr( "Auto Weigh" );
        break;
    case OpModeSemiWeigh:
        mode = tr( "Semi Weigh" );
        break;
    case OpModeManualWeigh:
        mode = tr( "Manual" );
        break;
    case OpModeSupervisor:
        mode = tr( "Supervisor" );
        break;
    case OpModeService:
        mode = tr( "Service" );
        break;
    case OpModeServiceLogin:
        mode = tr( "Service Login" );
        break;
    case OpModePowerDown:
        mode = tr( "Power Down" );
        break;
    case OpModeMonitor:
        mode = tr( "Monitor" );
        break;
    case OpModeWrapOnly:
        mode = tr( "Wrap Only" );
        break;
    case OpModeStatus:
        mode = tr( "Status" );
        break;
    case OpModeSelfService:
        mode = tr( "Self Service" );
        break;
    case OpModeAutoManualApply:
        mode = tr( "Prepack" ); // "Automatic"
        break;
    case OpModeProductionEntry:
        mode = tr( "PE Manual" ); // "Production Entry"
        break;
    case OpModePrintMultipleLabels:
        mode = tr( "Multiple Labels" );
        break;
    case OpModeAccumulateItems:
        mode = tr( "AI Manual" ); // "Accumulate Item"
        break;
    case OpModeMealBuilder:
        mode = tr( "Meal" ); // "Build-A-Meal"
        break;
    default:
        mode = "[" + QString::number(msg->weighMode()) +"]" + tr( "Unknown" );
        // This should never ever happen, so send a message to the back-end to indicate this
        session_->send( SessionDev, SnInvalidModeDetected, QString::number(msg->weighMode()) );
        break;
    }

    // FT-605: ignore supervisor and service modes (i.e. do not update mode text)
    if (( OpModeSupervisor == msg->weighMode() ) ||
        ( OpModeService == msg->weighMode() ) ||
        ( OpModeServiceLogin == msg->weighMode() ))
    {
        // do not update cap bar... just confuses people (as per Jim Highley)
        return;
    }

    // convert case if required
    if ( convertUppercase_ )
        mode = mode.toUpper();

    scaleMode_->setText( mode );

    QFontMetrics sm(scaleMode_->fontMetrics());

    ///////////////////////

    getSizes();

    operatorName_->repaint();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::setDateTimeText()
{
    QString text( session_->uiUtilities->updateDateTime() );

    // convert case if required
    if ( convertUppercase_ )
        text = text.toUpper();

    dateTime_->setText( text );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::initialize()
{
    const int pixmapOffset( scaleHeightForScreen( pixmapOffset_ ) );

    QList<QWidget*> widgets;

    deptNamePrimary = "";
    deptNameSecondary = "";

    ///////////////////////////////////////////////////////////////////////////
    // left side

    // capacity
    capacity_ = new QLabel( this );
    capacity_->setObjectName( "capacity" );
    widgets.push_back( capacity_ );

    // range
    scaleMax_ = new hobart::indicatorLabel( this );
    scaleMax_->setObjectName( "scaleMax" );
    widgets.push_back( scaleMax_ );

    scaleMax_->setIndicatorVisible( true );

    // dual range
    scaleDualMax_ = new hobart::indicatorLabel( this );
    scaleDualMax_->setObjectName( "scaleDualMax" );
    widgets.push_back( scaleDualMax_ );

    // zero indicator
    zeroIndicator_ = new hobart::indicatorLabel( this );
    zeroIndicator_->setObjectName( "zeroIndicator" );
    widgets.push_back( zeroIndicator_ );

    zeroIndicator_->setIndicatorDirection( hobart::indicatorLabel::Down );
    zeroIndicator_->setIndicatorVisible( true );

    // screen indicator
    int screenCount( 3 );

    // HTs scale (no left panel)
    if ( !operateMode_->isCached<WebBrowserScreen>() )
        screenCount = 2;

    screenIndicatorIcon_ = new hobart::screenIndicatorIcon( screenCount, this );
    screenIndicatorIcon_->setFixedSize( pixmapOffset, pixmapOffset );
    screenIndicatorIcon_->setObjectName( "screenIndicatorIcon" );
    screenIndicatorIcon_->setVisible( false );

    connect( screenIndicatorIcon_, SIGNAL( swipeScreenLeft() ), SLOT( onSwipeScreenLeft() ) );
    connect( screenIndicatorIcon_, SIGNAL( swipeScreenRight() ), SLOT( onSwipeScreenRight() ) );

    ///////////////////////////////////////////////////////////////////////////
    // alerts

    const int iconSize( scaleHeightForScreen( 36 ) );

    // signal mapper
    signalMapper_ = new QSignalMapper( this );
    connect( signalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onIconClicked(QWidget*) ) );

    // recall
    recallIcon_ = new hobart::recallIcon( this );
    recallIcon_->setFixedSize( iconSize, iconSize );
    recallIcon_->setObjectName( "recallIcon" );

    signalMapper_->setMapping( recallIcon_, recallIcon_ );
    connect( recallIcon_, SIGNAL( clicked() ), signalMapper_, SLOT( map() ) );

    // printer
    printerIcon_ = new hobart::printerIcon( this );
    printerIcon_->setFixedSize( iconSize, iconSize );
    printerIcon_->setObjectName( "printerIcon" );

    signalMapper_->setMapping( printerIcon_, printerIcon_ );
    connect( printerIcon_, SIGNAL( clicked() ), signalMapper_, SLOT( map() ) );

    // planogram
    planogramIcon_ = new hobart::planogramIcon( this );
    planogramIcon_->setFixedSize( iconSize, iconSize );
    planogramIcon_->setObjectName( "planogramIcon" );

    signalMapper_->setMapping( planogramIcon_, planogramIcon_ );
    connect( planogramIcon_, SIGNAL( clicked() ), signalMapper_, SLOT( map() ) );

    // wireless
    wirelessIcon_ = new hobart::wirelessSignalIcon( this );
    wirelessIcon_->setFixedSize( iconSize, iconSize );
    wirelessIcon_->setObjectName( "wirelessIcon" );

    signalMapper_->setMapping( wirelessIcon_, wirelessIcon_ );
    connect( wirelessIcon_, SIGNAL( clicked() ), signalMapper_, SLOT( map() ) );

    ///////////////////////////////////////////////////////////////////////////
    // right side

    // scale mode
    scaleMode_ = new QLabel( this );
    scaleMode_->setObjectName( "scaleMode" );
    widgets.push_back( scaleMode_ );

    scaleMode_->installEventFilter( this );

    scaleModeIcon_ = new hobart::dropDownArrowIcon( this );
    scaleModeIcon_->setFixedSize( pixmapOffset, pixmapOffset );
    scaleModeIcon_->setObjectName( "scaleModeIcon" );

    connect( scaleModeIcon_, SIGNAL( clicked() ), SLOT( onOperateModeClicked() ) );

    // mode is hidden for wrapper
    if ( session_->isPrepack() )
    {
        scaleMode_->hide();
        scaleModeIcon_->hide();
    }

    // operator
    operatorName_ = new QLabel( this );
    operatorName_->setObjectName( "operatorName" );

    operatorName_->setScaledContents(false);            // sets if we will scale the contents of this field.
    widgets.push_back( operatorName_ );

    // dept name
    deptName_ = new QLabel(this);
    deptName_->setObjectName("deptName");
    QPalette palette = deptName_->palette();
    palette.setColor(deptName_->foregroundRole(), hobart::colorPalette::lightBlue);
    deptName_->setPalette(palette);
    deptName_->setScaledContents(false);
    widgets.push_back( deptName_ );

    // spacer
    spacer_ = new QLabel( this );
    spacer_->setObjectName( "spacer" );
    spacer_->setScaledContents(false);
    spacer_->setMaximumWidth(0);
    spacer_->setMinimumWidth(0);
    spacer_->setText("");
    widgets.push_back(spacer_ );

    // date and time
    dateTime_ = new QLabel( this );
    dateTime_->setObjectName( "dateTime" );
    widgets.push_back( dateTime_ );

    ///////////////////////////////////////////////////////////////////////////
    // groom labels

    // adjust each field for height and width
    foreach ( QWidget *widget, widgets )
    {
        QLabel *label( dynamic_cast<QLabel*>( widget ) );

        if ( label )
        {
            if (label->objectName() == "operatorName")
            {
               label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            }
            else if (label->objectName() == "deptName")
            {
               label->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
            }
            else
            {
               label->setAlignment( Qt::AlignCenter );
            }
        }
        widget->setFixedHeight( pixmapOffset_ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::createLayout()
{
    QHBoxLayout *leftLayout( new QHBoxLayout() );
    leftLayout->addWidget( capacity_ );
    leftLayout->addWidget( scaleMax_ );
    leftLayout->addWidget( scaleDualMax_ );
    leftLayout->addStretch();
    leftLayout->addWidget( zeroIndicator_ );
    leftLayout->addStretch();
    leftLayout->addWidget( screenIndicatorIcon_ );

    QHBoxLayout *alerts( new QHBoxLayout() );
    alerts->addStretch();
    alerts->addWidget( recallIcon_ );
    alerts->addWidget( printerIcon_ );
    alerts->addWidget( planogramIcon_ );
    alerts->addWidget( wirelessIcon_ );
    alerts->addStretch();

    QVBoxLayout *middleLayout( new QVBoxLayout() );
    middleLayout->addLayout( alerts );
    middleLayout->addStrut( scaleWidthForScreen( 168 ) );

    QHBoxLayout *rightLayout( new QHBoxLayout() );
    rightLayout->addWidget( scaleMode_ );
    rightLayout->addWidget( scaleModeIcon_ );
    rightLayout->addWidget( operatorName_ );
    rightLayout->addStretch();
    rightLayout->addWidget( deptName_ );
    rightLayout->addWidget(spacer_);
    rightLayout->addWidget( dateTime_ );

    if ( TopCapacityBarArea == area_ )
    {
        const int pixmapOffset( scaleHeightForScreen( pixmapOffset_ ) );

        leftLayout->setContentsMargins( QMargins( 10, 0, 10, height() - pixmapOffset ) );
        middleLayout->setContentsMargins( QMargins( 0, 5, 0, 0 ) );
        rightLayout->setContentsMargins( QMargins( 10, 0, 7, height() - pixmapOffset ) );
    }
    else
    {
        const int pixmapOffset( scaleHeightForScreen( pixmapOffset_ ) );

        leftLayout->setContentsMargins( QMargins( 10, height() - pixmapOffset, 10, 0 ) );
        middleLayout->setContentsMargins( QMargins( 0, 5, 0, 0 ) );
        rightLayout->setContentsMargins( QMargins( 10, height() - pixmapOffset, 10, 0 ) );
    }

    QHBoxLayout *screenLayout( new QHBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    screenLayout->addLayout( leftLayout, 1 );
    screenLayout->addLayout( middleLayout, 0 );
    screenLayout->addLayout( rightLayout, 1 );

    if( session_->uiUtilities->isWrapOnly() )
    {
        capacity_->hide();
        scaleMax_->hide();
        scaleDualMax_->hide();
        zeroIndicator_->hide();
        screenIndicatorIcon_->hide();

        printerIcon_->hide();

        scaleMode_->hide();
        scaleModeIcon_->hide();
        operatorName_->hide();
    }
}

void CapacityBarWidget::getSizes()
{
    LOG_CLASS_INFO( "CapacityBarWidget", "getSizes" );

    ///     LOG_DEBUG( "XXXXXXXXXXXXXXXXXXx", NULL );

    operatorName_->setText(mode_->operatorName());
    int dateLength = dateTime_->fontMetrics().boundingRect(dateTime_->text()).width();
    int deptLength = deptName_->fontMetrics().boundingRect(deptName_->text()).width();

    int scaleModeLength = scaleMode_->fontMetrics().boundingRect(scaleMode_->text()).width();;
    int scaleModeIconLength = scaleModeIcon_->width();
    int maxDeptLength = 90;
    int newOperatorLength = 0;

    // Test other sizes
    QDesktopWidget *desktop = QApplication::desktop();
    int app_w = 1024;

    if ( desktop )
    {
        const QRect desktopRect( desktop->screenGeometry( 0 ) );  // 1024 = 10 inch | 1366 = 14 inch | 1920 = TV, my monitor
        app_w = desktopRect.width();
        //app_w = 1024;       // jhc testing
    }

    // half the capacitybar width
    int overallLength = (app_w/2) - 140;         // what about larger desktops resoltions?

    //if( 1024 == app_w )
    //    overallLength -= 40;
    //else if( 1366 == app_w )
    //    overallLength -= 40;

    int availableDeptOpLength = overallLength - ( dateLength + scaleModeLength + scaleModeIconLength );

    bool resizeDept = true;
    if( deptName_->text().length() == 0 )
        resizeDept = false;

    if( resizeDept )
    {
        if(deptLength > maxDeptLength)
        {
            deptLength = maxDeptLength;
        }

        deptLength += 4;
        deptName_->setMaximumWidth(deptLength);
    }
    else
    {
        deptName_->setMaximumWidth(4);
    }

    deptName_->repaint();

    newOperatorLength = availableDeptOpLength - deptLength - 10;

    int pixelsO = operatorName_->fontMetrics().boundingRect(operatorName_->text()).width();

    //Uncomment for testing
    /*
    LOG_DEBUG( "XXXXXXXXXXXXXXXXXXx", NULL );
    LOG_DEBUG( "scaleModeLength '%s'", qPrintable( QString::number(scaleModeLength) ) );
    LOG_DEBUG( "scaleIconLength '%s'", qPrintable( QString::number(scaleModeIconLength) ) );
    LOG_DEBUG( "departmentLength '%s'", qPrintable( QString::number(deptLength) ) );
    LOG_DEBUG( "dateTimeLength '%s'", qPrintable( QString::number(dateLength) ) );
    LOG_DEBUG( "overallLength '%s'", qPrintable( QString::number(overallLength) ) );
    LOG_DEBUG( "name:'%s' - pixels0:'%s'", qPrintable(operatorName_->text()), qPrintable( QString::number(pixelsO) ) );
    LOG_DEBUG( "newOperatorLength '%s'", qPrintable( QString::number(newOperatorLength) ) );
    LOG_DEBUG( "deptName '%s'", qPrintable(deptName_->text() ) );
    LOG_DEBUG( "XXXXXXXXXXXXXXXXXXx", NULL );
    */
    //refactor below when time available

    if( pixelsO + 10 > newOperatorLength)
    {
        LOG_DEBUG( "Longer so splitting into two lines", NULL );

        formattedOperatorName_ = "";
        //split and reduce
        QStringList operatorNameList = mode_->operatorName().split(" ");
        int count = operatorNameList.count();
        int lastName = 0;
        QString firstName = "";

        for(int i = 0; i < count; i++)
        {
            if (count > 1)
            {
                if(i != 0)
                {
                    if(i == (count - 1))
                    {
                        firstName = formattedOperatorName_;
                        lastName = i;
                        formattedOperatorName_ += "\n";
                    }
                    else
                    {
                        formattedOperatorName_ += " ";
                    }
                }
                formattedOperatorName_ += operatorNameList.at(i);

            }
            else formattedOperatorName_ = operatorNameList.at(0);
        }
    }
    else
    {
        formattedOperatorName_ = mode_->operatorName();
        if( resizeDept )
            newOperatorLength = pixelsO + 5;
        else
            newOperatorLength += 5;
    }

    operatorName_->setText(formattedOperatorName_);
    //operatorName_->setGeometry(nameX,nameY,newOperatorLength,nameHeight);     // needed?

    pixelsO = operatorName_->fontMetrics().boundingRect(operatorName_->text()).width(); // test again
    operatorName_->setMaximumWidth(newOperatorLength);


    operatorName_->repaint();
    deptName_->repaint();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CapacityBarWidget::setScaleModeIconVisible()
{
    // mode is hidden for wrapper
    if ( session_->isPrepack() )
        return;

    // hide/show scale mode icon based upon if we can actually set anything
    scaleModeIcon_->setVisible( OperateModeDialog::isEnabled( session_->scaleConfig ) );
}

// SOF-924
void CapacityBarWidget::setDeptName()
{
    if ( 1 == session_->scaleConfig->currentLanguage )
        deptName_->setText( deptNamePrimary );
    else
        deptName_->setText( deptNameSecondary );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QRect CapacityBarWidget::calcFieldGeometry( const QWidget *widget ) const
{
    int top = widget->pos().y();

    if ( BottomCapacityBarArea == area_ )
        top = height() - widget->height() - top;

    QRect rect( widget->geometry() );

    return QRect( QPoint( rect.left(), top ), rect.size() );
}
