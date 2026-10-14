/**
 * @file storeInformation.cpp
 *
 * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "screenwidget.h"
#include "storeInformation.h"

#include "../sessionmainwindow.h"

#include <ScaleConfiguration.h>
#include <TokenParser.h>

#include <logmanager.h>
#include <numericlineedit.h>

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
storeInformation::storeInformation( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent )
{
    // init
    initialize();
    createLayout();
    translate();

    //---------------------------------------
    // Request the list of available graphics
    //---------------------------------------
    session_->send( GraphicDev, GraphicReadAllNames );

    //-----------------------------------
    // Request the Store Information data
    //-----------------------------------
    session_->send( ConfigurationDev, CfgStoreInfoRead );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
storeInformation::~storeInformation()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void storeInformation::translate()
{
    lblStoreName->setText( tr( "Store name" ) );
    lblStoreID->setText( tr( "Store ID" ) );
    lblStoreGraphic->setText( tr( "Store graphic" ) );

    if ( cbStoreGraphic->count() )
        cbStoreGraphic->setItemText( 0, tr( "None" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void storeInformation::SetupChanged()
{
    bSetupChanged = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void storeInformation::resizeEvent( QResizeEvent *e )
{
    txtStoreName->setFixedSize( (60 * width()) / 100, (20 * height()) / 100 );

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool storeInformation::onScreenChanging()
{
    SaveDataOnExit();

    return _Mybase::onScreenChanging();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void storeInformation::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "storeInformation", "onSessionMessage" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // Store graphics
    if (( GraphicDev == device ) && ( GraphicReadAllNames == event ))
    {
        DomGraphicNameList m( msg );

        //------------------------------------
        // Populate dropdown for Store graphic
        //------------------------------------
        cbStoreGraphic->clear();
        cbStoreGraphic->addItem( tr( "None" ) );

        // Get the list of graphics
        const QVector<QString> namesList( TokenParser::splitStringTokens( m.graphicList() ) );

        if ( namesList.count() != m.numOfItems().toInt() )
            LOG_WARN( "mismatch in graphic list count %d %s", namesList.count(), qPrintable( m.numOfItems() ) );

        // Add the graphic name to the dropdown
        foreach ( const QString &name, namesList )
            cbStoreGraphic->addItem( name );

        bSetupChanged = false;
    }

    // Store Information data
    else if (( ConfigurationDev == device ) && ( CfgStoreInfoWrite == event ))
    {
        DomStoreInformation m( msg );

        // Update UI with the store name and store ID
        txtStoreName->setPlainText( m.storeName() );
        txtStoreID->setText( m.storeID() );

        // Select the correct item in the Store Graphic dropdown
        int index( cbStoreGraphic->findText( m.storeGraphicName() ) );

        if ( 0 < index )
            cbStoreGraphic->setCurrentIndex( index );

        bSetupChanged = false;
    }

    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void storeInformation::initialize()
{
    QFont f( font() );
    f.setPointSize( 16 );

    setFont( f );
    setObjectName( "storeInformation" );

    lblStoreName = new QLabel( this );
    lblStoreName->setAlignment( Qt::AlignTop | Qt::AlignLeft );
    lblStoreName->setObjectName( "lblStoreName" );

    txtStoreName = new QPlainTextEdit( this );
    txtStoreName->setObjectName( "txtStoreName" );

    lblStoreID = new QLabel( this );
    lblStoreID->setAlignment( Qt::AlignTop | Qt::AlignLeft );
    lblStoreID->setObjectName( "lblStoreID" );

    txtStoreID = new hobart::numericLineEdit( this );
    txtStoreID->setDecimalPlaces( 0 );
    txtStoreID->setInputWrapping( true );
    txtStoreID->setObjectName( "txtStoreID" );
    txtStoreID->setValidator( storeIDValidator_ = new QIntValidator( 0, makeValidatorTop(6), this ) );

    lblStoreGraphic = new QLabel( this );
    lblStoreGraphic->setAlignment( Qt::AlignTop | Qt::AlignLeft );
    lblStoreGraphic->setObjectName( "lblStoreGraphic" );

    cbStoreGraphic = new QComboBox( this );
    cbStoreGraphic->setEditable( false );
    cbStoreGraphic->setObjectName( "cbStoreGraphic" );

    //----------------------------------------------------------------
    // Set the connections for when one of the fields on the screen is
    // changed.  These all call the same method to set a property that
    // indicates something in the setup changed and the data needs to
    // be saved.
    //----------------------------------------------------------------
    connect( txtStoreName, SIGNAL(textChanged()), SLOT(SetupChanged()) );
    connect( txtStoreID, SIGNAL(editingFinished()), SLOT(SetupChanged()) );
    connect( cbStoreGraphic, SIGNAL(currentIndexChanged(int)), SLOT(SetupChanged()) );

    bSetupChanged = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void storeInformation::createLayout()
{
    QFormLayout *formLayout( new QFormLayout() );
    formLayout->setHorizontalSpacing( 32 );
    formLayout->setVerticalSpacing( 32 );
    formLayout->addRow( lblStoreName, txtStoreName );
    formLayout->addRow( lblStoreID, txtStoreID );
    formLayout->addRow( lblStoreGraphic, cbStoreGraphic );

    QVBoxLayout *vlayout( new QVBoxLayout() );
    vlayout->addStretch();
    vlayout->addLayout( formLayout );
    vlayout->addStretch();

    QHBoxLayout *hlayout( new QHBoxLayout( this ) );
    hlayout->addStretch();
    hlayout->addLayout( vlayout );
    hlayout->addStretch();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int storeInformation::makeValidatorTop( int digits )
{
    int top( 0 );

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }

    return top;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void storeInformation::SaveDataOnExit()
{
    if ( bSetupChanged )
    {
        QString param;

        param = "\"" + txtStoreName->toPlainText() + "\"";
        param += "," + txtStoreID->text();
        param += "," + cbStoreGraphic->currentText();

        session_->send( ConfigurationDev, CfgStoreInfoWrite, param );
        session_->send( ConfigurationDev, CfgStoreInfoSave );   // Requires an additional "Save" message
    }
}
