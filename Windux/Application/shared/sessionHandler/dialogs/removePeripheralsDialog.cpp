/**
 * @file removePeripheralsDialog.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */


#include <logmanager.h>

#include "removePeripheralsDialog.h"
#include "ui_removePeripheralsDialog.h"

#include "../sessionmainwindow.h"
#include "../sessioninfo.h"

#include <ScaleConfiguration.h>
#include <Messages.h>
#include <UiUtilities.h>

static const int X_POS  = 30;
static const int BOX_WIDTH = 740;
static const int BOX_HEIGHT = 41;

static const int WEIGHER_1 = 0;
static const int WEIGHER_2 = 1;
static const int PRINTER_1 = 2;
static const int PRINTER_2 = 3;
static const int PRINTER_3 = 4;
static const int LA_APPLIER_1 = 5;
static const int WRAPPER_1 = 6;
static const int NUMERIC_1 = 7;
static const int MARQUEE_1 = 8;
static const int GCD_1 = 9;

RemovePeripheralsDialog::RemovePeripheralsDialog(QWidget *parent): SessionDialog( TopDialogLocation, parent ),
    ui(new Ui::RemovePeripheralsDialog)
{
    sessionInfo info( this );

    setObjectName( "RemovePeripheralsDialog" );

    // get some pointers so we can do stuff
    mainWindow_ = info();
    session_ = mainWindow_->session();

    weigher1 = 0;
    weigher2 = 0;
    printer1 = 0;
    printer2 = 0;
    printer3 = 0;
    labeler1 = 0;
    wrapper1 = 0;
    numeric1 = 0;
    marquee1 = 0;
    gcd1 = 0;

    ui->setupUi(this);

    const QObjectList children( this->children() );

    if(mainWindow_->width() > mainWindow_->scalingDividerX_){
        foreach ( QObject *child, children )
            if ( child->isWidgetType() )
                SessionDialog::scaleWidget( qobject_cast<QWidget*>( child ) );
    }

    translate();

    initialize();

    // while this widget is open do not show system dialogs
    session_->send( SessionDev, SnStopDialogs );
}

RemovePeripheralsDialog::~RemovePeripheralsDialog()
{
    // resume dialogs
    session_->send( SessionDev, SnRestartDialogs );

    delete ui;
}

bool RemovePeripheralsDialog::allowOverlayClickDismiss() const
{
    // operator must click the done button to dismiss dialog
    return false;
}

