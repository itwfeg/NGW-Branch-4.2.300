/**
 * @file superChangeProductShelfLife.h
 * @brief super Change Product Shelf Life
  *
  * @copyright Copyright (c) 2013 Hobart Corporation
  * @author Todd
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/

#ifndef SUPERCHANGEPRODUCTSHELFLIFE_H
#define SUPERCHANGEPRODUCTSHELFLIFE_H

#include "screenwidget.h"

class DomCurrentPLU;
class DomNormalScaleOperation;
class MadpScaleConfig;
class MadpCurrencySettings;
class QIntValidator;
class MadpInternalConfig;

namespace Ui {
class superChangeProductShelfLife;
}

/**
* @class superChangeProductShelfLife
* @brief Widget allows product shelf life value changes.
*  
*/

class superChangeProductShelfLife  : public ScreenWidget
{
    Q_OBJECT

    typedef superChangeProductShelfLife _Myt;
    typedef ScreenWidget _Mybase;
    
public:
    /**
      * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit superChangeProductShelfLife(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~superChangeProductShelfLife();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve screen help text.
     *
     * When help is activated on a screen this property returns the screen title and help text.
     *
     * @param[out] title  screen title
     * @param[out] description  screen help description
     * @returns  @c true if help exists for the screen, @c false otherwise
     */
    virtual bool helpText( QString& title, QString& description ) const;

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:
// Sample Slot Methods for various widgets

    /**
    * SLOT method called when product or shelf/product life change to signal a save on close of form
    *
    * @param[in] none
    * @returns  none
    */
    void doSave();

    /**
    * SLOT method called when a new product number is entered.
    *
    * @param[in] none
    * @returns  none
    */
    void createNewProductOK_clicked();
    /**
    * SLOT method called when a new product number is not chosen to be created.
    *
    * @param[in] none
    * @returns  none
    */
    void createNewProductCancel_clicked();
    /**
    * SLOT method called when a new product number is entered and the data needs searched.
    *
    * @param[in] none
    * @returns  none
    */

    void doPluLookup();

protected:

    /**
     * Check for specific error message is handled by screen.
     *
     * Session will call this method upon receiving an error message. The default error handling
     * (i.e. popup an error dialog) shall be invoked unless @c true is returned from
     * this property.
     *
     * @returns  @c true to use custom error handler, @c false otherwise
     */
    virtual bool isErrorMessageHandler( const SessionMessage& errorMsg ) const;

    /**
     * Handle screen changing.
     *
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:
    Ui::superChangeProductShelfLife *ui;
    MadpInternalConfig *internalConfigData;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method to Popup Dialog to create new PLU
     *
     * @param[in] none
     * @returns  none
     */
    void popupCreateNewPLUDialog();
    /**
     * Method to reload data fields
     *
     * @param[in] none
     * @returns  none
     */

    void reloadFields();
    /**
     * Method to blank radeio buttons on new product creation.
     *
     * @param[in] none
     * @returns  none
     */

    void clearCreatePLURadios();

    void saveProduct();


    QIntValidator *productNumberValidator_;
    QIntValidator *shelfLifeDaysValidator_;
    QIntValidator *shelfLifeHoursValidator_;
    QIntValidator *shelfLifeMinutesValidator_;

    QIntValidator *productLifeDaysValidator_;
    QIntValidator *productLifeHoursValidator_;
    QIntValidator *productLifeMinutesValidator_;

    int makeValidatorTop(int digits);

    QString currentPLU;
    QString newPLUInfo;
    QString itemNumber;
    QString productNumber;
    QString description;
    QString previousTare;
    QString previousForcedTareCheckbox;
    QString previousProportionalTare;
    QString previousProductNumber;
    bool hasChanged;
    bool pluPassedIn;

    bool cameFromCancel;
    bool popupActive;
    bool switchScreensOnNewPLU;
    bool requestedPlu;

    QString scaleModel;
    QString isScaleMetric;
    QString isProportionalTareOn;
    QString isWeightOnByCount;
    QString productType;
    QString loadedProductType;
    QString itemFocus;
    QString askAgain;
    QString pluNumberChanged;
    QString changedElement;
    DomCurrentPLU *currentPLUData;
    DomNormalScaleOperation *normalScaleOperation;
    MadpScaleConfig *scaleConfigData;
    MadpCurrencySettings *currencyData;

    QString maxTareFromScaleConfig;
    QString tareEntryPrecisionFromScaleConfig;
    QString oldTareValidationString;

    double oldTareFloat;
    double newTareFloat;

    int pluEmitted;

    QStringList splitTareStringList;
    QString tempTarewholeNumberString;
    QString newTareString;

    QString separator;

    QString left;
    QString right;
    QString overTareLimitUserEntry;
    bool isANumber;

    bool ok;
    bool wasSeparatorEntered;

    bool enteredProportionalTareField;

    QString rightMostNewestEntry;

    QStringList splitProportionalTareStringList;
    QString tempProportionalTarewholeNumberString;
    QString newProportionalTareString;

//    QString separator;

    QString leftProportional;
    QString rightProportional;
    QString overProportionalTareLimitUserEntry;
    QString rightMostNewestProportionalEntry;
    QString oldProportionalTareValidationString;

/////////////////////////////////////////////////
    QString newShelfLifeDays;
    QString newShelfLifeHours;
    QString newProductLifeDays;
    QString newProductLifeHours;

    QString previousShelfLifeDays;
    QString previousShelfLifeHours;
    QString previousProductLifeDays;
    QString previousProductLifeHours;

    QString newProductNumber;
};

#endif // SUPERCHANGEPRODUCTSHELFLIFE_H
