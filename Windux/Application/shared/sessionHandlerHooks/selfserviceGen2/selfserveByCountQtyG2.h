/**
 * @file selfserveByCountQtyG2.h
 * @brief Widget to allow editing of the By Count Qty.
 *
 * @copyright Copyright (c) 2023, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe E. Crew
 */

#ifndef SELFSERVEBYCOUNTQTYG2_H
#define SELFSERVEBYCOUNTQTYG2_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QMutex>
#include <QIntValidator>
#include <QTimer>
#include <numericlineedit.h>

class SessionMainWindow;

class QPushButton;

namespace Ui {
class selfserveByCountQtyG2;
}

class QPushButton;
class QSignalMapper;

/// Hobart namespace.
namespace hobart {



///////////////////////////////////////////////////////////////////////////////////////////////////

class selfserveByCountQtyG2 : public QWidget
{
    Q_OBJECT
    
    typedef selfserveByCountQtyG2 _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] gs1  initial GS1 string
     * @param[in] ai  @c true for AI modification, @c false otherwise
     */
    explicit selfserveByCountQtyG2(const QString &qty, QWidget *parent = NULL );

    /// Destructor.
    virtual ~selfserveByCountQtyG2();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

    /**
     * Reset the to blank
     */
    virtual void resetKeypadDisplay();

    /**
     * Resets number pad entry.
     */
    virtual void resetEntry();

signals:

    /// Broadcast to anyone listening when the Done button is pressed.
    /**
     *
     */
    void enterClicked( const QString &qty );

    /// Broadcast to anyone listening when the Cancel button is pressed.
    /**
     *
     */
    void cancelClicked();

    void printError( int err );

    void resetIdleInactive();

protected:

    // ************************************************************************
    // Events
    // ************************************************************************

    /**
     * Show event.
     *
     * @param[in,out] e  event
     */
    virtual void showEvent( QShowEvent *e );

private slots:

    /**
    * Slot to handle button clicked.
    */
    void onButtonClicked( QWidget *w );

    /**
     * Slot for numeric input widget.
     */
    void onNumberPadTextChanged( const QString& text );

    /**
    * Slot for timer timeout.
    */
    void onKeypadTimeout();

private:

    Ui::selfserveByCountQtyG2 *ui;

    SessionMainWindow *mainWindow_;

    QString bycount_;

    QSignalMapper *buttonSignalMapper_;

    // buttons for number pad
    QPushButton *digitZero_;
    QPushButton *digitOne_;
    QPushButton *digitTwo_;
    QPushButton *digitThree_;
    QPushButton *digitFour_;
    QPushButton *digitFive_;
    QPushButton *digitSix_;
    QPushButton *digitSeven_;
    QPushButton *digitEight_;
    QPushButton *digitNine_;

    QPushButton *enter_;
    QPushButton *cancel_;
    QPushButton *activeKey_;
    QPushButton *backspace_;

    hobart::numericLineEdit   *quantity_;

    QMutex m_;
    const QIntValidator *validator_;
    QIntValidator *numValidator_;
    QTimer *cycleNumpadTimer_;


    /// Sets the top number of digits in the string being masked and validated
    int makeValidatorTop(int digits);

    /// Method to initialize all variables, sets components visible or hidden, and creates all signals and slots.
    void initialize();
};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // hobart namespace

#endif // SELFSERVEBYCOUNTQTYG2_H
