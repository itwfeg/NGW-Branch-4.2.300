/* @file superCategories.cpp
 * superCategories
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd

   @Brief Sets the confguration for Categories
 */

#include <filesystem.h>
#include <logmanager.h>

#include "superCategories.h"
#include "ui_superCategories.h"

#include "../sessionmainwindow.h"

#include "../dialogs/colordialog.h"

#include "../modes/supervisormode.h"

#include <numericlineedit.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QtGui>
#include <QtCore>
#include <QDir>
#include <QFileInfo>
#include <QIntValidator>

/*
//////////////////////
// NOTE Category images are found in OS rootfolder/images/categories

// So for development on windows copy your existing category images from

// C:\Program Files\Hobart\Access Scale\webPages\images\categories to the directory

// C:\program files\hobart\images\categories\
*/

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

namespace hobart {
class numericLineEdit;
}

superCategories::superCategories(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::superCategories)
{
    ui->setupUi(this);
    scaleScreen();

    loading = true;
    categoryHasBeenChosen = false;
    lookingUpCLU = false;
    categoryNumber = "";
    creatingNewCategory = false;
    leavingPage = false;
    primaryToSave = "";
    secondaryToSave = "";
    loadColorBoxes();

    scaleConfigData    = &session_->scaleConfig->scaleConfig;
    internalConfigData = &session_->scaleConfig->internalConfig;

    connect(ui->leCategoryNumber, SIGNAL(editingFinished()), this, SLOT(CLU()));
    connect(ui->cbBackgroundColor, SIGNAL(currentIndexChanged(QString)), this, SLOT(backgroundColorPicker(QString)));
    connect(ui->cbTextColor, SIGNAL(currentIndexChanged(QString)), this, SLOT(textColorPicker(QString)));

    ///////////////////
    connect(ui->lePrimary, SIGNAL(textChanged(QString)), this, SLOT(updatePrimaryToSave(QString)));
    connect(ui->leSecondary, SIGNAL(textChanged(QString)), this, SLOT(updateSecondaryToSave(QString)));
    connect(ui->cbFlashkeyGraphic, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateFlashkeyToSave(QString)));
    connect(ui->cbReportingCategory, SIGNAL(currentIndexChanged(int)), this, SLOT(updateReportingCategory(int)));

    connect(ui->butDelete, SIGNAL(clicked()), this, SLOT(deleteCategoryButClicked()));
    ///////////////////
    previousCategoryNumberText = "";

    ui->leCategoryNumber->setValidator(categoryValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leCategoryNumber->setDecimalPlaces(0);
    ui->leCategoryNumber->setInputWrapping(true);
    categoryValidator_->setTop(makeValidatorTop(6));

    ui->leCategoryNumber->installEventFilter(this);


    // show initializing dialog
    session_->send(CategoryDev, CategoryReadAllMainCategories); // 121,22

    loadPlaylists();

    translate();
}

superCategories::~superCategories()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool superCategories::onScreenChanging()
{

    if( categoryNumber!= "" )
    {
        // then we must have an active category so we must save it off
        leavingPage = true;
        updateDataToSave();
        saveCLU();
    }


    return _Mybase::onScreenChanging();
}


bool superCategories::eventFilter( QObject *obj, QEvent *event )
{
    if(obj == ui->leCategoryNumber)
    {
        if(QEvent::FocusIn == event->type())
        {
          //  qDebug() << "event click in";
            ui->leCategoryNumber->setText("");
         //   update();
        }
    }
    return _Mybase::eventFilter( obj, event );
}


void superCategories::deleteCategoryButClicked()
{
    // show are you sure
    //ui->gbAreYouSureDelete->show();
    QString msg = QString::number(121002) + ",1";

    // Ask User if it is OK to delete a category
    session_->send(Error, ErrCategoryDevice, msg);
    // okay button will delete it and cancel will close the are you sure question box
}

