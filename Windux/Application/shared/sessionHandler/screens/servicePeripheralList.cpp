/**
 * @file servicePeripheralList.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "servicePeripheralList.h"
#include "ui_servicePeripheralList.h"

#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"

#include "../sessionmainwindow.h"


servicePeripheralList::servicePeripheralList(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::servicePeripheralList),
    weigherUpdated( false ),
    printerUpdated( false ),
    wrapperUpdated( false )
{
    ui->setupUi(this);
    scaleScreen();

    tableRow = 1;
    primaryPrinterListed = false;
    secondaryPrinterListed = false;
    tertiaryPrinterListed = false;
    weigherListed = false;
    labelApplierListed = false;
    weigherListed = false;
    graphicalCustomerDisplayListed = false;
    marqueeListed = false;
    numericListed = false;
    wrapperListed = false;
    evolWrapperListed = false;

    weigherRowNumber = -1;
    wrapperRowNumber = -1;
    evolWrapperRowNumber = -1;
    labelApplierRowNumber = -1;
    primaryPrinterRowNumber = -1;
    secondaryPrinterRowNumber = -1;
    tertiaryPrinterRowNumber = -1;
    numericRowNumber = -1;
    marqueeRowNumber = -1;
    graphicalCustomerDisplayRowNumber = -1;
    resultsTableDeviceNamesForTranslation.clear();

    for(int i=0; i < 16; i++)
    {
        resultsTableDeviceNamesForTranslation.append("");
        resultsTableDeviceTypesForTranslation.append("");
    }

    // Adjust column widths
    ui->tblResults->setColumnWidth( 0, scaleWidthForScreen( 420 ) );
    ui->tblResults->setColumnWidth( 1, scaleWidthForScreen( 150 ) );
    ui->tblResults->setColumnWidth( 2, scaleWidthForScreen( 150 ) );
    ui->tblResults->setColumnWidth( 3, scaleWidthForScreen( 150 ) );

    //-------------------------
    // Request current peripheral list
    //-------------------------
    session_->send( System, SysReqPeripheralList );	//11, 101

    //-------------------------
    // Request the Weigher info
    //-------------------------
    session_->send( WeigherSystem, SysReqStatus ); // 2,13

    //----------------------------
    // Request the Printer #1 info
    //----------------------------
    session_->send( PrinterSystem, PrSysReqStatus, "0" ); // 8,13,0

    //----------------------------
    // Request the Printer #2 info
    //----------------------------
    session_->send( PrinterSystem, PrSysReqStatus, "1" ); // 8,13,1

    //----------------------------
    // Request the Printer #3 info
    //----------------------------
    session_->send( PrinterSystem, PrSysReqStatus, "2" ); // 8,13,2

    //-------------------------------
    // Request the Label Applier info
    //-------------------------------
    session_->send( LabelApplierSystem, LaSysReqStatus ); // 6,13

    //-------------------------
    // Request the Numeric info
    //-------------------------
    session_->send( NumericSystem, NmSysReqStatus ); // 142,13

    //-------------------------
    // Request the Marquee info
    //-------------------------
    session_->send( MarqueeSystem, MqSysReqStatus ); // 143,13

    //-------------------------
    // Request the Wrapper info
    //-------------------------
    session_->send( WrapperSystem, WrSysReqStatus ); // 10,13

    //---------------------
    // Request the GCD info
    //---------------------
    session_->send( GCDSessionDev, GcdSysReqStatus ); // 161,13

    translate();
}

servicePeripheralList::~servicePeripheralList()
{
    delete ui;
}

void servicePeripheralList::translate()
{
    updateTranslationInResultsTable();
    ui->retranslateUi(this);
    update();
}

void servicePeripheralList::onSessionMessage( const SessionMessage& msg )
{
    MadpPeripheralList *periphSettings = &session_->scaleConfig->peripheralList;
    //int nextRow = ui->tblResults->rowCount()-1;
    bool updateTheTable = true;

    //--------------------
    // Weigher Information
    //--------------------
    if ((WeigherSystem == msg[0].toInt() ) && ( SysUsbWgStatus == msg[1].toInt() ))
    {
        if ( weigherListed )
            return;

        weigherListed = true;

        DomUsbWeigherPowerupTests m( msg );

        const QString firstWeigherKind = QString::number( periphSettings->firstWeigher() );

        weigherRowNumber = tableRow;
        resultsTableDeviceNamesForTranslation.replace(tableRow, "Weigher");
        resultsTableDeviceTypesForTranslation.replace(tableRow, setWeigherKindText(firstWeigherKind));
        updateResultsTable(tr( "Weigher" ), setWeigherKindText(firstWeigherKind), m.softwareVersion(), m.hardwareVersion() );
    }

    //--------------------
    // Printer Information
    //--------------------
    if ((PrinterSystem == msg[0].toInt() ) && ( PrSysStatus == msg[1].toInt() ))
    {
        if( printerUpdated )
            return;

        printerUpdated = true;

        DomPrinterPowerupTests m( msg );

        QString printerType = m.printerType();
        QString printerKind;

        // There could be up to 3 printers ...
        if(printerType == "0")
        {
            if(primaryPrinterListed)
                updateTheTable = false;
            else
            {
                printerKind = periphSettings->firstPrinterRaw();
                primaryPrinterRowNumber = tableRow;
                primaryPrinterListed = true;
                resultsTableDeviceNamesForTranslation.replace(tableRow,"Primary Printer");
                resultsTableDeviceTypesForTranslation.replace(tableRow, setPrinterKindText(printerKind));
            }
        }
        else if(printerType == "1")
        {
            if(secondaryPrinterListed)
                updateTheTable = false;
            else
            {
                printerKind = periphSettings->secondPrinterRaw();
                secondaryPrinterRowNumber = tableRow;
                secondaryPrinterListed = true;
                resultsTableDeviceNamesForTranslation.replace(tableRow,"Second Printer");
                resultsTableDeviceTypesForTranslation.replace(tableRow, setPrinterKindText(printerKind));
            }
        }
        else if(printerType == "2")
        {
            if(tertiaryPrinterListed)
                updateTheTable = false;
            else
            {
                printerKind = periphSettings->thirdPrinterRaw();
                tertiaryPrinterRowNumber = tableRow;
                tertiaryPrinterListed = true;
                resultsTableDeviceNamesForTranslation.replace(tableRow,"Third Printer");
                resultsTableDeviceTypesForTranslation.replace(tableRow, setPrinterKindText(printerKind));
            }
        }

        if (updateTheTable)
        {
            QString sofware = m.softwareVersion();
            if( ( "" == sofware ) || ( "0.0" == sofware ) || ( "0" == sofware ) )
                sofware = "0.0.0";

            updateResultsTable(setPrinterNameText(printerType), setPrinterKindText(printerKind), sofware, m.hardwareVersion() );
        }
    }

    //--------------------
    // Numeric Information
    //--------------------
    if ((NumericSystem == msg[0].toInt() ) && ( NmStatus == msg[1].toInt() ))
    {
        if ( numericListed )
            return;

        numericListed = true;

        DomNumericPowerupTests m( msg );

        const QString numericKind = periphSettings->numericRaw();

        resultsTableDeviceNamesForTranslation.replace(tableRow,"Numeric");
        resultsTableDeviceTypesForTranslation.replace(tableRow, setNumericKindText(numericKind));
        numericRowNumber = tableRow;
        updateResultsTable(tr( "Numeric" ), setNumericKindText(numericKind), m.softwareVersion(), m.hardwareVersion() );
    }

    //--------------------
    // Marquee Information
    //--------------------
    if ((MarqueeSystem == msg[0].toInt() ) && ( MqStatus == msg[1].toInt() ))
    {
        if ( marqueeListed )
            return;

        marqueeListed = true;

        DomMarqueePowerupTests m( msg );

        const QString marqueeKind = periphSettings->marqueeRaw();

        marqueeRowNumber = tableRow;
        resultsTableDeviceNamesForTranslation.replace(tableRow,"Marquee");
        resultsTableDeviceTypesForTranslation.replace(tableRow, setMarqueeKindText(marqueeKind));
        updateResultsTable(tr( "Marquee" ), setMarqueeKindText(marqueeKind), m.softwareVersion(), m.hardwareVersion() );
    }

    //----------------
    // GCD Information
    //----------------
    if ((GCDSessionDev == msg[0].toInt() ) && ( GcdSessionStatus == msg[1].toInt() ))
    {
        if ( graphicalCustomerDisplayListed )
            return;

        graphicalCustomerDisplayListed = true;

        DomGcdPowerupTests m( msg );

        const QString gcdKind = periphSettings->gcdRaw();

        graphicalCustomerDisplayRowNumber = tableRow;
        resultsTableDeviceNamesForTranslation.replace(tableRow,"Graphical Customer Display");
        resultsTableDeviceTypesForTranslation.replace(tableRow, setGcdKindText(gcdKind));
        updateResultsTable(tr( "Graphical Customer Display" ), setGcdKindText(gcdKind), m.gcdMGRVersion(), m.gcdBoardVersion() );
    }

    //--------------------
    // Wrapper Information
    //--------------------
    if ((WrapperSystem == msg[0].toInt() ) && ( WrSysStatus == msg[1].toInt() ))
    {
        if ( wrapperListed )
            return;

        wrapperListed = true;

        DomWrapperPowerUpTests m( msg );

        const QString wrapperKind = periphSettings->wrapperRaw();

        wrapperRowNumber = tableRow;
        resultsTableDeviceNamesForTranslation.replace(tableRow,"Wrapper");
        resultsTableDeviceTypesForTranslation.replace(tableRow, setWrapperKindText(wrapperKind));
        updateResultsTable(tr( "Wrapper" ), setWrapperKindText(wrapperKind), m.storedChecksum(), tr( "Unknown") );
    }

    //------------------------
    // EvolWrapper Information
    //------------------------
    if ((WrapperSystem == msg[0].toInt() ) && ( SysEvolWrSysStatus == msg[1].toInt() ))
    {
        if ( evolWrapperListed )
            return;

        evolWrapperListed = true;

        DomEvolWrapperSystemStatus m( msg );

        const QString wrapperKind = periphSettings->wrapperRaw();
        const QVector<QString> appVersionList = TokenParser::splitStringTokens( m.appVersionList() );

        //Add wrapper to device list
        evolWrapperRowNumber = tableRow;
        resultsTableDeviceNamesForTranslation.replace(tableRow,"Wrapper");
        resultsTableDeviceTypesForTranslation.replace(tableRow, "");

        updateResultsTable(tr( "Wrapper" ), "", "", "" );

        //Now add the nodes to the list
        for(int i = 0; i < appVersionList.count(); ++i)
        {
            QString tmpAppVer = appVersionList.at(i);

            if(appVersionList.at(i) != QString::null)
            {
                evolWrapperRowNumber = tableRow;
                if( 0 == i )
                {
                    resultsTableDeviceNamesForTranslation.replace(tableRow,"    Camera " );
                    resultsTableDeviceTypesForTranslation.replace(tableRow, setWrapperKindText(wrapperKind));
                }
                else
                {
                    resultsTableDeviceNamesForTranslation.replace(tableRow,"    Node " + QString::number(i));
                    resultsTableDeviceTypesForTranslation.replace(tableRow, setWrapperKindText(wrapperKind));
                }

                // because the camera is in Node 0, but is in position 2 in applist, and vice versa
                if( 0 == i )
                    updateResultsTable(tr( "    Camera " ),                    setWrapperKindText(wrapperKind), appVersionList.at(2), "" );
                else if( 2 == i )
                    updateResultsTable(tr( "    Node " ) + QString::number(i), setWrapperKindText(wrapperKind), appVersionList.at(0), "" );
                else
                    updateResultsTable(tr( "    Node " ) + QString::number(i), setWrapperKindText(wrapperKind), appVersionList.at(i), "" );
            }
        }
    }

    //--------------------------
    // Label Applier Information
    //--------------------------
    if (((LabelApplierSystem == msg[0].toInt() ) && ( LaSysStatus == msg[1].toInt() )) ||
        ((LabelApplier == msg[0].toInt() ) && ( LaCLASysStatus == msg[1].toInt() )))
    {
        if ( labelApplierListed )
            return;

        labelApplierListed = true;

        DomLabelApplierPowerUpTests m( msg );

        const QString labelApplierKind = periphSettings->labelApplierRaw();

        labelApplierRowNumber = tableRow;
        resultsTableDeviceNamesForTranslation.replace(tableRow,"Label Applier");
        resultsTableDeviceTypesForTranslation.replace(tableRow, setLabelApplierKindText(labelApplierKind));
        updateResultsTable(tr( "Label Applier" ), setLabelApplierKindText(labelApplierKind), m.masterStoredChecksum(), tr( "Unknown") );
    }

}

void servicePeripheralList::updateTranslationInResultsTable()
{
    QString textToTranslate;
    QString typeToTranslate;

    //qDebug() << "tblResults has this many rows in it: " << QString::number(ui->tblResults->rowCount());
    for(int i=0; i < ui->tblResults->rowCount(); i++)
    {
        textToTranslate = resultsTableDeviceNamesForTranslation.at(i);
        typeToTranslate = resultsTableDeviceTypesForTranslation.at(i);

        if(textToTranslate == "Weigher")
        {
            ui->tblResults->setItem(i,0, new QTableWidgetItem(tr("Weigher")));
        }
        else if(textToTranslate == "Primary Printer")
        {
            ui->tblResults->setItem(i,0, new QTableWidgetItem(tr("Primary Printer")));
        }
        else if(textToTranslate == "Second Printer")
        {
            ui->tblResults->setItem(i,0, new QTableWidgetItem(tr("Second Printer")));
        }
        else if(textToTranslate == "Third Printer")
        {
            ui->tblResults->setItem(i,0, new QTableWidgetItem(tr("Third Printer")));
        }
        else if(textToTranslate == "Numeric")
        {
            ui->tblResults->setItem(i,0, new QTableWidgetItem(tr("Numeric")));
        }
        else if(textToTranslate == "Marquee")
        {
            ui->tblResults->setItem(i,0, new QTableWidgetItem(tr("Marquee")));
        }
        else if(textToTranslate == "Graphical Customer Display")
        {
            ui->tblResults->setItem(i,0, new QTableWidgetItem(tr("Graphical Customer Display")));
        }
        else if(textToTranslate == "Wrapper")
        {
            ui->tblResults->setItem(i,0, new QTableWidgetItem(tr("Wrapper")));
        }
        else if(textToTranslate == "Label Applier")
        {
            ui->tblResults->setItem(i,0, new QTableWidgetItem(tr("Label Applier")));
        }
        else if(textToTranslate == "Unknown")
        {
            ui->tblResults->setItem(i,0, new QTableWidgetItem(tr("Unknown")));
        }
        else if(textToTranslate == "    Node ")
        {
            ui->tblResults->setItem(i,0,new QTableWidgetItem(tr("    Node ")));
        }
        else if(textToTranslate == "    Camera ")
        {
            ui->tblResults->setItem(i,0,new QTableWidgetItem(tr("    Camera ")));
        }

        if(typeToTranslate == "Unknown" || typeToTranslate == "Desconocido" || typeToTranslate == "Inconnu")
        {
            ui->tblResults->setItem(i,1, new QTableWidgetItem(tr("Unknown")));
        }
        ui->tblResults->item(i,0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        ui->tblResults->item(i,1)->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    }
}

void servicePeripheralList::updateResultsTable(QString name, QString type, QString swVer, QString hwVer)
{
    ui->tblResults->setItem(tableRow,0,new QTableWidgetItem(name));
    ui->tblResults->setItem(tableRow,1,new QTableWidgetItem(type));
    ui->tblResults->setItem(tableRow,2,new QTableWidgetItem(swVer));
    ui->tblResults->setItem(tableRow,3,new QTableWidgetItem(hwVer));

    ui->tblResults->item(tableRow,0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->tblResults->item(tableRow,1)->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    ui->tblResults->item(tableRow,2)->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    ui->tblResults->item(tableRow,3)->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);

    ++tableRow;
}

QString servicePeripheralList::setWeigherKindText(QString weigherKind)
{
    QString weigherKindText = tr( "Unknown" );

    if(weigherKind == "2")
        weigherKindText = "USB";

    if(weigherKind == "3")
        weigherKindText = "APC";

    if(weigherKind == "4")
        weigherKindText = "AWS2";

    if(weigherKind == "5")
    	weigherKindText = "KPC";

    return weigherKindText;
}

QString servicePeripheralList::setPrinterNameText(QString printerType)
{
    QString printerNameText = tr( "Unknown" );

    if(printerType == "0")
        printerNameText = tr( "Primary Printer" );

    if(printerType == "1")
        printerNameText = tr( "Second Printer" );

    if(printerType == "2")
        printerNameText = tr( "Third Printer" );

    return printerNameText;
}

QString servicePeripheralList::setPrinterKindText(QString printerKind)
{
    QString printerKindText = tr( "Unknown" );

    if(printerKind == "1")
        printerKindText = "EPP";

    if(printerKind == "2")
        printerKindText = "ESS";

    if(printerKind == "3")
        printerKindText = "ESC";

    if(printerKind == "4")
        printerKindText = "APC";

    if(printerKind == "5")
        printerKindText = "EPP3";

    if(printerKind == "6")
        printerKindText = "EPP4";

    if(printerKind == "9")
    	printerKindText = "KPC";
    if(printerKind == "12")    
        printerKindText = "SSKP";
    return printerKindText;
}

QString servicePeripheralList::setNumericKindText(QString numericKind)
{
    QString numericKindText = tr( "Unknown" );

    if(numericKind == "1")
        numericKindText = "ESC";

    if(numericKind == "2")
        numericKindText = "APC";

    return numericKindText;
}

QString servicePeripheralList::setMarqueeKindText(QString marqueeKind)
{
    QString marqueeKindText = tr( "Unknown" );

    if(marqueeKind == "1")
        marqueeKindText = "ESC";

    if(marqueeKind == "2")
        marqueeKindText = "APC";

    return marqueeKindText;
}

QString servicePeripheralList::setGcdKindText(QString gcdKind)
{
    QString gcdKindText = tr( "Unknown" );

    if(gcdKind == "1")
        gcdKindText = "USB";

    return gcdKindText;
}

QString servicePeripheralList::setLabelApplierKindText(QString labelApplierKind)
{
    QString labelApplierKindText = tr( "Unknown" );

    // THESE TWO KINDS ARE NOT IMPLEMENTED
    //if(labelApplierKind == "1")
    //    labelApplierKindText = "ILA";

    //if(labelApplierKind == "2")
    //    labelApplierKindText = "Ultima";

    if(labelApplierKind == "3")
    {
        labelApplierKindText = "Access";
    }
    else if(labelApplierKind == "5")
    {
        labelApplierKindText = "CLAS";
    }
    else
    {
        labelApplierKindText = tr("Unknown");
    }
    return labelApplierKindText;
}

QString servicePeripheralList::setWrapperKindText(QString wrapperKind)
{
    QString wrapperKindText = tr( "Unknown" );

    if(wrapperKind == "1")
        wrapperKindText = "Ultima";

    if(wrapperKind == "2")
        wrapperKindText = "Access";

    return wrapperKindText;
}
