/**
 * @file pluinfoscreen.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>
#include <sessionhooks.h>

#include "appsscreen.h"
#include "buildamealscreen.h"
#include "meatspecsheetscreen.h"
#include "pluinfoscreen.h"
#include "plumainscreen.h"

#include "../sessionmainwindow.h"

#include "../dialogs/graphicdialog.h"

#include "../widgets/labeleddoublelineedit.h"
#include "../widgets/labeleddoublenumericlineedit.h"
#include "../widgets/labeleddoublenumericlineeditarray.h"
#include "../widgets/labeledgraphicedit.h"
#include "../widgets/labeledgraphiceditarray.h"
#include "../widgets/labeledlabelviewer.h"
#include "../widgets/labeledlineedit.h"
#include "../widgets/labelednumericlineedit.h"
#include "../widgets/labeledplaintextedit.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <filesystem.h>
#include <graphic.h>

#include <Session.h>
#include <Messages.h>
#include <ScaleClasses.h>
#include <UiUtilities.h>
#include <ScaleConfiguration.h>

#include <QDir>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QSignalMapper>
#include <QVBoxLayout>

const bool PluInfoScreen::convertUppercase_ = true;

const int PluInfoScreen::nutrifactsLabelId_ = 10080;
const int PluInfoScreen::nutrifactsLabelIdZoomed_ = 10081;

const int PluInfoScreen::promptUser_ = 9;

static const int BUILD_A_MEAL_SCREEN = 1;
static const int CUT_TEST_ONE_SCREEN = 2;

///////////////////////////////////////////////////////////////////////////////////////////////////
PluInfoScreen::PluInfoScreen( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    scaleIsMetric_( false ),
    signalPluLableOnce_( false )
{
    operateMode_ = dynamic_cast<OperateMode*>( mode_ );

    setObjectName( "PluInfoScreen" );

    // initialize screen
    initialize();
    createLayout();
    translate();

    clearFields();

    // connect slots
    connect( mode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ) );

    if ( operateMode_ )
    {
        connect( operateMode_, SIGNAL( clearPluForCutTest() ),              SLOT( onClearPluForCutTest() ) );
        connect( operateMode_, SIGNAL( currentProductChanged(bool) ),       SLOT( onCurrentProductChanged(bool) ) );
        connect( operateMode_, SIGNAL( currentProductSupportChanged(int) ), SLOT( onCurrentProductSupportChanged(int) ) );
        connect( operateMode_, SIGNAL( currentProductUpdateField(DomUpdateRunModeField) ), SLOT( onCurrentProductUpdateField(DomUpdateRunModeField) ) );
    }

    connect( mainWindow_, SIGNAL( numberPadMoved(Qt::DockWidgetArea) ), SLOT( onNumberPadMoved(Qt::DockWidgetArea) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PluInfoScreen::~PluInfoScreen()
{
    if(printerPrompter_ != NULL)
        delete printerPrompter_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::resetLabelData()
{
    // request nutrifacts label types
    session_->send( LabelTypeDev, LabelTypeDtRead, QString::number( nutrifactsLabelId_ ) + ",1" );
    session_->send( LabelTypeDev, LabelTypeDtRead, QString::number( nutrifactsLabelIdZoomed_ ) + ",1" );

    // request graphics
    session_->send( GraphicDev, GraphicReadAllNames );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::updatePrinterData( const SessionMessage& msg )
{
    DomPrWriteAllExternalPrinter m( msg );

    QString tmpNumList    = m.extPrinterNumList();          // list of printer scale numbers
    QString tmpNameList   =  m.extPrinterNamesList();       // list of printer names
    QString tmpTypeList   = m.extPrinterTypeList();         // list of printer types
    QString tmpDriverList = m.extPrinterDriverList();       // list of printer drivers (this orig field as become something else)

    printerNumsList    = tmpNumList.split(",");
    printerNamesList   = tmpNameList.split(",");
    printerTypesList   = tmpTypeList.split(",");
    printerDriversList = tmpDriverList.split(",");
}

void PluInfoScreen::updatePrinterPromptData( const SessionMessage& msg )
{
    DomPrWriteAllProductTextPrintParams m( msg );

    QString     approvedStrOfTexts  = "1,5,6,9,20";
    QStringList approvedListOfTexts = approvedStrOfTexts.split(",");

    QString names    = m.extPrinterDataNamesNumList();     // field enum offset num
    QString prompts  = m.extPrinterDataLabelsPrompt();     // user prompt action
    QString labels   = m.extPrinterDataLabelNumList();     // label ID numbers
    int numOfPrompts = m.extPrinterDataNamesNumCount();

    QStringList namesListStr  = names.split(",");
    QStringList promptListStr = prompts.split(",");
    QStringList lblNumListStr = labels.split(",");

    QStringList namesList;
    QStringList promptList;
    QStringList lblNumList;
    QString tmpStr = "";

    if( numOfPrompts == 0 )
        return;

    for(int j = 0; j < numOfPrompts; j++)
    {
        tmpStr = namesListStr.at(j);
        if ( approvedListOfTexts.indexOf(tmpStr) >= 0 )
        {
            namesList.append(  namesListStr.at(j)  );
            promptList.append( promptListStr.at(j) );
            lblNumList.append( lblNumListStr.at(j) );
        }
    }

    prDataNamesMap[namesList.at(0).toInt()] = promptList.at(0);                        // 1
    prDataNamesMap[namesList.at(1).toInt()] = promptList.at(1);                        // 5
    prDataNamesMap[namesList.at(2).toInt()] = promptList.at(2);                        // 6
    prDataNamesMap[namesList.at(3).toInt()] = promptList.at(3);                        // 9
    prDataNamesMap[namesList.at(4).toInt()] = promptList.at(4);                        // 20
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::requestPrinterStockMetrics()
{
    const MadpPeripheralList *peripheralList( &session_->scaleConfig->peripheralList );

    // clear existing metrics and field values
    labelStockMetrics_.clear();
    labelStock_->clear();

    // request label stock metrics
    if ( peripheralList->firstPrinter() )
        session_->send( Printer, PrReqStockMetrics, "0" );
    if ( peripheralList->secondPrinter() )
        session_->send( Printer, PrReqStockMetrics, "1" );
    if ( peripheralList->thirdPrinter() )
        session_->send( Printer, PrReqStockMetrics, "2" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::translate()
{
    // label text
    QString shelfLifeText = tr( "Shelf Life (Hours)" );
    QString productLifeText = tr( "Product Life (Hours)" );
    QString labelStockText = tr( "Label Stock" );
    QString primaryLabelIdText = tr( "Primary Label Id" );
    QString primaryGraphicIdText = tr( "Primary Graphic Id" );

    QString nutritionText = tr( "Nutrition\nFacts Panel" );

    QString expandedTextText = tr( "Expanded Text" );
    QString recipeText = tr( "Recipe" );
    QString cookingInstructionsText = tr( "Cooking Instructions" );
    QString allergensText = tr( "Allergens" );

    QString additionalLabelIdText = tr( "Additional Label Id" );
    QString additionalGraphicIdText = tr( "Additional Graphic Id" );

    // convert to upper
    if ( convertUppercase_ )
    {
        shelfLifeText = shelfLifeText.toUpper();
        productLifeText = productLifeText.toUpper();
        labelStockText = labelStockText.toUpper();
        primaryLabelIdText = primaryLabelIdText.toUpper();
        primaryGraphicIdText = primaryGraphicIdText.toUpper();

        nutritionText = nutritionText.toUpper();

        expandedTextText = expandedTextText.toUpper();
        recipeText = recipeText.toUpper();
        cookingInstructionsText = cookingInstructionsText.toUpper();
        allergensText = allergensText.toUpper();

        additionalLabelIdText = additionalLabelIdText.toUpper();
        additionalGraphicIdText = additionalGraphicIdText.toUpper();
    }

    // set label text
    shelfLife_->setLabelText( shelfLifeText );
    productLife_->setLabelText( productLifeText );
    labelStock_->setLabelText( labelStockText );
    primaryLabelId_->setLabelText( primaryLabelIdText );
    primaryGraphicId_->setLabelText( primaryGraphicIdText );

    nutrition_->setLabelText( nutritionText );

    expandedText_->setLabelText( expandedTextText );
    recipe_->setLabelText( recipeText );
    cookingInstructions_->setLabelText( cookingInstructionsText );
    allergens_->setLabelText( allergensText );

    additionalLabelId_->setLabelText( additionalLabelIdText );
    additionalGraphicId_->setLabelText( additionalGraphicIdText );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::setScaleMetric( bool value )
{
    // check for label stock change
    if ( scaleIsMetric_ != value )
    {
        scaleIsMetric_ = value;

        // request label stock metrics
        requestPrinterStockMetrics();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget *PluInfoScreen::swipeScreenDown() const
{
    // no apps installed
    if ( !AppsScreen::isExternalAppInstalled() )
        return NULL;

    // screen does not exist
    else if ( !operateMode_->isCached<AppsScreen>() )
        return NULL;

    return mode_->createScreen<AppsScreen>();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget *PluInfoScreen::swipeScreenRight() const
{
    // check what screen is the right screen...
    // 0 = PluInfoScreen
    // 1 = WrapperSettingsScreen
    // 2 = LabelPlacementScreen
    int rightScreen( session_->scaleConfig->scaleConfig.rightScreenType() );

    if (( session_->isPrepack() ) && ( 0 != rightScreen ))
        return NULL;

    ScreenWidget *screen = mode_->createScreen<PluMainScreen>();

    return screen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget *PluInfoScreen::swipeScreenUp() const
{
    ScreenWidget *screen( NULL );
    int bottomScreen      = session_->scaleConfig->scaleConfig.bottomScreenType();

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

        screen = mode_->createScreen<MeatSheetScreen>();
    }

    return screen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO ( "PluInfoScreen", "onSessionMessage" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // label stock metrics
    if (( Printer == device ) && ( PrStockMetrics == event ))
    {
        DomPrinterStockMetrics m( msg );

        // append metrics to list
        labelStockMetrics_[m.printerType()] = m;

        // update field
        updateLabelStock();
    }

    // weigher configuration
    else if (( Weigher == device ) && ( WgInquire == event ))
    {
        DomWeigherConfigRead m( msg );
        setScaleMetric( m.isScaleMetric() );
    }

    // updating the printer names, type, usb port and ip
    else if(Printer == msg[0].toInt() && PrWriteAllExternalPrinter == msg[1].toInt())
    {
        updatePrinterData(msg);
    }

    // printer print and ask for prompt status
    else if(Printer == msg[0].toInt() && PrWriteAllProductTextPrintParams == msg[1].toInt())
    {
        updatePrinterPromptData(msg);
    }

    // scale config
    else if (( ConfigurationDev == device ) && ( CfgScaleWrite == event ))
    {
        MadpScaleConfig m( msg );
        setScaleMetric( m.isScaleMetric() );
    }

    // peripheral list
    else if (( System == device ) && ( SysPeripheralList == event ))
    {
        // request settings
        requestPrinterStockMetrics();
    }

    // graphics
    else if (( GraphicDev == device ) && ( GraphicReadAllNames == event ))
    {
        QStringList filter;
        filter.append( "*.png" );

        // sync up graphics
        if ( !mainWindow_->isPrimarySessionType() )
        {
            LOG_TRACE( "sync label images...", NULL );

            if ( !filesystem::syncFolder( folderType::LabelImages ) )
                LOG_WARN( "could not sync label images!", NULL );
        }

        // get list of user graphics
        QDir userGraphicsDir( filesystem::folder( folderType::LabelImages ) );

        const QStringList userGraphics( userGraphicsDir.entryList( filter, QDir::Files | QDir::NoSymLinks, QDir::Name ) );

        // process graphics from session
        DomGraphicNameListProduct m( msg );

        const QStringList graphics( m.graphicList().split( ",", QString::KeepEmptyParts ) );

        // process!
        if ( graphics.size() )
        {
            ScaleConfiguration *scaleConfig( session_->scaleConfig );

            // clear session graphics
            scaleConfig->dataStore_.store<graphic>()->clear();
            scaleConfig->graphicMetaDataMap_.clear();

            // iterate graphics
            foreach ( const QString &graphicName, graphics )
            {
                const QString filename( filesystem::absoluteFilePath( graphicName, folderType::LabelImages ) );

                if ( QFile::exists( filename ) )
                {
                    // add graphic into data store
                    graphic g( graphicName, QImage( filename ) );

                    scaleConfig->dataStore_.store<graphic>()->insert( g );

                    // add graphic meta data
                    ScaleConfiguration::metaData md;
                    md.custom = userGraphics.contains( graphicName );

                    scaleConfig->graphicMetaDataMap_.insert( g.name(), md );
                }
            }
        }
    }

    // register session
    else if (( SessionDev == device ) && ( SnRegister == event ))
    {
        LOG_INFO( "session has been registered!", NULL );

        // request labels and graphics
        resetLabelData();
    }

    // database restore
    else if (( BackupRestoreDev == device ) && ( Terminate  == event ))
    {
        // FT-1115 request configuration after db restore
        resetLabelData();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onClearPluForCutTest()
{
    // clear current product details
    clearFields();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onCurrentProductChanged( bool nof )
{
    LOG_CLASS_INFO( "PluInfoScreen", "onCurrentProductChanged" );

    ScaleConfiguration *config = session_->scaleConfig;
    MadpOperatorCanModify *operatorCanModify = &config->operatorCanModify;

    // dont update update operate mode if we are in self service
    if( config->internalConfig.ssactive_lock )
        return;

    // clear current product details
    clearFields();

    // product not on file, nothing to do
    if ( nof )
    {
        LOG_TRACE( "item is nof", NULL );
        return;
    }

    DomCurrentPLU *product( currentProduct() );
    qDebug() << "PluInfoScreen::onCurrentProductChanged():  P3 label length = " << product->thirdLabelTypeLength();

    // handle product
    if ( product )
    {
        // shelf life
        shelfLife_->setAccessLevel( operatorCanModify->shelfLifeAccessLevel() );

        if ( product->shelfLifeHours() )
            shelfLife_->setText( QString::number( product->shelfLifeHours() ) );

        if ( !operatorCanModify->shelfLife() )
            LOG_DEBUG( "operator not allowed to modify shelf life", NULL );
        else
            shelfLife_->setReadOnly( false );

        // product life
        productLife_->setAccessLevel( operatorCanModify->productLifeAccessLevel() );

        if ( product->productLifeHours() )
            productLife_->setText( QString::number( product->productLifeHours() ) );

        if ( !operatorCanModify->productLife() )
            LOG_DEBUG( "operator not allowed to modify product life", NULL );
        else
            productLife_->setReadOnly( false );

        ///////////////////////////////////////////////////////////////////////
        // labels

        signalPluLableOnce_ = false;

         // bc we would rather leave the field blank rather than display 0
        if( product->primaryLabelType() > 0 )
        {
            primaryLabelId_->setText( QString::number( product->primaryLabelType() ) );
            primaryLabelId_->setTextSecond( makeLabelStockDimensions( product->primaryLabelTypeWidth(), product->primaryLabelTypeLength() ) );
        }

        if( product->secondLabelType() > 0 )
        {
            additionalLabelId_->setText( 0, QString::number( product->secondLabelType() ) );
            additionalLabelId_->setTextSecond( 0, makeLabelStockDimensions( product->secondLabelTypeWidth(), product->secondLabelTypeLength() ) );
        }

        if( product->thirdLabelType() > 0 )
        {
            additionalLabelId_->setText( 1, QString::number( product->thirdLabelType() ) );
            additionalLabelId_->setTextSecond( 1, makeLabelStockDimensions( product->thirdLabelTypeWidth(), product->thirdLabelTypeLength() ) );
        }

        primaryLabelId_->setAccessLevel( operatorCanModify->labelTypeAccessLevel() );

        for ( int i = 0; i < additionalLabelId_->arraySize(); ++i )
        {
            hobart::labeledDoubleNumericLineEdit *widget = additionalLabelId_->widgetAt( i );
            widget->setAccessLevel( operatorCanModify->labelTypeAccessLevel() );
        }

        if ( !operatorCanModify->labelType() )
            LOG_DEBUG( "operator not allowed to modify label types", NULL );
        else
        {
            primaryLabelId_->setReadOnly( false );
            additionalLabelId_->setReadOnly( false );
        }

        ///////////////////////////////////////////////////////////////////////
        // graphics

        primaryGraphicId_->setText( product->graphic1Name() );

        additionalGraphicId_->setText( 0, product->graphic2Name() );
        additionalGraphicId_->setText( 1, product->graphic3Name() );
        additionalGraphicId_->setText( 2, product->graphic4Name() );

        primaryGraphicId_->setReadOnly( false );
        additionalGraphicId_->setReadOnly( false );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onCurrentProductSupportChanged( int type )
{
    LOG_CLASS_INFO( "PluInfoScreen", "onCurrentProductSupportChanged" );

    DomPluSupport *support = operateMode_->currentProductSupport( (OperateMode::ProductSupportType) type );

    QString strippedSupportText( session_->uiUtilities->stripFontChanges( support->supportText() ) );

    switch ( type )
    {
    case OperateMode::ExpandedText:
        expandedText_->setText( strippedSupportText );

        // update hook
        sessionHooks::instance()->updateFieldValue( sessionHooks::itemExpandedText, strippedSupportText );

        break;

    case OperateMode::Nutrifacts:

        {
            ScaleConfiguration *config = session_->scaleConfig;

            // nutrifacts label
            if ( !config->dataStore_.store<label>()->contains( nutrifactsLabelId_ ) )
                LOG_WARN( "nutrifacts label %d is missing from data store", nutrifactsLabelId_ );
            else if ( !config->dataStore_.store<label>()->contains( nutrifactsLabelIdZoomed_ ) )
                LOG_WARN( "nutrifacts label %d is missing from data store", nutrifactsLabelIdZoomed_ );

            // nutrifacts text
            else if (( support->supportText().isEmpty() ) && ( support->supportText2().isEmpty() ) &&
                ( support->supportText3().isEmpty() ) && ( support->supportText4().isEmpty() ))
                LOG_DEBUG( "item has no nutrifact text", NULL );
            else
            {
                // create nutrifact data
                nutrifact n( support->pluNum().toInt() );

                n.setColumn( 0, support->supportText() );
                n.setColumn( 1, support->supportText2() );
                n.setColumn( 2, support->supportText3() );
                n.setColumn( 3, support->supportText4() );

                // insert into data store
                config->dataStore_.store<nutrifact>()->insert( n );

                // set the label
                nutrition_->setNutrifactsId( n.id() );

                nutrition_->setLabel( config->dataStore_.store<label>()->value( nutrifactsLabelId_ ) );
                nutrition_->setLabelZoomed( config->dataStore_.store<label>()->value( nutrifactsLabelIdZoomed_ ) );

                // update hooks
                QString labelFilename( nutrition_->saveRenderedImage() );

                if ( labelFilename.length() )
                    sessionHooks::instance()->updateFieldValue( sessionHooks::itemNutrifactsPanel, labelFilename );
            }
        }

        break;

    case OperateMode::Recipe:
        recipe_->setText( strippedSupportText );
        break;

    case OperateMode::CookingInstructions:
        cookingInstructions_->setText( strippedSupportText );
        break;

    case OperateMode::Allergens:
        allergens_->setText( strippedSupportText );
        break;

    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onCurrentProductUpdateField( const DomUpdateRunModeField& m )
{
    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;

    const QString newValue( const_cast<DomUpdateRunModeField*>( &m )->newValue() );

    // handle update
    switch ( (OperateMode::ProductWriteSingleFieldOffset) m.offsetIndex() )
    {
    case OperateMode::OffsetShelfLifeHours:
        shelfLife_->fromInt( m.field<int>( newValue ) );
        break;

    case OperateMode::OffsetProductLifeHours:
        productLife_->fromInt( m.field<int>( newValue ) );
        break;

    case OperateMode::OffsetPrimaryLabelType:
        primaryLabelId_->fromInt( m.field<int>( newValue ) );
        break;

    case OperateMode::OffsetSecondLabelType:
        additionalLabelId_->widgetAt( 0 )->fromInt( m.field<int>( newValue ) );
        break;

    case OperateMode::OffsetThirdLabelType:
        additionalLabelId_->widgetAt( 1 )->fromInt( m.field<int>( newValue ) );
        break;

    case OperateMode::OffsetGraphic1Name:
        primaryGraphicId_->setText( newValue );
        break;

    case OperateMode::OffsetGraphic2Name:
        additionalGraphicId_->widgetAt( 0 )->setText( newValue );
        break;

    case OperateMode::OffsetGraphic3Name:
        additionalGraphicId_->widgetAt( 1 )->setText( newValue );
        break;

    case OperateMode::OffsetGraphic4Name:
        additionalGraphicId_->widgetAt( 2 )->setText( newValue );
        break;

    case OperateMode::OffsetPrimaryLabelTypeWidth:
        primaryLabelId_->setTextSecond( makeLabelStockDimensions( product->primaryLabelTypeWidth(), product->primaryLabelTypeLength() ) );
        break;

    case OperateMode::OffsetPrimaryLabelTypeLength:
        primaryLabelId_->setTextSecond( makeLabelStockDimensions( product->primaryLabelTypeWidth(), product->primaryLabelTypeLength() ) );
        break;

    case OperateMode::OffsetSecondLabelTypeWidth:
        additionalLabelId_->setTextSecond( 0, makeLabelStockDimensions( product->secondLabelTypeWidth(), product->secondLabelTypeLength() ) );
        break;

    case OperateMode::OffsetSecondLabelTypeLength:
        additionalLabelId_->setTextSecond( 0, makeLabelStockDimensions( product->secondLabelTypeWidth(), product->secondLabelTypeLength() ) );
        break;

    case OperateMode::OffsetThirdLabelTypeWidth:
        additionalLabelId_->setTextSecond( 1, makeLabelStockDimensions( product->thirdLabelTypeWidth(), product->thirdLabelTypeLength() ) );
        break;

    case OperateMode::OffsetThirdLabelTypeLength:
        additionalLabelId_->setTextSecond( 1, makeLabelStockDimensions( product->thirdLabelTypeWidth(), product->thirdLabelTypeLength() ) );
        break;

    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onNumberPadMoved( Qt::DockWidgetArea where )
{
    ///////////////////////////////////////////////////////////////////////////
    // delete old layout

    QLayout *oldLayout = layout();

    if ( oldLayout )
        delete oldLayout;

    ///////////////////////////////////////////////////////////////////////////
    // create layout

    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;

    QVBoxLayout *screenLayout = new QVBoxLayout( this );
    screenLayout->setContentsMargins( QMargins() );
    screenLayout->setSpacing( 0 );

    // top row
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setContentsMargins( QMargins() );
    horizontalLayout->setSpacing( 0 );

    horizontalLayout->addStretch();
    horizontalLayout->addWidget( shelfLife_ );
    horizontalLayout->addWidget( productLife_ );
    horizontalLayout->addWidget( labelStock_ );
    horizontalLayout->addWidget( primaryLabelId_ );
    horizontalLayout->addWidget( primaryGraphicId_ );
    horizontalLayout->addStretch();

    screenLayout->addLayout( horizontalLayout );

    // bottom row
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setContentsMargins( QMargins() );
    horizontalLayout->setSpacing( 0 );

    // number pad on left side
    // * nutrition
    // * plu text fields
    // * label and graphics
    if ( Qt::LeftDockWidgetArea == where )
    {
        // left
        verticalLayout = new QVBoxLayout();
        verticalLayout->setContentsMargins( QMargins() );
        verticalLayout->setSpacing( 0 );

        verticalLayout->addWidget( nutrition_ );

        horizontalLayout->addLayout( verticalLayout );

        // middle
        verticalLayout = new QVBoxLayout();
        verticalLayout->setContentsMargins( QMargins() );
        verticalLayout->setSpacing( 0 );

        verticalLayout->addWidget( expandedText_ );
        verticalLayout->addWidget( recipe_ );
        verticalLayout->addWidget( cookingInstructions_ );
        verticalLayout->addWidget( allergens_ );

        horizontalLayout->addLayout( verticalLayout );

        // right
        verticalLayout = new QVBoxLayout();
        verticalLayout->setContentsMargins( QMargins() );
        verticalLayout->setSpacing( 0 );

        verticalLayout->addWidget( additionalLabelId_ );
        additionalLabelId_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
        additionalLabelId_->setLabelAlignment( Qt::AlignRight | Qt::AlignVCenter  );

        verticalLayout->addWidget( additionalGraphicId_ );
        additionalGraphicId_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
        additionalGraphicId_->setLabelAlignment( Qt::AlignRight | Qt::AlignVCenter  );

        horizontalLayout->addLayout( verticalLayout );

        horizontalLayout->addStretch();
    }

    // number pad on right side
    // * label and graphics
    // * plu text fields
    // * nutrition
    else if ( Qt::RightDockWidgetArea == where )
    {
        horizontalLayout->addStretch();

        // left
        verticalLayout = new QVBoxLayout();
        verticalLayout->setContentsMargins( QMargins() );
        verticalLayout->setSpacing( 0 );

        verticalLayout->addWidget( additionalLabelId_ );
        additionalLabelId_->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
        additionalLabelId_->setLabelAlignment( Qt::AlignLeft | Qt::AlignVCenter  );

        verticalLayout->addWidget( additionalGraphicId_ );
        additionalGraphicId_->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
        additionalGraphicId_->setLabelAlignment( Qt::AlignLeft | Qt::AlignVCenter  );

        horizontalLayout->addLayout( verticalLayout );

        // middle
        verticalLayout = new QVBoxLayout();
        verticalLayout->setContentsMargins( QMargins() );
        verticalLayout->setSpacing( 0 );

        verticalLayout->addWidget( expandedText_ );
        verticalLayout->addWidget( recipe_ );
        verticalLayout->addWidget( cookingInstructions_ );
        verticalLayout->addWidget( allergens_ );

        horizontalLayout->addLayout( verticalLayout );

        // right
        verticalLayout = new QVBoxLayout();
        verticalLayout->setContentsMargins( QMargins() );
        verticalLayout->setSpacing( 0 );

        verticalLayout->addWidget( nutrition_ );

        horizontalLayout->addLayout( verticalLayout );
    }

    screenLayout->addLayout( horizontalLayout );

    screenLayout->addStretch();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onScreenChanged()
{
    // only execute code when we are active screen
    if ( mode_->screen() != this )
        return;

    // make sure weight bar is visible
    if ( operateMode_ )
        operateMode_->showWeightBar( WeightBarWidget::FreqShopperDisplayMode );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onAdditionalGraphicIdEditingFinished( int index )
{
    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;

    int offset = 0;
    QString newValue( additionalGraphicId_->widgetAt( index )->text() );
    QString oldValue;

    if ( 0 == index )
    {
        offset = OperateMode::OffsetGraphic2Name;
        oldValue = product->graphic2Name();
    }
    else if ( 1 == index )
    {
        offset = OperateMode::OffsetGraphic3Name;
        oldValue = product->graphic3Name();
    }
    else if ( 2 == index )
    {
        offset = OperateMode::OffsetGraphic4Name;
        oldValue = product->graphic4Name();
    }

    // write
    operateMode_->writeSingleField( (OperateMode::ProductWriteSingleFieldOffset) offset, newValue, oldValue );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onAdditionalLabelIdEditingFinished( int index )
{
    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;

    QString newValue( QString::number( additionalLabelId_->widgetAt( index )->toInt() ) );
    OperateMode::ProductWriteSingleFieldOffset labelOffset = OperateMode::OffsetUnknown;
    QString origValue;

    if( signalPluLableOnce_ )
    {
        signalPluLableOnce_ = false;
        return;
    }

    signalPluLableOnce_ = true;

    if ( 0 == index )
    {
        labelOffset = OperateMode::OffsetSecondLabelType;
        origValue = product->secondLabelTypeRaw();
    }
    else if ( 1 == index )
    {
        labelOffset = OperateMode::OffsetThirdLabelType;
        origValue = product->thirdLabelTypeRaw();
    }

    // write
    operateMode_->writeSingleField( labelOffset, newValue, origValue );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onExpandedTextPrint()
{
#ifndef QT_DEBUG
    // allow only for primary session
    if ( !mainWindow_->isPrimarySessionType() )
        return;
#endif

    // the name of the field (here "expandedText" is mapped to its Product.h->ProdSupportTextTypes enum
    int fieldNumber  = prTextFieldNames.value( expandedText_->objectName() );

    // the fieldNumber is then mapped into the fieldName to find out it's prompt status - and what printer to print to
    int promptAction = prDataNamesMap[fieldNumber].toInt();

    if ( promptUser_ == promptAction )
    {
        // Prompt user for the printer to print on
        fieldToPrint_ = fieldNumber;
        launchPrinterPrompt();
    }
    else
    {
        // Go ahead and print
        QString msg = QString::number( OperateMode::ExpandedText ) + "," + prDataNamesMap[fieldNumber];
        session_->send( ProductDev, ProductPrintProductSupportText, msg );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onCookingInstructionsPrint()
{
#ifndef QT_DEBUG
    // allow only for primary session
    if ( !mainWindow_->isPrimarySessionType() )
        return;
#endif

    // see onExpandedTextPrint() for a full explanation of these fields and how they map
    int fieldNumber  = prTextFieldNames.value( cookingInstructions_->objectName() );
    int promptAction = prDataNamesMap[fieldNumber].toInt();

    if ( promptUser_ == promptAction )
    {
        fieldToPrint_ = fieldNumber;
        launchPrinterPrompt();                                          // Prompt user for the printer to print on
    }
    else
    {
        QString msg = QString::number( OperateMode::CookingInstructions ) + "," + prDataNamesMap[fieldNumber];
        session_->send( ProductDev, ProductPrintProductSupportText, msg );    // Go ahead and print
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onRecipePrint()
{
#ifndef QT_DEBUG
    // allow only for primary session
    if ( !mainWindow_->isPrimarySessionType() )
        return;
#endif

    //if(printerPrompter_ != NULL)
    //    delete printerPrompter_;

    // see onExpandedTextPrint() for a full explanation of these fields and how they map
    int fieldNumber  = prTextFieldNames.value( recipe_->objectName() );
    int promptAction = prDataNamesMap[fieldNumber].toInt();

    if ( promptUser_ == promptAction )
    {
        fieldToPrint_ = fieldNumber;
        launchPrinterPrompt();                                          // Prompt user for the printer to print on
    }
    else
    {
        QString msg = QString::number( OperateMode::Recipe ) + "," + prDataNamesMap[fieldNumber];
        session_->send( ProductDev, ProductPrintProductSupportText, msg );    // Go ahead and print
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onAllergensPrint()
{
#ifndef QT_DEBUG
    // allow only for primary session
    if ( !mainWindow_->isPrimarySessionType() )
        return;
#endif

    //if(printerPrompter_ != NULL)
    //    delete printerPrompter_;

    // see onExpandedTextPrint() for a full explanation of these fields and how they map
    int fieldNumber  = prTextFieldNames.value( allergens_->objectName() );
    int promptAction = prDataNamesMap[fieldNumber].toInt();

    if ( promptUser_ == promptAction )
    {
        fieldToPrint_ = fieldNumber;
        launchPrinterPrompt();                                          // Prompt user for the printer to print on
    }
    else
    {
        QString msg = QString::number( OperateMode::Allergens ) + "," + prDataNamesMap[fieldNumber];
        session_->send( ProductDev, ProductPrintProductSupportText, msg );    // Go ahead and print
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onNutrifactsPrint()
{
#ifndef QT_DEBUG
    // allow only for primary session
    if ( !mainWindow_->isPrimarySessionType() )
        return;
#endif

    // see onExpandedTextPrint() for a full explanation of these fields and how they map
    int fieldNumber  = prTextFieldNames.value( nutrition_->objectName() );
    int promptAction = prDataNamesMap[fieldNumber].toInt();

    if ( promptUser_ == promptAction )
    {
        fieldToPrint_ = fieldNumber;
        launchPrinterPrompt();                                          // Prompt user for the printer to print on
    }
    else
    {
        QString msg = QString::number( OperateMode::Nutrifacts ) + "," + prDataNamesMap[fieldNumber];
        session_->send( ProductDev, ProductPrintProductSupportText, msg );    // Go ahead and print
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onNutrifactsZoom()
{
    LOG_CLASS_INFO( "PluInfoScreen", "onNutrifactsZoom" );

    // update hooks
    const QString labelFilename( nutrition_->saveZoomedRenderedImage() );

    if ( labelFilename.isEmpty() )
    {
        LOG_WARN( "invalid filename", NULL );
        return;
    }

    // create dialog
    GraphicDialog *dialog( new GraphicDialog( labelFilename, mainWindow_ ) );

    // validate graphic
    if ( !dialog->isValid() )
    {
        LOG_WARN( "invalid graphic", NULL );
        dialog->deleteLater();
    }

    // show
    else
    {
        LOG_DEBUG( "showing dialog for graphic '%s'", qPrintable( labelFilename ) );

        // show dialog!
        dialog->setShowCustomerDisplay( true );
        dialog->open();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::launchPrinterPrompt()
{
    const QPoint pos(
        (mainWindow_->width() * 275) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 200) / SessionMainWindow::scalingDividerY_ );

    printerPrompter_ = new hobart::printPrompt( this, printerNumsList, printerNamesList, printerTypesList, printerDriversList, mainWindow_ );
    printerPrompter_->move( pos );
    printerPrompter_->show();

    // listening for SIGNALS from the printer prompt
    connect(printerPrompter_, SIGNAL( printPromptDoneClicked( int ) ),    SLOT( onPrintPromptDoneClicked( int )) );
}

void PluInfoScreen::onPrintPromptDoneClicked( int promptIndex )
{
    //int fieldNumber  = prTextFieldNames.value( fieldToPrint_ );

    QString msg = QString::number( fieldToPrint_ ) + "," + QString::number( promptIndex );

    session_->send( ProductDev, ProductPrintProductSupportText, msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onInvalidAccessLevel( QWidget *field )
{
    if ( !field )
        return;

    operateMode_->processInvalidAccessLevel( field );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onShelfLifeEditingFinished()
{
    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;

    // write
    operateMode_->writeSingleField(
        OperateMode::OffsetShelfLifeHours,
        QString::number( shelfLife_->toInt() ),
        product->shelfLifeDaysRaw() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onPrimaryGraphicIdEditingFinished()
{
    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;

    // write
    operateMode_->writeSingleField(
        OperateMode::OffsetGraphic1Name,
        primaryGraphicId_->text(),
        product->graphic1Name() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onPrimaryLabelIdEditingFinished()
{
    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;

    if( signalPluLableOnce_ )
    {
        signalPluLableOnce_ = false;
        return;
    }

    signalPluLableOnce_ = true;

    QString origVal = product->primaryLabelTypeRaw();

    operateMode_->writeSingleField(
        OperateMode::OffsetPrimaryLabelType,
        QString::number( primaryLabelId_->toInt() ),
        origVal );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
OperateMode::ProductWriteSingleFieldOffset PluInfoScreen::getModeLabelOffset(OperateMode::ProductWriteSingleFieldOffset off)
{
    // in:  primary, secondary, or third label type

    OperateMode::ProductWriteSingleFieldOffset newOff = OperateMode::OffsetUnknown;
    bool isPick5Enabled  = session_->scaleConfig->getIsPick5Enabled();
    bool isPick5Licensed = session_->uiUtilities->isFeatureLicensed( SCALE_PICK5 );

    // weigher mode
    weighModeTotalType_ = &session_->scaleConfig->weighModeTotalType;
    OpModeEvent mode( (OpModeEvent) weighModeTotalType_->weighMode() );

    // what is the default label type. By default we display the default label,
    // unless there is something in it's mode specific label type. Or if it's a wrapper,
    // and then we automatically use Prepack label if there is one no matter the mode

    // its always one of these 3 fields being edited on the screen.  But whose value is really there.
    if( OperateMode::OffsetDefaultLabelTypeNum1 == off )
    {
        if( isPick5Licensed && isPick5Enabled )
        {
            newOff = OperateMode::OffsetPick5LabelTypeNum1;             // cant test the mode against this bc pick5 is not a mode
            return newOff;
        }
    }
    else if( OperateMode::OffsetDefaultLabelTypeNum2 == off )
    {
        if( isPick5Licensed && isPick5Enabled )
        {
            newOff = OperateMode::OffsetPick5LabelTypeNum2;             // cant test the mode against this bc pick5 is not a mode
            return newOff;
        }
    }
    else if( OperateMode::OffsetDefaultLabelTypeNum3 == off )
    {
        if( isPick5Licensed && isPick5Enabled )
        {
            newOff = OperateMode::OffsetPick5LabelTypeNum3;             // cant test the mode against this bc pick5 is not a mode
            return newOff;
        }
    }

    // okay we have set up the default label, determine if overridden
    //newOff = off;           // now see we we can override it
    newOff = OperateMode::OffsetUnknown;

    switch( off )                                                               // field offset being changed pri(29), sec(31), or 3rd(33)?
    {
        case OperateMode::OffsetDefaultLabelTypeNum1:                           // 111: wasPrimaryDefaultLabelType
        {
            switch( mode )
            {
                case OpModeManualWeigh:                                         // 5
                    if( currentProduct()->primaryManualLabelType() > 0 )
                        newOff = OperateMode::OffsetManualLabelTypeNum1;            // [111][5]
                break;

                case OpModeAutoWeigh:                                           // 3 - Wrapper
                case OpModeSemiWeigh:                                           // 4 - Wrapper
                case OpModeAutoManualApply:                                     // 14 - Prepack OPeration on Counter Scale
                    if( currentProduct()->primaryPrepackLabelType() > 0 )
                        newOff = OperateMode::OffsetPrepackLabelTypeNum1;           // [111][14]
                break;

                case OpModeProductionEntry:                                     // 15
                    if( currentProduct()->primaryProdEntryLabelType() > 0 )
                        newOff = OperateMode::OffsetProdEntryLabelTypeNum1;         // [111][15]
                break;

                case OpModeSelfService:                                         // 13
                    if( currentProduct()->primarySelfServLabelType() > 0 )
                        newOff = OperateMode::OffsetSelfServLabelTypeNum1;          // [111][13]
                break;

                default:
                    // Modes not considered: OpModeIdle, OpModeOperate, OpModePluEntry, OpModeSupervisor, OpModeService, OpModeServiceLogin,
                    //                       OpModePowerDown, OpModeMonitor, OpModeStatus, OpModeAccumulateItems, OpModeBuildAMeal, OpModeWrapOnly
                break;
            }
        }
        break;

        case OperateMode::OffsetDefaultLabelTypeNum2:                               // 112: wasSecondLabelType
        {
            switch( mode )
            {
                case OpModeManualWeigh:                                             // 5
                    if( currentProduct()->secondManualLabelType() > 0 )
                        newOff = OperateMode::OffsetManualLabelTypeNum2;            // [112][5]
                break;

                case OpModeAutoWeigh:                                               // 3 - Wrapper
                case OpModeSemiWeigh:                                               // 4 - Wrapper
                case OpModeAutoManualApply:                                         // 14 - Prepack Operation on Counter Scale
                    if( currentProduct()->secondPrepackLabelType() > 0 )
                        newOff = OperateMode::OffsetPrepackLabelTypeNum2;           // [112][14]
                break;

                case OpModeProductionEntry:                                         // 15
                    if( currentProduct()->secondProdEntryLabelType() > 0 )
                        newOff = OperateMode::OffsetProdEntryLabelTypeNum2;         // [112][15]
                break;

                case OpModeSelfService:                                             // 13
                    if( currentProduct()->secondSelfServLabelType() > 0 )
                        newOff = OperateMode::OffsetSelfServLabelTypeNum2;          // [112][13]
                break;

                default:
                    // Modes not considered: OpModeIdle, OpModeOperate, OpModePluEntry, OpModeSupervisor, OpModeService, OpModeServiceLogin,
                    //                       OpModePowerDown, OpModeMonitor, OpModeStatus, OpModeAccumulateItems, OpModeBuildAMeal, OpModeWrapOnly
                break;
            }
        }
        break;

        case OperateMode::OffsetDefaultLabelTypeNum3:                               // 113: wasthirdLabelType
        {
            switch( mode )
            {
                case OpModeManualWeigh:                                             // 5
                    if( currentProduct()->thirdManualLabelType() > 0 )
                        newOff = OperateMode::OffsetManualLabelTypeNum3;            // [113][5]
                break;

                case OpModeAutoWeigh:                                               // 3 - Wrapper
                case OpModeSemiWeigh:                                               // 4 - Wrapper
                case OpModeAutoManualApply:                                         // 14 - Prepack Operation on Counter Scale
                    if( currentProduct()->thirdPrepackLabelType() > 0 )
                        newOff = OperateMode::OffsetPrepackLabelTypeNum3;           // [113][14]
                break;

                case OpModeProductionEntry:                                         // 15
                    if( currentProduct()->thirdProdEntryLabelType() > 0 )
                        newOff = OperateMode::OffsetProdEntryLabelTypeNum3;         // [113][15]
                break;

                case OpModeSelfService:                                             // 13
                    if( currentProduct()->thirdSelfServLabelType() > 0 )
                        newOff = OperateMode::OffsetSelfServLabelTypeNum3;          // [113][13]
                break;

                default:
                    // Modes not considered: OpModeIdle, OpModeOperate, OpModePluEntry, OpModeSupervisor, OpModeService, OpModeServiceLogin,
                    //                       OpModePowerDown, OpModeMonitor, OpModeStatus, OpModeAccumulateItems, OpModeBuildAMeal, OpModeWrapOnly
                break;
            }
        }
        break;
    }

    return newOff;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
QString PluInfoScreen::getModeLabelValue(OperateMode::ProductWriteSingleFieldOffset off)
{
    QString newLabel = modeLabels_[off];
    return newLabel;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::onProductLifeEditingFinished()
{
    DomCurrentPLU *product( currentProduct() );

    if ( !product )
        return;

    // write
    operateMode_->writeSingleField(
        OperateMode::OffsetProductLifeHours,
        QString::number( productLife_->toInt() ),
        product->productLifeDaysRaw() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::initialize()
{
    printerPrompter_ = NULL;

    static const QMargins margins( 10, 10, 10, 6 );

    static const QMargins arrayWidgetMargins( 10, 10, 10, 10 );
    static const QMargins arrayWidgetChildMargins( 8, 5, 8, 5 );

    ScaleConfiguration *config = session_->scaleConfig;

    // pixmaps
    const QSize upperWidgetSize( mainWindow_->width() / 5, scaleHeightForScreen( 100 ) );
    const QSize labelStockSize( mainWindow_->width() - (upperWidgetSize.width() * 4), upperWidgetSize.height() );

    const QSize additionalLabelsSize( scaleSizeForScreen( QSize( 270, 160 ) ) );
    const QSize additionalGraphicsSize( scaleSizeForScreen( QSize( 270, 220 ) ) );

    const QSize nutrifactsSize( scaleSizeForScreen( QSize( 230, 380 ) ) );

    const QSize textFieldsSize( mainWindow_->width() - additionalLabelsSize.width() - nutrifactsSize.width() - 45, scaleHeightForScreen( 380 ) / 4 );

    const hobart::backgroundWidgetRenderer renderField(
        new hobart::rectangleBackgroundWidget(),
        new hobart::darkOperatorBackgroundWidgetRendererAppearance() );

    const QPixmap upperWidgetPixmap( renderField.drawControl( upperWidgetSize, hobart::Normal ) );
    const QPixmap upperWidgetPixmapFocused( renderField.drawControl( upperWidgetSize, hobart::Focused ) );
    const QPixmap upperWidgetPixmapReadOnly( renderField.drawControl( upperWidgetSize, hobart::ReadOnly ) );
    const QPixmap upperWidgetPixmapShadow( renderField.drawDropShadow( upperWidgetSize ) );

    const QPixmap labelStockPixmap( renderField.drawControl( labelStockSize, hobart::Normal ) );
    const QPixmap labelStockPixmapFocused( renderField.drawControl( labelStockSize, hobart::Focused ) );
    const QPixmap labelStockPixmapReadOnly( renderField.drawControl( labelStockSize, hobart::ReadOnly ) );
    const QPixmap labelStockPixmapShadow( renderField.drawDropShadow( labelStockSize ) );

    const QPixmap field270x155( renderField.drawControl( additionalLabelsSize, hobart::ReadOnly ) );
    const QPixmap field270x155Shadow( renderField.drawDropShadow( additionalLabelsSize ) );

    const QPixmap field270x220( renderField.drawControl( additionalGraphicsSize, hobart::ReadOnly ) );
    const QPixmap field270x220Shadow( renderField.drawDropShadow( additionalGraphicsSize ) );

    const QPixmap field470x94( renderField.drawControl( textFieldsSize, hobart::ReadOnly ) );
    const QPixmap field470x94Shadow( renderField.drawDropShadow( textFieldsSize ) );

    const QPixmap field230x375( renderField.drawControl( nutrifactsSize, hobart::ReadOnly ) );
    const QPixmap field230x375Shadow( renderField.drawDropShadow( nutrifactsSize ) );

    const QSize fieldWidgetSize( additionalLabelsSize.width() - 20, scaleHeightForScreen( 55 ) );

    const hobart::backgroundWidgetRenderer renderFieldWidget(
        new hobart::panelButtonBackgroundWidget( 5 ),
        new hobart::darkOperatorBackgroundWidgetRendererAppearance() );

    const QPixmap fieldWidget250x55( renderFieldWidget.drawControl( fieldWidgetSize, hobart::Normal ) );
    const QPixmap fieldWidget250x55Focused( renderFieldWidget.drawControl( fieldWidgetSize, hobart::Focused ) );
    const QPixmap fieldWidget250x55ReadOnly( renderFieldWidget.drawControl( fieldWidgetSize, hobart::ReadOnly ) );

    // fonts
    modifyFontPointSize setFontPointSize9( scaleFontPointSizeForScreen( 9 ) );
    modifyFontPointSize setFontPointSize14( scaleFontPointSizeForScreen( 14 ) );
    modifyFontPointSize setFontPointSize18( scaleFontPointSizeForScreen( 18 ) );
    modifyFontPointSize setFontPointSize32( scaleFontPointSizeForScreen( 32 ) );

    QFont scaledFont( font() );
    scaledFont.setPointSize( scaleFontPointSizeForScreen( scaledFont.pointSize() ) );

    setFont( scaledFont );

    // signal mappers
    invalidAccessLevelSignal_ = new QSignalMapper( this );
    connect( invalidAccessLevelSignal_, SIGNAL( mapped(QWidget*) ), SLOT( onInvalidAccessLevel(QWidget*) ) );

    ///////////////////////////////////////////////////////////////////////////
    // create widgets

    // shelf life
    shelfLife_ = new hobart::labeledNumericLineEdit( this, upperWidgetPixmap );
    shelfLife_->setBackgroundFocused( upperWidgetPixmapFocused );
    shelfLife_->setBackgroundReadOnly( upperWidgetPixmapReadOnly );
    shelfLife_->setDropShadow( upperWidgetPixmapShadow );
    shelfLife_->setContentsMargins( margins );
    shelfLife_->setObjectName( "shelfLife" );
    shelfLife_->setLabelObjectName( "shelfLife_label" );

    shelfLife_->setInputTruncation( true );
    shelfLife_->setInputWrapping( true );
    shelfLife_->setValidator( shelfLifeValidator_ = new QIntValidator( 0, 23, this ) );
    setFontPointSize32( shelfLife_ );

    connect( shelfLife_, SIGNAL( editingFinished() ), SLOT( onShelfLifeEditingFinished() ) );

    invalidAccessLevelSignal_->setMapping( shelfLife_, shelfLife_ );
    connect( shelfLife_, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );

    // product life
    productLife_ = new hobart::labeledNumericLineEdit( this, upperWidgetPixmap );
    productLife_->setBackgroundFocused( upperWidgetPixmapFocused );
    productLife_->setBackgroundReadOnly( upperWidgetPixmapReadOnly );
    productLife_->setDropShadow( upperWidgetPixmapShadow );
    productLife_->setContentsMargins( margins );
    productLife_->setObjectName( "productLife" );
    productLife_->setLabelObjectName( "productLife_label" );

    productLife_->setInputTruncation( true );
    productLife_->setInputWrapping( true );
    productLife_->setValidator( productLifeValidator_ = new QIntValidator(  0, 23, this ) );
    setFontPointSize32( productLife_ );

    connect( productLife_, SIGNAL( editingFinished() ), SLOT( onProductLifeEditingFinished() ) );

    invalidAccessLevelSignal_->setMapping( productLife_, productLife_ );
    connect( productLife_, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );

    // label stock
    labelStock_ = new hobart::labeledPlainTextEdit( this, labelStockPixmap );
    labelStock_->setBackgroundFocused( labelStockPixmapFocused );
    labelStock_->setBackgroundReadOnly( labelStockPixmapReadOnly );
    labelStock_->setContentsMargins( margins );
    labelStock_->setDropShadow( labelStockPixmapShadow );
    labelStock_->setObjectName( "labelStock" );
    labelStock_->setLabelObjectName( "labelStock_label" );

    labelStock_->setExpandable( false );
    labelStock_->setSizeToFit( true );

    setFontPointSize18( labelStock_ );

    // primary label id
    primaryLabelId_ = new hobart::labeledDoubleNumericLineEdit( this, upperWidgetPixmap );
    primaryLabelId_->setBackgroundFocused( upperWidgetPixmapFocused );
    primaryLabelId_->setBackgroundReadOnly( upperWidgetPixmapReadOnly );
    primaryLabelId_->setContentsMargins( margins );
    primaryLabelId_->setDropShadow( upperWidgetPixmapShadow );
    primaryLabelId_->setObjectName( "primaryLabelId" );
    primaryLabelId_->setLabelObjectName( "primaryLabelId_label" );

    primaryLabelId_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    primaryLabelId_->setAlignmentSecond( Qt::AlignRight | Qt::AlignVCenter );
    primaryLabelId_->setEnabledSecond( false );
    primaryLabelId_->setInputTruncation( true );
    primaryLabelId_->setInputWrapping( true );
    primaryLabelId_->setLabelAlignment( Qt::AlignRight | Qt::AlignVCenter  );
    primaryLabelId_->setValidator( primaryLabelIdValidator_ = new QIntValidator(  0, 9999, this ) );
    setFontPointSize32( primaryLabelId_ );

    connect( primaryLabelId_, SIGNAL( editingFinished() ), SLOT( onPrimaryLabelIdEditingFinished() ) );

    invalidAccessLevelSignal_->setMapping( primaryLabelId_, primaryLabelId_ );
    connect( primaryLabelId_, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );

    // primary graphic id
    primaryGraphicId_ = new hobart::labeledGraphicEdit( this, upperWidgetPixmap );
    primaryGraphicId_->setBackgroundFocused( upperWidgetPixmapFocused );
    primaryGraphicId_->setBackgroundReadOnly( upperWidgetPixmapReadOnly );
    primaryGraphicId_->setContentsMargins( margins );
    primaryGraphicId_->setDropShadow( upperWidgetPixmapShadow );
    primaryGraphicId_->setObjectName( "primaryGraphicId" );
    primaryGraphicId_->setLabelObjectName( "primaryGraphicId_label" );

    primaryGraphicId_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    primaryGraphicId_->setLabelAlignment( Qt::AlignRight | Qt::AlignVCenter  );
    setFontPointSize18( primaryGraphicId_ );

    connect( primaryGraphicId_, SIGNAL( editingFinished() ), SLOT( onPrimaryGraphicIdEditingFinished() ) );

    invalidAccessLevelSignal_->setMapping( primaryGraphicId_, primaryGraphicId_ );
    connect( primaryGraphicId_, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );

    // nutrition facts
    nutrition_ = new hobart::labeledLabelViewer( this, field230x375 );
    nutrition_->setBackgroundFocused( field230x375 );
    nutrition_->setBackgroundReadOnly( field230x375 );
    nutrition_->setDropShadow( field230x375Shadow );
    nutrition_->setContentsMargins( margins );
    nutrition_->setObjectName( "nutrition" );
    nutrition_->setLabelObjectName( "nutrition_label" );

    nutrition_->setPrintButtonVisible( true );
    nutrition_->setStore( &config->dataStore_ );

    connect( nutrition_, SIGNAL( printButtonClicked() ), SLOT( onNutrifactsPrint() ) );
    connect( nutrition_, SIGNAL( zoomButtonClicked() ), SLOT( onNutrifactsZoom() ) );

    // expanded text
    expandedText_ = new hobart::labeledPlainTextEdit( this, field470x94 );
    expandedText_->setBackgroundFocused( field470x94 );
    expandedText_->setBackgroundReadOnly( field470x94 );
    expandedText_->setDropShadow( field470x94Shadow );
    expandedText_->setContentsMargins( margins );
    expandedText_->setObjectName( "expandedText" );
    expandedText_->setLabelObjectName( "expandedText_label" );

    expandedText_->setExpandedTextIconVisible( hobart::labeledPlainTextEdit::Always );
    expandedText_->setPrintButtonVisible( true );
    setFontPointSize9( expandedText_ );

    connect( expandedText_, SIGNAL( printButtonClicked() ), SLOT( onExpandedTextPrint() ) );

    // recipe
    recipe_ = new hobart::labeledPlainTextEdit( this, field470x94 );
    recipe_->setBackgroundFocused( field470x94 );
    recipe_->setBackgroundReadOnly( field470x94 );
    recipe_->setDropShadow( field470x94Shadow );
    recipe_->setContentsMargins( margins );
    recipe_->setObjectName( "recipe" );
    recipe_->setLabelObjectName( "recipe_label" );

    recipe_->setExpandedTextIconVisible( hobart::labeledPlainTextEdit::Always );
    recipe_->setPrintButtonVisible( true );
    setFontPointSize9( recipe_ );

    connect( recipe_, SIGNAL( printButtonClicked() ), SLOT( onRecipePrint() ) );

    // cooking instructions
    cookingInstructions_ = new hobart::labeledPlainTextEdit( this, field470x94 );
    cookingInstructions_->setBackgroundFocused( field470x94 );
    cookingInstructions_->setBackgroundReadOnly( field470x94 );
    cookingInstructions_->setDropShadow( field470x94Shadow );
    cookingInstructions_->setContentsMargins( margins );
    cookingInstructions_->setObjectName( "cookingInstructions" );
    cookingInstructions_->setLabelObjectName( "cookingInstructions_label" );

    cookingInstructions_->setExpandedTextIconVisible( hobart::labeledPlainTextEdit::Always );
    cookingInstructions_->setPrintButtonVisible( true );
    setFontPointSize9( cookingInstructions_ );

    connect( cookingInstructions_, SIGNAL( printButtonClicked() ), SLOT( onCookingInstructionsPrint() ) );

    // allergens
    allergens_ = new hobart::labeledPlainTextEdit( this, field470x94 );
    allergens_->setBackgroundFocused( field470x94 );
    allergens_->setBackgroundReadOnly( field470x94 );
    allergens_->setDropShadow( field470x94Shadow );
    allergens_->setContentsMargins( margins );
    allergens_->setObjectName( "allergens" );
    allergens_->setLabelObjectName( "allergens_label" );

    allergens_->setExpandedTextIconVisible( hobart::labeledPlainTextEdit::Always );
    allergens_->setPrintButtonVisible( true );
    setFontPointSize9( allergens_ );

    connect( allergens_, SIGNAL( printButtonClicked() ), SLOT( onAllergensPrint() ) );

    // additional label id
    additionalLabelId_ = new hobart::labeledDoubleNumericLineEditArray( 2, this, field270x155 );
    additionalLabelId_->setBackgroundFocused( field270x155 );
    additionalLabelId_->setBackgroundReadOnly( field270x155 );
    additionalLabelId_->setDropShadow( field270x155Shadow );
    additionalLabelId_->setContentsMargins( arrayWidgetMargins );
    additionalLabelId_->setObjectName( "additionalLabelId" );
    additionalLabelId_->setLabelObjectName( "additionalLabelId_label" );

    for ( int i = 0; i < additionalLabelId_->arraySize(); ++i )
    {
        hobart::labeledDoubleNumericLineEdit *widget = additionalLabelId_->widgetAt( i );

        widget->setBackground( fieldWidget250x55 );
        widget->setBackgroundFocused( fieldWidget250x55Focused );
        widget->setBackgroundReadOnly( fieldWidget250x55ReadOnly );
        widget->setContentsMargins( arrayWidgetChildMargins );
        setFontPointSize14( widget );

        widget->setInputTruncation( true );
        widget->setInputWrapping( true );
        widget->setLabelVisible( true );
        widget->setLabelText( QString::number( i+2 ) + "." );
        widget->setValidator( primaryLabelIdValidator_ );
        widget->adjustLabelOrientationLeft();

        widget->setEnabledSecond( false );

        invalidAccessLevelSignal_->setMapping( widget, widget );
        connect( widget, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );
    }

    connect( additionalLabelId_, SIGNAL( editingFinished(int) ), SLOT( onAdditionalLabelIdEditingFinished(int) ) );

    // additional graphic id
    additionalGraphicId_ = new hobart::labeledGraphicEditArray( 3, this, field270x220 );
    additionalGraphicId_->setBackgroundFocused( field270x220 );
    additionalGraphicId_->setBackgroundReadOnly( field270x220 );
    additionalGraphicId_->setDropShadow( field270x220Shadow );
    additionalGraphicId_->setContentsMargins( arrayWidgetMargins );
    additionalGraphicId_->setObjectName( "additionalGraphicId" );
    additionalGraphicId_->setLabelObjectName( "additionalGraphicId_label" );

    // list of field names mapped to their Enums
    prTextFieldNames["expandedText"]        = OperateMode::ExpandedText;            // 1
    prTextFieldNames["cookingInstructions"] = OperateMode::CookingInstructions;     // 5
    prTextFieldNames["recipe"]              = OperateMode::Recipe;                  // 6
    prTextFieldNames["allergens"]           = OperateMode::Allergens;               // 9
    prTextFieldNames["nutrition"]           = OperateMode::Nutrifacts;              // 20

    // The prompts for each field has. 0=primaryPrinter, 1=externalPrinter1, 2=externalPrinter2, 3=promptUser
    prDataNamesMap[OperateMode::ExpandedText]           = "0";                      // 1
    prDataNamesMap[OperateMode::CookingInstructions]    = "0";                      // 5
    prDataNamesMap[OperateMode::Recipe]                 = "0";                      // 6
    prDataNamesMap[OperateMode::Allergens]              = "0";                      // 9
    prDataNamesMap[OperateMode::Nutrifacts]             = "0";                      // 20

    // Initial set of messages after 100,1,1
    session_->send(Printer, PrReadAllProductTextPrintParams);       // Request Printer Data Settings (ask for Prompts?)
    session_->send(Printer, PrReadAllExternalPrinters);             // Request the names of all of the external printers
    session_->send( ConfigurationDev, CfgPrepackRead );             // need early bc need to know how many chars on a flashkey
    session_->send( SessionDev, SnReadWeighModeTransactionType );   // need to know early what is the MODE we are in. Manual or Self Service

    for ( int i = 0; i < additionalGraphicId_->arraySize(); ++i )
    {
        hobart::labeledLineEdit *widget = additionalGraphicId_->widgetAt( i );

        widget->setBackground( fieldWidget250x55 );
        widget->setBackgroundFocused( fieldWidget250x55Focused );
        widget->setBackgroundReadOnly( fieldWidget250x55ReadOnly );
        widget->setContentsMargins( arrayWidgetChildMargins );
        setFontPointSize14( widget );

        widget->setLabelVisible( true );
        widget->setLabelText( QString::number( i+2 ) + "." );
        widget->adjustChildCenter();
        widget->adjustLabelOrientationLeft();

        invalidAccessLevelSignal_->setMapping( widget, widget );
        connect( widget, SIGNAL( invalidAccessLevel() ), invalidAccessLevelSignal_, SLOT( map() ) );
    }

    connect( additionalGraphicId_, SIGNAL( editingFinished(int) ), SLOT( onAdditionalGraphicIdEditingFinished(int) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::createLayout()
{
    onNumberPadMoved( mainWindow_->numberPadArea() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::clearFields()
{
    shelfLife_->clear();
    shelfLife_->setReadOnly( true );

    productLife_->clear();
    productLife_->setReadOnly( true );

    //labelStock_->clear();
    labelStock_->setReadOnly( true );

    primaryLabelId_->clear();
    primaryLabelId_->clearSecond();
    primaryLabelId_->setReadOnly( true );

    primaryGraphicId_->clear();
    primaryGraphicId_->setReadOnly( true );

    nutrition_->resetLabel();
    nutrition_->setReadOnly( true );

    expandedText_->clear();
    expandedText_->setReadOnly( true );

    recipe_->clear();
    recipe_->setReadOnly( true );

    cookingInstructions_->clear();
    cookingInstructions_->setReadOnly( true );

    allergens_->clear();
    allergens_->setReadOnly( true );

    additionalLabelId_->clear();
    additionalLabelId_->clearSecond();
    additionalLabelId_->setReadOnly( true );

    additionalGraphicId_->clear();
    additionalGraphicId_->setReadOnly( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PluInfoScreen::updateLabelStock()
{
    bool bold( false );
    int pointSize( scaleFontPointSizeForScreen( 18 ) );

    // font point size
    if ( 3 <= labelStockMetrics_.size() )
    {
        bold = true;
        pointSize = scaleFontPointSizeForScreen( 12 );
    }
    else if ( 2 <= labelStockMetrics_.size() )
        pointSize = scaleFontPointSizeForScreen( 14 );

    modifyFontPointSize setFontPointSize( pointSize );
    setFontPointSize( labelStock_ );

    modifyFontBold setFontBold( bold );
    setFontBold( labelStock_ );

    // single printer
    if ( 1 == labelStockMetrics_.size() )
    {
        DomPrinterStockMetrics m( labelStockMetrics_[0] );

        labelStock_->setText( makeLabelStockDimensions( m.stockWidth(), m.stockLength() ) );
    }

    // multiple printers
    else if ( 2 <= labelStockMetrics_.size() )
    {
        QString lines;

        foreach ( DomPrinterStockMetrics m, labelStockMetrics_ )
        {
            QString dim( makeLabelStockDimensions( m.stockWidth(), m.stockLength(), false ) );

            if ( !lines.isEmpty() )
                lines.append( "\n" );

            lines.append( QString( "%1. %2" ).arg( m.printerType() + 1 ).arg( dim ) );
        }

        labelStock_->setText( lines );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DomCurrentPLU *PluInfoScreen::currentProduct() const
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
QString PluInfoScreen::makeLabelStockDimensions( const QString& width, const QString& height, bool wrapping ) const
{
    UiUtilities *util = session_->uiUtilities;

    QString result;
    QString labelUnits( isScaleMetric() ? "mm" : "\"" );

    // get stock width and height
    QString labelWidth( width );
    util->youGottaSetTheSeparator( labelWidth );

    QString labelHeight( height );

    bool isZero = false;
    int int_width = labelWidth.toInt();
    float float_width = labelWidth.toFloat(&isZero);
    qDebug() << "PluInfoScreen::makeLabelStockDimensions():  width = " << width << ". Conversion:  isZero = " << isZero << ", int_width = " << int_width << ", float_width = " << float_width;

    // set the label text
    if ( float_width == 0 )
        return result;

    result = tr( "Continuous" );
    if ( "continuous" != height.toLower() )
    {
        QString sep( " x " );

        if (( isScaleMetric() ) && ( wrapping ))
            sep.append( "\n" );

        util->youGottaSetTheSeparator( labelHeight );
        result = labelWidth + labelUnits + sep + labelHeight + labelUnits;
    }

    return result;
}
