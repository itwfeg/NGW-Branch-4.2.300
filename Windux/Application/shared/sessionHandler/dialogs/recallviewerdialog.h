/**
 * @file recallviewerdialog.h
 * Dialog that displays a recall to the operator.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef RECALLVIEWERDIALOG_H
#define RECALLVIEWERDIALOG_H

#include "../sessiondialog.h"

class QLabel;
class QPlainTextEdit;
class QPushButton;
class QSignalMapper;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog that displays a recall to the operator.
class RecallViewerDialog : public SessionDialog
{
    Q_OBJECT

    typedef RecallViewerDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] id  recall id
     * @param[in] date  recall date
     * @param[in] recallText  recall text body
     * @param[in] isNew  @c true is recall is new and can be dismissed, @c false otherwise
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit RecallViewerDialog(
        const QString& id,
        const QString& date,
        const QString& recallText,
        bool isNew,
        QWidget *parent = 0,
        Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~RecallViewerDialog();

private Q_SLOTS:

    /// Slot for button clicked.
    void onButtonClicked( QWidget *button );

private:

    QString id_;
    QString date_;
    QString recallText_;
    bool isNew_;

    QSignalMapper *signalMapper_;

    QWidget *icon_;
    QLabel *title_;
    QPlainTextEdit *text_;

    QPushButton *dismiss_;
    QPushButton *close_;

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // RECALLVIEWERDIALOG_H
