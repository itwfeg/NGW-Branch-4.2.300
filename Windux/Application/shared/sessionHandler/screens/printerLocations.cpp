

/* @file printerLocations.cpp
 * printerLocations
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala

   @Brief This file contains all the functions necessary for configuring printer locations
 */

#include "printerLocations.h"

#include "ui_printerLocations.h"

#include "../sessionmainwindow.h"

#include "../modes/servicemode.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QtCore>

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////
printerLocations::printerLocations(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::printerLocations)
{
    ui->setupUi(this);
    scaleScreen();

    //Update and Initialize the Page
    initialize();	//	HA-91. Set up the page BEFORE turning on the slots.

    //Setup Signals and Slots
    connect(ui->btnTestLabel1, SIGNAL( clicked()), this, SLOT(testLabel1()));
    connect(ui->btnTestLabel2, SIGNAL( clicked()), this, SLOT(testLabel2()));
    connect(ui->btnTestLabel3, SIGNAL( clicked()), this, SLOT(testLabel3()));
    connect(ui->btnUpdate, SIGNAL( clicked()), this, SLOT(updateLabel()));

    connect(ui->cboxPrintContrast1, SIGNAL( currentIndexChanged(int)), this, SLOT(printContrast1(int)));
    connect(ui->cboxPrintContrast2, SIGNAL( currentIndexChanged(int)), this, SLOT(printContrast2(int)));
    connect(ui->cboxPrintContrast3, SIGNAL( currentIndexChanged(int)), this, SLOT(printContrast3(int)));
    translate();

    //Turn on printer type tracking
    session_->send(Printer, PrPrinterTypeTracking, "0,1");//Printer, PrPrinterTypeTracking, PrimaryPrinter, On
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void printerLocations::initialize()
{
    //Add contrasts
    ui->cboxPrintContrast1->clear();
    ui->cboxPrintContrast1->addItem("0");
    ui->cboxPrintContrast1->addItem("1");
    ui->cboxPrintContrast1->addItem("2");
    ui->cboxPrintContrast1->addItem("3");
    ui->cboxPrintContrast1->addItem("4");
    ui->cboxPrintContrast1->addItem("5");
    ui->cboxPrintContrast1->addItem("6");
    ui->cboxPrintContrast1->addItem("7");

    ui->cboxPrintContrast2->clear();
    ui->cboxPrintContrast2->addItem("0");
    ui->cboxPrintContrast2->addItem("1");
    ui->cboxPrintContrast2->addItem("2");
    ui->cboxPrintContrast2->addItem("3");
    ui->cboxPrintContrast2->addItem("4");
    ui->cboxPrintContrast2->addItem("5");
    ui->cboxPrintContrast2->addItem("6");
    ui->cboxPrintContrast2->addItem("7");

    ui->cboxPrintContrast3->clear();
    ui->cboxPrintContrast3->addItem("0");
    ui->cboxPrintContrast3->addItem("1");
    ui->cboxPrintContrast3->addItem("2");
    ui->cboxPrintContrast3->addItem("3");
    ui->cboxPrintContrast3->addItem("4");
    ui->cboxPrintContrast3->addItem("5");
    ui->cboxPrintContrast3->addItem("6");
    ui->cboxPrintContrast3->addItem("7");

    //Add printer types
    ui->cboxPrinterType1->clear();
    ui->cboxPrinterType2->clear();
    ui->cboxPrinterType3->clear();

    ui->cboxPrinterType1->addItem(tr("Primary"));
    ui->cboxPrinterType2->addItem(tr("Primary"));
    ui->cboxPrinterType3->addItem(tr("Primary"));

    ui->cboxPrinterType1->addItem(tr("Second"));
    ui->cboxPrinterType2->addItem(tr("Second"));
    ui->cboxPrinterType3->addItem(tr("Second"));

    ui->cboxPrinterType1->addItem(tr("Third"));
    ui->cboxPrinterType2->addItem(tr("Third"));
    ui->cboxPrinterType3->addItem(tr("Third"));

    //Add label patterns
    ui->cboxLabelPattern->addItem(tr("Checker Board"));
    ui->cboxLabelPattern->addItem(tr("Horizontal Bar"));
    ui->cboxLabelPattern->addItem(tr("Vertical Lines"));
    ui->cboxLabelPattern->addItem(tr("High Power"));

    disableAll();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
printerLocations::~printerLocations()
{
    delete ui;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool printerLocations::eventFilter( QObject *obj, QEvent *event )
{
    return _Mybase::eventFilter( obj, event );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void printerLocations::translate()
{
    int currIndexPrinterType1 = ui->cboxPrinterType1->currentIndex();
    int currIndexPrinterType2 = ui->cboxPrinterType2->currentIndex();
    int currIndexPrinterType3 = ui->cboxPrinterType3->currentIndex();

    ui->retranslateUi(this);
    //Update the text on the cboxes
    //Printer Types
    ui->cboxPrinterType1->setItemText(0, tr("Primary"));
    ui->cboxPrinterType1->setItemText(1, tr("Second"));
    ui->cboxPrinterType1->setItemText(2, tr("Third"));

    ui->cboxPrinterType2->setItemText(0, tr("Primary"));
    ui->cboxPrinterType2->setItemText(1, tr("Second"));
    ui->cboxPrinterType2->setItemText(2, tr("Third"));

    ui->cboxPrinterType3->setItemText(0, tr("Primary"));
    ui->cboxPrinterType3->setItemText(1, tr("Second"));
    ui->cboxPrinterType3->setItemText(2, tr("Third"));

    if(currIndexPrinterType1 == 3)
        ui->cboxPrinterType1->setItemText(3, tr("No Response"));
    if(currIndexPrinterType2 == 3)
        ui->cboxPrinterType2->setItemText(3, tr("No Response"));
    if(currIndexPrinterType3 == 3)
        ui->cboxPrinterType3->setItemText(3, tr("No Response"));

    //Label Pattern
    ui->cboxLabelPattern->setItemText(0, tr("Checker Board"));
    ui->cboxLabelPattern->setItemText(1, tr("Horizontal Bar"));
    ui->cboxLabelPattern->setItemText(2, tr("Vertical Lines"));
    ui->cboxLabelPattern->setItemText(3, tr("High Power"));

    //Translate
    //ui->retranslateUi(this);

    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool printerLocations::onScreenChanging()
{
    //Turn off printer type tracking
    session_->send(Printer, PrPrinterTypeTracking, "0,0");
    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void printerLocations::disableAll()
{
    disablePrinter(PrimaryPrinter);
    disablePrinter(SecondaryPrinter);
    disablePrinter(TertiaryPrinter);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void printerLocations::enableAll()
{
    enablePrinter(PrimaryPrinter);
    enablePrinter(SecondaryPrinter);
    enablePrinter(TertiaryPrinter);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void printerLocations::enablePrinter(int printerNum)
{
    switch( printerNum )
    {
        case 0://Printer 1
            ui->cboxPrinterType1->setEnabled(true);
            ui->cboxPrintContrast1->setEnabled(true);
            ui->btnTestLabel1->setEnabled(true);
            ui->cboxPrinterType1->setCurrentIndex(PrimaryPrinter);
            ui->cboxPrinterType1->removeItem(3);
            break;
        case 1://Printer 2
            ui->cboxPrinterType2->setEnabled(true);
            ui->cboxPrintContrast2->setEnabled(true);
            ui->btnTestLabel2->setEnabled(true);
            ui->cboxPrinterType2->setCurrentIndex(SecondaryPrinter);
            ui->cboxPrinterType2->removeItem(3);
            break;
        case 2://Printer 3
            ui->cboxPrinterType3->setEnabled(true);
            ui->cboxPrintContrast3->setEnabled(true);
            ui->btnTestLabel3->setEnabled(true);
            ui->cboxPrinterType3->setCurrentIndex(TertiaryPrinter);
            ui->cboxPrinterType3->removeItem(3);
            break;
        default:
            //do nothing
            return;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void printerLocations::disablePrinter(int printerNum)
{
    setNoResponse(printerNum);
    switch( printerNum )
    {
        case 0://Printer 1
            ui->cboxPrinterType1->setEnabled(false);
            ui->cboxPrintContrast1->setEnabled(false);
            ui->cboxPrintContrast1->blockSignals(true);
            ui->cboxPrintContrast1->setCurrentIndex(0);
            ui->cboxPrintContrast1->blockSignals(false);
            ui->btnTestLabel1->setEnabled(false);
            break;
        case 1://Printer 2
            ui->cboxPrinterType2->setEnabled(false);
            ui->cboxPrintContrast2->setEnabled(false);
            ui->cboxPrintContrast2->blockSignals(true);
            ui->cboxPrintContrast2->setCurrentIndex(0);
            ui->cboxPrintContrast2->blockSignals(false);
            ui->btnTestLabel2->setEnabled(false);
            break;
        case 2://Printer 3
            ui->cboxPrinterType3->setEnabled(false);
            ui->cboxPrintContrast3->setEnabled(false);
            ui->cboxPrintContrast3->blockSignals(true);
            ui->cboxPrintContrast3->setCurrentIndex(0);
            ui->cboxPrintContrast3->blockSignals(false);
            ui->btnTestLabel3->setEnabled(false);
            break;
        default:
            //do nothing
            return;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void printerLocations::setNoResponse(int printerNum)
{
    switch( printerNum )
    {
    case 0:
        if(ui->cboxPrinterType1->count() < 4)
        {
            ui->cboxPrinterType1->addItem(tr("No Response"));
        }
        ui->cboxPrinterType1->setCurrentIndex(3);
        break;
    case 1:
        if(ui->cboxPrinterType2->count() < 4)
        {
            ui->cboxPrinterType2->addItem(tr("No Response"));
        }
        ui->cboxPrinterType2->setCurrentIndex(3);
        break;
    case 2:
        if(ui->cboxPrinterType3->count() < 4)
        {
            ui->cboxPrinterType3->addItem(tr("No Response"));
        }
        ui->cboxPrinterType3->setCurrentIndex(3);
        break;
    default:
        return;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void printerLocations::testLabel1()
{
    QString msg = "";
    msg += QString::number(PrimaryPrinter) + ",";
    //Offset of 2 because checkerBoard=2,horizontalBars=3,verticalLines=4 in PrTest
    msg += QString::number((ui->cboxLabelPattern->currentIndex()+2)) + ",";
    msg += "0";

    session_->send(Printer, PrTestLabel, msg);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void printerLocations::testLabel2()
{
    QString msg = "";
    msg += QString::number(SecondaryPrinter) + ",";
    //Offset of 2 because checkerBoard=2,horizontalBars=3,verticalLines=4 in PrTest
    msg += QString::number((ui->cboxLabelPattern->currentIndex()+2)) + ",";
    msg += "0";

    session_->send(Printer, PrTestLabel, msg);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void printerLocations::testLabel3()
{
    QString msg = "";
    msg += QString::number(TertiaryPrinter) + ",";
    //Offset of 2 because checkerBoard=2,horizontalBars=3,verticalLines=4 in PrTest
    msg += QString::number((ui->cboxLabelPattern->currentIndex()+2)) + ",";
    msg += "0";

    session_->send(Printer, PrTestLabel, msg);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void printerLocations::updateLabel()
{
    QString msg = "";

    //Printer 1
    msg += QString::number(ui->cboxPrinterType1->currentIndex()) + ",";
    msg += primaryPrinterMemAddr + ",";

    //Printer 2
    msg += QString::number(ui->cboxPrinterType2->currentIndex()) + ",";
    msg += secondaryPrinterMemAddr + ",";

    //Printer 3
    msg += QString::number(ui->cboxPrinterType3->currentIndex()) + ",";
    msg += tertiaryPrinterMemAddr;

    //Disable all printers so we only get a valid config back
    disableAll();
    session_->send(Printer, PrSetPrinters, msg);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void printerLocations::printContrast1(int index)
{
    QString msg = "";
    //Add printerNum and the selected contrast
    msg += QString::number(PrimaryPrinter) + "," + QString::number(index);
    session_->send(Printer, PrConfig, msg);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void printerLocations::printContrast2(int index)
{
    QString msg = "";
    //Add printerNum and the selected contrast
    msg += QString::number(SecondaryPrinter) + "," + QString::number(index);
    session_->send(Printer, PrConfig, msg);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void printerLocations::printContrast3(int index)
{
    QString msg = "";
    //Add printerNum and the selected contrast
    msg += QString::number(TertiaryPrinter) + "," + QString::number(index);
    session_->send(Printer, PrConfig, msg);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void printerLocations::setPrinterContrast(int printerType, int contrast)
{
    switch( printerType )
    {
    case 0:
        if(validContrast(contrast))
        {
            ui->cboxPrintContrast1->blockSignals(true);
            ui->cboxPrintContrast1->setCurrentIndex(contrast);
            ui->cboxPrintContrast1->blockSignals(false);
        }
        break;
    case 1:
        if(validContrast(contrast))
        {
            ui->cboxPrintContrast2->blockSignals(true);
            ui->cboxPrintContrast2->setCurrentIndex(contrast);
            ui->cboxPrintContrast2->blockSignals(false);
        }
        break;
    case 2:
        if(validContrast(contrast))
        {
            ui->cboxPrintContrast3->blockSignals(true);
            ui->cboxPrintContrast3->setCurrentIndex(contrast);
            ui->cboxPrintContrast3->blockSignals(false);
        }
        break;
    default:
        break;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool printerLocations::validContrast(int contrast)
{
    return (contrast >= 0 && contrast <= 7);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool printerLocations::validPrinterLocation(int printerLocation)
{
    return (printerLocation >=0 && printerLocation <=4);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void printerLocations::setPrinterType(int printerType, QString printerLocation)
{
    switch( printerType )
        {
            case 0:
                primaryPrinterMemAddr = printerLocation;
                enablePrinter(PrimaryPrinter);
                session_->send(Printer, PrReqInquire, QString::number(PrimaryPrinter));
                break;
            case 1:
                secondaryPrinterMemAddr = printerLocation;
                enablePrinter(SecondaryPrinter);
                session_->send(Printer, PrReqInquire, QString::number(SecondaryPrinter));
                break;
            case 2:
                tertiaryPrinterMemAddr = printerLocation;
                enablePrinter(TertiaryPrinter);
                session_->send(Printer, PrReqInquire, QString::number(TertiaryPrinter));
                break;
            default:
                break;
        }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void printerLocations::onSessionMessage( const SessionMessage& msg )
{
    if(Printer == msg[0].toInt() && PrPrinterType == msg[1].toInt())
    {
        //Save parsing of comma delimited message in case it ever changes
        DomPrinterLocations m(msg);

        //Set the printer type
        setPrinterType(m.printerType().toInt(), m.printerAddress());
    }
    else if(Printer == msg[0].toInt() && PrInquire == msg[1].toInt())
    {
        //Save parsing of comma delimited message in case it ever changes
        DomPrinterConfigRead m(msg);

        //Update the printer's contrasts
        setPrinterContrast(m.printerType(), m.printContrast());
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool printerLocations::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    return _Mybase::isErrorMessageHandler( errorMsg );
}