void superCategories::areYouSureOKButClicked()
{
    //delete it
    // Send: 121,4,category#
    QString msg = ui->leCategoryNumber->text();

    session_->send(CategoryDev, CategoryDtDeleteSingle, msg);
}

int superCategories::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

void superCategories::updateDataToSave()
{
    reportingCategoryToSave = ui->cbReportingCategory->itemData(ui->cbReportingCategory->currentIndex()).toString();
    categoryNumberToSave = ui->leCategoryNumber->text();
    primaryToSave = ui->lePrimary->text();
    secondaryToSave = ui->leSecondary->text();
    backgroundColorToSave = ui->cbBackgroundColor->itemData(ui->cbBackgroundColor->currentIndex()).toString();
    textColorToSave = ui->cbTextColor->itemData(ui->cbTextColor->currentIndex()).toString();
    playlistFileToSave = ui->cboPlaylist->itemData( ui->cboPlaylist->currentIndex() ).toString();

    flashkeyGraphicToSave = ui->cbFlashkeyGraphic->currentText();
}

void superCategories::updateReportingCategory(int)
{
    static const QString masterCategory( "999999" );

    if ( 0 < ui->leCategoryNumber->toInt() )
    {
        const QString itemData( ui->cbReportingCategory->itemData( ui->cbReportingCategory->currentIndex() ).toString() );

        // user trying to set the category to itself, do not allow.
        if ( itemData == ui->leCategoryNumber->text() )
        {
            // set category to previous category
            ui->cbReportingCategory->blockSignals( true );
            ui->cbReportingCategory->setCurrentIndex( ui->cbReportingCategory->findData( previousReportingCategory ) );
            ui->cbReportingCategory->blockSignals( false );
        }
        else
        {
            int index( ui->cbReportingCategory->findData( categoryNumber ) );

            if (( masterCategory == itemData ) && ( masterCategory != previousReportingCategory ))
            {
                // set flashkey graphic
                if ( !loading )
                {
                    ui->cbFlashkeyGraphic->setCurrentIndex( 0 );
                    ui->cbFlashkeyGraphic->setEnabled( false );
                }

                // this is no longer a sub-category... add to list
                if ( index < 0 )
                    ui->cbReportingCategory->addItem( ui->lePrimary->text() + "  (" + categoryNumber + ")", categoryNumber );
            }
            else if (( masterCategory != itemData ) && ( masterCategory == previousReportingCategory ))
            {
                ui->cbFlashkeyGraphic->setEnabled(true);

                // this now a sub-category... remove from list
                if ( 0 < index )
                    ui->cbReportingCategory->removeItem( index );
            }

            // valid setting... keep track of setting...
            previousReportingCategory = itemData;
        }
    }

    loading = false;
}

void superCategories::updatePrimaryToSave(QString)
{
    // Also have to update the Primary name in the combo box.

    primaryToSave = ui->lePrimary->text();

    // FT-148: at this point we have created the new category.  Now we want to either edit and
    // existing primary category number name, or add a new one.  The line above saved the new
    // primary name off.  Now we need to search the category combo box for the category number
    // and update/add the new primary name to it.



    QString myCat = "";
    int lastIndex = ui->cbReportingCategory->count();

    for(int j = 0; j < lastIndex; j++)
    {
        myCat = ui->cbReportingCategory->itemData(j).toString();

        if(myCat == categoryNumber)
        {
            ui->cbReportingCategory->setItemText(j, primaryToSave + "  (" + myCat + ")");
            break;                                                  // we found it, updated it, now we're outta here
        }
    }
}

