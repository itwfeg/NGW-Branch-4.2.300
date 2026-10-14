/**
 * @file supernutritioneditor.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "superChangeProductAllFields.h"
#include "supernutritioneditor.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../modes/operatemode.h"
#include "../modes/supervisormode.h"

#include <font.h>

#include <Session.h>
#include <ScaleConfiguration.h>
#include <Messages.h>
#include <UiUtilities.h>

#include <labelviewerwidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QLabel>
#include <QPushButton>

///////////////////////////////////////////////////////////////////////////////////////////////////
superNutritionEditorScreen::superNutritionEditorScreen( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    requiredDataCounter_( -1 ),
    showAllFieldsReuseMarquee_ (NULL),
    showAllFieldsReuseNutrifacts_ (NULL)
{
    supervisorMode_ = dynamic_cast<SupervisorMode*>( parent );

    // initialize screen
    initialize();
    translate();

    // get plu
    product_ = &session_->scaleConfig->currentPLU;

    // request data
    if ( supervisorMode_ )
    {
        //showAllFieldsReuseMarquee_ = &dynamic_cast<SupervisorMode*>( mode_)->showAllFieldsReuseMarquee_;
        //showAllFieldsReuseNutrifacts_ = &dynamic_cast<SupervisorMode*>( mode_)->showAllFieldsReuseNutrifacts_;

        // need the below 3 data messages to function properly
        requiredDataCounter_ = 3;

        // read generic text list
        session_->send( GenericTextDev, AppletViewerReadAll );

        // read nutrifact data
        session_->send( ProductDev, ProductReadSupportData, product_->itemNumber() + "," + QString::number( OperateMode::Nutrifacts ) );

        // read nutrifact label
        session_->send( ProductDev, ProductReadNutriLabel );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
superNutritionEditorScreen::~superNutritionEditorScreen()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superNutritionEditorScreen::translate()
{
    doneButton_->setText( tr( "Done" ) );
    message_->setText( tr( "Please assign a nutrifact label type to the product record." ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superNutritionEditorScreen::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "superNutritionEditorScreen", "onSessionMessage" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // generic text
    if (( GenericTextDev == device ) && ( AppletViewerWriteAll == event ))
    {
        // message is parsed in session
        updateLabelViewerVisible();
    }

    // nutrifact data
    else if (( ProductDev == device ) && ( ProductWriteSupportData == event ))
    {
        DomPluSupport m( msg );

        if ( OperateMode::Nutrifacts == m.textType() )
        {
            // no nutrifacts text
            if (( m.supportText().isEmpty() ) && ( m.supportText2().isEmpty() ) && ( m.supportText3().isEmpty() ) && ( m.supportText4().isEmpty() ))
                LOG_INFO( "item has no nutrifact text", NULL );

            ScaleConfiguration *config = session_->scaleConfig;

            // create nutrifact data
            nutrifact n( m.pluNum().toInt() );

            n.setColumn( 0, m.supportText() );
            n.setColumn( 1, m.supportText2() );
            n.setColumn( 2, m.supportText3() );
            n.setColumn( 3, m.supportText4() );

            // insert into data store
            config->dataStore_.store<nutrifact>()->insert( n );

            // set the label nutrition
            labelViewer_->setNutrifactsId( n.id() );

            updateLabelViewerVisible();
        }
    }

    // nutrifact label
    else if (( LabelTypeDev == device ) && ( LabelTypeDtWrite == event ))
    {
        DomLabelType m( msg );

        int id = m.id().toInt();

        // label id
        if ( id <= 0 )
            LOG_WARN( "bad label id %d", id );
        else
        {
            ScaleConfiguration *config = session_->scaleConfig;

            // set the label
            labelViewer_->setLabel( config->dataStore_.store<label>()->value( id ) );

            // check for nutrifacts
            if ( !labelViewer_->hasNutrifacts() )
            {
                LOG_WARN( "label does not contain nutrifacts", NULL );
                product_->hasNutrifacts() = "0";
            }
            else
            {
                updateLabelViewerVisible();
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superNutritionEditorScreen::onDoneButtonClicked()
{
    //showAllFieldsReuseMarquee_ = false;
    //showAllFieldsReuseNutrifacts_ = true;
    ScaleConfiguration *config = session_->scaleConfig;

    // retrieve data from data store
    nutrifact n = config->dataStore_.store<nutrifact>()->value(  product_->itemNumber().toInt() );

    QString column0( n.column( 0 ).data() );
    QString column1( n.column( 1 ).data() );
    QString column2( n.column( 2 ).data() );
    QString column3( n.column( 3 ).data() );

    // format string
    QString message(  product_->itemNumber() + "," + QString::number( OperateMode::Nutrifacts ) + ",1,,,\"" + column0 + "\",\"" + column1 + "\",\"" + column2 + "\",\"" + column3 + "\",\"" + "\"" );

    // save!
    session_->send( ProductDev, ProductWriteSupportData, message );
    session_->send( ProductDev, ProductSave );

    supervisorMode_->setCameFromProductMarqueeEditor(false);
    supervisorMode_->setCameFromLabelPlacementView(false);
    supervisorMode_->setCameFromProductNutritionEditor(true);

    // switch back to product screen
    mainWindow_->switchToModeScreen<SupervisorMode, superChangeProductAllFields>();

    // FT-731: refresh product info
    session_->send( ProductDev, ProductRead, product_->itemNumber() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superNutritionEditorScreen::initialize()
{
    ScaleConfiguration *config = session_->scaleConfig;

    modifyFontPointSize setFontPointSize10( scaleFontPointSizeForScreen( 10 ) );

    // done button
    doneButton_ = new QPushButton( this );
    doneButton_->setFocusPolicy( Qt::NoFocus );
    doneButton_->setObjectName( "doneButton" );

    connect( doneButton_, SIGNAL( clicked() ), SLOT( onDoneButtonClicked() ) );

    // message
    message_ = new QLabel( this );
    message_->setObjectName( "message" );

    // label viewer
    labelViewer_ = new LabelViewerWidget( true, this );
    labelViewer_->setNutrifactsEditingAllowed( true );
    labelViewer_->setObjectName( "labelViewer" );
    labelViewer_->setStore( &config->dataStore_ );
    labelViewer_->setVisible( false );
    setFontPointSize10( labelViewer_ );

    // layouts
    QVBoxLayout *screenLayout = new QVBoxLayout( this );
    QHBoxLayout *hlayout;

    screenLayout->setMargin( marginSpacing_ );

    hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget( message_ );
    hlayout->addStretch();
    screenLayout->addLayout( hlayout );

    screenLayout->addWidget( labelViewer_ );

    hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget( doneButton_ );
    screenLayout->addLayout( hlayout );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superNutritionEditorScreen::updateLabelViewerVisible()
{
    // when counter reaches zero we have all the data
    if ( 0 == --requiredDataCounter_ )
    {
        // hide message
        message_->hide();

        // show label viewer
        labelViewer_->show();
        product_->hasNutrifacts() == "1";
    }
}
