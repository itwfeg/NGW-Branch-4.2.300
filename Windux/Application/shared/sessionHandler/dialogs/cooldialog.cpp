/**
 * @file cooldialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "cooldialog.h"

#include "../sessionmainwindow.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QVBoxLayout>

const bool CoolDialog::convertUppercase_ = true;

///////////////////////////////////////////////////////////////////////////////////////////////////
CoolDialog::CoolDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( parent, f ),
    coolTextRequired_( false ),
    coolTrackingNumberRequired_( false ),
    handleSignals_( false )
{
    setObjectName( "CoolDialog" );

    // initialize interface
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CoolDialog::~CoolDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CoolDialog::allowOverlayClickDismiss() const
{
    if ( !allowReject() )
        return false;

    return _Mybase::allowOverlayClickDismiss();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int CoolDialog::cool() const
{
    QListWidgetItem *item = coolText_->currentItem();

    if ( !item )
        return 0;

    return item->data( Qt::UserRole ).toInt();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CoolDialog::isCoolEnabled() const
{
    return coolText_->isEnabled();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CoolDialog::isTrackingNumberEnabled() const
{
    return coolTrackingNumber_->isEnabled();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CoolDialog::setCool( int value )
{
    for ( int i = 0; i < coolText_->count(); ++i )
    {
        QListWidgetItem *item = coolText_->item( i );

        if ( value == item->data( Qt::UserRole ).toInt() )
        {
            coolText_->setCurrentItem( item );
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CoolDialog::setCoolEnabled( bool value )
{
    coolTextLabel_->setVisible( value );
    coolText_->setVisible( value );

    coolText_->setEnabled( value );
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CoolDialog::setTitle( const QString& value )
{
    title_->setText( value );
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CoolDialog::setTrackingNumber( const QString& value )
{
    coolTrackingNumber_->setText( value );
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CoolDialog::setTrackingNumberEnabled( bool value )
{
    coolTrackingNumberLabel_->setVisible( value );
    coolTrackingNumber_->setVisible( value );
    doneButton_->setVisible( value );

    coolTrackingNumber_->setEnabled( value );
    coolTrackingNumber_->setReadOnly( !value );
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString CoolDialog::trackingNumber() const
{
    return coolTrackingNumber_->text();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString CoolDialog::title() const
{
    return title_->text();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CoolDialog::addCoolTextItem( int id, const QString& text, bool isDefault )
{
    QListWidgetItem *item = new QListWidgetItem( text + (isDefault ? tr( " (default)" ) : "") );
    item->setData( Qt::UserRole, id );

    coolText_->addItem( item );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CoolDialog::showEvent( QShowEvent *event )
{
    _Mybase::showEvent( event );

    // save off original values
    savedCoolText_ = cool();
    savedCoolTrackingNumber_ = trackingNumber();

    // expand cool if possible
    if ( isTrackingNumberEnabled() )
    {
        // FT-817
        updateTrackingNumberForForcedCool();

        coolTrackingNumber_->setFocus();
        coolTrackingNumber_->selectAll();
    }
    else if ( isCoolEnabled() )
    {
        coolText_->setFocus();
    }

    handleSignals_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::widgetRendererAppearanceBase *CoolDialog::rendererAppearance() const
{
    return new hobart::darkOperatorBackgroundWidgetRendererAppearance();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int CoolDialog::rendererStyle() const
{
    // show focused border
    return hobart::Focused;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CoolDialog::onCoolItemClicked( QListWidgetItem *item )
{
    if ( !handleSignals_ )
        return;

    // FT-565 clicking selected item should accept the prompt
    if (( item->isSelected() ) && ( allowAccept() ))
        accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CoolDialog::onCoolItemSelectionChanged()
{
    if ( !handleSignals_ )
        return;

    if ( allowAccept() )
        accept();
    else
    {
        // FT-817
        updateTrackingNumberForForcedCool();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CoolDialog::onCoolTrackingNumberReturnPressed()
{
    if ( !handleSignals_ )
        return;

    if ( savedCoolTrackingNumber_ != coolTrackingNumber_->text() )
        if ( allowAccept() )
            accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CoolDialog::onDoneButtonClicked()
{
    if ( allowAccept() )
        accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CoolDialog::initialize()
{
    QHBoxLayout *hlayout;
    QVBoxLayout *vlayout;

    // setup layout
    QVBoxLayout *screenLayout = new QVBoxLayout( this );

    modifyFontPointSize setEntryFieldFontPointSize( (mainWindow_->height() * 18) / SessionMainWindow::scalingDividerY_ );
    modifyFontPointSize setFontPointSize( (mainWindow_->height() * 9) / SessionMainWindow::scalingDividerY_ );

    modifyPaletteHighlight setHighlight( hobart::colorPalette::blue );
    modifyPaletteHighlightText setHighlightText( hobart::colorPalette::white );

    modifyPaletteForeground setForegroundPalette( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );
    setForegroundPalette( this );

    // dialog title
    title_ = new QLabel( this );
    setFontPointSize( title_ );

    hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget( title_ );
    hlayout->addStretch();

    screenLayout->addLayout( hlayout );
    screenLayout->addStretch();

    // do not display title (as per Jim and Debra)
    title_->hide();

    // cool text label
    QString coolTextLabelText( tr( "Country of Origin" ) );

    if ( convertUppercase_ )
        coolTextLabelText = coolTextLabelText.toUpper();

    coolTextLabel_ = new QLabel( coolTextLabelText, this );
    coolTextLabel_->setObjectName( "coolTextLabel" );
    setFontPointSize( coolTextLabel_ );

    // cool text
    coolText_ = new QListWidget( this );
    coolText_->setObjectName( "coolText" );
    coolText_->setPalette( QApplication::palette() );
    coolText_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    setEntryFieldFontPointSize( coolText_ );
    setHighlight( coolText_ );
    setHighlightText( coolText_ );

    connect( coolText_, SIGNAL( itemClicked(QListWidgetItem*) ), SLOT( onCoolItemClicked(QListWidgetItem*) ) );
    connect( coolText_, SIGNAL( itemSelectionChanged() ), SLOT( onCoolItemSelectionChanged() ) );

    screenLayout->addWidget( coolTextLabel_ );
    screenLayout->addWidget( coolText_, 2 );
    screenLayout->addStretch();

    // tracking number label
    QString coolTrackingNumberLabelText( tr( "Tracking Number" ) );

    if ( convertUppercase_ )
        coolTrackingNumberLabelText = coolTrackingNumberLabelText.toUpper();

    coolTrackingNumberLabel_ = new QLabel( coolTrackingNumberLabelText, this );
    coolTrackingNumberLabel_->setObjectName( "coolTrackingNumberLabel" );
    setFontPointSize( coolTrackingNumberLabel_ );

    // tracking number
    coolTrackingNumber_ = new QLineEdit( this );
    coolTrackingNumber_->setObjectName( "coolTrackingNumber" );
    coolTrackingNumber_->setPalette( QApplication::palette() );
    setEntryFieldFontPointSize( coolTrackingNumber_ );

    connect( coolTrackingNumber_, SIGNAL( returnPressed() ), SLOT( onCoolTrackingNumberReturnPressed() ) );

    vlayout = new QVBoxLayout();
    vlayout->addWidget( coolTrackingNumberLabel_ );
    vlayout->addWidget( coolTrackingNumber_ );

    // done button
    doneButton_ = new QPushButton( tr( "Done" ), this );
    doneButton_->setObjectName( "doneButton" );
    doneButton_->setPalette( QApplication::palette() );

    connect( doneButton_, SIGNAL( clicked() ), SLOT( onDoneButtonClicked() ) );

    hlayout = new QHBoxLayout();
    hlayout->addLayout( vlayout );
    hlayout->addWidget( doneButton_ );

    screenLayout->addLayout( hlayout );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CoolDialog::allowReject() const
{
    return isValidInput( savedCoolText_, savedCoolTrackingNumber_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CoolDialog::allowAccept() const
{
    return isValidInput( cool(), trackingNumber() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CoolDialog::isValidInput( int c, const QString& t ) const
{
    // must have cool
    if ( isCoolRequired() )
        if ( 0 == c )
            return false;

    // must have tracking number (when cool is not "no cool text")
    if ( isTrackingNumberRequired() )
        if ( -1 != c )
            if ( !t.length() )
                return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CoolDialog::updateTrackingNumberForForcedCool()
{
    // expand cool if possible
    if ( isTrackingNumberEnabled() )
    {
        // FT-817
        // when tracking required and cool selected change the background to red... this is to help
        // the operator figure out tracking is required and we will not continue without a number
        // in the field
        if ( isTrackingNumberRequired() )
            if (( !allowAccept() ) && ( 0 < coolText_->selectedItems().count() ))
            {
                modifyPaletteBackground setBackground( hobart::colorPalette::red );
                modifyPaletteHighlight setHighlight( hobart::colorPalette::red );

                setBackground( coolTrackingNumber_ );
                setHighlight( coolTrackingNumber_ );
            }
    }
}



