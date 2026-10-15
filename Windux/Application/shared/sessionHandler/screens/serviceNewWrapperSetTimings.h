/**
 * @file serviceNewWrapperSetTimings.h
 * @Super Cool Create Categories allows operators to create category strings for cool
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup SuperCoolCreateCategoriesCsu
 * @{
 */

#ifndef SERVICENEWWRAPPERSETTIMINGS
#define SERVICENEWWRAPPERSETTIMINGS

#include "screenwidget.h"

class DomNewWrapperAllTrays;
class WrapperTimingsData;
class QIntValidator;
class QTableWidgetItem;
class MadpInternalConfig;
class QTimer;

namespace Ui {
    class serviceNewWrapperSetTimings;
}

/**
* @class serviceNewWrapperSetTimings
* @brief  
*  
*/

class serviceNewWrapperSetTimings  : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperSetTimings _Myt;
    typedef ScreenWidget _Mybase;
    
public:
    /**
      * Constructor.
      */
    explicit serviceNewWrapperSetTimings(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceNewWrapperSetTimings();

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
    * SLOT method called when a row in the table is clicked
    *
    * @param[in] row  row is the row of the cell selected.
    * @param[in] col  col is the column of the cell selected. Col is currently not being used but reqired by the SIGNAL.
    * @returns  none
    */
    void clickRow(int row, int col);

    /**
    * SLOT method called when a record has been selected, and then the Delete button has been clicked
    *
    * @param[in] none
    * @returns  none
    */
//    void deleteRecord();
	void viewEditTray();
	void deleteTray();
	void viewEditOneParamForAll();
	void pluExceptionTray();

    /**
    * SLOT method called when the Cancel button is clicked
    *
    * @returns  none
    */
//    void cancelChanges();

	void autoConfigureTray();
	void autoDetectTray();
	void moveUp();
	void moveDown();
	void toggleViewDefaultSettings();
	void resetToFactoryDefaults();

    /**
    * SLOT method called when the <ENTER> key is pressed after entering a number in the number field
    *
    * @param[in] none
    * @returns  none
    */
//    void readRecordFromElement();

    /**
    * SLOT method called when the <ENTER> key is pressed after entering a value in the name field
    *
    * @param[in] none
    * @returns  none
    */
//    void updateHasChanged();

    // SOF-1142 / SOF-1279
    /**
    * SLOT method called when the timer for exiting the screen elapses.
    *
    * Need to delay exiting the Timings screen until the wrapper has been
    * updated with the new timings.  The wrapper was getting the command to
    * go into Operate mode before the wrapper was finished updating.
    *
    * @param[in] none
    * @returns  none
    */
    void chkForWaitMessage();

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

    // SOF-1142 / SOF-1279
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

    Ui::serviceNewWrapperSetTimings *ui;
	WrapperTimingsData *wrTimingsData_;

    QIntValidator *txtRecordNumValidator_;                          // enforce limits on the plu number

    QStringList canChangeArray;

    bool requestingData;                                            // so we dont overload and fat finger click
    int activeRow;                                                  // the current row selected
    int numOfItems;                                                 // how many are there?
    int numOfRows;                                                  // how many total rows do we have?

	bool viewCurrentTimings_;
	bool switchToTimingsPage_;
	bool sendDonePkgTimingsMsg_;		//	Keep track so we only send 9,230 one time.
	bool trayDeleted_;					//	Mark this when we delete a tray.
    bool isExitingTimings_;

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
//    * Method to validate fields
    *
    * @param[in] digits  digits is current digits in the text input line.
    * @returns digits  where digits is the new number that has been rolled over due to being over length
    */
//	void buildTrayList(const SessionMessage& msg);
    void buildTrayList();

    /**
     * Method to clear all input fields and the Cool Table.
     *
     * @param[in] none
     * @returns  none
     */
    void clearTheTrayList();

    /**
     * Method to a currently selected record from the table in the description and number fields.
     *
     * @param[in] none
     * @returns  none
     */
	void setActiveRow(int row);
	QString getCurrentTrayName();
	QString getCurrentTrayPLUException();
	void prepareTimingsPageSwitch();

	void move(bool up);
	void checkMoveAvailability();
	QList<QTableWidgetItem*> takeRow(int row);
	void setRow(int row, const QList<QTableWidgetItem*>& rowItems);

	void setupWrapBoxParameters();

	void processReturnedWrapperErrors(const SessionMessage& msg);
	void continueResetToFactoryDefaults();
	void continueRemoveTray();

	void currentOrDefaultSettings();
    void disableForViewDefault();
    void enableForViewCurrent();

    // SOF-1142 / SOF-1279
    MadpInternalConfig *internalConfigData_;
    QTimer *startScreenExitTimer;
    bool okToCheckForWaitMessage;

    void sleepFor( int milliseconds ) const;
};

#endif // SERVICENEWWRAPPERSETTIMINGS
