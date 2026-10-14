/**
 * @file cuttingtestreport.h
 * Cutting test report screen.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef CUTTINGTESTREPORT_H
#define CUTTINGTESTREPORT_H

#include "screenwidget.h"

class OperateMode;

class DomCuttingTestTotals;

class QLabel;
class QTreeWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Cutting test report screen.
class CuttingTestReport : public ScreenWidget
{
    Q_OBJECT

    typedef CuttingTestReport _Myt;
    typedef ScreenWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit CuttingTestReport( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~CuttingTestReport();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set report data.
    /**
     * @param[in] value  report data
     */
    virtual void setReportData( const DomCuttingTestTotals& value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

protected:

    /// Resize event.
    /**
     * @param[in,out] e  resize event
     */
    virtual void resizeEvent( QResizeEvent *e );

private Q_SLOTS:

    /// Slot for when screen changes.
    void onScreenChanged();

private:

    OperateMode *operateMode_;

    // header
    QLabel *testNumberLabel_;
    QLabel *testNumber_;

    QLabel *productNumberLabel_;
    QLabel *productNumber_;

    QLabel *vendorNumberLabel_;
    QLabel *vendorNumber_;

    QLabel *operatorNumberLabel_;
    QLabel *operatorNumber_;

    QLabel *startTimeLabel_;
    QLabel *startTime_;

    QLabel *endTimeLabel_;
    QLabel *endTime_;

    // body
    QTreeWidget *items_;

    // footer
    QLabel *spacer_;
    QLabel *totalSellItemsLabel_;
    QLabel *totalShrinkLabel_;
    QLabel *totalBuyItemLabel_;

    QLabel *sellItemsWeight_;
    QLabel *sellItemsYield_;
    QLabel *sellItemsPrice_;

    QLabel *shrinkWeight_;
    QLabel *shrinkYield_;

    QLabel *buyItemWeight_;
    QLabel *buyItemYield_;
    QLabel *buyItemAvgPrice_;

    QLabel *buyItemCostLabel_;
    QLabel *buyItemCost_;

    QLabel *imuLabel_;
    QLabel *imu_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Calculate item table column widths.
    void calcItemColumnWidths();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // CUTTINGTESTREPORT_H
