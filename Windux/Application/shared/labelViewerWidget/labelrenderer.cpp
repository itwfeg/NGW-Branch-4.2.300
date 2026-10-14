/**
 * @file labelrenderer.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "labelrenderer.h"
#include "labelformrenderer.h"

#include <QApplication>
#include <QDrag>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QSignalMapper>

///////////////////////////////////////////////////////////////////////////////////////////////////
LabelRenderer::LabelRenderer( bool readOnly, QWidget *parent ) :
    _Mybase( parent ),
    selected_( NULL ),
    readOnly_( readOnly ),
    allowNutrifactsEditing_( false ),
    previewMode_( false ),
    dataStore_( NULL ),
    nutrifactsId_( 0 )
{
    setAutoFillBackground( true );
    setVisible( false );

    setAcceptDrops( !readOnly_ );

    // allocate signal mapper
    nutrifactLabelFormSignalMapper_ = new QSignalMapper( this );
    connect( nutrifactLabelFormSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onNutrifactTextEdited(QWidget*) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
LabelRenderer::~LabelRenderer()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool LabelRenderer::hasNutrifacts() const
{
    if ( label_ )
    {
        int column;
        QString key;
        QString value;

        // iterate forms
        foreach ( const labelForm& form, (*label_)->forms() )
            if ( isNutrifactLabelForm( form, column, key, value ) )
                return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelRenderer::setLabel( const ::label& value )
{
    LOG_CLASS_INFO( "LabelRenderer", "setLabel" );

    // size the widget
    resize( value.physicalSize() );

    // setup our palette
    QPalette p( palette() );
    p.setColor( backgroundRole(), Qt::white );
    p.setColor( foregroundRole(), Qt::black );
    setPalette( p );

    LOG_INFO( "have new label", NULL );

    // load the label
    label_ = labelPointer( new labelGuard( value ) );

    // nothing currently selected
    selected_ = NULL;

    // allocate nutrifact fields
    allocNutrifactLabelForms();

    // our label has changed
    Q_EMIT labelChanged();

    // only set visible when we have a parent, otherwise you will see graphic artifacts on the
    // screen when embedded labels render
    setVisible( parent() ? true : false );

    // repaint
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelRenderer::setReadOnly( bool value )
{
    readOnly_ = value;
    setAcceptDrops( !readOnly_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelRenderer::resetLabel()
{
    // free nutrifact fields
    freeNutrifactLabelForms();

    // clear out the label
    label_.clear();

    // no longer visible
    setVisible( false );

    // our label has changed
    Q_EMIT labelChanged();

    // repaint
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelRenderer::paintEvent( QPaintEvent *e )
{
    Q_UNUSED( e );

    LOG_CLASS_INFO( "LabelRenderer", "paintEvent" );

    if ( label_.isNull() )
        LOG_TRACE( "nothing to do, no label loaded", NULL );

    // paint!
    else
    {
        QReadLocker lock( &(*label_) );

        // make sure our size is correct
        if ( size() != (*label_)->physicalSize() )
            resize( (*label_)->physicalSize() );

        QPainter painter( this );

        LOG_TRACE( "painting label", NULL );
        render( &painter );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelRenderer::mousePressEvent( QMouseEvent *e )
{
    LOG_CLASS_INFO( "LabelRenderer", "mousePressEvent" );

    if ( label_.isNull() )
    {
        LOG_TRACE( "nothing to do, no label loaded", NULL );
        return;
    }
    else if ( readOnly() )
    {
        LOG_TRACE( "nothing to do, read only", NULL );
        return;
    }
    else if ( Qt::LeftButton != e->button() )
    {
        LOG_TRACE( "nothing to do, unhandled button", NULL );
        return;
    }

    startPos_ = QPoint();

    QReadLocker lock( &(*label_) );

    // figure out which form was clicked
    foreach ( const labelForm& form, (*label_)->forms() )
    {
        QRect formRect( form.rect() );

        // lines use different field for height/width
        if (( labelFormType::USER_LINE == form.type() ) ||
            ( labelFormType::DIVIDING_LINE == form.type() ))
        {
            // line width is stored in sub type... so grab it and adjust the rect size
            int width = form.subType();

            // horizontal line
            if ( 0 == formRect.height() )
                formRect.setHeight( width );
            // vertical line
            else if ( 0 == formRect.width() )
                formRect.setWidth( width );
        }

        // see if form rect contains our mouse click
        if ( formRect.contains( e->pos() ) )
        {
            LOG_DEBUG( "found form! type %d '%s'", form.type(), form.typeText().toLatin1().constData() );

            startPos_ = e->pos();
            selected_ = const_cast<labelForm*>( &form );

            Q_EMIT labelFormSelected();

            update();
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelRenderer::mouseMoveEvent( QMouseEvent *e )
{
    LOG_CLASS_INFO( "LabelRenderer", "mouseMoveEvent" );

    if ( label_.isNull() )
    {
        LOG_TRACE( "nothing to do, no label loaded", NULL );
        return;
    }
    else if ( readOnly() )
    {
        LOG_TRACE( "nothing to do, read only", NULL );
        return;
    }

    if (( Qt::LeftButton & e->buttons() ) && ( !startPos_.isNull() ))
    {
        int distance = (e->pos() - startPos_).manhattanLength();

        // drag initiated
        if (( distance >= QApplication::startDragDistance() ) && ( selected_ ))
        {
            QReadLocker lock( &(*label_) );

            LOG_INFO( "dragging item...", NULL );

            QPoint pos( selected_->rect().topLeft() );
            QSize size( selected_->rect().size() );

            QPixmap pixmap( QSize( size.width() + 1, size.height() + 1 ) );
            pixmap.fill( Qt::white );

            QPainter painter( &pixmap );
            painter.translate( QPoint( -pos.x(), -pos.y() ) );

            labelFormRenderer r( this, selected_ );
            r.paint( &painter );

            QDrag *drag = new QDrag( this );
            drag->setMimeData( labelFormType::toMimeData( selected_->type() ) );
            drag->setPixmap( pixmap );

            lock.unlock();
            drag->exec( Qt::MoveAction );
        }
    }

    _Mybase::mouseMoveEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelRenderer::dragEnterEvent( QDragEnterEvent *e )
{
    LOG_CLASS_INFO( "LabelRenderer", "dragEnterEvent" );

    if ( label_.isNull() )
    {
        LOG_TRACE( "nothing to do, no label loaded", NULL );
        return;
    }
    else if ( readOnly() )
    {
        LOG_TRACE( "nothing to do, read only", NULL );
        return;
    }

    LOG_TRACE( "checking for valid drag...", NULL );

    if ( labelFormType::isTypeOfMimeData( e->mimeData() ) )
    {
        LOG_INFO( "somebody is dragging a form on us", NULL );
        e->acceptProposedAction();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelRenderer::dropEvent( QDropEvent *e )
{
    LOG_CLASS_INFO( "LabelRenderer", "dropEvent" );

    if ( label_.isNull() )
    {
        LOG_TRACE( "nothing to do, no label loaded", NULL );
        return;
    }
    else if ( readOnly() )
    {
        LOG_TRACE( "nothing to do, read only", NULL );
        return;
    }

    LOG_TRACE( "checking for valid drop...", NULL );

    if (( Qt::MoveAction & e->dropAction() ) || ( Qt::CopyAction & e->dropAction() ))
        LOG_TRACE( "valid action %d", e->dropAction() );
    else
    {
        LOG_WARN( "invalid action %d!", e->dropAction() );
        return;
    }

    labelFormType::type type;

    // check for mime data is type of label form
    if ( labelFormType::isTypeOfMimeData( e->mimeData(), &type ) )
    {
        // accept the drop
        e->accept();

        // copy the object being dragged
        if ( Qt::CopyAction & e->dropAction() )
        {
            QWriteLocker lock( &(*label_) );
            label_->setDirtyFlag( true );

            // create a form
            labelForm form( type );

            QRect rect( form.rect() );
            rect.moveTo( e->pos() );
            form.setRect( rect );

            // add it to our list
            (*label_)->forms().push_back( form );

            // fire off an event
            selected_ = &(*label_)->forms().last();

            lock.unlock();
            Q_EMIT labelFormSelected();
        }

        // move the object being dragged
        else if ( Qt::MoveAction & e->dropAction() )
        {
            QWriteLocker lock( &(*label_) );
            label_->setDirtyFlag( true );

            QRect rect( selected_->rect() );
            rect.moveTo( e->pos() );
            selected_->setRect( rect );
        }

        // repaint
        update();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QImage LabelRenderer::toImage() const
{
    LOG_CLASS_INFO( "LabelRenderer", "toImage" );

    if ( label_.isNull() )
    {
        LOG_TRACE( "nothing to do, no label loaded", NULL );
        return QImage();
    }

    QReadLocker lock( &(*label_) );

    QImage image( (*label_)->physicalSize(), QImage::Format_RGB32 );
    QPainter painter( &image );

    LOG_TRACE( "rendering image...", NULL );

    image.fill( Qt::white );
    render( &painter );

    return image;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelRenderer::render( QPainter *painter ) const
{
    QReadLocker lock( &(*label_) );

    // paint forms
    foreach ( const labelForm& form, (*label_)->forms() )
    {
        labelFormRenderer r( this, &form );

        if ( isPreviewMode() )
            r.setMode( labelFormRenderer::PREVIEW );

        r.paint( painter, (selected_ == &form) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelRenderer::onNutrifactTextEdited( QWidget *w )
{
    QLineEdit *lineEdit;

    // sanity check
    if ( !w )
        return;
    else if ( !(lineEdit = dynamic_cast<QLineEdit*>( w )) )
        return;
    else if ( !nutrifactLabelForms_.contains( lineEdit ) )
        return;

    // get column and key from map
    columnKeyPair p = nutrifactLabelForms_[lineEdit];

    int column = p.first;
    QString key = p.second;
    QString value = lineEdit->text();

    if ( key.isEmpty() )
        return;

    // update the store with the modified value
    if ( store() )
    {
        dataStoreBase *s = const_cast<dataStoreBase*>( store() );
        dataObjectContainer<nutrifact> *c = s->store<nutrifact>();

        // retrieve nutrifacts object
        int keyInt = nutrifactsId();

        if ( c->contains( keyInt ) )
        {
            nutrifact n( c->value( keyInt ) );

            // lookup nutrifact value
            if ( n.columns().contains( column ) )
            {
                // modify the value
                nutrifactColumn data( n.column( column ) );
                data[key] = value;

                // save
                n.setColumn( column, data );
            }

            // save
            c->insert( n );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelRenderer::allocNutrifactLabelForms()
{
    LOG_CLASS_INFO( "LabelRenderer", "allocNutrifactLabelForms" );

    LOG_TRACE( "alloc nutrifact label forms...", NULL );

    // free existing
    freeNutrifactLabelForms();

    // nothing to do if not allowed
    // nothing to do if no nutrifacts
    if ( !isNutrifactsEditingAllowed() )
        return;
    else if ( !hasNutrifacts() )
        return;

    // create widgets
    foreach ( const labelForm& form, (*label_)->forms() )
    {
        int column;
        QString key;
        QString value;

        // check for nutrifact form type
        if ( isNutrifactLabelForm( form, column, key, value ) )
            if ( key.length() )
            {
                QLineEdit *w = new QLineEdit( this );
                w->setGeometry( form.rect() );
                w->setObjectName( key );
                w->setText( value );

                nutrifactLabelFormSignalMapper_->setMapping( w, w );
                connect( w, SIGNAL( textEdited(QString) ), nutrifactLabelFormSignalMapper_, SLOT( map() ) );

                nutrifactLabelForms_.insert( w, columnKeyPair( column, key ) );
            }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelRenderer::freeNutrifactLabelForms()
{
    LOG_CLASS_INFO( "LabelRenderer", "freeNutrifactLabelForms" );

    LOG_DEBUG( "free nutrifact label forms...", NULL );

    // destroy each widget
    for ( nutrifactLabelFormMap::iterator i = nutrifactLabelForms_.begin(); i != nutrifactLabelForms_.end(); ++i )
    {
        QLineEdit *w = i.key();

        w->hide();
        w->deleteLater();
    }

    // clear the map
    nutrifactLabelForms_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool LabelRenderer::isNutrifactLabelForm( const labelForm& form, int& column, QString &key, QString& value ) const
{
    bool result = false;

    labelFormType::type t = form.type();

    // check for nutrifact columns
    if (( labelFormType::NUTRI_COLUMN_1 == t ) || ( labelFormType::NUTRI_COLUMN_2 == t ) ||
        ( labelFormType::NUTRI_COLUMN_3 == t ) || ( labelFormType::NUTRI_COLUMN_4 == t ))
    {
        column = t - labelFormType::NUTRI_COLUMN_1;

        // sanity check
        if ( store() )
        {
            const dataObjectContainer<nutrifact> *c = store()->store<nutrifact>();

            // retrieve nutrifacts object
            int keyInt = nutrifactsId();

            if ( c->contains( keyInt ) )
            {
                nutrifact n( c->value( keyInt ) );

                // lookup nutrifact value
                if ( n.columns().contains( column ) )
                {
                    key = form.tag();

                    if ( n.column( column ).contains( key ) )
                        value = n.column( column ).value( key );

                    result = true;
                }
            }
        }
    }

    return result;
}

