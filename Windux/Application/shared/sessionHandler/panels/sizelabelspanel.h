/**
 * @file voidtransactionpanel.h
 * Panel for void transaction in operate mode.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SIZELABELSPANEL_H
#define SIZELABELSPANEL_H

#include "operatemodepanelex.h"

class QHBoxLayout;
class QPushButton;
class QTreeWidget;
class QLabel;
class QGridLayout;


///////////////////////////////////////////////////////////////////////////////////////////////////

/// Panel for void transaction in operate mode.
class SizeLabelsPanel : public OperateModePanelEx
{
    Q_OBJECT

    typedef SizeLabelsPanel _Myt;
    typedef OperateModePanelEx _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c SessionDialog object
     */
    explicit SizeLabelsPanel( SessionMainWindow *mainWindow, SessionDialog *parent = 0 );

    /// Destructor.
    virtual ~SizeLabelsPanel();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot for resize Printer1 button.
    void onResizePrinter1ButtonClicked();

    /// Slot for resize Printer2 button.
    void onResizePrinter2ButtonClicked();

    /// Slot for resize Printer3 button.
    void onResizePrinter3ButtonClicked();

    /// Slot for void button.
    void onUseContinuous1ButtonClicked();

private:

    QLabel *spacer_;
    QLabel *printer1Label_;
    QLabel *printer2Label_;
    QLabel *printer3Label_;

    QLabel *printer1Size_;
    QLabel *printer2Size_;
    QLabel *printer3Size_;

    QPushButton *resizePrinter1_;
    QPushButton *resizePrinter2_;
    QPushButton *resizePrinter3_;
    QPushButton *useContinuous1_;

    QLabel *areWeSizingLabels_;

    // label sizes
    QVector<QString> labelSizes_;

    QGridLayout *theGrid_;

    /// Initialize.
    void initialize();
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SIZELABELSPANEL_H
