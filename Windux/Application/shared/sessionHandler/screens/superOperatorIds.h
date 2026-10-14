/**
 * @file SuperOperatorIds.h
 * @brief Super Operator Ids allows for the configuration of scale operators. Operators can
 * be created or existing operators can be edited such as their name, access level, and 
 * preferred language.
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup SuperOperatorIdsCsu
 * @{
*/

#ifndef SUPEROPERATORIDS_H
#define SUPEROPERATORIDS_H

#include "screenwidget.h"

#include <Messages.h>

class QIntValidator;

namespace Ui {
class SuperOperatorIds;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Sets up and initializes any default fields for a product.
class SuperOperatorIds  : public ScreenWidget
{
    Q_OBJECT

    typedef SuperOperatorIds _Myt;
    typedef ScreenWidget _Mybase;
    
public:

    /// Constructor.
    /**
     * @param[in] mainWindow  main window
     * @param[in] parent  parent widget
     */
    explicit SuperOperatorIds( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~SuperOperatorIds();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle screen changing.
    /**
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     * @return  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private slots:

    /// SLOT method called when text in access level field is modified
    void checkAcccessLevel();

    /// SLOT method called when any checkbox is selected or deselected.
    /**
     * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
     */
    void checkboxChanged( int state );

    /// SLOT method called when any checkbox is selected or deselected.
    /**
     * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
     */
    void deleteOperator();

    /// SLOT method called when an operator ID is entered into the operator field.
    /**
     * Either returns the entered Operator ID, or requests if you would like to create it if it
     * doesn't exist.
     */
    void operatorIDEntered();

    /// SLOT method to highlight a row when it is selected
    /**
     * @param[in] row  row is the row of the cell selected.
     * @param[in] col  col is the column of the cell selected. Col is currently not being used but reqired by the SIGNAL.
     */
    void selectTheRow( int row, int col );

    /// SLOT method called when text in a text field is modified
    void textboxChanged();

    /// SLOT method called when a new index is selected in the Preferred Langaugecombo box.
    /**
     * @param[in] newIndex, newIndex is the new currentIndex in the combo box.
     */
    void updatePreferredLangauge( int newIndex );

private:

    enum operTableColumns
    {
        COL_ID = 0,
        COL_ACCESS,
        COL_LANG,
        COL_OPER_NAME,
        COL_TAG_ID,
        NUM_COLS
    };

    Ui::SuperOperatorIds *ui_;

    MadpScaleConfig *scaleConfigData_;
    DomSingleOperator *singleOperator_;
    DomOperatorInfomation *operatorInformation_;

    bool ignoreSignals_;                                            // when certain events occur, don't trigger SIGNALS
    bool hasChanged_;                                               // has data been modified
    bool createNew_;                                                // flag if we are creating a new one

    QString nameEmitted_;                                           // so we dont double call a method on clicking ENTER
    int currentSelectedRow_;                                        // the id of the row clicked, may or may not match the id of the row
    int numOfOperators_;                                            // well how many are there?

    QStringList ignored_;                                           // ignored tags
    QString currentTagPending_;                                     // tag pending answer

    bool handsFreeLoginEnabled_;                                    // is hands free login or RFID enabled

    QIntValidator *accessLevelValidator_;

    /// Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
    void initialize();

    /// Method to disable, clear, and reset input fields for an operator
    void clearOperatorFields();

    /// Method to update all of the scale operators information from editing existing to creating new ones.
    void requestToCreateNewOperator( int buttonAction );

    /// Method to insert the newly created ID at the top of the list
    /**
     * @param[in] opId  opId is the new operator to be created
     * @param[in] opRFID  operator tag
     */
    void insertOperatorRow( const QString& opId, const QString& opRFID = QString() );

    /// Method called by deleteOperator() to physically remove an item from the table.
    /**
     * @param[in] opId  opId is the operator to be removed
     */
    bool removeOperatorRow( const QString& opId );

    /// Method to save the currently selected operator.
    void saveCurrentOperator();

    /// Method to update a single operator request, or create a new one.
    /**
     * The single operator can represent a new operator or one selected from the list of operators
     * in the table.
     * @param[in] msg  msg is the data received from the Scale Application.
     */
    void updateSingleOperator( const DomSingleOperator& msg );

    /// Method to update all of the scale operators information from editing existing to creating new ones.
    /**
     * @param[in] msg  msg is the data received from the Scale Application.
     */
    void updateOperatorInformation( const DomOperatorInfomation& msg );

    /// Method to return the full language name based upon the selection selected in the language preference combo box.
    /**
     * @param[in] langNum  langNum is 1=primary language and 2=secondary langauge, whatever language that may be at the time.
     * @return langName  langName is the full name of the language when either primary or seconday languge is selected in the combo.
     */
    QString getLanguageText( int langNum ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SUPEROPERATORIDS
