/**
 * @file graphicchooserdialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "graphicchooserdialog.h"

#include "../sessionmainwindow.h"

#include <font.h>
#include <graphic.h>
#include <logmanager.h>

#include <ScaleConfiguration.h>

#include <QListWidget>

const QString GraphicChooserDialog::ext_( ".png" );

const QSize GraphicChooserDialog::iconSize_( 128, 64 );
const QSize GraphicChooserDialog::iconSizeList_( 160, 64 );

typedef dataObjectContainer<graphic> graphicContainer;

///////////////////////////////////////////////////////////////////////////////////////////////////
GraphicChooserDialog::GraphicChooserDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( parent, f )
{
    setObjectName( "GraphicChooserDialog" );

    // initialize interface
    initialize();

    // populate list

    // first item is "none"
    _Mybase::addItemToList( QPixmap(), tr( "(none)" ), QString( "" ) );

    // remaining items come from session
    if ( session_ )
    {
        const graphicContainer *graphics( session_->scaleConfig->dataStore_.store<graphic>() );

        for ( graphicContainer::const_iterator i = graphics->begin(); i != graphics->end(); ++i )
            addItemToList( *i );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
GraphicChooserDialog::~GraphicChooserDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString GraphicChooserDialog::selectedGraphic() const
{
    QString name( selected_ );

    // no extension or wrong extension
    if ( name.length() )
        if (( name.length() < 4 ) || ( 0 != QString::compare( name.right( 4 ), ext_, Qt::CaseInsensitive ) ))
            name.append( ext_ );

    return name;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicChooserDialog::setSelectedGraphic( const QString& value )
{
    QString name( value );

    // check for file extension
    if ( 4 < name.length() )
        if ( 0 == QString::compare( name.right( 4 ), ext_, Qt::CaseInsensitive ) )
            name = name.left( name.length() - 4 );

    // find our graphic
    for ( int i = 0; i < list_->count(); ++i )
    {
        QListWidgetItem *item = list_->item( i );

        // found!!
        if ( name == item->data( Qt::UserRole ).toString() )
            list_->setCurrentItem( item );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicChooserDialog::addItemToList( const graphic& g )
{
    _Mybase::addItemToList( g.image(), g.name(), g.name() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicChooserDialog::onListItemClicked( QListWidgetItem *item, const QVariant& cookie )
{
    if ( !item )
        return;

    selected_ = cookie.toString();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicChooserDialog::initialize()
{
    modifyFontPointSize setFontPointSize12( (mainWindow_->height() * 12) / SessionMainWindow::scalingDividerY_ );

    list_->setIconSize( iconSizeList_ );
    setFontPointSize12( list_ );

    setThumbnailSize( iconSize_ );
}


