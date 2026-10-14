/**
 * @file predictionwidget.h
 * Edgify Prediction viewer widget.
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef PREDICTIONWIDGET_H
#define PREDICTIONWIDGET_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QTcpServer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Edgify Prediction viewer widget.
class PredictionWidget : public QWidget
{
    Q_OBJECT

    typedef PredictionWidget _Myt;
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
    explicit PredictionWidget( QWidget *parent = 0 );

private slots:

    /// Slot for button clicked.
    void onButtonClicked();

    /// Slot for new callback connection.
    void onNewConnection();

    /// Slot for connection read read.
    void onReadyRead();

    /// Slot for text changed.
    void onTextChanged();

private:

    bool scanDataExists_;

    QTcpServer *callback_;

    QLabel *image_;

    QLabel *pluCodeLabel_;
    QLineEdit *pluCode_;
    QPushButton *setGroundTruth_;

    QPlainTextEdit *results_;

    QPushButton *predict_;

    QPushButton *exit_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    // not implemented
    PredictionWidget( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // PREDICTIONWIDGET_H
