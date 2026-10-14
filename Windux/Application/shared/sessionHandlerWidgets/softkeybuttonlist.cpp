/**
 * @file softkeybuttonlist.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "backgroundwidgettypes.h"
#include "softkeybutton.h"
#include "softkeybuttonlist.h"

#include <QApplication>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QUrl>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeyButtonListWidget::softkeyButtonListWidget( QWidget *parent ) :
    _Mybase( parent ),
    rowHeight_( -1 )
{
    setObjectName( "hobartSoftkeyButtonListWidget" );

    // set item delegate
    setItemDelegate( new softkeyButtonListItemDelegate( this ) );

    // enable dragging and dropping
    setAcceptDrops( true );
    setDragEnabled( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeyButtonListWidget::~softkeyButtonListWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButtonListWidget::setRowHeight( int value )
{
    // nothing to do
    if ( rowHeight_ == value )
        return;

    QAbstractItemDelegate *w = itemDelegate();

    if ( !w )
        return;

    softkeyButtonListItemDelegate *delegate = dynamic_cast<softkeyButtonListItemDelegate*>( w );

    if ( !delegate )
        return;

    delegate->setFixedHeight( rowHeight_ = value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeyButton *softkeyButtonListWidget::softkeyButtonItem( int index ) const
{
    QListWidgetItem *listItem( item( index ) );

    if ( !listItem )
        return NULL;

    QWidget *w( itemWidget( listItem ) );

    if ( !w )
        return NULL;

    return dynamic_cast<softkeyButton*>( w );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButtonListWidget::addSoftkeyButtonItem( const QString& text, int action )
{
    // add to list
    addSoftkeyButtonItem( createSoftkeyButton( text, action ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButtonListWidget::addSoftkeyButtonItem( softkeyButton *b )
{
    // add new item to list
    QListWidgetItem *item = new QListWidgetItem();
    addItem( item );

    // set item widget
    setItemWidget( item, b );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButtonListWidget::insertSoftkeyButtonItem( int row, const QString& text, int action )
{
    // insert into list
    insertSoftkeyButtonItem( row, createSoftkeyButton( text, action ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButtonListWidget::insertSoftkeyButtonItem( int row, softkeyButton *b )
{
    // insert new item into list
    QListWidgetItem *item = new QListWidgetItem();
    insertItem( row, item );

    // set item widget
    setItemWidget( item, b );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
Qt::DropAction softkeyButtonListWidget::setDropAction( T *e )
{
    // use copy action by default
    Qt::DropAction action = Qt::CopyAction;

    // when dragging from grid widget we are moving, not copying!
    QWidget *source( dynamic_cast<QWidget*>( e->source() ) );
    int parents = 2;

    while (( source ) && ( 0 <= parents-- ))
    {
        if ( !dynamic_cast<_Myt*>( source ) )
            source = source->parentWidget();
        else
        {
            action = Qt::MoveAction;
            break;
        }
    }

    e->setDropAction( action );
    return action;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButtonListWidget::dragEnterEvent( QDragEnterEvent *e )
{
    if ( e->mimeData()->hasFormat( softkeyButton::mimeDataType() ) )
    {
        e->acceptProposedAction();
        return;
    }

    _Mybase::dragEnterEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButtonListWidget::dragMoveEvent( QDragMoveEvent *e )
{
    // sanity check
    if ( !e )
        return;

    // validate mime type
    if ( e->mimeData()->hasFormat( softkeyButton::mimeDataType() ) )
    {
        // set drop action
        setDropAction( e );

        // accept
        e->accept();
        return;
    }

    _Mybase::dragMoveEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButtonListWidget::dropEvent( QDropEvent *e )
{
    LOG_CLASS_INFO( "softkeyButtonListWidget", "dropEvent" );

    // sanity check
    if ( !e )
        return;

    // validate mime type
    if ( e->mimeData()->hasFormat( softkeyButton::mimeDataType() ) )
    {
        // set drop action
        Qt::DropAction action = setDropAction( e );

        LOG_TRACE( "have '%s' action", ((Qt::MoveAction == action) ? "move" : "copy") );

        QByteArray a( e->mimeData()->data( softkeyButton::mimeDataType() ) );
        softkeyButton *b( createSoftkeyButton( a ) );

        // figure out where it was dropped
        QModelIndex index( indexAt( e->pos() ) );
        int row = index.row();

        // valid row, insert at location
        if ( 0 <= row )
        {
            LOG_INFO( "inserting button to list at row %d", row );
            insertSoftkeyButtonItem( row, b );
        }

        // invalid row, add to list
        else
        {
            LOG_INFO( "adding button to list", NULL );
            addSoftkeyButtonItem( b );
        }

        LOG_DEBUG( "done", NULL );
        e->accept();

        return;
    }

    _Mybase::dropEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButtonListWidget::onSoftkeyButtonDestroyed( QObject *obj )
{
    // find and remove item associated with this object
    for ( int i = 0; i < count(); ++i )
    {
        QListWidgetItem *listItem( item( i ) );

        if ( !listItem )
            continue;

        QWidget *w( itemWidget( listItem ) );

        // found it
        if (( !w ) || ( obj == w ))
        {
            // remove widget
            removeItemWidget( listItem );

            // delete item
            delete listItem;

            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeyButton *softkeyButtonListWidget::createSoftkeyButton( const QString& text, int action )
{
    // create button (no parent, ownership taken later)
    softkeyButton *b = new softkeyButton();
    b->setAction( action );
    b->setText( text );

    groomSoftkeyButton( b );
    return b;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeyButton *softkeyButtonListWidget::createSoftkeyButton( const QByteArray& a )
{
    // create button (no parent, ownership taken later)
    softkeyButton *b = new softkeyButton();
    b->deserialize( a );

    groomSoftkeyButton( b );
    return b;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButtonListWidget::groomSoftkeyButton( softkeyButton *b ) const
{
    b->resetBackgroundImage();
    b->resetForegroundImage();

    b->setBackgroundColor( hobart::colorPalette::gray );
    b->setBorderColor( hobart::colorPalette::white );
    b->setCornerStyle( softkeyButton::RoundedCornerStyle );
    b->setDraggable( true );
    b->setGridSize( QSize( 1, 1 ) );
    b->setTextAlignment( Qt::AlignCenter );
    b->setTextColor( hobart::colorPalette::white );

    if ( 0 <= rowHeight() )
        b->setFixedHeight( rowHeight() );

    connect( b, SIGNAL( destroyed(QObject*) ), SLOT( onSoftkeyButtonDestroyed(QObject*) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeyButtonListItemDelegate::softkeyButtonListItemDelegate( QWidget *parent ) :
    _Mybase( parent ),
    fixedHeight_( -1 ),
    fixedWidth_( -1 ),
    list_( dynamic_cast<softkeyButtonListWidget*>( parent ) )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeyButtonListItemDelegate::~softkeyButtonListItemDelegate()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize softkeyButtonListItemDelegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex& index ) const
{
    QSize result( _Mybase::sizeHint( option, index ) );

    if (( !result.isValid() ) && ( list_ ))
    {
        QListWidgetItem *item( list_->item( index.row() ) );
        QRect itemRect( list_->visualItemRect( item ) );

        result = itemRect.size();
    }

    if ( !result.isValid() )
        return QSize();

    if ( 0 <= fixedHeight_ )
        result.setHeight( fixedHeight_ );

    if ( 0 <= fixedWidth_ )
        result.setWidth( fixedWidth_ );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace


