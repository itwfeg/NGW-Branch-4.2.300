/**
 * @file configurewidget.h
 * Edgify Configuration widget.
 *
 * @copyright Copyright (c) 2022 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef CONFIGUREWIDGET_H
#define CONFIGUREWIDGET_H

#include <QDir>
#include <QMap>
#include <QString>
#include <QWidget>

class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Edgify Configuration viewer widget.
class ConfigureWidget : public QWidget
{
    Q_OBJECT

    typedef ConfigureWidget _Myt;
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
    explicit ConfigureWidget( QWidget *parent = 0 );

private slots:

    /// Slot for button clicked.
    void onButtonClicked();

    /// Slot for devices changed.
    void onDevicesChanged();

    /// Slot for list widget item clicked.
    void onItemClicked( QListWidgetItem *item );

private:

    typedef QMap<QPushButton*, QString> ButtonTagMap;

    QLabel *serverLabel_;
    QLineEdit *server_;

    QListWidget *servers_;

    QPushButton *accept_;

    QPushButton *edgifyOff_;                        // turn edgify off
    QPushButton *edgifyTrust1_;                     // show dialog with results
    QPushButton *edgifyTrust2_;                     // pull up item when 100% certain, otherwise show dialog
    QPushButton *edgifyLearningOnly_;               // learning mode (no dialogs)

    QPushButton *exit_;

    ButtonTagMap tags_;
    QDir downloadFolder_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Parse configuration.
    void parseConfig();

    /// Save configuration.
    void saveConfig();

    // not implemented
    ConfigureWidget( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CONFIGUREWIDGET_H
