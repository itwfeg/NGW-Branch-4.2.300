/**
  * @file serviceWeigherConfig.h
  * @brief Allows the Weigher to be configured.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup serviceWeigherConfigCsu
  * @{
  */

#ifndef SERVICEWEIGHERCONFIG_H
#define SERVICEWEIGHERCONFIG_H

#include "screenwidget.h"

class QIntValidator;
class DomNormalScaleOperation;

namespace Ui {
class serviceWeigherConfig;
}

/**
 * @class serviceWeigherConfig
 * @brief Allows the Weigher to be configured.
 */

class serviceWeigherConfig : public ScreenWidget
{
    Q_OBJECT

    typedef serviceWeigherConfig _Myt;
    typedef ScreenWidget _Mybase;
    
public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit serviceWeigherConfig(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceWeigherConfig();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public slots:

    /**
     * Slot for when a field on the screen is changed.checkbox is changed.
     * Sets property indicating that the setup needs to be saved when exiting.
     */
    void SetupChanged();

    /**
     * Slot for when the division size changes on the screen
     * Sets property indicating that the setup needs to be saved when exiting.
     */
    void DivisionSizeChanged();

    /**
     * Slot for when a Maximum Weight dropdown is changed.
     * Sets property indicating that the setup needs to be saved when exiting.
     * Also rebuilds Division Size dropdown, toggles lbs/kgs.
     */
    void MaximumWeightChanged();

    /**
     * Slot for when a Weigher Model dropdown is changed.
     * Sets property indicating that the setup needs to be saved when exiting.
     * Also hides/shows fields on the screen as needed.
     */
    void WeigherModelChanged();

    /**
    * SLOT method called when the Value Max checkbox is selected or deselected.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedValueMax(int state);


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
    Ui::serviceWeigherConfig *ui;

    enum MaximumWeightCapacity
    {
        capNone = 0,
        cap30lb,
        cap50lb,
        cap300lb,
        cap15kg,
        cap25kg,
        cap150kg
    };

    enum DivisionSizeFactor
    {
        div0_005_lb = 0,
        div0_01_lb,
        div0_1_lb,
        div0_002_kg,
        div0_005_kg,
        div0_05_kg
    };

    // Save initial values from message requests
    int weigherModel;
    QString weigherFilterSpeed;
    int weigherScaleFactor; // Used for Division Size
    QString weigherMaximumWt;
    QString weigherMininumWt;
    QString netWeightDRLimitCheck;
    bool valuemaxIsEnabled;

    QString weigherDualRangeLimit;
    QString weigherIsScaleValuemax;
    QString weigherGcdLanguage;

    bool bWeigherSwitchPressed;

    bool bSetupChanged;

    bool bReloadScreen;

    bool hasSystemTestKey;

    QIntValidator *minimumWeightToPrintValidator_;

    bool doNotSaveWhileUpdate_;

    DomNormalScaleOperation *normalScaleOperation;

    /**
     * Used with numericLineEdit widget to validate Minimum Weight to Print.
     */
    int makeValidatorTop(int digits);

    /**
     * Builds the parameter string and saves the setup.
     *
     */
    void SaveWeigherConfig();

    /**
     * Builds the parameter string and saves the normal scale operation weigher values.
     *
     */
    void SaveWeigherNormalScaleConfig();

    /**
     * Populate dropdowns based on the Scale Model and Weigher Model.
     *
     */
    void initialize();

    /**
     * Reloads the screen when Weigher Model set to None.
     *
     */
    void ReloadScreen();

    /**
     * Populate the Division Size dropdown.
     *
     */
    void PopulateDivisionSize();

    /**
     * Populate the Maximum Weight dropdown.
     *
     */
    void PopulateMaximumWeight();

    /**
     * Set up the validator for the mininum weight to print text field.
     *
     */
    void SetupMinWtValidator();

    /**
     * Set the units on the Minimum Weight to Print text box.
     *
     */
    void SetUnits();

    /**
     * Enables all fields after the Weigher Switch is pressed.
     *
     */
    void EnableAllFields();

    /**
     * Disable all fields.
     *
     */
    void DisableAllFields();

    /**
     * Hides all fields.  Used when there is no Weigher Model.
     *
     */
    void HideAllFields();

    /**
     * Shows all fields.  Used when there is a Weigher Model selected.
     *
     */
    void ShowAllFields();

    /**
     * Called when Weigher switch is pressed.  Update local variable and variable in DOM.
     *
     */
    void setSwitchToPressed();

    /**
    * Determines if Weigher switch has previously been pressed.
    *
    */
    bool wasWeigherSwitchPressed();

    /**
     * Updates the validators based on dual-range vs non-dual range weighers
     *
     */
    void setMinWeightToPrintValidator();

    /**
     * SOF-4090: Determines whether to show or hide the "Enable Value Max" field
     *
     */
    void showHideEnableValueMax();
};

#endif // SERVICEWEIGHERCONFIG_H
