/**
 * @file wmconfigviewerwidget.h
 * Walmart configuration viewer widget for Walmart Web Service calls.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef WMCONFIGVIEWERWIDGET_H
#define WMCONFIGVIEWERWIDGET_H

#include <QList>
#include <QPair>
#include <QWidget>

class QPlainTextEdit;
class QPushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Class to view Walmart web service configuration.
class walmartConfigViewerWidget : public QWidget
{
    Q_OBJECT

    typedef walmartConfigViewerWidget _Myt;
    typedef QWidget _Mybase;

signals:

    /// Signal for when screen exited.
    void exitScreen();

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit walmartConfigViewerWidget( QWidget *parent = 0 );

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Retrieve conf filename.
    /**
     * @return  conf filename
     */
    static QString hobartConfFilename();

    /// Retrieve conf filename.
    /**
     * @return  conf filename
     */
    static QString walmartConfFilename();

private slots:

    /// Slot for button clicked.
    void onButtonClicked();

private:

    typedef QPair<QWidget*, QWidget*> widgetPair;

    QList<widgetPair> settings_;

    QPlainTextEdit *wmconfig_;

    QPushButton *exit_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    // not implemented
    walmartConfigViewerWidget( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // WMCONFIGVIEWERWIDGET_H
