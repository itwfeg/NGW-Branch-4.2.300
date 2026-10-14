/**
 * @file buildamealwidget.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "backgroundwidgettypes.h"
#include "buildamealwidget.h"
#include "pushbutton.h"

#include "../sessioninfo.h"
#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../modes/operatemode.h"

#include <font.h>
#include <palette.h>
#include <pixmap.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>
#include <QTimer>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
mealBuilderWidget::mealBuilderWidget( QWidget *parent ) :
    _Mybase( parent ),
    session_( NULL ),
    operateMode_( NULL ),
    scaleConfig_( NULL ),
    currencySettings_( NULL ),
    accumulatedItemsCount_( 0 ),
    sidesRemaining_( 0 )
{
    LOG_CLASS_INFO( "mealBuilderWidget", "mealBuilderWidget" );

    setAutoFillBackground( true );
    setObjectName( "mealBuilderWidget" );

    // hook into session
    sessionInfo info( this );

    if ( !(mainWindow_ = info()) )
        LOG_WARN( "could not retrieve main window!", NULL );
    else if ( !(session_ = mainWindow_->session()) )
        LOG_WARN( "bad session pointer", NULL );
    else
    {
        // hook into session
        connect( session_, SIGNAL( receive(SessionMessage) ), SLOT( onSessionMessageReceiveInternal(SessionMessage) ) );

        // retrieve config
        scaleConfig_ = session_->scaleConfig;
        currencySettings_ = &scaleConfig_->currencySettings;
    }

    // initialize
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mealBuilderWidget::~mealBuilderWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mealBuilderWidget::translate()
{
    // table headers
    QStringList tableHeaders;
    tableHeaders.push_back( tr( "Key" ) );
    tableHeaders.push_back( tr( "Description" ) );
    tableHeaders.push_back( tr( "Price" ) );

    table_->setHeaderLabels( tableHeaders );

    // summary
    numberOfSidesRemainingLabel_->setText( tr( "Sides Remaining:" ) );
    totalLabel_->setText( tr( "Total:" ) );

    // buttons
    updateButtons();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mealBuilderWidget::updateList()
{
    LOG_CLASS_INFO( "mealBuilderWidget", "updateList" );

    // nothing to do
    if ( !isVisible() )
        return;

    if ( !session_ )
        LOG_WARN( "bad session pointer", NULL );
    else
    {
        LOG_DEBUG( "requesting accumulated items...", NULL );
        session_->send( ProductDev, ProductRequestReadAllAccumulatedItems );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mealBuilderWidget::resizeEvent( QResizeEvent *e )
{
    LOG_CLASS_INFO( "mealBuilderWidget", "resizeEvent" );

    LOG_DEBUG( "height %d width %d", height(), width() );

    // table column widths
    table_->setColumnHidden( 0, true );
    table_->setColumnWidth( 1, (80 * table_->width()) / 100 );
    table_->setColumnWidth( 2, (20 * table_->width()) / 100 );

    // widget background
    const backgroundWidgetRenderer renderer(
        new hobart::rectangleBackgroundWidget(),
        new hobart::lightOperatorBackgroundWidgetRendererAppearance() );

    const QPixmap background( renderer.drawControl( size(), hobart::ReadOnly ) );

    // button background
    const QSize buttonSize( (30 * width()) / 100, 50 );

    const backgroundWidgetRenderer buttonRenderer(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::darkButtonBackgroundWidgetRendererAppearance );

    const QPixmap buttonBackground( buttonRenderer.drawControl( buttonSize, hobart::ReadOnly ) );
    const QPixmap buttonBackgroundPressed( invertPixmap()( buttonBackground ) );

    // set palette
    modifyPaletteBackground setBackgroundPalette( background );
    setBackgroundPalette( this );

    // set button palette
    QList<hobart::pushButton*> buttons;
    buttons.append( removeAllItems_ );
    buttons.append( removeItem_ );
    buttons.append( print_ );

    foreach ( hobart::pushButton *button, buttons )
    {
        button->setFixedSize( buttonSize );
        button->setBackground( buttonBackground );
        button->setBackgroundPressed( buttonBackgroundPressed );
    }

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mealBuilderWidget::showEvent( QShowEvent *e )
{
    // hook into operate mode
    if ( !operateMode_ )
        updateMode();

    // request data
    updateList();

    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mealBuilderWidget::onSessionMessageReceive( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "mealBuilderWidget", "onSessionMessageReceive" );

    // sanity check
    if ( !session_ )
    {
        LOG_WARN( "bad session pointer", NULL );
        return;
    }

    // make sure we have a mode
    if ( !operateMode_ )
    {
        updateMode();

        if ( !operateMode_ )
            return;
    }

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // accumulated items
    if (( ProductDev == device ) && ( ProductWriteAllAccumulatedItems == event ))
    {
        const QString currencySymbol( currencySettings_->priCurrSymbol() );

        DomCurrentPLU *item( operateMode_->currentProduct() );
        DomAccumulatedItems m( msg );

        // not a build-a-meal item
        if (( !item ) || ( item->sideItemsGroupNum().toInt() <= 0 ))
        {
            numberOfSidesRemaining_->clear();
            total_->clear();

            table_->clear();

            removeAllItems_->setEnabled( false );
            removeItem_->setEnabled( false );
            print_->setEnabled( false );
        }

        else
        {
            const int numSides( item->sideItemsIncludedInMeal().toInt() );

            // update total
            total_->setText( currencySymbol + session_->uiUtilities->youGottaSetTheSeparator( m.totalPrice() ) );

            // parse data
            QStringList listLineNums( m.field<QStringList>( m.listLineNums() ) );
            QStringList listPluNumbers( m.field<QStringList>( m.listPluNumbers() ) );
            QStringList listPluDescs( m.field<QStringList>( m.listPluDescs() ) );
            QStringList listByCountWeights( m.field<QStringList>( m.listByCountWeights() ) );
            QStringList listUnitPrices( m.field<QStringList>( m.listUnitPrices() ) );
            QStringList listTotalPrices( m.field<QStringList>( m.listTotalPrices() ) );

            if ( m.numberLineNums() != listLineNums.count() )
                LOG_WARN( "mismatch in line nums %d %d", m.numberLineNums(), listLineNums.count() );

            if ( m.numberPluNumbers() != listPluNumbers.count() )
                LOG_WARN( "mismatch in plu numbers %d %d", m.numberPluNumbers(), listPluNumbers.count() );

            if ( m.numberPluDescs() != listPluDescs.count() )
                LOG_WARN( "mismatch in plu descs %d %d", m.numberPluDescs(), listPluDescs.count() );

            if ( m.numberByCountWeights() != listByCountWeights.count() )
                LOG_WARN( "mismatch in by count weights %d %d", m.numberByCountWeights(), listByCountWeights.count() );

            if ( m.numberUnitPrices() != listUnitPrices.count() )
                LOG_WARN( "mismatch in unit prices %d %d", m.numberUnitPrices(), listUnitPrices.count() );

            if ( m.numberTotalPrices() != listTotalPrices.count() )
                LOG_WARN( "mismatch in total prices %d %d", m.numberTotalPrices(), listTotalPrices.count() );

            // compute count of line items
            accumulatedItemsCount_ = qMin( m.numberLineNums(), listLineNums.count() );
            accumulatedItemsCount_ = qMin( accumulatedItemsCount_, listPluNumbers.count() );
            accumulatedItemsCount_ = qMin( accumulatedItemsCount_, listPluDescs.count() );
            accumulatedItemsCount_ = qMin( accumulatedItemsCount_, listByCountWeights.count() );
            accumulatedItemsCount_ = qMin( accumulatedItemsCount_, listUnitPrices.count() );
            accumulatedItemsCount_ = qMin( accumulatedItemsCount_, listTotalPrices.count() );

            LOG_DEBUG( "have %d entries", accumulatedItemsCount_ );

            // update number of sides
            numberOfSidesRemainingLabel_->setVisible( 0 < numSides );
            numberOfSidesRemaining_->setVisible( 0 < numSides );

            numberOfSidesRemaining_->setText( QString::number( qMax( 0, numSides - (accumulatedItemsCount_ - 1) ) ) );

            // clear table
            table_->clear();

            // update table
            for ( int i = 0; i < accumulatedItemsCount_; ++i )
            {
                QTreeWidgetItem *item;
                QStringList rows;

                // item data
                rows.push_back( listLineNums[i] );
                rows.push_back( session_->uiUtilities->stripFontChanges( listPluDescs[i] ) );
                rows.push_back( session_->uiUtilities->youGottaSetTheSeparator( listTotalPrices[i] ) );

                // create widget item
                item = new QTreeWidgetItem( rows );
                item->setSizeHint( 1, QSize( 32, 32 ) );

                LOG_DEBUG( "adding item with key '%s' to table", qPrintable( listLineNums[i] ) );
                table_->addTopLevelItem( item );
            }

            // refresh buttons
            updateButtons();
        }
    }

    // product change
    else if ((( ProductDev == device ) && ( ProductWrite == event )) ||
        (( ProductDev == device ) && ( ProductClearUI == event )))
    {
        ScaleConfiguration *config = session_->scaleConfig;
        if( config->internalConfig.ssactive_lock )
            return;

        // clear list when no product
        if ( ProductClearUI == event )
            table_->clear();

        // refresh ui
        updateButtons();
    }

    // printer error
    else if (( Error == device ) && ( ErrPrinterDevice == event ))
    {
        // Who knows what state we were in when the error occured... we could be printing the item,
        // or perhaps the total label, or whatever. Lets try to get back in sync with the app...

        // request data
        updateList();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mealBuilderWidget::onButtonClicked()
{
    LOG_CLASS_INFO( "mealBuilderWidget", "onButtonClicked" );

    // remove all items
    if ( removeAllItems_ == sender() )
    {
        LOG_INFO( "removing all items...", NULL );
        session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( Action_RemoveAllAccumulatedItems ) );
    }

    // remove item
    else if ( removeItem_ == sender() )
    {
        QList<QTreeWidgetItem*> items( table_->selectedItems() );

        LOG_DEBUG( "remove item clicked (count %d)", items.count() );

        // when nothing selected send remove last item command
        if ( !items.count() )
        {
            LOG_INFO( "removing last item from list...", NULL );
            session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( Action_RemoveLastItemFromAccumulatedItems ) );
        }

        // otherwise remove selected ones
        else
        {
            QString list;

            // create list of keys
            foreach ( const QTreeWidgetItem *item, items )
            {
                if ( list.length() )
                    list += ",";

                list += item->data( 0, 0 ).toString();
            }

            LOG_INFO( "removing items '%s' from list...", qPrintable( list ) );
            session_->send( ProductDev, ProductRemoveAccumulatedItems,
                "\"" + QString::number( items.count() ) + ",\"\"" + list + "\"\"\"" );
        }
    }

    // print
    else if ( print_ == sender() )
    {
        if ( mainWindow_ )
            mainWindow_->printKeyPressed();
    }

    // request data
    updateList();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mealBuilderWidget::onTableItemSelectionChanged()
{
    updateButtons();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mealBuilderWidget::onSessionMessageReceiveInternal( const SessionMessage& msg )
{
    onSessionMessageReceive( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mealBuilderWidget::initialize()
{
    modifyFontPointSize setFontPointSize12( (window()->height() * 12) / 600 );

    modifyPaletteForeground setForegroundBlack( modifyPaletteForeground::createPalette( hobart::colorPalette::black ) );
    modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );

    modifyPaletteHighlight setHighlight( hobart::colorPalette::blue );
    modifyPaletteHighlightText setHighlightText( hobart::colorPalette::white );

    setForegroundBlack( this );

    // number of entries
    numberOfSidesRemainingLabel_ = new QLabel( this );
    numberOfSidesRemainingLabel_->setVisible( false );
    numberOfSidesRemainingLabel_->setObjectName( "numberOfSidesRemainingLabel" );
    setFontPointSize12( numberOfSidesRemainingLabel_ );

    numberOfSidesRemaining_ = new QLabel( this );
    numberOfSidesRemaining_->setVisible( false );
    numberOfSidesRemaining_->setObjectName( "numberOfSidesRemaining" );
    setFontPointSize12( numberOfSidesRemaining_ );

    // table headers
    QStringList tableHeaders;
    tableHeaders.push_back( "" ); // key
    tableHeaders.push_back( "" ); // description
    tableHeaders.push_back( "" ); // total price

    // table
    table_ = new QTreeWidget( this );
    table_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    table_->setObjectName( "table" );
    table_->setRootIsDecorated( false );
    table_->setSelectionMode( QAbstractItemView::MultiSelection );
    table_->setUniformRowHeights( true );
    table_->setFocusPolicy( Qt::NoFocus );
    table_->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setHighlight( table_ );
    setHighlightText( table_ );

    table_->setColumnCount( tableHeaders.size() );
    table_->setHeaderLabels( tableHeaders );

    connect( table_, SIGNAL( itemSelectionChanged() ), SLOT( onTableItemSelectionChanged() ) );

    // total
    totalLabel_ = new QLabel( this );
    totalLabel_->setObjectName( "totalLabel" );

    total_ = new QLabel( this );
    total_->setObjectName( "total" );

    // remove all items button
    removeAllItems_ = new hobart::pushButton( this );
    removeAllItems_->setEnabled( false );
    removeAllItems_->setFocusPolicy( Qt::NoFocus );
    removeAllItems_->setObjectName( "removeAllItems" );
    setForeground( removeAllItems_ );

    connect( removeAllItems_, SIGNAL( clicked() ), SLOT( onButtonClicked() ) );

    // remove item button
    removeItem_ = new hobart::pushButton( this );
    removeItem_->setEnabled( false );
    removeItem_->setFocusPolicy( Qt::NoFocus );
    removeItem_->setObjectName( "removeItem" );
    setForeground( removeItem_ );

    connect( removeItem_, SIGNAL( clicked() ), SLOT( onButtonClicked() ) );

    // print button
    print_ = new hobart::pushButton( this );
    print_->setEnabled( false );
    print_->setFocusPolicy( Qt::NoFocus );
    print_->setObjectName( "print" );
    setForeground( print_ );

    connect( print_, SIGNAL( clicked() ), SLOT( onButtonClicked() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mealBuilderWidget::createLayout()
{
    QHBoxLayout *remaining( new QHBoxLayout() );
    remaining->addWidget( numberOfSidesRemainingLabel_ );
    remaining->addWidget( numberOfSidesRemaining_ );
    remaining->addStretch();

    QHBoxLayout *total( new QHBoxLayout() );
    total->addStretch();
    total->addWidget( totalLabel_ );
    total->addWidget( total_ );

    QHBoxLayout *buttons( new QHBoxLayout() );
    buttons->addWidget( removeAllItems_ );
    buttons->addStretch();
    buttons->addWidget( removeItem_ );
    buttons->addStretch();
    buttons->addWidget( print_ );

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->addLayout( remaining );
    screenLayout->addWidget( table_, 1 );
    screenLayout->addLayout( total );
    screenLayout->addLayout( buttons );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mealBuilderWidget::updateButtons()
{
    QList<QTreeWidgetItem*> items( table_->selectedItems() );

    bool haveItems( table_->topLevelItemCount() );
    bool haveSelection( items.count() );

    // remove buttons
    removeAllItems_->setText( tr( "Remove\nAll Items" ) );
    removeItem_->setText( haveSelection ? tr( "Remove\nSelected" ) : tr( "Remove\nLast Item" ) );

    removeAllItems_->setEnabled( haveItems );
    removeItem_->setEnabled( haveSelection || haveItems );

    // print button
    print_->setText( tr( "Print" ) );

    print_->setEnabled( haveItems );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mealBuilderWidget::updateMode()
{
    LOG_CLASS_INFO( "mealBuilderWidget", "updateMode" );

    if ( !mainWindow_ )
        LOG_WARN( "could not retrieve main window!", NULL );
    else if ( !(operateMode_ = mainWindow_->retrieveMode<OperateMode>()) )
        LOG_WARN( "operate mode does not exist", NULL );
    else
    {
        // refresh list when product changes
        connect( operateMode_, SIGNAL( currentProductChanged(bool) ), SLOT( updateList() ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
