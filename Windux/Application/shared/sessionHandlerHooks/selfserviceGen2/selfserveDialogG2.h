/**
  * @file selfserveDialogG2.h
  * @brief Widget to allow editing of the EAN string.
  *
  * @copyright Copyright (c) 2022, ITW Hobart Food Equipment Group, U.S.A.
  * @author Job E. Crew
  * @addtogroup selfserveDialogCsu
  * @{
  */

#ifndef SELFSERVEDIALOGG2_H
#define SELFSERVEDIALOGG2_H

#include <QWidget>

//#include "../screens/screenwidget.h"

class selfServiceHook;

class SessionMainWindow;
class QPixmap;
class Session;

class QPushButton;
class ScaleConfiguration;

class MadpScaleConfig;
class SessionMainWindow;
class QLabel;
class QTimer;

namespace Ui {
class selfserveDialogG2;
}

/// Hobart namespace.
namespace hobart {

/**
 * @class selfserveDialogG2
 * @brief Widget to display messages.
 */

class selfserveDialogG2 : public QWidget
{
    Q_OBJECT
    
    typedef selfserveDialogG2 _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit selfserveDialogG2(selfServiceHook *h, Session *s, QWidget *parent);

    /**
     * Destructor.
     */
    virtual ~selfserveDialogG2();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool isValid() const {return valid_;}

    /**
     * @return  @c current error code of dialog
     */
    virtual QString getErrorCode() const;

    /**
     * @return  @c current error style of dialog
     */
    virtual int getStyleCode() const;

    /**
     * If the dialog already exists then update the error code and dialog style.
     *
     * @param[in] value  error code
     */
    virtual void updateCodes(const QString& value , int style);

    /**
     * If the dialog already exists then just udpate the text.
     *
     * @param[in] value  error code
     */
    virtual void refreshText();

    /**
     * If the dialog already exists then sometimes we have to force close it depending on the error code.
     *
     * @param[in] value  error code
     */
    virtual void closeDialog(QString okCancel);

    /**
     * Get the configuration of the buttons on the dialog box
     *
     * @param[in] errorCode
     * @returns  bool true if the error code is invalid and needs to be tossed out
     */
    virtual bool invalidErrorCode( int code );

signals:

    /**
     * Method emits the selfserveDialogDone signal when the Rezero/OK button is pressed.
     *
     * @param[in] none
     * @returns  none
     */
    void dialogDone();

    /**
     * Method emits the selfserveDialogOK to signal Quantity Keypad should pop back up
     *
     * @param[in] none
     * @returns  none
     */
     void popQuantity();

private Q_SLOTS:

    /**
     * SIGNAL broadcast to anyone listening when the OK button is pressed.
     *
     * @param[in] msg  session message
     */
    void onOKExit();

    /**
     * SIGNAL broadcast to anyone listening when the Cancel button is pressed.
     *
     * @param[in] msg  session message
     */
    void onCancelExit();

    /**
     * SIGNAL broadcast to anyone listening when the Rezero button is pressed.
     *
     * @param[in] msg  session message
     */
    void onRezeroExit();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /**
     * Resize event.
     *
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /**
     * Show event.
     *
     * @param[in,out] e  event
     */
    virtual void showEvent( QShowEvent *e );

    /**
     * Paint event.
     *
     * @param[in,out] e  event
     */
    virtual void paintEvent( QPaintEvent *e );

private:

    Ui::selfserveDialogG2 *ui;
    selfServiceHook *hook_;

    //SessionMainWindow *mainWindow_;
    Session *session_;

    ScaleConfiguration *config_;
    MadpScaleConfig *scaleConfig_;

    QPixmap pixmapBg_;

    QLabel *errorTitle_;
    QLabel *errorInstructions_;

    QPushButton *ok_;
    QPushButton *cancel_;
    QPushButton *rezero_;

    QString code_;
    int style_;
    bool valid_;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Update text associated with an error code
     *
     * @param[in] none
     * @returns  none
     */
    void updateText();

    /**
     * Get the title associated with an error code
     *
     * @param[in] none
     * @returns  error code text
     */
    QString getErrorTitle();

    /**
     * Get the text associated with an error code
     *
     * @param[in] none
     * @returns  error code text
     */
    QString getErrorText();

    /**
     * Get the configuration of the buttons on the dialog box
     *
     * @param[in] none
     * @returns  none
     */
    void addButtons( int buttonstyle );

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELFSERVEDIALOGG2
