/**
 * @file datamatrixEditor.cpp
 *
 * @copyright Copyright (c) 2018, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe E. Crew
 */

#include "ui_datamatrixEditor.h"
#include "datamatrixEditor.h"

#include "../sessioninfo.h"
#include "../modes/supervisormode.h"

#include <logmanager.h>

#include <QDebug>

//#include <QtGui>
//#include <QtCore>
#include <Session.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <QDebug>
#include <QSignalMapper>
#include <QToolTip>

// Hobart namespace
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
datamatrixEditor::datamatrixEditor(const QString &initialFormatStr, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::datamatrixEditor),
    signalMapper_( NULL ),
    datamatrixString_( initialFormatStr )
{
    LOG_CLASS_INFO( "datamatrixEditor", "datamatrixEditor" );

    ui->setupUi(this);

    // hook into session
    sessionInfo info( this );

    if ( !(mainWindow_ = info()) )
            LOG_WARN( "could not retrieve main window!", NULL );

    initialize();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
datamatrixEditor::~datamatrixEditor()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void datamatrixEditor::translate()
{
    ui->butCancel->setText(tr("Cancel"));
    ui->butDone->setText(tr("Done"));
    ui->butClear->setText(tr("Clear"));

    ui->butBestBefore->setText( tr( "B : Best Before" ) );
    ui->butSellBy->setText( tr( "C : Sell By" ) );
    ui->butDateTime->setText( tr( "D : Current Date/Time" ) );
    ui->butDeptNum->setText( tr( "d : Department Number" ) );
    ui->butEanCode->setText( tr( "E : Ean Code" ) );
    ui->butFormattedEAN->setText( tr( "e : Formatted EAN Code" ) );
    ui->butFormatWtOrBc->setText( tr( "F : Formatted Wt/BC" ) );
    ui->butFormatTare->setText( tr( "f : Formatted Tare" ) );
    ui->butDescription->setText( tr( "I : Description" ) );
    ui->butPriceCheckDigit5->setText( tr( "K : Price Check Digit (5)" ) );
    ui->butWeightCheckDigit5->setText( tr( "k : Weight Check Digit (5)" ) );
    ui->butPluLookupNum->setText( tr( "L : PLU Lookup Number" ) );
    ui->butPriceDecs->setText( tr( "M : Price decimals" ) );
    ui->butProductNum->setText( tr( "N : Product Number" ) );
    ui->butUnitPrice->setText( tr( "O : Unit Price" ) );
    ui->butOrigUnitPrice->setText( tr( "o : Original Unit Price" ) );
    ui->butPrice->setText( tr( "P : Price" ) );
    ui->butPriceCheckDigit4->setText( tr( "p : Price Check Digit (4)" ) );
    ui->butByCount->setText( tr( "Q : By Count" ) );
    ui->butTareWt->setText( tr( "R : Tare Weight" ) );
    ui->butScaleId->setText( tr( "S : Scale Id" ) );
    ui->butTransNum->setText( tr( "T : Transaction Number" ) );
    ui->butFormattedUPC->setText( tr( "u : Formatted UPC Code" ) );
    ui->butVendorNum->setText( tr( "V : Vendor Number" ) );
    ui->butWeight->setText( tr( "W : Weight" ) );
    ui->butWeightCheckDigit4->setText( tr( "w : Weight Check Digit (4)" ) );
    ui->butBarcodePrefix->setText( tr( "X : Barcode Prefix" ) );
    ui->butItemType->setText( tr( "Y : Item Type" ) );
    ui->butWtDecs->setText( tr( "Z : Weight decimals" ) );
    ui->butWeightType->setText( tr( "z : Weight Type" ) );
    ui->butPercent->setText( tr( "% : Format Specifier" ) );
    ui->butFlags->setText( tr( "FLAGS" ) );
    ui->butDash->setText( tr( "- : Dash" ) );
    ui->butZero->setText( tr( "0 : Zero" ) );
    ui->butWidth->setText( tr( "WIDTH" ) );
    ui->butWidthNum->setText( tr( "( number )" ) );
    ui->butPrecision->setText( tr( "PRECISION" ) );
    ui->butPrecisionNum->setText( tr( ".number" ) );
    ui->butSpecial->setText( tr( "SPECIAL" ) );
    ui->butFNC1->setText( tr( "^ : FNC1" ) );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void datamatrixEditor::initialize()
{
    connect(ui->butClear,           SIGNAL(clicked(bool)),      this, SLOT(onButtonClicked()));
    connect(ui->butDone,            SIGNAL(clicked(bool)),      this, SLOT(onButtonClicked()));          // clicked Done
    connect(ui->butCancel,          SIGNAL(clicked(bool)),      this, SLOT(onButtonClicked()));

    // create signal mapper

    signalMapper_ = new QSignalMapper( this );
    connect( signalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onInstructionClicked(QWidget*) ) );

    QList<QPushButton*> tooltipButtons = this->findChildren<QPushButton*>();

    foreach (QPushButton *tip, tooltipButtons)
    {
        signalMapper_->setMapping( tip, tip );
        connect( tip, SIGNAL( clicked() ), signalMapper_, SLOT( map() ) );
    }

    ui->txtFormatString->setText( datamatrixString_ );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void datamatrixEditor::onButtonClicked()
{
    const QString formatStr( ui->txtFormatString->toPlainText() );

    if ( sender() == ui->butDone )
    {
        emit doneClickedDM( formatStr );
    }
    else if( sender() == ui->butCancel )
    {
        emit cancelClickedDM();
    }
    else if( sender() == ui->butClear )
    {
        ui->txtFormatString->clear();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void datamatrixEditor::onInstructionClicked( QWidget *widget)
{
    //QToolTip::showText(ui->butBestBefore->mapToGlobal(QPoint()), "Clicked B", ui->butBestBefore);

    QString tip = widget->toolTip();
    QToolTip::showText(widget->mapToGlobal( QPoint(0,50 )), tip, widget);
}

} // hobart namespace
