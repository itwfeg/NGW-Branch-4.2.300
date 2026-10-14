/**
 * @file selfserveWeightsMeasures.h
 * Weights and measures screen for self service.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup selfServeCsu
 * @{
 */

#ifndef SELFSERVEWEIGHTSMEASURES_H
#define SELFSERVEWEIGHTSMEASURES_H

#include <QSignalMapper>
#include <QWidget>

class selfServiceHook;
class selfServiceWmNumberPad;

class QLabel;

/// Hobart namespace.
namespace hobart {

class pushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Weights and measures screen for self service.
class selfserveWeightsMeasures : public QWidget
{
    Q_OBJECT

    typedef selfserveWeightsMeasures _Myt;
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
    selfserveWeightsMeasures( selfServiceHook *h, QWidget *parent = NULL );

    /// DTOR
    virtual ~selfserveWeightsMeasures();

    // ************************************************************************
    // Methods
    // ************************************************************************

    /// Refresh widget.
    virtual void refresh();

    /// Translate the user interface to current language.
    virtual void translate();

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

private:

    selfServiceHook *hook_;

    QSignalMapper *buttonSignalMapper_;

    bool unlocked_;
    bool cat3_;

    hobart::pushButton *rezero_;
    hobart::pushButton *auditlog_;
    hobart::pushButton *quit_;

    QLabel *title_;

    QLabel *exitLabel_;
    hobart::pushButton *exit_;

    QWidget *weightsMeasures_;
    QWidget *auditLog_;

    selfServiceWmNumberPad *numberPad_;
    hobart::pushButton *enter_;

    QLabel *indicator_SoftwareVersionLabel_;
    QLabel *indicator_SoftwareVersion_;

    QLabel *indicator_ApprovalLabel_;
    QLabel *indicator_Approval_;

    QLabel *indicator_nMaxLabel_;
    QLabel *indicator_nMax_;

    QLabel *spacer1_Label_;
    QLabel *spacer1_;

    QLabel *weigherModelLabel_;
    QLabel *weigherModel_;

    QLabel *softwareVersionLabel_;
    QLabel *softwareVersion_;

    QLabel *hardwareVersionLabel_;
    QLabel *hardwareVersion_;

    QLabel *capacityLabel_;
    QLabel *capacity_;

    QLabel *eMinLabel_;
    QLabel *eMin_;

    QLabel *classLabel_;
    QLabel *class_;

    QLabel *nMaxLabel_;
    QLabel *nMax_;

    QLabel *approvalUSLabel_;
    QLabel *approvalUS_;

    QLabel *approvalCanLabel_;
    QLabel *approvalCan_;

    QLabel *spacer2_Label_;
    QLabel *spacer2_;

    QLabel *configParamsLabel_;
    QLabel *configParams_;

    QLabel *calibrationLabel_;
    QLabel *calibration_;

    QLabel *tempRangeLabel_;
    QLabel *tempRange_;

    // not implemented
    selfserveWeightsMeasures( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

    /// Initialize object.
    void initialize();

    /// Create layout.
    void createLayout();

    /// launch audit log
    void launchAuditLog();

    /// Generate exit button pixmap.
    static QPixmap exitButtonPixmap();

    // SOF-894
    int wayer_model;
    QString countryCode;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELFSERVEWEIGHTSMEASURES_H
