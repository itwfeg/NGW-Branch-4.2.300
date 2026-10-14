/**
 * @file printPrompt.cpp
 *
 * @class printPrompt
 * @brief Operate mode - Press Print Icon beside a field on the PLU INFORMATION window
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Job E. Crew with some co-author assistance from Larry Pevoar
 */

#include "printPrompt.h"
#include "ui_printPrompt.h"
#include "../sessionmainwindow.h"

#include <Session.h>
#include <ScaleConfiguration.h>

#include <QtGui>

// Hobart namespace
namespace hobart {

printPrompt::printPrompt(QWidget *parent, QStringList numsList, QStringList nameList, QStringList typesList, QStringList driverList, SessionMainWindow *mainWindow ) :
    _Mybase( parent ),
    ui(new Ui::printPrompt)
{
    // may pass in the name of the printers as a list
    ui->setupUi(this);

    mainWindow_ = mainWindow;

    initialize( numsList, nameList, typesList, driverList );
}

printPrompt::~printPrompt()
{
    delete ui;
}

//-----------------------------------------------
// Only show certain fields on the initial screen
//-----------------------------------------------
void printPrompt::initialize( QStringList printerNumList, QStringList printerNameList, QStringList printerTypeList, QStringList printerDriversList )
{
    // Use these as the colors if I want to go gradient
    // static const QColor darkButton_L0( 192, 192, 192 );                 /// Dark button.
    // static const QColor darkButton_L1( 96, 96, 96 );                    /// Dark button.


    if ( printerNumList.length() > 0 )
    {
        ui->cboPrintMethod->clear();

        // If I have a network printer, I cannot add it to this list.
        // Need to test each printer individually for addability

        int type1 = printerTypeList.at(0).toInt();
        QString num1  = printerNumList.at(0);
        QString name1 = printerNameList.at(0);
        QString driver1 = printerDriversList.at(0);

        int type2 = 0;
        QString num2  = "";
        QString name2 = "";
        QString driver2 = "";

        // Irreguardless of what happens I need this

        ui->cboPrintMethod->addItem(tr ( "Primary" ) );     // always add this

        if ( printerNumList.length() >= 1 )
        {
            bool isZebraNetworkPrinter = false;
            if( type1 == NetworkPrinter && driver1.contains("zebra", Qt::CaseInsensitive) )
                isZebraNetworkPrinter = true;

            if ( type1 != NetworkPrinter || isZebraNetworkPrinter )
                ui->cboPrintMethod->addItem( name1, num1 );
        }

        if ( printerNumList.length() == 2 )
        {
            num2  = printerNumList.at(1);
            name2 = printerNameList.at(1);
            type2 = printerTypeList.at(1).toInt();
            driver2 = printerDriversList.at(1);

            bool isZebraNetworkPrinter = false;
            if( type2 == NetworkPrinter && driver2.contains("zebra", Qt::CaseInsensitive) )
                isZebraNetworkPrinter = true;

            if ( type2 != NetworkPrinter || isZebraNetworkPrinter )
                ui->cboPrintMethod->addItem(name2, num2);
        }
    }

    connect(ui->butDone,        SIGNAL(clicked()),                  this, SLOT(updateDone()));              // clicked Done button
    connect(ui->cboPrintMethod, SIGNAL(currentIndexChanged(int)),   this, SLOT(changedPrintPrompt(int)));   // selected item in combo

    translate();

    repaint();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool printPrompt::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void printPrompt::translate()
{
    ui->retranslateUi( this );

    ui->cboPrintMethod->setItemText(0, tr("Primary"));

    update();
}

void printPrompt::changedPrintPrompt(int index)
{
    Q_UNUSED( index );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void printPrompt::emitDoneClicked()
{
    int cboPrompt = ui->cboPrintMethod->currentIndex();

    int cboData = ui->cboPrintMethod->itemData(cboPrompt).toInt();

    emit printPromptDoneClicked( cboData );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void printPrompt::updateDone()
{
    // pass back to the plu info page the prompted selection printer

    emitDoneClicked();

    setVisible(false);
}

} // hobart namespace

