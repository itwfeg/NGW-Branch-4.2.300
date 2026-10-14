/**
  * @file labelGraphics.cpp
  *
  * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  */

#include <QtGui>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QMessageBox>

#include "labelGraphics.h"
#include "ui_labelGraphics.h"
#include "ScaleConfiguration.h"

#include "../sessionmainwindow.h"

#include <filesystem.h>
#include <logmanager.h>

labelGraphics::labelGraphics(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::labelGraphics)
{
    ui->setupUi(this);
    scaleScreen();

    ignoreSignals_ = true;

    //------------------------------------------
    // Set the connections for the button clicks
    //------------------------------------------
    connect(ui->btnPrevious, SIGNAL(clicked()), this, SLOT(DisplayPreviousImage()));
    connect(ui->btnNext, SIGNAL(clicked()), this, SLOT(DisplayNextImage()));
    connect(ui->btnDelete, SIGNAL(clicked()), this, SLOT(DeleteImage()));
    connect(ui->cbGraphicName,SIGNAL(currentIndexChanged(int)), this, SLOT(ImageDropdownChanged(int)));

    //-----------------------------------------------------------
    // Get list of images and display the first image in the list
    //-----------------------------------------------------------
    if(GetListOfImages() > 0)
    {
        PopulateGraphicDropdown();

        DisplayImage(0);
    }

    ignoreSignals_ = false;

    translate();
}

labelGraphics::~labelGraphics()
{
    delete ui;
}

void labelGraphics::translate()
{
    ui->retranslateUi(this);

    update();
}

int labelGraphics::GetListOfImages()
{
    LOG_CLASS_INFO( "labelGraphics", "GetListOfImages" );

    QString nameFilter;
    nameFilter = "*.png";

    //----------------------------------------------------------
    // Sync folders when remote access
    //----------------------------------------------------------
    if ( !mainWindow_->isPrimarySessionType() )
    {
        LOG_TRACE( "sync label images...", NULL );

        if ( !filesystem::syncFolder( folderType::LabelImages ) )
            LOG_WARN( "could not sync label images!", NULL );
    }

    //----------------------------------------------------------
    // Get a list of all of the System-level label graphic files
    //----------------------------------------------------------
    QDir dir1;
    QString systemGraphics = filesystem::folder(folderType::LabelImages, true);
    dir1 =  QDir(systemGraphics);

    QStringList list1;
    list1 = dir1.entryList(QStringList(nameFilter), QDir::Files | QDir::NoSymLinks, QDir::Name);

    for (int i = 0; i < list1.size(); ++i)
    {
        QFile file(dir1.absoluteFilePath(list1[i]));
        QString fnameNoExt = QFileInfo(file).baseName();
        QString fullname = QFileInfo(file).absoluteFilePath();

        //---------------------------------------------------------------------
        // Need the file name with no extension for sorting
        // Need "S" for System graphic - indicates this image CANNOT be deleted
        // String will be in the format:
        //  <filename>|S|<fullpath>
        //---------------------------------------------------------------------
        list.append(fnameNoExt + "|S|" + fullname);

//        QMessageBox::information(this, "List of Images - DIR1", "Name=" + fname + "\nBaseName=" + fnameNoExt + "\nPath=" + fullname, QMessageBox::Ok);
    }

    //--------------------------------------------------------
    // Get a list of all of the User-level label graphic files
    //--------------------------------------------------------
    QDir dir2;
    QString userGraphics = filesystem::folder(folderType::LabelImages);
    dir2 = QDir(userGraphics);

    QStringList list2;
    list2 = dir2.entryList(QStringList(nameFilter), QDir::Files | QDir::NoSymLinks, QDir::Name);

    for (int i = 0; i < list2.size(); ++i)
    {
        QFile file(dir2.absoluteFilePath(list2[i]));
        QString fnameNoExt = QFileInfo(file).baseName();
        QString fullname = QFileInfo(file).absoluteFilePath();

        //----------------------------------------------------------------
        // Need the file name with no extension for sorting
        // Need "U" for User graphic - indicates this image CAN be deleted
        // String will be in the format:
        //  <filename>|U|<fullpath>
        //----------------------------------------------------------------
        list.append(fnameNoExt + "|U|" + fullname);

//        QMessageBox::information(this, "List of Images - DIR2", "Name=" + fname + "\nBaseName=" + fnameNoExt + "\nPath=" + fullname, QMessageBox::Ok);
    }

    //--------------------------------------------------------------------------
    // Make sure the list is sorted.
    // It will sort based on the filename, which is the first part of the string
    //--------------------------------------------------------------------------
    list.sort();

    //========================================================
    // ... DEBUGGING
//    for (int i = 0; i < list.size(); ++i)
//    {
//        // String will be in the format:
//        //  <filename>|S|<fullpath> or
//        //  <filename>|U|<fullpath>

//        int offset1 = list[i].indexOf("|");
//        int offset2 = list[i].lastIndexOf("|");

//        QString fnameNoExt = list[i].mid(0,offset1);
//        QString deleteFlag = list[i].mid((offset1) + 1, 1);
//        QString fullname = list[i].mid(offset2 + 1);

//        QMessageBox::information(this, "List of Images - ALL", "BaseName=" + fnameNoExt + "\nDeleteFlag=" + deleteFlag + "\nPath=" + fullname, QMessageBox::Ok);
//    }
    //========================================================

    count = list.size();
    currentImage = 0;

    return count;
}

