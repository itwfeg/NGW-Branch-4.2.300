/**
 * @file cuttingtestdialog.h
 * Dialog for starting a cutting test.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef CUTTINGTESTDIALOG_H
#define CUTTINGTESTDIALOG_H

#include "../sessiondialog.h"

namespace hobart {
class numericLineEdit;
}

class QLabel;
class QPushButton;
class QSignalMapper;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog for starting a cutting test.
class CuttingTestDialog : public SessionDialog
{
    Q_OBJECT

    typedef CuttingTestDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] num  test number
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit CuttingTestDialog( int num, QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~CuttingTestDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const;

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Show event.
    /**
     * @param[in,out] event  show event
     */
    virtual void showEvent( QShowEvent *event );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve renderer appearance.
    /**
     * @param[in] useLight  @c true to use light instead of dark background
     * @return  appearance object
     */
    virtual hobart::widgetRendererAppearanceBase *rendererAppearance() const;

    /// Retrieve renderer style.
    /**
     * @param[in] useLight  @c true to use light instead of dark background
     * @return  style
     */
    virtual int rendererStyle() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Event filter.
    /**
     * @param obj[in,out]  object being filtered
     * @param event[in,out]  event
     * @return  @c true to filter event, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *event );

private Q_SLOTS:

    /// Slot for start button clicked.
    void onStartButtonClicked();

    /// Slot for timer timeout.
    void onTimerTimeout();

private:

    int num_;

    QTimer *timer_;

    QLabel *testNumber_;

    QLabel *productNumberLabel_;
    hobart::numericLineEdit *productNumber_;

    QLabel *vendorNumberLabel_;
    hobart::numericLineEdit *vendorNumber_;

    QLabel *operatorNumberLabel_;
    hobart::numericLineEdit *operatorNumber_;

    QLabel *productCostLabel_;
    hobart::numericLineEdit *productCost_;

    QLabel *productWeightLabel_;
    hobart::numericLineEdit *productWeight_;

    QPushButton *startButton_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // CUTTINGTESTDIALOG_H
