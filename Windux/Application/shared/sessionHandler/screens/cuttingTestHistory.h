/**
* @file cuttingTestHistory.h
* Cutting test history screen.
*
* @copyright Copyright (c) 2016 Hobart Corporation
* @author Joe E. Crew
*
* @addtogroup sessionHandlerCsu
* @{
*/

#ifndef CUTTINGTESTHISTORY_H
#define CUTTINGTESTHISTORY_H

#include "screenwidget.h"

class OperateMode;
class DomCurrentPLU;

class DomCuttingTestHistoryTotals;

namespace Ui {
class CuttingTestHistory;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

/// Cutting test history screen.
class CuttingTestHistory : public ScreenWidget
{
   Q_OBJECT

   typedef CuttingTestHistory _Myt;
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
   explicit CuttingTestHistory( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

   /// Destructor.
   virtual ~CuttingTestHistory();

   // ========================================================================
   // Properties
   // ========================================================================

   /// Set history data.
   /**
    * @param[in] value  history report data
    */
   virtual void updateReportData( const DomCuttingTestHistoryTotals& value );

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

protected:

    // ========================================================================
    // Events
    // ========================================================================


   /// Resize event.
   /**
    * @param[in,out] e  resize event
    */
   virtual void resizeEvent( QResizeEvent *e );

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

   /**
    * SLOT when the screen changes to another screen like when we leave this one.
    *
    * @param[in] none
    * @returns none
    */
   void onScreenChanged();

   /**
    * SLOT when Home button is clicked.
    *
    * @param[in] none
    * @returns none
    */
   void onHomeButtonClicked();

   /**
    * SLOT when Details button is clicked.
    *
    * @param[in] none
    * @returns none
    */
   void onDetailsButtonClicked();

   /**
    * SLOT when Resume button is clicked.
    *
    * @param[in] none
    * @returns none
    */
   void onResumeButtonClicked();

   /**
    * SLOT when Delete button is clicked.
    *
    * @param[in] none
    * @returns none
    */
   void onDeleteButtonClicked();

   /**
    * SLOT when Export History button is clicked.
    *
    * @param[in] none
    * @returns none
    */
   void onExportHistoryButtonClicked();

   /**
    * SLOT method to highlight a row when it is selected
    *
    * @param[in] row  row is the row of the cell selected.
    * @param[in] col  col is the column of the cell selected. Col is currently not being used but reqired by the SIGNAL.
    * @returns  none
    */
   void selectTheRow(int row, int col);

   /**
   * SLOT method called a product is returned by the application
   *
   * @param[in] nof - Product not on file
   * @returns  none
   */
   void onCurrentProductChanged(bool nof);

private:

   Ui::CuttingTestHistory *ui;
   OperateMode *operateMode_;

   int selectedTest_;                           ///< indicates the test row selected
   int selectedPlu_;                            ///< indicates the plu of the row selected
   int numOfCutTests_;                          ///< how many tests need to be displayed
   int meatSpecSheet_;                          ///< the spec sheet for the selected row item plu
   int currentSelectedRow_;                     ///< indicates the current Operator Changes Row selected

   int operatorAccess_;                         ///< what is the logged on operator have access to?
   QString operatorName_;                       ///< what is the logged on operators name?
   QString operatorNumber_;                     ///< what is the logged on operators id number?

   bool isValidResumePlu;                       ///< does the selected line have a valid PLU since we clear it later.
   bool isJumpingToInitInfo;                    ///< returning to initial information screen to complete the data

   /**
   * Initialize
   *
   * @param[in] none
   * @returns none
   */
   void initialize();

   /**
   * Determines if we have any blank data
   *
   * @param[in] none
   * @returns if we have blank data to Resume
   */
   bool isDataValid();

   /**
   * Calculate item table column widths
   *
   * @param[in] none
   * @returns none
   */
   void calcItemColumnWidths();

   /** checkFileName()
    *
    *  checkFileName checks to determine if a valid file was entered. If the filename
    *  is an empty string, or one of a number of illegal characters, a warning will
    *  be displayed to the user indicating the problem.
    *  @param[in] myfile       - the file name the user entered
    *        [in] fileOrFolder - indicates if a filename or foldername was entered
    *  @return if file was valid
    */
   bool checkFileName( QString &myfile, int fileOrFolder );

   /** getFileName()
    *
    *  getFileName determines if a filename was entered with an extension. The extension
    *  is displayed in the file type combo box, and is automatically append the extension
    *  selected by default. However, without question some user will add the extension
    *  while entering the file name itself. This method fixes any issues that may be
    *  encountered if the user adds the entension themselves.
    *  @param: myfile       - the file name the user entered
    *  @return: String - the valid file name with the extension properly appended.
    */
   QString getFileName(QString myfile);

   /**
    * The method determines if we are running on LINUX or WINDOWS to set initial directories
    *
    * @param[in] none
    * @returns  @c true if system OS is a LINUX based system, @c else false which indicates Windows
    */
   static bool isLinux();

   /**
    * Retrieve current product.
    * @param[in] none
    * @returns The current product
    */
   DomCurrentPLU *currentProduct() const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // CUTTINGTESTHISTORY_H
