/**
 * @file listdialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */


#include "listdialog.h"

#include "../sessionmainwindow.h"

#include <backgroundwidgettypes.h>
#include <logmanager.h>
#include <palette.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPainter>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
ListDialog::ListDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f )
{
    // set object name
    setObjectName( "ListDialog" );

    // initialize interface
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ListDialog::~ListDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize ListDialog::thumbnailSize() const
{
    if ( thumbnailSize_.isValid() )
        return thumbnailSize_;

    return list_->iconSize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize ListDialog::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 500) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 550) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::widgetRendererAppearanceBase *ListDialog::rendererAppearance() const
{
    if ( mainWindow_->isOperateMode() )
        return _Mybase::rendererAppearance();

    return new hobart::darkOperatorBackgroundWidgetRendererAppearance();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int ListDialog::rendererStyle() const
{
    if ( mainWindow_->isOperateMode() )
        return _Mybase::rendererStyle();

    return hobart::ReadOnly;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ListDialog::addItemToList( const QImage& icon, const QString& text, const QVariant& cookie )
{
    QSize listIconSize( list_->iconSize() );
    QSize iconSize( icon.size() );

    // scale the icon
    iconSize.scale( thumbnailSize(), Qt::KeepAspectRatio );

    QPoint pos(
        (listIconSize.width() - iconSize.width()) / 2,
        (listIconSize.height() - iconSize.height()) / 2 );

    // draw the thumbnail
    QPixmap thumbnail( listIconSize );
    thumbnail.fill( Qt::transparent );

    QPainter painter( &thumbnail );
    painter.drawImage( QRect( pos, iconSize ), icon );

    // create list item
    QListWidgetItem *item = new QListWidgetItem( thumbnail, text );
    item->setData( Qt::UserRole, cookie );

    // add it!
    list_->addItem( item );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ListDialog::addItemToList( const QPixmap& icon, const QString& text, const QVariant& cookie )
{
    addItemToList( icon.toImage(), text, cookie );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ListDialog::addItemToList( const QString& icon, const QString& text, const QVariant& cookie )
{
    QPalette p( list_->palette() );

    // draw the thumbnail
    QPixmap thumbnail( thumbnailSize() );
    thumbnail.fill( Qt::transparent );

    QPainter painter( &thumbnail );
    painter.setFont( list_->font() );
    painter.setPen( p.color( list_->foregroundRole() ) );
    painter.drawText( QRect( QPoint( 0, 0 ), thumbnailSize() ), Qt::AlignCenter, icon );

    addItemToList( thumbnail, text, cookie );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ListDialog::addItemToList( const QString& text, const QVariant& cookie )
{
    // create list item
    QListWidgetItem *item = new QListWidgetItem( text );
    item->setData( Qt::UserRole, cookie );

    // add it!
    list_->addItem( item );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ListDialog::onListItemClicked( QListWidgetItem *item, const QVariant& cookie )
{
    Q_UNUSED( item );
    Q_UNUSED( cookie );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ListDialog::listItemClicked( QListWidgetItem *item )
{
    if ( !item )
        return;

    onListItemClicked( item, item->data( Qt::UserRole ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ListDialog::initialize()
{
    static const QColor selectedBackgroundColor( hobart::colorPalette::blue );

    screenLayout_ = new QHBoxLayout( this );

    screenLayoutInner_ = new QVBoxLayout();
    screenLayoutInner_->setContentsMargins( QMargins() );

    // top layout
    topLayout_ = new QHBoxLayout();
    topLayout_->setContentsMargins( QMargins() );

    // list of items
    list_ = new QListWidget( this );
    list_->setFocusPolicy( Qt::NoFocus );
    list_->setFont( QApplication::font() );
    list_->setObjectName( "list" );
    list_->setPalette( QApplication::palette() );
    list_->setStyleSheet(
        "QListWidget::item { border-bottom: 1px solid black; }"
        "QListWidget::item::selected { background: " + selectedBackgroundColor.name() + "; }" );

    connect( list_, SIGNAL( itemClicked(QListWidgetItem*) ), SLOT( listItemClicked(QListWidgetItem*) ) );

    // bottom layout
    bottomLayout_ = new QHBoxLayout();
    bottomLayout_->setContentsMargins( QMargins() );

    // nest layouts
    screenLayoutInner_->addLayout( topLayout_ );
    screenLayoutInner_->addWidget( list_ );
    screenLayoutInner_->addLayout( bottomLayout_ );

    screenLayout_->addLayout( screenLayoutInner_ );
}
