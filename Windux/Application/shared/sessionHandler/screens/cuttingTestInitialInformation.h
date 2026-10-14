/**
 * @file cuttingTestInitialInformation.h
 * Screen for starting a cutting test.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Job E. Roni
 * @inspiredby Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef CUTTINGTESTINITIALINFORMATION_H
#define CUTTINGTESTINITIALINFORMATION_H

#include "screenwidget.h"

#include <Session.h>
#include <Messages.h>

class OperateMode;
class DomCurrentPLU;

namespace Ui {
class CuttingTestInitialInformation;
}

class QLabel;
class QLineEdit;
class QPushButton;
class QDateTimeEdit;
class QPushButton;
class QLineEdit;
class QDateTime;
class QIntValidator;
class QValidator;
class QWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Screen for starting a cutting test.
class CuttingTestInitialInformation : public ScreenWidget
{
    Q_OBJECT

    typedef CuttingTestInitialInformation _Myt;
    typedef ScreenWidget _Mybase;

public:

   // ========================================================================
   // CTOR / DTOR
   // ========================================================================

   /// Constructor.
   /**
    * @param[in] mainWindow  the @c SessionMainWindow object
    * @param[in] parent  parent @c ModeWidget widget
    */
   explicit CuttingTestInitialInformation(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

   /// Destructor.
   virtual ~CuttingTestInitialInformation();

   // ========================================================================
   // Properties
   // ========================================================================

   // ========================================================================
   // Methods
   // ========================================================================

    /**
     * Translate the user interface to current language.
     */
   virtual void translate();

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
    * updateInformation
    *
    * @param[in] testnum    - the number of the current test
    * @param[in] beginOrEnd - indicates if we are beginning and ending a cutting test
    * @returns none
    */
    virtual void updateInformation(int testnum, int testState);

    /**
    * When a user is logged out of a cut test due to inactivity
    *
    * @param[in] none
    * @returns none
    */
    virtual void inactivityLogout();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  resize event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /// Handle screen changing.
    /**
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

private Q_SLOTS:

    /**
     * SLOT when the screen changes to another screen like when we leave this one.
     *
     * @param[in] none
     * @returns none
     */
    void onScreenChanged();

    /**
     * SLOT method to pop the keyboard on field click.
     *
     * @param[in] none
     * @returns none
     */
    void onPackingDateClicked();

    /**
     * Slot method for Start button clicked.
     *
     * @param[in] none
     * @returns none
     */
    void onStartButtonClicked();

    /**
     * Slot method for Suspend button clicked.
     *
     * @param[in] none
     * @returns none
     */
    void onSuspendButtonClicked();

    /**
     * Slot method for Commment button clicked.
     *
     * @param[in] none
     * @returns none
     */
    void onCommentButtonClicked();

    /**
     * Slot method for Cancel the Cut Test button clicked.
     *
     * @param[in] none
     * @returns none
     */
    void onCancelCutTestClicked();

    /**
    * SLOT method called when a Calendar date is clicked.
    *
    * @param[in] newDate - the new date selected for the packing date
    * @returns  none
    */
    void onCalendarClicked(QDate newDate);

    /**
    * SLOT method Product Number field is finished editing.
    *
    * @param[in] none
    * @returns  none
    */
    void onProductNumberFinished();

    /**
    * SLOT method called when the Cancel button is selected. Returns the user back to the
    * Start a Cut Test dialog and discards any entered comments
    *
    * @param[in] none
    * @returns  none
    */
    void onCutTestCancelComments();

    /**
    * SLOT method called when the Return To Cut Test button is selected. Returns the user back to the
    * Main Run Screen of a Cut Test and allows the user to continue the current cut test.
    *
    * @param[in] none
    * @returns  none
    */
    void onReturnToCutTest();

    /**
    * SLOT method called when the Save button is selected on the initial comments screen.
    * Returns the initial comments to be saved later.
    *
    * @param[in] none
    * @returns  comments - initial comments
    */
    void onCutTestSaveComments();

    /**
    * SLOT method called a user types a PLU number in the product field and
    * this method will request that PLU and verify its existance.
    * The user will see an error if the PLU does not exist.
    *
    * @param[in] none
    * @returns  none
    */
    void onLookupPluNumber();

    /**
    * SLOT method called a product is returned by the application
    *
    * @param[in] nof - Product not on file
    * @returns  none
    */
    void onCurrentProductChanged(bool nof);

private:

   Ui::CuttingTestInitialInformation *ui;

   OperateMode *operateMode_;

   DomOperatorLoginData operatorLoginData_;                        ///< allows access to the operator login info

   int plu_num;                                                    ///< the entered product number
   int testnum_;                                                   ///< current cutting test number
   int beginOrEndTest_;                                            ///< 1= begin test, 2=end test

   bool isPluValid_;                                               ///< true or false value indicating if the entered plu exists in the database
   bool startedTest_;                                              ///< used in case we click off of the dialog and didnt start a test
   bool suspendedTest_;                                            ///< used when we suspend a test so we can exit
   bool ignoreSignals_;
   bool testActive_;
   bool canceledTest_;                                             ///< if the user clicked to cancel the test
   bool continueTest_;                                             ///< user decided to continue the test
   bool imOuttaHere;                                               ///< If leaving hitting the home button.
   QString comments_;                                              ///< initial comments for the test
   QString finalcomments_;                                         ///< final comments about a cut test
   QString saveddate_;                                             ///< saved date sent to APP which is in a different format than displayed
   QString displaydate_;                                           ///< the date in the format we want to display it
   QString displayhour_;                                           ///< the currnet hour
   QString displaymins_;                                           ///< the current minutes

   QIntValidator *prodNumValidator_;                               ///< field validator to the prod num
   QIntValidator *prodCostValidator_;                              ///< field validator to the prod cost
   QIntValidator *prodWtValidator_;                                ///< field validator to the prod weight
   QIntValidator *packingHoursValidator_;                          ///< field validator to the hours
   QIntValidator *packingMinsValidator_;                           ///< field validator to the mins
   QValidator    *supplierValidator_;                              ///< field validator to the supplier
   QValidator    *lotNumValidator_;                                ///< field validator to the lot number

   /**
   * Initialize
   *
   * @param[in] none
   * @returns none
   */
   void initialize();

   /**
    * When we pop this dialog its because we started or ended a cut test. If we are starting
    * a cut test we want to display the data entry part of this dialog. If its because we
    * ended a test, we want to display the comments part so we can enter final comments
    *
    * @param[in] none
    * @returns none
    */
   void displayInitialState();

   /**
    * When we pop this dialog its because we ended a cut test, maybe. On this screen the
    * user has an option of continuing the current test and they got to this screen by
    * accident. Now we wont end a test if the user clicks continue test and only end it
    * when the user clicks Cancel or Save Comments.
    *
    * @param[in] none
    * @returns none
    */
   void endCurrentCuttingTest();

   /**
    * Retrieve current product.
    */
   DomCurrentPLU *currentProduct() const;

   /**
   * Validate the data in all of the fields in the dialog
   *
   * @param[in] none
   * @returns none
   */
   bool validData();

   /**
    * Sets the top number of digits in the string being masked and validated
    *
    * @param[in] the number of current character max
    * @returns The formatted date dd/mm/yyyy hh:mm
    */
   int makeValidatorTop(int digits);

   /**
    * Method to detail regression and impact
    *
    * @param[in] none
    * @returns none
    */
   void doxygenImpactAndRegressionTestingComments();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // CUTTINGTESTINITIALINFORMATION_H