void superCategories::updateFlashkeyToSave(QString flashkeyGraphicChoice)
{
    //load the pic in the User's Image sample area
    // update size to particular image size and redraw

    const QString fullFilename( filesystem::absoluteFilePath( flashkeyGraphicChoice, folderType::CategoriesImages ) );
    const QPixmap pixmap( fullFilename );

    QGraphicsScene *scene( new QGraphicsScene( this ) );
    scene->addPixmap( pixmap );

    if(pixmap.height()>201)
    {
        // increase width for scroll bar
        if(pixmap.width() > 571)
        {
            pixmapWidth = 571;
            pixmapHeight = 201-40;
        }
        else
        {
            pixmapWidth = pixmap.width()+40;
            pixmapHeight = 201;
        }
        pixmapY = 369;
    }
    else
    {
        pixmapWidth = pixmap.width();
        pixmapHeight = pixmap.height();
        pixmapY = 369 + ((201-pixmap.height())/2);
    }

    QGraphicsScene *oldScene( ui->sampleImage->scene() );

    ui->sampleImage->setGeometry(661-(pixmapWidth/2), pixmapY, pixmapWidth, pixmapHeight);
    ui->sampleImage->setScene(scene);
    ui->sampleImage->show();

    flashkeyGraphicToSave = ui->cbFlashkeyGraphic->currentText();

	// FT-1168: fix memory leak
    if ( oldScene )
        delete oldScene;
}

void superCategories::updateSecondaryToSave(QString)
{
    secondaryToSave = ui->leSecondary->text();
}

void superCategories::backgroundColorPicker(QString choice)
{
    QColor color;

    QGraphicsScene *scene = new QGraphicsScene( this );
    scene->setSceneRect(ui->sampleImage->geometry().x(),ui->sampleImage->geometry().y(),ui->sampleImage->geometry().width(), ui->sampleImage->geometry().height());

    ui->sampleImage->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    ui->sampleImage->setCacheMode(QGraphicsView::CacheNone);

    if(choice == tr("Custom..."))
    {
        // open color picker
        ColorDialog colorDialog( mainWindow_ );
        colorDialog.setCurrentColor( Qt::white );
        colorDialog.setWindowTitle( tr( "Background Color" ) );

        if ( QDialog::Accepted != colorDialog.exec() )
        {
            ui->cbBackgroundColor->setCurrentIndex(0);
            return;
        }

        color = colorDialog.selectedColor();

        // set the combobox to the hex code for the color
        // add color to bottom of combo with a # symbol in front of hex triplet code, e.g., #994455
        QString colorText = color.name().toUpper();
        QString colorData = color.name().toUpper().replace("#","");

        if(ui->cbBackgroundColor->count() == 18)
        {
            ui->cbBackgroundColor->addItem(colorText,colorData);
        }
        else
        {
            ui->cbBackgroundColor->setItemText(18,colorText);
            ui->cbBackgroundColor->setItemData(18,colorData);
        }
        ui->cbBackgroundColor->setCurrentIndex(18);
        //set color to background color
    }
    else
    {
        if(ui->cbBackgroundColor->currentIndex() == 0 )
        {
            // Must be Hobart Blue
            color.setNamedColor("#000099");
        }
        else
        {
            // get data text from combobox and add # to it
            color.setNamedColor("#" + QString(ui->cbBackgroundColor->itemData(ui->cbBackgroundColor->currentIndex()).toString()));
        }
    }

    QGraphicsScene *oldScene( ui->sampleBackground->scene() );

    ui->sampleBackground->setBackgroundBrush(QBrush(color, Qt::SolidPattern));
    ui->sampleBackground->setScene(scene);
    backgroundColorToSave = ui->cbBackgroundColor->itemData(ui->cbBackgroundColor->currentIndex()).toString();
    ui->sampleBackground->update();
    ui->cbBackgroundColor->setFocus();

	// FT-1168: fix memory leak
    if ( oldScene )
        delete oldScene;
}

