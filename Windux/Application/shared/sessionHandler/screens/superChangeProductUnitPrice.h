/**
 * @file superChangeProductUnitPrice.h
 * @brief superChangeProductUnitPrice
  *
  * @copyright Copyright (c) 2013 Hobart Corporation
  * @author Todd
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/

#ifndef SUPERCHANGEPRODUCTUNITPRICE_H
#define SUPERCHANGEPRODUCTUNITPRICE_H

#include "screenwidget.h"

class DomCurrentPLU;
class MadpScaleConfig;
class MadpCurrencySettings;
class MadpInternalConfig;
class QIntValidator;

namespace Ui {
class superChangeProductUnitPrice;
}

/**
* @class superChangeProductUnitPrice
* @brief  Allows User to change the Unit price values.
*  
*/

class superChangeProductUnitPrice  : public ScreenWidget
{
    Q_OBJECT

    typedef superChangeProductUnitPrice _Myt;
    typedef ScreenWidget _Mybase;
    
public:
    /**
      * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit superChangeProductUnitPrice(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~superChangeProductUnitPrice();

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

    /* *
       * Method to save a product to backend with UI's data during UI destruction.
       *
       * @param[in] none
       * @returns  none

      void saveProduct();
*/
      /**
        * Method to handle creating a new PLU.
        *
        * @param[in] none
        * @returns  none
        */
       void popupCreateNewPLUDialog();


    /**
         * Slot to blank pounds for pricing count field.
         */
    void reactToForcedPoundsForPricingClick(int);

    /**
         * Slot to blank fractional pricing field.
         */
    void reactToForcedFractionalPricingClick(int);

    /**
         * Slot to blank unit price field.
         */
    void reactToForcedUnitPriceClick(int);

    /**
         * Slot to clear previously checked radio buttons from new product.
         *
         * @param[in] none
         * @returns  none
         */
        void clearCreatePLURadios();

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
         * Sets up moving field focus to PLU field when Enter clicked on Unit Price field.
         *
         * @param[in] none
         * @returns  none
         */
        //void transferToThePluField();  // moved to a private method called by next 3 methods

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
         * Slot to trigger setting a flag when a field is edited
         *
         * @param[in] none
         * @returns  none
         */
        void onUnitPriceTextChanged(QString focusTxt);

        /**
         * Slot to trigger setting a flag when a field is edited
         *
         * @param[in] none
         * @returns  none
         */
        void onLbsForTextChanged(QString focusTxt);

        /**
         * Slot to trigger setting a flag when a field is edited
         *
         * @param[in] none
         * @returns  none
         */
        void onFracPriceTextChanged(QString focusTxt);


        /**
         * A timer that moves field focus to PLU field when Enter clicked on Unit Price field.
         *
         * @param[in] none
         * @returns  none
         */
        void setHardFocusToPluField();

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
    virtual bool eventFilter( QObject *obj, QEvent *event );

private:
    Ui::superChangeProductUnitPrice *ui;

    /**
     * Method to set up the dates in the range on the dialog so the user can select a range
     * of dates in order to track changes in that range of dates.
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateTransactionDates(const SessionMessage &msg);

    void reloadFields();

    void hasUIChanged();

    int makeValidatorTop(int digits);

    void transferToTheUnitPriceField();

    void saveProduct();

    void transferToThePluField();

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

    SaveWho whoHasFocus;                                                // who has focus

    QIntValidator *unitPriceValidator_;
    QIntValidator *fractionalPriceValidator_;
    QIntValidator *poundsForPriceValidator_;
    QIntValidator *productNumberValidator_;

    QString currentPLU;
    QString newPLUInfo;
    QString itemNumber;
    QString productNumber;
    QString description;
    QString isMetric;
    QString previousProductType;
    QString separator;
    QString prev_unitprice;
    QString prev_lbsforprice;
    QString prev_fracprice;

    bool showForcedFields;
    int shownGroupBox;

    bool showForcedFieldsBOOL;
    bool cameFromCancel;
    bool popupActive;
    bool switchScreensOnNewPLU;

    bool ignoreSignals;
    bool requestedPlu;
    bool alwaysSaveThisPlu;
    bool savingPLUData;
    bool hasChanged;
    bool didThisAlready;
    bool wroteTransaction;
    bool justUncheckedForcedPrice;
    bool ifUpdating;

    DomCurrentPLU *currentPLUData;
    MadpScaleConfig *scaleConfigData;
    MadpCurrencySettings *currencyData;
    MadpInternalConfig *internalConfigData;

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

};

#endif // superChangeProductUnitPrice
