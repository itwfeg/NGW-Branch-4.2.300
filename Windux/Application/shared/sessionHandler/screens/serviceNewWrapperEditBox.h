/**
 * @file serviceNewWrapperEditBox.h
 * @Super Cool Create Categories allows operators to create category strings for cool
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup SuperCoolCreateCategoriesCsu
 * @{
 */

#ifndef SERVICENEWWRAPPEREDITBOX
#define SERVICENEWWRAPPEREDITBOX

#include <QWidget>
#include <QSpinBox>
#include <QTableWidget>
#include "screenwidget.h"

//wrap box parameters
#define WB_MAX_NUMBER_OF_WRAP_BOXES       34
#define WB_MIN_LENGTH           		  40	//	4.0"
#define WB_MAX_LENGTH          			 110 	//	11.0"
#define WB_MIN_WIDTH             		  40           
#define WB_MAX_WIDTH             		 160
#define WB_MIN_LIP                     	   5	//	0.5"
#define WB_MAX_LIP                        60
 
//height box min / max validation parameters
#define HB_MIN_DISTANCE                   10        // was 5
#define HB_MAX_DISTANCE                   40        // was 50
#define HB_MIN_DISTANCE_AWS1_AND_2         5
#define HB_MAX_DISTANCE_AWS1_AND_2        50

#define HB_NEG_MIN_GRIPPER_ADJ            -5
#define HB_MIN_GRIPPER_ADJ                 0        // was -60
#define HB_MAX_GRIPPER_ADJ                10        // was 60
#define HB_MIN_GRIPPER_ADJ_AWS1_AND_2    -60
#define HB_MAX_GRIPPER_ADJ_AWS1_AND_2     60

#define HB_MIN_PERCENT                     0
#define HB_MAX_PERCENT                    60        // was 100
#define HB_MAX_PERCENT_AWS1_AND_2        100                     // SOF-4434

#define HB_NEG_MIN_CLAMP_OPEN             -5
#define HB_MIN_CLAMP_OPEN                  0
#define HB_MAX_CLAMP_OPEN                 10        // was 1300
// SOF-4434
#define HB_MAX_CLAMP_OPEN_AWS1_AND_2    1230

#define HB_MIN_SIDE_FOLD_ADJ  		   	   0        // was -60
#define HB_MAX_SIDE_FOLD_ADJ  		   	  10        // was 60
#define HB_MIN_SIDE_FOLD_ADJ_AWS1_AND_2  -60
#define HB_MAX_SIDE_FOLD_ADJ_AWS1_AND_2   60

#define HB_MIN_BRAKE_TIME                  0
#define HB_MAX_BRAKE_TIME                 10        // was 9
#define HB_MAX_BRAKE_TIME_AWS1_AND_2       9

#define HB_MIN_WRAP_SPEED                 12        // was 1
#define HB_MAX_WRAP_SPEED                 30        // was 40
#define HB_MIN_WRAP_SPEED_AWS1_AND_2       1
#define HB_MAX_WRAP_SPEED_AWS1_AND_2      40

#define HB_MIN_DISCHARGE_SPEED             1
#define HB_MAX_DISCHARGE_SPEED            30        // was 40
#define HB_MAX_DISCHARGE_SPEED_AWS1_AND_2 40

#define HB_MIN_HEIGHT_RESTRICT             0
#define HB_MAX_HEIGHT_RESTRICT            10
#define HB_MAX_HEIGHT_RESTRICT_AWS1_AND_2 60

#define HB_MIN_REAR_TRIP_DELAY             0
#define HB_MAX_REAR_TRIP_DELAY             5
#define HB_MAX_REAR_TRIP_DELAY_AWS1_AND_2 1000

#define HB_MIN_TRAILEDGE_LABEL_OFFSET      0
#define HB_MAX_TRAILEDGE_LABEL_OFFSET     10

#define HB_MIN_VERTICAL_LABEL_OFFSET       0
#define HB_MAX_VERTICAL_LABEL_OFFSET      10

#define HB_MIN_LEADEDGE_LABEL_OFFSET       0
#define HB_MAX_LEADEDGE_LABEL_OFFSET      10

