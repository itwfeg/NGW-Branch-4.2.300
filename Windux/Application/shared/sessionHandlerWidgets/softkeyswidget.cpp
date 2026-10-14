//#define USE_QDEBUG
/**
 * @file softkeyswidget.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "softkeyswidget.h"
#include "softkeybutton.h"

#include "backgroundwidgettypes.h"
#include "../sessionHandler/widgets/dynamiccontentwidget.h"

#include <palette.h>
#include <font.h>

#include <Messages.h>
#include <Session.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <ScaleClasses.h>
#include <../sessionHandlerHooks/selfservicehook.h>

#include <QMessageBox>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QHBoxLayout>
#include <QMap>
#include <QMimeData>
#include <QPainter>
#include <QSignalMapper>
#include <QVBoxLayout>
#include <QFileInfo>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeysWidget::softkeysWidget( Session *s, QWidget *parent ) :
    _Mybase( parent ),
    factory_( new softkeyButtonFactory() ),
    session_( s ),
    filter_( (GroupType) 0 ),
    clicks_( 0 ),
    pages_( 0 ),
    editMode_( false ),
    keysPending_( false ),
    isBlinking_(false),
    fixedKeysRefreshed_( true ),
    scaleConfig_( NULL ),
    internalConfigData_( NULL )
{
    setObjectName( "hobartSoftkeysWidget" );

    // enable dropping
    setAcceptDrops( true );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeysWidget::~softkeysWidget()
{
    if ( previous_ )
    {
        delete previous_;
        previous_ = NULL;
    }

    if (scaleConfig_ )
    {
        scaleConfig_ = NULL;
    }

    if ( factory_ )
    {
        delete factory_;
        factory_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeyButton *softkeysWidget::at( const QPoint& gridPos ) const
{
    softkeyButtonList buttonList( buttons() );

    foreach ( softkeyButton *button, buttonList )
        if ( gridPos == button->gridPos() )
            return button;

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool softkeysWidget::canExpandSoftkeyHorz( const softkeyButton *button, ActionCode& reason ) const
{
    QPoint buttonGridPos( button->gridPos() );
    QSize buttonGridSize( button->gridSize() );

    buttonGridSize.rwidth() += 1;

    // check grid size
    if ( gridSize().width() < (buttonGridPos.x() + buttonGridSize.width()) )
    {
        reason = GridTooSmall;
        return false;
    }

    // look for conflicting softkey
    if ( overlapsExistingButton( button, buttonGridPos, buttonGridSize ) )
    {
        reason = ContainsSoftkey;
        return false;
    }

    reason = Okay;
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool softkeysWidget::canExpandSoftkeyVert( const softkeyButton *button, ActionCode& reason ) const
{
    QPoint buttonGridPos( button->gridPos() );
    QSize buttonGridSize( button->gridSize() );

    buttonGridSize.rheight() += 1;

    // check grid size
    if ( gridSize().height() < (buttonGridPos.y() + buttonGridSize.height()) )
    {
        reason = GridTooSmall;
        return false;
    }

    // look for conflicting softkey
    if ( overlapsExistingButton( button, buttonGridPos, buttonGridSize ) )
    {
        reason = ContainsSoftkey;
        return false;
    }

    reason = Okay;
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool softkeysWidget::canRemoveColumn( ActionCode& reason ) const
{
    softkeyButtonList buttonList( buttons() );
    int width = gridSize().width();

    // check for sofktey
    foreach ( softkeyButton *button, buttonList )
        if ( button->gridPos().x() == (width - 1) )
        {
            reason = ContainsSoftkey;
            return false;
        }

    // check for spanned softkey
    foreach ( softkeyButton *button, buttonList )
        if ( width <= (button->gridPos().x() + button->gridSize().width()) )
        {
            reason = ContainsSpannedSoftkey;
            return false;
        }

    reason = Okay;
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool softkeysWidget::canRemoveRow( ActionCode& reason ) const
{
    softkeyButtonList buttonList( buttons() );
    int height = gridSize().height();

    // check for sofktey
    foreach ( softkeyButton *button, buttonList )
        if ( button->gridPos().y() == (height - 1) )
        {
            reason = ContainsSoftkey;
            return false;
        }

    // check for spanned softkey
    foreach ( softkeyButton *button, buttonList )
        if ( height <= (button->gridPos().y() + button->gridSize().height()) )
        {
            reason = ContainsSpannedSoftkey;
            return false;
        }

    reason = Okay;
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPoint softkeysWidget::gridPosAt( const QPoint& pos ) const
{
    for ( int w = 0; w < gridSize().width(); ++w )
        for ( int h = 0; h < gridSize().height(); ++h )
        {
            QRect rect( calcButtonGeometry( QPoint( w, h ), QSize( 1, 1 ) ) );

            if (( rect.left() <= pos.x() ) && ( pos.x() < rect.right() ) &&
                ( rect.top() <= pos.y() ) && ( pos.y() < rect.bottom() ))
                return QPoint( w, h );
        }

    return QPoint( -1, -1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeyButton *softkeysWidget::selected() const
{
    softkeyButtonList buttonList( buttons() );

    foreach ( softkeyButton *button, buttonList )
        if ( button->isSelected() )
            return button;

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::setEditMode( bool value )
{
    softkeyButtonList buttonList( buttons() );

    // nothing to do
    if ( editMode() == value )
        return;

    editMode_ = value;

    // set button draggable
    foreach ( softkeyButton *button, buttonList )
    {
        button->setDraggable( editMode_ );
        //disables flashkey overlays on flashkey editor screen
        button->disableOverlay(true);
    }
    // update
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::setGridSize( const QSize& value )
{
    // nothing to do
    if ( gridSize() == value )
        return;

    gridSize_ = value;

    // resize buttons
    resizeButtons();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::setHSpacing( int value )
{
    // nothing to do
    if ( hSpacing() == value )
        return;

    hSpacing_ = value;

    // resize buttons
    resizeButtons();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::setTextAlignment( int value )
{
    softkeyButtonList buttonList( buttons() );
    Qt::Alignment a( Qt::AlignCenter );

    // nothing to do
    if ( textAlignment() == value )
        return;

    // invalid value
    else if (( value < 0 ) || ( 3 <= value ))
        return;

    textAlignment_ = value;

    if ( 0 == textAlignment() ) // align top
        a = (Qt::AlignHCenter | Qt::AlignTop);
    else if ( 2 == textAlignment() ) // align bottom
        a = (Qt::AlignHCenter | Qt::AlignBottom);

    foreach ( softkeyButton *button, buttonList )
        button->setTextAlignment( a );

    // refresh
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString softkeysWidget::setTextToFit( QString key_text )
{
    if( 0 == numOfDescChars_ )                                      // a zero here means that we display entire description
        return key_text;

    QString new_key_text = key_text;                                // at least return the original

    if( key_text.length() > numOfDescChars_ )
        new_key_text = key_text.left( numOfDescChars_ );

    return new_key_text;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::setFactory( softkeyButtonFactory *value )
{
    if (( value ) && ( factory_ != value ))
    {
        delete factory_;
        factory_ = value;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::setVSpacing( int value )
{
    // nothing to do
    if ( vSpacing() == value )
        return;

    vSpacing_ = value;

    // resize buttons
    resizeButtons();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::setFRowColMethod( int value )
{
    // nothing to do
    if ( fRowColMethod() == value )
        return;

    fixedRowColMethod_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::setFRowColDefButton( int value )
{
    // nothing to do
    if ( fRowColDefButton() == value )
        return;

    fixedRowColDefButton_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool softkeysWidget::updateToggledBackground( const DomSoftkey* m, hobart::softkeyButton *flashkey )
{
    if( !m )
        return false;

    if( flashkey )
        flashkey->setBackgroundImage( m->bgToggledImage() );
    else
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool softkeysWidget::updatePrimaryToggledBackground( const DomSoftkey* m, hobart::softkeyButton *flashkey )
{
    if( !m )
        return false;

    if( flashkey )
    {
        flashkey->setBackgroundImage( m->bgToggledImage() );
        //flashkey->setText( m->name2() );		// just a test to see if I could toggle text
    }
    else
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool softkeysWidget::resetPrimaryToggledBackground( const DomSoftkey* m, hobart::softkeyButton *flashkey )
{
    if( !m )
        return false;

    if( flashkey )
    {
        flashkey->setBackgroundImage( m->bgImage() );
        //flashkey->setText( m->name1() );			// just a test to see if I could toggle text
    }
    else
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool softkeysWidget::create( const DomLoadSoftkeyGroup* m )
{
    LOG_CLASS_INFO( "softkeysWidget", "create" );

    // sanity check
    if ( !m )
        return false;

    DomLoadSoftkeyGroup* cm = const_cast<DomLoadSoftkeyGroup*>( m );
    int count;

    quickFeatureNum_ = m->featureNum();
    if (( scaleConfig_->scaleConfig.enableUseCharacterSearching() ) && ( SelfservCategoryResultsButtons == feature_ ))
    {
        scaleConfig_->charSearchSoftkeyCount = 0;

        if( quickFeatureNum_ == ProductGrid || quickFeatureNum_ == RunScreenButtons || quickFeatureNum_ == PresetTareGrid || quickFeatureNum_ == MostCommonlyUsedItems || quickFeatureNum_ == SelfservCategoryButtons || quickFeatureNum_ == RunScreenNumericKeypadButtons )
            return false;
    }

    // check for filter
    if (( createFilter() ) && ( createFilter() != ((GroupType) m->featureNum()) ))
    {
        return false;
    }
    else if( FixedRowColumnGrid == quickFeatureNum_ )
    {
         LOG_DEBUG( "quickFeatureNum_ = 999020 and filter = %d", createFilter() );
    }
    else if( FreshServeFixedButtons == quickFeatureNum_ )
    {
         LOG_DEBUG( "quickFeatureNum_ = 888050 and filter = %d", createFilter() );
    }
    else if( SelfservAZCategoryButtons == quickFeatureNum_ )
    {
         LOG_DEBUG( "quickFeatureNum_ = 999016 and filter = %d", createFilter() );
    }
    else if ( (*previous_) == (*m) )
    {
        // If Character by character searching, same softkeys, nothing to do - unless we are changing language
        if( ( FixedRowColumnGrid == quickFeatureNum_ ) && ( internalConfigData_->isLeftPanelActivated ) && ( fixedKeysRefreshed_ ) )
        {
            LOG_DEBUG( "DO Nothing, just wanted to see if left active and 999020", NULL );
            fixedKeysRefreshed_ = false;
        }
        else if ( scaleConfig_->changeLanguage == 0 )
            return false;
    }
    // save off softkeys
    (*previous_) = (*m);

    // free existing softkeys
    freeSoftkeyButtons();

    // save group info
    feature_ = (GroupType) m->featureNum();
    group_ = (GroupType) m->groupNum();

    // find out how many characters are allowed on a flashkey and if we are allowed to limit it
    numOfDescChars_ = operatorDisplayFields_->numberOfCharacters();

    // save off grid info
    gridSize_ = QSize( m->horizGridSize(), m->vertGridSize() );
    if( scaleConfig_->gen2UIActive )
    {
        hSpacing_ = 10;
        vSpacing_ = 10;                     // ALWAYS 10
        textAlignment_ = Qt::AlignBottom;   // ALWAYS bottom
    }
    else
    {
        hSpacing_ = m->horizSpacing();
        vSpacing_ = m->vertSpacing();
        textAlignment_ = m->textVertAlign();
    }

    LOG_DEBUG( "group %d grid size (%d, %d) spacing (%d, %d) count %d",
        m->groupNum(),
        gridSize_.width(),
        gridSize_.height(),
        hSpacing_,
        vSpacing_,
        m->numOfSoftkeys() );

    // increment page counter
    ++pages_;

    // process buttons
    if ( (count = m->numOfSoftkeys()) )
    {
        // retrieve button info
        integerList hPosList( m->field<integerList>( cm->hPosList() ) );
        integerList vPosList( m->field<integerList>( cm->vPosList() ) );
        integerList hSizeList( m->field<integerList>( cm->hSizeList() ) );
        integerList vSizeList( m->field<integerList>( cm->vSizeList() ) );
        QStringList bgColorList( m->field<QStringList>( cm->bgColorList() ) );
        QStringList bgImageList( m->field<QStringList>( cm->bgImageList() ) );
        QStringList nameList( m->field<QStringList>( cm->nameList() ) );
        integerList actionList( m->field<integerList>( cm->actionList() ) );
        QStringList actionParamList( m->field<QStringList>( cm->actionParamList() ) );
        integerList actionFeatureNumList( m->field<integerList>( cm->actionFeatureNumList() ) );
        integerList isEnabledList( m->field<integerList>( cm->isEnabledList() ) );
        QStringList fgImageList( m->field<QStringList>( cm->fgImageList() ) );
        QStringList textColorList( m->field<QStringList>( cm->textColorList() ) );
        QStringList scriptNameList( m->field<QStringList>( cm->scriptNameList() ) );
        integerList scriptIntervalList( m->field<integerList>( cm->scriptIntervalList() ) );
        
        /////////////////////////////////////////////////////////

        // compute count of buttons
        count = qMin( count, hPosList.count() );
        count = qMin( count, vPosList.count() );
        count = qMin( count, hSizeList.count() );
        count = qMin( count, vSizeList.count() );
        count = qMin( count, bgColorList.count() );
        count = qMin( count, bgImageList.count() );
        count = qMin( count, nameList.count() );
        count = qMin( count, actionList.count() );
        count = qMin( count, actionParamList.count() );
        count = qMin( count, actionFeatureNumList.count() );
        count = qMin( count, isEnabledList.count() );
        count = qMin( count, fgImageList.count() );
        count = qMin( count, textColorList.count() );
        count = qMin( count, scriptNameList.count() );
        count = qMin( count, scriptIntervalList.count() );


        if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && ( SelfservCategoryResultsButtons == feature_) )
        {
            scaleConfig_->charSearchSoftkeyCount = count;
            int hs = m->horizGridSize();
            int vs = m->vertGridSize();
            gridSize_ = QSize( hs, vs );
        }
        else if( ( scaleConfig_->scaleConfig.enableUseCharacterSearching() ) && ( SelfservCategoryResultsButtons == feature_) )
        {
            scaleConfig_->charSearchSoftkeyCount = count;
            // pjr383 - we could use the horizGridSize and vertGridSize here. It might make more sense actually since we can now SIZE the categories now
            gridSize_ = QSize( 6, 3 );
        }
        
        if ( m->numOfSoftkeys() != count )
            LOG_WARN( "mismatch in button count %d %d", m->numOfSoftkeys(), count );

        // create softkeys

        QFont desc_font;
        desc_font.setFamily( "Poppins" );
        desc_font.setPointSize( 48 );           // no impact

        while ( count-- )
        {
            QPoint gridPos( hPosList[count], vPosList[count] );
            QSize gridSize( hSizeList[count], vSizeList[count] );

           // JHC - TEST TEST TEST
            if (( 0 <= gridPos.x() ) && ( gridPos.x() < gridSize_.width() ) && ( gridPos.x() + gridSize.width() <= gridSize_.width() ) &&
                ( 0 <= gridPos.y() ) && ( gridPos.y() < gridSize_.height() ) && ( gridPos.y() + gridSize.height() <= gridSize_.height() ))
            {
                modifyPaletteForeground setForeground(
                    modifyPaletteForeground::createPalette(
                        QColor( makeColor( textColorList[count] ) ),
                        QColor( makeColor( cm->disabledTextColor() ) ) ) );

                softkeyButton *button = createSoftkeyButton();

                // delete this test on the next line - does not do anything as far as I can tell
                QFont font = button->property("font").value<QFont>();// ui->pushButton->property("font").value<QFont>();

                if (button)
                {
                    button->setObjectName( QString( "softkeyButton_%0_%1_%2" ).arg( objectName() ).arg( gridPos.x() ).arg( gridPos.y() ) );

                    int theaction = actionList[count];
                    button->setAction( actionList[count] );
                    button->setGroupNum( group_ );
                    button->setActionFeature( actionFeatureNumList[count] );
                    button->setActionParam( actionParamList[count] );
                    button->setBackgroundColor( makeColor( bgColorList[count] ) );

                    int posX = getFixedToggledXCoordinate( scaleConfig_->toggledFixed );
                    int posY = getFixedToggledYCoordinate( scaleConfig_->toggledFixed );

                    if( scaleConfig_->gen2UIActive )
                    {
                        button->setGen2ActiveUI( scaleConfig_->gen2UIActive );
                        button->setBackgroundImage( bgImageList[count]);
                    }
                    else
                    {
                        if( theaction == hobart::softkeysWidget::Action_ToggleExtraShelfLifeDay )
                            scaleConfig_->extraShelfLifeDayAvail = true;

                        int gridPosX = gridPos.x();
                        int gridPosY = gridPos.y();

                        // anything done here is also affecting operate mode
                    
                        button->setGen2ActiveUI( false );

                        if( (quickFeatureNum_ == FixedRowColumnGrid ) && scaleConfig_->showToggled && scaleConfig_->toggledState && ( gridPosX == scaleConfig_->toggledRow ) && ( gridPosY == scaleConfig_->toggledCol ) )
                        {
                            DomSoftkey *clickedSoftkey( &scaleConfig_->softkeyItem );
                            button->setBackgroundImage( clickedSoftkey->bgToggledImage() );
                        }
                        else if ( scaleConfig_->toggleExtraDayON || scaleConfig_->toggleExtraDayNumPadON || scaleConfig_->toggleExtraDayLeftON )
                        {
                            if ( ( hobart::softkeysWidget::RunScreenButtons == quickFeatureNum_ ) && ( gridPos.x() == scaleConfig_->toggleExtraDayRow ) && ( gridPos.y() == scaleConfig_->toggleExtraDayCol ) )
                            {
                                // completely redrawing the flashkey set on the screen, display the toggled one if it is toggled
                                QString toggled_image( scaleConfig_->toggledExtraDayImage );

                                if ( toggled_image.length() )
                                    button->setBackgroundImage( toggled_image );
                            }
                            else if( ( hobart::softkeysWidget::RunScreenNumericKeypadButtons == quickFeatureNum_ ) && ( gridPos.x() == scaleConfig_->toggleExtraDayRowNumPad ) && ( gridPos.y() == scaleConfig_->toggleExtraDayColNumPad ) )
                            {
                                // completely redrawing the flashkey set on the screen, display the toggled one if it is toggled
                                QString toggled_image( scaleConfig_->toggledExtraDayImageNumPad );

                                if ( toggled_image.length() )
                                    button->setBackgroundImage( toggled_image );

                            }
                            else if( hobart::softkeysWidget::ProductGrid == quickFeatureNum_ && ( gridPos.x() == scaleConfig_->toggleExtraDayRowLeft ) && ( gridPos.y() == scaleConfig_->toggleExtraDayColLeft ) )
                            {
                                // completely redrawing the flashkey set on the screen, display the toggled one if it is toggled
                                QString toggled_image( scaleConfig_->toggledExtraDayImageLeft );

                                if ( toggled_image.length() )
                                    button->setBackgroundImage( toggled_image );
                            }
                            else if ( ( hobart::softkeysWidget::FixedRowColumnGrid == quickFeatureNum_ ) && ( hobart::softkeysWidget::Action_ToggleExtraShelfLifeDay == theaction ) && ( posX == gridPos.x() ) && ( posY == gridPos.y() ) )
                            {
                                // completely redrawing the flashkey set on the screen, display the toggled one if it is toggled
                                QString toggled_image( scaleConfig_->toggledExtraDayImage );

                                if ( toggled_image.length() )
                                    button->setBackgroundImage( toggled_image );
                            }
                            else
                                button->setBackgroundImage( bgImageList[count]);
                            desc_font.setPointSize(27);
                        }
                        else
                        {
                            button->setBackgroundImage( bgImageList[count]);
                            desc_font.setPointSize(27);
                        }
                    }

                    button->setForegroundImage( fgImageList[count] );
                    // apparently this is not looked at in HLX, always enabled
                    //button->setEnabled( isEnabledList[count] ? true : false );
                    button->setGridPos( gridPos );
                    button->setGridSize( gridSize );
                    button->setFont( desc_font );                           // appears to have no impact on FS
                    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
                        button->setText( setTextToFit( nameList[count] ).toUpper() );
                    else
                        button->setText( setTextToFit( nameList[count] ) );
                    setForeground( button );
                    button->setScript( scriptNameList[count] );
                    button->setScriptInterval( scriptIntervalList[count] );

                    LOG_DEBUG( "have button '%s' location (%d, %d, %d, %d)",
                        qPrintable( button->text() ),
                        button->gridPos().x(),
                        button->gridPos().y(),
                        button->gridSize().width(),
                        button->gridSize().height() );

                    button->show();
                }
            }
        }

        // resize buttons
        resizeButtons();
    }

    // refresh widget
    update();

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeyButton *softkeysWidget::findButton( int action ) const
{
    softkeyButtonList buttonList( buttons() );

    foreach ( softkeyButton *button, buttonList )
        if ( action == button->action() )
            return button;

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::resizeButtons()
{
    softkeyButtonList buttonList( buttons() );

    foreach ( softkeyButton *button, buttonList )
    {
        QRect rect( calcButtonGeometry( button->gridPos(), button->gridSize() ) );

        // avoid Qt complaining about invalid rects
        if (( rect.right() < rect.left() ) || ( rect.bottom() < rect.top() ))
            continue;

        button->setFixedSize( rect.size() );
        button->move( rect.topLeft() );
    }

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::sendResetMessage( bool force )
{
//#ifdef USE_QDEBUG
//    qDebug() << "softkeysWidget::sendResetMessage(" << force << ")";
//#endif
    LOG_CLASS_INFO( "softkeysWidget", "sendResetMessage" );

    // sanity check
    if ( !session_ )
        return;

    if( scaleConfig_->internalConfig.ssactive_lock )
    {
        if( scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() == 1 && scaleConfig_->scaleConfig.selfServeDisplayBarcodeLabel() == 1)
            return;
    }

    const bool isLeftPaneled( internalConfigData_->isLeftPanelActivated );
    bool mostCommonKeysActive( false );
    int leftScreen( scaleConfig_->scaleConfig.dcaStartScreenType() );
    int centerScreen( scaleConfig_->scaleConfig.dcaRunScreenType() );
    if( ( hobart::dynamicContentWidget::BestSellerList == leftScreen ) || (  hobart::dynamicContentWidget::BestSellerList == centerScreen ) )
        mostCommonKeysActive = true;

    // only request when filter installed
    if ( !createFilter() )
        LOG_TRACE( "not reseting since filter not installed", NULL );
    else if ( ( force ) || ( ( 0 < clicks_ ) && ( 1 < pages_ ) ) || ( isLeftPaneled ) )
    {
        // reset tracking vars
        clicks_ = 0;
        pages_ = 0;

        LOG_DEBUG( "requesting feature %d", createFilter() );

        if( FixedRowColumnGrid  == createFilter() && isLeftPaneled )
        {
            // (136,40)
            session_->send( SoftkeyDev, ReadSoftkeyFeature, QString::number( FixedRowColumnGrid ) );

            // ( 136,93,999020) -> moved up top - ooops that didnt work - this worked for the center screen
            if( !scaleConfig_->internalConfig.ssactive_lock )
                session_->send( SoftkeyDev, LoadSoftkeyGroup, QString::number( FixedRowColumnGrid ) );

            return;
        }

        // special case for most commonly used items (best seller list)
        if ( MostCommonlyUsedItems  == createFilter() )
        {
            const bool isScaleBooted( internalConfigData_->scaleBootComplete );

            // I think this is only used at boot up time.
            if (( isScaleBooted ) || ( !keysPending_ ))
            {
                // These were being requested 6 times at boot so I decided to stop that.
                // Got it down to 2 requests due to 2 instances of this class I believe
                keysPending_ = true;
                if( mostCommonKeysActive )
                    session_->send( SoftkeyDev, ExecuteSoftkey, "42,," + QString::number( MostCommonlyUsedItems ) );        // 1X
            }
        }
        else if( FixedRowColumnGrid  == createFilter() )
        {
            // (136,40)
            session_->send( SoftkeyDev, ReadSoftkeyFeature, QString::number( FixedRowColumnGrid ) );                    // 1X

            // ( 136,93,999020) -> moved up top - this works for the center screen
            //if( !scaleConfig_->internalConfig.ssactive_lock )
            session_->send( SoftkeyDev, LoadSoftkeyGroup, QString::number( FixedRowColumnGrid ) );                      // 1X
        }
        else if ( SelfservProductGrid == createFilter() )
        {
           // Not being called for some reason even though I set this filter up instead of MCUI. But MCUI still being called
            session_->send( SoftkeyDev, LoadSoftkeyGroup, QString::number( SelfservProductGrid ) );
        }
        else
        {
            if( ( RunScreenCutTestOn  == createFilter() ) || ( RunScreenCutTestOnOptionOne  == createFilter() ) )
            {
                if( scaleConfig_->internalConfig.ssactive_lock && ( scaleConfig_->scaleConfig.cuttingTestType() == 0 ) )
                    return;
                else
                    session_->send( SoftkeyDev, LoadSoftkeyGroup, QString::number( createFilter() ) );
            }
            else if( RunScreenEnhancedCenterScreen  == createFilter() )
            {
                if( scaleConfig_->scaleConfig.enableEnhancedCenterScreen() )
                    session_->send( SoftkeyDev, LoadSoftkeyGroup, QString::number( createFilter() ) );
            }
            else
                session_->send( SoftkeyDev, LoadSoftkeyGroup, QString::number( createFilter() ) );
        }

    }
//#ifdef USE_QDEBUG
//    qDebug() << "softkeysWidget::sendResetMessage(" << force << "):  Done.";
//#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::editSelection()
{
    softkeyButtonList buttonList( buttons() );

    if ( !editMode() )
        return;

    softkeyButton *editButton( selected() );

    // hide all buttons except the one we are editing
    foreach ( softkeyButton *button, buttonList )
    {
        bool isVisible( editButton == button );

        button->setVisible( isVisible );
        button->setBlinkWhenSelected( !isVisible );
        button->setDraggable( false );
    }

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::resetSelection()
{
    softkeyButtonList buttonList( buttons() );

    foreach ( softkeyButton *button, buttonList )
    {
        button->setSelected( false );
        button->setBlinkWhenSelected( editMode() );
        button->setDraggable( editMode() );

        button->setVisible( true );
    }

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::resetSelfServTareFlashkey()
{
    // dont have to be in edit mode to blink
    if(!isBlinking_)
        return;

    if( ( scaleConfig_->scaleConfig.selfServeDisplayTareOnRandomWeight() ) && ( PresetTareGrid == feature_ ) && ( scaleConfig_->internalConfig.ssactive_lock ) )
    {
        softkeyButtonList buttonList( buttons() );

        foreach ( softkeyButton *button, buttonList )
            button->setBlinkWhenSelected( false );

        isBlinking_ = false;
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::setSelfServTareFlashkey( softkeyButton *value )
{
    // dont have to be in edit mode to

    if( ( scaleConfig_->scaleConfig.selfServeDisplayTareOnRandomWeight() ) && ( PresetTareGrid == feature_ ) && ( scaleConfig_->internalConfig.ssactive_lock ) )
    {
        resetSelfServTareFlashkey();

        value->setSelected( true );
        value->setBlinkWhenSelected( true );
        isBlinking_ = true;
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::resetToggled()
{
    softkeyButtonList buttonList( buttons() );

    foreach ( softkeyButton *button, buttonList )
        button->setToggled( false );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::setSelected( softkeyButton *value )
{
    softkeyButtonList buttonList( buttons() );

    // cannot select when not in edit mode
    if ( !editMode() )
        return;

    // only selected widget is visible
    foreach ( softkeyButton *button, buttonList )
        button->setSelected( (value == button) );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::setToggled( softkeyButton *value )
{
    softkeyButtonList buttonList( buttons() );

    // only selected widget is visible
    foreach ( softkeyButton *button, buttonList )
        button->setToggled( (value == button) );

    update();
}



///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
Qt::DropAction softkeysWidget::setDropAction( T *e )
{
    // use copy action by default
    Qt::DropAction action = Qt::CopyAction;

    // when dragging from grid widget we are moving, not copying!
    QWidget *source( dynamic_cast<QWidget*>( e->source() ) );
    int parents = 2;

    while (( source ) && ( 0 <= parents-- ))
    {
        if ( !dynamic_cast<_Myt*>( source ) )
            source = source->parentWidget();
        else
        {
            action = Qt::MoveAction;
            break;
        }
    }

    e->setDropAction( action );
    return action;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::dragEnterEvent( QDragEnterEvent *e )
{
    LOG_CLASS_INFO( "softkeysWidget", "dragEnterEvent" );

    // sanity check
    if ( !e )
        return;

    LOG_TRACE( "object dragged into widget 0x%08x", e->source() );

    // cannot drop when not in edit mode
    if ( !editMode() )
    {
        LOG_INFO( "rejecting, not in edit mode", NULL );

        e->ignore();
        return;
    }

    // validate mime format
    if ( e->mimeData()->hasFormat( softkeyButton::mimeDataType() ) )
    {
        LOG_DEBUG( "valid object 0x%08x", e->source() );

        e->acceptProposedAction();
        return;
    }

    _Mybase::dragEnterEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::dragMoveEvent( QDragMoveEvent *e )
{
    LOG_CLASS_INFO( "softkeysWidget", "dragMoveEvent" );

    // sanity check
    if ( !e )
        return;

    // validate mime format
    if ( e->mimeData()->hasFormat( softkeyButton::mimeDataType() ) )
    {
        QPoint gridPos( gridPosAt( e->pos() ) );

        // cannot perform action on invalid point
        // cannot perform action where something exists already
        if (( gridPos.x() < 0 ) || ( gridSize().width() <= gridPos.x() ) ||
            ( gridPos.y() < 0 ) || ( gridSize().height() <= gridPos.y() ) ||
            ( at( gridPos ) ))
        {
            LOG_DEBUG( "bad drag location %d %d", gridPos.x(), gridPos.y() );

            e->ignore();
            return;
        }

        // set drop action
        Qt::DropAction action = setDropAction( e );

        LOG_TRACE( "have '%s' action", ((Qt::MoveAction == action) ? "move" : "copy") );

        // there are two cases:
        // 1) we are creating a brand new softkey
        // 2) we are moving an existing softkey

        // create new softkey
        if ( Qt::CopyAction == action )
        {
            // check for overlap
            if ( overlapsExistingButton( NULL, gridPos, QSize( 1, 1 ) ) )
            {
                LOG_DEBUG( "bad location %d %d for new softkey", gridPos.x(), gridPos.y() );

                e->ignore();
                return;
            }
        }

        // move existing softkey
        else if ( Qt::MoveAction == action )
        {
            QByteArray a( e->mimeData()->data( softkeyButton::mimeDataType() ) );

            softkeyButton b;
            b.deserialize( a );

            softkeyButton *existing( at( b.gridPos() ) );

            // cannot move something if overlap exists
            if (( !existing ) || ( overlapsExistingButton( existing, gridPos, existing->gridSize() ) ))
            {
                LOG_DEBUG( "bad location %d %d for softkey 0x%08x", gridPos.x(), gridPos.y(), existing );

                e->ignore();
                return;
            }

            // cannot move something if larger than grid
            if (( gridSize().width() <= (gridPos.x() + existing->gridSize().width() - 1) ) ||
                ( gridSize().height() <= (gridPos.y() + existing->gridSize().height() - 1) ))
            {
                LOG_DEBUG( "grid size %d %d to small for softkey %d %d %d %d",
                    gridSize().width(), gridSize().height(),
                    gridPos.x(), gridPos.y(),
                    existing->gridSize().width(), existing->gridSize().height() );

                e->ignore();
                return;
            }
        }

        // accept
        e->accept();
        return;
    }

    _Mybase::dragMoveEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::dropEvent( QDropEvent *e )
{
    LOG_CLASS_INFO( "softkeysWidget", "dropEvent" );

    // sanity check
    if ( !e )
        return;

    LOG_INFO( "handle object drop 0x%08x", e->source() );

    // validate mime format
    if ( e->mimeData()->hasFormat( softkeyButton::mimeDataType() ) )
    {
        QByteArray a( e->mimeData()->data( softkeyButton::mimeDataType() ) );
        QPoint gridPos( gridPosAt( e->pos() ) );

        // cannot perform action on invalid point
        if (( gridPos.x() < 0 ) || ( gridSize().width() <= gridPos.x() ) ||
            ( gridPos.y() < 0 ) || ( gridSize().height() <= gridPos.y() ))
        {
            LOG_DEBUG( "bad drag location %d %d", gridPos.x(), gridPos.y() );

            e->ignore();
            return;
        }

        softkeyButton b;
        b.deserialize( a );

        // set drop action
        Qt::DropAction action = setDropAction( e );

        LOG_TRACE( "have '%s' action", ((Qt::MoveAction == action) ? "move" : "copy") );

        // there are two cases:
        // 1) we are creating a brand new softkey
        // 2) we are moving an existing softkey

        // create new softkey
        if ( Qt::CopyAction == action )
        {
            LOG_INFO( "create new cell at %d %d of type %d", gridPos.x(), gridPos.y(), b.action() );
            softkeyButton *cell = createSoftkeyButton();
            cell->setObjectName( QString( "softkeyButton_%0_%1_%2" ).arg( objectName() ).arg( gridPos.x() ).arg( gridPos.y() ) );
            cell->setGridPos( gridPos );
            cell->setGridSize( b.gridSize() );

            resizeButtons();

            Q_EMIT createNewCell( this, gridPos, b.action() );
        }

        // move existing softkey
        else if ( Qt::MoveAction == action )
        {
            softkeyButton *existing( at( b.gridPos() ) );

            // sanity check
            if (( !existing ) || ( existing != e->source() ))
            {
                LOG_WARN( "bad move cell 0x%08x", existing );

                e->ignore();
                return;
            }

            LOG_INFO( "moving cell from %d %d to %d %d", b.gridPos().x(), b.gridPos().y(), gridPos.x(), gridPos.y() );
            softkeyButton *cell = createSoftkeyButton();
            cell->deserialize( a );
            cell->setObjectName( QString( "softkeyButton_%0_%1_%2" ).arg( objectName() ).arg( gridPos.x() ).arg( gridPos.y() ) );
            cell->setGridPos( gridPos );

            resizeButtons();

            Q_EMIT moveCell( this, existing, b.gridPos(), gridPos );
        }

        else
        {
            LOG_WARN( "bad drop action %d", e->dropAction() );

            e->ignore();
            return;
        }

        LOG_DEBUG( "done", NULL );
        e->accept();

        return;
    }

    _Mybase::dropEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::paintEvent( QPaintEvent *e )
{
//#ifdef USE_QDEBUG
//    qDebug() << "softkeysWidget::paintEvent()";
//#endif
    // call base method
    _Mybase::paintEvent( e );

    // only something to do when editing
    if ( !editMode() )
        return;

    softkeyButton *button( selected() );

    // nothing to do when in button edit mode
    if (( button ) && ( !button->blinkWhenSelected() ))
        return;

    QPainter p( this );
    p.setRenderHint( QPainter::Antialiasing, true );

    // draw empty cells
    renderEmptyButtons( &p );
//#ifdef USE_QDEBUG
//    qDebug() << "softkeysWidget::paintEvent():  Done.";
//#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::resizeEvent( QResizeEvent *e )
{
    LOG_CLASS_INFO( "softkeysWidget", "resizeEvent" );

    LOG_DEBUG( "group %d size is now (%d, %d)", group(), width(), height() );
    _Mybase::resizeEvent( e );

    // resize buttons
    resizeButtons();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::onSoftkeyButtonClicked( QWidget *w )
{
    if ( w )
    {
        softkeyButton *button( dynamic_cast<softkeyButton*>( w ) );

        if ( button )
        {
            // increment click counter
            ++clicks_;

            // needs refreshed again since we clicked a key
            fixedKeysRefreshed_ = true;

            // emit signal
#ifdef USE_QDEBUG
    qDebug() << "softkeysWidget::onSoftkeyButtonClicked():  Emitting clicked signal:  action = " << button->action() << ",  actionParam = "  << button->actionParam() << ",  actionFeature = " << button->actionFeature();
#endif
            // CALLS selfServiceHook::emitSoftkeyClicked()
            // EMITS  softkeyClicked
            // CALLS OperateMode::handleFlashkey
//	        LOG_DEBUG( "Emitting clicked signal:  action = %d,  actionParam = %s,  actionFeature = %d", button->action(), qPrintable( button->actionParam() ), button->actionFeature() );
            Q_EMIT clicked( this, button, button->action(), button->actionParam(), button->actionFeature() );

            setSelfServTareFlashkey( button );
        }
    }
#ifdef USE_QDEBUG
    qDebug() << "softkeysWidget::onSoftkeyButtonClicked():  Done.";
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::initialize()
{
#ifdef USE_QDEBUG
    qDebug() << "softkeysWidget::initialize()";
#endif
    // setup widget
    setAutoFillBackground( true );
    setContentsMargins( QMargins( 0, 0, 0, 0 ) );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    modifyPaletteBackground setBackgroundPalette( Qt::transparent );
    setBackgroundPalette( this );

    // signal mappers
    signalMapper_ = new QSignalMapper( this );
    connect( signalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onSoftkeyButtonClicked(QWidget*) ) );

    // get the scale configuration values
    scaleConfig_ = session_->scaleConfig;
    internalConfigData_ = &session_->scaleConfig->internalConfig;

    // create softkey group message
    previous_ = new DomLoadSoftkeyGroup;

    operatorDisplayFields_ = &session_->scaleConfig->operatorDisplayFields;
    numOfDescChars_ = 0;
#ifdef USE_QDEBUG
    qDebug() << "softkeysWidget::initialize():  Done.";
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeysWidget::softkeyButtonList softkeysWidget::buttons() const
{
#ifdef USE_QDEBUG
    qDebug() << "softkeysWidget::buttons():  Done.";
#endif
    return findChildren<softkeyButton*>();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool softkeysWidget::overlapsExistingButton( const softkeyButton *button, const QPoint& pos, const QSize& size ) const
{
    softkeyButtonList buttonList( buttons() );

    // look for conflicting softkey
    for ( int bx = pos.x(); bx < (pos.x() + size.width()); ++bx )
        for ( int by = pos.y(); by < (pos.y() + size.height()); ++by )
            foreach ( softkeyButton *other, buttonList )
                if ( button != other )
                    for ( int x = other->gridPos().x(); x < (other->gridPos().x() + other->gridSize().width()); ++x )
                        for ( int y = other->gridPos().y(); y < (other->gridPos().y() + other->gridSize().height()); ++y )
                            if (( bx == x ) && ( by == y ))
                                return true;

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QRect softkeysWidget::calcButtonGeometry( const QPoint& pos, const QSize& size ) const
{
    int searchResultsHeight = 172;     // Gen1 - to it's intended size (yes spacing is 20)
    int searchResultsWidth  = 180;     // includes built in spacing  so it takes it down

	QPoint topLeft( 0,0 );
    QPoint bottomRight( 0,0 );

    if( scaleConfig_->gen2UIActive )
    {
        topLeft = calcPoint( pos );
        bottomRight.rx() = topLeft.x() + searchResultsWidth;
        bottomRight.ry() = topLeft.y() + searchResultsHeight;
    }
    else
    {
		// default
        //QPoint bottomRight( calcPoint( QPoint( pos.x() + size.width(), pos.y() + size.height() ) ) );
        topLeft = calcPoint( pos );
        bottomRight = calcPoint( QPoint( pos.x() + size.width(), pos.y() + size.height() ) ) ;
        bottomRight.rx() -= (hSpacing_ + 1);
        bottomRight.ry() -= (vSpacing_ + 1);
    }

    return QRect( topLeft, bottomRight );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPoint softkeysWidget::calcPoint( const QPoint& pos ) const
{
    int wTotal = 0;
    int hTotal = 0;

    if( scaleConfig_->gen2UIActive )
    {
        wTotal =  width();
        hTotal =  height();
    }
    else
    {
        wTotal =  width() - contentsMargins().left() - contentsMargins().right() + hSpacing_;
        hTotal =  height() - contentsMargins().top() - contentsMargins().bottom() + hSpacing_;
    }

    // calculate point
    int grid_width = gridSize_.width();
    // JHC TEST TEST TEST
    
    QPoint p(
        (wTotal * pos.x()) / grid_width,
        (hTotal * pos.y()) / gridSize_.height() );

    // offset by contents margins
    p.rx() += contentsMargins().left();
    p.ry() += contentsMargins().top();

    return p;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeyButton *softkeysWidget::createSoftkeyButton()
{
#ifdef USE_QDEBUG
    qDebug() << "softkeysWidget::createSoftkeyButton()";
#endif

    Qt::Alignment textAlignment( Qt::AlignCenter );

    // compute alignment
    // FS refresh 2022
    if( scaleConfig_->internalConfig.ssactive_lock )
        textAlignment = (Qt::AlignHCenter | Qt::AlignBottom);
    else if ( 0 == textAlignment_ ) // align top
        textAlignment = (Qt::AlignHCenter | Qt::AlignTop);
    else if ( 2 == textAlignment_ ) // align bottom
        textAlignment = (Qt::AlignHCenter | Qt::AlignBottom);

    // create button
    softkeyButton *button = factory_->create( this );
    if (button)
    {
        button->setDraggable( editMode() );
        button->setTextAlignment( textAlignment );

        // connect slots
        signalMapper_->setMapping( button, button );
        connect( button, SIGNAL( clicked() ), signalMapper_, SLOT( map() ) );

#ifdef USE_QDEBUG
        qDebug() << "softkeysWidget::createSoftkeyButton():  Done.";
#endif
    }
    return button;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::freeSoftkeyButtons()
{
    softkeyButtonList buttonList( buttons() );

    // delete buttons
    foreach ( softkeyButton *button, buttonList )
        factory_->destroy( button );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeysWidget::renderEmptyButtons( QPainter *p )
{
    softkeyButtonList buttonList( buttons() );

    typedef QPair<int,int> cellPos;
    typedef QMap<cellPos, bool> cellInUseMap;

    cellInUseMap cells;
    int x, y;

    // make list of cells
    for ( x = 0; x < gridSize().width(); ++x )
        for ( y = 0; y < gridSize().height(); ++y )
            cells.insert( cellPos( x, y ), false );

    // mark used cell as used
    foreach ( softkeyButton *button, buttonList )
        for ( x = 0; x < button->gridSize().width(); ++x )
            for ( y = 0; y < button->gridSize().height(); ++y )
                cells[ cellPos( button->gridPos().x() + x, button->gridPos().y() + y ) ] = true;

    // draw blank area for each unused cell
    for ( cellInUseMap::iterator i = cells.begin(); i != cells.end(); ++i )
        if ( !i.value() )
        {
            // used by flashkey editor
            QRect rect( calcButtonGeometry( QPoint( i.key().first, i.key().second ), QSize( 1, 1 ) ) );

            p->fillRect( rect, QBrush( hobart::colorPalette::white ) );
            p->fillRect( rect, QBrush( hobart::colorPalette::gray, Qt::Dense4Pattern ) );
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QColor softkeysWidget::makeColor( const QString& c )
{
    // hexadecimal string (i.e. 80FF80)
    if (( 6 <= c.length() ) && ( c.length() <= 7 ))
    {
        if ( "#" != c.left( 1 ) )
            return QColor( "#" + c );

        return QColor( c );
    }

    return QColor();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int softkeysWidget::getFixedToggledXCoordinate( int fixedRowOrCol )
{
    int row = 0;                                            // Rows are VERTICAL/UP+DOWN Locations
    int posX  = scaleConfig_->toggleExtraDayRow;            // Row

    if( scaleConfig_->toggleExtraDayON )
    {
        if( 2 == fixedRowOrCol )                            // left col
            row = posX + 1;
        else                                                // top row, right col, and bottom row all don't shift
            row = posX;
    }

    return row;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int softkeysWidget::getFixedToggledYCoordinate(int fixedRowOrCol)
{
    int col = 0;                                            // Rows are HORIZONTAL/LEFT+RIGHT Locations
    int posY  = scaleConfig_->toggleExtraDayCol;                  // Col

    if( scaleConfig_->toggleExtraDayON )
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
