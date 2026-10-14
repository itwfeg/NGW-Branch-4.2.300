/* @file superSpecials.cpp
 * superSpecials 
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd

   @Brief Sets the special's graphics
 */

#include <logmanager.h>

#include <QtGui>
#include <QtCore>

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include "superSpecials.h"
#include "ui_superSpecials.h"

static const int UPGRADE = 0; // set to 1 when backend fixed to accept quoted strings. Password bug in current backend truncates if comma is entered by User

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

superSpecials::superSpecials(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::superSpecials)
{
    ui->setupUi(this);
    scaleScreen();

    loading = true;
    isNewSLU = false;
    lastRow = -1;
    currentRow = -1;
    creatingNewSpecial = false; // FT-103
    fnt.setPointSize(15);

    setTableHeadings( 1 );

    session_->send(SpecialsDev,ReadAllGraphics); //124,20
    session_->send(SpecialsDev,SpecialsDtRead); //124,1

    ui->butDelete->setEnabled(false);

   // connect(ui->leHostname, SIGNAL(textChanged(const QString)), this, SLOT(preventSpacesInHostnames(const QString&)));
    connect(ui->butDelete, SIGNAL(clicked()), this, SLOT(deleteNow()));
    connect(ui->tvSpecials, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(rowSelected(QTableWidgetItem*)));
    /* These were asked to be remvoed due to: FT-668 - Need to remove the referencesand controls to graphics on the Specials page in Supervisor
       However, since things tend to come back, I'm going to just comment these out for now in case we need them.
       Also keep in mind you need to pull up the UI and add the 2 combo boxes and labels back in, plus make the # of cols=4
       Of course  you can just go get the .ui previous to this one
    */
    //connect(ui->cbPrimary, SIGNAL(currentIndexChanged(const QString&)),this,SLOT(primaryComboChanged(const QString&)));
    //connect(ui->cbSecondary, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(secondaryComboChanged(const QString&)));
    //connect(ui->butCancel, SIGNAL(clicked()), this, SLOT(CancelButClicked()));
    //connect(ui->butOK, SIGNAL(clicked()), this, SLOT(OKButClicked()));
    connect(ui->leDescription,SIGNAL(editingFinished()), this, SLOT(updateDescription()));
    connect(ui->leSpecialNumber, SIGNAL(editingFinished()), this, SLOT(SLU()));


    ui->leSpecialNumber->setValidator(new QIntValidator(1,999999, this));

    //ui->gbNew->hide();
    translate();
}

superSpecials::~superSpecials()
{
    delete ui;
}

void superSpecials::preventSpacesInSpecialNumber(const QString&)
{
    ui->leSpecialNumber->setText(ui->leSpecialNumber->text().replace(" ",""));
}

void superSpecials::deleteNow()
{
    // SpecialsDev, DtDelete  124,4
    // Example: 124,4,999991
    session_->send(SpecialsDev, SpecialsDtDelete, ui->leSpecialNumber->text());
    ui->tvSpecials->removeRow(currentRow);

    if(currentRow != 0)
    {
        ui->tvSpecials->selectRow(currentRow-1);
        currentRow--;
    }
    else
    {
        ui->tvSpecials->selectRow(0);
        currentRow = 0;
    }


    // FT-518 - only update the edit fields from the table if there are rows left to pull data from
    if( ui->tvSpecials->rowCount() )
    {
        ui->leSpecialNumber->setText(ui->tvSpecials->item(currentRow,0)->text());
        ui->leDescription->setText(ui->tvSpecials->item(currentRow,1)->text());

        //FT-668 - Need to remove the referencesand controls to graphics on the Specials page
        //ui->cbPrimary->setCurrentIndex(ui->cbPrimary->findData(ui->tvSpecials->item(currentRow,2)->text()));
        //ui->cbSecondary->setCurrentIndex(ui->cbSecondary->findData(ui->tvSpecials->item(currentRow,3)->text()));

        // FT-518 - Backend needs to now what special is "current" or it will
        // not allow it to be saved/updated later
        session_->send( SpecialsDev, SpecialsDtReadSingle, ui->leSpecialNumber->text() );
    }
}

