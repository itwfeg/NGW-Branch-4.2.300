/**
 * @file labeledarraywidget.h
 * Array of widgets with custom background, highlighting, and label.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LABELEDARRAYWIDGET_H
#define LABELEDARRAYWIDGET_H

#include "labeledwidget.h"

#include <QWidget>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class arrayWidget
 * @brief Widget that contains an array of children widgets.
 *
 * @tparam WIDGET  widget type
 */
template <class WIDGET>
class arrayWidget : public QWidget
{
    typedef arrayWidget<WIDGET> _Myt;
    typedef QWidget _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit arrayWidget( QWidget *parent = 0 ) : _Mybase( parent ), readOnly_( false )
    {
        QPalette p( palette() );
        p.setBrush( backgroundRole(), QBrush( Qt::transparent ) );
        setPalette( p );
    }

    /**
     * Destructor.
     */
    virtual ~arrayWidget() {}

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Check if widget is read only.
     *
     * @returns  @c true if read only, @c false otherwise
     */
    virtual bool isReadOnly() const {return readOnly_;}

    /**
     * Set if control is read only.
     *
     * @param[in] value  @c true for read only, @c false for otherwise
     */
    virtual void setReadOnly( bool value )
    {
        readOnly_ = value;

        foreach ( WIDGET *widget, array_ )
            widget->setReadOnly( readOnly_ );

        update();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Initialize widget array.
     *
     * @param[in] count  number of widgets
     */
    virtual void initializeArray( int count )
    {
        QVBoxLayout *layout = new QVBoxLayout( this );
        layout->setContentsMargins( QMargins() );

        while ( count-- )
        {
            layout->insertWidget( 0, array_[count] = new WIDGET( this ) );

            QString objectName( "arrayWidget%1" );
            array_[count]->setObjectName( objectName.arg( count ) );
        }
    }

    /**
     * Retrieve child widget at index.
     *
     * @param[in] index  index of child widget
     */
    virtual WIDGET *widgetAt( int index ) {return array_[index];}

protected:

    /**
     * Map of widgets.
     */
    typedef QMap<int, WIDGET*> widgetMap;

    widgetMap array_;                               ///< array of child widgets

private:

    bool readOnly_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class labeledArrayWidget
 * @brief Array of widgets with custom background, highlighting, and label.
 *
 * @tparam WIDGET  widget type
 */
template <class WIDGET>
class labeledArrayWidget : public labeledWidget<arrayWidget<WIDGET> >
{
    typedef labeledArrayWidget<WIDGET> _Myt;
    typedef labeledWidget<arrayWidget<WIDGET> > _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // DTOR

    /**
     * Destructor.
     */
    virtual ~labeledArrayWidget() {}

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve array size.
     *
     * @returns  size of array
     */
    virtual int arraySize() const {return arraySize_;}

    /**
     * Retrieve child widget at index.
     *
     * @param[in] index  index of child widget
     */
    virtual WIDGET *widgetAt( int index ) {return _Mybase::child_->widgetAt( index );}

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Translate the widget fields.
     */
    virtual void translate()
    {
        int i = arraySize_;

        while ( i-- )
            _Mybase::child_->widgetAt( i )->translate();

        _Mybase::translate();
        _Mybase::update();
    }

protected:

    int arraySize_;                                 ///< How many widgets in array.

    ///////////////////////////////////////////////////////////////////////////
    // CTOR

    /**
     * Constructor.
     *
     * @param[in] count  count of widgets in array
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     */
    explicit labeledArrayWidget( int count, QWidget *parent = 0, const QPixmap& image = QPixmap() ) :
        _Mybase( parent, image ), arraySize_( count )
    {
         _Mybase::child_->initializeArray( arraySize_ );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // LABELEDARRAYWIDGET_H
