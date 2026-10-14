/**
  * @file selfserveByCount.h
  * @brief Widget for Self-Service By Count entry.
  *
  * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup selfserveByCountCsu
  * @{
  */

#ifndef SELFSERVEBYCOUNT_H
#define SELFSERVEBYCOUNT_H

#include <QWidget>

namespace Ui {
class selfserveByCount;
}

class Session;
class selfServiceHook;
class ScaleConfiguration;
class QPushButton;
class QSignalMapper;

/// Hobart namespace.
namespace hobart {

class pushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class selfserveByCount
 * @brief Widget for Self-Service By Count entry.
 */
class selfserveByCount : public QWidget
{
    Q_OBJECT

    typedef selfserveByCount _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /// Emitted when print is triggered.
    void print();

    /// Emitted when print error is triggered.
    //void printError(int err);


public:

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * Constructor.
     *
     * @param[in] h  pointer to hook
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit selfserveByCount( selfServiceHook *h, QWidget *parent = NULL );

    /**
     * Destructor.
     */
    virtual ~selfserveByCount();

    // ************************************************************************
    // Methods
    // ************************************************************************

    /**
     * Refresh widget.
     */
    virtual void refresh();

    /**
     * @brief  Update barcode only label.
     */
    virtual void renderBarcodeOnlyLabel();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

    /**
     * Reset the keypad back to the initail state
     */
    virtual void resetKeypad();

    /**
     * Reset the to blank
     */
    virtual void resetKeypadDisplay();

    /**
     * Get the stack index of the keypad
     */
    virtual int getStackState();

protected:

    // ************************************************************************
    // Events
    // ************************************************************************

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
     * Mouse press event.
     *
     * @param[in,out] e  mouse event to print a label
     */
    virtual void mousePressEvent( QMouseEvent *e );

private slots:

    /**
     * Slot for numeric input widget.
     */
    void onNumberPadTextChanged( const QString& text );

    /**
     * Slot to reload images.
     */
    void onReloadImages();

    /// Emitted when print error is triggered.
    void onPrintError(int err);

    /// Emitted when a bycount is entered.
    void onShowBackButton();

private:

    Ui::selfserveByCount *ui;

    Session *s;
    selfServiceHook *hook_;
    ScaleConfiguration *scaleConfig_;

    QSignalMapper *buttonSignalMapper_;

    QWidget *buttons_;                              // might not need this is I use the one in the UI
    hobart::pushButton *printLabel_;

    /**
     * Initialize.
     */
    void initialize();

    int checkScreenSize();
};


///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELFSERVEBYCOUNT_H
