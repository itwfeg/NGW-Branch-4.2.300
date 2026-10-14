/**
 * @file serviceNewWrapperOtherTimings.h
 * @Super Cool Create Categories allows operators to create category strings for cool
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup SuperCoolCreateCategoriesCsu
 * @{
 */

#ifndef SERVICENEWWRAPPEROTHERTIMINGS
#define SERVICENEWWRAPPEROTHERTIMINGS

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
    class serviceNewWrapperOtherTimings;
}

/**
* @class serviceNewWrapperOtherTimings
* @brief  
*  
*/

class serviceNewWrapperOtherTimings  : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperOtherTimings _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
      */
    explicit serviceNewWrapperOtherTimings(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceNewWrapperOtherTimings();

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
    * SLOT method called when the 'Advanced timings' button has been clicked
    *
    * @param[in] none
    * @returns  none
    */
    void goToAdvancedTimings();

    /**
    * SLOT method called when the 'Basic timings' button has been clicked
    *
    * @param[in] none
    * @returns  none
    */
    void goToBasicTimings();

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
    * SLOT method called when the Height box list changes its selection.
    *
    * @param[in] none
    * @returns  none
    */
    void requestHeightBox();

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

	void buildWrapDimensions(const SessionMessage& msg);
	void buildHeightCombo();
	void setupHeightBox(const SessionMessage& msg);
	void selectHeight();
	void buildParamTable();
	void setupHeightBoxParams();
    QSpinBox* createQSpinBox(int offset, int minVal, int maxVal, QString currVal);
	int validateField(int valWas, int valIs, int MinValue, int MaxValue);
    void validateEntry(int offset, QSpinBox *intField);
	void updateWrapperPkgDescription(const SessionMessage& msg);
	void writeWrapBox();
	void writeHeightBox();
	void wrapperCycleComplete(const SessionMessage& msg);
    bool isNGWWrapper() const;

    Ui::serviceNewWrapperOtherTimings *ui;
	WrapperTimingsData *wrTimingsData_;
    QIntValidator *txtRecordNumValidator_;                          // enforce limits on the plu number
    DomWrapBoxParams *wrapBoxParams_;
	DomHeightBox *heightBox_;

    QComboBox *cbo_FOEState_;
    QComboBox *cbo_FilmRoll_;
    QSpinBox *le_HeightRestrict_;
    QSpinBox *le_SideClampGap_;

    QSignalMapper *widgetUpdatedSignalMapper_;
    QMap<QWidget*, int> offsetMap_;

	bool firstPass;													// First time to this page.
    int activeRow;                                                  // the current row selected
    int numOfItems;                                                 // how many are there?
    int boxCount;                                                   // how many Height boxes do we have for this wrap box?

	int filmRollRow;

	bool viewCurrentTimings_;
	bool saveAnyChanges_;

	QString	heightBoxSelected_;
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

#endif // SERVICENEWWRAPPEROTHERTIMINGS