#define HB_MIN_HEIGHT                      5
#define HB_MAX_HEIGHT                     60
#define HB_MIN_KNIFE_UP                    0                                           
#define HB_MAX_KNIFE_UP                 1230		//was1300
#define HB_MIN_INCL_LENGTH                 0
#define HB_MAX_INCL_LENGTH                 1                            
#define HB_MIN_FRONT_ADJ                 -60
#define HB_MAX_FRONT_ADJ                  60
#define HB_MIN_HEIGHT_RESTRICT             0
#define HB_MAX_HEIGHT_RESTRICT            10
#define HB_MIN_SIDEFOLDER_HOME             0
#define HB_MAX_SIDEFOLDER_HOME            20
#define HB_MIN_GRIPPER_SHORT_PULL  	 	   0
#define HB_MAX_GRIPPER_SHORT_PULL          1
#define HB_MIN_SIDE_CLAMP_GAP            -20
#define HB_MAX_SIDE_CLAMP_GAP             30
#define HB_MIN_REAR_TRIP                   0
#define HB_MAX_REAR_TRIP                   1
#define HB_MIN_GRIPPER_RELEASE             0
#define HB_MAX_GRIPPER_RELEASE            20
#define HB_MIN_GRIPPER_RELAX               0
#define HB_MAX_GRIPPER_RELAX               1
#define HB_MIN_INSIDE_GRIPPER_RELEASE      0
#define HB_MAX_INSIDE_GRIPPER_RELEASE 	   9
#define HB_MIN_VERTICAL_ADJUSTMENT         0
#define HB_MAX_VERTICAL_ADJUSTMENT  	   9

class DomNewWrapperAllTrays;
class MadpPeripheralList;
class WrapperTimingsData;
class DomWrapBoxParams;
class DomHeightBox;
class QIntValidator;
class QString;
class WrapParameter;
class QComboBox;
class WrapParameter;
class QComboBox;

namespace hobart {
class numericLineEdit;
}

namespace Ui {
    class serviceNewWrapperEditBox;
}
/**
* @class serviceNewWrapperEditBox
* @brief  
*  
*/

class serviceNewWrapperEditBox  : public ScreenWidget
{
    Q_OBJECT

    friend class WrapBoxData;

