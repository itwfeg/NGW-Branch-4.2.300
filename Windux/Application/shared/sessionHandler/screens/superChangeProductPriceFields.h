/**
 * @file superChangeProductPriceFields.h
 * @brief My Class

  *
  * @copyright Copyright (c) 2013 Hobart Corporation
  * @author Todd
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/

#ifndef SUPERCHANGEPRODUCTPRICEFIELDS_H
#define SUPERCHANGEPRODUCTPRICEFIELDS_H

#include "screenwidget.h"

class DomCurrentPLU;
class MadpScaleConfig;
class MadpCurrencySettings;
class MadpInternalConfig;
class QIntValidator;
class MadpFrequentShopper;
class DomNormalScaleOperation;

namespace Ui {
class superChangeProductPriceFields;
}

/**
* @class superChangeProductPriceFields
* @brief  
*  
*/

class superChangeProductPriceFields  : public ScreenWidget
{
    Q_OBJECT

    typedef superChangeProductPriceFields _Myt;
    typedef ScreenWidget _Mybase;
    
public:
    /**
      * Constructor.
      */
    explicit superChangeProductPriceFields(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~superChangeProductPriceFields();

    ///////////////////////////////////////////////////////////////////////////
    // Properties
    /**
     * Process the help text
     */
    virtual bool helpText( QString& title, QString& description ) const;

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:
// Sample Slot Methods for various widgets

    /**
     * Slot to blank By count field.
     */
    void reactToForcedByCountClick(int);

    /**
     * Slot to blank Unit Price field.
     */
    void reactToForcedPriceClick(int);

    /**
     * Slot to blank Tare field.
     */
    void reactToForcedTareClick(int);

    /**
     * Slot to blank Pounds for Price field.
     */
    void reactToPoundsForPriceForcedPriceClick(int state);

    /**
     * Slot to blank Fractional Pricing field.
     */
    void reactToFractionalPricingForcedPriceClick(int);

    void reactToDiscountTypeChanged(int index);

    /**
    * SLOT method called when a New product is chosen to be created.
    *
    * @param[in] none
    * @returns  none
    */
    void createNewProductOK_clicked();

    /**
    * SLOT method called when a New product is not chosen to be created (Cancel Button).
    *
    * @param[in] none
    * @returns  none
    */
    void createNewProductCancel_clicked();

    /**
     * Slot to lookup a PLU.
     *
     * @param[in] none
     * @returns  none
     */
    void doPluLookup();

    /**
     * Slot to trigger a single field write message when the Unit Price is finished being edited
     *
     * @param[in] none
     * @returns  none
     */
    void trackSuperChangeUnitPrice();

    /**
     * Slot to trigger a single field write message when the Pounds For Price is finished being edited
     *
     * @param[in] none
     * @returns  none
     */
    void trackSuperChangeLbsForPrice();

    /**
     * Slot to trigger a single field write message when the Fractional Price is finished being edited
     *
     * @param[in] none
     * @returns  none
     */
    void trackSuperChangeFractionalPrice();

    /**
     * Slot to trigger a single field write message when the Tare is finished being edited
     *
     * @param[in] none
     * @returns  none
     */
    void trackSuperChangeTare();

    /**
     * Slot to trigger a single field write message when the Proportional Tare is finished being edited
     *
     * @param[in] none
     * @returns  none
     */
    void trackSuperChangePropTare();

    /**
     * Slot to trigger a single field write message when the By Count is finished being edited
     *
     * @param[in] none
     * @returns  none
     */
    void trackSuperChangeByCount();


    /**
     * Slot to trigger a single field write message when the Net Weight is finished being edited
     *
     * @param[in] none
     * @returns  none
     */
    void trackSuperChangeNetWeight();

    /**
     * Slot to trigger a single field write message when the Fluid Net Weight is finished being edited
     *
     * @param[in] none
     * @returns  none
     */
    void trackSuperChangeFluidNetWeight();

    /**
     * Slot to trigger a focus event to indicate the last field to have focus
     *
     * @param[in] none
     * @returns  none
     */
    void onUnitPriceTextChanged(QString focusTxt);

    /**
     * Slot to trigger a focus event to indicate the last field to have focus
     *
     * @param[in] none
     * @returns  none
     */
    void onLbsForTextChanged(QString focusTxt);

    /**
     * Slot to trigger a focus event to indicate the last field to have focus
     *
     * @param[in] none
     * @returns  none
     */
    void onFracPriceTextChanged(QString focusTxt);

    /**
     * Slot to trigger a focus event to indicate the last field to have focus
     *
     * @param[in] none
     * @returns  none
     */
    void onTareTextChanged(QString focusTxt);

    /**
     * Slot to trigger a focus event to indicate the last field to have focus
     *
     * @param[in] none
     * @returns  none
     */
    void onPropTareTextChanged(QString focusTxt);

    /**
     * Slot to trigger a focus event to indicate the last field to have focus
     *
     * @param[in] none
     * @returns  none
     */
    void onByCountTextChanged(QString focusTxt);

    /**
     * Slot to trigger a focus event to indicate the last field to have focus
     *
     * @param[in] none
     * @returns  none
     */
    void onNetWtTextChanged(QString focusTxt);

    /**
     * Slot to trigger a focus event to indicate the last field to have focus
     *
     * @param[in] none
     * @returns  none
     */
    void onFluidNetWtTextChanged(QString focusTxt);

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

    /**
     * Method to save a product to backend with UI's data during UI destruction.
     *
     * @param[in] none
     * @returns  none
     */
    void saveProduct();

    /**
     * Method to reload data fields with a PLU's data.
     *
     * @param[in] none
     * @returns  none
     */
    void reloadFields();

    /**
     * Method to handle creating a new PLU.
     *
     * @param[in] none
     * @returns  none
     */
    void popupCreateNewPLUDialog();

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

    int makeValidatorTop(int digits);
    void hidePriceFields();
    void zeroOutTareFromForcedTareClick();
    void zeroOutAPrice(QString field);

    Ui::superChangeProductPriceFields *ui;

    DomCurrentPLU *currentPLUData;
    MadpScaleConfig *scaleConfigData;
    MadpCurrencySettings *currencyData;
    MadpInternalConfig *internalConfigData;
    MadpFrequentShopper *frequentShopperData;
    DomNormalScaleOperation *normalScaleOperation;

    SaveWho whoHasFocus;

    QString currentPLU;
    QString newPLUInfo;
    QString itemNumber;
    QString productNumber;
    QString description;
    QString previousClass;
    QString previousProductNumber;
    bool hasChanged;
    bool pluPassedIn;

    QString newProductNumber;
    QString newClass;
    ///////////////////////
    QString ProductType;
    QString isProportionalTareOn;
    QString weightOnByCount;
    QString isFractionalPricing;
    QString fractionalPricingOn;
    QString poundsForPricingOn;
    QString isScaleMetric;
    QString isEanOrUpc;
    QString numberSystem;
    QString scaleModel;
    QString separator;

    QString prev_unitprice;
    QString prev_lbsforprice;
    QString prev_fracprice;
    QString prev_tareA;
    QString prev_tareB;
    QString prev_proptare;
    QString prev_bycount;
    QString prev_netwt;
    QString prev_fluidnetwt;

    int nextSectionAvailable;
    int nextSectionAvailableX;
    int nextSectionAvailableY;
    int locationOfDiscountOrRegularPriceGroupBoxY;
    int YForPoints;
    int pluEmitted;

    QIntValidator *productNumberValidator_,
    *byCountValidator_,
    *discountValue1Validator_,
    *discountValue2Validator_,
    *discountValue3Validator_,
    *unitPriceValidator_,
    *regularPriceValidator_,
    *tareValidator_,
    *netWeightValidator_,
    *netWeightFluidValidator_,
    *exceptionPriceValidator_,
    *fractionalPriceValidator_,
    *poundsForPriceValidator_,
    *proportionalTareValidator_,
    *pointsValidator_;

    bool cameFromCancel;
    bool popupActive;
    bool switchScreensOnNewPLU;
    bool requestedPlu;
    bool isPlatterTareOn;
    bool showForcedFields;
    bool wroteTransaction;
    bool ifUpdating;

    bool gbUnitPriceShown,
    gbByCountShown,
    gbTareShown,
    gbNetWeightOuncesShown,
    gbNetWeightFluidShown,
    gbExceptionPriceShown,
    gbFractionalPriceShown,
    gbPoundsForPriceShown,
    gbProportionalTareShown,
    gbTarePlatterTypeShown,
    gbRegularPriceShown,
    gbDiscountShown;

    QString widgetUsedForPriceModifier;
    QString widgetUsedForDiscountValue1;
    QString widgetUsedForFixedWeight;

    QString maxTareFromScaleConfig;
    QString tareEntryPrecisionFromScaleConfig;


};

#endif // SUPERCHANGEPRODUCTPRICEFIELDS_H
