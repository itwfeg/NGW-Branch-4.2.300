/**
 * @file clientcommdialog.h
 * Client communication dialog.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef CLIENTCOMMDIALOG_H
#define CLIENTCOMMDIALOG_H

#include <QDialog>
#include <QWidgetList>

class QByteArray;
class QPlainTextEdit;
class QStringList;
class QTabWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Client communication dialog.
class clientCommDialog : public QDialog
{
    Q_OBJECT

    typedef clientCommDialog _Myt;
    typedef QDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] header  message header
     * @param[in] body  message body
     * @param[in] parent  perent widget
     */
    clientCommDialog( const QStringList &header, const QByteArray &body, QWidget *parent = 0 );

    /// Constructor.
    /**
     * @param[in] requestHeader  request message header
     * @param[in] requestBody  request message body
     * @param[in] responseHeader  response message header
     * @param[in] responseBody  response message body
     * @param[in] parent  perent widget
     */
    clientCommDialog( const QStringList &requestHeader, const QByteArray &requestBody, const QStringList &responseHeader, const QByteArray &responseBody, QWidget *parent = 0 );

    /// Destructor.
    virtual ~clientCommDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve size hint.
    /**
     * @return  size
     */
    virtual QSize sizeHint() const;

private:

    QTabWidget *tabs_;

    QWidget *request_;
    QWidgetList requestHeader_;
    QPlainTextEdit *requestBody_;

    QWidget *response_;
    QWidgetList responseHeader_;
    QPlainTextEdit *responseBody_;

    /// Initialize.
    void initialize( const QStringList &requestHeader, const QByteArray &requestBody, const QStringList &responseHeader = QStringList(), const QByteArray &responseBody = QByteArray() );

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CLIENTCOMMDIALOG_H
