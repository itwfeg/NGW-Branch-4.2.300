
/**
 * @file digitEntry.cpp
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe E. Crew
 */

#include <QtGui>
#include <QtCore>
#include <QDebug>

#include "digitEntry.h"
#include "ui_digitEntry.h"

#include "../sessioninfo.h"

#include "../modes/supervisormode.h"

#include <Session.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

// Hobart namespace
namespace hobart {

digitEntry::digitEntry(QWidget *parent, SessionMainWindow *mainWindow ) :
    _Mybase( parent ),
    ui(new Ui::digitEntry)
{
    ui->setupUi(this);

    mainWindow_ = mainWindow;

    initialize();

}

digitEntry::~digitEntry()
{
    delete ui;
}

//-----------------------------------------------
// Only show certain fields on the initial screen
//-----------------------------------------------
void digitEntry::initialize()
{

    // Had a signal mapper but it was twice as much code as making a connect for each button so
    // I figured whats the point and I deleted it.

    connect(ui->butNum_0,       SIGNAL(clicked()),      this,   SLOT(updateEntering_0()));        // clicked 0
    connect(ui->butNum_1,       SIGNAL(clicked()),      this,   SLOT(updateEntering_1()));        // clicked 1
    connect(ui->butNum_2,       SIGNAL(clicked()),      this,   SLOT(updateEntering_2()));        // clicked 2
    connect(ui->butNum_3,       SIGNAL(clicked()),      this,   SLOT(updateEntering_3()));        // clicked 3
    connect(ui->butNum_4,       SIGNAL(clicked()),      this,   SLOT(updateEntering_4()));        // clicked 4
    connect(ui->butNum_5,       SIGNAL(clicked()),      this,   SLOT(updateEntering_5()));        // clicked 5
    connect(ui->butNum_6,       SIGNAL(clicked()),      this,   SLOT(updateEntering_6()));        // clicked 6
    connect(ui->butNum_7,       SIGNAL(clicked()),      this,   SLOT(updateEntering_7()));        // clicked 7
    connect(ui->butNum_8,       SIGNAL(clicked()),      this,   SLOT(updateEntering_8()));        // clicked 8
    connect(ui->butNum_9,       SIGNAL(clicked()),      this,   SLOT(updateEntering_9()));        // clicked 9

    connect(ui->butNum_X,       SIGNAL(clicked()),      this,   SLOT(updateDoneEnteringDigits()));        // clicked Done
    connect(ui->butNum_C,       SIGNAL(clicked()),      this,   SLOT(updateEnteringClear()));             // clicked Clear
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                       PRIVATE METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
void digitEntry::updateEnteringDigits(QString digits)
{
    // send a digit to the original form calling the method to update its field

    emit digitEntryNumberClicked( digits );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void digitEntry::updateEntering_0()
{
    updateEnteringDigits("0");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void digitEntry::updateEntering_1()
{
    updateEnteringDigits("1");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void digitEntry::updateEntering_2()
{
    updateEnteringDigits("2");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void digitEntry::updateEntering_3()
{
    updateEnteringDigits("3");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void digitEntry::updateEntering_4()
{
    updateEnteringDigits("4");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void digitEntry::updateEntering_5()
{
    updateEnteringDigits("5");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void digitEntry::updateEntering_6()
{
    updateEnteringDigits("6");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void digitEntry::updateEntering_7()
{
    updateEnteringDigits("7");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void digitEntry::updateEntering_8()
{
    updateEnteringDigits("8");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void digitEntry::updateEntering_9()
{
    updateEnteringDigits("9");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void digitEntry::updateEnteringClear()
{
    emit digitEntryClearClicked();                                  // clicked Clear
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void digitEntry::updateDoneEnteringDigits()
{
    emit digitEntryDoneClicked();                                   // clicked Done

    setVisible(false);
}



} // hobart namespace
