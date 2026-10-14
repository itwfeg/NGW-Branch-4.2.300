/**
 * @file gs1Editor.h
 * @brief Widget to allow editing of the GS1 Databar( aka RSS14) string.
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe E. Crew
 */

#ifndef GS1EDITOR_H
#define GS1EDITOR_H

#include <QWidget>

class SessionMainWindow;

class QPushButton;

namespace Ui {
class gs1Editor;
}

/// Hobart namespace.
namespace hobart {

class digitEntry;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget to allow the GS1 string to be created and modified.
class gs1Editor : public QWidget
{
    Q_OBJECT
    
    typedef gs1Editor _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] gs1  initial GS1 string
     * @param[in] ai  @c true for AI modification, @c false otherwise
     */
    explicit gs1Editor( const QString &gs1, bool ai, QWidget *parent = NULL );

    /// Destructor.
    virtual ~gs1Editor();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

signals:

    /// Broadcast to anyone listening when the Done button is pressed.
    /**
     * @param[in] gs1  updated GS1 string
     */
    void doneClicked( const QString &gs1 );

    /// Broadcast to anyone listening when the Cancel button is pressed.
    void cancelClicked();

private slots:

    /// Slot for button clicked.
    void onButtonClicked();

    /// Slot for digit entry.
    void onDigitEntryNumberClicked( const QString &value );

    /// Slot for digit entry clear.
    void onDigitEntryClearClicked();

    /// Slot for digit entry done.
    void onDigitEntryDoneClicked();

private:

    Ui::gs1Editor *ui_;

    QString gs1_;
    bool ai_;

    SessionMainWindow *mainWindow_;

    hobart::digitEntry *digitEntry_;

    /// Method to initialize all variables, sets components visible or hidden, and creates all signals and slots.
    void initialize();

    /// Check current state.
    void checkCurrentState();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // hobart namespace

#endif // GS1EDITOR_H
