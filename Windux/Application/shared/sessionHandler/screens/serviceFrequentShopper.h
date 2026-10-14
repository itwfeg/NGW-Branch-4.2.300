/**
  * @file serviceFrequentShopper.h
  * @brief Allows service to modify Frequent Shopper settings.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup serviceFrequentShopperCsu
  * @{
  */

#ifndef SERVICEFREQUENTSHOPPER_H
#define SERVICEFREQUENTSHOPPER_H

#include "screenwidget.h"

class QIntValidator;

namespace Ui {
class serviceFrequentShopper;
}

/**
 * @class serviceFrequentShopper
 * @brief Allows service to modify Frequent Shopper settings.
 *
 * Settings include:
 *      Rounding Method
 *      Set up Frequent Shopper (i.e. Per scale or Per Product)
 *      Default Discount Type
 *      Default Discount Formula
 */

class serviceFrequentShopper : public ScreenWidget
{
    Q_OBJECT

    typedef serviceFrequentShopper _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit serviceFrequentShopper(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceFrequentShopper();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public slots:

    /**
     * Slot for when a dropdown is changed.
     * Sets property indicating that the setup needs to be saved
     * when exiting.
     */
    void SetupChanged();

    /**
     * Slot for when the Default Discount Type dropdown is changed.
     * This enables/disables the Default Discount Formula based
     * on the Default Discount Type.
     */
    void CheckDiscountFormula(int index);

    // HT-155
    /**
     * Slot for when the Enable Global Percent checkbox is changed.
     * This enables/disables the text field for the Global Percentage
     * Off and the combo boxes for Rounding Weight Precision, Setup
     * Frequent Shopper, Default Discount Type and Default Disount
     * Formula.
     */
    void checkboxChangedEnableGlobalDiscount(int state);

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


private:

    Ui::serviceFrequentShopper *ui;

    bool bSetupChanged;

    // Save values from initial data request.
    // Not used in the screen but are needed when the screen is saved.
    QString totalPriceText1;
    QString totalPriceText2;
    QString savingsText1;
    QString savingsText2;
    QString unitPriceText1;
    QString unitPriceText2;

    /**
     * Populate the dropdowns with the correct text and values.
     *
     */
    void initialize();

    /**
     * Builds the parameter string and saves the setup.
     *
     */
    void SaveDataOnExit();

    // HT-155
    QIntValidator *txtGlobalPercentageOffValidator_;
    int makeValidatorTop( int digits );

    // HT-213
    void setRoundingWeightPrecision();

};

#endif // SERVICEFREQUENTSHOPPER_H
