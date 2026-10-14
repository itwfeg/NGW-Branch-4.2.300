/**
  * @file auditlogdialog.h
  * @brief Dialog for Weighter Audit Trail information.
  *
  * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
  * @author Joe Crew
  * @addtogroup auditlogdialogCsu
  * @{
  */


#ifndef AUDITLOGDIALOG_H
#define AUDITLOGDIALOG_H

#include <Messages.h>
#include "../sessiondialog.h"

#include <QStringList>

namespace Ui {
class auditlogdialog;
}

/**
 * @class auditlogdialog
 * @brief Dialog for Viewing and Saving the Weigher Audit Log information.
 *
 * This dialog contains weigher audit log information */
class auditlogdialog : public SessionDialog
{
    Q_OBJECT

    typedef auditlogdialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     * @param[in] blocking  @c true to block session dialogs, @c false otherwise
     */
    explicit auditlogdialog(QWidget *parent = 0, bool blocking = true , QString display_text = "" );

    /**
     * Destructor.
     */
    virtual ~auditlogdialog();

    /**
     * Overridding the sizeHint() for this specific UI's characteristics (h/w)
     * @return QSize
     */
    QSize sizeHint() const;

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
    virtual void onSessionMessageReceive( const SessionMessage& msg );


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

   /**
    * SLOT when the OK button is clicked.
    *
    * @param[in] none
    * @returns none
    */
   void onOkButtonClicked();


private:

    Ui::auditlogdialog *ui;

    // ========================================================================
    // Methods
    // ========================================================================

    bool blockSessionDialogs_;
    QString initialText_;

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

#endif // AUDITLOGDIALOG_H
