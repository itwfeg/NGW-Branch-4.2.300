/**
 * @file plumainscreen.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>
#include <sessionhooks.h>

#include "appsscreen.h"
#include "labelplacement.h"
#include "buildamealscreen.h"
#include "meatspecsheetscreen.h"
#include "plumainscreen.h"
#include "pluinfoscreen.h"
#include "webbrowserscreen.h"
#include "wrappersettings.h"

#include "../numberpadwidget.h"
#include "../sessionmainwindow.h"

#include "../dialogs/numericinputboxdialog.h"
#include "../dialogs/wrapperforcetraysizedialog.h"

#include "../widgets/dynamiccontentwidget.h"
#include "../widgets/dynamicsoftkeybutton.h"
#include "../widgets/labeledcooledit.h"
#include "../widgets/labeleddateedit.h"
#include "../widgets/labelednumericlineedit.h"
#include "../widgets/labeledplaintextedit.h"
#include "../widgets/labeledpluedit.h"
#include "../widgets/webViewer.h"
#include "../sessionHandlerWidgets/softkeyswidget.h"

#include <backgroundwidgettypes.h>
#include <filesystem.h>
#include <font.h>
#include <softkeyswidget.h>

#include <Messages.h>
#include <Session.h>
#include <ScaleConfiguration.h>
#include <TokenParser.h>
#include <UiUtilities.h>

#include <QHBoxLayout>
#include <QIntValidator>
#include <QSettings>
#include <QSignalMapper>
#include <QTimer>
#include <QVBoxLayout>
#include <QWebView>
#include <QWebFrame>
#include <ScaleClasses.h>

const bool PluMainScreen::convertUppercase_ = true;

const int PluMainScreen::pluValidatorBottom_ = 0;
const int PluMainScreen::pluValidatorTop_ = 999999;

static const int BUILD_A_MEAL_SCREEN = 1;
static const int CUT_TEST_ONE_SCREEN = 2;

static const int MENU_OVERLAY_SPACING = 45;
static const bool FAKE_WRAPPER = false;

const int PluMainScreen::dcaPrimaryWidgets_ =
    hobart::dynamicContentWidget::Flashkeys |
    hobart::dynamicContentWidget::WebBrowser |
    hobart::dynamicContentWidget::BestSellerList |
    hobart::dynamicContentWidget::FixedFlashkeys |
    hobart::dynamicContentWidget::AccumulateItems |
    hobart::dynamicContentWidget::FreshProduction |
    hobart::dynamicContentWidget::OperatorMessage |
    hobart::dynamicContentWidget::TareFlashkeys |
    hobart::dynamicContentWidget::CutTestFlashkeysDefault |
    hobart::dynamicContentWidget::CutTestFlashkeysOptionOne;

const int PluMainScreen::dcaPrimaryWidgetsExcluded_ =
    hobart::dynamicContentWidget::AccumulateItems |
    hobart::dynamicContentWidget::FreshProduction |
    hobart::dynamicContentWidget::OperatorMessage |
    hobart::dynamicContentWidget::TareFlashkeys |
    hobart::dynamicContentWidget::CutTestFlashkeysDefault |
    hobart::dynamicContentWidget::CutTestFlashkeysOptionOne;

const int PluMainScreen::dcaPrimarySoftkeyGroup_ = hobart::softkeysWidget::RunScreenButtons;

const int PluMainScreen::dcaSecondaryWidgets_ =
    hobart::dynamicContentWidget::NumberPad |
    hobart::dynamicContentWidget::Flashkeys |
    //hobart::dynamicContentWidget::BestSellerList | // FT-669
    hobart::dynamicContentWidget::FixedFlashkeys |
    hobart::dynamicContentWidget::ProductImage;

const int PluMainScreen::dcaSecondaryWidgetsExcluded_ = 0;

const int PluMainScreen::dcaSecondarySoftkeyGroup_ = hobart::softkeysWidget::RunScreenNumericKeypadButtons;

const int PluMainScreen::dcaEnhancedCenterScreenWidgets_ =
    hobart::dynamicContentWidget::Flashkeys |
    hobart::dynamicContentWidget::WebBrowser |
    hobart::dynamicContentWidget::BestSellerList |
    hobart::dynamicContentWidget::FixedFlashkeys;

const int PluMainScreen::dcaEnhancedCenterScreenWidgetsExcluded_ =
    hobart::dynamicContentWidget::Flashkeys;

const int PluMainScreen::dcaEnhancedCenterScreenSoftkeyGroup_ = hobart::softkeysWidget::RunScreenEnhancedCenterScreen;

///////////////////////////////////////////////////////////////////////////////////////////////////
PluMainScreen::PluMainScreen( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    dcaPrimaryWebView_( NULL ),
    dcaSecondaryWebView_( NULL ),
    dcaEnhancedCenterScreenWebView_( NULL ),
    dcaPrimaryWebViewProgress_( -1 ),
    dcaSecondaryWebViewProgress_( -1 ),
    dcaEnhancedCenterScreenWebViewProgress_( -1 ),
    forceCoolDialog_( false ),
    forcePackedOnDateDialog_( false ),
    forceNumberOfLabelsDialog_( false ),
    forceNetWeightDialog_( false ),
    forceExpirationDaysDialog_( false ),
    forceTraySizeDialog_( false ),
    traySizesRequired_( false ),
    traySizesExist_( false ),
    ignoreSignals_( true ),
    registered_( false ),
    printBeforeNumOfLabelsEntered_( false ),
    networkOnline_( false )
{
    operateMode_ = dynamic_cast<OperateMode*>( mode_ );

    setObjectName( "PluMainScreen" );

    // initialize screen
    initialize();
    createLayout();
    translate();

    // setup screen
    clearFields();

    setOperatorDisplayFields();

    // connect slots
    connect( mode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ) );

    if ( operateMode_ )
    {
        connect( operateMode_, SIGNAL( clearPluForCutTest() ),            SLOT( onClearPluForCutTest() ) );
        connect( operateMode_, SIGNAL( currentProductChanged(bool) ),     SLOT( onCurrentProductChanged(bool) ) );
        connect( operateMode_, SIGNAL( widgetSettingsLockChanged(bool) ), SLOT( onWidgetSettingsLockChanged(bool) ) );
        connect( operateMode_, SIGNAL( currentProductUpdateField(DomUpdateRunModeField) ), SLOT( onCurrentProductUpdateField(DomUpdateRunModeField) ) );
    }

    connect( mainWindow_, SIGNAL( numberPadKeyLayoutStyleChanged() ), SLOT( onNumberPadKeyLayoutStyleChanged() ) );
    connect( mainWindow_, SIGNAL( numberPadMoved(Qt::DockWidgetArea) ), SLOT( onNumberPadMoved(Qt::DockWidgetArea) ) );

    connect( mainWindow_, SIGNAL( cuttingTestChanged(bool) ), SLOT( onCuttingTestChanged(bool) ) );

    // plu field is our focus proxy
    setFocusProxy( plu_ );

    // install event filter
    mainWindow_->numberPad()->installEventFilter( this );

    // SOF-3750
    // timer to restore focus when web view steals it
    webViewFocusTimer_ = new QTimer( this );
    webViewFocusTimer_->setSingleShot( true );

    connect( webViewFocusTimer_, SIGNAL(timeout()), SLOT(setFocusPluEntry()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PluMainScreen::~PluMainScreen()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QRect PluMainScreen::dcaGeometry( int index ) const
{
    const bool useEnhancedCenterScreen( isEnhancedCenterScreenActive() );

    if ( 1 == index )
    {
        if ( useEnhancedCenterScreen )
            return dcaEnhancedCenterScreen_->geometry();
        else
            return dcaPrimary_->geometry();
    }

    return dcaSecondary_->geometry();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QStringList PluMainScreen::editableFeatures() const
{
    QStringList list;

    if (( hobart::dynamicContentWidget::Flashkeys == dcaPrimary_->currentIndex() ) ||
        ( hobart::dynamicContentWidget::BestSellerList == dcaPrimary_->currentIndex() ) ||
        ( hobart::dynamicContentWidget::FixedFlashkeys == dcaPrimary_->currentIndex() ) )
    {
        list.append( QString::number( dcaPrimarySoftkeyGroup_ ) );
    }

    if (( hobart::dynamicContentWidget::Flashkeys == dcaSecondary_->currentIndex() ) ||
        ( hobart::dynamicContentWidget::BestSellerList == dcaSecondary_->currentIndex() ) ||
        ( hobart::dynamicContentWidget::FixedFlashkeys == dcaSecondary_->currentIndex() ) )
    {
        list.append( QString::number( dcaSecondarySoftkeyGroup_ ) );
    }

    if (( hobart::dynamicContentWidget::Flashkeys == dcaEnhancedCenterScreen_->currentIndex() ) ||
        ( hobart::dynamicContentWidget::BestSellerList == dcaEnhancedCenterScreen_->currentIndex() ) ||
        ( hobart::dynamicContentWidget::FixedFlashkeys == dcaEnhancedCenterScreen_->currentIndex() ) )
    {
        list.append( QString::number( dcaEnhancedCenterScreenSoftkeyGroup_ ) );
    }

    return list;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PluMainScreen::isForcedFields() const
{
    LOG_CLASS_INFO( "PluMainScreen", "isForcedFields" );

    DomCurrentPLU *product( currentProduct() );

    // sanity check
    if ( !product )
        return false;

    QString productNumOfLabels( product->numOfLabelsRaw().trimmed() );
    QString productKeyboardNetWeight( product->keyboardNetWeight().trimmed() );
    QString productExpirationDays( product->expirationDaysRaw().trimmed() );

    // forced cool
    if ( cool_->isForced() )
    {
        LOG_DEBUG( "cool is forced", NULL );
        return true;
    }
    // "force cool verification" turned on (plu field)
    // FT-178 force verification even when no cool text
    //else if (( product ) && ( product->forcedCool() ) && ( coolText_.size() ))
    else if ( product->forcedCool() )
    {
        LOG_DEBUG( "cool is forced verification", NULL );
        return true;
    }
    // forced packed on (aka backdating)
    else if ( packedOnDate_->isForced() )
    {
        LOG_DEBUG( "packed on date is forced", NULL );
        return true;
    }
    // forced number of labels (must use print multiple labels softkey)
    else if (( product->numOfLabelsForced() ) && ( productNumOfLabels.isEmpty() ))
    {
        LOG_DEBUG( "number of labels is forced", NULL );
        return true;
    }
    // forced net weight (aka turkey weigher)
    else if (( product->acceptKbdNetWeight() ) && ( productKeyboardNetWeight.isEmpty() ))
    {
        LOG_DEBUG( "net weight forced", NULL );
        return true;
    }
    // forced expiration days
    else if (( product->expirationDaysForced() ) && ( productExpirationDays.isEmpty() ))
    {
        LOG_DEBUG( "expiration days forced", NULL );
        return true;
    }
    // forced tray entry
    else if ( traySizesRequired_ )
    {
        LOG_DEBUG( "wrapper tray selection forced", NULL );
        return true;
    }

    LOG_TRACE( "no forced fields", NULL );
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PluMainScreen::isPopupNumberPad() const
{
    return (hobart::dynamicContentWidget::NumberPad != dcaSecondary_->currentIndex());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PluMainScreen::pluHasFocus() const
{
    return plu_->hasFocus();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::setCurrentItemNumber( const QString& value )
{
    LOG_CLASS_INFO( "PluMainScreen", "setCurrentItemNumber" );

    // validate entry
    if ( value.isEmpty() )
        return;

    if( session_->scaleConfig->internalConfig.ssactive_lock )
    {
    	// SOF-5355 - This needed to be here from the changes made on 8/15/22
        // FT-947
        currentItemNumber_ = value;

        if( hasRemoteLic && mainWindow_->isRemotePrintItem(value) )
        {
            session_->send( ProductDev, ProductRead, "0" );
            session_->send( Error, ErrProductDevice, QString::number( ProductDevError::ProductErrPrintWhiteListOnScaleRule ) + ",5" );
        }
        else
        {
            // if we are in an error state type of situation - dont look up PLUs
            //QString test =  sessionHooks::instance()->getFieldValue( sessionHooks::selfServiceRestrictedDialog );
            if( "0" == sessionHooks::instance()->getFieldValue( sessionHooks::selfServiceRestrictedDialog ) )
            {
                session_->send( SessionDev, SnReadWeighModeTransactionType );
                session_->send( ProductDev, ProductRead, value + ",3" );
            }
        }
    }
    else
    {
         MadpUpdateCurrentPLUTotals *productTotals( currentProductTotals() );

        // check for different product number
        if ( value == currentItemNumber_ )
        {
            plu_->resetInputTruncationFlag();
            return;
        }
        else if (( "0" == value ) && ( currentItemNumber_.isEmpty() ))
        {
            plu_->clear();
            return;
        }

        // FT-947
        currentItemNumber_ = value;

        // FT-732
        // also set plu text... if an error occurs loading plu focus will be taken away and we will
        // revert to the previous plu...
        if ( plu_->text() != value )
        {
            bool restorePluFocus;

            ignoreSignals_ = true;

            if ( (restorePluFocus = pluHasFocus()) )
                plu_->clearFocus();

            plu_->setText( currentItemNumber_ );

            if ( restorePluFocus )
                plu_->setFocus();

            ignoreSignals_ = false;
        }

        // request plu but first clear the price and weight
        if( hasRemoteLic && mainWindow_->isRemotePrintItem(value) )
        {
            // its okay to clear it here
            operateMode_->clearPriceAndWeightFields();
            LOG_INFO( "requesting plu %s...", qPrintable( currentItemNumber_ ) );
            session_->send( ProductDev, ProductRead, "0" );
            session_->send( Error, ErrProductDevice, QString::number( ProductDevError::ProductErrPrintWhiteListOnScaleRule ) + ",5" );
        }
        else
        {
            if( session_->scaleConfig->isWrapperTwoPrinter() && !( session_->uiUtilities->isWrapOnly() ) )
            {
                // if we have a 2 wrapper system, we exit out for some specific reasons.
                //      a. mainPower is off - so look up PLUs all you want, unless they are the same
                //      b. entered PLU is same as PLU on screen (dont do repeated look ups on same PLU)
                //      c. mainPower is on and weights are ---, i.e., unstable weight, look up the PLU
                //      d. mainPower is on, and the tare and netweight = 0.000 (no pkg on platter), then it is ok to look up PLUs
                int curplu = session_->scaleConfig->currentPLU.itemNumber().toInt();
                bool isMainPowerOn = session_->scaleConfig->evolWrapperStatus.mainPower();
                QString weight( "" );
                QString tare( "" );

                if( value.toInt() == curplu )
                {
                    // same plu so no reason to do anything. Bail now
                    session_->scaleConfig->internalConfig.isOkToLookUpPlu = true;
                    session_->scaleConfig->internalConfig.isOkDroppedPLU = 0;

                    plu_->resetInputTruncationFlag();
                    return;
                }

                if( productTotals )
                {
                    tare = productTotals->totalTare();                                                      // if no weight on scale, but want to look up product,
                    weight = productTotals->totalNetWeight();                                               // then -weight should equal tare
                }

                if ( (   mainWindow_->isSupervisorMode() || mainWindow_->isServiceMode() ) ||               // always true in service or supervisor
                     (   isMainPowerOn == false ) ||                                                        // if mainPower off then look up PLUs all day
                     ( ( isMainPowerOn == true ) && ( ("-" + tare) ==  weight ) ) ||
                     ( ( isMainPowerOn == true ) && ( tare.toDouble() == 0.000 ) && ( weight.toDouble() == 0.000 ) ) )  // if mainPower on but no pkg on platter, lookup PLUs OK
                {
                    session_->scaleConfig->internalConfig.isOkToLookUpPlu = true;                           // simple tests
                    session_->scaleConfig->internalConfig.isOkDroppedPLU = 0;
                }

                if( value.toInt() != curplu )
                {
                    // if the new_value does not equal previous value, DO SOMETHING, if lookup PLU flag says its ok
                    if( session_->scaleConfig->internalConfig.isOkToLookUpPlu )
                    {
                        // we are going to look up the PLU, but since there is weight on platter, a REZERO dialog is
                        // going to get popped.  If there is another PLU being cycled, and label orientation is different,
                        // then you will also have to wait for the window to open back up before the pkg is pulled in
                        if( ( weight.toFloat() > 0 ) && ( session_->scaleConfig->currentPLU.productType() == ProductType::PRODUCTTYPE_RANDOMWEIGHT ) )
                        {
                            if( session_->scaleConfig->currentPLU.acceptKbdNetWeight() )
                            {
                                // exception for Keyboard Net Weight - users can look up next PLU
                                session_->scaleConfig->internalConfig.isOkToLookUpPlu = false;                                                       // false bc I have to still process this dropped PLU
                                session_->scaleConfig->internalConfig.isOkDroppedPLU = value.toInt();
                            }
                            else
                            {
                                session_->send( Error, ErrWeigherDevice, QString::number( WeigherDevError::WeigherErrScaleNotZero ) + ",11" );       // static const short RESPONSESTYLE_REZEROIGNORE	= 11;
                                if( 0 == value.toInt() )
                                {
                                    // if 0 PLU, we are gonna allow it be looked up and clear the screen
                                    operateMode_->clearPriceAndWeightFields();
                                    session_->send( SessionDev, SnReadWeighModeTransactionType );
                                    session_->send( ProductDev, ProductRead, value + ",3" );
                                    session_->scaleConfig->internalConfig.poppedRezero = false;
                                    session_->scaleConfig->internalConfig.isOkToLookUpPlu = true;
                                    plu_->resetInputTruncationFlag();
                                    return;
                                }

                                session_->scaleConfig->internalConfig.poppedRezero = true;
                                session_->scaleConfig->internalConfig.isOkToLookUpPlu = false;                                                       // false bc I have to still process this dropped PLU
                                session_->scaleConfig->internalConfig.isOkDroppedPLU = value.toInt();

                                plu_->resetInputTruncationFlag();

                                // since we didn't look up the PLU - reset the plu field back to the original

                                bool restorePluFocus;
                                if ( (restorePluFocus = pluHasFocus()) )
                                    plu_->clearFocus();

                                plu_->setText( QString::number( curplu ) );

                                if ( restorePluFocus )
                                    plu_->setFocus();

                                return;
                            }
                        }

                        // we are going to look up a PLU, and until label UI receives prodcomplete msg, only then is it is ok again to look up a new PLU
                        if( session_->scaleConfig->currentPLU.productType() == ProductType::PRODUCTTYPE_RANDOMWEIGHT )
                        {
                            session_->scaleConfig->internalConfig.isOkToLookUpPlu = false;
                            session_->scaleConfig->internalConfig.isOkDroppedPLU = value.toInt();
                        }

                        // request plu but first clear the price and weight
                        operateMode_->clearPriceAndWeightFields();
                        operateMode_->clearPLU();

                        session_->send( SessionDev, SnReadWeighModeTransactionType );
                        session_->send( ProductDev, ProductRead, value + ",3" );

                        if( 0 == value.toInt() )
                        {
                            // if new plu entered (value) is 0, then OK to lookup PLUs again on next entry
                            session_->scaleConfig->internalConfig.isOkToLookUpPlu = true;
                            session_->scaleConfig->internalConfig.isOkDroppedPLU = 0;
                        }
                    }
                    else
                    {
                        // it was NOT ok to look up PLUs
                        // Entered PLU with weight on platter - tell operator Hey there is weight on the platter, rezero or ignore
                        session_->scaleConfig->internalConfig.isOkDroppedPLU = value.toInt();
                        session_->scaleConfig->internalConfig.isOkToLookUpPlu = false;                                  // false bc I have to still process this dropped PLU

                        // we are going to look up the PLU, but since there is weight on platter, a REZERO dialog is going to get popped.
                        if( weight.toFloat() > 0 )
                        {
                            plu_->resetInputTruncationFlag();
                        }
                    }
                } 
            }
            else
            {
                // ORIGINAL CODE 1P System - business as usual. Continue lookup as usual request plu but first clear the price and weight
                operateMode_->clearPriceAndWeightFields();

                session_->send( SessionDev, SnReadWeighModeTransactionType );
                session_->send( ProductDev, ProductRead, value + ",3" );
            }
        }

        // reset plu input field
        plu_->resetInputTruncationFlag();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::setOnlineOrderingProductUrl( const QString& url )
{
    if ( dcaPrimary_ )
        dcaPrimary_->setOnlineOrderingProductUrl( url );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::processForcedFields()
{
    LOG_CLASS_INFO( "PluMainScreen", "processForcedFields" );

    DomCurrentPLU *product( currentProduct() );

    // sanity check
    if ( !product )
        return;

    QString productNumOfLabels( product->numOfLabelsRaw().trimmed() );
    QString productKeyboardNetWeight( product->keyboardNetWeight().trimmed() );
    QString productExpirationDays( product->expirationDaysRaw().trimmed() );

    // forced cool
    if ( cool_->isForced() )
    {
        if ( !forceCoolDialog_ )
        {
            MadpScaleConfig *scaleConfig = &session_->scaleConfig->scaleConfig;

            // reset verification flag as we are forcing it
            if ( product->forcedCool() )
            {
                LOG_TRACE( "resetting forced cool since forced...", NULL );

                product->setForcedCool( false );
                operateMode_->writeSingleField( OperateMode::OffsetForcedCool, "0", "1" );
            }

            // prevent cool dialog from popping twice
            forceCoolDialog_ = true;

            LOG_INFO( "processing forced cool...", NULL );
            cool_->promptForInput( product->coolRequired(), scaleConfig->coolTrackingNum() );
        }
    }
    // "force cool verification" turned on (plu field)
    // FT-178 force verification even when no cool text
    //else if (( product ) && ( product->forcedCool() ) && ( coolText_.size() ))
    else if ( product->forcedCool() )
    {
        // reset flags so it does not pop again
        LOG_TRACE( "resetting forced cool since verification only...", NULL );

        product->setForcedCool( false );
        operateMode_->writeSingleField( OperateMode::OffsetForcedCool, "0", "1" );

        LOG_INFO( "processing forced cool verification...", NULL );
        cool_->promptForInput();
    }
    // forced packed on (aka backdating)
    else if ( packedOnDate_->isForced() )
    {
        if ( !forcePackedOnDateDialog_ )
        {
            // prevent packed on date dialog from popping twice
            forcePackedOnDateDialog_ = true;

            LOG_INFO( "processing forced packed on date...", NULL );
            packedOnDate_->promptForInput();
        }
    }
    // forced number of labels (must use print multiple labels softkey)
    else if (( product->numOfLabelsForced() ) && ( productNumOfLabels.isEmpty() ))
    {
        if ( !forceNumberOfLabelsDialog_ )
        {
            // prevent dialog from popping twice
            forceNumberOfLabelsDialog_ = true;
            promptNumberOfLabels();

            forceNumberOfLabelsDialog_ = false;

            // FT-677 restore focus
            if ( !operateMode_->isForcedFields() )
                operateMode_->setFocusForPopupNumberPad();
        }
    }
    // forced net weight (aka turkey weigher)
    else if (( product->acceptKbdNetWeight() ) && ( productKeyboardNetWeight.isEmpty() ))
    {
        if ( !forceNetWeightDialog_ )
        {
            // prevent dialog from popping twice
            forceNetWeightDialog_ = true;
            promptNetWeight();

            forceNetWeightDialog_ = false;

            // FT-677 restore focus
            if ( !operateMode_->isForcedFields() )
                operateMode_->setFocusForPopupNumberPad();

             session_->scaleConfig->internalConfig.isOkToLookUpPlu = true;
        }
    }
    // forced expiration days
    else if (( product->expirationDaysForced() ) && ( productExpirationDays.isEmpty() ))
    {
        if ( !forceExpirationDaysDialog_ )
        {
            // prevent dialog from popping twice
            forceExpirationDaysDialog_ = true;
            promptExpirationDays(); // TODO

            forceExpirationDaysDialog_ = false;

            // FT-677 restore focus
            if ( !operateMode_->isForcedFields() )
                operateMode_->setFocusForPopupNumberPad();
        }
    }
    // forced tray sizes
    else if (( traySizesRequired_ ) && ( !forceTraySizeDialog_ ))
    {
        // prevent dialog from popping twice
        forceTraySizeDialog_ = true;

        DomCurrentPLU *product( currentProduct() );

        // prompt tray
        WrapperForcedTraySizeDialog d( mainWindow_ );

        if (( !product ) || ( !d.traySizesExist() ))
            setForcedTrayEntry( false );
        else
        {
            const bool result( QDialog::Accepted == d.exec() );

            // they cancelled tray entry... remove the current product
            if (( !result ) && ( d.dismissedFromOverlayClick() ))
            {
                traySizesRequired_ = false;

                LOG_WARN( "user cancelled tray entry", NULL );
                setCurrentItemNumber( "0" );
            }

            // FT-677 restore focus
            if ( !operateMode_->isForcedFields() )
                operateMode_->setFocusForPopupNumberPad();
        }

        forceTraySizeDialog_ = false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PluMainScreen::checkExpirationDaysForced()
{
    DomCurrentPLU *product( currentProduct() );

    // sanity check
    if ( !product )
        return false;

    QString productExpirationDays( product->expirationDaysRaw().trimmed() );

    // forced expiration days
    if (( product->expirationDaysForced() ) && ( productExpirationDays.isEmpty() ))
        return true;

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PluMainScreen::checkProductionEntryForced()
{
    DomCurrentPLU *product( currentProduct() );

    // sanity check
    if ( !product )
        return false;

    QString productNumOfLabels( product->numOfLabelsRaw().trimmed() );


    if (( product->numOfLabelsForced() ) && ( productNumOfLabels.isEmpty() ))
    {
        printBeforeNumOfLabelsEntered_ = true;
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::reloadWebViewers()
{
    UiUtilities *util( mainWindow_->session()->uiUtilities );
    MadpScaleConfig *config( &session_->scaleConfig->scaleConfig );

    if ( dcaEnhancedCenterScreen_ )
    {
        // retrieve web view
        hobart::webViewer *ecsWebViewer( dynamic_cast<hobart::webViewer*>( dcaEnhancedCenterScreen_->widget( hobart::dynamicContentWidget::WebBrowser ) ) );

        if ( util->isFeatureLicensed( SCALE_ENHANCED_CENTERSCREEN, false ) && config->enableEnhancedCenterScreen() )
        {
            const QString url( config->enhancedCenterHomePage().trimmed() );

            if ( url.length() )
            {
                // set URL
                if ( ecsWebViewer )
                {
                    if ( dcaEnhancedCenterScreenReload_ )
                        dcaEnhancedCenterScreenReload_->stop();

                    ecsWebViewer->setURL( url );
                }
            }
        }
        else
        {
            ecsWebViewer->setURL( EMPTY_URL );
        }
    }

    if ( webView_ && hobart::dynamicContentWidget::WebBrowser != config->dcaRunScreenType() )
        webView_->setURL( EMPTY_URL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::resetFlashkeys( bool force )
{
    // no sense requesting anything if we are not registered yet
    if ( !registered_ )
        return;

    // If we are headed to Operate Mode and we have already requested and updated the flashkeys, then bail
    if( mainWindow_->getIsHeadingToHome() && !session_->scaleConfig->internalConfig.updateNavigateHomeFlashkeys )
        return;

    if( !session_->scaleConfig->internalConfig.ssactive_lock )
    {
        if( mainWindow_->getIsHeadingToHome() )
            dcaPrimary_->resetFlashkeys( true );
        else
            dcaPrimary_->resetFlashkeys( force );

        dcaSecondary_->resetFlashkeys( force );
        dcaEnhancedCenterScreen_->resetFlashkeys( force );
    }

    // request tray flashkeys
    if (( session_->isPrepack() ) && ( force ))
        session_->send( SoftkeyDev, LoadSoftkeyGroup, QString::number( softkeysWidget::ForcedCameraOverride) );

    // we updated the flashkeys do dont need to do it again
    session_->scaleConfig->internalConfig.updateNavigateHomeFlashkeys = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::setForcedTrayEntry( bool value )
{
    traySizesRequired_ = value;

    plu_->setTrayTypeForced( value );

    // !! Start of HACK !!
    //
    // This is really janky but there is no easy way to indicate to other screen that tray
    // entry is forced. The PLU Operate Screen basically duplicates the plu edit widget
    // so we need to duplicate the forced "SELECT TRAY" text in that widget as well as this
    // widget. But there is no signal that indicates forced tray like the other forced fields
    // coming out of operate mode.
    //
    // So this change will find the screen and update it manually.

    // retrieve operate screen
    PluOperateScreen *pluOperateScreen( mode_->isCached<PluOperateScreen>() );

    if ( pluOperateScreen )
        pluOperateScreen->setForcedTrayEntry( value );

    // !! End of HACK !!
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int PluMainScreen::toggleFixedXCoordinate( int fixedRowOrCol, int defaultBtn )
{
    // New for toggle extra shelf life day
    ScaleConfiguration *config = session_->scaleConfig;

    int row = 0;
    int posX  = session_->scaleConfig->toggleExtraDayRow;           // Row
    int hSize = session_->scaleConfig->toggledLastRow;

    if( config->toggleExtraDayNow )
    {
        if( 2 == fixedRowOrCol )                                    // left col
            row = posX - 1;
        else                                                        // top row, right col, bottom row don't have to shift
            row = posX;
    }
    else
    {
        if( 1 == fixedRowOrCol )                                    // top row
            row = defaultBtn - 1;
        else if( 2 == fixedRowOrCol )                               // left col
            row = 0;
        else if( 3 == fixedRowOrCol )                               // bottom row
            row =  defaultBtn - 1;
        else if( 4 == fixedRowOrCol )                               // right col
            row = hSize - 1;
    }

    return row;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int PluMainScreen::toggleFixedYCoordinate( int fixedRowOrCol, int defaultBtn )
{
    // New for toggle extra shelf life day
    ScaleConfiguration *config = session_->scaleConfig;

    int col = 0;
    int posY  = config->toggleExtraDayCol;                          // Col
    int vSize = config->toggledLastCol;

    if( config->toggleExtraDayNow )
    {
        if( 1 == fixedRowOrCol )                                    // top row is only one who shifts
            col =  posY - 1;
        else                                                        // left col, right col, all don't have to shift
            col = posY;
    }
    else
    {
        if( 1 == fixedRowOrCol )                                    // top row
            col = 0;
        else if( 2 == fixedRowOrCol )                               // left col
            col = defaultBtn - 1;
        else if( 3 == fixedRowOrCol )                               // bottom row
            col = vSize - 1;
        else if( 4 == fixedRowOrCol )                               // right col
            col = defaultBtn - 1;
    }

    return col;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::setFocusPluEntry()
{
    LOG_CLASS_INFO( "PluMainScreen", "setFocusPluEntry" );

    if ( !pluHasFocus() )
    {
        LOG_TRACE( "set focus to plu", NULL );
        plu_->setFocus();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::setPluDisabled( bool disable )
{
    // So far, disabling the PLU field is ONLY done when we pop a PE dialog box

    plu_->setDisabled( disable );

    if( !disable )
        plu_->setFocus();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::setSpecialFlasheyWidget( bool value )
{
    // cutting test
    dcaPrimary_->setCuttingTest( value );

    // update layout
    updateLayout( mainWindow_->numberPadArea() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::setTrayName( const QString& name )
{
    plu_->setTrayTypeVisible( true, name );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::translate()
{
    // reload flashkeys (for new language)
    resetFlashkeys( true );

    // label text
    QString pluText = tr( "PLU#" );
    QString descriptionText = tr( "Description" );
    QString sellByDateText = tr( "Sell By" );
    QString coolText = tr( "COOL" );
    QString trackingText = tr( "Tracking#" );
    QString bestBeforeDateText = tr( "Best Before" );
    QString packedOnDateText = tr( "Packed On" );

    // convert to upper
    if ( convertUppercase_ )
    {
        pluText = pluText.toUpper();
        descriptionText = descriptionText.toUpper();
        sellByDateText = sellByDateText.toUpper();
        coolText = coolText.toUpper();
        trackingText = trackingText.toUpper();
        bestBeforeDateText = bestBeforeDateText.toUpper();
        packedOnDateText = packedOnDateText.toUpper();
    }

    // set label text
    plu_->setLabelText( pluText );
    description_->setLabelText( descriptionText );
    sellByDate_->setLabelText( sellByDateText );
    cool_->setLabelText( coolText );
    bestBeforeDate_->setLabelText( bestBeforeDateText );
    packedOnDate_->setLabelText( packedOnDateText );

//    tof_->setLabelText("Primary");
//    tof2_->setLabelText("Secondary");

    // set field text
    sellByDate_->translate();
    bestBeforeDate_->translate();
    packedOnDate_->translate();

    // translate widgets
    dcaPrimary_->translate();
    dcaSecondary_->translate();
    dcaEnhancedCenterScreen_->translate();
    plu_->translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::hideAccumulateItems()
{
    if ( dcaPrimary_->isAccumulateItems() )
        dcaPrimary_->hideSpecialWidget();

    // update layout
    updateLayout( mainWindow_->numberPadArea() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::showAccumulateItems()
{
    // validate mode
    if ( OpModeAccumulateItems != session_->scaleConfig->weighModeTotalType.weighMode() )
        return;

    if ( !dcaPrimary_->isAccumulateItems() )
        dcaPrimary_->showSpecialWidget( hobart::dynamicContentWidget::AccumulateItems );

    // update layout
    updateLayout( mainWindow_->numberPadArea() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::hideOperatorMessage()
{
    // Remote Print - had to add the AccumulateItems option, bc if we do a remote print, there
    // might be an accumulate items table on the screen. Ordinarily in PE mode, there NEVER
    // would be an accumulate items table bc the modes are mutually exclusive, but not in remote print
    //
    // FT-676: make sure operator message shown before hiding
    // FT-717: operator message *or* tare flashkeys
    // SOF-1759: check we are not accumulate items mode before hiding (broken by remote print above)
    if (( dcaPrimary_->isSpecialWidget( hobart::dynamicContentWidget::OperatorMessage ) ) ||
        ( dcaPrimary_->isSpecialWidget( hobart::dynamicContentWidget::TareFlashkeys ) ) ||
        ( dcaPrimary_->isSpecialWidget( hobart::dynamicContentWidget::AccumulateItems ) && OpModeAccumulateItems != session_->scaleConfig->weighModeTotalType.weighMode() ))
    {
        dcaPrimary_->hideSpecialWidget();
    }

    // update layout
    updateLayout( mainWindow_->numberPadArea() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::showOperatorMessage( int type, const QString& message, bool allowCancelProdEntry )
{
    hobart::dynamicContentWidget::WidgetType index = (hobart::dynamicContentWidget::WidgetType) type;

    if ( hobart::dynamicContentWidget::OperatorMessage == index )
    {
        QString title( tr( "System Message" ) );

        // convert to upper
        if ( convertUppercase_ )
            title = title.toUpper();

        if ( allowCancelProdEntry )
            dcaPrimary_->setOperatorMessage( title, message, hobart::show_cancel_prod_entry );
        else
            dcaPrimary_->setOperatorMessage( title, message );
    }

    dcaPrimary_->showSpecialWidget( index );

    // update layout
    updateLayout( mainWindow_->numberPadArea() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
MadpUpdateCurrentPLUTotals *PluMainScreen::currentProductTotals() const
{
    if ( !currentProduct() )
        return NULL;

    return (operateMode_ ? operateMode_->currentProductTotals() : NULL);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::updateProductionPlanningMessage(const QString& weight)
{
    // HTs
    // this feature is disabled
    if ( !session_->uiUtilities->isFeatureLicensed( SCALE_FPP ) )
        return;

    // FT-816 FPP PLU Weight showing blank.
    // This was moved to here beccause it needs to be called when onCurrentProductChanged() where is was originally
    // located.  Additionally, if the weight changes due to a 101,15 message, we need to be able to call this from
    // the weightbarwidget (where 101,15 is processed) to update the weight in the DCA area for FPP.
    // side note: when I need the weight in the dca update, it's not right.  So I am passing it along

    DomCurrentPLU *product( currentProduct() );

    dcaPrimary_->setFreshProduction( true, product, &session_->scaleConfig->operatorDisplayFields, weight );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PluMainScreen::isEnhancedCenterScreenActive() const
{
    MadpScaleConfig *config( &session_->scaleConfig->scaleConfig );

    UiUtilities *util( mainWindow_->session()->uiUtilities );

    // must be licensed and enabled
    if ( !util->isFeatureLicensed( SCALE_ENHANCED_CENTERSCREEN, false ) )
        return false;
    else if ( !config->enableEnhancedCenterScreen() )
        return false;

    // this feature is scale only
    if ( session_->isPrepack() )
        return false;

    // not when product pulled up
    if ( currentProduct() )
        return false;

    // not valid when any of the following modes
    if (( dcaPrimary_->isSpecialWidget( hobart::dynamicContentWidget::AccumulateItems ) ) ||
        ( dcaPrimary_->isSpecialWidget( hobart::dynamicContentWidget::FreshProduction ) ) ||
        ( dcaPrimary_->isSpecialWidget( hobart::dynamicContentWidget::OperatorMessage ) ) ||
        ( dcaPrimary_->isSpecialWidget( hobart::dynamicContentWidget::TareFlashkeys ) ) ||
        ( dcaPrimary_->isSpecialWidget( hobart::dynamicContentWidget::CutTestFlashkeysDefault ) ) ||
        ( dcaPrimary_->isSpecialWidget( hobart::dynamicContentWidget::CutTestFlashkeysOptionOne ) ))
    {
        return false;
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
QDate PluMainScreen::retrieveTimestamp( int addDays, int addHours ) const
{
    QDateTime temp( session_->scaleConfig->currentDate(), session_->scaleConfig->currentTime() );
    temp = temp.addDays( addDays );
    temp = temp.addSecs( 3600 * addHours );

    return temp.date();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// FT-564
QDate PluMainScreen::retrieveTimestamp( const QDate& baseDate, int addDays, int addHours ) const
{
    QDateTime temp( baseDate, session_->scaleConfig->currentTime() );

    temp = temp.addDays( addDays );
    temp = temp.addSecs( 3600 * addHours );

    return temp.date();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget *PluMainScreen::swipeScreenDown() const
{
    // do not allow swipe if we are processing going to self service
    if( session_->scaleConfig->internalConfig.ssactive_tenative_lock )
        return NULL;

    // do not allow swipe while processing forced fields
    if ( operateMode_->isForcedFields() )
        return NULL;

    // no apps installed
    else if ( !AppsScreen::isExternalAppInstalled() )
        return NULL;

    // screen does not exist
    else if ( !operateMode_->isCached<AppsScreen>() )
        return NULL;

    return mode_->createScreen<AppsScreen>();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget *PluMainScreen::swipeScreenLeft() const
{
    // Swipe LEFT to get back to the CENTER
    session_->scaleConfig->internalConfig.isLeftPanelActivated = false;

    // do not allow swipe if we are processing going to self service
    if( session_->scaleConfig->internalConfig.ssactive_tenative_lock )
        return NULL;

    // do not allow swipe while processing forced fields
    if ( operateMode_->isForcedFields() )
        return NULL;

    ScreenWidget *screen( NULL );

    if ( session_->isPrepack() )
    {
        // check what screen is the right screen...
        // 0 = PluInfoScreen
        // 1 = WrapperSettingsScreen
        // 2 = LabelPlacementScreen
        int rightScreen( session_->scaleConfig->scaleConfig.rightScreenType() );

        if ( 0 == rightScreen )
            screen = mode_->createScreen<PluInfoScreen>();
        else if ( 1 == rightScreen )
            screen = mode_->createScreen<WrapperSettingsScreen>();
        else if ( 2 == rightScreen )
            screen = mode_->createScreen<LabelPlacementScreen>();
    }
    else if ( session_->isServiceCounter() )
    {
        // create plu info screen
        screen = mode_->createScreen<PluInfoScreen>();
    }

        return screen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget *PluMainScreen::swipeScreenRight() const
{
    // swipe RIGHT to get the LEFT panel
     int leftScreen( session_->scaleConfig->scaleConfig.dcaStartScreenType() );

     if( hobart::dynamicContentWidget::FixedFlashkeys == leftScreen )
     {
        session_->scaleConfig->internalConfig.isLeftPanelActivated = true;
        session_->scaleConfig->toggledState = true;
     }

    // do not allow swipe if we are processing going to self service
    if( session_->scaleConfig->internalConfig.ssactive_tenative_lock )
        return NULL;

    // do not allow swipe while processing forced fields
    if ( operateMode_->isForcedFields() )
        return NULL;

    // HTs
    // this feature is disabled
    if ( !operateMode_->isCached<WebBrowserScreen>() )
        return NULL;

    // create web browser screen
    ScreenWidget *screen = mode_->createScreen<WebBrowserScreen>();

    return screen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget *PluMainScreen::swipeScreenUp() const
{
    ScreenWidget *screen( NULL );

    MadpScaleConfig *madpConfig = &session_->scaleConfig->scaleConfig;
    int bottomScreen = madpConfig->bottomScreenType();

    // do not allow swipe if we are processing going to self service
    if( session_->scaleConfig->internalConfig.ssactive_tenative_lock )
        return NULL;

    if ( bottomScreen == BUILD_A_MEAL_SCREEN )
    {
        // screen does not exist
        if ( !operateMode_->isCached<MealBuilderScreen>() )
            return NULL;

        screen = mode_->createScreen<MealBuilderScreen>();
    }
    else if ( bottomScreen == CUT_TEST_ONE_SCREEN )
    {
        // screen does not exist
        if ( !operateMode_->isCached<MeatSheetScreen>() )
            return NULL;

        if( session_->scaleConfig->cuttingTestActive )
            screen = mode_->createScreen<MeatSheetScreen>();
        else
            return NULL;
    }

    return screen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PluMainScreen::eventFilter( QObject *obj, QEvent *e )
{
    LOG_CLASS_INFO( "PluMainScreen", "eventFilter" );

    // show event
    if ( QEvent::Show == e->type() )
    {
        // lock up (save) widget settings when operator clicks into input field
        if (( mainWindow_->numberPad() == obj ) && ( dcaSecondary_ ))
            operateMode_->setWidgetSettingsLock( true );
    }

    // focus in
    else if ( QEvent::FocusIn == e->type() )
    {
        // SOF-3750
        // web view stealing focus fix... set a timer to restore focus unless operator actaully
        // clicked inside the web view
        if ( isWebViewChild( qobject_cast<QWidget*>( obj ) ) )
        {
            LOG_DEBUG( "focus into web view", NULL );

            // timer will restore focus unless cancelled below
            webViewFocusTimer_->start( 200 );
        }
    }

    // mouse button release
    else if ( QEvent::MouseButtonRelease == e->type() )
    {
        if ( isWebViewChild( qobject_cast<QWidget*>( obj ) ) )
        {
            LOG_DEBUG( "mouse button into web view", NULL );
            webViewFocusTimer_->stop();
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PluMainScreen::onScreenChanging()
{
    // when leaving the screen reset the flashkeys to default page
    resetFlashkeys();

    // lock up widget settings
    operateMode_->setWidgetDragLock( true );
    operateMode_->setWidgetSettingsLock( true );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "PluMainScreen", "onSessionMessage" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // weigh mode configuration
    if (( SessionDev == device ) && ( SnWriteWeighModeTransactionType == event ))
    {
        DomWeighModeTotalType m( msg );

        // accumulate items
        dcaPrimary_->setAccumulateItems( OpModeAccumulateItems == m.weighMode() );

        setPackedOnDateReadOnly();

        // update layout
        updateLayout( mainWindow_->numberPadArea() );
    }

    // scale configuration
    else if (( ConfigurationDev == device ) && ( CfgScaleWrite == event ))
    {
        MadpScaleConfig m( msg );

        // set plu auto lookup parameters
        setPluAutoLookup();

        if(  session_->scaleConfig->internalConfig.ssactive_lock )
            return;

        // browser configuration
        if ( webView_ )
        {
            QString this_url = m.runScreenHome();
            QString defaultHomePage( m.runScreenHome().trimmed() );       // old HLX method - dont see the need
            //QString defaultHomePage( m.defaultHomePage().trimmed() );

            // save off home url
            if ( homeUrl_ != defaultHomePage )
                webView_->setURL( homeUrl_ = defaultHomePage );

            // enable/disable address bar
            webView_->setUrlVisible( true, m.allowAddressBar() ); // FT-1451
        }

        // show total type
        plu_->setTotalTypeVisible( m.showTotalTypeOnRunScreen() );

        // enhanced center screen webpage
        if ( m.enableEnhancedCenterScreen() )
        {
            static QString prevEnhancedCenterHomePage;

            // check webpage changed
            const QString enhancedCenterHomePage( m.enhancedCenterHomePage().trimmed() );

            if ( prevEnhancedCenterHomePage != enhancedCenterHomePage )
            {
                prevEnhancedCenterHomePage = enhancedCenterHomePage;

                // retrieve web view
                hobart::webViewer *ecsWebViewer( dynamic_cast<hobart::webViewer*>( dcaEnhancedCenterScreen_->widget( hobart::dynamicContentWidget::WebBrowser ) ) );

                // set URL
                if ( ecsWebViewer )
                {
                    if ( dcaEnhancedCenterScreenReload_ )
                        dcaEnhancedCenterScreenReload_->stop();

                    // Don't set the URL until the network is online
                    if (networkOnline_) ecsWebViewer->setURL( enhancedCenterHomePage );

                    ecsWebViewer->setDisplayAsDialog( false );
                    ecsWebViewer->setToolBarVisible( false );
                    ecsWebViewer->setUrlVisible( false, false );
                }
            }
        }
    }

    // scale display fields configuration
    else if (( ConfigurationDev == device ) && ( CfgPrepackWrite == event ))
    {
        if(  session_->scaleConfig->internalConfig.ssactive_lock )
            return;

        setOperatorDisplayFields();
    }

    // cool
    else if (( COOLDev == device ) && ( COOLDevShortlistWriteAllCoolText  == event ))
    {
        DomCurrentPLU *product( currentProduct() );

        if ( product )
        {
            DomCoolTextList m( msg );

            QVector<QString> recordNums( TokenParser::splitStringTokens( m.recordNums() ) );
            QVector<QString> names( TokenParser::splitStringTokens( m.names() ) );

            LOG_DEBUG( "counts record nums %d names %d",
                recordNums.count(),
                names.count() );

            if ( recordNums.count() != m.numOfItems() )
                LOG_WARN( "mismatch in number of records %d %d", recordNums.count(), m.numOfItems() );

            if ( names.count() != m.numOfNames() )
                LOG_WARN( "mismatch in number of names %d %d", names.count(), m.numOfNames() );

            int count = qMin( recordNums.count(), m.numOfItems() );

            count = qMin( count, names.count() );

            LOG_DEBUG( "have %d entries", count );

            // clear existing entries
            coolText_.clear();

            // add to cool text map
            for ( int i = 0; i < count; ++i )
            {
                int id = recordNums[i].toInt();
                QString text( names[i] );

                LOG_TRACE( "have cool %d '%s'", id, qPrintable( text ) );
                coolText_.insert( id, text );
            }

            // populate cool
            ignoreSignals_ = true;

            cool_->setCoolTextValues( coolText_ );
            cool_->setDefaultCool( product->coolDefaultTextNum() );
            cool_->setCool( product->coolTextNum() );
            cool_->setReadOnly( false );

            ignoreSignals_ = false;

            // update forced fields
            updateForcedFields();
        }
    }

    // softkeys (136,93)
    else if (( SoftkeyDev == device ) && ( LoadSoftkeyGroup == event ))
    {
        // dont update update operate mode if we are in self service
        if( session_->scaleConfig->internalConfig.ssactive_lock )
            return;

        DomLoadSoftkeyGroup m( msg );

        LOG_DEBUG( "loading primary flashkeys...", NULL );
        dcaPrimary_->setFlashkeys( &m );

        LOG_DEBUG( "loading secondary flashkeys...", NULL );
        dcaSecondary_->setFlashkeys( &m );

        LOG_DEBUG( "loading enhanced center screen flashkeys...", NULL );
        dcaEnhancedCenterScreen_->setFlashkeys( &m ); 

        if ( hobart::softkeysWidget::ForcedCameraOverride == m.featureNum() )
            traySizesExist_ = (0 < m.totalNumOfSoftkeys());
    }

    // toggle flashkey depressed (136,22)
    else if( ( SoftkeyDev == device ) && ( WriteSoftkey == event ) )
    {
        ScaleConfiguration *config = session_->scaleConfig;

        if( !mainWindow_->isSupervisorMode() )
        {
            // If we are in the middle of toggling the Add Extra Shelf Life Day ON OR OFF - Process it
            if( config->toggleExtraDayNow )
            {
                // PJR 420 - Flashkey Options to Toggle Extra Day
                // Sequence of Steps:
                // 1: CLICK Toggle Extra Day Flashkey
                // 2: Send Flashkey action to toggle day or remove it (not done yet)
                // 3: Send 136,40 request to get the default group
                // 4: Receive 136,42 reply with default group
                // 5: Send 136,20 to read the Flashkey clicked
                // 6: Receive 136,22 (this msg) with Flashkey information including secondary image
                // 7: Toggle/Flip the background image

                DomSoftkey m( msg );
                DomCurrentPLU *product( currentProduct() );

                int featNum = config->softkeyFeature.featureNum();
                // Test if we just had a toggle of Add Extra Sell By Day
                if( mainWindow_->isOperateMode() )
                {
                    // updating the left panel is done over in webbrowserscreen not here
                    if( hobart::softkeysWidget::RunScreenButtons == featNum )
                    {
                        if( hobart::dynamicContentWidget::FixedFlashkeys == config->scaleConfig.dcaRunScreenType() )
                        {
                            dcaPrimary_->setToggledFlashkey( &m );
                            dcaSecondary_->setToggledFlashkey( &m );        // added 3/21
                        }
                        else
                        {
                            dcaPrimary_->setPrimaryToggledFlashkey( featNum, &m );
                            dcaSecondary_->setPrimaryToggledFlashkey( featNum, &m );        // added 3/21
                        }
                    }
                    else if( hobart::softkeysWidget::RunScreenNumericKeypadButtons == featNum )
                    {
                        // toggle add sell by extra days flashkey in the number pad
                        dcaSecondary_->setPrimaryToggledFlashkey(featNum, &m );
                    }

                    config->toggleExtraDayNow = false;                  // any toggling that may have been happening with Add Extra Sell By, is done
                    if( product && product->itemNumber().toInt() > 0 )
                    {
                        session_->send( SessionDev, SnReadWeighModeTransactionType );
                        session_->send( ProductDev, ProductRead, product->itemNumber() );
                    }
					else
                    {
                        mainWindow_->setEnabled( true );
                        ignoreSignals_ = true;
                        plu_->setFocus();
                        ignoreSignals_ = false;
                    }
                }
            }
            //else if( ( 0 < config->softkeyFeature.fixedRowColMethod() )  && ( 0 < config->softkeyFeature.defaultButton() ) )
            // Test if we have the fixed flashkeys loaded in the center panel
            else if( hobart::dynamicContentWidget::FixedFlashkeys == config->scaleConfig.dcaRunScreenType() )
            {
                // Might need to test if we have clicked on the toggable col or row first.
                DomSoftkey m( msg );

                // toggles default key
                dcaPrimary_->setToggledFlashkey( &m );

                // added to make left PANEL toggle DEFAULT key - DOES NOTHING
                dcaSecondary_->setToggledFlashkey( &m );
            }
        }   
        mainWindow_->setEnabled( true );
    }
        //  write softkey feature (136,42)
    else if (( SoftkeyDev == device ) && ( WriteSoftkeyFeature == event ))
    {
        ScaleConfiguration *config = session_->scaleConfig;
        DomSoftkeyFeature m( msg );
        int featNum = config->softkeyFeature.featureNum();

        if( !isSelfServiceModeActive() && mainWindow_->isOperateMode() && ( hobart::dynamicContentWidget::FixedFlashkeys != config->scaleConfig.dcaRunScreenType() ) )
        {
            if( ( hobart::softkeysWidget::RunScreenButtons == featNum ) || ( hobart::softkeysWidget::RunScreenNumericKeypadButtons == featNum ) || ( hobart::softkeysWidget::FixedRowColumnGrid == featNum ) )
            {
                // PJR 420 - Flashkey Options to Toggle Extra Day
                // Sequence of Step:
                // 1: CLICK Toggle Extra Day Flashkey
                // 2: Send Flashkey action to toggle day or remove it
                // 3: Send 136,40 request to get the default group
                // 4: Receive 136,42 (this msg) reply with default group
                // 5: Send 136,20 to read the Flashkey clicked
                // 6: Receive 136,22 msg with Flashkey information including secondary image
                // 7: Toggle/Flip the background image

                int posX = config->toggleExtraDayRow;                   // Row
                int posY = config->toggleExtraDayCol;                   // Col

                if( hobart::softkeysWidget::RunScreenNumericKeypadButtons == featNum )
                {
                    posX = config->toggleExtraDayRowNumPad;            // Row
                    posY = config->toggleExtraDayColNumPad;            // Col
                }

                session_->send(
                   SoftkeyDev,
                   ReadSoftkey,
                   QString::number( m.featureNum() ) + "," +
                   QString::number( m.defaultGroupNum() ) + "," +
                   QString::number( posX ) + "," +
                   QString::number( posY ) );
            }
        }
        else if( ( hobart::dynamicContentWidget::FixedFlashkeys == config->scaleConfig.dcaRunScreenType() ) || ( SelfServicePrintOption::SelfServOption_LookupFixedFlashkeys == config->scaleConfig.selfServeItemEntryScreenState() ) )
        {
            // For Toggled Fixed Row/Col Feature
            if( !mainWindow_->isSupervisorMode() )
            {
                int fixedRowCol = 0;
                if( config->toggleExtraDayNow )
                    fixedRowCol = config->toggledFixed;
                else
                    fixedRowCol = m.fixedRowColMethod();

                config->toggledFixed = fixedRowCol;

                // Clicked on Fixed Flashkeys, but we are looking for a flashkey from RunScreen Group. Calculate POS by removing Fixed Row/Col of flashkeys
                int row = toggleFixedXCoordinate( fixedRowCol, m.defaultButton() );         // does NOT take into account if INSIDE of a Fixed Flashey Group
                int col = toggleFixedYCoordinate( fixedRowCol, m.defaultButton() );         // does NOT take into account if INSIDE of a Fixed Flashey Group

                // request a softkey based on its clicked position
                if( config->toggleExtraDayNow )
                {
                    // we clicked in a Fixed Flashkey set, but we clicked a toggle Add Extra Sell By Day flashkey - toggle it instead
                    config->toggleExtraDayRow = row;        // save for later
                    config->toggleExtraDayCol = col;        // this one too

                    // just saw that when we are a fixed flashkey set we could still have clicked the keypad.
                    if( hobart::softkeysWidget::RunScreenNumericKeypadButtons == featNum )
                    {
                        row = config->toggleExtraDayRowNumPad;            // Row
                        col = config->toggleExtraDayColNumPad;            // Col
                    }

                    session_->send(
                       SoftkeyDev,
                       ReadSoftkey,
                       QString::number( m.featureNum() ) + "," +
                       QString::number( m.defaultGroupNum() ) + "," +
                       QString::number( row ) + "," +
                       QString::number( col ) );
                }
                else
                {
                    // proceed with the Fixed Flashkeys like usual
                    if( session_->scaleConfig->internalConfig.ssactive_lock )
                        config->toggledState = true;
                    config->toggledRow = row;
                    config->toggledCol = col;

                    session_->send(
                       SoftkeyDev,
                       ReadSoftkey,
                       QString::number( hobart::softkeysWidget::FixedRowColumnGrid ) + "," +
                       QString::number( m.defaultGroupNum() ) + "," +
                       QString::number( row ) + "," +
                       QString::number( col ) );

                    if( session_->isSelfServiceMode() )
                    {
                        if( session_->scaleConfig->scaleConfig.selfServeEnableFreshServeUI() && ( hobart::softkeysWidget::SelfservCategoryResultsButtons == m.featureNum() ) )
                            LOG_DEBUG( "Only load when we want fixed flashkeys on main lookup or search A-Z", NULL );
                        else
                            session_->send( SoftkeyDev, LoadSoftkeyGroup, QString::number( hobart::softkeysWidget::FixedRowColumnGrid ) );
                    }
                }
            }
        }
    }

    // register session
    else if (( SessionDev == device ) && ( SnRegister == event ))
    {
        LOG_INFO( "session has been registered!", NULL );
        registered_ = true;

        // request softkeys
        resetFlashkeys( true );
    }

    // database restore
    else if (( BackupRestoreDev == device ) && ( Terminate  == event ))
    {
        // FT-1115 request configuration after db restore
        resetFlashkeys( true );
    }

    // send Online Order Totals to the webViewer so it can send them to the blackbox/server
    else if (( ProductDev == device ) && ( ProductWriteOnlineOrderingProductTotals  == event ))
    {
        // FT-1115 request configuration after db restore
        DomOnlineOrderingProductTotals m( msg );
        QString data("{\"DeptNum\":" + QString::number(session_->scaleConfig->currentDept.deptNum()) +
                     ", \"OrderNumber\":" + m.orderNumber() +
                     ", \"OrderNumId\":" + m.orderProductId() +
                     ", \"FinalPrice\":" + m.totalPrice() +
                     ", \"FinalWeight\":" + m.totalNetWeight() +
                     ", \"FinalCount\":" + m.totalByCount() + "}");


        // get the other browsers (left and ECS)
        WebBrowserScreen* leftBrowser = operateMode_->isCached<WebBrowserScreen>();
        hobart::webViewer *ecsWebViewer( dynamic_cast<hobart::webViewer*>( dcaEnhancedCenterScreen_->widget( hobart::dynamicContentWidget::WebBrowser ) ) );

        bool swipe = true;
        // if we're using the ECS and have an OO page, don't swipe
        if (ecsWebViewer)
        {
          QWebView* webView = ecsWebViewer->webView();
          if (webView && webView->url().path().contains("OnlineOrdering"))
          {
            swipe = false;
          }
        }
 
        QString cmd("reportFinal(" + data + "," + (swipe?QString("true"):QString("false")) + ");");
        bool sent = false;

        // try the ECS webview
        if (!swipe)
        {
            LOG_INFO("sending Online Ordering product totals via JS (enhanced center panel webview)", NULL);
            ecsWebViewer->runJavaScript(cmd);
            sent = true;
        }
 
        // try the left panel webview
        if (!sent && leftBrowser)
        {
            //QWebView* webView = leftBrowser->webView();
            //if (leftBrowser->url().path().contains("OnlineOrdering"))
            {
              LOG_INFO("sending Online Ordering product totals via JS (left screen webview)", NULL);

              leftBrowser->runJavascript(cmd);
              sent = true;
            }
        }

        // try the non-ECS center webview
        if (!sent && webView_ )
        {
            QWebView* webView = webView_->webView();
            if (webView && webView->url().path().contains("OnlineOrdering"))
            {
              LOG_INFO("sending Online Ordering product totals via JS (normal webview)", NULL);

              webView_->runJavaScript(cmd);
              sent = true;
            }
        }

        if (!sent)
        {
          LOG_WARN( "Online Ordering product printed but can't find webView to report through!", NULL );
        }
    }
    else if (( NetworkConnectivityDev == device ) && ( CurrentStatus == event ))
    {
        MadpNetworkConnectivity m( msg );
        bool isNetworkConnected = m.isNetworkConnected();

        if (isNetworkConnected && !networkOnline_)
        {
          networkOnline_ = true;
          LOG_INFO("network came online, starting enhanced center panel webview", NULL);
          reloadWebViewers();
        }
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::showEvent( QShowEvent *e )
{
    LOG_CLASS_INFO("PluMainScreen", "showEvent");
    LOG_DEBUG("Entered PluMainScreen::showEvent()", NULL);
    _Mybase::showEvent( e );

    // activate plu entry
    if ( !isAnimating() ){
        LOG_DEBUG("is animating", NULL);
        if ( !isPopupNumberPad() ){
            LOG_DEBUG("setting focus to PluMainScreen", NULL);
            plu_->setFocus();
            LOG_DEBUG("focus is set, returning", NULL);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onBestBeforeDateChanged()
{
    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;
    else if ( ignoreSignals_ )
        return;

    QDateTime now( session_->scaleConfig->currentDate(), session_->scaleConfig->currentTime() );
    now = now.addSecs( 3600 * bestBeforeDate_->hoursModifier() );

    // write
    operateMode_->writeSingleField(
        OperateMode::OffsetProductLifeDays,
        QString::number( abs( bestBeforeDate_->date().daysTo( packedOnDate_->date() ) ) ),
        product->productLifeDaysRaw() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onBestBeforeDateDoNotPrintChanged()
{
    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;
    else if ( ignoreSignals_ )
        return;

    // write
    operateMode_->writeSingleField(
        OperateMode::OffsetDoNotPrintBestBefore,
        (bestBeforeDate_->doNotPrint() ? "1" : "0"),
        product->dontPrintBestBeforeRaw() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onCoolChanged()
{
    LOG_CLASS_INFO( "PluMainScreen", "onCoolChanged" );

    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;
    else if ( ignoreSignals_ )
        return;

    int index = 0;
    QString newValue;
    QString oldValue;

    LOG_DEBUG( "cool text %d been selected", cool_->cool() );

    // a cool value of -1 means "no cool text" (aka no cool required)
    if ( cool_->cool() < 0 )
    {
        index = OperateMode::OffsetCoolRequired;
        newValue = "0";
        oldValue = product->coolRequiredRaw();
    }

    else if ( 0 < cool_->cool() )
    {
        index = OperateMode::OffsetCoolTextNum;
        newValue = QString::number( cool_->cool() );
        oldValue = product->coolTextNumRaw();

        // if cool not required we need to turn it on
        if (( originalCoolRequired_ ) && ( !product->coolRequired() ))
        {
            LOG_DEBUG( "restoring cool required to true", NULL );
            operateMode_->writeSingleField( OperateMode::OffsetCoolRequired, "1", "0" );
        }
    }

    // write
    operateMode_->writeSingleField( (OperateMode::ProductWriteSingleFieldOffset) index, newValue, oldValue );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onCoolDialogActionComplete( int rc )
{
    LOG_CLASS_INFO( "PluMainScreen", "onCoolDialogActionComplete" );

    LOG_TRACE( "cool dialog closed rc %d", rc );

    // cool dialog was closed, update forced fields
    forceCoolDialog_ = false;
    updateForcedFields();

    // FT-595 restore focus
    if ( !operateMode_->isForcedFields() )
        operateMode_->setFocusForPopupNumberPad();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onCoolTrackingNumberChanged()
{
    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;
    else if ( ignoreSignals_ )
        return;

    // write
    operateMode_->writeSingleField(
        OperateMode::OffsetCoolTrackingText,
        cool_->trackingNumber(),
        product->coolTrackingText() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onClearPluForCutTest()
{
    // clear current product details
    clearFields();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onCurrentProductChanged( bool nof )
{
    LOG_CLASS_INFO( "PluMainScreen", "onCurrentProductChanged" );

    DomCurrentPLU *product( currentProduct() );
    ScaleConfiguration *config = session_->scaleConfig;

    if( session_->scaleConfig->internalConfig.ssactive_lock )
    {
        // handle product
        if ( !product )
            return;

        // description
        QString strippedDescription( session_->uiUtilities->stripFontChanges( product->description() ) );

        QString filename = "";
        if( session_->isSelfServiceMode() && config->scaleConfig.enableHelpPrompts() )
        {
            // Get the two scale languages
            QString primaryLang   = config->scaleConfig.languageCode()    + "." + config->scaleConfig.countryCode();
            QString secondaryLang = config->scaleConfig.secLanguageCode() + "." + config->scaleConfig.secCountryCode();

            // This is so we display large help instructions on the ByCount and Random Weight Print Screens
            if( product->productType() == 1 )           // so if Random Weight
            {
                if( 1 == config->currentLanguage )
                {
                    if ( "es.MX" == primaryLang )
                        filename = "rw_instructions_es.png";
                    else if ( "fr.CA" == primaryLang )
                        filename = "rw_instructions_fr.png";
                    else
                        filename = "rw_instructions.png";
                }
                else
                {
                    if ( "es.MX" == secondaryLang )
                        filename = "rw_instructions_es.png";
                    else if ( "fr.CA" == secondaryLang )
                        filename = "rw_instructions_fr.png";
                    else
                        filename = "rw_instructions.png";
                }
            }
            else if(  product->productType() == 3 )     // or if By Count
            {
                if( 1 == config->currentLanguage )
                {
                    if ( "es.MX" == primaryLang )
                        filename = "bc_instructions_es.png";
                    else if ( "fr.CA" == primaryLang )
                        filename = "bc_instructions_fr.png";
                    else
                      filename = "bc_instructions.png";
                }
                else
                {
                    if ( "es.MX" == secondaryLang )
                        filename = "bc_instructions_es.png";
                    else if ( "fr.CA" == secondaryLang )
                        filename = "bc_instructions_fr.png";
                    else
                      filename = "bc_instructions.png";
                }
            }
        }
        else
        {
            // This is so we can display product images on the ByCount and Random Weight Prinit Screens
            filename = product->productGraphic();
        }

        if ( filename.length() )
        {
            LOG_TRACE( "have product image '%s'", qPrintable( filename ) );

            QString imageFilename;

            if( session_->isSelfServiceMode() && config->scaleConfig.enableHelpPrompts() )
                imageFilename = filesystem::absoluteFilePath( filename, folderType::SelfServeImages );
            else
                imageFilename = filesystem::absoluteFilePath( filename, folderType::ProductsImages );

            QPixmap image( imageFilename );

            if ( image.isNull() )
                LOG_WARN( "could not load product image '%s'", qPrintable( filename ) );
            else
            {
                //dcaSecondary_->setProductImage( image );

                // update hook
                sessionHooks::instance()->updateFieldValue( sessionHooks::itemImage, imageFilename );
            }
        }

        // update hook
        sessionHooks::instance()->updateFieldValue( sessionHooks::itemCode, currentItemNumber_ );
        sessionHooks::instance()->updateFieldValue( sessionHooks::itemDescription, strippedDescription );
        sessionHooks::instance()->updateFieldValue( sessionHooks::itemExpandedText, "<pending>" );
    }
    else
    {
        // request softkeys
        resetFlashkeys();

        // clear current product details
        clearFields();

        // update layout
        updateLayout( mainWindow_->numberPadArea() );

        // product not on file, nothing to do
        if ( nof )
        {
            LOG_DEBUG( "cleared product", NULL );

            // lets clear the field also...
            plu_->clear();
            currentItemNumber_.clear();

            return;
        }

        // reset plu input field
        plu_->resetInputTruncationFlag();

        // handle product
        if ( !product )
            return;

        MadpOperatorCanModify *operatorCanModify = &config->operatorCanModify;

        ignoreSignals_ = true;

        // plu
        if ( plu_->text() != (currentItemNumber_ = product->itemNumber()) )
        {
            bool restorePluFocus;

            if ( (restorePluFocus = pluHasFocus()) )
                plu_->clearFocus();

            plu_->setText( currentItemNumber_ );

            if ( restorePluFocus )
                plu_->setFocus();
        }

        // See if we can turn the background orange but we cant be doin any cuttin test
        if( session_->uiUtilities->isFeatureLicensed( SCALE_PICK5 ) && !( session_->scaleConfig->cuttingTestActive ) )
        {
            bool is5Enabled = config->getIsPick5Enabled();

            //ScaleConfiguration *scaleConfiguration( session_->scaleConfig );
            // scaleConfiguration->getIsPick5Enabled();   // menu or flashkey turned it on

            if( is5Enabled )
            {
                // at this point I am testing
                // 1. are we licensed?
                // 2. do we have non blank fields > 0 in weight or price.
                // If so, turn orange. Dialog pops when printing happens.
                const QString minWtStr( product->minWeight() );
                const QString maxWtStr( product->maxWeight() );
                const QString minPrStr( product->minPrice() );
                const QString maxPrStr( product->maxPrice() );

                float minWt = minWtStr.toFloat();
                float maxWt = maxWtStr.toFloat();
                float minPr = minPrStr.toFloat();
                float maxPr = maxPrStr.toFloat();

                if( (minWt >= 0 && maxWt > 0) || (minPr >= 0 && maxPr > 0) )
                {
                    mainWindow_->togglePickFive( true );    // set screen orange
                    config->setIsPick5Active( true );
                }
                else
                {
                    mainWindow_->togglePickFive( false );
                    config->setIsPick5Active( false );
                }
            }
        }           // if licensed

        // description
        QString strippedDescription( session_->uiUtilities->stripFontChanges( product->description() ) );
        description_->setText( strippedDescription );

        // sell by date
        sellByDate_->setAccessLevel( operatorCanModify->shelfLifeAccessLevel() );
        sellByDate_->setBasisDate( session_->scaleConfig->currentDate() );
        sellByDate_->setDate( retrieveTimestamp( product->shelfLifeDays(), product->shelfLifeHours() ) ); // FT-384
        sellByDate_->setDoNotPrint( product->dontPrintSellBy() );
        sellByDate_->setDoNotPrintEnabled( true );
        sellByDate_->setHoursModifier( product->shelfLifeHours() ); // FT-384

        if ( !operatorCanModify->shelfLife() )
            LOG_DEBUG( "operator not allowed to modify sell by date (shelf life)", NULL );
        else
            sellByDate_->setReadOnly( false );

        // cool
        // country of origin is handled when we get the cool list from
        // session (requested in operate mode)

        // tracking
        cool_->setTrackingNumber( product->coolTrackingText() );

        originalCoolRequired_ = product->coolRequired();

        // best before
        bestBeforeDate_->setAccessLevel( operatorCanModify->productLifeAccessLevel() );
        bestBeforeDate_->setBasisDate( session_->scaleConfig->currentDate() );
        bestBeforeDate_->setDate( retrieveTimestamp( product->productLifeDays(), product->productLifeHours() ) ); // FT-384
        bestBeforeDate_->setDoNotPrint( product->dontPrintBestBefore() );
        bestBeforeDate_->setDoNotPrintEnabled( true );
        bestBeforeDate_->setHoursModifier( product->productLifeHours() ); // FT-384

        if ( !operatorCanModify->productLife() )
            LOG_DEBUG( "operator not allowed to modify best before date (product life)", NULL );
        else
            bestBeforeDate_->setReadOnly( false );

        // packed on
        packedOnDate_->setBasisDate( session_->scaleConfig->currentDate() );           // SOF-3866
        packedOnDate_->setDate( retrieveTimestamp( -product->backDateDays() ) );
        packedOnDate_->setDoNotPrint( product->dontPrintPackedOn() );
        packedOnDate_->setDoNotPrintEnabled( true );
        setPackedOnDateReadOnly();

        // FT-816 FPP PLU Weight showing blank - moved to it's own method so it could be called from elsewhere
        // fresh production
        updateProductionPlanningMessage("0");           // could pass the total netWeight, but it's 0 anyway, or blank

        // restore accumulated items mode
        if ( config->accumulateItemsConfig.enableAccumItemsAndFlashkeys() )
            showAccumulateItems();

        QString filename = product->productGraphic();       // This is so we can display product images on the ByCount and Random Weight Prinit Screens

        if ( filename.length() )
        {
            LOG_TRACE( "have product image '%s'", qPrintable( filename ) );

            QString imageFilename;
            if( session_->isSelfServiceMode() && config->scaleConfig.enableHelpPrompts() )
                imageFilename = filesystem::absoluteFilePath( filename, folderType::SelfServeImages );
            else
                imageFilename = filesystem::absoluteFilePath( filename, folderType::ProductsImages );

            QPixmap image( imageFilename );

            if ( image.isNull() )
                LOG_WARN( "could not load product image '%s'", qPrintable( filename ) );
            else
            {
                dcaSecondary_->setProductImage( image );

                // update hook
                sessionHooks::instance()->updateFieldValue( sessionHooks::itemImage, imageFilename );
            }
        }

        // check forced tray entry
        setForcedTrayEntry( ( session_->isPrepack() ) && ( traySizesExist_ ) );

        updateForcedFields();
    }

    ignoreSignals_ = false;
    config->internalConfig.selfServicePreventClicks = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onCurrentProductUpdateField( const DomUpdateRunModeField& m )
{
    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;

    const QString newValue( const_cast<DomUpdateRunModeField*>( &m )->newValue() );

    bool processed = true;
    ignoreSignals_ = true;

    // handle update
    switch ( (OperateMode::ProductWriteSingleFieldOffset) m.offsetIndex() )
    {
    case OperateMode::OffsetShelfLifeDays:
    case OperateMode::OffsetShelfLifeHours:
        sellByDate_->setDate( retrieveTimestamp( packedOnDate_->date(), product->shelfLifeDays(), product->shelfLifeHours() ) ); // FT-384
        sellByDate_->setHoursModifier( product->shelfLifeHours() ); // FT-384
        break;

    case OperateMode::OffsetProductLifeDays:
    case OperateMode::OffsetProductLifeHours:
        bestBeforeDate_->setDate( retrieveTimestamp( packedOnDate_->date(), product->productLifeDays(), product->productLifeHours() ) ); // FT-384
        bestBeforeDate_->setHoursModifier( product->productLifeHours() ); // FT-384
        break;

    case OperateMode::OffsetBackDateDays:
        packedOnDate_->setDate( retrieveTimestamp( -product->backDateDays() ) );

        // FT-564: Adjust Sell By and Best Before dates based on the updated Packed On date
        sellByDate_->setBasisDate( packedOnDate_->date() );
        bestBeforeDate_->setBasisDate( packedOnDate_->date() );

        sellByDate_->setDate( retrieveTimestamp( packedOnDate_->date(), product->shelfLifeDays(), product->shelfLifeHours() ) );
        bestBeforeDate_->setDate( retrieveTimestamp( packedOnDate_->date(), product->productLifeDays(), product->productLifeHours() ) );

        break;

    case OperateMode::OffsetDoNotPrintPackedOn:
        packedOnDate_->setDoNotPrint( m.field<bool>( newValue ) );
        break;

    case OperateMode::OffsetDoNotPrintSellBy:
        sellByDate_->setDoNotPrint( m.field<bool>( newValue ) );
        break;

    case OperateMode::OffsetDoNotPrintBestBefore:
        bestBeforeDate_->setDoNotPrint( m.field<bool>( newValue ) );
        break;

    case OperateMode::OffsetCoolTextNum:
        cool_->setCool( m.field<int>( newValue ) );
        break;

    case OperateMode::OffsetCoolTrackingText:
        cool_->setTrackingNumber( newValue );
        break;

    case OperateMode::OffsetCoolRequired:
        cool_->setCool( product->coolRequired() ? product->coolTextNum() : -1 ); // set cool back to original number
        break;

    default:
        processed = false;
        break;
    }

    // post field processing
    if ( processed )
    {
        // update forced fields
        updateForcedFields();
    }

    ignoreSignals_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onCuttingTestChanged( bool value )
{
    // cutting test
    dcaPrimary_->setCuttingTest( value );

    // update layout
    updateLayout( mainWindow_->numberPadArea() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onInvalidAccessLevel( QWidget *field )
{
    if ( !field )
        return;

    operateMode_->processInvalidAccessLevel( field );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onNumberPadKeyLayoutStyleChanged()
{
    LOG_CLASS_INFO( "PluMainScreen", "onNumberPadKeyLayoutStyleChanged" );

    NumberPadWidget *numberPad( mainWindow_->numberPad() );

    if ( !numberPad )
        LOG_WARN( "number pad invalid", NULL );
    else
    {
        // update dca widget with new appearance
        const QRect numberPadRect( QPoint( 0, 0 ), numberPad->size() );
        const QPixmap numberPadPixmap( numberPad->grab( numberPadRect ) );

        if ( numberPadPixmap.isNull() )
            LOG_WARN( "have invalid pixmap!", NULL );

        LOG_DEBUG( "setting dca numberpad background", NULL );
        dcaSecondary_->setWidgetBackground( hobart::dynamicContentWidget::NumberPad, QBrush( numberPadPixmap ) );

        // Qt seems to resize the widget when we call grab() so lets fix
        // the widget back to it's proper size...
        numberPad->setFixedSize( numberPadRect.size() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onNumberPadMoved( Qt::DockWidgetArea where )
{
    updateLayout( where );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onPackedOnDateChanged()
{
    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;
    else if ( ignoreSignals_ )
        return;

    // write
    operateMode_->writeSingleField(
        OperateMode::OffsetBackDateDays,
        QString::number( abs( packedOnDate_->date().daysTo( session_->scaleConfig->currentDate() ) ) ),
        product->backDateDaysRaw() );

    // FT-564: Adjust Sell By and Best Before dates based on the updated Packed On date
    ignoreSignals_= true;

    sellByDate_->setDate( retrieveTimestamp( packedOnDate_->date(), product->shelfLifeDays(), product->shelfLifeHours() ) );
    bestBeforeDate_->setDate( retrieveTimestamp( packedOnDate_->date(), product->productLifeDays(), product->productLifeHours() ) );

    ignoreSignals_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onPackedOnDateDialogActionComplete( int rc )
{
    LOG_CLASS_INFO( "PluMainScreen", "onPackedOnDateDialogActionComplete" );

    LOG_TRACE( "packed on date dialog closed rc %d", rc );

    // Note:
    // Special case for when operator accepts backdate of 0 days (i.e. today) and forced
    // backdate... the packed on date changed slot will not be called (since it did not change). So
    // here we call it manually.
    if ( QDialog::Accepted == rc )
        if ( packedOnDate_->isForced() )
            onPackedOnDateChanged();

    // packed on date dialog was closed, update forced fields
    forcePackedOnDateDialog_ = false;
    updateForcedFields();

    // FT-595 restore focus
    if ( !operateMode_->isForcedFields() )
        operateMode_->setFocusForPopupNumberPad();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onPackedOnDateDoNotPrintChanged()
{
    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;
    else if ( ignoreSignals_ )
        return;

    // write
    operateMode_->writeSingleField(
        OperateMode::OffsetDoNotPrintPackedOn,
        (packedOnDate_->doNotPrint() ? "1" : "0"),
        product->dontPrintPackedOnRaw() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onPluEditingFinished()
{
    if ( ignoreSignals_ )
        return;

    setCurrentItemNumber( plu_->text() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onPluReturnPressed()
{
    LOG_CLASS_INFO( "PluMainScreen", "onPluReturnPressed" );

    if ( ignoreSignals_ )
        return;

    // FT-957: keep track of print key
    // FT-958: keep track of print key
    // nothing to do when ENTER due to print key press
    if ( session_->scaleConfig->printKeyPressed )
        return;

    // FT-786
    // reset current item number... this will force a refresh of current plu
    if (( 0 != plu_->toInt() ) && ( !plu_->text().isEmpty() ))
    {
        const DomWeighModeTotalType *weighModeTotalType( &session_->scaleConfig->weighModeTotalType );

        DomCurrentPLU *product( currentProduct() );
        if ( !product )
            return;

        // HW-74: do not do this in prepack, when weight on platter it will refresh the current
        // plu and print a label (which is undesired behavior)
        //  Note: This fix for HW-74 is ONLY applicable for a Service counter scale.
        if ( ( session_->isServiceCounter() && OpModeAutoManualApply == weighModeTotalType->weighMode() ) && ( !product->acceptKbdNetWeight() ) )
            LOG_DEBUG( "enter key blocked due to prepack mode", NULL );
        else
            currentItemNumber_.clear();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onPluTextEdited( const QString& text )
{
    // check for entry
    if ( !text.length() )
        return;

    MadpScaleConfig *config = &session_->scaleConfig->scaleConfig;

    // check for auto lookup
    if ( config->autoLookupProduct() )
    {
        int digits = config->autoLookupNumOfDigits();

        // make sure valid number
        if ( digits )
        {
            QString overrideDigit = config->autoLookupOverrideDigit();
            int overrideTop = qMin( pluValidatorTop_, makeIntValidatorTop( digits+1 ) );

            // look for override
            if (( digits == text.length() ) && ( overrideDigit == text.left( 1 ) ) && ( overrideTop != pluValidatorTop_ ))
                pluValidator_->setTop( overrideTop );

            // perform lookup
            else if ( digits <= text.length() )
                onPluEditingFinished();

            // reset validator top
            else
            {
                pluValidator_->setTop( makeIntValidatorTop( digits ) );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onScreenChanged()
{
    // only execute code when we are active screen
    if ( mode_->screen() != this )
        return;

    // make sure weight bar is visible
    if ( operateMode_ )
    {
        operateMode_->showWeightBar();

        // SOF-4921
        dcaPrimary_->resizeProdPlanning();
    }

    // activate plu entry
    if ( !isPopupNumberPad() )
        plu_->setFocus();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onSellByDateChanged()
{
    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;
    else if ( ignoreSignals_ )
        return;

    QDateTime now( session_->scaleConfig->currentDate(), session_->scaleConfig->currentTime() );
    now = now.addSecs( 3600 * sellByDate_->hoursModifier() );

    // write
    operateMode_->writeSingleField(
        OperateMode::OffsetShelfLifeDays,
        QString::number( abs( sellByDate_->date().daysTo( packedOnDate_->date() ) ) ), // FT-384
        product->shelfLifeDaysRaw() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onSellByDateDoNotPrintChanged()
{
    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;
    else if ( ignoreSignals_ )
        return;

    // write
    operateMode_->writeSingleField(
        OperateMode::OffsetDoNotPrintSellBy,
        (sellByDate_->doNotPrint() ? "1" : "0"),
        product->dontPrintSellByRaw() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onWebViewLoadProgress( int progress )
{
    LOG_CLASS_INFO( "PluMainScreen", "onWebViewLoadProgress" );

    if ( dcaPrimaryWebView_ == sender() )
    {
        LOG_DEBUG( "dca primary progress %d", progress );
        dcaPrimaryWebViewProgress_ = progress;
    }
    else if ( dcaSecondaryWebView_ == sender() )
    {
        LOG_DEBUG( "dca secondary progress %d", progress );
        dcaSecondaryWebViewProgress_ = progress;
    }
    else if ( dcaEnhancedCenterScreenWebView_ == sender() )
    {
        LOG_DEBUG( "dca ecs progress %d", progress );
        dcaEnhancedCenterScreenWebViewProgress_ = progress;

        // web view progressing again... stop queued reload
        if (( 0 < progress ) && ( progress < 100 ))
          dcaEnhancedCenterScreenReload_->stop();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onWebViewLoadFinished( bool success )
{
    LOG_CLASS_INFO( "PluMainScreen", "onWebViewLoadFinished" );

    LOG_INFO("success: %d", (int)success);

    if ( dcaEnhancedCenterScreenWebView_ == sender())
    {
        const int progress( dcaEnhancedCenterScreenWebViewProgress_ );

        LOG_DEBUG( "dca ecs finished %d (progress %d)", success, progress );
        dcaEnhancedCenterScreenReload_->stop();

        // refresh web viewer when not successfull
        if ( !success )
            dcaEnhancedCenterScreenReload_->start( 5000 );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::onWidgetSettingsLockChanged( bool locked )
{
    const bool useEnhancedCenterScreen( isEnhancedCenterScreenActive() );

    LOG_CLASS_INFO( "PluMainScreen", "onWidgetSettingsLockChanged" );

    LOG_INFO( "widget lock has changed %d", locked );

    // enable widget controls
    sellByDate_->setToggleButtonVisible( !locked );
    cool_->setToggleButtonVisible( !locked );
    bestBeforeDate_->setToggleButtonVisible( !locked );
    packedOnDate_->setToggleButtonVisible( !locked );

    // locking controls
    if ( locked )
    {
        if ( useEnhancedCenterScreen )
            dcaEnhancedCenterScreen_->hideContentSelectors();
        else
        {
            sellByDate_->setVisible( sellByDate_->widgetAvailable() );
            cool_->setVisible( cool_->widgetAvailable() );
            bestBeforeDate_->setVisible( bestBeforeDate_->widgetAvailable() );
            packedOnDate_->setVisible( packedOnDate_->widgetAvailable() );
            dcaPrimary_->hideContentSelectors();
        }

        dcaSecondary_->hideContentSelectors();

        // save!
        writeConfiguration();
    }

    // unlocking controls
    else
    {
        // hide input widgets
        mainWindow_->hideInputWidgets();

        if ( useEnhancedCenterScreen )
            dcaEnhancedCenterScreen_->showContentSelectors();
        else
        {
            sellByDate_->show();
            cool_->show();
            bestBeforeDate_->show();
            packedOnDate_->show();
            dcaPrimary_->showContentSelectors();
        }

        dcaSecondary_->showContentSelectors();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::initialize()
{
    static const QMargins margins( 10, 10, 10, 6 );

    // pixmaps
    const QSize numberPadSize( mainWindow_->numberPad()->size() );

    const QSize tallSideWidgetSize( scaleSizeForScreen( QSize( 270, 100 ) ) );
    const QSize tallMiddleWidgetSize( mainWindow_->width() - (tallSideWidgetSize.width() * 2), tallSideWidgetSize.height() );

    const QSize shortSideWidgetSize( scaleSizeForScreen( QSize( 270 - MENU_OVERLAY_SPACING, 85 ) ) );
    const QSize shortMiddleWidgetSize( (mainWindow_->width() - (numberPadSize.width() + 10) - shortSideWidgetSize.width() - MENU_OVERLAY_SPACING) / 2, shortSideWidgetSize.height() );

    const hobart::backgroundWidgetRenderer renderField(
        new hobart::rectangleBackgroundWidget(),
        new hobart::darkOperatorBackgroundWidgetRendererAppearance() );

    const hobart::backgroundWidgetRenderer renderForcedField(
        new hobart::rectangleBackgroundWidget(),
        new hobart::forcedOperatorBackgroundWidgetRendererAppearance<0>() );

    const QPixmap field270x100( renderField.drawControl( tallSideWidgetSize, hobart::Normal ) );
    const QPixmap field270x100Focused( renderField.drawControl( tallSideWidgetSize, hobart::Focused ) );
    const QPixmap field270x100ReadOnly( renderField.drawControl( tallSideWidgetSize, hobart::ReadOnly ) );
    const QPixmap field270x100Forced( renderForcedField.drawControl( tallSideWidgetSize, hobart::Normal ) );
    const QPixmap field270x100ForcedFocused( renderForcedField.drawControl( tallSideWidgetSize, hobart::Focused ) );
    const QPixmap field270x100ForcedReadOnly( renderForcedField.drawControl( tallSideWidgetSize, hobart::ReadOnly ) );
    const QPixmap field270x100Shadow( renderField.drawDropShadow( tallSideWidgetSize ) );

    const QPixmap field470x100( renderField.drawControl( tallMiddleWidgetSize, hobart::Normal ) );
    const QPixmap field470x100Focused( renderField.drawControl( tallMiddleWidgetSize, hobart::Focused ) );
    const QPixmap field470x100ReadOnly( renderField.drawControl( tallMiddleWidgetSize, hobart::ReadOnly ) );
    const QPixmap field470x100Shadow( renderField.drawDropShadow( tallMiddleWidgetSize ) );

    const QPixmap field270x85( renderField.drawControl( shortSideWidgetSize, hobart::Normal ) );
    const QPixmap field270x85Focused( renderField.drawControl( shortSideWidgetSize, hobart::Focused ) );
    const QPixmap field270x85ReadOnly( renderField.drawControl( shortSideWidgetSize, hobart::ReadOnly ) );
    const QPixmap field270x85Forced( renderForcedField.drawControl( shortSideWidgetSize, hobart::Normal ) );
    const QPixmap field270x85ForcedFocused( renderForcedField.drawControl( shortSideWidgetSize, hobart::Focused ) );
    const QPixmap field270x85ForcedReadOnly( renderForcedField.drawControl( shortSideWidgetSize, hobart::ReadOnly ) );
    const QPixmap field270x85Shadow( renderField.drawDropShadow( shortSideWidgetSize ) );

    const QPixmap field185x85( renderField.drawControl( shortMiddleWidgetSize, hobart::Normal ) );
    const QPixmap field185x85Focused( renderField.drawControl( shortMiddleWidgetSize, hobart::Focused ) );
    const QPixmap field185x85ReadOnly( renderField.drawControl( shortMiddleWidgetSize, hobart::ReadOnly ) );
    const QPixmap field185x85Forced( renderForcedField.drawControl( shortMiddleWidgetSize, hobart::Normal ) );
    const QPixmap field185x85ForcedFocused( renderForcedField.drawControl( shortMiddleWidgetSize, hobart::Focused ) );
    const QPixmap field185x85ForcedReadOnly( renderForcedField.drawControl( shortMiddleWidgetSize, hobart::ReadOnly ) );
    const QPixmap field185x85Shadow( renderField.drawDropShadow( shortMiddleWidgetSize ) );

    // fonts
    modifyFontPointSize setFontPointSize14( scaleFontPointSizeForScreen( 14 ) );
    modifyFontPointSize setFontPointSize16( scaleFontPointSizeForScreen( 16 ) );
    modifyFontPointSize setFontPointSize24( scaleFontPointSizeForScreen( 24 ) );
    modifyFontPointSize setFontPointSize32( scaleFontPointSizeForScreen( 32 ) );

    QFont scaledFont( font() );
    scaledFont.setPointSize( scaleFontPointSizeForScreen( scaledFont.pointSize() ) );

    setFont( scaledFont );

    // signal mappers
    invalidAccessLevelSignal_ = new QSignalMapper( this );
    connect( invalidAccessLevelSignal_, SIGNAL( mapped(QWidget*) ), SLOT( onInvalidAccessLevel(QWidget*) ) );

    ///////////////////////////////////////////////////////////////////////////
    // create widgets

    const MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

    hasRemoteLic = false;
    QString licenses  = scaleConfig->licensedFeatures();
    if( licenses.contains( "SCALE_REMOTE_PRINTING" ) )
        hasRemoteLic = true;

    // plu
    if ( session_->isServiceCounter() )
        plu_ = new hobart::labeledPluEdit( this, field270x100, false ); // FT-1466
    else
    {
        plu_ = new hobart::labeledPluEdit( this, field270x100, true );	//	HA-176
        plu_->setModeUpdateEnabled( true );
    }

    plu_->setBackgroundFocused( field270x100Focused );
    plu_->setBackgroundReadOnly( field270x100ReadOnly );
    plu_->setDropShadow( field270x100Shadow );
    plu_->setContentsMargins( margins );
    plu_->setObjectName( "plu" );
    plu_->setLabelObjectName( "plu_label" );
    plu_->setTrayTypeObjectName( "plu_traytype" );
    plu_->setTotalTypeObjectName( "plu_totaltype" );
    plu_->setTotalTypeVisible( scaleConfig->showTotalTypeOnRunScreen() ); // FT-1466

    plu_->setInputFocusZeroing( false );
    plu_->setInputTruncation( true );
    plu_->setInputWrapping( true );
    plu_->setValidator( pluValidator_ = new QIntValidator( pluValidatorBottom_, pluValidatorTop_, this ) );
    setFontPointSize32( plu_ );

    connect( plu_, SIGNAL( editingFinished() ), SLOT( onPluEditingFinished() ) );
    connect( plu_, SIGNAL( returnPressed() ), SLOT( onPluReturnPressed() ) ); // FT-786
    connect( plu_, SIGNAL( textEdited(QString) ), SLOT( onPluTextEdited(QString) ) );

    // description
    description_ = new hobart::labeledPlainTextEdit( this, field470x100 );
    description_->setBackgroundFocused( field470x100Focused );
    description_->setBackgroundReadOnly( field470x100ReadOnly );
    description_->setDropShadow( field470x100Shadow );
    description_->setContentsMargins( margins );
    description_->setObjectName( "description" );
    description_->setLabelObjectName( "description_label" );

    description_->setLabelVisible( false );
    setFontPointSize16( description_ );

    // sell by date
    sellByDate_ = new hobart::labeledDateEdit( this, field270x100 );
    sellByDate_->setBackgroundFocused( field270x100Focused );
    sellByDate_->setBackgroundReadOnly( field270x100ReadOnly );
    sellByDate_->setBackground( field270x100Forced, true );
    sellByDate_->setBackgroundFocused( field270x100ForcedFocused, true );
    sellByDate_->setBackgroundReadOnly( field270x100ForcedReadOnly, true );
    sellByDate_->setDropShadow( field270x100Shadow );
    sellByDate_->setContentsMargins( margins );
    sellByDate_->setObjectName( "sellByDate" );
    sellByDate_->setLabelObjectName( "sellByDate_label" );

    sellByDate_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    sellByDate_->setLabelAlignment( Qt::AlignRight | Qt::AlignVCenter  );
    sellByDate_->setDoNotPrintEnabled( true );
    sellByDate_->setToggleButtonEnabled( true );
    setFontPointSize32( sellByDate_ );

    connect( sellByDate_, SIGNAL( dateChanged() ), SLOT( onSellByDateChanged() ) );
    connect( sellByDate_, SIGNAL( doNotPrintChanged() ), SLOT( onSellByDateDoNotPrintChanged() ) );

    invalidAccessLevelSignal_->setMapping( sellByDate_, sellByDate_ );
    connect( sellByDate_, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );

    // cool
    cool_ = new hobart::labeledCoolEdit( this, field270x85 );
    cool_->setBackgroundFocused( field270x85Focused );
    cool_->setBackgroundReadOnly( field270x85ReadOnly );
    cool_->setBackground( field270x85Forced, true );
    cool_->setBackgroundFocused( field270x85ForcedFocused, true );
    cool_->setBackgroundReadOnly( field270x85ForcedReadOnly, true );
    cool_->setDropShadow( field270x85Shadow );
    cool_->setContentsMargins( margins );
    cool_->setObjectName( "cool" );
    cool_->setLabelObjectName( "cool_label" );

    cool_->setToggleButtonEnabled( true );
    setFontPointSize14( cool_ );

    connect( cool_, SIGNAL( coolChanged() ), SLOT( onCoolChanged() ) );
    connect( cool_, SIGNAL( dialogActionComplete(int) ), SLOT( onCoolDialogActionComplete(int) ) );
    connect( cool_, SIGNAL( trackingNumberChanged() ), SLOT( onCoolTrackingNumberChanged() ) );

    invalidAccessLevelSignal_->setMapping( cool_, cool_ );
    connect( cool_, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );

    // best before date
    bestBeforeDate_ = new hobart::labeledDateEdit( this, field185x85 );
    bestBeforeDate_->setBackgroundFocused( field185x85Focused );
    bestBeforeDate_->setBackgroundReadOnly( field185x85ReadOnly );
    bestBeforeDate_->setBackground( field185x85Forced, true );
    bestBeforeDate_->setBackgroundFocused( field185x85ForcedFocused, true );
    bestBeforeDate_->setBackgroundReadOnly( field185x85ForcedReadOnly, true );
    bestBeforeDate_->setDropShadow( field185x85Shadow );
    bestBeforeDate_->setContentsMargins( margins );
    bestBeforeDate_->setObjectName( "bestBeforeDate" );
    bestBeforeDate_->setLabelObjectName( "bestBeforeDate_label" );

    bestBeforeDate_->setDoNotPrintEnabled( true );
    bestBeforeDate_->setToggleButtonEnabled( true );
    setFontPointSize24( bestBeforeDate_ );

    connect( bestBeforeDate_, SIGNAL( dateChanged() ), SLOT( onBestBeforeDateChanged() ) );
    connect( bestBeforeDate_, SIGNAL( doNotPrintChanged() ), SLOT( onBestBeforeDateDoNotPrintChanged() ) );

    invalidAccessLevelSignal_->setMapping( bestBeforeDate_, bestBeforeDate_ );
    connect( bestBeforeDate_, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );

    // packed on date
    packedOnDate_ = new hobart::labeledDateEdit( this, field185x85 );
    packedOnDate_->setBackgroundFocused( field185x85Focused );
    packedOnDate_->setBackgroundReadOnly( field185x85ReadOnly );
    packedOnDate_->setBackground( field185x85Forced, true );
    packedOnDate_->setBackgroundFocused( field185x85ForcedFocused, true );
    packedOnDate_->setBackgroundReadOnly( field185x85ForcedReadOnly, true );
    packedOnDate_->setDropShadow( field185x85Shadow );
    packedOnDate_->setContentsMargins( margins );
    packedOnDate_->setObjectName( "packedOnDate" );
    packedOnDate_->setLabelObjectName( "packedOnDate_label" );

    packedOnDate_->setBackDating( true );
    packedOnDate_->setDoNotPrintEnabled( true );
    packedOnDate_->setToggleButtonEnabled( true );
    setFontPointSize24( packedOnDate_ );
/*
    tof_ = new hobart::labeledPlainTextEdit(this, field185x85);
    tof_->setBackgroundFocused( field185x85Focused );
    tof_->setBackgroundReadOnly( field185x85ReadOnly );
    tof_->setDropShadow( field185x85Shadow );
    tof_->setContentsMargins( margins );
    tof_->setObjectName( "tof" );
    tof_->setLabelObjectName( "tof_label" );
    setFontPointSize24( tof_ );

    tof2_ = new hobart::labeledPlainTextEdit(this, field185x85);
    tof2_->setBackgroundFocused( field185x85Focused );
    tof2_->setBackgroundReadOnly( field185x85ReadOnly );
    tof2_->setDropShadow( field185x85Shadow );
    tof2_->setContentsMargins( margins );
    tof2_->setObjectName( "tof2" );
    tof2_->setLabelObjectName( "tof2_label" );
    setFontPointSize24( tof2_ );
*/
    connect( packedOnDate_, SIGNAL( dateChanged() ), SLOT( onPackedOnDateChanged() ) );
    connect( packedOnDate_, SIGNAL( dialogActionComplete(int) ), SLOT( onPackedOnDateDialogActionComplete(int) ) );
    connect( packedOnDate_, SIGNAL( doNotPrintChanged() ), SLOT( onPackedOnDateDoNotPrintChanged() ) );

    invalidAccessLevelSignal_->setMapping( packedOnDate_, packedOnDate_ );
    connect( packedOnDate_, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );

    // RANDY - hook for NEW DELI VISION license
    // SCALE_ENHANCED_CENTERSCREEN

    // primary dca area
    int dcaWidgets( dcaPrimaryWidgets_ );
    int dcaWidgetsExcluded( dcaPrimaryWidgetsExcluded_ );

    // HTs scale
    // this feature is disabled
    if ( !session_->uiUtilities->isFeatureLicensed( SCALE_WEB_BROWSER ) )
        dcaWidgets &= ~hobart::dynamicContentWidget::WebBrowser;

    // HTi Access
    // append cutting test
    //if ( session_->isPrepack() )      // cut test is no longer restricted to prepack
    MadpScaleConfig *madpConfig = &session_->scaleConfig->scaleConfig;
    int cuttesttype = madpConfig->cuttingTestType();

    if( CutTestOption::CutTestOption_Default == cuttesttype )       // Default
    {
        dcaWidgets |= hobart::dynamicContentWidget::CutTestFlashkeysDefault;
        dcaWidgetsExcluded |= hobart::dynamicContentWidget::CutTestFlashkeysDefault;
    }
    else if( CutTestOption::CutTestOption_Option1 == cuttesttype )      // Option1
    {
        dcaWidgets |= hobart::dynamicContentWidget::CutTestFlashkeysOptionOne;
        dcaWidgetsExcluded |= hobart::dynamicContentWidget::CutTestFlashkeysOptionOne;
    }

    dcaPrimary_ = new hobart::dynamicContentWidget( (hobart::dynamicContentWidget::WidgetType) dcaWidgets, this );
    dcaPrimary_->setExcludedContent( (hobart::dynamicContentWidget::WidgetType) dcaWidgetsExcluded );
    dcaPrimary_->setFlashkeysFeature( dcaPrimarySoftkeyGroup_ );
    dcaPrimary_->setObjectName( "dynamicContentPrimary" );

    connect( dcaPrimary_, SIGNAL( flashkeyClicked(QWidget*,QWidget*,int,QString,int) ), operateMode_, SLOT( handleFlashkey(QWidget*,QWidget*,int,QString,int) ) );

    // retrieve softkeys
    hobart::softkeysWidget *dcaPrimarySoftkeys;

    if ( (dcaPrimarySoftkeys = dynamic_cast<hobart::softkeysWidget*>( dcaPrimary_->widget( hobart::dynamicContentWidget::Flashkeys ) )) )
        dcaPrimarySoftkeys->setFactory( new dynamicSoftkeyButtonFactory() );

    // retrieve web view
    if ( (webView_ = dynamic_cast<hobart::webViewer*>( dcaPrimary_->widget( hobart::dynamicContentWidget::WebBrowser ) )) )
    {
        webView_->setToolBarVisible( true ); // TODO: should toolbar be hidden all the time??
        webView_->setDisplayAsDialog( false );

        if ( (dcaPrimaryWebView_ = webView_->webView()) )
        {
            connect( dcaPrimaryWebView_, SIGNAL(loadProgress(int)), SLOT(onWebViewLoadProgress(int)) );
            connect( dcaPrimaryWebView_, SIGNAL(loadFinished(bool)), SLOT(onWebViewLoadFinished(bool)) );

            dcaPrimaryWebView_->installEventFilter( this );
        }
    }

    // secondary dca area
    dcaWidgets = dcaSecondaryWidgets_;
    dcaWidgetsExcluded = dcaSecondaryWidgetsExcluded_;

    // FT-1779 - removed the check for "if ( session_->isPrepack() )" before the following line
    dcaWidgets |= hobart::dynamicContentWidget::RunTotals;

    dcaSecondary_ = new hobart::dynamicContentWidget( (hobart::dynamicContentWidget::WidgetType) dcaWidgets, this );
    dcaSecondary_->setExcludedContent( (hobart::dynamicContentWidget::WidgetType) dcaWidgetsExcluded );
    dcaSecondary_->setFixedSize( numberPadSize );
    dcaSecondary_->setFlashkeysFeature( dcaSecondarySoftkeyGroup_ );
    dcaSecondary_->setObjectName( "dynamicContentSecondary" );

    // retrieve softkeys
    hobart::softkeysWidget *dcaSecondarySoftkeys;

    if ( (dcaSecondarySoftkeys = dynamic_cast<hobart::softkeysWidget*>( dcaSecondary_->widget( hobart::dynamicContentWidget::Flashkeys ) )) )
        dcaSecondarySoftkeys->setFactory( new dynamicSoftkeyButtonFactory() );

    if ( (dcaSecondarySoftkeys = dynamic_cast<hobart::softkeysWidget*>( dcaSecondary_->widget( hobart::dynamicContentWidget::FixedFlashkeys ) )) )
        dcaSecondarySoftkeys->setFactory( new dynamicSoftkeyButtonFactory() );

    // retrieve web view
    hobart::webViewer *secWebView;

    if ( (secWebView = dynamic_cast<hobart::webViewer*>( dcaSecondary_->widget( hobart::dynamicContentWidget::WebBrowser ) )) )
    {
        if ( (dcaSecondaryWebView_ = secWebView->webView()) )
        {
            connect( dcaSecondaryWebView_, SIGNAL(loadProgress(int)), SLOT(onWebViewLoadProgress(int)) );
            connect( dcaSecondaryWebView_, SIGNAL(loadFinished(bool)), SLOT(onWebViewLoadFinished(bool)) );

            dcaSecondaryWebView_->installEventFilter( this );
        }
    }

    connect( dcaSecondary_, SIGNAL( flashkeyClicked(QWidget*,QWidget*,int,QString,int) ), operateMode_, SLOT( handleFlashkey(QWidget*,QWidget*,int,QString,int) ) );

    // enhanced center screen dca area
    dcaEnhancedCenterScreen_ = new hobart::dynamicContentWidget( (hobart::dynamicContentWidget::WidgetType) dcaEnhancedCenterScreenWidgets_, this );
    dcaEnhancedCenterScreen_->setExcludedContent( (hobart::dynamicContentWidget::WidgetType) dcaEnhancedCenterScreenWidgetsExcluded_ );
    dcaEnhancedCenterScreen_->setFlashkeysFeature( dcaEnhancedCenterScreenSoftkeyGroup_ );
    dcaEnhancedCenterScreen_->setObjectName( "dynamicContentEnhancedCenterScreen" );

    // retrieve softkeys
    hobart::softkeysWidget *dcaEnhancedCenterScreenSoftkeys;

    if ( (dcaEnhancedCenterScreenSoftkeys = dynamic_cast<hobart::softkeysWidget*>( dcaEnhancedCenterScreen_->widget( hobart::dynamicContentWidget::Flashkeys ) )) )
        dcaEnhancedCenterScreenSoftkeys->setFactory( new dynamicSoftkeyButtonFactory() );

    if ( (dcaEnhancedCenterScreenSoftkeys = dynamic_cast<hobart::softkeysWidget*>( dcaEnhancedCenterScreen_->widget( hobart::dynamicContentWidget::FixedFlashkeys ) )) )
        dcaEnhancedCenterScreenSoftkeys->setFactory( new dynamicSoftkeyButtonFactory() );

    // retrieve web view
    hobart::webViewer *enhWebView;

    if ( (enhWebView = dynamic_cast<hobart::webViewer*>( dcaEnhancedCenterScreen_->widget( hobart::dynamicContentWidget::WebBrowser ) )) )
    {
        if ( (dcaEnhancedCenterScreenWebView_ = enhWebView->webView()) )
        {
            connect( dcaEnhancedCenterScreenWebView_, SIGNAL(loadProgress(int)), SLOT(onWebViewLoadProgress(int)) );
            connect( dcaEnhancedCenterScreenWebView_, SIGNAL(loadFinished(bool)), SLOT(onWebViewLoadFinished(bool)) );

            dcaEnhancedCenterScreenReload_ = new QTimer( this );
            dcaEnhancedCenterScreenReload_->setSingleShot( true );

            connect( dcaEnhancedCenterScreenReload_, SIGNAL(timeout()), dcaEnhancedCenterScreenWebView_, SLOT(reload()) );

            dcaEnhancedCenterScreenWebView_->installEventFilter( this );
        }
    }

    connect( dcaEnhancedCenterScreen_, SIGNAL( flashkeyClicked(QWidget*,QWidget*,int,QString,int) ), operateMode_, SLOT( handleFlashkey(QWidget*,QWidget*,int,QString,int) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::createLayout()
{
    onNumberPadKeyLayoutStyleChanged();
    updateLayout( mainWindow_->numberPadArea() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::updateLayout( Qt::DockWidgetArea dockLocation )
{
    const bool useEnhancedCenterScreen( isEnhancedCenterScreenActive() );

    // only execute code when we are active screen
    // SOF-4921
    if ( mode_->screen() == this && operateMode_ )
    {
        dcaPrimary_->resizeProdPlanning();
    }

    // optimization logic
    // if exists an old layout and nothing has changed since last time then exit, there is nothing to do
    static bool prevUseEnhancedCenterScreen( false );
    static Qt::DockWidgetArea prevDockLocation( Qt::NoDockWidgetArea );

    if (( prevUseEnhancedCenterScreen == useEnhancedCenterScreen ) &&
        ( prevDockLocation == dockLocation ))
    {
        return;
    }

    const bool useEnhancedCenterScreenChanged( prevUseEnhancedCenterScreen != useEnhancedCenterScreen );

    prevUseEnhancedCenterScreen = useEnhancedCenterScreen;
    prevDockLocation = dockLocation;

    // delete old layout
    QLayout *oldLayout( layout() );

    if ( oldLayout )
        delete oldLayout;

    // ======================
    // Enhanced Center Screen
    // ======================

    if ( useEnhancedCenterScreen )
    {
        // +----------------------------+---------+
        // |                            |   PLU   |
        // |                            +---------+
        // |                            |         |
        // |          Web Page          |         |
        // |                            | Keypad  |
        // |                            |         |
        // |                            |         |
        // +----------------------------+---------+

        description_->hide();
        sellByDate_->hide();
        //tof_->hide();
        //tof2_->hide();

        cool_->hide();
        bestBeforeDate_->hide();
        packedOnDate_->hide();

        dcaPrimary_->hide();
        dcaEnhancedCenterScreen_->show();

        //
        // LEFT
        //

        QVBoxLayout *left( new QVBoxLayout() );
        left->setContentsMargins( QMargins( 5, 5, 5, 5 ) );
        left->setSpacing( 0 );

        left->addWidget( dcaEnhancedCenterScreen_ );

        //
        // RIGHT
        //

        QVBoxLayout *right( new QVBoxLayout() );
        right->setContentsMargins( QMargins( 5, 0, 5, 5 ) );
        right->setSpacing( 0 );

        right->addWidget( plu_ );
        right->addStretch();
        right->addWidget( dcaSecondary_ );

        //
        // SCREEN
        //

        QHBoxLayout *screenLayout( new QHBoxLayout( this ) );
        screenLayout->setContentsMargins( QMargins() );
        screenLayout->setSpacing( 0 );

        if ( Qt::RightDockWidgetArea == dockLocation )
        {
            screenLayout->addStretch();
            screenLayout->addLayout( left, 1 );
            screenLayout->addLayout( right );
        }
        else
        {
            screenLayout->addLayout( right );
            screenLayout->addLayout( left, 1 );
            screenLayout->addStretch();
        }
    }

    // ======================
    // Normal Center Screen
    // ======================

    else
    {
        // +---------+------------------+---------+
        // |   PLU   | Description      | SELL BY |
        // +---------+------------------+---------+
        // |   COOL  | BestB4 |  Packed |         |
        // +---------+--------+---------|         |
        // |                            | Keypad  |
        // |       Dynamic Content      |         |
        // |                            |         |
        // +----------------------------+---------+

        description_->show();
        sellByDate_->show();
        //tof_->show();
        //tof2_->show();

        cool_->show();
        bestBeforeDate_->show();
        packedOnDate_->show();

        dcaPrimary_->show();
        dcaEnhancedCenterScreen_->hide();

        //
        // TOP
        //

        // top row (plu, plu description, sell by date)
        QHBoxLayout *top( new QHBoxLayout() );
        top->setContentsMargins( QMargins() );
        top->setSpacing( 0 );

        top->addWidget( plu_ );
        top->addWidget( description_ );
        top->addWidget( sellByDate_ );
        top->addStretch();

        //
        // BOTTOM LEFT
        //

        // bottom left side TOP (cool, best before, packed on)
        QHBoxLayout *bottomLeftTop( new QHBoxLayout() );
        bottomLeftTop->setContentsMargins( QMargins() );
        bottomLeftTop->setSpacing( 0 );

        if ( Qt::LeftDockWidgetArea == dockLocation )
            bottomLeftTop->addStretch();

        bottomLeftTop->addWidget( cool_ );
        bottomLeftTop->addWidget( bestBeforeDate_ );
        bottomLeftTop->addWidget( packedOnDate_ );

        //bottomLeftTop->addWidget( tof_ );
        //bottomLeftTop->addWidget( tof2_ );

        if ( Qt::RightDockWidgetArea == dockLocation )
            bottomLeftTop->addStretch();

        // bottom left side BOTTOM (primary dca)
        QHBoxLayout *bottomLeftBottom( new QHBoxLayout() );
        bottomLeftBottom->setContentsMargins( QMargins( 5, 5, 5, 5 ) );
        bottomLeftBottom->setSpacing( 0 );

        bottomLeftBottom->addWidget( dcaPrimary_, 1 );

        // bottom left side
        QVBoxLayout *bottomLeft( new QVBoxLayout() );
        bottomLeft->setContentsMargins( QMargins() );
        bottomLeft->setSpacing( 0 );

        bottomLeft->addLayout( bottomLeftTop );
        bottomLeft->addLayout( bottomLeftBottom );

        //
        // BOTTOM RIGHT
        //

        // bottom right side (secondary dca)
        QVBoxLayout *bottomRight( new QVBoxLayout() );
        bottomRight->setContentsMargins( QMargins( 5, 0, 5, 5 ) );
        bottomRight->setSpacing( 0 );

        bottomRight->addStretch();
        bottomRight->addWidget( dcaSecondary_ );

        //
        // BOTTOM
        //

        // bottom
        QHBoxLayout *bottom( new QHBoxLayout() );
        bottom->setContentsMargins( QMargins() );
        bottom->setSpacing( 0 );

        if ( Qt::RightDockWidgetArea == dockLocation )
        {
            bottom->addSpacing( MENU_OVERLAY_SPACING );
            bottom->addLayout( bottomLeft, 1 );
            bottom->addLayout( bottomRight );
        }
        else
        {
            bottom->addLayout( bottomRight );
            bottom->addLayout( bottomLeft, 1 );
            bottom->addSpacing( MENU_OVERLAY_SPACING );
        }

        //
        // SCREEN
        //

        QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
        screenLayout->setContentsMargins( QMargins() );
        screenLayout->setSpacing( 0 );

        screenLayout->addStretch();
        screenLayout->addLayout( top );
        screenLayout->addLayout( bottom );
    }

    // check if we need to update bitmap for PLU widget
    if ( useEnhancedCenterScreenChanged )
    {
        QSize tallSideWidgetSize;

        if ( useEnhancedCenterScreen )
            tallSideWidgetSize = scaleSizeForScreen( QSize( mainWindow_->numberPad()->width(), 100 ) );
        else
            tallSideWidgetSize = scaleSizeForScreen( QSize( 270, 100 ) );

        const hobart::backgroundWidgetRenderer renderField(
            new hobart::rectangleBackgroundWidget(),
            new hobart::darkOperatorBackgroundWidgetRendererAppearance() );

        const QPixmap field270x100( renderField.drawControl( tallSideWidgetSize, hobart::Normal ) );
        const QPixmap field270x100Focused( renderField.drawControl( tallSideWidgetSize, hobart::Focused ) );
        const QPixmap field270x100ReadOnly( renderField.drawControl( tallSideWidgetSize, hobart::ReadOnly ) );
        const QPixmap field270x100Shadow( renderField.drawDropShadow( tallSideWidgetSize ) );

        plu_->setBackground( field270x100 );
        plu_->setBackgroundFocused( field270x100Focused );
        plu_->setBackgroundReadOnly( field270x100ReadOnly );
        plu_->setDropShadow( field270x100Shadow );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::updateForcedFields()
{
    DomCurrentPLU *product( currentProduct() );

    if ( product )
    {
        bool forcedCool( false );
        bool forcedPackedOnDate( (product->forcedBackDateDays() && product->backDateDaysRaw().trimmed().isEmpty()) );

        // make sure we have values before we force!
        // FT-178 force verification even when no cool text
        //if ( coolText_.size() )
        {
            MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

            // "cool is required" turned on (plu field) and no country of origin
            if ( product->coolRequired() )
            {
                forcedCool |= product->coolTextNumRaw().trimmed().isEmpty();

                // "enable cool tracking number" turned on (normal scale operations) and no tracking number
                if ( scaleConfig->coolTrackingNum() )
                    forcedCool |= product->coolTrackingText().trimmed().isEmpty();
            }
        }

        // set forced
        cool_->setForced( forcedCool );
        packedOnDate_->setForced( forcedPackedOnDate );

        // process forced fields
        if ( operateMode_->isForcedFields() )
            operateMode_->processForcedFields();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::clearFields()
{
    LOG_CLASS_INFO( "PluMainScreen", "clearFields" );

    LOG_DEBUG( "clearing fields...", NULL );
    ignoreSignals_ = true;

    plu_->clear();
    currentItemNumber_.clear();

    description_->clear();
    description_->setReadOnly( true );
/*
    tof_->clear();
    tof_->setReadOnly( true );

    tof2_->clear();
    tof2_->setReadOnly( true );
*/
    sellByDate_->clear();
    sellByDate_->setDoNotPrintEnabled( false );
    sellByDate_->setReadOnly( true );

    cool_->clear();
    cool_->clearSecond();
    cool_->setForced( false );
    cool_->setReadOnly( true );

    coolText_.clear();
    originalCoolRequired_ = false;
    forceCoolDialog_ = false;
    forcePackedOnDateDialog_ = false;
    forceNumberOfLabelsDialog_ = false;
    forceNetWeightDialog_ = false;
    forceExpirationDaysDialog_ = false;

    bestBeforeDate_->clear();
    bestBeforeDate_->setDoNotPrintEnabled( false );
    bestBeforeDate_->setReadOnly( true );

    packedOnDate_->clear();
    packedOnDate_->setForced( false );
    packedOnDate_->setDoNotPrintEnabled( false );
    packedOnDate_->setReadOnly( true );

    // HT-410
    if( !( session_->scaleConfig->cuttingTestActive ) )
    {
        dcaPrimary_->setFreshProduction( false );
        dcaSecondary_->resetProductImage();
    }

    setForcedTrayEntry( false );

    ignoreSignals_ = false;

    // update layout
    updateLayout( mainWindow_->numberPadArea() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DomCurrentPLU *PluMainScreen::currentProduct() const
{
    DomCurrentPLU *product = (operateMode_ ? operateMode_->currentProduct() : NULL);

    if ( product )
    {
        QString itemNumber( product->itemNumber() );

        if ( itemNumber.isEmpty() )
            product = NULL;
    }

    return product;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::setPluAutoLookup()
{
    MadpScaleConfig *config = &session_->scaleConfig->scaleConfig;

    // no auto lookup set
    if ( !config->autoLookupProduct() )
        pluValidator_->setTop( pluValidatorTop_ );
    else
    {
        int digits = config->autoLookupNumOfDigits();

        // set validator top
        if ( digits )
            pluValidator_->setTop( makeIntValidatorTop( digits ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::setOperatorDisplayFields()
{
    const bool useEnhancedCenterScreen( isEnhancedCenterScreenActive() );

    LOG_CLASS_INFO( "PluMainScreen", "setOperatorDisplayFields" );

    MadpOperatorDisplayFields *config = &session_->scaleConfig->operatorDisplayFields;
    hobart::dynamicContentWidget::WidgetType index;

    // sell by date
    sellByDate_->setVisible( config->enableSellBy() && !useEnhancedCenterScreen );
    sellByDate_->setWidgetAvailable( config->enableSellBy() );

    // cool
    bool enableCool = (config->enableCountryOfOrgin() || config->enableTrackingNumber());
    cool_->setVisible( enableCool && !useEnhancedCenterScreen );
    cool_->setWidgetAvailable( enableCool );

    cool_->setCoolEnabled( config->enableCountryOfOrgin() );
    cool_->setTrackingNumberEnabled( config->enableTrackingNumber() );

    // best before
    bestBeforeDate_->setVisible( config->enableBestBefore() && !useEnhancedCenterScreen );
    bestBeforeDate_->setWidgetAvailable( config->enableBestBefore() );

    // packed on
    packedOnDate_->setVisible( config->enablePackedOn() && !useEnhancedCenterScreen );
    packedOnDate_->setWidgetAvailable( config->enablePackedOn() );

    // run screen dca type
    index = (hobart::dynamicContentWidget::WidgetType) config->dcaRunScreenType();

    // do not restore if excluded
    if ( dcaPrimaryWidgetsExcluded_ & index )
        LOG_DEBUG( "not restoring primary excluded", NULL );
    else
    {
        dcaPrimary_->setCurrentIndex( index );
    }

    // keypad area dca type
    index = (hobart::dynamicContentWidget::WidgetType) config->dcaKeypadAreaType();

    // do not restore if excluded
    if ( dcaSecondaryWidgetsExcluded_ & index )
        LOG_DEBUG( "not restoring secondary excluded", NULL );
    else
    {
        dcaSecondary_->setCurrentIndex( index );
    }

    dcaEnhancedCenterScreen_->setCurrentIndex( hobart::dynamicContentWidget::WebBrowser );

    // wrapper mode
    if ( session_->isPrepack() )
    {
        if( FAKE_WRAPPER )
            plu_->setModeButtonVisible( true );
        else
            plu_->setModeButtonVisible( config->enableWrapTypes() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::setPackedOnDateReadOnly()
{
    DomCurrentPLU *product( currentProduct() );
    bool readOnly = true;

    // check for a valid product
    if ( product )
    {
        DomWeighModeTotalType *weighModeTotalType = &session_->scaleConfig->weighModeTotalType;

        // enable packed on date when backdating allowed
        if ( weighModeTotalType->allowProductBackdating() )
            readOnly = false;
    }

    packedOnDate_->setReadOnly( readOnly );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::writeConfiguration()
{
    LOG_CLASS_INFO( "PluMainScreen", "writeConfiguration" );

    MadpOperatorDisplayFields *operatorDisplayFields = &session_->scaleConfig->operatorDisplayFields;
    MadpScaleConfig *scaleConfig = &session_->scaleConfig->scaleConfig;

    // sell by
    operatorDisplayFields->setEnableSellBy( sellByDate_->isVisible() );

    // cool
    if (( cool_->isVisible() ) && ( !operatorDisplayFields->enableCountryOfOrgin() ) && ( !operatorDisplayFields->enableTrackingNumber() ))
    {
        operatorDisplayFields->setEnableCountryOfOrgin( true );
        operatorDisplayFields->setEnableTrackingNumber( scaleConfig->coolTrackingNum() ); // if tracking number enabled then show it
    }
    else if ( !cool_->isVisible() )
    {
        operatorDisplayFields->setEnableCountryOfOrgin( false );
        operatorDisplayFields->setEnableTrackingNumber( false );
    }

    // best before
    operatorDisplayFields->setEnableBestBefore( bestBeforeDate_->isVisible() );

    // packed on
    operatorDisplayFields->setEnablePackedOn( packedOnDate_->isVisible() );

    // run screen dca type
    // do not save if excluded
    if ( dcaPrimaryWidgetsExcluded_ & dcaPrimary_->currentIndex() )
        LOG_DEBUG( "not saving primary excluded", NULL );
    else
    {
        operatorDisplayFields->setDcaRunScreenType( dcaPrimary_->currentIndex() );
    }

    // keypad area dca type
    // do not save if excluded
    if ( dcaSecondaryWidgetsExcluded_ & dcaSecondary_->currentIndex() )
        LOG_DEBUG( "not saving secondary excluded", NULL );
    else
    {
        operatorDisplayFields->setDcaKeypadAreaType( dcaSecondary_->currentIndex() );
    }

    LOG_DEBUG( "sending screen configuration...", NULL );
    session_->send( *operatorDisplayFields );
    session_->send( ConfigurationDev, CfgPrepackSave );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::promptNumberOfLabels()
{
    LOG_CLASS_INFO( "PluMainScreen", "promptNumberOfLabels" );

    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;

    bool clearTheProduct( false );

    NumericInputBoxDialog dialog( mainWindow_ );

    // FT-1471: added cancel button to number of labels prompt; when cancelling
    // we will clear the product

    dialog.setText( tr( "Please enter number of labels:" ) );
    dialog.setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );

    LOG_INFO( "processing forced number of labels...", NULL );
    QString result;

    if ( QDialog::Accepted == dialog.exec() )
        result = dialog.inputText();
    else
    {
        LOG_WARN( "forced number of labels was not accepted", NULL );
        clearTheProduct = true;
    }

    if (( result.isEmpty() ) || ( "0" == result ))
    {
        LOG_WARN( "nothing entered for number of labels", NULL );
        clearTheProduct = true;
        result = "0";
    }

    LOG_DEBUG( "updating number of labels to '%s'", qPrintable( result ) );
    operateMode_->writeSingleField( OperateMode::OffsetNumOfLabels, result, product->numOfLabelsRaw() );

    if( printBeforeNumOfLabelsEntered_ && ( ( !result.isEmpty() ) && ( "0" != result ) ) )
        session_->send( SessionDev, SnPrintKeyPressed );

    printBeforeNumOfLabelsEntered_ = false;

    if ( clearTheProduct )
        setCurrentItemNumber( "0" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::promptNetWeight()
{
    LOG_CLASS_INFO( "PluMainScreen", "promptNetWeight" );

    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;    

    NumericInputBoxDialog dialog( mainWindow_ );

    dialog.setText( tr( "Please enter net weight:" ) );
    dialog.setRequired( true );

    dialog.setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );

    // set decimal
    MadpCurrencySettings *currencySettings( &session_->scaleConfig->currencySettings );
    dialog.setDecimal( currencySettings->unitSeparator() );

    // set suffix
    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );
    dialog.setSuffix( scaleConfig->isScaleMetric() ? " kg" : " lb" );

    // Note: Now we have config param to indicate the number of decimal digits.
    int valid;
    if (scaleConfig->isScaleMetric())
    {
        valid = 999999; // it's always a 3 digit characteristic and 3 digit mantissa
        dialog.setDecimalPlaces(3);
    }
    else
    {
        valid = ( (2 == scaleConfig->keyboardNetWeightPrecision()) ? 99999 : 999999 ); //  HTA-100:  999.99 or 999.999
        dialog.setDecimalPlaces( scaleConfig->keyboardNetWeightPrecision() );
    }
    dialog.setInputWrapping(true);

    dialog.setValidator( new QIntValidator( 0, valid, this ) );

    LOG_INFO( "processing forced net weight...", NULL );

    QString result( "-1" );

    if ( QDialog::Accepted == dialog.exec() )
        result = dialog.inputText();
    else
        LOG_WARN( "forced net weight was rejected", NULL );

    if ( result.isEmpty() )
    {
        LOG_WARN( "nothing entered for net weight", NULL );
        return;
    }

    LOG_DEBUG( "updating net weight to '%s'", qPrintable( result ) );
    operateMode_->writeSingleField( OperateMode::OffsetKeyboardNetWeight, result, product->keyboardNetWeight() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::promptExpirationDays()
{
    LOG_CLASS_INFO( "PluMainScreen", "promptExpirationDays" );
    
    // SOF-5076 -1 tells the backend that the user cancelled out
    QString days("-1");

    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;

    NumericInputBoxDialog dialog( mainWindow_ );
    QIntValidator *v = new QIntValidator( 0, 999, this );

    dialog.setText( tr( "Days until product expires:" ) );
    dialog.setValidator( v );

    LOG_INFO( "processing forced expiration days...", NULL );

    // prompt!
    if ( QDialog::Accepted != dialog.exec() )
        LOG_WARN( "forced expiration days was not accepted", NULL );
    else
    {
        QString result( dialog.inputText() );

        // validate entry
        if ( result.isEmpty() )
            LOG_WARN( "nothing entered for expiration days", NULL );
        else
            days = result;
    }

    LOG_INFO( "updating expiration days to '%s'", qPrintable( days ) );
    operateMode_->writeSingleField( OperateMode::OffsetExpirationDays, days, product->expirationDaysRaw() );

    // cleanup
    dialog.setValidator( NULL );
    v->deleteLater();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PluMainScreen::isSelfServiceModeActive()
{
    bool retVal = false;

    ScaleConfiguration *config = session_->scaleConfig;

    if( config->internalConfig.activate_selfservice || config->internalConfig.ssactive_lock )
        retVal = true;

    return retVal;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int PluMainScreen::makeIntValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }

    return top;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PluMainScreen::isWebViewChild( QWidget *obj ) const
{
    if ( !obj )
        return false;

    QList<const QWidget*> webViews;
    webViews.append( dcaPrimaryWebView_ );
    webViews.append( dcaSecondaryWebView_ );
    webViews.append( dcaEnhancedCenterScreenWebView_ );

    // check for web view(s) ancestor of obj
    foreach ( const QWidget *w, webViews )
        if (( w ) && (( w == obj ) || ( w->isAncestorOf( obj ) )))
            return true;

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluMainScreen::resizeEvent(QResizeEvent *event)
{
    LOG_CLASS_INFO( "PluMainScreen", "resizeEvent" );

    // only execute code when we are active screen
    if ( mode_->screen() == this && operateMode_ )
    {
        // SOF-4921
        LOG_INFO("resizing FPP because PluMainScreen resized", NULL);
        dcaPrimary_->resizeProdPlanning();
    }

    QWidget::resizeEvent(event);
}
