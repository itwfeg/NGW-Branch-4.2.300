/**
 * @file diagresolutiondialog.h
 * Dialog for resolving diagnostic code.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DIAGRESOULTIONDIALOG_H
#define DIAGRESOULTIONDIALOG_H

#include "../sessiondialog.h"

class QComboBox;
class QLabel;
class QPlainTextEdit;
class QPushButton;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog for resolving diagnostic code.
class DiagnosticResolutionDialog : public SessionDialog
{
    Q_OBJECT

    typedef DiagnosticResolutionDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] code  diagnostic code
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit DiagnosticResolutionDialog( int code, QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~DiagnosticResolutionDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve service method.
    /**
     * @return  method of service
     */
    virtual int serviceMethod( int index ) const;

    /// Retrieve service notes.
    /**
     * @return  service notes
     */
    virtual QString serviceNotes() const;

    /// Retrieve service part.
    /**
     * @return  service part
     */
    virtual int servicePart( int index ) const;

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const;

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Callback for session messages.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessageReceive( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot for index changed.
    void onCurrentIndexChanged( int index );

    /// Slot for push button clicked.
    void onPushButtonClicked();

    /// Slot for timer timeout.
    void onTimerTimeout();

private:

    typedef QPair<int, int> methodPartKey;          // method, part
    typedef QPair<QString, bool> partName;          // name, comments required

    QList<int> methodOrder_;                        // how to order methods in the dropdown
    QList<methodPartKey> partOrder_;                // how to order parts in the dropdown

    QMap<int, QString> methods_;                    // map of all methods
    QMap<methodPartKey, partName> parts_;           // map of all parts


    QLabel *header_;

    QLabel *serviceMethodLabel_;

    QLabel *serviceActionLabel1_;
    QComboBox *serviceMethod1_;
    QComboBox *servicePart1_;

    QLabel *serviceActionLabel2_;
    QComboBox *serviceMethod2_;
    QComboBox *servicePart2_;

    QLabel *serviceActionLabel3_;
    QComboBox *serviceMethod3_;
    QComboBox *servicePart3_;

    QLabel *serviceActionLabel4_;
    QComboBox *serviceMethod4_;
    QComboBox *servicePart4_;

    QLabel *serviceNotesLabel_;
    QPlainTextEdit *serviceNotes_;

    QPushButton *okayButton_;
    QPushButton *cancelButton_;

    QTimer *timer_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Populate method combo boxes.
    void populateMethods();

    /// Populate part combo box.
    void populateParts( QComboBox *serviceMethod, QComboBox *servicePart );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIAGRESOULTIONDIALOG_H
