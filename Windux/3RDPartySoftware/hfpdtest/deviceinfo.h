#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QBluetoothUuid>
#include <QDialog>

class bleDevice;

class QBluetoothDeviceInfo;
class QByteArray;
class QLineEdit;
class QPushButton;
class QTableWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////
class deviceInfo : public QDialog
{
    Q_OBJECT

    typedef deviceInfo _Myt;
    typedef QDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent
     */
    deviceInfo( const QBluetoothDeviceInfo &info, QWidget *parent = 0 );

    /// Destructor.
    virtual ~deviceInfo();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve size hint.
    /**
     * @return  size
     */
    virtual QSize sizeHint() const;

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

private slots:

    /// Slot for push button clicked.
    void onPushButtonClicked();

    /// Slot for tag characteristic changed.
    void onTagValueChanged( const QBluetoothUuid &uuid, const QByteArray &value );

private:

    bleDevice *tag_;

    QMap<QPushButton*, QLineEdit*> lineEdit_;
    QMap<QPushButton*, QBluetoothUuid> uuid_;

    // ---- //

    QTableWidget *table_;

    /// Initialize.
    void initialize();

    /// Create Layout.
    void createLayout();

    /// Resize table widget.
    void resizeTableWidget();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DEVICEINFO_H
