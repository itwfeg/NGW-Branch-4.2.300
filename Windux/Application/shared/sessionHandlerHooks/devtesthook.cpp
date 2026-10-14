/**
 * @file devtesthook.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "devtesthook.h"

#include "selfservice/selfservicesoftkeyswidget.h"

#include <QButtonGroup>
#include <QGroupBox>
#include <QDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

///////////////////////////////////////////////////////////////////////////////////////////////////
developmentTestHook::developmentTestHook( Session *s, QObject *parent ) :
    _Mybase( QString( "Randy Testing Hook" ), parent ),
    d_( NULL )
{
    // set pointer to session
    setSession( s );

    // initialize
    initialize();

    // create layouts
    createLayout();

    // show!
    if ( d_ )
        d_->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
developmentTestHook::~developmentTestHook()
{
    if ( d_ )
        d_->deleteLater();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void developmentTestHook::onDismissDialog()
{
    log_->appendPlainText( "dialog DISMISSED" );

    // hide
    error_->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void developmentTestHook::onLoadSoftkeyGroup( const DomLoadSoftkeyGroup* m )
{
    softkeys_->create( m );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void developmentTestHook::onShowDialog( const QString& title, const QString& message, const sessionHooks::buttonMap& buttons )
{
    log_->appendPlainText( "dialog VISIBLE:" );
    log_->appendPlainText( "  " + title );
    log_->appendPlainText( "  " + message );

    for ( sessionHooks::buttonMap::const_iterator i = buttons.begin(); i != buttons.end(); ++i )
        log_->appendPlainText( "    " + i.value() + " (" + QString::number( i.key() ) + ")" );

    // remove existing buttons
    QList<QAbstractButton*> oldButtons( errorButtonsGroup_->buttons() );

    foreach ( QAbstractButton *b, oldButtons )
    {
        b->hide();
        b->deleteLater();
    }

    // create new buttons
    for ( sessionHooks::buttonMap::const_iterator i = buttons.begin(); i != buttons.end(); ++i )
    {
        QPushButton *b = new QPushButton( i.value(), errorButtons_ );

        errorButtonsGroup_->addButton( b, i.key() );
        errorButtonsLayout_->addWidget( b );
    }

    // show everything
    errorTitle_->setText( title );
    errorMessage_->setText( message );
    error_->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void developmentTestHook::onUpdateCurrentItem( itemType type )
{
    item_->setVisible( sessionHooks::none != type );

    byCountLabel_->setEnabled( sessionHooks::forcedByCount == type  );
    byCount_->setEnabled( sessionHooks::forcedByCount == type  );

    if ( sessionHooks::none == type )
        log_->appendPlainText( "ITEM removed from system" );
    else
        log_->appendPlainText( "HAVE NEW ITEM!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void developmentTestHook::onUpdateCurrentLanguage( const QString& language )
{
    softkeys_->translate();

    log_->appendPlainText( "language changed to: " + language );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void developmentTestHook::onUpdateFieldValue( fieldType type, const QString& value )
{
    log_->appendPlainText( "FIELD update: " + value + " (" + QString::number( type ) + ")" );

    switch ( type )
    {
    // weight info
    case sessionHooks::capacityIndicator:
        capHiRange_->setText( value ); break;
    case sessionHooks::capacityIndicatorString:
        capHiRangeLabel_->setText( value ); break;
    case sessionHooks::capacityString:
        capacityLabel_->setText( value ); break;
    case sessionHooks::dualCapacityIndicator:
        capLoRange_->setText( value ); break;
    case sessionHooks::dualCapacityIndicatorString:
        capLoRangeLabel_->setText( value ); break;
    case sessionHooks::netWeight:
        netWeight_->setText( value ); break;
    case sessionHooks::netWeightString:
        netWeightLabel_->setText( value ); break;
    case sessionHooks::zeroIndicator:
        zero_->setText( value ); break;
    case sessionHooks::zeroIndicatorString:
        zeroLabel_->setText( value ); break;

    // item info
    case sessionHooks::itemCode:
        plu_->clear();
        byCount_->clear();
        // fall through...
    case sessionHooks::itemByCount:
    case sessionHooks::itemDescription:
    case sessionHooks::itemDiscountedTotalPrice:
    case sessionHooks::itemDiscountedUnitPrice:
    case sessionHooks::itemExpandedText:
    case sessionHooks::itemHasFreqShopperPricing:
    case sessionHooks::itemImage:
    case sessionHooks::itemNutrifactsPanel:
    case sessionHooks::itemTare:
    case sessionHooks::itemTotalPrice:
    case sessionHooks::itemUnitPrice:
    case sessionHooks::itemUnitPriceFractional:
    case sessionHooks::itemUnitPricePoundsFor:
        updateItemInfo(); break;

    default:
        break;
    }

    d_->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void developmentTestHook::onByCountTextChanged( const QString& value )
{
    if ( value.isEmpty() )
    {
        Q_EMIT changeFieldValue( sessionHooks::itemByCount, 0 );
    }
    else
    {
        bool ok( false );
        int n( value.toInt( &ok ) );

        if (( !ok ) || ( n < 0 ))
            return;

        log_->appendPlainText( "new BY COUNT: " + value );

        Q_EMIT changeFieldValue( sessionHooks::itemByCount, value );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void developmentTestHook::onPluEditingFinished()
{
    QString value( plu_->text() );

    if ( value.isEmpty() )
        return;

    bool ok( false );
    int n( value.toInt( &ok ) );

    if (( !ok ) || ( n < 0 ))
        return;

    log_->appendPlainText( "new ITEM: " + value );

    Q_EMIT changeFieldValue( sessionHooks::itemCode, value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void developmentTestHook::initialize()
{
    //
    // setup dialog
    //

    d_ = new QDialog();

    Qt::WindowFlags flags( d_->windowFlags() );
    flags |= Qt::WindowMinimizeButtonHint;

    d_->setWindowFlags( flags );
    d_->setWindowTitle( "DEV TEST" );

    //
    // setup tabbed interface
    //

    mainTab_ = new QWidget();
    flashkeysTab_ = new QWidget();

    tabs_ = new QTabWidget( d_ );
    tabs_->addTab( mainTab_, "Main" );
    tabs_->addTab( flashkeysTab_, "Flashkeys" );

    //
    // setup main tab
    //

    // error box
    error_ = new QGroupBox( mainTab_ );
    error_->setVisible( false );

    errorTitle_ = new QLabel( error_ );
    errorMessage_ = new QLabel( error_ );
    errorButtons_ = new QGroupBox( error_ );

    errorButtonsGroup_ = new QButtonGroup( errorButtons_ );
    connect( errorButtonsGroup_, SIGNAL( buttonClicked(int) ), SIGNAL( dialogButtonPress(int) ) );

    // weight box
    weight_ = new QGroupBox( mainTab_ );

    capacityLabel_ = new QLabel( weight_ );
    capacity_ = new QLabel( weight_ );
    capHiRangeLabel_ = new QLabel( weight_ );
    capHiRange_ = new QLabel( weight_ );
    capLoRangeLabel_ = new QLabel( weight_ );
    capLoRange_ = new QLabel( weight_ );
    netWeightLabel_ = new QLabel( weight_ );
    netWeight_ = new QLabel( weight_ );
    zeroLabel_ = new QLabel( weight_ );
    zero_ = new QLabel( weight_ );

    // item box
    item_ = new QGroupBox( mainTab_ );
    item_->setVisible( false );

    itemCodeLabel_ = new QLabel( "PLU", item_ );
    itemCode_ = new QLabel( item_ );
    itemDescLabel_ = new QLabel( "Description", item_ );
    itemDesc_ = new QLabel( item_ );
    itemByCountLabel_ = new QLabel( "By Count", item_ );
    itemByCount_ = new QLabel( item_ );
    itemTareLabel_ = new QLabel( "Tare", item_ );
    itemTare_ = new QLabel( item_ );
    itemUnitPriceLabel_ = new QLabel( "Unit Price", item_ );
    itemUnitPrice_ = new QLabel( item_ );
    itemTotalPriceLabel_ = new QLabel( "Total", item_ );
    itemTotalPrice_ = new QLabel( item_ );
    itemImageLabel_ = new QLabel( "Image", item_ );
    itemImage_ = new QLabel( item_ );
    itemExTextLabel_ = new QLabel( "Exp. Text", item_ );
    itemExText_ = new QLabel( item_ );
    itemNutrifactsLabel_ = new QLabel( "Nutrifacts", item_ );
    itemNutrifacts_ = new QLabel( item_ );

    // Dont need these print buttons anymore since we added it to the keypad
    //print_ = new QPushButton( "PRINT", item_ );
    //connect( print_, SIGNAL( clicked() ), SIGNAL( print() ) );

    // other
    log_ = new QPlainTextEdit( mainTab_ );
    log_->setReadOnly( true );

    changeLanguage_ = new QPushButton( "Change Language", mainTab_ );
    changeLanguage_->setDefault( false );
    connect( changeLanguage_, SIGNAL( clicked() ), SIGNAL( changeLanguage() ) );

    zeroScale_ = new QPushButton( "Zero Scale", mainTab_ );
    zeroScale_->setDefault( false );
    connect( zeroScale_, SIGNAL( clicked() ), SIGNAL( zeroScale() ) );

    auditLog_ = new QPushButton( "Audit Log", mainTab_ );
    auditLog_->setDefault( false );
    connect( auditLog_, SIGNAL( clicked() ), SIGNAL( auditLog() ) );

    pluLabel_ = new QLabel( "PLU#", mainTab_ );
    plu_ = new QLineEdit( mainTab_ );
    connect( plu_, SIGNAL( editingFinished() ), SLOT( onPluEditingFinished() ) );

    byCountLabel_ = new QLabel( "By Count", mainTab_ );
    byCountLabel_->setEnabled( false );

    byCount_ = new QLineEdit( mainTab_ );
    byCount_->setEnabled( false );
    connect( byCount_, SIGNAL( textChanged(QString) ), SLOT( onByCountTextChanged(QString) ) );

    //
    // setup flashkeys tab
    //

    // jhc: I've changed the call to the selfServiceSoftkeysWidget constructor, and as the 2nd argument (where this gets called for real)
    // I've changed it pass and receive as it's 2nd arg, the pointer hook_.  Here in this class we don't have hook_ available.  I was going
    // to then pass hook_ into this class, but this class is not called from anywhere and was used for development testing. Therefore I
    // couldn't add hook_ here.  So I added NULL as 2nd arg so the project would compile. If however at some point in the future this class
    // is brought back to life, NULL here will have to be changed to hook_ and we need to get hook_ into this class when it's called.
    softkeys_ = new hobart::selfServiceSoftkeysWidget( session(), NULL,  flashkeysTab_ );
    connect( softkeys_, SIGNAL( softkeyClicked(QWidget*,QWidget*,int,QString,int) ), SIGNAL( softkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void developmentTestHook::createLayout()
{
    QHBoxLayout *dialog( new QHBoxLayout( d_ ) );
    dialog->addWidget( tabs_ );

    //
    // setup main tab
    //

    // buttons on bottom
    QHBoxLayout *buttons = new QHBoxLayout();
    buttons->addWidget( pluLabel_ );
    buttons->addWidget( plu_ );
    buttons->addWidget( byCountLabel_ );
    buttons->addWidget( byCount_ );
    buttons->addStretch();
    buttons->addWidget( changeLanguage_ );
    buttons->addWidget( zeroScale_ );

    // dialog layout
    QVBoxLayout *layout( new QVBoxLayout( mainTab_ ) );
    layout->addWidget( error_ );
    layout->addWidget( weight_ );
    layout->addWidget( item_ );
    layout->addWidget( log_ );
    layout->addLayout( buttons );

    // error dialog layout
    QVBoxLayout *error = new QVBoxLayout( error_ );
    error->addWidget( errorTitle_ );
    error->addWidget( errorMessage_ );
    error->addWidget( errorButtons_ );

    // weight box layout
    QFormLayout *weight = new QFormLayout( weight_ );
    weight->addRow( capacityLabel_, capacity_ );
    weight->addRow( capHiRangeLabel_, capHiRange_ );
    weight->addRow( capLoRangeLabel_, capLoRange_ );
    weight->addRow( netWeightLabel_, netWeight_ );
    weight->addRow( zeroLabel_, zero_ );

    // item box layout
    QFormLayout *item = new QFormLayout();
    item->addRow( itemCodeLabel_, itemCode_ );
    item->addRow( itemDescLabel_, itemDesc_ );
    item->addRow( itemByCountLabel_, itemByCount_ );
    item->addRow( itemTareLabel_, itemTare_ );
    item->addRow( itemUnitPriceLabel_, itemUnitPrice_ );
    item->addRow( itemTotalPriceLabel_, itemTotalPrice_ );
    item->addRow( itemImageLabel_, itemImage_ );
    item->addRow( itemExTextLabel_, itemExText_ );
    item->addRow( itemNutrifactsLabel_, itemNutrifacts_ );

    // Dont need these print buttons anymore since we added it to the keypad
    //QHBoxLayout *itemBox = new QHBoxLayout( item_ );
    //itemBox->addLayout( item, 1 );
    //itemBox->addWidget( print_ );
    // This is for Walmart CA Self Serve Requests
    //print_->setVisible( false );

    errorButtonsLayout_ = new QHBoxLayout( errorButtons_ );

    //
    // setup flashkeys tab
    //

    QHBoxLayout *softkeys = new QHBoxLayout( flashkeysTab_ );
    softkeys->addWidget( softkeys_ );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void developmentTestHook::updateItemInfo()
{
    QString itemCode( fieldValue( sessionHooks::itemCode ) );

    itemCode_->setText( itemCode );

    if ( itemCode.isEmpty() )
    {
        itemDesc_->clear();
        itemByCount_->clear();
        itemTare_->clear();
        itemUnitPrice_->clear();
        itemTotalPrice_->clear();
        itemImage_->clear();
        itemExText_->clear();
        itemNutrifacts_->clear();
        return;
    }

    itemDesc_->setText( fieldValue( sessionHooks::itemDescription ) );
    itemByCount_->setText( fieldValue( sessionHooks::itemByCount ) );
    itemTare_->setText( fieldValue( sessionHooks::itemTare ) );

    bool isFreqShopperItem( "1" == fieldValue( sessionHooks::itemHasFreqShopperPricing ) );

    // look for discounted pricing
    if ( isFreqShopperItem )
    {
        QString unitPrice( fieldValue( sessionHooks::itemDiscountedUnitPrice ) );

        unitPrice = unitPrice.trimmed();

        if ( unitPrice.isEmpty() )
            isFreqShopperItem = false;
        else
        {
            itemUnitPrice_->setText( unitPrice );
            itemTotalPrice_->setText( fieldValue( sessionHooks::itemDiscountedTotalPrice ) );
        }
    }

    if ( !isFreqShopperItem )
    {
        QString unitPrice( fieldValue( sessionHooks::itemUnitPrice ) );
        QString unitPriceFractional( fieldValue( sessionHooks::itemUnitPriceFractional ) );
        QString unitPricePoundsFor( fieldValue( sessionHooks::itemUnitPricePoundsFor ) );

        unitPriceFractional = unitPriceFractional.trimmed();
        unitPricePoundsFor = unitPricePoundsFor.trimmed();

        if (( unitPriceFractional.length() ) && ( "1" != unitPriceFractional ))
            unitPrice.append( " / " + unitPriceFractional );
        else if (( unitPricePoundsFor.length() ) && ( "1" != unitPricePoundsFor ))
            unitPrice.prepend( unitPricePoundsFor + " for " );

        itemUnitPrice_->setText( unitPrice );
        itemTotalPrice_->setText( fieldValue( sessionHooks::itemTotalPrice ) );
    }

    itemImage_->setText( fieldValue( sessionHooks::itemImage ) );
    itemExText_->setText( fieldValue( sessionHooks::itemExpandedText ) );
    itemNutrifacts_->setText( fieldValue( sessionHooks::itemNutrifactsPanel ) );
}