void superSpecials::rowSelected(QTableWidgetItem* item)
{
    LOG_CLASS_INFO( "superSpecials", "rowSelected" );

    // The word None is bugged in the backend as of 2/18/2013 It is translating it. The frontend is also sending the word Ninguno instead of None.

    if(!loading)
    {
        currentRow = item->row();
        if(lastRow > -1)
        {
        // send 124,6 and data of last row

            QString msg;

            // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
            //msg = specialNumberToSave + "," + descriptionToSave + "," + primaryToSave + "," + secondaryToSave + ",30";
            msg = specialNumberToSave + "," + descriptionToSave + "," + "None" + "," + "None" + ",30";

            /*
            msg = ui->tvSpecials->item(lastRow,0)->text() + "," + ui->tvSpecials->item(lastRow,1)->text() + ",";

            if(ui->tvSpecials->item(lastRow,2)->text() == tr("None"))
                msg += "None,";
            else msg += ui->tvSpecials->item(lastRow,2)->text() + ",";

            if(ui->tvSpecials->item(lastRow,3)->text() == tr("None"))
                msg += "None,30";
            else msg +=  ui->tvSpecials->item(lastRow,3)->text() +",30";
           */
            session_->send(SpecialsDev, SpecialsDtSave, msg);
        }

        // FT-103 - Backend needs to now what special is "current" or it will
        // not allow it to be saved/updated later
        QString clickedValue = ui->tvSpecials->item(item->row(),0)->text();
        session_->send( SpecialsDev, SpecialsDtReadSingle, clickedValue );

        ui->tvSpecials->selectRow(item->row());
        ui->leSpecialNumber->setText(ui->tvSpecials->item(item->row(),0)->text());
        ui->leDescription->setText(ui->tvSpecials->item(item->row(),1)->text());

        //FT-668 - Need to remove the references and controls to graphics on the Specials page
        //ui->cbPrimary->setCurrentIndex(ui->cbPrimary->findData(ui->tvSpecials->item(item->row(),2)->text()));

        LOG_DEBUG( "here is: %s", ui->tvSpecials->item(item->row(),1)->text().toLatin1().constData() );
        //ui->cbSecondary->setCurrentIndex(ui->cbSecondary->findData(ui->tvSpecials->item(item->row(),3)->text()));

        specialNumberToSave = ui->leSpecialNumber->text();
        descriptionToSave = ui->leDescription->text();
    }

    lastRow = item->row();
    ui->tvSpecials->resizeRowsToContents();

    if(ui->leSpecialNumber->text() == "")
    {
        ui->butDelete->setEnabled(false);
    }
    else
    {
        ui->butDelete->setEnabled(true);
    }

/*    C.Send: 000092124,6,999991,Price Reductions SSZSSZSSZ SEEEWSEEEERR ZSXDDRRSSD,sale_fr.gif,new.gif,30

    below is not needed
    C.Send: 000015124,0,256
    C.Update: 124,2,256,"","new.gif","new_fr.gif",0
  */
}

// FT-668 - Need to remove the referencesand controls to graphics on the Specials page
/*
void superSpecials::primaryComboChanged(const QString &text)
{
    if(!isNewSLU)
    {
        if(!loading)
        {
            if(currentRow != -1)    // FT-103
                ui->tvSpecials->item(currentRow,2)->setText(text);
        }

        ui->tvSpecials->resizeRowsToContents();


        if(ui->cbPrimary->currentIndex() != 0)
            primaryToSave = text;
        else
            primaryToSave = "None";
    }
}
*/
// FT-668 - Need to remove the referencesand controls to graphics on the Specials page
/*
void superSpecials::secondaryComboChanged(const QString &text)
{
    if(!isNewSLU)
    {
        if(!loading)
        {
            if(currentRow != -1)    // FT-103
                ui->tvSpecials->item(currentRow,3)->setText(text);
        }
        ui->tvSpecials->resizeRowsToContents();

        if(ui->cbSecondary->currentIndex() != 0)
            secondaryToSave = text;
        else
            secondaryToSave = "None";
    }
}
*/
void superSpecials::updateDescription()
{
    if(!isNewSLU)
    {
        if(!loading)
        {
            // FT-103 - Skip if not on a row
            if(currentRow != -1)
                ui->tvSpecials->item(currentRow, 1)->setText(ui->leDescription->text());
        }
        ui->tvSpecials->resizeRowsToContents();
        descriptionToSave = ui->leDescription->text();
    }
}

