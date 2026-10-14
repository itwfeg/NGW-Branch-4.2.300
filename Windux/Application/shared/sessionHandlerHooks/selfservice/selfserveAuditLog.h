/**
 * @file selfserveAuditLog.h
 * Weights and measures Audit Log screen for self service.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Job E. Crew
 *
 * @addtogroup selfServeCsu
 * @{
 */

#ifndef SELFSERVEAUDITLOG_H
#define SELFSERVEAUDITLOG_H

#include <QSignalMapper>
#include <QWidget>
#include <Messages.h>

class Session;
class selfServiceHook;
class selfServiceWmNumberPad;
class ScaleConfiguration;

class QLabel;
class QTableWidget;

typedef QVector<QString> SessionMessage;

/// Hobart namespace.
namespace hobart {

class pushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Weights and measures Audit Log screen for self service
class selfserveAuditLog : public QWidget
{
    Q_OBJECT

    typedef selfserveAuditLog _Myt;
    typedef QWidget _Mybase;

public:

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * @brief CTOR.
     * @param[in] h  pointer to hook
     * @param[in] parent  parent widget
     */
    selfserveAuditLog( selfServiceHook *h, QWidget *parent = NULL );

    /// DTOR
    virtual ~selfserveAuditLog();

    // ************************************************************************
    // Methods
    // ************************************************************************

    /// Refresh widget.
    virtual void refresh();

    /// Translate the user interface to current language.
    virtual void translate();

protected:

    // ************************************************************************
    // Events
    // ************************************************************************

    /**
     * @brief  Resize event,
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /**
     * @brief  Show event.
     * @param[in,out] e  event
     */
    virtual void showEvent( QShowEvent *e );

    // ************************************************************************
    // Methods
    // ************************************************************************

    /**
     * @brief  Event filter.
     * @param[in] obj  pointer to object
     * @param[in] e  pointer to event
     * @return  @c true to filter event, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *e );

private Q_SLOTS:

    /// Slot to handle button clicked.
    void onButtonClicked( QWidget *w );

    /**
     * Slot to reload images.
     */
    void onReloadImages();

    /**
     * Method used to build and update Weigher Audit Log information table
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    // could not get it to work. Would not called SLOT - revisit later
    // void onEmitUpdateAuditLogList(DomAuditLogInformation *m);
    void onEmitUpdateAuditLogList();

     /**
     * @brief updates the screen when we are finished saving the augit log file
     * @param[in]
     */
     void onEmitUpdateSaveMessage();

private:

    selfServiceHook *hook_;
    ScaleConfiguration *scaleConfig_;

    QSignalMapper *buttonSignalMapper_;

    bool unlocked_;

    QLabel *title_;
    QLabel *userinfo_;
    QLabel *exitLabel_;

    QLabel *spacer1_Label_;
    QLabel *spacer1_;

    hobart::pushButton *exit_;
    hobart::pushButton *quit_;
    hobart::pushButton *rezero_;

    hobart::pushButton *savebut_;
    hobart::pushButton *printbut_;

    QTableWidget *audittable_;

    QWidget *auditLog_;

    selfServiceWmNumberPad *numberPad_;
    hobart::pushButton *enter_;

    // not implemented
    selfserveAuditLog( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

    /**
    * initialize the screen.
    *
    * @param[in] none
    * @returns none
    */
    void initialize();

    /**
    * Create layout.
    *
    * @param[in] none
    * @returns none
    */
    void createLayout();

    /**
    * Generate exit button pixmap.
    *
    * @param[in] none
    * @returns none
    */
    static QPixmap exitButtonPixmap();

    /**
    * Calculate item table column widths
    *
    * @param[in] none
    * @returns none
    */
    void calcItemColumnWidths();

    /**
     * method called when Save Log is selected
     * @param[in] none
     * returns a good file, or not if an invalid file name or location was entered.
     */
    void onSaveButtonClicked();

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
} // hobart namespace

/** @} */

#endif // SELFSERVEAUDITLOG_H
