/**
 * @file terminalwidget.h
 * Edgify Terminal widget.
 *
 * @copyright Copyright (c) 2022 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef TERMINALWIDGET_H
#define TERMINALWIDGET_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Edgify Terminal widget.
class TerminalWidget : public QWidget
{
    Q_OBJECT

    typedef TerminalWidget _Myt;
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
    explicit TerminalWidget( QWidget *parent = 0 );

private slots:

    /// Slot for button clicked.
    void onButtonClicked();

    /// Slot for timeout
    void onTimeout();

private:

    enum {REFRESH_TIMER = 1000};

    QLabel *cmdLabel_;
    QLineEdit *cmd_;

    QListWidget *results_;

    QPushButton *run_;

    QPushButton *clearPrompt_;
    QPushButton *clearResults_;

    QPushButton *exit_;

    QTimer *refresh_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    // not implemented
    TerminalWidget( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TERMINALWIDGET_H
