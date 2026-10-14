/**
 * @file selfserveAuditLogG2.h
 * Weights and measures Audit Log screen for self service.
 *
 * @copyright Copyright (c) 2023 Hobart Corporation
 * @author Job E. Crew
 *
 * @addtogroup selfServeCsu
 * @{
 */

#ifndef SELFSERVEAUDITLOGG2_H
#define SELFSERVEAUDITLOGG2_H

#include <QSignalMapper>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <Messages.h>
#include <QTimer>
#include <QIntValidator>
#include <numericlineedit.h>

namespace Ui {
class selfserveAuditLogG2;
}

class Session;
class selfServiceHook;
class ScaleConfiguration;
class QTableWidget;

typedef QVector<QString> SessionMessage;

/// Hobart namespace.
namespace hobart {

class pushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Weights and measures Audit Log screen for self service
class selfserveAuditLogG2 : public QWidget
{
    Q_OBJECT

    typedef selfserveAuditLogG2 _Myt;
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
    selfserveAuditLogG2( selfServiceHook *h, QWidget *parent = NULL );

    /// DTOR
    virtual ~selfserveAuditLogG2();

    // ************************************************************************
    // Methods
    // ************************************************************************

    /// Refresh widget.
    virtual void refresh();

    /// Translate the user interface to current language.
    virtual void translate();

    /// Update the connectivity background
    virtual void updateNetworkConnectivity( bool online );

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
     * Slot for timer timeout.
     */
    void onKeypadTimeout();

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

    Ui::selfserveAuditLogG2 *ui;
    selfServiceHook *hook_;
    ScaleConfiguration *scaleConfig_;

    QSignalMapper *buttonSignalMapper_;

    bool unlocked_;

    QWidget *screen_;
    QLabel *title_;
    QLabel *userinfo_;
    QLabel *exitLabel_;

    QLabel *spacer1_Label_;
    QLabel *spacer1_;

    QPushButton *exit_;
    QPushButton *quit_;
    QPushButton *rezero_;
    QPushButton *enter_;
    QPushButton *backspace_;
    QPushButton *activeKey_;

    QPushButton *printbut_;
    
    hobart::numericLineEdit  *opID_;
    QIntValidator *numValidator_;

    QWidget *auditLog_;
    QTableWidget *audittable_;

    QTimer *cycleNumpadTimer_;

    /**
     * Sets the top number of digits in the string being masked and validated
     *
     * @param[in] the number of current character max
     * @returns The formatted date dd/mm/yyyy hh:mm
     */
    int makeValidatorTop(int digits);

    // not implemented
    selfserveAuditLogG2( const _Myt& rhs );

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
} // hobart namespace

/** @} */

#endif // SELFSERVEAUDITLOGG2_H