void superCategories::textColorPicker(QString choice)
{
    QPalette plt;
    QColor color;
    if(choice == tr("Custom..."))
    {
        // open color picker
        ColorDialog colorDialog( mainWindow_ );
        colorDialog.setCurrentColor( Qt::white );
        colorDialog.setWindowTitle( tr( "Background Color" ) );

        if( QDialog::Accepted != colorDialog.exec() )
        {
            ui->cbTextColor->setCurrentIndex(0);
            return;
        }

        color = colorDialog.selectedColor();

        // set the combobox to the hex code for the color
        // add color to bottom of combo with a # symbol in front of hex triplet code, e.g., #994455
        QString colorText = color.name().toUpper();
        QString colorData = color.name().toUpper().replace("#","");

        if(ui->cbTextColor->count() == 18)
        {
            ui->cbTextColor->addItem(colorText,colorData);
        }
        else
        {
            ui->cbTextColor->setItemText(18,colorText);
            ui->cbTextColor->setItemData(18,colorData);
        }
        ui->cbTextColor->setCurrentIndex(18);
        // set color to text color
        plt.setColor(QPalette::WindowText, color);
        ui->lblSampleText->setPalette(plt);
    }
    else
    {
        if(ui->cbTextColor->currentIndex() == 0 )
        {
            // Must be Hobart Blue
            color.setNamedColor("#000099");
        }
        else
        {
            // get data text from combobox and add # to it
            color.setNamedColor("#" + QString(ui->cbTextColor->itemData(ui->cbTextColor->currentIndex()).toString()));
        }
        plt.setColor(QPalette::WindowText, color);
        ui->lblSampleText->setPalette(plt);
    }
    textColorToSave = ui->cbTextColor->itemData(ui->cbTextColor->currentIndex()).toString();
    ui->cbTextColor->setFocus();
    ui->lblSampleText->setText(ui->cbTextColor->currentText() + tr(" Text"));
}

void superCategories::setDeleteButEnabling()
{
    if(ui->leCategoryNumber->text() != "" || ui->leCategoryNumber->text().length() >0)
    {
        ui->butDelete->setEnabled(true);
    }
    else
    {
        ui->butDelete->setEnabled(false);
    }
}

void superCategories::loadPlaylists()
{
    LOG_CLASS_INFO( "superCategories", "loadPlaylists" );

    // for remote session we need to sync playlist folder
    if ( !mainWindow_->isPrimarySessionType() )
    {
        if ( !filesystem::syncFolder( folderType::Playlists ) )
            LOG_WARN( "could not sync playlist folder!", NULL );
    }

    // populate combo with playlist files
    QDir dir( filesystem::folder( folderType::Playlists ) );

    QStringList filters;
    filters << "*.txt";

    dir.setFilter( QDir::Files );
    dir.setNameFilters( filters );

    QStringList files( dir.entryList() );
    if ( files.length() > 0 )
    {
        ui->cboPlaylist->clear();
        ui->cboPlaylist->addItem("None", "None");
    }

    for( QStringList::ConstIterator file = files.begin(); file != files.end(); ++file )
    {
        QString filename( *file );
        ui->cboPlaylist->addItem( filename, filename );
    }
}

