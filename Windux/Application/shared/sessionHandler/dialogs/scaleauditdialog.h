/**
 * @file scaleauditdialog.h
 * Dialog for scale audit notes entry.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SCALEAUDITDIALOG_H
#define SCALEAUDITDIALOG_H

#include "../sessiondialog.h"

#include <QMap>
#include <QString>

class QComboBox;
class QLabel;
class QPlainTextEdit;
class QToolButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog for scale audit log.
class ScaleAuditDialog : public SessionDialog
{
    Q_OBJECT

    typedef ScaleAuditDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit ScaleAuditDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~ScaleAuditDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve audit report.
    /**
     * @return  audit report
     */
    virtual QString report() const;

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const;

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve renderer appearance.
    /**
     * @return  appearance object
     */
    virtual hobart::widgetRendererAppearanceBase *rendererAppearance() const;

    /// Retrieve renderer style.
    /**
     * @return  style
     */
    virtual int rendererStyle() const;

private Q_SLOTS:

    /// Slot to check okay enabled.
    void checkOkayEnabled();

    /// Slot for combo index changed.
    void onCurrentIndexChanged( int index );

    /// Slot for button pressed
    void onPushButtonClicked();

private:

    QMap<int, QString> reasonTree_;

    QLabel *reasonForVisitLabel_;
    QComboBox *reason1_;
    QComboBox *reason2_;
    QComboBox *reason3_;

    QLabel *commentsLabel_;
    QPlainTextEdit *comments_;

    QLabel *error_;
    QToolButton *okay_;
    QToolButton *cancel_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Populate reasons.
    void populateReason( int which );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SCALEAUDITDIALOG_H
