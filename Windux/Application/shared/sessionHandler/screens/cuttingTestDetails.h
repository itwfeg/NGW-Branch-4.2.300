/**
* @file cuttingTestDetails.h
* Cutting test details screen.
*
* @copyright Copyright (c) 2016 Hobart Corporation
* @author Joe E. Crew
*
* @addtogroup sessionHandlerCsu
* @{
*/

#ifndef CUTTINGTESTDETAILS_H
#define CUTTINGTESTDETAILS_H

#include "screenwidget.h"

class OperateMode;

class DomCuttingTestTotals;

#include "python.h"

namespace Ui {
class CuttingTestDetails;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

/// Cutting test details screen.
class CuttingTestDetails : public ScreenWidget
{
    Q_OBJECT

    typedef CuttingTestDetails _Myt;
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
    explicit CuttingTestDetails( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~CuttingTestDetails();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set details data.
    /**
    * @param[in] value  details report data
    */
    virtual void updateReportData( const DomCuttingTestTotals& value );

    /// Set where the Details Screen returns to when Finish is pressed
    /**
    * @param[in] value  what screen to return to on Finish
    *                   1: History Screen
    *                   2: Start Screen
    */
    void setReturnHomeLocation(int homeLocation);

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
   * SLOT when Print button is clicked.
   *
   * @param[in] none
   * @returns none
   */
   void onPrintButtonClicked();

   /**
   * SLOT when Finish button is clicked.
   *
   * @param[in] none
   * @returns none
   */
   void onFinishedButtonClicked();

   /**
    * SLOT when Save button is clicked.
    *
    * @param[in] none
    * @returns none
    */
   void onSaveButtonClicked();

   /**
    * SLOT method to highlight a row when it is selected
    *
    * @param[in] row  row is the row of the cell selected.
    * @param[in] col  col is the column of the cell selected. Col is currently not being used but reqired by the SIGNAL.
    * @returns  none
    */
   void selectTheRow(int row, int col);


private:

   Ui::CuttingTestDetails *ui;
   OperateMode *operateMode_;

   DomCuttingTestTotals *tests;                 ///< cut test totals information

   python *pythonProcess;                       ///< pointer to call the python script to save to usb

   int numOfItems_;                             ///< indicates how many PLUs we have in this batch
   int reportNumber_;                           ///< what report of the 3 are we printing. One at a time on a reply of terminate/finished (116,2)
   int numOfTotalRows_;                         ///< indicates how many rows we draw.  DEFAULT_ROWS+GRAND_TOTALS or numOfItems+GRAND_TOTALS if numOfItems>DEFAULT_ROWS
   int selectedTest_;                           ///< indicates the test row selected
   int returnHomeToWhere_;                      ///< indicates what screen to return to when Finish pressed
   int currentSelectedRow_;                     ///< indicates the current Operator Changes Row selected

   int operatorAccess_;                         ///< what is the logged on operator have access to?

   QString test_num_;                           ///< the test number being displayed
   QString csvFileData;                         ///< the CSV data primal data written to a selected USB file
   QString csvFileSubData;                      ///< the CSV data about the sub primals
   /**
   * Initialize
   *
   * @param[in] none
   * @returns none
   */
   void initialize();

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
   * Calculate item table column widths
   *
   * @param[in] none
   * @returns none
   */
   void calcItemColumnWidths();

   /**
    * The method determines if we are running on LINUX or WINDOWS to set initial directories
    *
    * @param[in] none
    * @returns  @c true if system OS is a LINUX based system, @c else false which indicates Windows
    */
   static bool isLinux();
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // CUTTINGTESTDETAILS_H