void RemovePeripheralsDialog::translate()
{
    ui->retranslateUi(this);

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::initialize()
{
    currentRow = 0;                                                 // what row are we inserting on?
    rowPosition_ << 125 << 175 << 225 << 275 << 325 << 375 << 425 << 475 << 525 << 575;		//	HA-41. Making space for heading.

    // if missing, flip to 1. in same order as UI
    weigherTypeNames   << tr("Unknown") << tr("Ultima") << tr("USB") << tr("APC") << tr("AWS2");
    printerTypeNames   << tr("Unknown") << tr("EPP") << tr("ESS") << tr("ESC") << tr("APC") << tr("EPP2") << tr("EPP4");
    laApplierTypeNames << tr("Unknown") << tr("ILA") << tr("Ultima") << tr("Access") << tr("CLAS");
    wrapperTypeNames   << tr("Unknown") << tr("Ultima") << tr("Access");
    numericTypeNames   << tr("Unknown") << tr("ESC") << tr("APC") << tr("Unknown");
    marqueeTypeNames   << tr("Unknown") << tr("ESC") << tr("APC") << tr("Unknown");
    gcdTypeNames       << tr("Unknown") << tr("USB") << tr("Unknown");

    // hide everything to start with, since we know everything is working properly and everything will be connected
    ui->gbFirstWeigher->setVisible( false );
    ui->gbSecondWeigher->setVisible( false );
    ui->gbFirstPrinter->setVisible( false );
    ui->gbSecondPrinter->setVisible( false );
    ui->gbThirdPrinter->setVisible( false );
    ui->gbGcd->setVisible( false );
    ui->gbNumeric->setVisible( false );
    ui->gbMarquee->setVisible( false );
    ui->gbWrapper->setVisible( false );
    ui->gbLabelApplier->setVisible( false );

    connect(ui->butRemoveFirstWeigher,      SIGNAL(clicked()), this, SLOT(removeFirstWeigher()));
    connect(ui->butRemoveSecondWeigher,     SIGNAL(clicked()), this, SLOT(removeSecondWeigher()));
    connect(ui->butRmoveFirstPrinter,       SIGNAL(clicked()), this, SLOT(removeFirstPrinter()));
    connect(ui->butRemoveSecondPrinter,     SIGNAL(clicked()), this, SLOT(removeSecondPrinter()));
    connect(ui->butRemoveThirdPrinter,      SIGNAL(clicked()), this, SLOT(removeThirdPrinter()));
    connect(ui->butRemoveNumeric,           SIGNAL(clicked()), this, SLOT(removeNumeric()));
    connect(ui->butRemoveMarquee,           SIGNAL(clicked()), this, SLOT(removeMarquee()));
    connect(ui->butRemoveLabelApplier,      SIGNAL(clicked()), this, SLOT(removeLabelApplier()));
    connect(ui->butRemoveWrapper,           SIGNAL(clicked()), this, SLOT(removeWrapper()));
    connect(ui->butRemoveGcd,               SIGNAL(clicked()), this, SLOT(removeGcd()));

    connect(ui->butDone,                    SIGNAL(clicked()), this, SLOT(onClickDone()));

    showLostPeripherals();

    displayElements();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          PRIVATE METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::displayElements()
{
    // once we get to displayElements(), which is called by initialize(), the 138,20 message has
    // already been received unsolicited so now we can decide what fields to display and not.

    // TEST TEST - hardcode the values
    /*
    weigher1 = 3;
    weigher2 = 2;
    printer1 = 4;
    printer2 = 3;
    printer3 = 2;
    labeler1 = 1;
    wrapper1 = 2;
    numeric1 = 2;
    marquee1 = 1;
    gcd1 = 1;
    */

    if( (0 != weigher1 ) || (0 != weigher2) )
    {
        displayWeighers();
    }

    if( (0 != printer1 ) || (0 != printer2) ||  (0 != printer3) )
    {
        displayPrinters();
    }

    if( 0 != labeler1 )
    {
        displayLabelApplier();
    }

    if( 0 != wrapper1 )
    {
        displayWrapper();
    }

    if( 0 != numeric1 )
    {
        displayNumeric();
    }

    if( 0 != marquee1 )
    {
        displayMarquee();
    }

    if( 0 != gcd1 )
    {
        displayGCD();
    }

    update();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::displayWeighers()
{
    const int x( (mainWindow_->width() * X_POS) / SessionMainWindow::scalingDividerX_ );

    const int width( (mainWindow_->width() * BOX_WIDTH) / SessionMainWindow::scalingDividerX_ );
    const int height( (mainWindow_->height() * BOX_HEIGHT) / SessionMainWindow::scalingDividerY_ );

    if( 0 != weigher1 )
    {
        ui->gbFirstWeigher->setGeometry( x, (mainWindow_->height() * rowPosition_[currentRow++]) / SessionMainWindow::scalingDividerY_, width, height );
        ui->gbFirstWeigher->setVisible( true );

        if( weigher1 < weigherTypeNames.length() )
            ui->lblFirstWeigherType->setText(weigherTypeNames.at(weigher1));
        else
            ui->lblFirstWeigherType->setText(tr("Unknown"));
    }

    if( 0 != weigher2  )
    {
        ui->gbSecondWeigher->setGeometry( x, (mainWindow_->height() * rowPosition_[currentRow++]) / SessionMainWindow::scalingDividerY_, width, height );
        ui->gbSecondWeigher->setVisible( true );

        if( weigher2 < weigherTypeNames.length() )
            ui->lblSecondWeigherType->setText(weigherTypeNames.at(weigher2));
        else
            ui->lblSecondWeigherType->setText(tr("Unknown"));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::displayPrinters()
{
    const int x( (mainWindow_->width() * X_POS) / SessionMainWindow::scalingDividerX_ );

    const int width( (mainWindow_->width() * BOX_WIDTH) / SessionMainWindow::scalingDividerX_ );
    const int height( (mainWindow_->height() * BOX_HEIGHT) / SessionMainWindow::scalingDividerY_ );

    if( 0 != printer1 )
    {
        ui->gbFirstPrinter->setGeometry( x, (mainWindow_->height() * rowPosition_[currentRow++]) / SessionMainWindow::scalingDividerY_, width, height );
        ui->gbFirstPrinter->setVisible( true );

        if( printer1 < printerTypeNames.length() )
            ui->lblPrimaryPrinterType->setText(printerTypeNames.at(printer1));
        else
            ui->lblPrimaryPrinterType->setText(tr("Unknown"));
    }

    if( 0 != printer2  )
    {
        ui->gbSecondPrinter->setGeometry( x, (mainWindow_->height() * rowPosition_[currentRow++]) / SessionMainWindow::scalingDividerY_, width, height );
        ui->gbSecondPrinter->setVisible( true );

        if( printer2 < printerTypeNames.length() )
            ui->lblSecondaryPrinterType->setText(printerTypeNames.at(printer2));
        else
            ui->lblSecondaryPrinterType->setText(tr("Unknown"));
    }

    if( 0 != printer3 )
    {
        ui->gbThirdPrinter->setGeometry( x, (mainWindow_->height() * rowPosition_[currentRow++]) / SessionMainWindow::scalingDividerY_, width, height );
        ui->gbThirdPrinter->setVisible( true );

        if( printer3 < printerTypeNames.length() )
            ui->lblThirdPrinterType->setText(printerTypeNames.at(printer3));
        else
            ui->lblThirdPrinterType->setText(tr("Unknown"));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::displayGCD()
{
    const int x( (mainWindow_->width() * X_POS) / SessionMainWindow::scalingDividerX_ );

    const int width( (mainWindow_->width() * BOX_WIDTH) / SessionMainWindow::scalingDividerX_ );
    const int height( (mainWindow_->height() * BOX_HEIGHT) / SessionMainWindow::scalingDividerY_ );

    ui->gbGcd->setGeometry( x, (mainWindow_->height() * rowPosition_[currentRow++]) / SessionMainWindow::scalingDividerY_, width, height );
    ui->gbGcd->setVisible( true );

    if( gcd1 < gcdTypeNames.length() )
        ui->lblGcdType->setText(gcdTypeNames.at(gcd1));
    else
        ui->lblGcdType->setText(tr("Unknown"));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::displayNumeric()
{
    const int x( (mainWindow_->width() * X_POS) / SessionMainWindow::scalingDividerX_ );

    const int width( (mainWindow_->width() * BOX_WIDTH) / SessionMainWindow::scalingDividerX_ );
    const int height( (mainWindow_->height() * BOX_HEIGHT) / SessionMainWindow::scalingDividerY_ );

    ui->gbNumeric->setGeometry( x, (mainWindow_->height() * rowPosition_[currentRow++]) / SessionMainWindow::scalingDividerY_, width, height );
    ui->gbNumeric->setVisible( true );

    if( numeric1 < numericTypeNames.length() )
        ui->lblNumericType->setText(numericTypeNames.at(numeric1));
    else
        ui->lblNumericType->setText(tr("Unknown"));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::displayMarquee()
{
    const int x( (mainWindow_->width() * X_POS) / SessionMainWindow::scalingDividerX_ );

    const int width( (mainWindow_->width() * BOX_WIDTH) / SessionMainWindow::scalingDividerX_ );
    const int height( (mainWindow_->height() * BOX_HEIGHT) / SessionMainWindow::scalingDividerY_ );

    ui->gbMarquee->setGeometry( x, (mainWindow_->height() * rowPosition_[currentRow++]) / SessionMainWindow::scalingDividerY_, width, height );
    ui->gbMarquee->setVisible( true );

    if( marquee1 < marqueeTypeNames.length() )
        ui->lblMarqueeType->setText(marqueeTypeNames.at(marquee1));
    else
        ui->lblMarqueeType->setText(tr("Unknown"));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::displayWrapper()
{
    const int x( (mainWindow_->width() * X_POS) / SessionMainWindow::scalingDividerX_ );

    const int width( (mainWindow_->width() * BOX_WIDTH) / SessionMainWindow::scalingDividerX_ );
    const int height( (mainWindow_->height() * BOX_HEIGHT) / SessionMainWindow::scalingDividerY_ );

    ui->gbWrapper->setGeometry( x, (mainWindow_->height() * rowPosition_[currentRow++]) / SessionMainWindow::scalingDividerY_, width, height );
    ui->gbWrapper->setVisible( true );

    if( wrapper1 < wrapperTypeNames.length() )
        ui->lblWrapperType->setText(wrapperTypeNames.at(wrapper1));
    else
        ui->lblWrapperType->setText(tr("Unknown"));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::displayLabelApplier()
{
    const int x( (mainWindow_->width() * X_POS) / SessionMainWindow::scalingDividerX_ );

    const int width( (mainWindow_->width() * BOX_WIDTH) / SessionMainWindow::scalingDividerX_ );
    const int height( (mainWindow_->height() * BOX_HEIGHT) / SessionMainWindow::scalingDividerY_ );

    ui->gbLabelApplier->setGeometry( x, (mainWindow_->height() * rowPosition_[currentRow++]) / SessionMainWindow::scalingDividerY_, width, height );
    ui->gbLabelApplier->setVisible( true );

    if( labeler1 < laApplierTypeNames.length() )
        ui->lblLabelApplierType->setText(laApplierTypeNames.at(labeler1));
    else
        ui->lblLabelApplierType->setText(tr("Unknown"));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::deletePeripherial(int perifNum)
{
    QString msg = "";

    switch(perifNum)
    {
        case WEIGHER_1:
            msg = "1,0";                                                            // where 1 = weigher and 0 = 0th item
            weigher1 = 0;
            ui->gbFirstWeigher->setVisible( false );
            break;
        case WEIGHER_2:
            msg = "1,1";                                                            // where 1 = weigher and 1 = 1th item
            weigher2 = 0;
            ui->gbSecondWeigher->setVisible( false );
            break;
        case PRINTER_1:
            msg = "7,0";                                                            // where 7 = printer and 0 = 0th printer
            printer1 = 0;
            ui->gbFirstPrinter->setVisible( false );
            break;
        case PRINTER_2:
            msg = "7,1";                                                            // where 7 = printer and 1 = 1th item
            printer2 = 0;
            ui->gbSecondPrinter->setVisible( false );
            break;
        case PRINTER_3:
            msg = "7,2";                                                            // where 7 = printer and 2 = 2nd item
            printer3 = 0;
            ui->gbThirdPrinter->setVisible( false );
            break;
        case LA_APPLIER_1:
            msg = "5,0";                                                            // where 5 = label applier and 0 = 0th item
            labeler1 = 0;
            ui->gbLabelApplier->setVisible( false );
            break;
        case WRAPPER_1:
            msg = "9,0";                                                            // where 9 = wrapper and 0 = 0th item
            wrapper1 = 0;
            ui->gbWrapper->setVisible( false );
            break;
        case NUMERIC_1:
            msg = "139,0";                                                          // where 139 = numeric and 0 = 0th item
            numeric1 = 0;
            ui->gbNumeric->setVisible( false );
            break;
        case MARQUEE_1:
            msg = "140,0";                                                  // where 1 = marquee and 0 = 0th item
            marquee1 = 0;
            ui->gbMarquee->setVisible( false );
            break;
        case GCD_1:
            msg = "161,0";                                                          // where 161 = gcd and 0 = 0th item
            gcd1 = 0;
            ui->gbGcd->setVisible( false );
            break;
    }

    updateLocations();

    session_->send(PeripheralDev, RemoveCheckingLostPeripheral, msg);               // 128,21 - remove lost perif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::updateLocations()
{
    const int x( (mainWindow_->width() * X_POS) / SessionMainWindow::scalingDividerX_ );

    const int width( (mainWindow_->width() * BOX_WIDTH) / SessionMainWindow::scalingDividerX_ );
    const int height( (mainWindow_->height() * BOX_HEIGHT) / SessionMainWindow::scalingDividerY_ );

    int i( 0 );

    if( 0 != weigher1 )
        ui->gbFirstWeigher->setGeometry( x, (mainWindow_->height() * rowPosition_[i++]) / SessionMainWindow::scalingDividerY_, width, height );

    if( 0 != weigher2  )
        ui->gbSecondWeigher->setGeometry( x, (mainWindow_->height() * rowPosition_[i++]) / SessionMainWindow::scalingDividerY_, width, height );

    if( 0 != printer1  )
        ui->gbFirstPrinter->setGeometry( x, (mainWindow_->height() * rowPosition_[i++]) / SessionMainWindow::scalingDividerY_, width, height );

    if( 0 != printer2  )
        ui->gbSecondPrinter->setGeometry( x, (mainWindow_->height() * rowPosition_[i++]) / SessionMainWindow::scalingDividerY_, width, height );

    if( 0 != printer3  )
        ui->gbThirdPrinter->setGeometry( x, (mainWindow_->height() * rowPosition_[i++]) / SessionMainWindow::scalingDividerY_, width, height );

    if( 0 != labeler1  )
        ui->gbLabelApplier->setGeometry( x, (mainWindow_->height() * rowPosition_[i++]) / SessionMainWindow::scalingDividerY_, width, height );

    if( 0 != wrapper1  )
        ui->gbWrapper->setGeometry( x, (mainWindow_->height() * rowPosition_[i++]) / SessionMainWindow::scalingDividerY_, width, height );

    if( 0 != numeric1  )
        ui->gbNumeric->setGeometry( x, (mainWindow_->height() * rowPosition_[i++]) / SessionMainWindow::scalingDividerY_, width, height );

    if( 0 != marquee1  )
        ui->gbMarquee->setGeometry( x, (mainWindow_->height() * rowPosition_[i++]) / SessionMainWindow::scalingDividerY_, width, height );

    if( 0 != gcd1  )
        ui->gbGcd->setGeometry( x, (mainWindow_->height() * rowPosition_[i++]) / SessionMainWindow::scalingDividerY_, width, height );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::showLostPeripherals()
{
    ScaleConfiguration *config = session_->scaleConfig;
    MadpLostPeripheralList *lostItems  = &config->lostPeripheralList_;

    weigher1 = lostItems->firstWeigher();
    weigher2 = lostItems->secondWeigher();
    printer1 = lostItems->firstPrinter();
    printer2 = lostItems->secondPrinter();
    printer3 = lostItems->thirdPrinter();
    labeler1 = lostItems->labelApplier();
    wrapper1 = lostItems->wrapper();
    numeric1 = lostItems->numeric();
    marquee1 = lostItems->marquee();
    gcd1 = lostItems->gcd();

    if(weigher1 != 0)
        ui->gbFirstWeigher->setVisible( true );

    if(weigher2 != 0)
        ui->gbSecondWeigher->setVisible( true );

    if(printer1 != 0)
        ui->gbFirstPrinter->setVisible( true );

    if(printer2 != 0)
        ui->gbSecondPrinter->setVisible( true );

    if(printer3 != 0)
        ui->gbThirdPrinter->setVisible( true );

    if(labeler1 != 0)
        ui->gbLabelApplier->setVisible( true );

    if(wrapper1 != 0)
        ui->gbWrapper->setVisible( true );

    if(numeric1 != 0)
        ui->gbNumeric->setVisible( true );

    if(marquee1 != 0)
        ui->gbMarquee->setVisible( true );

    if(gcd1 != 0)
        ui->gbGcd->setVisible( true );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::peripheralListChanged(const SessionMessage &msg)
{
    MadpPeripheralList m( msg );

    if((weigher1 != 0) && (m.firstWeigher() != 0))
    {
		removeFirstWeigher();
	}

    if((weigher2 != 0) && (m.secondWeigher() != 0))
    {
		removeSecondWeigher();
	}

    if((printer1 != 0) && (m.firstPrinter() != 0))
    {
		removeFirstPrinter();
	}

    if((printer2 != 0) && (m.secondPrinter() != 0))
    {
		removeSecondPrinter();
	}

    if((printer3 != 0) && (m.thirdPrinter() != 0))
    {
		removeThirdPrinter();
	}

    if((labeler1 != 0) && (m.labelApplier() != 0))
    {
		removeLabelApplier();
	}

    if((wrapper1 != 0) && (m.wrapper() != 0))
    {
		removeWrapper();
	}

    if((numeric1 != 0) && (m.numeric() != 0))
    {
		removeNumeric();
	}

    if((marquee1 != 0) && (m.marquee() != 0))
    {
		removeMarquee();
	}

    if((gcd1 != 0) && (m.gcd() != 0))
    {
		removeGcd();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::removeFirstWeigher()
{
    deletePeripherial(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::removeSecondWeigher()
{
    deletePeripherial(1);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::removeFirstPrinter()
{
    deletePeripherial(2);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::removeSecondPrinter()
{
    deletePeripherial(3);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::removeThirdPrinter()
{
    deletePeripherial(4);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::removeLabelApplier()
{
    deletePeripherial(5);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::removeWrapper()
{
    deletePeripherial(6);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::removeNumeric()
{
    deletePeripherial(7);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::removeMarquee()
{
    deletePeripherial(8);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::removeGcd()
{
    deletePeripherial(9);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
void RemovePeripheralsDialog::onClickDone()
{
    // kill it
    //mainWindow_->closeRemovePeripherals();

    accept();                                                       // shut 'er down
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          MESSAGE CATCHER
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

void RemovePeripheralsDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    // while handling a lost perif message, a 138,20, I may end up receiving a regular perif message, 11,100
    if (( System == msg[0].toInt() ) && ( SysPeripheralList == msg[1].toInt() ))
    {
        // 11,100 - Update the peripheral list, which may have changed since we received the initial missing list

        peripheralListChanged(msg);
    }
    
        // call base handler
    _Mybase::onSessionMessageReceive( msg );
}



