/**
 * @file superCategories.h
 * @brief Widget to set configuration of Categories.
  *
  * @copyright Copyright (c) 2013 Hobart Corporation
  * @author Todd
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/

#ifndef SUPERCATEGORIES_H
#define SUPERCATEGORIES_H

#include "screenwidget.h"

#include <QDir>

class MadpScaleConfig;
class MadpInternalConfig;
class QIntValidator;

namespace Ui {
class superCategories;
}

/**
* @class superCategories
* @brief  Changes a category class
*  
*/

class superCategories  : public ScreenWidget
{
    Q_OBJECT

    typedef superCategories _Myt;
    typedef ScreenWidget _Mybase;
    
public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit superCategories(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~superCategories();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:

    /**
     * Slot to perform a Category Look Up
     *
     * @param[in] none
     * @returns  none
     */
    void CLU();
    /**
     * Slot to offer User choice of Flashkey graphic.
     *
     * @param[in] none
     * @returns  none
     */
    void choseFlashkeyGraphic();

    /**
     * Slot to enable/disable Delete Button.
     *
     * @param[in] none
     * @returns  none
     */
    void setDeleteButEnabling();

    /**
     * SLOT to offer color choice for background
     *
     * @param[in] none
     * @returns  none
     */
    void backgroundColorPicker(QString);

    /**
     * SLOT to offer color choice for text
     *
     * @param[in] none
     * @returns  none
     */
    void textColorPicker(QString);

    /**
     * SLOT to update primary caption to save
     *
     * @param[in] none
     * @returns  none
     */
    void updatePrimaryToSave(QString);

    /**
     * SLOT to update secondary caption to save
     *
     * @param[in] none
     * @returns  none
     */
    void updateSecondaryToSave(QString);

    /**
     * SLOT to update flashkey graphic to save
     *
     * @param[in] none
     * @returns  none
     */
    void updateFlashkeyToSave(QString);

    /**
     * SLOT to update reporting category to save
     *
     * @param[in] none
     * @returns  none
     */
    void updateReportingCategory(int);

    /**
     * SLOT to prompt assurance when delete button clicked
     *
     * @param[in] none
     * @returns  none
     */
    void deleteCategoryButClicked();

    /**
     * SLOT to delete a category when OK button clicked
     *
     * @param[in] none
     * @returns  none
     */
    void areYouSureOKButClicked();

protected:
    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

    /**
     * Handle screen changing.
     *
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    virtual bool eventFilter( QObject *obj, QEvent *event );



private:
    Ui::superCategories *ui;

    QDir dir;

    /**
     * SLOT to prompt assurance when delete button clicked
     *
     * @param[in] none
     * @returns  none
     */
    void loadPlaylists();

    /**
     * Method to load combo boxes with predefined colors
     *
     * @param[in] none
     * @returns  none
     */
    void loadColorBoxes();

    /**
     * Method to store previous Category data for an upcoming save
     *
     * @param[in] none
     * @returns  none
     */
    void setPreviousCategoryData();

    /**
     * Method to save CLU data
     *
     * @param[in] none
     * @returns  none
     */
    void saveCLU();

    void updateDataToSave();

    void reloadFields(const SessionMessage& msg);

    int makeValidatorTop(int digits);

    void clearTheFields();

    void removeCategoryFromCombo(QString category);

    QIntValidator *categoryValidator_;

    MadpScaleConfig *scaleConfigData;
    MadpInternalConfig *internalConfigData;

    bool isIPSelected;
    bool categoryHasBeenChosen;
    bool loading;
    bool lookingUpCLU;
    bool creatingNewCategory;
    bool leavingPage;

    int pixmapWidth;
    int pixmapHeight;
    int pixmapY;

    int backgroundColorIndex;
    int textColorIndex;
    QString previousReportingCategory;

    QString alertMsg;

    QString previousCategoryNumberText;
    QString previousCategoryNumberCB;
    QString previousPrimary;
    QString previousSecondary;
    QString previousBackgroundColor;
    QString previousTextColor;
    QString previousFlashkeyGraphic;

    /////////
    QString categoryNumberToSave;
    QString reportingCategoryToSave;
    QString primaryToSave;
    QString secondaryToSave;
    QString backgroundColorToSave;
    QString textColorToSave;
    QString playlistFileToSave;
    QString flashkeyGraphicToSave;
    /////////

    QString categoryNumber;

};
/** @} */
#endif // SUPERCATEGORIES
