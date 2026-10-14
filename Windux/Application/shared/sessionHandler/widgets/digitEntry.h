/**
  * @file digitEntry.h
  * @brief Widget to allow editing of the EAN string.
  *
  * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
  * @author Job E. Crew
  * @addtogroup digitEntryCsu
  * @{
  */

#ifndef DIGITENTRY_H
#define DIGITENTRY_H

#include <QWidget>

#include "../screens/screenwidget.h"

class SessionMainWindow;
class QPushButton;

namespace Ui {
class digitEntry;
}

/// Hobart namespace.
namespace hobart {

/**
 * @class digitEntry
 * @brief Widget to allow the ean string to be created and modified.
 */

class digitEntry : public QWidget
{
    Q_OBJECT
    
    typedef digitEntry _Myt;
    typedef QWidget _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] parent  parent @c ModeWidget widget
     * @param[in] mainWindow  the @c SessionMainWindow object
     */
    explicit digitEntry(QWidget *parent, SessionMainWindow *mainWindow );

    /**
     * Destructor.
     */
    virtual ~digitEntry();

signals:
    
    /**
     * SIGNAL broadcast to anyone listening when a number button is pressed.
     *
     * @param[in] QString is the number digit clicked
     */
    void digitEntryNumberClicked(QString);

    /**
     * SIGNAL broadcast to anyone listening when the clear button is pressed.
     *
     * @param[in] none
     */
    void digitEntryClearClicked();

    /**
     * SIGNAL broadcast to anyone listening when the done button is pressed.
     * Need to have the ability to have custom close down methods
     *
     * @param[in] none
     */
    void digitEntryDoneClicked();

public slots:

    /**
     * SLOT methods called when one of the digits 0-9 are pressed.
     * Eventually emits a signal to the calling form.
     *
     * @param[in] none
     * @returns  none
     */
    void updateEntering_0();

    /**
     * SLOT methods called when one of the digits 0-9 are pressed.
     * Eventually emits a signal to the calling form.
     *
     * @param[in] none
     * @returns  none
     */
    void updateEntering_1();

    /**
     * SLOT methods called when one of the digits 0-9 are pressed.
     * Eventually emits a signal to the calling form.
     *
     * @param[in] none
     * @returns  none
     */
    void updateEntering_2();

    /**
     * SLOT methods called when one of the digits 0-9 are pressed.
     * Eventually emits a signal to the calling form.
     *
     * @param[in] none
     * @returns  none
     */
    void updateEntering_3();

    /**
     * SLOT methods called when one of the digits 0-9 are pressed.
     * Eventually emits a signal to the calling form.
     *
     * @param[in] none
     * @returns  none
     */
    void updateEntering_4();

    /**
     * SLOT methods called when one of the digits 0-9 are pressed.
     * Eventually emits a signal to the calling form.
     *
     * @param[in] none
     * @returns  none
     */void updateEntering_5();
    void updateEntering_6();

    /**
     * SLOT methods called when one of the digits 0-9 are pressed.
     * Eventually emits a signal to the calling form.
     *
     * @param[in] none
     * @returns  none
     */
    void updateEntering_7();

    /**
     * SLOT methods called when one of the digits 0-9 are pressed.
     * Eventually emits a signal to the calling form.
     *
     * @param[in] none
     * @returns  none
     */
    void updateEntering_8();

    /**
     * SLOT methods called when one of the digits 0-9 are pressed.
     * Eventually emits a signal to the calling form.
     *
     * @param[in] none
     * @returns  none
     */
    void updateEntering_9();

    /**
     * SLOT method called when clear is selected. The button
     * is processed and emitted to anyone listening for it.
     *
     * @param[in] none
     * @returns  none
     */
    void updateEnteringClear();

    /**
      * SLOT method called when the Done button is selected. Returns the user back to the
      * form that initially launched the keypad
      *
      * @param[in] none
      * @returns  none
      */
    void updateDoneEnteringDigits();



//protected:
// nothing in here right now

private:
    Ui::digitEntry *ui;

    SessionMainWindow *mainWindow_;



    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * SLOT method called when the one of the 10 numbers are selected. The number
     * is then processed and emitted to anyone listening for it.
     *
     * @param[in] *dititButton  *dititButton is the ui button passed into the signal mapping method
     * @returns  none
     */
    void updateEnteringDigits(QString digits);

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // DIGITENTRY_H
