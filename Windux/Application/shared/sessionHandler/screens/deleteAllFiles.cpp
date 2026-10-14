/**
 * @file deleteAllFiles.cpp
 *
 * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "deleteAllFiles.h"
#include "ui_deleteAllFiles.h"

#include "../sessionmainwindow.h"

#include <Messages.h>
#include <ScaleConfiguration.h>

#include <QMessageBox>
#include <QFileInfo>

deleteAllFiles::deleteAllFiles(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::deleteAllFiles),
    internalConfig_( NULL ),
    scaleConfig_( NULL )
{
    ui->setupUi(this);
    scaleScreen();

    //----------------------------------------------------------------------
    // Set the connection for when the "Delete all files" button is clicked.
    //----------------------------------------------------------------------
    connect(ui->btnDeleteAll, SIGNAL(clicked()), this, SLOT(executeDeleteAll()));

    scaleConfig_    = &session_->scaleConfig->scaleConfig;
    internalConfig_ = &session_->scaleConfig->internalConfig;

    translate();
}

deleteAllFiles::~deleteAllFiles()
{
    delete ui;
}

void deleteAllFiles::translate()
{
    ui->retranslateUi(this);

    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool deleteAllFiles::onScreenChanging()
{
    session_->send( TransactionTypeDev, TransactionTypeWriteCurrentType, "1" );       // Production

    return _Mybase::onScreenChanging();
}

// Send the message to delete all of the data
void deleteAllFiles::executeDeleteAll()
{
    // pjr383
    //if ( scaleConfig_->SelfServeWeightRequiredToSelectItem() || scaleConfig_->selfServeCombineSearchKeypadFlashkeys() )
    if( scaleConfig_->selfServeItemEntryScreenState() < SelfServicePrintOption::SelfServOption_LookupKeypadOnly )
        internalConfig_->updateSelfServiceFlashkeys = true;           // reload flashkeys in self service mode
    
    session_->send( ConfigurationDev, CfgDeleteAllFiles);


   /////////////////////
    QString checksumFileName = "/usr/local/hobart/checksums.txt";
    QFile fileChecksum(checksumFileName);

    QFile fOut(checksumFileName);
       if (fOut.open(QFile::WriteOnly | QFile::Text))
       {
            QTextStream s(&fOut);
            s << "";
            fOut.close();

       }
       else
       {
            ;
       }
}