void labelGraphics::PopulateGraphicDropdown()
{
    for (int i = 0; i < list.size(); ++i)
    {
        // Get the filename (label name)
        // String will be in the format:
        //  <filename>|S|<fullpath> or
        //  <filename>|U|<fullpath>

        int offset1 = list[i].indexOf("|");
        QString fnameNoExt = list[i].mid(0,offset1);

        ui->cbGraphicName->insertItem(i, fnameNoExt);
    }
}

void labelGraphics::DisplayImage(int imageOffset)
{
    if ( imageOffset < 0 )
        return;
    else if ( list.size() <= imageOffset )
        return;

    // String will be in the format:
    //  <filename>|S|<fullpath> or
    //  <filename>|U|<fullpath>

    int offset1 = list[imageOffset].indexOf("|");
    int offset2 = list[imageOffset].lastIndexOf("|");

    QString deleteFlag = list[imageOffset].mid((offset1) + 1, 1);
    QString fullname = list[imageOffset].mid(offset2 + 1);

    QPixmap pixmap(fullname);
    QSizeF pixsize = pixmap.size();
    int width = pixsize.width();
    int height = pixsize.height();

    QGraphicsScene *scene = new QGraphicsScene( this );
    scene->addPixmap(pixmap);

    QGraphicsScene *oldScene( ui->graphicsView->scene() );

    ui->graphicsView->setScene(scene);
    ui->graphicsView->show();

	// FT-1168: fix memory leak
    if ( oldScene )
        delete oldScene;

    // Update dropdown containing image name
    ui->cbGraphicName->setCurrentIndex(imageOffset);

    // Update display of Label Graphic Size
    ui->lblGraphicWidth->setText(QString::number(width) + " px W");
    ui->lblGraphicHeight->setText(QString::number(height) + " px H");

    // Enable/Disable Delete button based on the delete flag (System or User)
    ui->btnDelete->setEnabled((deleteFlag == "S") ? false : true);
}

////////////
//
// S L O T S
//
////////////

void labelGraphics::DisplayPreviousImage()
{
    currentImage = (currentImage == 0 ? (count - 1) : (currentImage - 1));

    DisplayImage(currentImage);
}

void labelGraphics::DisplayNextImage()
{
    currentImage = (currentImage == (count - 1) ? 0 : (currentImage + 1));

    DisplayImage(currentImage);
}

void labelGraphics::DeleteImage()
{
    if ( currentImage < 0 )
        return;
    else if ( list.size() <= currentImage )
        return;

    //------------------------------------------------------------------
    // Need to send message to database to delete the image.
    // This will also delete the corresponding PNG file from the system.
    //------------------------------------------------------------------

    // Get the filename (label name)
    // String will be in the format:
    //  <filename>|U|<fullpath>

    int offset = list[currentImage].indexOf("|");
    QString filename = list[currentImage].mid(0,offset);

    session_->send(GraphicDev, GrDtDelete, filename);

    // Remove the entry from the list
    list.removeAt(currentImage);
    --count;

    // Delete the image name from the dropdown
    ui->cbGraphicName->removeItem(currentImage);

    // Display the next image in the list
    --currentImage;
    DisplayNextImage();

}

void labelGraphics::ImageDropdownChanged(int index)
{
    if( ignoreSignals_ )
        return;

    currentImage = index;

    DisplayImage(currentImage);
}
