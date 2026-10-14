/**
  * @file serviceWeigherInfo.h
  * @brief Screen for displaying Weigher information for the weigher attached to the system.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup serviceWeigherInfoCsu
  * @{
  */

#ifndef SERVICEWEIGHERAUDITLOG_H
#define SERVICEWEIGHERAUDITLOG_H

#include <Messages.h>
#include "screenwidget.h"

namespace Ui {
class serviceWeigherAuditLog;
}

/**
 * @class serviceWeigherAuditLog
 * @brief Screen for displaying Weigher Audit Log information for the weigher attached to the system.
 */
class serviceWeigherAuditLog : public ScreenWidget
{
    Q_OBJECT

    typedef serviceWeigherAuditLog _Myt;
    typedef ScreenWidget _Mybase;
    
public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit serviceWeigherAuditLog(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceWeigherAuditLog();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  resize event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    // ========================================================================
    // Slots
    // ========================================================================

   /**
    * SLOT when the Print button is clicked.
    *
    * @param[in] none
    * @returns none
    */
   void onPrintButtonClicked();

   /**
    * SLOT when Save button is clicked.
    *
    * @param[in] none
    * @returns none
    */
   void onSaveButtonClicked();

private:
    Ui::serviceWeigherAuditLog *ui;

    // ========================================================================
    // Methods
    // ========================================================================

    /**
    * Initialize
    *
    * @param[in] none
    * @returns none
    */
    void initialize();

    /**
     * Method used to build and update Weigher Audit Log information table
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
   void updateAuditLogList(const SessionMessage &msg);

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

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SERVICEWEIGHERINFO_H
