/**
 * @file cooltextpanel.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "cooltextpanel.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../modes/operatemode.h"

#include <backgroundwidgettypes.h>
#include <palette.h>
#include <pushbutton.h>

#include <Messages.h>
#include <ScaleConfiguration.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QVBoxLayout>

static const int offsetCoolTextNum = 79;

///////////////////////////////////////////////////////////////////////////////////////////////////
CoolTextPanel::CoolTextPanel( SessionMainWindow *mainWindow, SessionDialog *parent ) :
    _Mybase( mainWindow, parent ),
    coolTextNum_( 0 )
{
    setObjectName( "CoolTextPanel" );

    // initialize
    initialize();

    // create new cool text
    session_->send( COOLDev, COOLDevCreateNewCoolText );

    // compute cool category
    int coolCategoryNum = 0;

    // need a product for create cool button to show
    if ( mainWindow_->isOperateMode() )
    {
        DomCurrentPLU *product( mainWindow_->retrieveMode<OperateMode>()->currentProduct() );

        // valid product with item number
        if ( product )
            coolCategoryNum = product->coolCategoryNum();
    }

    // retrieve countries
    session_->send( COOLDev, COOLDevReadByAllAndByCategoryCountries, QString::number( coolCategoryNum ) );

    // retrieve production steps
    session_->send( COOLDev, COOLDevReadByAllAndByCategoryProductionSteps, QString::number( coolCategoryNum ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CoolTextPanel::~CoolTextPanel()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CoolTextPanel::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "CoolTextPanel", "onSessionMessage" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // cool text number
    if (( COOLDev == device ) && ( COOLDevCreateNewCoolText == event ))
    {
        DomCoolTextFormNewCool m( msg );

        // save off record number
        coolTextNum_ = m.recordNum();

        LOG_DEBUG( "cool text num %d", coolTextNum_ );
    }
    // cool countries
    else if (( COOLDev == device ) && ( COOLDevWriteCountriesByCategory == event ))
    {
        DomCoolCountryCatList m( msg );

        QStringList list( m.field<QStringList>( m.names() ) );

        // verify list size
        if ( list.count() != m.numOfItems() )
            LOG_WARN( "mismatch in countries list size %d %d", list.count(), m.numOfItems() );
        else if ( !list.count() )
            LOG_WARN( "no countries to process", NULL );
        else
        {
            foreach ( const QString& t, list )
            {
                QListWidgetItem *item = new QListWidgetItem( t );
                item->setSizeHint( QSize( 100, 32 ) );

                countries_->addItem( item );
            }
        }
    }
    // cool production steps
    else if (( COOLDev == device ) && ( COOLDevWriteProductionStepsByCategory == event ))
    {
        DomCoolProdStepCatList m( msg );

        QStringList list( m.field<QStringList>( m.names() ) );

        // verify list size
        if ( list.count() != m.numOfItems() )
            LOG_WARN( "mismatch in production steps list size %d %d", list.count(), m.numOfItems() );
        else if ( !list.count() )
            LOG_WARN( "no production steps to process", NULL );
        else
        {
            foreach ( const QString& t, list )
            {
                QListWidgetItem *item = new QListWidgetItem( t );
                item->setSizeHint( QSize( 100, 32 ) );

                productionSteps_->addItem( item );
            }
        }
    }

    // update buttons
    updateButtons();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CoolTextPanel::onClearButtonClicked()
{
    text_->clear();

    updateButtons();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CoolTextPanel::onCreateButtonClicked()
{
    LOG_CLASS_INFO( "CoolTextPanel", "onCreateButtonClicked" );

    const QString coolTextNum( QString::number( coolTextNum_ ) );

    // 135,106
    // save cool text
    session_->send( COOLDev, COOLDevSaveCoolText, coolTextNum + ",\"" + text_->text() + "\"" );

    // retrieve plu from operate mode
    if ( !mainWindow_->isOperateMode() )
        LOG_WARN( "not in operate mode cannot add cool text to short list", NULL );
    else
    {
        OperateMode *operateMode( mainWindow_->retrieveMode<OperateMode>() );
        DomCurrentPLU *product( operateMode->currentProduct() );

        // validate plu
        if (( !product ) || ( product->itemNumber().isEmpty() ))
            LOG_WARN( "no plu in system cannot add cool text to short list", NULL );
        else
        {
            const QString itemNumber( product->itemNumber() );

            // 135,115
            // add cool text to plu short list
            session_->send( COOLDev, COOLDevShortlistAddText, itemNumber + "," + coolTextNum + ",1" );

            // 101,16
            // write single field to plu record
            session_->send( ProductDev, ProductWriteSingleField,
                itemNumber + "," + QString::number( offsetCoolTextNum ) + ",\"" + coolTextNum + "\",\"" + product->coolTextNumRaw() + "\",0" );

            // 135,113
            // request cool text for operate mode
            session_->send( COOLDev, COOLDevShortlistReadAllCoolText, itemNumber + ",1" );
        }
    }

    // done!
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CoolTextPanel::onListItemClicked( QListWidgetItem *item )
{
    QString text( text_->text() );

    if ( text.length() )
        text.append( " " );

    text.append( item->text() );

    text_->setText( text );

    updateButtons();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CoolTextPanel::initialize()
{
    static const int spacing( 7 );

    const QSize buttonSize(
        (mainWindow_->width() * 100) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 45) / SessionMainWindow::scalingDividerY_ );

    const hobart::backgroundWidgetRenderer renderButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::darkButtonBackgroundWidgetRendererAppearance() );

    const QPixmap buttonPixmap( renderButton.drawControl( buttonSize, hobart::ReadOnly ) );

    modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );

    QHBoxLayout *hlayout;
    QVBoxLayout *vlayout;

    widgetsLayout_->setSpacing( spacing );
    QLayoutItem *doomed = widgetsLayout_->takeAt( 1 );

    if ( doomed )
        delete doomed;

    // set panel title
    setTitle( tr( "Cool Text" ) );

    // "add to shortlist" button
    create_ = new hobart::pushButton( tr( "Add to Shorlist" ), this, buttonPixmap );
    create_->setFocusPolicy( Qt::NoFocus );
    create_->setObjectName( "create" );
    setForeground( create_ );

    // not visible until we get all the cool messages
    create_->setVisible( false );

    topLayout_->addWidget( create_ );

    connect( create_, SIGNAL( clicked() ), SLOT( onCreateButtonClicked() ) );

    // "clear" button
    clear_ = new hobart::pushButton( tr( "Clear" ), this, buttonPixmap );
    clear_->setFocusPolicy( Qt::NoFocus );
    clear_->setObjectName( "clear" );
    setForeground( clear_ );

    topLayout_->addWidget( clear_ );

    connect( clear_, SIGNAL( clicked() ), SLOT( onClearButtonClicked() ) );

    // cool text label
    textLabel_ = new QLabel( tr( "Name" ), this );
    textLabel_->setObjectName( "textLabel" );

    // cool text
    text_ = new QLineEdit( this );
    text_->setFocusPolicy( Qt::NoFocus );
    text_->setObjectName( "text" );
    text_->setReadOnly( true );

    hlayout = new QHBoxLayout();
    hlayout->setContentsMargins( QMargins() );
    hlayout->setSpacing( spacing );
    hlayout->addWidget( textLabel_ );
    hlayout->addWidget( text_ );

    widgetsLayout_->addLayout( hlayout );

    // production steps label
    productionStepsLabel_ = new QLabel( tr( "Production Steps" ), this );
    productionStepsLabel_->setObjectName( "productionStepsLabel" );

    // production steps
    productionSteps_ = new QListWidget( this );
    productionSteps_->setFocusPolicy( Qt::NoFocus );
    productionSteps_->setFont( QApplication::font() );
    productionSteps_->setObjectName( "productionSteps" );
    productionSteps_->setPalette( QApplication::palette() );
    productionSteps_->setSelectionMode( QAbstractItemView::NoSelection );
    productionSteps_->setStyleSheet( "QListWidget::item { border-bottom: 1px solid black; }" );

    connect( productionSteps_, SIGNAL( itemClicked(QListWidgetItem*) ), SLOT( onListItemClicked(QListWidgetItem*) ) );

    // countries label
    countriesLabel_ = new QLabel( tr( "Countries" ), this );
    countriesLabel_->setObjectName( "countriesLabel" );

    // countries
    countries_ = new QListWidget( this );
    countries_->setFocusPolicy( Qt::NoFocus );
    countries_->setFont( QApplication::font() );
    countries_->setObjectName( "countries" );
    countries_->setPalette( QApplication::palette() );
    countries_->setSelectionMode( QAbstractItemView::NoSelection );
    countries_->setStyleSheet( "QListWidget::item { border-bottom: 1px solid black; }" );

    connect( countries_, SIGNAL( itemClicked(QListWidgetItem*) ), SLOT( onListItemClicked(QListWidgetItem*) ) );

    hlayout = new QHBoxLayout();
    hlayout->setContentsMargins( QMargins() );
    hlayout->setSpacing( 0 );

    vlayout = new QVBoxLayout();
    vlayout->setContentsMargins( QMargins() );
    vlayout->setSpacing( spacing );
    vlayout->addWidget( productionStepsLabel_ );
    vlayout->addWidget( productionSteps_ );
    hlayout->addLayout( vlayout );

    vlayout = new QVBoxLayout();
    vlayout->setContentsMargins( QMargins() );
    vlayout->setSpacing( spacing );
    vlayout->addWidget( countriesLabel_ );
    vlayout->addWidget( countries_ );
    hlayout->addLayout( vlayout );

    widgetsLayout_->addLayout( hlayout );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CoolTextPanel::updateButtons()
{
    bool haveData = (( coolTextNum_ ) &&
        ( productionSteps_ ) && ( productionSteps_->count() ) &&
        ( countries_ ) && ( countries_->count() ));

    bool haveText = (( text_ ) && ( text_->text().length() ));

    // create button
    create_->setVisible( haveData );
    create_->setEnabled( haveText );

    // clear button
    clear_->setEnabled( haveText );
}