void superSpecials::OKButClicked()
{
 //SpecialsDev, DtCreateNew
    //ui->gbNew->hide();
    ui->tvSpecials->show();
    session_->send(SpecialsDev, SpecialsDtCreateNew, ui->leSpecialNumber->text());
}

void superSpecials::CancelButClicked()
{
  //  ui->gbNew->hide();
    ui->tvSpecials->show();
}

void superSpecials::popupSLU()
{
  //  ui->gbNew->show();
    ui->tvSpecials->hide();
}

void superSpecials::requestSLU()
{
    session_->send(SpecialsDev, SpecialsDtReadSingle, ui->leSpecialNumber->text());
}

void superSpecials::SLU()
{
   // Example Send: 124,0,596
    if(ui->leSpecialNumber->text() != "")
    {
        //1) SAVE THE OLD SPECIAL

        //2) SLU
        QString newSLU = ui->leSpecialNumber->text();
        bool isNew = true;

        for(int i = 0; i < ui->tvSpecials->rowCount() && isNew; i++)
        {
            if(newSLU == ui->tvSpecials->item(i,0)->text())
            {
                isNew = false;
                currentRow = i;
            }
        }

        if(isNew)
            requestSLU();
        else
        {
            // load data from row number
            ui->tvSpecials->selectRow(currentRow);
            ui->leDescription->setText(ui->tvSpecials->item(currentRow,1)->text());

            // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
            //primaryName = ui->tvSpecials->item(currentRow,2)->text();
            //ui->cbPrimary->setCurrentIndex(ui->cbPrimary->findText(primaryName));
            //secondaryName = ui->tvSpecials->item(currentRow,3)->text();
            //ui->cbSecondary->setCurrentIndex(ui->cbSecondary->findText(secondaryName));
        }
    }
    else
        ui->butDelete->setEnabled(false);
    // send below line when Backend fixed to teel frontend that SLU doesn't exist
   // session_->send(SpecialsDev, SpecialsDtReadSingle, ui->leSpecialNumber->text());
}

