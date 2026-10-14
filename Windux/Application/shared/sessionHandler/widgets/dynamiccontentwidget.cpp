/**
 * @file dynamiccontentwidget.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "dynamiccontentwidget.h"

#include "webViewer.h"

#include "../sessionmainwindow.h"
#include "../sessioninfo.h"

#include "../widgets/accumulateitems.h"
#include "../widgets/runtotalswidget.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>
#include <pushbutton.h>
#include <softkeyswidget.h>
#include <filesystem.h>

#include <ScaleConfiguration.h>
#include <ScaleClasses.h>

#include <QDir>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QPainter>

// Note:
// New Walmart requirements are for an FPP pane that uses the bottom half of the dynamic
// content widget. This will sit on top of the run screen flashkey buttons. If you need to
// switch back the full screen mode comment the below line.
#define INTEGRATED_FRESH_PRODUCTION 1

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
dynamicContentWidget::dynamicContentWidget( WidgetType widgets, QWidget *parent ) :
    _Mybase( parent ),
    buttonsVisible_( false ),
    propigateSignals_( false ),
    excludedContent_( (WidgetType) NULL ),
    prev_flashkey( NULL ),
    onlineOrderingShown_( false )
{
    setObjectName( "hobartDynamicContentWidget" );

    // initialize
    initialize();

    // add widgets to control
    createWidgets( widgets );

    // done!
    propigateSignals_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dynamicContentWidget::~dynamicContentWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int dynamicContentWidget::count() const
{
    return stackedWidget_->count();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dynamicContentWidget::WidgetType dynamicContentWidget::currentIndex() const
{
    return indexToWidget( stackedWidget_->currentIndex() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QWidget *dynamicContentWidget::currentWidget() const
{
    return stackedWidget_->currentWidget();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dynamicContentWidget::WidgetType dynamicContentWidget::excludedContent() const
{
    return excludedContent_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dynamicContentWidget::WidgetType dynamicContentWidget::indexOf( QWidget *w ) const
{
    return indexToWidget( stackedWidget_->indexOf( w ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dynamicContentWidget::isSpecialWidget( WidgetType t ) const
{
    for ( QStack<WidgetType>::const_iterator i = special_.begin(); i != special_.end(); ++i )
        if ( t == (*i) )
            return true;

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString dynamicContentWidget::operatorMessage() const
{
    if ( !operatorMessage_ )
        return QString();

    return operatorMessage_->text();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPixmap dynamicContentWidget::productImage() const
{
    // TODO
    return QPixmap();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::resetProductImage()
{
    // for now, set null pixmap; in future we may want static image
    setProductImage( QPixmap() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::setAccumulateItems( bool value )
{
    // enable/disable mode
    setSpecialWidgetMode( AccumulateItems, value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::setCuttingTest( bool value )
{
    // enable/disable mode depending on what cut test is being used

    sessionInfo info( this );
    SessionMainWindow *mainWindow = info();
    Session *session = (mainWindow ? mainWindow->session() : NULL );
    ScaleConfiguration *config = session->scaleConfig;

    int cuttesttype = config->scaleConfig.cuttingTestType();

    if( cuttesttype )
    {
        if( CutTestState::CutTestState_Begin == cuttesttype )
            setSpecialWidgetMode( CutTestFlashkeysDefault, value );
        else if( CutTestState::CutTestState_End == cuttesttype )
            setSpecialWidgetMode( CutTestFlashkeysOptionOne, value );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::setFlashkeys( const DomLoadSoftkeyGroup *value )
{
    if ( flashkeys_ )
    {
        flashkeys_->create( value );

#ifdef INTEGRATED_FRESH_PRODUCTION
        calcProdPlanningFrame();
#endif
    }

    if ( bestSellerList_ )
        bestSellerList_->create( value );

    if( fixedFlashkeys_ )
        fixedFlashkeys_->create( value );

    if ( tareFlashkeys_ )
        tareFlashkeys_->create( value );

    if ( cutTestFlashkeysDefault_ )
        cutTestFlashkeysDefault_->create( value );

    if ( cutTestFlashkeysOptionOne_ )
        cutTestFlashkeysOptionOne_->create( value );
}

void dynamicContentWidget::setToggledFlashkey(  const DomSoftkey *value )
{
    LOG_CLASS_INFO( "dynamicContentWidget", "setToggledFlashkey" );

    sessionInfo info( this );
    SessionMainWindow *mainWindow = info();
    Session *session = (mainWindow ? mainWindow->session() : NULL );
    ScaleConfiguration *config = session->scaleConfig;

    // (136,22) write softkey

    if ( fixedFlashkeys_ )
    {
        LOG_DEBUG( "try to update the toggled flashkey", NULL );

        hobart::softkeyButton *flashkey;
        QString filename = value->bgToggledImage();
        LOG_DEBUG( "flashkey filename to toggle %s", qPrintable(filename) );

        if( config->toggleExtraDayNow )
        {
            // if we are clicking on a toggable flashkey IN a a Fixed Flashkey grid but the flashkey info we have is from a CenterPanel, so need to add the Row/Col to it as offset
            int posX = getFixedToggledXCoordinate( config->toggledFixed );
            int posY = getFixedToggledYCoordinate( config->toggledFixed );

            config->toggledExtraDayImage = filename;              // in case we leave this feature, and come back, we need this saved off
            flashkey = fixedFlashkeys_->at( QPoint( posX, posY ) );
        }
        else
        {
            flashkey = fixedFlashkeys_->at( QPoint( value->hPosition(), value->vPosition() ) );
        }

        bool updated = false;

        if( config->toggleExtraDayNow && !config->toggleExtraDayON )
            updated = fixedFlashkeys_->resetPrimaryToggledBackground(value, flashkey );     // if we are a toggleable flashkey, in a fixed flashkey feature, and we are toggled on, then turn off
        else
            updated = fixedFlashkeys_->updateToggledBackground( value, flashkey );

        LOG_DEBUG( "flashkey toggled successfully %d", updated );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::setPrimaryToggledFlashkey( int featureNum, const DomSoftkey *value )
{
    sessionInfo info( this );
    SessionMainWindow *mainWindow = info();
    Session *session = (mainWindow ? mainWindow->session() : NULL );
    ScaleConfiguration *config = session->scaleConfig;

    // the following sets toggled backgrounds of flashkeys that are not fixed flashkeys.
    LOG_CLASS_INFO( "dynamicContentWidget", "setStandardToggledFlashkey" );

    // (136,22) write softkey
    LOG_DEBUG( "try to updated the center panel toggled flashkey", NULL );

        // find softkey
    hobart::softkeyButton *flashkey( flashkeys_->at( QPoint( value->hPosition(), value->vPosition() ) ) );
        bool updated = false;

        QString filename = value->bgToggledImage();
        LOG_DEBUG( "flashkey filename to toggle %s", qPrintable(filename) );

    if( ( featureNum == hobart::softkeysWidget::RunScreenButtons ) && config->toggleExtraDayON )
    {
        config->toggledExtraDayImage = value->bgToggledImage();
        updated = flashkeys_->updatePrimaryToggledBackground( value, flashkey );
    }
    else if( ( featureNum == hobart::softkeysWidget::RunScreenNumericKeypadButtons ) && config->toggleExtraDayNumPadON )
    {
        config->toggledExtraDayImageNumPad = value->bgToggledImage();
        updated = flashkeys_->updatePrimaryToggledBackground( value, flashkey );
    }
    else if( ( featureNum == hobart::softkeysWidget::ProductGrid ) && config->toggleExtraDayLeftON )
    {
        config->toggledExtraDayImageLeft = value->bgToggledImage();
        updated = flashkeys_->updatePrimaryToggledBackground( value, flashkey );
    }
    else
    {
        updated = flashkeys_->resetPrimaryToggledBackground( value, flashkey );
    }

    LOG_DEBUG( "center panel flashkey toggled successfully %d", updated );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::setFlashkeysFeature( int value )
{
    if ( flashkeys_ )
        flashkeys_->setCreateFilter( (softkeysWidget::GroupType) value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::setFreshProduction( bool value, const DomCurrentPLU *product, const MadpOperatorDisplayFields *fields, const QString& weight )
{
    LOG_CLASS_INFO( "dynamicContentWidget", "setFreshProduction" );

    if ( !fpp_ )
        return;

    static const DomCurrentPLU *productPrev( NULL );
    static QString urlPrev;

    // disable
    if ( !value )
    {
        productPrev = NULL;

#ifdef INTEGRATED_FRESH_PRODUCTION
        fpp_->hide();
        hideFpp_->hide();

        if ( onlineOrderingShown_ )
        {
            onlineOrderingProductUrl_.clear();
            onlineOrderingShown_ = false;
        }
#else
        setSpecialWidgetMode( FreshProduction, value );
#endif

    }

    // need to validate product information
    else if (( !product ) || ( !product->productionPlanningType() && onlineOrderingProductUrl_.isEmpty() ))
        LOG_DEBUG( "no product or fpp not enabled for this product", NULL );

    // need to validate operator display fields
    else if (( !fields ) || ( !fields->enableProdPlan() && onlineOrderingProductUrl_.isEmpty() ))
        LOG_DEBUG( "no fields or fpp not enabled for this operator", NULL );

    // enable
    else
    {
        QString url( prodPlanningUrl( product, fields, weight ) );

        // override FPP with online ordering
        if ( onlineOrderingProductUrl_.length() )
            url = onlineOrderingProductUrl_;

        if ( url.isEmpty() )
            LOG_DEBUG( "no fpp url", NULL );
        else
        {
            bool forceUrlChange( false );

            if ( product != productPrev )
            {
                productPrev = product;
                forceUrlChange = true;

#ifdef INTEGRATED_FRESH_PRODUCTION
                fpp_->setFocusOnNavigate( false );

                fpp_->show();
                hideFpp_->show();

                calcProdPlanningFrame();
#else
                setSpecialWidgetMode( FreshProduction, value );
#endif
            }

            if (( url != urlPrev ) || ( forceUrlChange ))
            {
                fpp_->setURL( urlPrev = url, true );

                if ( onlineOrderingProductUrl_.length() )
                    onlineOrderingShown_ = true;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::setOnlineOrderingProductUrl( const QString& url )
{
    if (( url.isEmpty() ) && ( onlineOrderingShown_ ))
        setFreshProduction( false );

    onlineOrderingProductUrl_ = url;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::setOperatorMessage( const QString& value )
{
    if ( operatorMessage_ )
        operatorMessage_->setText( value );

    operatorMessageCancelButton_->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::setOperatorMessage( const QString& title, const QString& value )
{
    if ( operatorMessageTitle_ )
        operatorMessageTitle_->setText( title );

    setOperatorMessage( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::setOperatorMessage( const QString& title, const QString& value, show_cancel_prod_entry_t )
{
    setOperatorMessage( title, value );

    // show the button
    operatorMessageCancelButton_->show();
    operatorMessageCancelButtonClicks_ = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::setProductImage( const QPixmap& value )
{
    // sanity checks
    if ( !productImageWidget_ )
        return;

    // target size
    const QSize targetSize( size() );

    // create pixmap
    QPixmap pixmap( targetSize );
    pixmap.fill( Qt::transparent );

    // handle source
    if ( !value.isNull() )
    {
        const QSize sourceSize( value.size() );

        // scale source
        QSize scaledSourceSize( sourceSize );
        scaledSourceSize.scale( targetSize, Qt::KeepAspectRatio );

        QRect rect(
            (targetSize.width() - scaledSourceSize.width()) / 2,
            (targetSize.height() - scaledSourceSize.height()) / 2,
            scaledSourceSize.width(),
            scaledSourceSize.height() );

        // create painter
        QPainter painter( &pixmap );
        painter.setRenderHint( QPainter::Antialiasing, true );

        // draw pixmap
        painter.drawPixmap( rect, value );
    }

    // set background
    modifyPaletteBackground setBackgroundPalette( pixmap );
    setBackgroundPalette( productImageWidget_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::setWidgetBackground( WidgetType index, const QBrush& brush )
{
    LOG_CLASS_INFO( "dynamicContentWidget", "setWidgetBackground" );

    QWidget *w;

    // set the palette background
    if ( (w = widget( index )) )
    {
        LOG_DEBUG( "setting background for widget at index %d", index );

        modifyPaletteBackground setBackground( brush );
        setBackground( w );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QWidget *dynamicContentWidget::widget( WidgetType t ) const
{
    LOG_CLASS_INFO( "dynamicContentWidget", "widget" );

    int index = widgetToIndex( t );

    LOG_TRACE( "looking for widget type 0x%08x at index %d...", t, index );

    if ( index < 0 )
    {
        LOG_WARN( "widget type 0x%08x does not exist", t );
        return NULL;
    }

    return stackedWidget_->widget( index );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::resetFlashkeys( bool force )
{
    // find out if I need to update the best sellers in self serv bc I just left flashkey editor
    sessionInfo info( this );
    SessionMainWindow *mainWindow = info();
    Session *session = (mainWindow ? mainWindow->session() : NULL );
    ScaleConfiguration *config = session->scaleConfig;

    bool fixedKeysActive( false );
    bool mostCommonKeysActive( false );
    int leftScreen( config->scaleConfig.dcaStartScreenType() );
    int centerScreen( config->scaleConfig.dcaRunScreenType() );
    if( ( hobart::dynamicContentWidget::FixedFlashkeys == leftScreen ) || (  hobart::dynamicContentWidget::FixedFlashkeys == centerScreen ) )
        fixedKeysActive = true;
    if( ( hobart::dynamicContentWidget::BestSellerList == leftScreen ) || (  hobart::dynamicContentWidget::BestSellerList == centerScreen ) )
        mostCommonKeysActive = true;


    if( config->internalConfig.ssactive_lock )
    {
        if ( bestSellerList_ && ( config->scaleConfig.bestSellersOnSelfServeMainScreen() == 1 ) )
            bestSellerList_->sendResetMessage( force );

        if ( fixedFlashkeys_ && ( config->scaleConfig.selfServeItemEntryScreenState() == 3 ) )
            fixedFlashkeys_->sendResetMessage( force );

        if ( tareFlashkeys_ && ( config->softkeyFeature.featureNum() == softkeysWidget::PresetTareGrid ) )
            tareFlashkeys_->sendResetMessage( force );

        if ( config->internalConfig.updateSelfServiceFlashkeys )
        {
            if ( ( SelfServicePrintOption::SelfServOption_LookupCategories == config->scaleConfig.selfServeItemEntryScreenState() ) ||
                 ( SelfServicePrintOption::SelfServOption_LookupCategoriesShowKeypad == config->scaleConfig.selfServeItemEntryScreenState() ) ||
                 ( SelfServicePrintOption::SelfServOption_SearchNone == config->scaleConfig.selfServeSearchScreenState() ) )

                session->send( SoftkeyDev, LoadSoftkeyGroup, QString::number( hobart::softkeysWidget::SelfservProductGrid ) );

            config->internalConfig.updateSelfServiceFlashkeys = false;
        }
    }
    else
    {
        if( config->internalConfig.isLeftPanelActivated )
        {
            if( QString::number( hobart::dynamicContentWidget::FixedFlashkeys ) == config->scaleConfig.dcaRunScreenTypeRaw()  )
            {
                // Going to the LEFT panel ONLY and toggling the default flashkey
                int fixedRowCol = config->softkeyFeature.fixedRowColMethod();
                config->toggledFixed = fixedRowCol;
                int hSize = config->toggledLastRow;
                int vSize = config->toggledLastCol;
                int row = 0;
                int col = 0;

                if( 1 == fixedRowCol )          // top row OK for (0,0,1)
                {
                    row = config->softkeyFeature.defaultButton() - 1;
                    col = 0;
                }
                else if( 2 == fixedRowCol )     // left col
                {
                    row = 0;
                    col = config->softkeyFeature.defaultButton() - 1;
                }
                else if( 3 == fixedRowCol )     // bottom row
                {
                    row = config->softkeyFeature.defaultButton() - 1;
                    col = vSize - 1;
                }
                else if( 4 == fixedRowCol)      // right col
                {
                    row = hSize - 1;
                    col = config->softkeyFeature.defaultButton() - 1;
                }

                config->toggledRow = row;
                config->toggledCol = col;

                session->send(
                   SoftkeyDev,
                   ReadSoftkey,
                   QString::number( hobart::softkeysWidget::FixedRowColumnGrid ) + "," +
                   QString::number( config->softkeyFeature.defaultGroupNum() ) + "," +
                   QString::number( row ) + "," +
                   QString::number( col ) );
            }

            if ( bestSellerList_ && mostCommonKeysActive )
                bestSellerList_->sendResetMessage( force );

            if ( fixedFlashkeys_ && fixedKeysActive )
                fixedFlashkeys_->sendResetMessage( force );

            return;
        }

        if ( flashkeys_ )
            flashkeys_->sendResetMessage( force );                  // 999003

        if ( bestSellerList_ && mostCommonKeysActive )
            bestSellerList_->sendResetMessage( force );             // 999007

        if ( fixedFlashkeys_ && fixedKeysActive )
            fixedFlashkeys_->sendResetMessage( force );             // 999020

        if ( tareFlashkeys_ && ( config->internalConfig.scaleBootComplete == false ) )
            tareFlashkeys_->sendResetMessage( force );              // 999004     // why would we ever need this? Do it at boot and forget it

        if( !config->internalConfig.ssactive_lock && ( config->scaleConfig.cuttingTestType() > 0 ) )
        {
            if ( cutTestFlashkeysDefault_ && ( config->scaleConfig.cuttingTestType() == 1 ) )
                cutTestFlashkeysDefault_->sendResetMessage( force );

            if ( cutTestFlashkeysOptionOne_  && ( config->scaleConfig.cuttingTestType() == 2 ) )
                cutTestFlashkeysOptionOne_->sendResetMessage( force );
        }

        //session()->scaleConfig->cuttingTestDialogActive = false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::translate()
{
    webViewer *wv;
    accumulateItemsWidget *aiw;
    runTotalsWidget *rt;

    // web viewer
    if ( (wv = dynamic_cast<webViewer*>( widget( WebBrowser ) )) )
        wv->translate();

    // accumulate items
    if ( (aiw = dynamic_cast<accumulateItemsWidget*>( widget( AccumulateItems ) )) )
        aiw->translate();

    // product image
    if ( productImage_ )
        productImage_->setText( tr( "Product\nImage" ) );

    // run totals
    if ( (rt = dynamic_cast<runTotalsWidget*>( widget( RunTotals ) )) )
        rt->translate();

    // fresh production
    if ( fpp_ )
        fpp_->translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::hideContentSelectors()
{
    LOG_CLASS_INFO( "dynamicContentWidget", "hideContentSelectors" );

    // nothing to do
    if ( !buttonsVisible_ )
        return;

    LOG_INFO( "hiding content selectors", NULL );
    prev_->hide();
    next_->hide();

    if ( productImage_ )
        productImage_->hide();

    buttonsVisible_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::hideSpecialWidget()
{
    LOG_CLASS_INFO( "dynamicContentWidget", "hideSpecialWidget" );

    if ( !isSpecialWidget() )
        LOG_WARN( "no special widget to hide", NULL );
    else
    {
        LOG_INFO( "hiding special widget %d (%d)", special_.top(), special_.size() );
        special_.pop();

        // show next special widget
        if ( special_.count() )
            setCurrentIndex( special_.top() );

        // restore original widget
        else
            setCurrentIndex( originalWidget_ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::setExcludedContent( WidgetType index )
{
    excludedContent_ = index;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::setCurrentIndex( WidgetType t )
{
    LOG_CLASS_INFO( "dynamicContentWidget", "setCurrentIndex" );

    // when special widget make sure we are only setting to special
    if (( isSpecialWidget() ) && ( t != special_.top() ))
    {
        LOG_WARN( "cannot set current index showing special widget!", NULL );
        return;
    }

    int index = widgetToIndex( t );

    LOG_TRACE( "setting current index to widget type 0x%08x at index %d...", t, index );

    if ( index < 0 )
    {
        LOG_WARN( "widget type 0x%08x does not exist", t );
        return;
    }

    stackedWidget_->setCurrentIndex( index );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::setCurrentWidget( QWidget *w )
{
    LOG_CLASS_INFO( "dynamicContentWidget", "setCurrentWidget" );

    // when special widget make sure we are only setting to special
    if (( isSpecialWidget() ) && ( w != stackedWidget_->widget( widgetToIndex( special_.top() ) ) ))
    {
        LOG_WARN( "cannot set current widget showing special widget!", NULL );
        return;
    }

    stackedWidget_->setCurrentWidget( w );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::showContentSelectors()
{
    LOG_CLASS_INFO( "dynamicContentWidget", "showContentSelectors" );

    if ( isSpecialWidget() )
    {
        LOG_WARN( "cannot show content selectors showing special widget", NULL );
        return;
    }
    else if ( isExcludedContentWidget() )
    {
        LOG_WARN( "cannot show content selectors showing excluded content", NULL );
        return;
    }

    LOG_INFO( "showing content selectors", NULL );
    prev_->show();
    next_->show();

    if ( productImage_ )
        productImage_->show();

    buttonsVisible_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::showSpecialWidget( WidgetType index )
{
    LOG_CLASS_INFO( "dynamicContentWidget", "showSpecialWidget" );

    if ( !widgetToIndex( index ) )
    {
        LOG_WARN( "cannot set special widget %d does not exist!", index );
        return;
    }

    // save off original widget
    if ( !special_.size() )
        originalWidget_ = currentIndex();

    LOG_INFO( "showing special widget %d (%d)", index, special_.size() );
    special_.push( index );

    // show widget
    setCurrentIndex( index );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::setSpecialWidgetMode( WidgetType t, bool value )
{
    LOG_CLASS_INFO( "dynamicContentWidget", "setSpecialWidgetMode" );

    // enable
    if ( value )
    {
        // cannot show mode when any other mode already shown!
        if (( isSpecialWidget( AccumulateItems ) ) || ( isSpecialWidget( CutTestFlashkeysDefault ) ) || ( isSpecialWidget( CutTestFlashkeysOptionOne ) ) || ( isSpecialWidget( FreshProduction ) ) )
            LOG_WARN( "cannot show mode %d; different mode already shown", t );
        else if ( isSpecialWidget() )
            special_.push_back( t );
        else
            showSpecialWidget( t );
    }

    // disable
    else if (( !value ) && ( isSpecialWidget( t ) ))
    {
        if ( 1 < special_.count() )
            special_.pop_back();
        else
            hideSpecialWidget();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dynamicContentWidget::addWidget( QWidget *w, WidgetType t )
{
    LOG_CLASS_INFO( "dynamicContentWidget", "addWidget" );

    // we already have a widget of this type
    if ( widgetToIndexMap_.contains( t ) )
    {
        LOG_WARN( "already have a widget of type 0x%08x", t );
        return false;
    }

    LOG_TRACE( "adding widget 0x%08x of type 0x%08x...", w, t );
    int index = stackedWidget_->addWidget( w );

    LOG_TRACE( "indexing...", NULL );
    widgetToIndexMap_[t] = index;
    indexToWidgetMap_[index] = t;

    LOG_INFO( "widget 0x%08x of type 0x%08x at index %d", w, t, index );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::removeWidget( QWidget *w )
{
    LOG_CLASS_INFO( "dynamicContentWidget", "removeWidget" );

    LOG_INFO( "removing widget 0x%08x...", w );
    stackedWidget_->removeWidget( w );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::resizeEvent( QResizeEvent *e )
{
    LOG_CLASS_INFO( "dynamicContentWidget", "resizeEvent" );

    _Mybase::resizeEvent( e );

    // move buttons to top right corner
    QPoint pos( rect().topRight() );
    next_->move( QPoint( pos.x() - next_->width(), 0 ) );
    prev_->move( QPoint( pos.x() - next_->width()- prev_->width(), 0 ) );

    // update background of operator widget
    if ( operatorMessageWidget_ )
    {
        LOG_DEBUG( "height %d width %d", height(), width() );

        const hobart::backgroundWidgetRenderer renderBackground(
            new hobart::rectangleBackgroundWidget(),
            new hobart::lightOperatorBackgroundWidgetRendererAppearance() );

        modifyPaletteBackground setBackgroundPalette( renderBackground.drawControl( size(), hobart::ReadOnly ) );
        setBackgroundPalette( operatorMessageWidget_ );
    }

#ifdef INTEGRATED_FRESH_PRODUCTION
    // resize fpp window
    calcProdPlanningFrame();
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::showEvent( QShowEvent *e )
{
    _Mybase::showEvent( e );

    prev_->setVisible( buttonsVisible_ );
    next_->setVisible( buttonsVisible_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::onButtonClicked( QWidget *w )
{
    LOG_CLASS_INFO( "dynamicContentWidget", "onButtonClicked" );

    int index = stackedWidget_->currentIndex();
    WidgetType t;

    LOG_DEBUG( "current index is %d", index );

    // grab previous or next widget from the stacked widget... if the widget is excluded from
    // content selection keep looking...
    do
    {
        if ( prev_ == w )
            --index;
        else if ( next_ == w )
            ++index;

        if ( index < 0 )
            index = (count() - 1);
        else if ( count() <= index )
            index = 0;

        t = indexToWidget( index );
        LOG_TRACE( "checking index %d widget type 0x%08x against filter 0x%08x...", index, t, excludedContent() );

    } while ( t & excludedContent() );

    LOG_DEBUG( "setting index to %d", index );
    stackedWidget_->setCurrentIndex( index );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::onCurrentChanged( int index )
{
    if ( !propigateSignals_ )
        return;

    Q_EMIT currentChanged( indexToWidget( index ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::onOperatorMessageCancelButtonClicked()
{
    // FT-1092
    // only do this once
    if ( 0 == operatorMessageCancelButtonClicks_++ )
    {
        sessionInfo info( this );
        SessionMainWindow *mainWindow = info();
        Session *session( mainWindow ? mainWindow->session() : NULL );

        // FT-1023
        if ( session )
            session->send( ProductDev, ProductStopPrintAllLabels );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::onWidgetRemoved( int index )
{
    LOG_CLASS_INFO( "dynamicContentWidget", "onWidgetRemoved" );

    // convert index to widget type
    WidgetType t = indexToWidget( index );

    // sanity check
    if ( t < 0 )
    {
        LOG_ERROR( "bad widget index %d type 0x%08x", index, t );
        return;
    }

    indexToWidgetMap_.remove( index );
    widgetToIndexMap_.remove( t );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::initialize()
{
    modifyPaletteBackground setBackgroundPalette( Qt::transparent );
    setBackgroundPalette( this );

    // setup widget
    setAutoFillBackground( true );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    // create layout
    QVBoxLayout *screenLayout = new QVBoxLayout( this );
    screenLayout->setContentsMargins( QMargins() );

    screenLayout->addWidget( stackedWidget_ = new QStackedWidget( this ) );
    stackedWidget_->setObjectName( "stackedWidget" );

    // create signal mapper
    buttons_ = new QSignalMapper( this );

    // create buttons
    prev_ = new QPushButton( "<", this );
    prev_->setFixedSize( QSize( 48, 48 ) ); // FT-1258 increased to 48 from 32
    prev_->setFocusPolicy( Qt::NoFocus );
    prev_->setFont( QApplication::font() );
    prev_->setObjectName( "prevButton" );
    prev_->setPalette( QApplication::palette() );
    prev_->setVisible( buttonsVisible_ );

    next_ = new QPushButton( ">", this );
    next_->setFixedSize( QSize( 48, 48 ) ); // FT-1258 increased to 48 from 32
    next_->setFocusPolicy( Qt::NoFocus );
    next_->setFont( QApplication::font() );
    next_->setObjectName( "nextButton" );
    next_->setPalette( QApplication::palette() );
    next_->setVisible( buttonsVisible_ );

    // product image
    productImageWidget_ = NULL;
    productImage_ = NULL;

    // run totals
    runTotals_ = NULL;

    // operator message
    operatorMessageWidget_ = NULL;
    operatorMessageTitle_ = NULL;
    operatorMessage_ = NULL;

    // flashkeys
    flashkeys_ = NULL;
    bestSellerList_ = NULL;
    fixedFlashkeys_ = NULL;
    tareFlashkeys_ = NULL;
    cutTestFlashkeysDefault_ = NULL;
    cutTestFlashkeysOptionOne_ = NULL;

    // fresh production
    fpp_ = NULL;
    hideFpp_ = NULL;

    // connect slots
    connect( stackedWidget_, SIGNAL( currentChanged(int) ), SLOT( onCurrentChanged(int) ) );
    connect( stackedWidget_, SIGNAL( widgetRemoved(int) ), SLOT( onWidgetRemoved(int) ) );
    connect( buttons_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    connect( prev_, SIGNAL( clicked() ), buttons_, SLOT( map() ) );
    buttons_->setMapping( prev_, prev_ );

    connect( next_, SIGNAL( clicked() ), buttons_, SLOT( map() ) );
    buttons_->setMapping( next_, next_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::createWidgets( WidgetType widgets )
{
    LOG_CLASS_INFO( "dynamicContentWidget", "createWidgets" );

    sessionInfo info( this );
    SessionMainWindow *mainWindow = info();

    ///////////////////////////////////////////////////////////////////////////
    // create widgets

    if ( Empty & widgets )
    {
        QWidget *w = new QWidget( stackedWidget_ );
        w->setAutoFillBackground( true );
        w->setObjectName( "empty" );

        LOG_DEBUG( "adding empty widget...", NULL );
        addWidget( w, Empty );
    }

    if ( NumberPad & widgets )
    {
        QWidget *w = new QWidget( stackedWidget_ );
        w->setAutoFillBackground( true );
        w->setObjectName( "numberPad" );

        LOG_DEBUG( "adding numberpad widget...", NULL );
        addWidget( w, NumberPad );
    }

    if ( Flashkeys & widgets )
    {
        flashkeys_ = new softkeysWidget( mainWindow->session(), stackedWidget_ );
        flashkeys_->setObjectName( "flashkeys" );

        connect( flashkeys_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), SIGNAL( flashkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

        LOG_DEBUG( "adding flashkeys widget...", NULL );
        addWidget( flashkeys_, Flashkeys );
    }

    if ( WebBrowser & widgets )
    {
        QWidget *w = new webViewer( stackedWidget_, false, false, mainWindow);
        w->setObjectName( "webBrowser" );
        w->setPalette( QApplication::palette() );

        LOG_DEBUG( "adding web browser widget...", NULL );
        addWidget( w, WebBrowser );
    }

    if ( ProductImage & widgets )
    {
        productImageWidget_ = new QWidget( stackedWidget_ );
        productImageWidget_->setObjectName( "productImage" );
        productImageWidget_->setAutoFillBackground( true );

        productImage_ = new QLabel( productImageWidget_ );
        productImage_->setAlignment( Qt::AlignCenter );
        productImage_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
        productImage_->setVisible( false );

        modifyFontPointSize setFontPointSize( (window()->height() * 18) / 600 );
        setFontPointSize( productImage_ );

        QVBoxLayout *layout = new QVBoxLayout( productImageWidget_ );
        layout->setContentsMargins( QMargins() );
        layout->addWidget( productImage_ );

        LOG_DEBUG( "adding product image widget...", NULL );
        addWidget( productImageWidget_, ProductImage );
    }

    if ( BestSellerList & widgets )
    {
        bestSellerList_ = new softkeysWidget( mainWindow->session(), stackedWidget_ );
        bestSellerList_->setCreateFilter( softkeysWidget::MostCommonlyUsedItems );
        bestSellerList_->setObjectName( "bestSellerList" );

        connect( bestSellerList_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), SIGNAL( flashkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

        LOG_DEBUG( "adding best sellers widget...", NULL );
        addWidget( bestSellerList_, BestSellerList );
    }


    ////////////////////////////////////////////////////////////////////////////////
    // pjr383
    if ( FixedFlashkeys & widgets )
    {
        fixedFlashkeys_ = new softkeysWidget( mainWindow->session(), stackedWidget_ );
        fixedFlashkeys_->setCreateFilter( softkeysWidget::FixedRowColumnGrid );
        fixedFlashkeys_->setObjectName( "fixedFlashkeys" );

        connect( fixedFlashkeys_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), SIGNAL( flashkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

        LOG_DEBUG( "adding fixed flashkeys widget...", NULL );
        addWidget( fixedFlashkeys_, FixedFlashkeys );
    }
    /////////////////////////////////////////////////////////////////////////////


    if ( RunTotals & widgets )
    {
        runTotals_ = new runTotalsWidget( stackedWidget_ );
        runTotals_->setAutoFillBackground( true );
        runTotals_->setObjectName( "runTotals" );

        LOG_DEBUG( "adding run totals widget...", NULL );
        addWidget( runTotals_, RunTotals );
    }

    if ( AccumulateItems & widgets )
    {
        QWidget *w = new accumulateItemsWidget( stackedWidget_ );
        w->setObjectName( "accumulateItems" );
        w->setPalette( QApplication::palette() );

        LOG_DEBUG( "adding accumulate items widget...", NULL );
        addWidget( w, AccumulateItems );
    }

    if ( FreshProduction & widgets )
    {
#ifdef INTEGRATED_FRESH_PRODUCTION
        fpp_ = new webViewer( flashkeys_, false, false, mainWindow );
        fpp_->setVisible( false );
#else
        fpp_ = new webViewer( stackedWidget_, false, false, mainWindow );
#endif
        fpp_->setDisplayAsDialog( false );
        fpp_->setObjectName( "freshProduction" );
        fpp_->setPalette( QApplication::palette() );
        fpp_->setToolBarVisible( false );
        fpp_->setUrlVisible( false, false );

#ifdef INTEGRATED_FRESH_PRODUCTION

        // FT-1258 increased to 48 from 32
        hobart::pushButton *hideFppButton( new hobart::pushButton( "X", flashkeys_, exitButtonPixmap( QSize( 48, 48 ) ) ) );

        QFont hideFppButtonFont( QApplication::font() );
        hideFppButtonFont.setPointSize( 16 );

        hideFpp_ = hideFppButton;
        hideFpp_->setFocusPolicy( Qt::NoFocus );
        hideFpp_->setFont( hideFppButtonFont );
        hideFpp_->setObjectName( "hideFpp" );
        hideFpp_->setVisible( false );

        connect( hideFpp_, SIGNAL(clicked()), fpp_, SLOT(hide()) );
        connect( hideFpp_, SIGNAL(clicked()), hideFpp_, SLOT(hide()) );
#endif

#ifndef INTEGRATED_FRESH_PRODUCTION
        LOG_DEBUG( "adding fresh production widget...", NULL );
        addWidget( fpp_, FreshProduction );
#endif
    }

    if ( OperatorMessage & widgets )
    {
        operatorMessageWidget_ = new QWidget( stackedWidget_ );
        operatorMessageWidget_->setObjectName( "operatorMessage" );
        operatorMessageWidget_->setAutoFillBackground( true );

        modifyPaletteForeground setForegroundPalette( modifyPaletteForeground::createPalette( hobart::colorPalette::gray ) );
        setForegroundPalette( operatorMessageWidget_ );

        operatorMessageTitle_ = new QLabel( operatorMessageWidget_ );
        operatorMessageTitle_->setAlignment( Qt::AlignCenter );
        operatorMessageTitle_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );

        operatorMessage_ = new QLabel( operatorMessageWidget_ );
        operatorMessage_->setAlignment( Qt::AlignCenter );
        operatorMessage_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );

        operatorMessageCancelButton_ = new QPushButton( operatorMessageWidget_ );
        operatorMessageCancelButton_->setFocusPolicy( Qt::NoFocus );
        operatorMessageCancelButton_->setText( tr( "Cancel" ) );

        connect( operatorMessageCancelButton_, SIGNAL( clicked() ), SLOT( onOperatorMessageCancelButtonClicked() ) );

        modifyFontPointSize setFontPointSize( (window()->height() * 18) / 600 );
        setFontPointSize( operatorMessageTitle_ );
        setFontPointSize( operatorMessage_ );
        setFontPointSize( operatorMessageCancelButton_ );

        QHBoxLayout *hlayout = new QHBoxLayout();
        hlayout->addStretch();
        hlayout->addWidget( operatorMessageCancelButton_ );

        QVBoxLayout *layout = new QVBoxLayout( operatorMessageWidget_ );
        layout->addWidget( operatorMessageTitle_ );
        layout->addStretch();
        layout->addWidget( operatorMessage_ );
        layout->addStretch();
        layout->addLayout( hlayout );

        LOG_DEBUG( "adding operator message widget...", NULL );
        addWidget( operatorMessageWidget_, OperatorMessage );
    }

    if ( TareFlashkeys & widgets )
    {
        tareFlashkeys_ = new softkeysWidget( mainWindow->session(), stackedWidget_ );
        tareFlashkeys_->setCreateFilter( softkeysWidget::PresetTareGrid );
        tareFlashkeys_->setObjectName( "tareFlashkeys" );

        connect( tareFlashkeys_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), SIGNAL( flashkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

        LOG_DEBUG( "adding tare flashkeys widget...", NULL );
        addWidget( tareFlashkeys_, TareFlashkeys );
    }

    if ( CutTestFlashkeysDefault & widgets )
    {
        cutTestFlashkeysDefault_ = new softkeysWidget( mainWindow->session(), stackedWidget_ );
        cutTestFlashkeysDefault_->setCreateFilter( softkeysWidget::RunScreenCutTestOn );
        cutTestFlashkeysDefault_->setObjectName( "cutTestFlashkeys" );

        connect( cutTestFlashkeysDefault_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), SIGNAL( flashkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

        LOG_DEBUG( "adding cut test flashkeys widget...", NULL );
        addWidget( cutTestFlashkeysDefault_, CutTestFlashkeysDefault );
    }

    if ( CutTestFlashkeysOptionOne & widgets )
    {
        cutTestFlashkeysOptionOne_ = new softkeysWidget( mainWindow->session(), stackedWidget_ );
        cutTestFlashkeysOptionOne_->setCreateFilter( softkeysWidget::RunScreenCutTestOnOptionOne );
        cutTestFlashkeysOptionOne_->setObjectName( "cutTestFlashkeys" );

        connect( cutTestFlashkeysOptionOne_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), SIGNAL( flashkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

        LOG_DEBUG( "adding cut test flashkeys widget...", NULL );
        addWidget( cutTestFlashkeysOptionOne_, CutTestFlashkeysOptionOne );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int dynamicContentWidget::widgetToIndex( WidgetType t ) const
{
    LOG_CLASS_INFO( "dynamicContentWidget", "widgetToIndex" );

    LOG_TRACE( "looking up widget type 0x%08x", t );

    // sanity check
    if ( !widgetToIndexMap_.contains( t ) )
    {
        LOG_ERROR( "missing key 0x%08x", t );
        return -1;
    }

    return widgetToIndexMap_[t];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dynamicContentWidget::WidgetType dynamicContentWidget::indexToWidget( int index ) const
{
    LOG_CLASS_INFO( "dynamicContentWidget", "indexToWidget" );

    LOG_TRACE( "looking up index %d", index );

    // sanity check
    if ( !indexToWidgetMap_.contains( index ) )
    {
        LOG_ERROR( "missing key %d", index );
        return (dynamicContentWidget::WidgetType) -1;
    }

    return indexToWidgetMap_[index];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString dynamicContentWidget::prodPlanningUrl( const DomCurrentPLU *product, const MadpOperatorDisplayFields *fields, const QString &weight ) const
{
    sessionInfo info( this );

    SessionMainWindow *mainWindow = info();
    Session *session = (mainWindow ? mainWindow->session() : NULL );

    DomStoreInformation *storeInformation( &session->scaleConfig->storeInfo );

    QString sourceURL( fields->prodPlanningFrame().trimmed() );
    QString destURL;

    int index = sourceURL.indexOf( "?" );

    QString argument;
    bool firstArgument = true;

    if ( -1 == index )
        return sourceURL;

    destURL = sourceURL.mid( 0, index );
    sourceURL = sourceURL.mid( index + 1, sourceURL.length() );

    do
    {
        index = sourceURL.indexOf( "&" );

        if ( -1 == index )
            argument = sourceURL;
        else
        {
            argument = sourceURL.mid( 0, index );
            sourceURL = sourceURL.mid( index + 1, sourceURL.length() );
        }

        QString tag;
        int index2 = argument.indexOf( "=" );

        if ( -1 == index2 )
            tag = argument;
        else
            tag = argument.mid( 0, index2 );

        if ( tag.length() )
        {
            QString argument;

            if ( session->scaleConfig->scaleConfig.httpQueryStore() == tag )
                argument = tag + "=" + storeInformation->storeID();
            else if ( session->scaleConfig->scaleConfig.httpQueryDept() == tag )
                argument = tag + "=" + storeInformation->deptNum();
            else if ( session->scaleConfig->scaleConfig.httpQueryModel() == tag )
                argument = tag + "=Hobart_HTi";
            else if ( session->scaleConfig->scaleConfig.httpQueryPLU() == tag )
                argument = tag + "=" + const_cast<DomCurrentPLU*>( product )->itemNumber();
            else if ( session->scaleConfig->scaleConfig.httpQueryLanguage() == tag )
                argument = tag + "=" + session->scaleConfig->getFPPLanguage();
            else if( session->scaleConfig->scaleConfig.httpQueryQuantity() == tag)
                argument = tag + "=" + const_cast<DomCurrentPLU*>( product )->byCountRaw();
            else if( session->scaleConfig->scaleConfig.httpQueryWeight() == tag)
                argument = tag + "=" + weight;      // Don't know why, but when I was using the MadpUpdateCurrentPLUTotals here, the data was incorrect/junk data usually blank

            if ( argument.length() )
            {
                if ( !firstArgument )
                    destURL += "&";
                else
                {
                    firstArgument = false;
                    destURL += "?";
                }

                destURL += argument;
            }
        }

    } while ( -1 != index );

    return destURL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicContentWidget::calcProdPlanningFrame()
{
#ifdef INTEGRATED_FRESH_PRODUCTION

    // integrated fpp widget is on bottom half of parent
    QRect rect( flashkeys_->geometry() );
    rect.setTop( rect.height() / 2 );

    if ( fpp_ )
    {
        fpp_->setGeometry( rect );
        fpp_->raise();
    }

    // close button is in top right cornert
    if ( hideFpp_ )
    {
        hideFpp_->move( rect.right() - hideFpp_->width(), rect.top() );
        hideFpp_->raise();
    }
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPixmap dynamicContentWidget::exitButtonPixmap( const QSize &size )
{
    QPixmap pixmap( size );
    pixmap.fill( Qt::transparent );

    QPainter p( &pixmap );
    p.setRenderHint( QPainter::Antialiasing, true );

    p.setBrush( hobart::colorPalette::red );
    p.setPen( hobart::colorPalette::red );

    QRectF rect( QPoint( 0, 0 ), size );
    rect.adjust( 0.5, 0.5, -0.5, -0.5 );

    p.drawRoundedRect( rect, 3, 3 );

    return pixmap;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int dynamicContentWidget::getFixedToggledXCoordinate( int fixedRowOrCol )
{
    sessionInfo info( this );
    SessionMainWindow *mainWindow = info();
    Session *session = (mainWindow ? mainWindow->session() : NULL );
    ScaleConfiguration *config = session->scaleConfig;

    int row = 0;                                            // Rows are VERTICAL/UP+DOWN Locations
    int posX  = config->toggleExtraDayRow;                  // Row

    if( config->toggleExtraDayNow )
    {
        if( 2 == fixedRowOrCol )                            // left col
            row = posX + 1;
        else                                                // top row, right col, and bottom row all don't shift
            row = posX;
    }

    return row;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int dynamicContentWidget::getFixedToggledYCoordinate(int fixedRowOrCol)
{
    sessionInfo info( this );
    SessionMainWindow *mainWindow = info();
    Session *session = (mainWindow ? mainWindow->session() : NULL );
    ScaleConfiguration *config = session->scaleConfig;

    int col = 0;                                            // Rows are HORIZONTAL/LEFT+RIGHT Locations
    int posY  = config->toggleExtraDayCol;                  // Col

    if( config->toggleExtraDayNow )
    {
        if( 1 == fixedRowOrCol )                            // top row
            col = posY + 1;
        else                                                // left col, right col, and bottom row don't shift
            col = posY;
    }

    return col;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
