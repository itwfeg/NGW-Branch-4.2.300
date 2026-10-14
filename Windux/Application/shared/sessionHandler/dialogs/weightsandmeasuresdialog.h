/**
  * @file weightsandmeasuresdialog.h
  * @brief Dialog for Weights and Measures information.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup weightsandmeasuresdialogCsu
  * @{
  */


#ifndef WEIGHTSANDMEASURESDIALOG_H
#define WEIGHTSANDMEASURESDIALOG_H

#include <Messages.h>
#include "../sessiondialog.h"

#include <QStringList>

namespace Ui {
class weightsandmeasuresdialog;
}

/**
 * @class weightsandmeasuresdialog
 * @brief Dialog for Weights and Measures information.
 *
 * This dialog contains information on the scale and its certification.
 */
class weightsandmeasuresdialog : public SessionDialog
{
    Q_OBJECT

    typedef weightsandmeasuresdialog _Myt;
    typedef SessionDialog _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     * @param[in] blocking  @c true to block session dialogs, @c false otherwise
     */
    explicit weightsandmeasuresdialog( QWidget *parent = 0, bool blocking = true );

    /**
     * Destructor.
     */
    virtual ~weightsandmeasuresdialog();

    /**
     * Overridding the sizeHint() for this specific UI's characteristics (h/w)
     * @return QSize
     */
    QSize sizeHint() const;

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

    /**
     * Retrieve all data fields.
     *
     * @returns  list of fields
     */
    virtual QStringList allDataFields() const;

protected:

    /// Use default renderer.
    /**
     * @param[in] value  @c true to use default, @c false otherwise
     */
    virtual bool useDefaultRenderer() const {return false;}

    /**
     * Slot to allow the widget to handle processing of incoming session messages.
     *
     * @param[in] msg  session message
     */
    void onSessionMessageReceive( const SessionMessage& msg );

private Q_SLOTS:

    // ========================================================================
    // Slots
    // ========================================================================

   /**
    * SLOT when the OK button is clicked.
    *
    * @param[in] none
    * @returns none
    */
   void onOkButtonClicked();

   /**
    * SLOT when Audit Log button is clicked.
    *
    * @param[in] none
    * @returns none
    */
   void onAuditTrailLogButtonClicked();

private:

    Ui::weightsandmeasuresdialog *ui;

    bool blockSessionDialogs_;

    /**
     * Send message to turn Continuous Date and Time back on.
     *
     */
    void ClearSettings();

    /**
     * Sets the Weigher Model Name based on the Weigher Model.
     *
     */
    QString SetWeigherModelName(int WeigherModel);

    /**
     * Sets the U.S. Approval based on the Weigher Model.
     *
     */
    QString Set_US_Approval(int WeigherModel);

    /**
     * Sets the Canadian Approval based on the Weigher Model.
     *
     */
    QString Set_CAN_Approval(int WeigherModel);

    /**
     * Sets the text for the Temperature Range.
     *
     */
    void SetTemperatureRange();

    /**
     * updates the version info in own method
     *
     */
    void updateVersionInfo(int updateInfo);
};

#endif // WEIGHTSANDMEASURESDIALOG_H
