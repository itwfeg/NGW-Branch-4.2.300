
#include "deviceinfo.h"

#include "device/hfpdtag.h"
#include "device/hfpdtag_v2.h"

#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
deviceInfo::deviceInfo( const QBluetoothDeviceInfo &info, QWidget *parent ) :
    _Mybase( parent ),
    tag_( NULL )
{
    // init
    initialize();
    createLayout();

    resizeTableWidget();

    // open tag device
    tag_ = new hfpdTagV2( info, this );

    connect( tag_, SIGNAL(valueChanged(QBluetoothUuid,QByteArray)), SLOT(onTagValueChanged(QBluetoothUuid,QByteArray)) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
deviceInfo::~deviceInfo()
{
    if ( tag_ )
        delete tag_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize deviceInfo::sizeHint() const
{
    return QSize( 800, 500 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void deviceInfo::resizeEvent( QResizeEvent *e )
{
    resizeTableWidget();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void deviceInfo::onPushButtonClicked()
{
    QPushButton *button( qobject_cast<QPushButton*>( sender() ) );

    if ( !button )
        return;
    else if ( !lineEdit_.contains( button ) )
        return;
    else if ( !uuid_.contains( button ) )
        return;

    // set new value
    const QString value( lineEdit_[button]->text() );
    const QByteArray buffer( QByteArray::fromStdString( value.toStdString() ) );

    tag_->setValue( uuid_[button], QByteArray::fromHex( buffer ) );

    if ( !tag_->isWriteOnly( uuid_[button] ) )
        button->setEnabled( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void deviceInfo::onTagValueChanged( const QBluetoothUuid &uuid, const QByteArray &value )
{
    const QByteArray buffer( value.toHex() );

    int row( table_->rowCount() );

    while ( row-- )
    {
        const QTableWidgetItem *item( table_->item( row, 0 ) );

        if ( item->text() == uuid.toString() )
            break;
    }

    // item exists in table already
    if ( 0 <= row )
    {
        QLineEdit *column2_widget( qobject_cast<QLineEdit*>( table_->cellWidget( row, 2 ) ) );
        QPushButton *column3_widget( qobject_cast<QPushButton*>( table_->cellWidget( row, 3 ) ) );

        if ( column2_widget )
            column2_widget->setText( QString::fromStdString( buffer.toStdString() ) );

        if ( column3_widget )
            column3_widget->setEnabled( true );
    }

    // item does not exist in table
    else
    {
        row = table_->rowCount();

        table_->insertRow( row );
        table_->setItem( row, 0, new QTableWidgetItem( uuid.toString() ) );
        table_->setItem( row, 1, new QTableWidgetItem( tag_->characteristicName( uuid ) ) );
        table_->setItem( row, 2, new QTableWidgetItem() );
        table_->setItem( row, 3, new QTableWidgetItem() );

        QLineEdit *column2_widget( new QLineEdit( this ) );
        column2_widget->setText( QString::fromStdString( buffer.toStdString() ) );

        table_->setCellWidget( row, 2, column2_widget );

        if ( !tag_->isReadOnly( uuid ) )
        {
            QPushButton *column3_widget( new QPushButton( this ) );
            column3_widget->setText( tr( "Update" ) );

            connect( column3_widget, SIGNAL(clicked(bool)), SLOT(onPushButtonClicked()) );

            table_->setCellWidget( row, 3, column3_widget );

            // save date related to this row
            lineEdit_[column3_widget] = column2_widget;
            uuid_[column3_widget] = uuid;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void deviceInfo::initialize()
{
    table_ = new QTableWidget( this );

    table_->setColumnCount( 4 );
    table_->setHorizontalHeaderItem( 0, new QTableWidgetItem() );
    table_->setHorizontalHeaderItem( 1, new QTableWidgetItem( tr( "Characteristic" ) ) );
    table_->setHorizontalHeaderItem( 2, new QTableWidgetItem( tr( "Value" ) ) );
    table_->setHorizontalHeaderItem( 3, new QTableWidgetItem() );

    table_->verticalHeader()->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void deviceInfo::createLayout()
{
    QVBoxLayout *formLayout( new QVBoxLayout( this ) );
    formLayout->addWidget( table_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void deviceInfo::resizeTableWidget()
{
    const int tableWidth( table_->width() );

    table_->hideColumn( 0 ); // uuid
    table_->setColumnWidth( 1, (tableWidth * 35) / 100 ); // characteristic
    table_->setColumnWidth( 2, (tableWidth * 50) / 100 ); // value
    table_->setColumnWidth( 3, ((tableWidth * 15) / 100) - 16 ); // update button
}
