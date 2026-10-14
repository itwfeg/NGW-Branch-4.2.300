/**
 * @file wrappermodedialog.h
 * Dialog for choosing wrapper mode.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef WRAPPERAUDITDIALOG_H
#define WRAPPERAUDITDIALOG_H

#include "../sessiondialog.h"

class QComboBox;
class QLabel;
class QPlainTextEdit;
class QToolButton;
class QTreeWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog for wrapper audit log.
class WrapperAuditDialog : public SessionDialog
{
    Q_OBJECT

    typedef WrapperAuditDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mf  mode flags
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit WrapperAuditDialog( const QStringList &trays, bool doubleRoll, int upperRollWidth, int lowerRollWidth, QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~WrapperAuditDialog();

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
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  resize event
     */
    virtual void resizeEvent( QResizeEvent *e );

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

    /// Slot for button pressed
    void onPushButtonClicked();

private:

    bool doubleRoll_;

    QLabel *traysLabel_;
    QTreeWidget *trays_;

    QLabel *upperRollLabel_;
    QLabel *upperRollManfLabel_;
    QComboBox *upperRollManf_;
    QLabel *upperRollWidthLabel_;
    QComboBox *upperRollWidth_;
    QLabel *upperRollGaugeLabel_;
    QComboBox *upperRollGauge_;

    QLabel *lowerRollLabel_;
    QLabel *lowerRollManfLabel_;
    QComboBox *lowerRollManf_;
    QLabel *lowerRollWidthLabel_;
    QComboBox *lowerRollWidth_;
    QLabel *lowerRollGaugeLabel_;
    QComboBox *lowerRollGauge_;

    QLabel *commentsLabel_;
    QPlainTextEdit *comments_;

    QLabel *error_;
    QToolButton *okay_;
    QToolButton *cancel_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Populate trays.
    void populateTrays( const QStringList &trays );

    /// Calculate column widths.
    void calcItemColumnWidths();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // WRAPPERAUDITDIALOG_H
