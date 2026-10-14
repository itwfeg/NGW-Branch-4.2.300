/**
  * @file timeEntryDialog.h
  * @brief Dialog to enter on and off times for the wrapper sealer belt for each day of the week.
  *
  * @copyright Copyright (c) 2014, Hobart / ITW  Food Equipment Group, U.S.A.
  * @author Clare Dickey
  * @addtogroup 
  * @{
  */


#ifndef TIMEENTRYDIALOG_H
#define TIMEENTRYDIALOG_H

#include "../sessiondialog.h"

#include <QStringList>

class QIntValidator;

namespace Ui {
class timeEntryDialog;
}

/**
 * @class timeEntryDialog
 * @brief Dialog for Weights and Measures information.
 *
 * This dialog contains information on the scale and its certification.
 */
class timeEntryDialog : public SessionDialog
{
    Q_OBJECT

    typedef timeEntryDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     * @param[in] blocking  @c true to block session dialogs, @c false otherwise
     */
    explicit timeEntryDialog( const QMap<QWidget*, int>& timesList, int offset, QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~timeEntryDialog();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

    /**
     * Whether or not clicking the overlay rejects the dialog.
     *
     * @returns  @c true if allowed, @c false otherwise
     */
    virtual bool allowOverlayClickDismiss() const {return false;}		//	HA-111


private slots:

	void toggleAmPm();
	void applyTime(bool clearTime = false);
	void setTimeToWidget(int theIndex, bool isOn, bool clearTime = false);
	void applyTimeThenExit();
    void exitTimeEntry();
    void setFieldToNone();
    void setAllFieldsToNone();


protected:

    /**
     * Slot to allow the widget to handle processing of incoming session messages.
     *
     * @param[in] msg  session message
     */
    void onSessionMessageReceive( const SessionMessage& msg );

private:

    Ui::timeEntryDialog *ui;
	const QMap<QWidget*, int>& timesList_;

	int offset_;
    int dayOfWeekIndex_;
    int onOffIndex_;
	bool isAM_;
    bool blockSessionDialogs_;

    QIntValidator *valHours_;
    QIntValidator *valMinutes_;

	SessionMainWindow *mainWindow_;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

};

#endif // TIMEENTRYDIALOG_H
