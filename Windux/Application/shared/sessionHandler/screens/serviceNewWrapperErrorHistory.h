/**
 * @file serviceNewWrapperErrorHistory.h
 * @brief Service screen for viewing wrapper error history.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup Supervisor and Service Mode CSU
 * @{
*/

#ifndef SERVICENEWWRAPPERERRORHISTORY_H
#define SERVICENEWWRAPPERERRORHISTORY_H

#include "screenwidget.h"

class DomEvolWrapperErrors;
class DomEvolWrapperErrorsList;

class QTextEdit;
class QTreeWidget;
class QTreeWidgetItem;
class QPushButton;
class QSignalMapper;
class QStackedWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Service screen for viewing wrapper error history.
class serviceNewWrapperErrorHistory : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperErrorHistory _Myt;
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
    explicit serviceNewWrapperErrorHistory( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~serviceNewWrapperErrorHistory();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

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
    // Methods
    // ========================================================================

    /// Insert schematics table into error text.
    /**
     * @param[in,out] edit  text edit to insert into
     */
    virtual void insertSchematics( QTextEdit *edit ) const;

    /// Insert limits table into error text.
    /**
     * @param[in,out] edit  text edit to insert into
     */
    virtual void insertLimits( QTextEdit *edit ) const;

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot for handling push button clicked.
    void onPushButtonClicked( QWidget *widget );

private:

    QSignalMapper *pushButtonSignalMapper_;

    QStackedWidget *pages_;

    QWidget *tablePage_;
    QTreeWidget *table_;
    QPushButton *saveErrorHistory_;
    QPushButton *clearHistory_;

    QWidget *helpPage_;
    QTextEdit *helpText_;
    QPushButton *back_;

    QFont tableFont_;
    QFont tableFontSplitRow_;

    QList<DomEvolWrapperErrors*> errors_;
    QList<DomEvolWrapperErrorsList*> errorsList_;

    QStringList errorStr_;

    QMap<QWidget*, int> indexMap_;

    QMap<int, int> errorMap_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Resize table column widths.
    void calcTableColumnWidths();

    /// Set font point sizes for item in table.
    void calcTableFontSizes( QTreeWidgetItem *item ) const;

    /// Lookup text for error code.
    QString errorText( int errCode ) const;

    /// Insert html into text edit.
    bool insertHtml( QTextEdit *edit, const QString& filename, const QMap<QString, QString>& translations ) const;

    /// Save error history to file.
    void exportErrorHistory() const;

    void fakeData();

    bool checkFileName( QString& myfile, int fileOrFolder ) const;

    static QString getFileName( QString myfile );

    static bool isLinux();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SERVICENEWWRAPPERERRORHISTORY_H
