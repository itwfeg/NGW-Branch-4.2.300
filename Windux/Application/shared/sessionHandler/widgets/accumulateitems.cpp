/**
 * @file accumulateitems.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "accumulateitems.h"
#include "backgroundwidgettypes.h"
#include "dynamiccontentwidget.h"
#include "pushbutton.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"
#include "../sessioninfo.h"

#include "../modes/operatemode.h"

#include <palette.h>

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
accumulateItemsWidget::accumulateItemsWidget( QWidget *parent ) :
    _Mybase( parent ),
    session_( NULL ),
    operateMode_( NULL ),
    needToPrintPrintAccumulatedItems_( false ),
    alreadyHereSoGetOut( false ),
    accumulatedItemsCount_( 0 )
{
    LOG_CLASS_INFO( "accumulateItemsWidget", "accumulateItemsWidget" );

    setObjectName( "hobartAccumulateItemsWidget" );

    // hook into session
    sessionInfo info( this );
    SessionMainWindow *mainWindow = info();

    if ( !mainWindow )
        LOG_WARN( "could not retrieve main window!", NULL );
    else if ( !(session_ = mainWindow->session()) )
        LOG_WARN( "bad session pointer", NULL );
    else
    {
        // hook into session
        connect( session_, SIGNAL( receive(SessionMessage) ), SLOT( onSessionMessageReceiveInternal(SessionMessage) ) );
    }

    // initialize
    initialize();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
accumulateItemsWidget::~accumulateItemsWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void accumulateItemsWidget::translate()
{
    // table headers
    QStringList tableHeaders;
    tableHeaders.push_back( tr( "PLU" ) );
    tableHeaders.push_back( tr( "Description" ) );
    tableHeaders.push_back( tr( "By Count/\nNet Wgt" ) );
    tableHeaders.push_back( tr( "Unit\nPrice" ) );
    tableHeaders.push_back( tr( "Total\nPrice" ) );
    tableHeaders.push_back( tr( "Key" ) );

    table_->setHeaderLabels( tableHeaders );

    // summary
    numberOfEntriesLabel_->setText( tr( "Entries:" ) );
    totalLabel_->setText( tr( "Total:" ) );

    // buttons
    updateButtons();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void accumulateItemsWidget::updateList()
{
    LOG_CLASS_INFO( "accumulateItemsWidget", "updateList" );

    if( alreadyHereSoGetOut )
    {
        // I think I am getting in here twice so prevent it
        // LOG_DEBUG( "Already in updateList() so get out", NULL );
        return;
    }

    alreadyHereSoGetOut = true;

    // nothing to do
    if ( !isVisible() )
    {
        alreadyHereSoGetOut = false;
        return;
    }

    if ( !session_ )
        LOG_WARN( "bad session pointer", NULL );
    else
    {
        LOG_DEBUG( "requesting accumulated items...", NULL );
        session_->send( ProductDev, ProductRequestReadAllAccumulatedItems );
    }

    alreadyHereSoGetOut = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void accumulateItemsWidget::onSessionMessageReceive( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "accumulateItemsWidget", "onSessionMessageReceive" );

    // sanity check
    if ( !session_ )
    {
        LOG_WARN( "bad session pointer", NULL );
        return;
    }

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // accumulated items
    if (( ProductDev == device ) && ( ProductWriteAllAccumulatedItems == event ))
    {        
        DomAccumulatedItems m( msg );

        ScaleConfiguration *scaleConfig( session_->scaleConfig );
        MadpCurrencySettings *currencySettings( &scaleConfig->currencySettings );
        QString currencySymbol( currencySettings->priCurrSymbol() );

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
        int count = qMin( m.numberLineNums(), listLineNums.count() );
        count = qMin( count, listPluNumbers.count() );
        count = qMin( count, listPluDescs.count() );
        count = qMin( count, listByCountWeights.count() );
        count = qMin( count, listUnitPrices.count() );
        count = qMin( count, listTotalPrices.count() );

        LOG_DEBUG( "have %d entries", count );

        // update number of entries
        numberOfEntries_->setText( QString::number( count ) );

        numberOfEntriesLabel_->setVisible( count );
        numberOfEntries_->setVisible( count );

        // clear table
        table_->clear();

        if( 0 == count )
        {
            // send a message to the application to clear the printing list
            session_->send( ProductDev, ProductClearPrintingProductsList );
        }

        // update table
        for ( int i = 0; i < count; ++i )
        {
            QTreeWidgetItem *item;
            QStringList rows;

            // item data
            rows.push_back( listPluNumbers[i] );
            rows.push_back( session_->uiUtilities->stripFontChanges( listPluDescs[i] ) );
            rows.push_back( session_->uiUtilities->youGottaSetTheSeparator( listByCountWeights[i] ) );
            rows.push_back( session_->uiUtilities->youGottaSetTheSeparator( listUnitPrices[i] ) );
            rows.push_back( session_->uiUtilities->youGottaSetTheSeparator( listTotalPrices[i] ) );
            rows.push_back( listLineNums[i] );

            // create widget item
            item = new QTreeWidgetItem( rows );
            item->setSizeHint( 0, QSize( 32, 32 ) );

            LOG_DEBUG( "adding item with key '%s' to table", qPrintable( listLineNums[i] ) );
            table_->addTopLevelItem( item );
        }

        for ( int j = 0; j < table_->columnCount(); j++ )
            table_->resizeColumnToContents(j);

        // check flag to print on update
        if ( needToPrintPrintAccumulatedItems_ )
        {
            // do not issue print unless item was added... the button they pressed was labeled
            // "add item and print" after all...
            if ( accumulatedItemsCount_ < table_->topLevelItemCount() )
            {
                needToPrintPrintAccumulatedItems_ = false;
                session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( Action_PrintAccumulatedItems ) );
            }
        }

        // refresh buttons
        updateButtons();
    }

    // product change
    else if ((( ProductDev == device ) && ( ProductWrite == event )) ||
        (( ProductDev == device ) && ( ProductClearUI == event )))
    {
        ScaleConfiguration *config = session_->scaleConfig;
        if( config->internalConfig.ssactive_lock )
            return;

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
void accumulateItemsWidget::onButtonClicked( QWidget *button )
{
    LOG_CLASS_INFO( "accumulateItemsWidget", "onButtonClicked" );

    if ( !button )
        return;

    // reset flag to print on update
    needToPrintPrintAccumulatedItems_ = false;

    // add item
    // add item and print
    if (( addItem_ == button ) || ( addItemPrint_ == button ))
    {
        // FT-997
        if ( operateMode_ )
            operateMode_->setFocusForPopupNumberPad();

        LOG_INFO( "adding item to accumulated items list...", NULL );
        session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( Action_AddItemToAccumulatedItems ) );

        // set flag to print on update
        needToPrintPrintAccumulatedItems_ |= (addItemPrint_ == button);
        accumulatedItemsCount_ = table_->topLevelItemCount();

        LOG_DEBUG( "need to print is %d", needToPrintPrintAccumulatedItems_ );
    }

    // remove item
    else if ( removeItem_ == button )
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

                list += item->data( 5, 0 ).toString();
            }

            LOG_INFO( "removing items '%s' from list...", qPrintable( list ) );
            session_->send( ProductDev, ProductRemoveAccumulatedItems,
                "\"" + QString::number( items.count() ) + ",\"\"" + list + "\"\"\"" );
        }
    }

    // remove all items
    else if ( removeAllItems_ == button )
    {
        LOG_INFO( "removing all items...", NULL );
        session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( Action_RemoveAllAccumulatedItems ) );
    }

    // hide button
    else if ( hide_ == button )
    {
        for ( QWidget *w( parentWidget() ); w; w = w->parentWidget() )
        {
            dynamicContentWidget *dca( dynamic_cast<dynamicContentWidget*>( w ) );

            if (( dca ) && ( dca->isAccumulateItems() ))
            {
                dca->hideSpecialWidget();
                break;
            }
        }

        return;
    }

    // request data
    updateList();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void accumulateItemsWidget::onTableItemSelectionChanged()
{
    updateButtons();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void accumulateItemsWidget::onSessionMessageReceiveInternal( const SessionMessage& msg )
{
    onSessionMessageReceive( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void accumulateItemsWidget::resizeEvent( QResizeEvent *e )
{
    LOG_CLASS_INFO( "accumulateItemsWidget", "resizeEvent" );

    _Mybase::resizeEvent( e );

    LOG_DEBUG( "height %d width %d", height(), width() );

    const hobart::backgroundWidgetRenderer renderBackground(
        new hobart::rectangleBackgroundWidget(),
        new hobart::lightOperatorBackgroundWidgetRendererAppearance() );

    // set palette
    modifyPaletteBackground setBackgroundPalette( renderBackground.drawControl( size(), hobart::ReadOnly ) );
    setBackgroundPalette( this );

    setAutoFillBackground( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void accumulateItemsWidget::showEvent( QShowEvent *e )
{
    LOG_CLASS_INFO( "accumulateItemsWidget", "showEvent" );

    // hook into operate mode
    if ( !operateMode_ )
    {
        // hook into session
        sessionInfo info( this );
        SessionMainWindow *mainWindow = info();

        if ( !mainWindow )
            LOG_WARN( "could not retrieve main window!", NULL );
        else if ( !(operateMode_ = mainWindow->retrieveMode<OperateMode>()) )
            LOG_WARN( "operate mode does not exist", NULL );
        else
        {
            // refresh list when product changes
            DomCurrentPLU *product( operateMode_->currentProduct() );

            if ( ( product ) && ( product->itemNumber().toInt() == 0 ))
                connect( operateMode_, SIGNAL( currentProductChanged(bool) ), SLOT( updateList() ) );
        }
    }

    // request data
    updateList();
    
    // show the hide button
    hide_->move( width() - hide_->width(), 0 );
    hide_->raise();

    if ( session_ )
    {
        const bool visible( session_->scaleConfig->accumulateItemsConfig.enableAccumItemsAndFlashkeys() );

        hideSpacer_->setVisible( visible );
        hide_->setVisible( visible );

        // for initial boot we need to force hide/show to make button visible
        if (( visible ) && ( !session_->scaleConfig->internalConfig.scaleBootComplete ))
        {
            QTimer *t( new QTimer( this ) );

            connect( t, SIGNAL(timeout()), SLOT(hide()) );
            connect( t, SIGNAL(timeout()), SLOT(show()) );
            connect( t, SIGNAL(timeout()), t, SLOT(deleteLater()) );

            t->setSingleShot( true );
            t->start( 0 );
        }
    }

    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void accumulateItemsWidget::initialize()
{
    static const int spacing = 7;

    const hobart::backgroundWidgetRenderer renderButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::darkButtonBackgroundWidgetRendererAppearance() );

    const QPixmap buttonPixmap( renderButton.drawControl( QSize( 70, 50 ), hobart::ReadOnly ) );

    modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );

    modifyPaletteHighlight setHighlight( hobart::colorPalette::blue );
    modifyPaletteHighlightText setHighlightText( hobart::colorPalette::white );

    // buttons signal mapper
    buttons_ = new QSignalMapper( this );

    connect( buttons_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    // table headers
    QStringList tableHeaders;
    tableHeaders.push_back( "" ); // plu
    tableHeaders.push_back( "" ); // description
    tableHeaders.push_back( "" ); // by count/net weight
    tableHeaders.push_back( "" ); // unit price
    tableHeaders.push_back( "" ); // total price
    tableHeaders.push_back( "" ); // key

    // table
    table_ = new QTreeWidget( this );
    table_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    table_->setHeaderLabels( tableHeaders );
    table_->setObjectName( "table" );
    table_->setRootIsDecorated( false );
    table_->setSelectionMode( QAbstractItemView::MultiSelection );
    table_->setUniformRowHeights( true );
    table_->setFocusPolicy( Qt::NoFocus );
    setHighlight( table_ );
    setHighlightText( table_ );

    table_->setColumnCount( tableHeaders.size() );
    table_->setColumnWidth( 0, 48 );
    table_->setColumnWidth( 1, 218 );
    table_->setColumnWidth( 2, 64 );
    table_->setColumnWidth( 3, 64 );
    table_->setColumnWidth( 4, 96 );
    table_->setColumnWidth( 5, 0 );
    table_->setColumnHidden( 5, true );

    connect( table_, SIGNAL( itemSelectionChanged() ), SLOT( onTableItemSelectionChanged() ) );

    // number of entries
    numberOfEntriesLabel_ = new QLabel( this );
    numberOfEntriesLabel_->setVisible( false );
    numberOfEntriesLabel_->setObjectName( "numberOfEntriesLabel" );

    numberOfEntries_ = new QLabel( this );
    numberOfEntries_->setVisible( false );
    numberOfEntries_->setObjectName( "numberOfEntries" );

    // total
    totalLabel_ = new QLabel( this );
    totalLabel_->setObjectName( "totalLabel" );

    total_ = new QLabel( this );
    total_->setObjectName( "total" );

    // add item button
    addItem_ = new hobart::pushButton( this, buttonPixmap );
    addItem_->setEnabled( false );
    addItem_->setFocusPolicy( Qt::NoFocus );
    addItem_->setObjectName( "addItem" );
    setForeground( addItem_ );

    connect( addItem_, SIGNAL( clicked() ), buttons_, SLOT( map() ) );
    buttons_->setMapping( addItem_, addItem_ );

    // add item and print button
    addItemPrint_ = new hobart::pushButton( this, buttonPixmap );
    addItemPrint_->setEnabled( false );
    addItemPrint_->setFocusPolicy( Qt::NoFocus );
    addItemPrint_->setObjectName( "addItemPrint" );
    setForeground( addItemPrint_ );

    connect( addItemPrint_, SIGNAL( clicked() ), buttons_, SLOT( map() ) );
    buttons_->setMapping( addItemPrint_, addItemPrint_ );

    // remove item button
    removeItem_ = new hobart::pushButton( this, buttonPixmap );
    removeItem_->setEnabled( false );
    removeItem_->setFocusPolicy( Qt::NoFocus );
    removeItem_->setObjectName( "removeItem" );
    setForeground( removeItem_ );

    connect( removeItem_, SIGNAL( clicked() ), buttons_, SLOT( map() ) );
    buttons_->setMapping( removeItem_, removeItem_ );

    // remove all items button
    removeAllItems_ = new hobart::pushButton( this, buttonPixmap );
    removeAllItems_->setEnabled( false );
    removeAllItems_->setFocusPolicy( Qt::NoFocus );
    removeAllItems_->setObjectName( "removeAllItems" );
    setForeground( removeAllItems_ );

    connect( removeAllItems_, SIGNAL( clicked() ), buttons_, SLOT( map() ) );
    buttons_->setMapping( removeAllItems_, removeAllItems_ );

    // hide spacer
    hideSpacer_ = new QLabel( " ", this );
    hideSpacer_->setFixedHeight( 20 );
    hideSpacer_->setObjectName( "hideSpacer" );
    hideSpacer_->setVisible( false );

    // hide button
    hide_ = new hobart::pushButton( "X", this, exitButtonPixmap( QSize( 64, 32 ) ) );

    QFont hideButtonFont( QApplication::font() );
    hideButtonFont.setPointSize( 16 );

    hide_->setFocusPolicy( Qt::NoFocus );
    hide_->setFont( hideButtonFont );
    hide_->setObjectName( "hide" );
    hide_->setVisible( false );
    setForeground( hide_ );

    connect( hide_, SIGNAL( clicked() ), buttons_, SLOT( map() ) );
    buttons_->setMapping( hide_, hide_ );

    // setup layouts
    QHBoxLayout *screenLayout = new QHBoxLayout( this );
    screenLayout->setSpacing( spacing );

    QHBoxLayout *summary = new QHBoxLayout();
    summary->addWidget( numberOfEntriesLabel_ );
    summary->addWidget( numberOfEntries_ );
    summary->addStretch();
    summary->addWidget( totalLabel_ );
    summary->addWidget( total_ );

    QVBoxLayout *left = new QVBoxLayout();
    left->addWidget( table_ );
    left->addLayout( summary );

    QVBoxLayout *right = new QVBoxLayout();
    right->addWidget( hideSpacer_ );
    right->addStretch();
    right->addWidget( addItem_ );
    right->addStretch();
    right->addWidget( addItemPrint_ );
    right->addStretch();
    right->addWidget( removeItem_ );
    right->addStretch();
    right->addWidget( removeAllItems_ );
    right->addStretch();

    screenLayout->addLayout( left );
    screenLayout->addLayout( right );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool accumulateItemsWidget::isCurrentProduct() const
{
    if ( operateMode_ )
    {
        DomCurrentPLU *product( operateMode_->currentProduct() );

        return (( product ) && ( product->itemNumber().length() ));
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void accumulateItemsWidget::updateButtons()
{
    QList<QTreeWidgetItem*> items( table_->selectedItems() );

    bool haveProduct = isCurrentProduct();

    bool haveItems = table_->topLevelItemCount();
    bool haveSelection = items.count();

    // add buttons
    addItem_->setText( tr( "Add Item" ) );
    addItemPrint_->setText( tr( "Add Item\nand Print" ) );

    addItem_->setEnabled( haveProduct );
    addItemPrint_->setEnabled( haveProduct );

    // remove buttons
    removeItem_->setText( haveSelection ? tr( "Remove\nSelected" ) : tr( "Remove\nLast Item" ) );
    removeAllItems_->setText( tr( "Remove\nAll Items" ) );

    removeItem_->setEnabled( haveSelection || haveItems );
    removeAllItems_->setEnabled( haveItems );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPixmap accumulateItemsWidget::exitButtonPixmap( const QSize &size )
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
} // hobart namespace