void superCategories::loadColorBoxes()
{
    ui->cbBackgroundColor->addItem("Hobart "+ tr("Blue"), "000099" );
    ui->cbBackgroundColor->addItem(tr("Black"), "000000" );
    ui->cbBackgroundColor->addItem(tr("Navy"), "000080" );
    ui->cbBackgroundColor->addItem(tr("Blue"), "0000FF" );
    ui->cbBackgroundColor->addItem(tr("Aqua"), "00FFFF" );
    ui->cbBackgroundColor->addItem(tr("Purple"), "800080" );
    ui->cbBackgroundColor->addItem(tr("Fuchsia"), "FF00FF" );
    ui->cbBackgroundColor->addItem(tr("Maroon"), "800000" );
    ui->cbBackgroundColor->addItem(tr("Red"), "FF0000" );
    ui->cbBackgroundColor->addItem(tr("Olive"), "808000" );
    ui->cbBackgroundColor->addItem(tr("Yellow"), "FFFF00" );
    ui->cbBackgroundColor->addItem(tr("Lime"), "00FF00" );
    ui->cbBackgroundColor->addItem(tr("Green"), "009900" );
    ui->cbBackgroundColor->addItem(tr("Teal"), "008080" );
    ui->cbBackgroundColor->addItem(tr("Gray"), "808080" );
    ui->cbBackgroundColor->addItem(tr("Silver"), "C0C0C0" );
    ui->cbBackgroundColor->addItem(tr("White"), "FFFFFF" );
    ui->cbBackgroundColor->addItem(tr("Custom..."), "custom" );

    ui->cbTextColor->addItem("Hobart "+tr("Blue"), "000099" );
    ui->cbTextColor->addItem(tr("Black"), "000000" );
    ui->cbTextColor->addItem(tr("Navy"), "000080" );
    ui->cbTextColor->addItem(tr("Blue"), "0000FF" );
    ui->cbTextColor->addItem(tr("Aqua"), "00FFFF" );
    ui->cbTextColor->addItem(tr("Purple"), "800080" );
    ui->cbTextColor->addItem(tr("Fuchsia"), "FF00FF" );
    ui->cbTextColor->addItem(tr("Maroon"), "800000" );
    ui->cbTextColor->addItem(tr("Red"), "FF0000" );
    ui->cbTextColor->addItem(tr("Olive"), "808000" );
    ui->cbTextColor->addItem(tr("Yellow"), "FFFF00" );
    ui->cbTextColor->addItem(tr("Lime"), "00FF00" );
    ui->cbTextColor->addItem(tr("Green"), "009900" );
    ui->cbTextColor->addItem(tr("Teal"), "008080" );
    ui->cbTextColor->addItem(tr("Gray"), "808080" );
    ui->cbTextColor->addItem(tr("Silver"), "C0C0C0" );
    ui->cbTextColor->addItem(tr("White"), "FFFFFF" );
    ui->cbTextColor->addItem(tr("Custom..."), "custom" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superCategories::translate()
{
    ui->retranslateUi(this);
    ui->cbBackgroundColor->setItemText(0,"Hobart " + tr("Blue"));
    ui->cbBackgroundColor->setItemText(1,tr("Black"));
    ui->cbBackgroundColor->setItemText(2,tr("Navy") );
    ui->cbBackgroundColor->setItemText(3,tr("Blue") );
    ui->cbBackgroundColor->setItemText(4,tr("Aqua") );
    ui->cbBackgroundColor->setItemText(5,tr("Purple") );
    ui->cbBackgroundColor->setItemText(6,tr("Fuchsia") );
    ui->cbBackgroundColor->setItemText(7,tr("Maroon") );
    ui->cbBackgroundColor->setItemText(8,tr("Red") );
    ui->cbBackgroundColor->setItemText(9,tr("Olive") );
    ui->cbBackgroundColor->setItemText(10,tr("Yellow") );
    ui->cbBackgroundColor->setItemText(11,tr("Lime") );
    ui->cbBackgroundColor->setItemText(12,tr("Green") );
    ui->cbBackgroundColor->setItemText(13,tr("Teal") );
    ui->cbBackgroundColor->setItemText(14,tr("Gray") );
    ui->cbBackgroundColor->setItemText(15,tr("Silver") );
    ui->cbBackgroundColor->setItemText(16,tr("White") );
    ui->cbBackgroundColor->setItemText(17,tr("Custom...") );

    ui->cbTextColor->setItemText(0,"Hobart " + tr("Blue") );
    ui->cbTextColor->setItemText(1,tr("Black"));
    ui->cbTextColor->setItemText(2,tr("Navy"));
    ui->cbTextColor->setItemText(3,tr("Blue"));
    ui->cbTextColor->setItemText(4,tr("Aqua"));
    ui->cbTextColor->setItemText(5,tr("Purple"));
    ui->cbTextColor->setItemText(6,tr("Fuchsia"));
    ui->cbTextColor->setItemText(7,tr("Maroon"));
    ui->cbTextColor->setItemText(8,tr("Red"));
    ui->cbTextColor->setItemText(9,tr("Olive"));
    ui->cbTextColor->setItemText(10,tr("Yellow"));
    ui->cbTextColor->setItemText(11,tr("Lime"));
    ui->cbTextColor->setItemText(12,tr("Green"));
    ui->cbTextColor->setItemText(13,tr("Teal"));
    ui->cbTextColor->setItemText(14,tr("Gray"));
    ui->cbTextColor->setItemText(15,tr("Silver"));
    ui->cbTextColor->setItemText(16,tr("White"));
    ui->cbTextColor->setItemText(17,tr("Custom..."));
    update();
}

void superCategories::choseFlashkeyGraphic()
{
  ;
}

void superCategories::CLU() // do a Category Look Up
{
    // Send to lookup CLU (Category Look Up)
    //Send: 000058121,6,2,999999,CHEESE,,photo_swiss.jpg,FFFFFF,000000
    //Send: 000013121,0,1

    if(categoryNumber != ui->leCategoryNumber->text() && ui->leCategoryNumber->text() != "")
    {
        if(categoryNumber != "" && categoryNumber != "0")
        {
            saveCLU();
        }

        //Look It Up
        session_->send(CategoryDev, 0, ui->leCategoryNumber->text());
    }
}

void superCategories::setPreviousCategoryData()
{
     //   previousCategoryNumberText = ui->leCategoryNumber->text();
    previousCategoryNumberCB = ui->cbReportingCategory->itemData(ui->cbReportingCategory->currentIndex()).toString();
    previousPrimary = ui->lePrimary->text();
    previousSecondary = ui->leSecondary->text();
    previousFlashkeyGraphic = ui->cbFlashkeyGraphic->currentText();
    previousBackgroundColor = ui->cbBackgroundColor->itemData(ui->cbBackgroundColor->currentIndex()).toString();
    previousTextColor = ui->cbTextColor->itemData(ui->cbTextColor->currentIndex()).toString();
}

void superCategories::saveCLU()
{
    QString msg = categoryNumber + "," + ui->cbReportingCategory->itemData(ui->cbReportingCategory->currentIndex()).toString() + ",\"" + ui->lePrimary->text() + "\",\"" + ui->leSecondary->text() + "\",";

    if(ui->cbFlashkeyGraphic->currentIndex() != 0)
    {
        msg += ui->cbFlashkeyGraphic->currentText();
    }
    else
    {
        // FT-1109 save "None" graphic
        msg += "\"\"";
    }

    msg += "," + ui->cbBackgroundColor->itemData(ui->cbBackgroundColor->currentIndex()).toString() + "," + ui->cbTextColor->itemData(ui->cbTextColor->currentIndex()).toString();
    msg += "," + ui->cboPlaylist->itemData( ui->cboPlaylist->currentIndex() ).toString();

    session_->send(CategoryDev, CategoryDtWriteSingle, msg);

    // FT-148: now that we saved off the new category, add the new category to the combo box at the end
    // right now we only have the categoryNumber, we will have to update the name in the combo box
    // tied to the categorynumber in "updatePrimaryToSave() SLOT method
    //if( leavingPage == false )
    //    ui->cbReportingCategory->addItem( "(" + categoryNumber + ")", categoryNumber);

}

void superCategories::reloadFields(const SessionMessage& msg)
{
     DomCategoryItem m (msg);

     previousCategoryNumberText = m.categorySetupNum();

     if(m.categorySetupNum() == "0" || m.categorySetupNum() == "999999")
     {
         ui->leCategoryNumber->setText("");
         ui->cbReportingCategory->setCurrentIndex(0);
         ui->butDelete->setEnabled(false);
     }
     else
     {
         ui->leCategoryNumber->fromSessionField(m.categorySetupNum());
         ui->butDelete->setEnabled(true);
     }

     int index = ui->cbReportingCategory->findData(m.reportingCategory());

     if(index != -1)
     {
         ui->cbReportingCategory->setCurrentIndex(index);
     }
     else
     {
         ui->cbReportingCategory->setCurrentIndex(0);
     }

     if(m.categorySetupNum() == "999999")
     {
         ui->cbReportingCategory->setEnabled(false);
     }
     else
     {
         ui->cbReportingCategory->setEnabled(true);
     }
     previousReportingCategory = m.reportingCategory();

     // JHC For testing.  I entered spanish characters into these 2 fields.
     // Now trying to display them.  It only displays weird chars or ? chars.
     // ENTERING them from the keyboard using the virtual keyboard works.

     // Initial test - does not work - displays question marks
     //QString testStr1 = m.primaryName();
     //QString testStr2 = m.secondName();

     // test 2 - did not work - displays question marks
     //QString testStr1 = m.primaryName().toLatin1();
     //QString testStr2 = m.secondName().toLatin1();

     // test 3 - did not work - displays question marks
     //QString testStr1 = m.primaryName().toLatin1().constData();
     //QString testStr2 = m.secondName().toLatin1().constData();

     // test 4 - did not work - displays question marks
     // QString testStr1 = QString::fromUtf8(m.primaryName().toLatin1());
     // QString testStr2 = QString::fromUtf8(m.secondName().toLatin1());

     // test 5 - did not work - displays black diamond question marks
     //QString testStr1 = m.primaryName().toUtf8();
     //QString testStr2 = m.secondName().toUtf8();

     // test 6 - did not work - displays black diamond question marks
     //QString testStr1 = QString::fromUtf8( m.primaryName() );
     //QString testStr2 = QString::fromUtf8( m.secondName() );

     // Test 7 - This works, and ENTERING them works.  So if I got them properly, I would display them right.
     //QString testStr1 = "ÁÉÍÓÚÜÑ";
     //QString testStr2 = "áéíóúüñ";

     //ui->lePrimary->setText( testStr1 );
     //ui->leSecondary->setText( testStr2 );

     // real non-working code
     ui->lePrimary->setText( m.primaryName() );
     ui->leSecondary->setText( m.secondName() );

     backgroundColorIndex = ui->cbBackgroundColor->findData(m.bgColor());

     if(backgroundColorIndex < 0)
     {
         // add it to the combo
         ui->cbBackgroundColor->addItem("#" + m.bgColor(), m.bgColor());
         // set combobox
         backgroundColorIndex = ui->cbBackgroundColor->count()-1;
     }
     ui->cbBackgroundColor->setCurrentIndex(backgroundColorIndex);

     textColorIndex = ui->cbTextColor->findData(m.textColor());

     if(textColorIndex < 0)
     {
         // add it to the combo
         ui->cbTextColor->addItem("#" + m.textColor(), m.textColor());
         // set combobox
         textColorIndex = ui->cbTextColor->count()-1;
     }

     ui->cbTextColor->setCurrentIndex(textColorIndex);

     if(m.graphicName() == "None.jpg" || m.graphicName() == "")
     {
         ui->cbFlashkeyGraphic->setCurrentIndex(0);
     }
     else
     {
         ui->cbFlashkeyGraphic->setCurrentIndex(ui->cbFlashkeyGraphic->findText(m.graphicName()));
     }

     textColorPicker("#" + m.textColor());
     backgroundColorPicker("#" + m.bgColor());

     // Update the category playlist
     int playlistIndex = 0;
     if ( (playlistIndex = ui->cboPlaylist->findText( m.categoryPlaylist() )) < 0 )
     {
         // Dont add a blank item.
         if ( m.categoryPlaylist().length() > 0 && m.categoryPlaylist() != " ")
         {
            ui->cboPlaylist->addItem( m.categoryPlaylist(), m.categoryPlaylist() );
            playlistIndex = ui->cboPlaylist->count() - 1;
         }
     }

     ui->cboPlaylist->blockSignals( true );
     ui->cboPlaylist->setCurrentIndex( playlistIndex );
     ui->cboPlaylist->blockSignals( false );

     categoryNumberToSave = m.categorySetupNum();

     update();
}

void superCategories::removeCategoryFromCombo(QString category)
{
    ui->cbReportingCategory->removeItem(ui->cbReportingCategory->findData(category));
}

void superCategories::clearTheFields()
{
    ui->leCategoryNumber->setText("");
    categoryNumber = "";
    ui->cbReportingCategory->setCurrentIndex(0);
    ui->lePrimary->setText("");
    ui->leSecondary->setText("");
    ui->cbFlashkeyGraphic->setCurrentIndex(0);
    ui->cbFlashkeyGraphic->setEnabled(true);
    ui->butDelete->setEnabled(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superCategories::onSessionMessage( const SessionMessage& msg )
{
    if(CategoryDev == msg[0].toInt() && CategoryWriteAllMainCategories == msg[1].toInt())//121,23
    {
        //Example Update: 121,23,5,34,"999999,30,40,10,11,50,12"

        // We need a couple string lists
        QVector<QString> names( TokenParser::splitStringTokens( msg[6] ) );
        QStringList categoryNumsList;
        QStringList categoryNamesList;

        categoryNumsList = msg[4].split(",", QString::KeepEmptyParts);
        categoryNamesList = msg[6].split(",", QString::KeepEmptyParts);

        ui->cbReportingCategory->clear();
        for(int i = 0; i < categoryNumsList.count(); i++)
        {
            ui->cbReportingCategory->addItem((names.at(i) + "  (" + categoryNumsList.at(i) + ")"), categoryNumsList.at(i));
        }
        session_->send(CategoryDev, CategoryReadAllCatGraphicNames); // 121,20

    }
    else if(CategoryDev == msg[0].toInt() && CategoryWriteAllCatGraphicNames == msg[1].toInt())            //121,21
    {
        // Example Update: 121,21,68,"beef.gif,beef_b.gif,biscuit.gif,biscuit_b.gif,blue.gif"
        // We need a string list

        // FT-1109: moved the clear call before the "None" option is added
        ui->cbFlashkeyGraphic->clear();

        // first add the "None" choice
        ui->cbFlashkeyGraphic->addItem(tr("None"),"None.jpg");

        QStringList flashkeyGraphicsList;

        flashkeyGraphicsList = msg[3].split(",", QString::KeepEmptyParts);

        for(int i = 0; i < flashkeyGraphicsList.count(); i++)
        {
            ui->cbFlashkeyGraphic->addItem((flashkeyGraphicsList.at(i), flashkeyGraphicsList.at(i)));
        }
    }
    else if(CategoryDev == msg[0].toInt() && CategoryDtWrite == msg[1].toInt()) //121,2
    {
        // Example Update: 121,2,2,999999,"CHEESE","","photo_swiss.jpg",FFFFFF,000000
        //                  0  1 2   3        4     5         6            7      8
        // Got data for a new category, load the screen
        if(creatingNewCategory)
        {
            QString message = msg[2] + "," + msg[3] + "," + msg[4] + "," + msg[5] + "," + msg[6] + "," + msg[7] + "," + msg[8];
            session_->send(CategoryDev, CategoryDtWriteSingle, message);
            creatingNewCategory = false;

            ui->cbReportingCategory->addItem( "(" + msg[2] + ")", msg[2]);
        }
        categoryNumber = msg[2];
        loading = true;
        reloadFields(msg);

    }
    else if (Error == msg[0].toInt() && ErrCategoryDevice == msg[1].toInt())
    {
        // Received 103,131 - send read category, it doesn't exist so create new or delete one
        returnedErrorsCategoryDev m(msg);

        int var = m.errorData2();

        if (var)
        {
            var = 121001 == msg[2].toInt() ? CategoryDtCreateNewSingle:CategoryDtDeleteSingle;
            if(var == CategoryDtCreateNewSingle)
            {
                creatingNewCategory = true;
            }
            else
            {
                categoryNumber = "";
                //remove from reporting category list
                removeCategoryFromCombo(ui->leCategoryNumber->text());
            }
            // Create or Delete category
            session_->send(CategoryDev, var, ui->leCategoryNumber->text());
        }
        else
        {
            clearTheFields();
        }
    }
}
