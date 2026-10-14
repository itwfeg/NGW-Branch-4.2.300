/**
 * @file superChangeProductTare.h
 * @brief super Change Product Tare
*
* @copyright Copyright (c) 2013 Hobart Corporation
* @author Todd
* @addtogroup Supervisor and Service Mode CSU
* @{
*/

#ifndef SUPERCHANGEPRODUCTTARE_H
#define SUPERCHANGEPRODUCTTARE_H

#include "screenwidget.h"
#include "ScaleConfiguration.h"

class DomCurrentPLU;
class DomNormalScaleOperation;
class MadpScaleConfig;
class MadpCurrencySettings;
class MadpInternalConfig;
class QIntValidator;
class ScaleConfiguration;

namespace Ui {
class superChangeProductTare;
}

/**
* @class superChangeProductTare
* @brief Allows changes in a product's tare values.
*  
*/

class superChangeProductTare  : public ScreenWidget
{
    Q_OBJECT

    typedef superChangeProductTare _Myt;
    typedef ScreenWidget _Mybase;
    
public:
    /**
      * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit superChangeProductTare(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~superChangeProductTare();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:
// Sample Slot Methods for various widgets


    /**
    * SLOT method called when product or class change to signal a save on close of form
    *
    * @param[in] none
    * @returns  none
    */

    void doSave();

    /**
    * SLOT method called to create new product and user clicks OK.
    */

    void createNewProductOK_clicked();
    /**
    * SLOT method called wehn user clicks cancel to not create a new product.
    */
    void createNewProductCancel_clicked();
    /**
    * SLOT method called to search data for a PLU
    */
    void doPluLookup();
    /**
    * SLOT method to hide the over limit ntoice to user.
    */
    void hideMaxReachedGroupBox();

    void reactToForcedTareClick(int);

    void checkAgainstMaxTare(QString);

    //void trackSuperChangeTare();      // Turned into a privcate method called by existing SLOT method
    void trackSuperChangePropTare();

    void onTareTextChanged(QString focusTxt);
    void onPropTareTextChanged(QString focusTxt);


protected:
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

    virtual bool eventFilter( QObject *obj, QEvent *event );

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

private:
    Ui::superChangeProductTare *ui;

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
    * Method to reload fields with new data.
    */

    void reloadFields();

    void zeroOutTareFromForcedTareClick();

    void saveProduct();

    enum SaveWho
    {
        SAV_UNITPRICE,
        SAV_LBSFOR,
        SAV_FRACPRICE,
        SAV_TARE,
        SAV_PROPTARE,
        SAV_BYCOUNT,
        SAV_NETWT,
        SAV_FLNETWT,
        SAV_NONE
    };

    QIntValidator *productNumberValidator_;
    QIntValidator *tareValidator_;
    QIntValidator *proportionalTareValidator_;

    int makeValidatorTop(int digits);

    void trackSuperChangeTare();

    QString currentPLU;
    QString newPLUInfo;
    QString itemNumber;
    QString productNumber;
    QString description;
    QString previousTare;
    QString previousForcedTareCheckbox;
    QString previousProportionalTare;
    QString previousProductNumber;
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
    QString prev_tareA;
    QString prev_tareB;
    QString prev_proptare;

    DomCurrentPLU *currentPLUData;
    DomNormalScaleOperation *normalScaleOperation;
    MadpScaleConfig *scaleConfigData;
    MadpCurrencySettings *currencyData;
    MadpInternalConfig *internalConfig;
    // *weigherData;
    DomWeigherConfigRead *weigherConfig;

    SaveWho whoHasFocus;                                                // who has focus

    QString maxTareFromScaleConfig;
    QString tareEntryPrecisionFromScaleConfig;
    QString oldTareValidationString;

    bool hasChanged;
    bool pluPassedIn;
    bool showForcedFields;
    bool firstTimeIn;
    bool wroteTransaction;
    bool ifUpdating;

    int nextSectionAvailableX;
    int nextSectionAvailableY;
    int pluEmitted;                                                 // what plu are we requesting?

    bool realCheck, latentCheck, reachedLimit;

    QString separator;

    bool cameFromCancel;
    bool popupActive;
    bool switchScreensOnNewPLU;
    bool requestedPlu;

    bool ok;
    bool wasSeparatorEntered;

//    QString separator;



};

#endif // SUPERCHANGEPRODUCTTARE_H
