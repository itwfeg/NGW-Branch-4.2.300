/**
 * @file selfserveWeightsMeasures.h
 * Weights and measures screen for self service.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Joe Crew
 * @absorbed from Randy Blankley
 *
 * @addtogroup selfServeCsu
 * @{
 */

#ifndef SELFSERVEWEIGHTSMEASURESG2_H
#define SELFSERVEWEIGHTSMEASURESG2_H

#include <QSignalMapper>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QTimer>
#include <QIntValidator>
#include <numericlineedit.h>

namespace Ui {
class selfserveWeightsMeasuresG2;
}

class selfServiceHook;

/// Hobart namespace.
namespace hobart {

class pushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Weights and measures screen for self service.
class selfserveWeightsMeasuresG2 : public QWidget
{
    Q_OBJECT

    typedef selfserveWeightsMeasuresG2 _Myt;
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
    selfserveWeightsMeasuresG2( selfServiceHook *h, QWidget *parent = NULL );

    /// DTOR
    virtual ~selfserveWeightsMeasuresG2();

    // ************************************************************************
    // Methods
    // ************************************************************************

    /// Refresh widget.
    virtual void refresh();

    /// Translate the user interface to current language.
    virtual void translate();

    /// Update the connectivity background
    virtual void updateNetworkConnectivity( bool online );

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

    /**
     * Hide event.
     *
     * @param[in,out] e  event
     */
    virtual void hideEvent( QHideEvent *e );

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

    /// Slot to reload images.
    void onReloadImages();

    /// Slot for timer timeout.
    void onKeypadTimeout();

private:

    Ui::selfserveWeightsMeasuresG2 *ui;
    selfServiceHook *hook_;

    QSignalMapper *buttonSignalMapper_;
    QIntValidator *numValidator_;

    bool unlocked_;
    bool cat3_;

    QString errorcode_;
    int errorstyle_;

    QPushButton *rezero_;
    QPushButton *auditlog_;
    QPushButton *quit_;
    QPushButton *exit_;
    QPushButton *enter_;
    QPushButton *backspace_;
    QPushButton *activeKey_;

    hobart::numericLineEdit  *opID_;

    QLabel *title_;

    QLabel *exitLabel_;

    QWidget *weightsMeasures_;
    QWidget *screen_;

    QLabel *indicator_SoftwareVersionLabel_;
    //QLabel *indicator_SoftwareVersion_;

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
    selfserveWeightsMeasuresG2( const _Myt& rhs );

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

    /// Sets the top number of digits in the string being masked and validated
    int makeValidatorTop(int digits);

    // SOF-894
    int wayer_model;
    QString countryCode;

    QTimer *cycleNumpadTimer_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELFSERVEWEIGHTSMEASURESG2_H
