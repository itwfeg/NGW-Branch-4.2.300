/**
 * @file datamatrixEditor.h
 * @brief Widget to allow editing of the GS1 Databar( aka RSS14) string.
 *
 * @copyright Copyright (c) 2018, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe E. Crew
 */

#ifndef DATAMATRIXEDITOR_H
#define DATAMATRIXEDITOR_H

#include <QWidget>
#include <QMap>

class SessionMainWindow;
class QPushButton;
class QSignalMapper;

namespace Ui {
class datamatrixEditor;
}

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget to allow the DataMatrix strings to be created and modified.
class datamatrixEditor : public QWidget
{
    Q_OBJECT

    typedef datamatrixEditor _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================
    /// Constructor.
    /**
     * @param[in] formatStr  initial data matrix string
     * @param[in] qr  @c true for QR modification, @c false otherwise
     */
    explicit datamatrixEditor(const QString &formatStr, QWidget *parent = NULL );

    /// Destructor.
    virtual ~datamatrixEditor();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

signals:


    /**
     * SIGNAL broadcast to anyone listening when the Done button is pressed.
     *
     * @param[in] msg  session message
     */
    /// TEST TO REMOVE
    void dataMatrixEditorDoneClicked(QString);

    /**
     * SIGNAL broadcast to anyone listening when the Cancel button is pressed.
     *
     * @param[in] msg  session message
     */
    /// TEST TO REMOVE
    void dataMatrixEditorCancelClicked( QString );


    /// Broadcast to anyone listening when the Done button is pressed.
    /**
     * @param[in] formatString  updated Data Matrix string
     */
    void doneClickedDM( const QString &formatString );


    /// Broadcast to anyone listening when the Cancel button is pressed.
    void cancelClickedDM();

private slots:

    /// Slot to handle clicks on key words to trigger instructions.
    void onInstructionClicked( QWidget *widget );

    /// Slot for button clicked.
    void onButtonClicked();

private:
    Ui::datamatrixEditor *ui;

    SessionMainWindow *mainWindow_;

    QSignalMapper *signalMapper_;

    int datamatrixType_;
    int datamatrixLength_;
    QString datamatrixString_;

    bool hasChanged;


    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method that disables features once the GS1 format string max length is reached.
     *
     * @param[in] none
     * @returns  bool true if the max length has been reached, otherwise false
     */
    bool checkForMaxLength();


    //void emitDoneClicked(QString datamatrixString);
    //void emitCancelClicked(QString datamatrixString);

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // DATAMATRIXEDITOR_H
