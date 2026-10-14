/**
 * @file settingsviewerwidget.h
 * Settings viewer widget for Walmart Web Service calls.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SETTINGSVIEWERWIDGET_H
#define SETTINGSVIEWERWIDGET_H

#include <QList>
#include <QPair>
#include <QWidget>

class QPushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Class to view Walmart Web Service settings.
class settingsViewerWidget : public QWidget
{
    Q_OBJECT

    typedef settingsViewerWidget _Myt;
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
    explicit settingsViewerWidget( QWidget *parent = 0 );

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Retrieve log filename.
    /**
     * @return  log filename
     */
    static QString filename();

private slots:

    /// Slot for button clicked.
    void onButtonClicked();

private:

    typedef QPair<QWidget*, QWidget*> widgetPair;

    QList<widgetPair> settings_;

    QPushButton *exit_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    // not implemented
    settingsViewerWidget( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SETTINGSVIEWERWIDGET_H
