/**
 * @file productdescriptioneditorscreen.cpp
 *
 * @copyright Copyright (c) 2013 Hobart Corporation
 * @author Todd Davis
 */

#include <logmanager.h>
#include "productdescriptioneditorscreen.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../modes/operatemode.h"
#include "../modes/supervisormode.h"

// Developer Note: Add your .h file below if you want to use this source file
#include "../screens/superProductDefaults.h"

#include "../dialogs/texteditor.h"
//#include <marqueelabelwidget.h>

#include <Session.h>
#include <ScaleConfiguration.h>
#include <Messages.h>
#include <UiUtilities.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

///////////////////////////////////////////////////////////////////////////////////////////////////
ProductDescriptionEditorScreen::ProductDescriptionEditorScreen( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    pluSupport_( NULL ),
    showAllFieldsReuseMarquee_ (NULL),
    showAllFieldsReuseNutrifacts_ (NULL),
    dirty_( false )
{
    supervisorMode_ = dynamic_cast<SupervisorMode*>(parent);
    // initialize screen
    initialize();
    translate();
    product_ = &session_->scaleConfig->currentPLU;

    textTypeLoc = 4; //3
    usedByManyLoc = "0"; //4
    textFunctionLoc = "0"; //5
    ; //6
    supportTextLoc = ""; //7
    supportText2Loc = ""; //8
    supportText3Loc = ""; //9
    supportText4Loc = ""; //10
    supportText5Loc = ""; //11

    // retrieve config object
    if ( supervisorMode_ )
    {
        //showAllFieldsReuseMarquee_ = &dynamic_cast<SupervisorMode*>( mode_)->showAllFieldsReuseMarquee_;
        //showAllFieldsReuseNutrifacts_ = &dynamic_cast<SupervisorMode*>( mode_)->showAllFieldsReuseNutrifacts_;

        // 0                 1                      2                 3
        session_->send(ProductDev, ProductReadSupportData, product_->itemNumber() + ",4");

        //showAllFieldsReuseMarquee_ = true;
        // Just to be safe
        //showAllFieldsReuseNutrifacts_ = false;

        //pluSupport_ = &dynamic_cast<SupervisorMode*>( mode_ )->pluSupport_;

        // set marquee text
        //marqueeEditor_->setText( pluSupport_->supportText() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ProductDescriptionEditorScreen::~ProductDescriptionEditorScreen()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductDescriptionEditorScreen::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    /*
    title = tr( "Help Page for Marquee Editor" );

    description = tr(
        "Marquee Help Data Here" );
    */
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////


void ProductDescriptionEditorScreen::onSessionMessage( const SessionMessage& msg )
{
    //LOG_CLASS_INFO( "productMarqueeEditorScreen", "onSessionMessage" );
    int device = msg[0].toInt();
    int event = msg[1].toInt();

    //qDebug() << "device,event is: " << device + ":" + event;

    if( ProductDev == device && ProductWriteSupportData == event)
    {
        //101,10
        DomPluSupport m(msg);

        //marqueeEditor_->setText(m.supportText());
        textTypeLoc = m.textType(); //3
        usedByManyLoc = m.usedByMany(); //4
        textFunctionLoc = m.textFunction(); //5
        supportPLULoc = m.supportPLU(); //6
        supportTextLoc = m.supportText(); //7
        supportText2Loc = m.supportText2(); //8
        supportText3Loc = m.supportText3(); //9
        supportText4Loc = m.supportText4(); //10
        supportText5Loc = m.supportText5(); //11
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDescriptionEditorScreen::translate()
{
    //cancelButton_->setText( tr( "Cancel" ) );
    //doneButton_->setText( tr( "Done" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget *ProductDescriptionEditorScreen::swipeScreenRight() const
{
    if ( mainWindow_->isSupervisorMode() )
    {
        //saveMarqueeConfig();

        //return new MarqueeConfigScreen( mainWindow_, mode_ );
    }

    return _Mybase::swipeScreenRight();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDescriptionEditorScreen::onCancelButtonClicked()
{
    showAllFieldsReuseMarquee_ = true;
    showAllFieldsReuseNutrifacts_ = false;

    supervisorMode_->setCameFromProductMarqueeEditor(true);
    supervisorMode_->setCameFromProductNutritionEditor(false);
    supervisorMode_->setCameFromLabelPlacementView(false);
    if ( mainWindow_->isSupervisorMode() )
        ;//mainWindow_->switchToModeScreen<SupervisorMode, superChangeProductAllFields>();
    else
    {
        close();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDescriptionEditorScreen::onDoneButtonClicked()
{
    //showAllFieldsReuseMarquee_ = true;
    //showAllFieldsReuseNutrifacts_ = false;
    //saveMarqueeConfig();

    //onCancelButtonClicked();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDescriptionEditorScreen::onTextEditorTextChanged()
{
    //marqueeLabel_->setText( marqueeEditor_->toPlainText() );
    dirty_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDescriptionEditorScreen::initialize()
{
    QHBoxLayout *horizontalLayout;

    QVBoxLayout *screenLayout = new QVBoxLayout( this );

    screenLayout->setMargin( marginSpacing_ );

    ///////////////////////////////////////////////////////////////////////////
    // marquee editor

    //marqueeEditor_ = new MarqueeTextEditorWidget( this );
    //marqueeEditor_->setObjectName( "marqueeEditor" );

    //screenLayout->addWidget( marqueeEditor_ );

    ///////////////////////////////////////////////////////////////////////////
    // marquee label

    //marqueeLabel_ = new MarqueeLabelWidget( this );
    //marqueeLabel_->setObjectName( "marqueeLabel" );

    //marqueeLabel_->setAutoFillBackground( true );
    //marqueeLabel_->setMinimumHeight( 60 );

    //QPalette p = marqueeLabel_->palette();
    //p.setColor( marqueeLabel_->backgroundRole(), Qt::black );
    //p.setColor( marqueeLabel_->foregroundRole(), Qt::green );
    //marqueeLabel_->setPalette( p );

    //QFont f = marqueeLabel_->font();
    //f.setPointSize( 32 );
    //marqueeLabel_->setFont( f );

    //screenLayout->addWidget( marqueeLabel_ );

    //connect( marqueeEditor_, SIGNAL( textChanged() ), this, SLOT( onMarqueeEditorTextChanged() ) );

    screenLayout->addStretch();

    ///////////////////////////////////////////////////////////////////////////
    // buttons

    //cancelButton_ = new QPushButton( this );
    //cancelButton_->setObjectName( "cancelButton" );

    //doneButton_ = new QPushButton( this );
    //doneButton_->setObjectName( "doneButton" );

    //cancelButton_->setFocusPolicy( Qt::NoFocus );
    //doneButton_->setFocusPolicy( Qt::NoFocus );

    //connect( cancelButton_, SIGNAL( clicked() ), this, SLOT( onCancelButtonClicked() ) );
    //connect( doneButton_, SIGNAL( clicked() ), this, SLOT( onDoneButtonClicked() ) );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->addStretch();
    //horizontalLayout->addWidget( cancelButton_ );
   // horizontalLayout->addWidget( doneButton_ );

    screenLayout->addLayout( horizontalLayout );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDescriptionEditorScreen::saveProductDescriptionConfig() const
{
    // no reason to save if nothing changed
    if ( !dirty_ )
        return;

    /*
    pluNum
    int textTypeLoc; //3
    QString usedByManyLoc; //4
    QString textFunctionLoc; //5
    QString supportPLULoc; //6
    QString supportTextLoc; //7
    QString supportText2Loc; //8
    QString supportText3Loc; //9
    QString supportText4Loc; //10
    QString supportText5Loc; //11
    */

    //QString writeMessage( product_->itemNumber() + "," + QString::number(textTypeLoc)  + "," +  usedByManyLoc + ",0," /*+ textFunctionLoc + ","*/ + supportPLULoc + ",\"" + marqueeEditor_->text() + "\",\"" + supportText2Loc + "\",\"" +supportText3Loc + "\",\"" + supportText4Loc + "\",\"" + supportText5Loc + "\"");

    // Build this from passed in button choice new use copy edit or delete delete all
    //QString changeMessage(product_->itemNumber() + "," + QString::number(textTypeLoc) + ",0,1,,");

    //session_->send(ProductDev, ProductChangeSupportData, changeMessage );
    //session_->send( ProductDev, ProductWriteSupportData, writeMessage );
    session_->send( ProductDev, ProductSave);

    // Send it to all fields page now

    supervisorMode_->setCameFromProductMarqueeEditor(true);
    supervisorMode_->setCameFromProductNutritionEditor(false);
    supervisorMode_->setCameFromLabelPlacementView(false);
    //mainWindow_->switchToModeScreen<SupervisorMode, superChangeProductAllFields>();

 }

