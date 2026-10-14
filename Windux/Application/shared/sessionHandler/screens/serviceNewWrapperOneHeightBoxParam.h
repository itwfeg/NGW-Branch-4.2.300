/**
 * @file serviceNewWrapperOneHeightBoxParam.h
 * @Super Cool Create Categories allows operators to create category strings for cool
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup SuperCoolCreateCategoriesCsu
 * @{
 */

#ifndef SERVICENEWWRAPPERONEHEIGHTBOXPARAM
#define SERVICENEWWRAPPERONEHEIGHTBOXPARAM

#include "screenwidget.h"
#include <QSpinBox>

class DomNewWrapperAllTrays;
class MadpPeripheralList;
class WrapperTimingsData;
class DomWrapBoxParams;
class DomHeightBox;
class QIntValidator;
class QString;
class QSignalMapper;
class QComboBox;

namespace hobart {
class numericLineEdit;
}

namespace Ui {
    class serviceNewWrapperOneHeightBoxParam;
}

/**
* @class serviceNewWrapperOneHeightBoxParam
* @brief
*
*/

class serviceNewWrapperOneHeightBoxParam  : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperOneHeightBoxParam _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
      */
    explicit serviceNewWrapperOneHeightBoxParam(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceNewWrapperOneHeightBoxParam();

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

    /**
    * SLOT method called when the Cycle wrapper button has been clicked
    *
    * @param[in] none
    * @returns  none
    */
    void cycleWrapper();

    /**
    * SLOT method called when the Cancel button is clicked
    *
    * @returns  none
    */
    void cancelChanges();

    /**
    * SLOT method called when the Save changes button is clicked
    *
    * @returns  none
    */
    void saveChanges();

    /**
    * SLOT method called when the Done button is clicked
    *
    * @returns  none
    */
    void doneWithChanges();

    /**
    * SLOT method called when the Tray name list changes its selection.
    *
    * @param[in] none
    * @returns  none
    */
    void trayChanged();

    /**
    * SLOT method called when the Height box list changes its selection.
    *
    * @param[in] none
    * @returns  none
    */
    void parameterChanged();

    /**
    * SLOT method called when the Height box list changes its selection.
    *
    * @param[in] none
    * @returns  none
    */
    void onWidgetUpdated( QWidget *widget );

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

    void buildTrayList();		//	const SessionMessage &msg);
    void buildParameterList();
    void requestParameterInfo();
    void buildTable(const SessionMessage& msg);
    QComboBox* createBooleanCombo(int offset, QString value);
    QComboBox* createFilmRollToUseCombo(int offset, QString value);
    QComboBox* createForceOutsideElevatorCombo(int offset, QString value);
    QComboBox* createOutsideGripperReleaseCombo(int offset, QString value);
    QComboBox* createSideUnderfolderReturnHomeCombo(int offset, QString value);
    QSpinBox* createQSpinBox(int offset, int minVal, int maxVal, QString currVal);
    void validateSpinBox(int offset, QSpinBox* intField);
    void validateEntry(int offset, hobart::numericLineEdit *intField);
    void comboBoxChanged(int offset, QComboBox *box);
    void saveAttributeChange(int offset, int newVal);
    void updateWrapperPkgDescription(const SessionMessage& msg);
    void wrapperCycleComplete(const SessionMessage& msg);

    //NEW! For the AWS Enhanced
    bool isAWSEnhancedWrapParameter(int wrapParam) const;
    bool isUsingSimplifiedTimings() const;
    bool isDualRollWrapper() const;
    bool isNGWWrapper() const;

    Ui::serviceNewWrapperOneHeightBoxParam *ui;
    WrapperTimingsData *wrTimingsData_;
    QIntValidator *txtRecordNumValidator_;                          // enforce limits on the plu number
    DomWrapBoxParams *wrapBoxParams_;
    DomHeightBox *heightBox_;

    class AllHeightBoxes
    {
        public:
            AllHeightBoxes() : boxName(""), value(0) {}
            virtual ~AllHeightBoxes() {}

            QString boxName;
            int     value;
    };

    QList<AllHeightBoxes*> allHeightBoxes;       //  To keep track of the latest name & values for all height boxes

    QSignalMapper *widgetUpdatedSignalMapper_;
    QMap<QWidget*, int> offsetMap_;

    bool firstPass;													// First time to this page.
    int activeRow;                                                  // The current tray/wrap box selected
    int numOfItems;                                                 // How many are there?
    int boxCount;                                                   // How many Height boxes do we have for this wrap box?

    int currentParam;       //  Which height box parameter we are looking at.

    bool viewCurrentTimings_;
    bool saveAnyChanges_;

    bool cycleWrapperInProgress_;

    QString cycleButtonStyle;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method to save anything at the end that may need to be saved.
     *
     * @param[in] none
     * @returns  none
     */
    void saveDataOnExit();

    /**
     * Method to send the WrEvolDonePackageTimings when exiting the Timings pages.
     *
     * @param[in] none
     * @returns  none
     */
    void exitWrapperTimings();

    /**
    * Method to validate fields
    *
    * @param[in] digits  digits is current digits in the text input line.
    * @returns digits  where digits is the new number that has been rolled over due to being over length
    */
    int makeValidatorTop(int digits);

};

#endif // SERVICENEWWRAPPERONEHEIGHTBOXPARAM
