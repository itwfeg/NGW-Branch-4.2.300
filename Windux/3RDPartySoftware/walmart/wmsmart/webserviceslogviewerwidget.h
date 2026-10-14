/**
 * @file webserviceslogviewerwidget.h
 * Log viewer widget for Walmart Web Service calls.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef WEBSERVICESLOGVIEWERWIDGET_H
#define WEBSERVICESLOGVIEWERWIDGET_H

#include <QMap>
#include <QString>
#include <QStringList>
#include <QWidget>

class QCheckBox;
class QPushButton;
class QTreeWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Class to view Walmart Web Service calls.
class webServicesLogViewerWidget : public QWidget
{
    Q_OBJECT

    typedef webServicesLogViewerWidget _Myt;
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
    explicit webServicesLogViewerWidget( QWidget *parent = 0 );

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Retrieve log filename.
    /**
     * @return  log filename
     */
    static QString filename();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    void resizeEvent( QResizeEvent *e );

private slots:

    /// Slot for button clicked.
    void onButtonClicked();

private:

    QMap<QString, QString> columnHeaders_;
    QMap<QString, int> columnWidths_;

    QStringList queries_;

    QTreeWidget *table_;

    QCheckBox *includeScaleLogs_;

    QPushButton *saveLogs_;
    QPushButton *exit_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Load all queries.
    bool loadAllQueries();

    /// Calcuate column widths.
    void calcColumnWidths();

    /// Retrieve hook version.
    static int hookVersion();

    // not implemented
    webServicesLogViewerWidget( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////


#endif // WEBSERVICESLOGVIEWERWIDGET_H