    typedef serviceNewWrapperEditBox _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
      */
    explicit serviceNewWrapperEditBox(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceNewWrapperEditBox();

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
    * SLOT method called when the Set Min button has been clicked
    *
    * @param[in] none
    * @returns  none
    */
    void setMinValues();

    /**
    * SLOT method called when the Set Max button has been clicked
    *
    * @param[in] none
    * @returns  none
    */
    void setMaxValues();

    /**
    * SLOT method called when the Set Min & Max button has been clicked
    *
    * @param[in] none
    * @returns  none
    */
    void setMinMaxValues();

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
    void goToOtherTimings();

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
    * SLOT method called when the 'New Height box' button has been clicked
    *
    * @param[in] none
    * @returns  none
    */
    void newHeightBox();

    /**
    * SLOT method called when a new Height box has been selected from the Available list
    *
    * @param[in] none
    * @returns  none
    */
    void addNewHeightBox();

    /**
    * SLOT method called when the 'Cancel new Height box' button has been clicked
    *
    * @param[in] none
    * @returns  none
    */
    void cancelNewHeightBox();

    /**
    * SLOT method called the Delete Height button has been clicked
    *
    * @param[in] none
    * @returns  none
    */
    void deleteSelectedHeight();

    /**
    * SLOT method called when the Height box list changes its selection.
    *
    * @param[in] none
    * @returns  none
    */

    // SOF-2550
    /**
    * SLOT method called when the Cycle Wrapper with Leading Edge button has been clicked
    *
    * @param[in] none
    * @returns  none
    */
    void cycleWrapperLeadingEdge();

    // SOF-2550
    /**
    * SLOT method called when the Cycle Wrapper with Trailing Edge button has been clicked
    *
    * @param[in] none
    * @returns  none
    */
    void cycleWrapperTrailingEdge();

    void requestHeightBox();

	void pluExceptionChanged();

	void updateMinLength();
	void updateMaxLength();
	void updateMinWidth();
	void updateMaxWidth();
	void updateMaxLip();

    void autoConfigureTray();
    void updateTrayName();
    void activateTrayName();	//	HTA-79

    void cycleWrapperSetupTime();       // have to give the wrapper time to set up, make sure everyone is good to go and ready, and then cycle wrapper

    void printTimings();

    void promptForcedTraySize();

    void onWidgetUpdated();

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

    /**
     * Event filter.
     * @param[in,out] obj  object being filtered
     * @param[in,out] e  event
     * @return  @c true to filter, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *e );

private Q_SLOTS:

    void onStepDown();
    void onStepUp();

    void onWidgetUpdatedInternal( QWidget *w );

    void onCellClicked( int row, int column );

private:

	void promptPLUException();
	void buildWrapDimensions(const SessionMessage& msg);
	void buildHeightCombo(DomWrapBoxParams &wrapBoxParams);
	void setupHeightBox(const SessionMessage& msg);
	void selectHeight();
	void buildParamTable();
	void setupHeightBoxParams();
    QSpinBox* createQSpinBox(int offset, int minVal, int maxVal, QString currVal);
    QComboBox *createFilmRollToUseCombo(int offset, QString currVal );
	int validateField(int valWas, int valIs, int MinValue, int MaxValue);
    void validateEntry(int offset, QSpinBox *intField);
    int getIntFromString(QString &decimalStr);
    void updateWrapperPkgDescription(const SessionMessage& msg);
	void writeWrapBox();
	void writeHeightBox();
	void wrapperCycleComplete(const SessionMessage& msg);
    void showTablesAndButtons();
    bool isDualRollWrapper() const;
    bool isNGWWrapper() const;
	
	//NEW! For AWS Enhanced project
    void updateWrapParam(QTableWidget* table, int wrapParam, QVector<QString> &paramArray, unsigned int row);
    void addWrapParameter(int wrapParam, QTableWidget* table, int min, int max, const QColor &c = Qt::white);
    void setDefaultTableSize(QTableWidget* table);
    void buildSmallWrapParameterTable();
    bool isUsingSimplifiedTimings();

    Ui::serviceNewWrapperEditBox *ui;
	WrapperTimingsData *wrTimingsData_;
    QIntValidator *lengthValidator_;                          // enforce limits on the plu number
    QIntValidator *widthValidator_;                          // enforce limits on the plu number
    QIntValidator *lipValidator_;                          // enforce limits on the plu number
	DomHeightBox *heightBox_;
//	No longer used.	QObject *currentField_;

    QSpinBox *le_PercentStretchClampOn_;

    QMap<QWidget*, int> offsetMap_;
    QMap<QWidget*, QSpinBox*> incDecMap_;

    QTimer *timerAutoConfigTrayNameEntry_;		//	HTA-79
    QTimer *cycleWrapperTestLabel_;

	bool firstPass;													// First time to this page.
    int activeRow;                                                  // the current row selected
    int numOfItems;                                                 // how many are there?
    int boxCount;                                                   // how many Height boxes do we have for this wrap box?

	int clampOnRow;

	bool viewCurrentTimings_;
	bool saveAnyChanges_;
	bool gotNewTrayName_;

    bool cycleWrapperInProgress_;

	QString	heightBoxSelected_;
	QString cycleButtonStyle;
	QString sizeValue_;
    QString currentTrayName_;
    QString pLabelSize_;
    QString sLabelSize_;
    QVector<int> wrapParamOrdering_;

    bool leadingEdge;

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

    /**
     * Method to a process the return of an error dialog
     *
     * @param[in] none
     * @returns  none
     */
    void processReturnedWrapperErrors(const SessionMessage& msg);

    /**
     * Method to proceed with the deletion of a height box
     *
     * @param[in] none
     * @returns  none
     */
    void continueDeleteSelectedHeight();

    /**
     * Sends a message to the Wrapper to auto detect the tray type.
     *
     * @param[in] none
     * @returns  none
     */
    void autoDetectTray();

    void hideShowForNewTray(bool hideThem);


    class WrapBoxData
	{
		public:
			WrapBoxData() : minLength(0), maxLength(0),
							minWidth (0), maxWidth(0),
                            maxLip(0) { }

			virtual ~WrapBoxData() { }

            void setMinLength(int value) { minLength = value; }
            void setMaxLength(int value) { maxLength = value; }
            void setMinWidth(int value) { minWidth = value; }
            void setMaxWidth(int value) { maxWidth = value; }
            void setMaxLip(int value) { maxLip = value; }

            int getMinLength() { return minLength; }
            int getMaxLength() { return maxLength; }
            int getMinWidth() { return minWidth; }
            int getMaxWidth() { return maxWidth; }
            int getMaxLip() { return maxLip; }

		private:
            int	minLength;
			int	maxLength;
			int	minWidth;
			int	maxWidth;
			int	maxLip;
    };

    WrapBoxData wrapBoxData_;

};

#endif // SERVICENEWWRAPPEREDITBOX