void superSpecials::saveSpecialsInfo()
{
    // Save that last row in case it was edited before destructor
    if(lastRow > -1)
    {
        // send 124,6 and data of last row

        QString msg;

        msg = ui->leSpecialNumber->text() + "," + ui->leDescription->text() + ",";

        // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
        //if(ui->cbPrimary->currentIndex() == 0)
        //{
            msg += "None,";
        //}
        //else
        //{
        //    msg += ui->cbPrimary->currentText() + ",";
        //}
        //
        //if(ui->cbSecondary->currentIndex() == 0)
        //{
            msg += "None,30";
        //}
        //else
        //{
        //    msg += ui->cbSecondary->currentText() + ",30";
        //}

        session_->send(SpecialsDev, SpecialsDtSave, msg);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superSpecials::translate()
{
    ui->retranslateUi(this);

    QStringList headersStr;
    headersStr << tr("Special Number") << tr("Description");
    ui->tvSpecials->setHorizontalHeaderLabels(headersStr);

    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superSpecials::clearTheFields()
{
    ui->leSpecialNumber->setText("");
    ui->leDescription->setText("");
    // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
    //ui->cbPrimary->setCurrentIndex(0);
    //ui->cbSecondary->setCurrentIndex(0);
    ui->butDelete->setEnabled(false);
    ui->leSpecialNumber->setFocus();
}


void superSpecials::setTableHeadings( int numberOfRows )
{
    // Set up table dimensions
//    ui->tvSpecials->setRowCount( numberOfIds );
    ui->tvSpecials->setRowCount( numberOfRows );

    ui->tvSpecials->setColumnWidth( 0, scaleWidthForScreen( 136 ) ); // alerts checkbox
    // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
    //ui->tvSpecials->setColumnWidth(0,136); // alerts checkbox
    //ui->tvSpecials->setColumnWidth(1,316); // alerts text
    //ui->tvSpecials->setColumnWidth(2,200); // notify checkbox
    //ui->tvSpecials->setColumnWidth(3,210); // notify text
    // FT-668 - Now column width
    ui->tvSpecials->setColumnWidth( 1, scaleWidthForScreen( 702 ) );         // this is a total of all commented out columns - 24

    // How about some headers
    QFont fntLower;
    fntLower.setPointSize(10);
    fntLower.setBold(true);

    QTableWidgetItem *specialHeaderItem = new QTableWidgetItem;
    QTableWidgetItem *descriptionHeaderItem = new QTableWidgetItem;
    // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
    //QTableWidgetItem *primaryHeaderItem = new QTableWidgetItem;
    //QTableWidgetItem *secondaryHeaderItem = new QTableWidgetItem;

    specialHeaderItem->setFont(fntLower);
    descriptionHeaderItem->setFont(fntLower);
    // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
    //primaryHeaderItem->setFont(fntLower);
    //secondaryHeaderItem->setFont(fntLower);

    specialHeaderItem->setText(tr("Special Number"));
    descriptionHeaderItem->setText(tr("Description"));
    // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
    //primaryHeaderItem->setText(tr("Primary Graphic"));
    //secondaryHeaderItem->setText(tr("Secondary Graphic"));

    specialHeaderItem->setForeground(Qt::white);
    descriptionHeaderItem->setForeground(Qt::white);
    // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
    //primaryHeaderItem->setForeground(Qt::white);
    //secondaryHeaderItem->setForeground(Qt::white);

    specialHeaderItem->setBackgroundColor(Qt::black);
    descriptionHeaderItem->setBackgroundColor(Qt::black);
    // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
    //primaryHeaderItem->setBackgroundColor(Qt::black);
    //secondaryHeaderItem->setBackgroundColor(Qt::black);

    ui->tvSpecials->setHorizontalHeaderItem(0, specialHeaderItem);
    ui->tvSpecials->setHorizontalHeaderItem(1, descriptionHeaderItem);
    // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
    //ui->tvSpecials->setHorizontalHeaderItem(2, primaryHeaderItem);
    //ui->tvSpecials->setHorizontalHeaderItem(3, secondaryHeaderItem);

    ui->tvSpecials->setWordWrap(true);
}

void superSpecials::removeSpecialFromTV(QString specialText)
{
    Q_UNUSED( specialText );

    ui->tvSpecials->removeRow(currentRow);
    ui->butDelete->setEnabled(false);

    if(ui->tvSpecials->rowCount() == 0)
    {
        currentRow = 0;
    }
}

bool superSpecials::onScreenChanging()
{
    saveSpecialsInfo();

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superSpecials::onSessionMessage( const SessionMessage& msg )
{
    if(SpecialsDev == msg[0].toInt() && SpecialsDtWriteAll == msg[1].toInt()) //124,3
    {
        DomSpecialsList m( msg );

        int numberOfIds = m.numOfIds().toInt();

        //Example: Update: 124,3,2,"999990,999991",2,"New Products,Price Reductions SSZSSZSSZ SEEEWSEEEERR ZSXDDRRSSD",2,"new.gif,sale.gif",2,"new.gif,sale.gif"
        //                  0  1 2        3        4                                5                                  6         7          8         9
        // Load the table widget

        // Prep some lists with incoming values
        QVector<QString> specialNumbersListTV = TokenParser::splitStringTokens( m.idList() );
        QVector<QString> descriptionListTV = TokenParser::splitStringTokens( m.typeList() );
        QVector<QString> primaryGraphicsListTV = TokenParser::splitStringTokens( m.firstGraphicList() );
        QVector<QString> secondaryGraphicsListTV = TokenParser::splitStringTokens( m.secondGraphicList() );

        // Set up table dimensions
        setTableHeadings( numberOfIds );

        for(int i = 0; i < numberOfIds; i++)
        {
           //ui->tvSpecials->setRowHeight(i, 50);

           QTableWidgetItem *specialNumberItem     = new QTableWidgetItem;
           QTableWidgetItem *descriptionItem       = new QTableWidgetItem;
           // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
           //QTableWidgetItem *primaryGraphicItem    = new QTableWidgetItem;
           //QTableWidgetItem *secondaryGraphicItem  = new QTableWidgetItem;

           specialNumberItem->setText(specialNumbersListTV.at(i));
           descriptionItem->setText(descriptionListTV.at(i));

           // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
           //if(primaryGraphicsListTV.at(i) == "" || primaryGraphicsListTV.at(i) == "None.gif" || primaryGraphicsListTV.at(i) == "None.jpg")
           //{
           //    primaryGraphicItem->setText(tr("None"));
           //}
           //else
           //{
           //    primaryGraphicItem->setText(primaryGraphicsListTV.at(i));
           //}
           //
           //if(secondaryGraphicsListTV.at(i) == "" || secondaryGraphicsListTV.at(i) == "None.gif" || secondaryGraphicsListTV.at(i) == "None.jpg")
           //{
           //    secondaryGraphicItem->setText(tr("None"));
           //}
           //else
           //{
           //    secondaryGraphicItem->setText(secondaryGraphicsListTV.at(i));
           //}

           specialNumberItem->setFont(fnt);
           descriptionItem->setFont(fnt);
           // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
           //primaryGraphicItem->setFont(fnt);
           //secondaryGraphicItem->setFont(fnt);

           ui->tvSpecials->setItem(i,0,specialNumberItem);
           ui->tvSpecials->setItem(i,1,descriptionItem);
           // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
           //ui->tvSpecials->setItem(i,2,primaryGraphicItem);
           //ui->tvSpecials->setItem(i,3,secondaryGraphicItem);

           ui->tvSpecials->item(i,0)->setTextAlignment(Qt::AlignVCenter);
           ui->tvSpecials->item(i,1)->setTextAlignment(Qt::AlignVCenter);
           // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
           //ui->tvSpecials->item(i,2)->setTextAlignment(Qt::AlignVCenter);
           //ui->tvSpecials->item(i,3)->setTextAlignment(Qt::AlignVCenter);

         //  ui->tvSpecials->resizeColumnsToContents();
           ui->tvSpecials->resizeRowsToContents();
        }
        loading = false;
    }
    else if(SpecialsDev == msg[0].toInt() && ReadAllGraphics == msg[1].toInt()) //124,20
    {
        DomSpecialsGraphics m( msg );

        // Example: Update: 124,20,11,"new.gif,newES.gif,new_en.gif,new_es.gif,new_fr.gif,sale.gif,saleES.gif,sale_en.gif,sale_es.gif,sale_fr.gif,template.gif"
        // Load the combo boxes with these file names of graphics

        //load "None" for first item

        // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
        //QVector<QString> graphicsFileNames = TokenParser::splitStringTokens( m.graphicsList() );

        // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
        //ui->cbPrimary->addItem(tr("None"), "None");
        //ui->cbSecondary->addItem(tr("None"), "None");

        // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
        //for (int i = 0; i < graphicsFileNames.count(); i++)
        //{
        //    ui->cbPrimary->addItem(graphicsFileNames.at(i), graphicsFileNames.at(i));
        //    ui->cbSecondary->addItem(graphicsFileNames.at(i), graphicsFileNames.at(i));
        //}
    }
    else if (Error == msg[0].toInt() && ErrSpecialsDevice == msg[1].toInt())
    {
        // Use this message handler for creating or deleting specials

        // Received 103,124 - send read category, it doesn't exist so create new or delete one
        returnedErrorsSpecialsDev m( msg );

        int var = m.errorData2();

        if (var)
        {
            var = ( 124001 == m.errorCode() ) ? SpecialsDtCreateNew : SpecialsDtDelete;

            if(var == SpecialsDtCreateNew)
            {
                creatingNewSpecial = true;
            }
            else
            {
                creatingNewSpecial = false;
            }

            // Create or Delete category
            session_->send(SpecialsDev, var, ui->leSpecialNumber->text());

            if(!creatingNewSpecial)
            {
                // We are deleting
                removeSpecialFromTV(ui->leSpecialNumber->text());
                specialNumber = "";
                clearTheFields();
            }
        }
        else
        {
            clearTheFields();
        }
    }

    else if(SpecialsDev == msg[0].toInt() && SpecialsDtWrite == msg[1].toInt()) //Update: 124,2,596,"","","",0 a new SLU response and its data
    {
        // FT-103 - Only process this message when creating a new special.
        // Otherwise this logic causes row to be duplicated in the table.
        if(!creatingNewSpecial)
            return;

        DomSingleSpecial m( msg );

       // ui->leSpecialNumber->setText(msg[2]);
        isNewSLU = true;

        ui->leDescription->setText( m.specialDesc() );

        // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
        //if( m.graphicName1() == "" || m.graphicName1() == "None.gif" || m.graphicName1() == "None.jpg")
        //{
        //    ui->cbPrimary->setCurrentIndex(0);
        //}
        //else
        //{
        //    ui->cbPrimary->setCurrentIndex(ui->cbPrimary->findData( m.graphicName1() ));
        //}

        // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
        //if( m.graphicName2() == "" || m.graphicName2() == "None.gif" || m.graphicName2() == "None.jpg")
        //{
        //    ui->cbSecondary->setCurrentIndex(0);
        //}
        //else
        //{
        //    ui->cbSecondary->setCurrentIndex(ui->cbSecondary->findData( m.graphicName2() ));
        //}

        // Larry said to ignore length for now which is how long the special is good for

        //Add it to the tablewidget
        QTableWidgetItem *newSpecialNumberItem = new QTableWidgetItem;
        QTableWidgetItem *newDescriptionItem = new QTableWidgetItem;
        // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
        //QTableWidgetItem *newPrimaryGraphicItem = new QTableWidgetItem;
        //QTableWidgetItem *newSecondaryGraphicItem = new QTableWidgetItem;

        newSpecialNumberItem->setFont(fnt);
        newDescriptionItem->setFont(fnt);
        // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
        //newPrimaryGraphicItem->setFont(fnt);
        //newSecondaryGraphicItem->setFont(fnt);

        newDescriptionItem->setText( m.specialDesc() );
        newSpecialNumberItem->setText( m.specialID() );

        // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
        //if( m.graphicName1() == "" || m.graphicName1() == "None.gif" || m.graphicName1() == "None.jpg")
        //{
        //    newPrimaryGraphicItem->setText(tr("None"));
        //}
        //else
        //{
        //    newPrimaryGraphicItem->setText( m.graphicName1() );
        //}

        // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
        //if( m.graphicName2() == "" || m.graphicName2() == "None.gif" || m.graphicName2() == "None.jpg")
        //{
        //    newSecondaryGraphicItem->setText(tr("None"));
        //}
        //else
        //{
        //    newSecondaryGraphicItem->setText( m.graphicName2() );
        //}

        ui->tvSpecials->insertRow(0);
        currentRow = 0;
        lastRow = 0;    // FT-103
        //   ui->tvSpecials->setRowCount(ui->tvSpecials->rowCount()++);
        ui->tvSpecials->setItem(0,0,newSpecialNumberItem);
        ui->tvSpecials->setItem(0,1,newDescriptionItem);
        // FT-668 - Need to remove the referencesand controls to graphics on the Specials page
        //ui->tvSpecials->setItem(0,2,newPrimaryGraphicItem);
        //ui->tvSpecials->setItem(0,3,newSecondaryGraphicItem);

    //    ui->tvSpecials->item(0,0)->setTextAlignment(Qt::AlignVCenter);
    //    ui->tvSpecials->item(0,1)->setTextAlignment(Qt::AlignVCenter);
    //    ui->tvSpecials->item(0,2)->setTextAlignment(Qt::AlignVCenter);
    //    ui->tvSpecials->item(0,3)->setTextAlignment(Qt::AlignVCenter);
/*
        ui->tvSpecials->item(ui->tvSpecials->rowCount(),0)->setTextAlignment(Qt::AlignVCenter);
        ui->tvSpecials->item(ui->tvSpecials->rowCount(),1)->setTextAlignment(Qt::AlignVCenter);
        ui->tvSpecials->item(ui->tvSpecials->rowCount(),2)->setTextAlignment(Qt::AlignVCenter);
        ui->tvSpecials->item(ui->tvSpecials->rowCount(),3)->setTextAlignment(Qt::AlignVCenter);
*/
          isNewSLU = false;

          creatingNewSpecial = false;    // FT-103
    }
}
