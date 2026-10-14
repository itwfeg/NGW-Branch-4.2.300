/**
 * @file tabbableiplineedit.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Todd Davis
 */

#include <logmanager.h>

#include "tabbableiplineedit.h"

#include <QIntValidator>
#include <QKeyEvent>
#include <QMutex>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
tabbableIPLineEdit::tabbableIPLineEdit( QWidget *parent ) :
    _Mybase( parent ),
    decimalPlaces_( 0 )
{
    setObjectName( "hobartTabbableIPLineEdit" );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tabbableIPLineEdit::tabbableIPLineEdit( int decimalPlaces, QWidget *parent ) :
    _Mybase( parent ),
    decimalPlaces_( decimalPlaces )
{
    setObjectName( "hobartTabbableIPLineEdit" );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tabbableIPLineEdit::~tabbableIPLineEdit()
{
    if ( mutex_ )
        delete mutex_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tabbableIPLineEdit::fromInt( int value )
{
    LOG_CLASS_INFO( "tabbableIPLineEdit", "fromInt" );

    const QIntValidator *intValidator;
    QString s;

    LOG_TRACE( "setting value to %d", value );

    // validate
    if ( (intValidator = validator()) )
    {
        if ( value < intValidator->bottom() )
            value = intValidator->bottom();
        else if ( intValidator->top() < value )
        {
            // Note:
            // Going above the top value with input wrapping behaves a little differently... the
            // value should be truncated until it is less than to the top value.
            //
            // For example, suppose my top is 23 and I currently have 14 in the widget. Any digit
            // I type will result in 14X (where X is the digit I typed). 4X is larger than the
            // top value of 23 so we should truncate again down to just X.

            if ( !inputWrapping() )
                value = intValidator->top();
            else
            {
                do
                {
                    // loop until our value is less than top
                    QString temp( QString::number( value ) );
                    value = temp.right( temp.length() - 1 ).toInt();

                } while (( 0 < value ) && ( intValidator->top() < value ));
            }
        }
    }

    // set text
    if ( !decimalPlaces() )
        setText( s.sprintf( format().toLatin1().constData(), value ) );
    else
    {
        int loops = decimalPlaces();
        int split = 10;

        // calculate split
        while ( --loops )
            split *= 10;

        // format number string
        if ( 0 <= value )
            s.sprintf( format().toLatin1().constData(), value / split, value % split );
        else
        {
            value = qAbs( value );
            s.sprintf( format().toLatin1().constData(), value / split, value % split );
            s.prepend( "-" );
        }

        // set!
        setText( s );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tabbableIPLineEdit::fromSessionField( const QString& value )
{
    LOG_CLASS_INFO( "tabbableIPLineEdit", "fromSessionField" );

    QString text( value );
    int textLength = text.length();

    LOG_TRACE( "setting value to '%s'", qPrintable( text ) );

    // empty string
    if ( !textLength )
    {
        setText( text );
        return;
    }

    int textIndexOfSeperator = text.indexOf( "!" );

    // strip display value
    if ( 0 < textIndexOfSeperator )
    {
        text = text.left( textIndexOfSeperator );
        textLength = text.length();
    }

    int textIndexOfDecimal = text.indexOf( "." );

    // set decimal places
    if ( 0 < textIndexOfDecimal )
    {
        setDecimalPlaces( textLength - textIndexOfDecimal - 1 );
        text.replace( ".", "" );
    }

    // set value
    fromInt( text.toInt() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tabbableIPLineEdit::fromSessionField( const QString& value, no_numeric_ip_validation_t )
{
    LOG_CLASS_INFO( "tabbableIPLineEdit", "fromSessionField" );

    QString text( value );
    int textLength = text.length();

    // empty string
    if ( !textLength )
    {
        setText( text );
        return;
    }

    bool isValid = true;

    // check validity
    for ( int i = 0; i < textLength; ++i )
    {
        QChar c( text[i] );

        if (( c < QChar( '0' ) ) || ( QChar( '9' ) < c ))
            if ( QChar( '.' ) != c )
            {
                isValid = false;
                break;
            }
    }

    // not valid, set directly
    if ( !isValid )
    {
        LOG_TRACE( "setting value to '%s' (non-numeric)", qPrintable( text ) );

        setText( text );
        return;
    }

    // set normally
    fromSessionField( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool tabbableIPLineEdit::inputLeadingZeros() const
{
    return (( leadingZeros_ ) && ( inputWrapping() ) && ( !decimalPlaces() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tabbableIPLineEdit::setDecimal( const QString& value )
{
    LOG_CLASS_INFO( "tabbableIPLineEdit", "setDecimal" );

    // nothing to do
    if ( value == decimal_ )
        return;

    LOG_TRACE( "setting decimal to '%s'", qPrintable( value ) );

    // when empty just set property
    if ( text().isEmpty() )
        decimal_ = value;

    else
    {
        int temp = toInt();
        decimal_ = value;
        fromInt( temp );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tabbableIPLineEdit::setDecimalPlaces( int value )
{
    LOG_CLASS_INFO( "tabbableIPLineEdit", "setDecimal" );

    // nothing to do
    if ( value == decimalPlaces_ )
        return;

    // update validator top to reflect new top
    if ( validator_ )
    {
        QIntValidator *v = const_cast<QIntValidator*>( validator_ );
        int oldValue = decimalPlaces_;

        // more decimal places
        while ( oldValue < value )
        {
            int oldTop( v->top() );
            int newTop( oldTop * 10 );

            // FT-273: make sure we are increasing top
            if ( oldTop < newTop )
                v->setTop( newTop * 10 );

            ++oldValue;
        }

        // less decimal places
        while ( value < oldValue )
        {
            v->setTop( v->top() / 10 );
            --oldValue;
        }
    }

    LOG_TRACE( "setting number of decimal places to %d", value );

    // when empty just set property
    if ( text().isEmpty() )
        decimalPlaces_ = value;

    else
    {
        int temp = toInt();
        decimalPlaces_ = value;
        fromInt( temp );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tabbableIPLineEdit::setPrefix( const QString& value )
{
    LOG_CLASS_INFO( "tabbableIPLineEdit", "setPrefix" );

    // nothing to do
    if ( value == prefix_ )
        return;

    LOG_TRACE( "setting prefix to '%s'", qPrintable( value ) );

    // when empty just set property
    if ( text().isEmpty() )
        prefix_ = value;

    else
    {
        int temp = toInt();
        prefix_ = value;
        fromInt( temp );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tabbableIPLineEdit::setSuffix( const QString& value )
{
    LOG_CLASS_INFO( "tabbableIPLineEdit", "setSuffix" );

    // nothing to do
    if ( value == suffix_ )
        return;

    LOG_TRACE( "setting suffix to '%s'", qPrintable( value ) );

    // when empty just set property
    if ( text().isEmpty() )
        suffix_ = value;

    else
    {
        int temp = toInt();
        suffix_ = value;
        fromInt( temp );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tabbableIPLineEdit::setValidator( const QIntValidator *value )
{
    // nothing to do
    if ( value == validator_ )
        return;

    // when empty just set property
    if ( text().isEmpty() )
        validator_ = value;

    else
    {
        int temp = toInt();
        validator_ = value;
        fromInt( temp );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const QIntValidator *tabbableIPLineEdit::validator() const
{
    if ( !userTextInput_ )
        return NULL;

    return validator_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tabbableIPLineEdit::resetInputTruncationFlag()
{
    firstEntry_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int tabbableIPLineEdit::toInt() const
{
    return convertToInt( text() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString tabbableIPLineEdit::toSessionField() const
{
    QString value( strip( text() ) );

    value.replace( decimal(), "." );

    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tabbableIPLineEdit::setText( const QString& value )
{
    LOG_CLASS_INFO( "tabbableIPLineEdit", "setText" );

    bool locked = mutex_->tryLock();

    LOG_TRACE( "setting value to '%s'", qPrintable( value ) );

    // do not allow programmatic modification if currently editing
    if (( locked ) && ( hasFocus() && !allowUpdatesWhileFocused_ ))
        LOG_DEBUG( "not setting due to focus %d (locked %d)", hasFocus(), locked );

    else
    {
        // set!
        _Mybase::setText( decorate( value ) );
    }

    // unlock mutex
    if ( locked )
    {
        // force cursor position
        forceCursorPosition();

        // done
        mutex_->unlock();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool tabbableIPLineEdit::event( QEvent *e )
{
    if (( QEvent::KeyPress == e->type() ) || ( QEvent::KeyRelease == e->type() ))
    {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>( e );

        if ( keyEvent )
        {
            bool isValidKeyEvent = false;

            // allowed keys
            if (( Qt::Key_0 <= keyEvent->key() ) && ( keyEvent->key() <= Qt::Key_9 ))
                isValidKeyEvent = true;
            else if (( Qt::Key_Return == keyEvent->key() ) || ( Qt::Key_Enter == keyEvent->key() ))
                isValidKeyEvent = true;
            else if ( Qt::Key_Backspace == keyEvent->key() )
                isValidKeyEvent = true;
            else if (Qt::Key_Tab == keyEvent->key() )
                isValidKeyEvent = true;
            else if (Qt::Key_Period == keyEvent->key())
            {
                isValidKeyEvent = true;
            }
            // denied!
            if ( !isValidKeyEvent )
                return true;
        }
    }

    // invoke base method
    return _Mybase::event( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tabbableIPLineEdit::focusInEvent( QFocusEvent *event )
{
    LOG_CLASS_INFO( "tabbableIPLineEdit", "focusInEvent" );

    // set zero value when empty
    if (( !text().length() ) && ( inputFocusZeroing() ))
    {
        // allow temporary updates
        bool saved = allowUpdatesWhileFocused_;

        LOG_DEBUG( "zeroing value...", NULL );

        allowUpdatesWhileFocused_ = true;
        fromInt( 0 );
        allowUpdatesWhileFocused_ = saved;
    }

    LOG_TRACE( "focus in event 0x%08x [%s]", this, qPrintable( objectName() ) );

    // call base method
    _Mybase::focusInEvent( event );

    // force the cursor position
    forceCursorPosition();

    // typing something will be the first entry into this widget
    firstEntry_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tabbableIPLineEdit::focusOutEvent( QFocusEvent *event )
{
    LOG_CLASS_INFO( "tabbableIPLineEdit", "focusOutEvent" );

    LOG_TRACE( "focus out event 0x%08x [%s]", this, qPrintable( objectName() ) );

    // call base method
    _Mybase::focusOutEvent( event );

    // we are no longer focused
    firstEntry_ = false;

    // editing finished
    emitEditingFinished();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tabbableIPLineEdit::keyPressEvent( QKeyEvent *event )
{
    LOG_CLASS_INFO( "tabbableIPLineEdit", "keyPressEvent" );

    bool isNumericKeyPress = (( Qt::Key_0 <= event->key() ) && ( event->key() <= Qt::Key_9 ));

    LOG_TRACE( "key press event", NULL );

    // set user input flag
    userTextInput_ = isNumericKeyPress;

    // when truncation enabled clear out existing text first
    if (( inputTruncation() ) && ( hasFocus() ) && ( firstEntry_ ))
    {
        if ( isNumericKeyPress )
        {
            firstEntry_ = false;
            clear();
        }
        else if ( Qt::Key_Backspace == event->key() )
        {
            // allow backspace without truncating field
            firstEntry_ = false;
        }
    }

    // call base method
    _Mybase::keyPressEvent( event );

    // if this was the return key emit editing finished
    if (( Qt::Key_Return == event->key() ) || (  Qt::Key_Enter == event->key() ))
        emitEditingFinished();
    else if(Qt::Key_Period == event->key())
    {
        emit focusNextChild();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tabbableIPLineEdit::onCursorPositionChanged( int oldPos, int newPos )
{
    Q_UNUSED( oldPos );
    Q_UNUSED( newPos );

    if ( !mutex_->tryLock() )
        return;

    // force the cursor position
    forceCursorPosition();

    // done
    mutex_->unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tabbableIPLineEdit::onTextChanged( const QString& text )
{
    LOG_CLASS_INFO( "tabbableIPLineEdit", "onTextChanged" );

    LOG_TRACE( "on text changed '%s'", qPrintable( text ) );

    // nothing to do for empty string
    if ( text.isEmpty() )
        return;

    if ( !mutex_->tryLock() )
        return;

    QString textToConvert( text );

    // when allowed leading zeros set a fixed length format
    if ( inputLeadingZeros() )
        fixedLengthFormat_ = textToConvert.length();

    // handle input precision
    if (( 0 <= inputPrecision() ) && ( inputPrecision() < decimalPlaces() ))
    {
        // there are two cases to handle:
        // 1) first digit entry into the widget
        // 2) n^th digit entry into the widget

        // this is case (1) above
        // first digit has been entered and the widget text was truncated down to just that
        // keyed digit... for this case we simply pad the entry out to precision
        if ( 1 == textToConvert.length() )
        {
            int current = inputPrecision();

            while ( current++ < decimalPlaces() )
                textToConvert.append( "0" );
        }

        // this is case (2) above
        else
        {
            // nothing to do
            // input should have occured at proper location
        }
    }

    // convert!
    QString value( QString::number( convertToInt( textToConvert ) ) );

    // when wrapping input lop off excess digits
    if ( inputWrapping() )
    {
        const QIntValidator *intValidator;

        if ( (intValidator = validator()) )
        {
            QString top( QString::number( intValidator->top() ) );

            if ( top.length() < value.length() )
                value = value.right( top.length() );

            fixedLengthFormat_ = qMin( fixedLengthFormat_, top.length() );
        }
    }

    // set text!
    fromInt( value.toInt() );

    // force cursor position
    forceCursorPosition();

    // done handling input
    userTextInput_ = false;

    // done
    mutex_->unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tabbableIPLineEdit::initialize()
{
    // install validator on base object
    _Mybase::setValidator( new QIntValidator( this ) );

    // set default values
    decimal_ = ".";
    validator_ = (defaultValidator_ = new QIntValidator( 0, INT_MAX, this ));
    wrap_ = false;
    leadingZeros_ = false;
    truncate_ = false;
    zeroEmpty_ = true;
    precision_ = -1;
    allowUpdatesWhileFocused_ = false;

    fixedLengthFormat_ = 0;
    firstEntry_ = false;
    userTextInput_ = false;
    mutex_ = new QMutex();

    // connect slots
    connect( this, SIGNAL( cursorPositionChanged(int,int) ), SLOT( onCursorPositionChanged(int,int) ) );
    connect( this, SIGNAL( textChanged(QString) ), SLOT( onTextChanged(QString) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString tabbableIPLineEdit::format() const
{
    QString numberFormat( "%d" );

    if ( fixedLengthFormat_ )
        numberFormat.insert( 1, "0" + QString::number( fixedLengthFormat_ ) );

    if ( decimalPlaces() )
        numberFormat += decimal() + "%0" + QString::number( decimalPlaces() ) + "d";

    return numberFormat;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString tabbableIPLineEdit::decorate( const QString& text ) const
{
    QString value( text );

    // do not decorate empty string
    if ( !value.isEmpty() )
    {
        const QString p( prefix() );
        const int pl = p.length();

        const QString s( suffix() );
        const int sl = s.length();

        // add prefix
        if (( pl ) && ( p != value.left( pl ) ))
            value = p + value;

        // add suffix
        if (( sl ) && ( s != value.right( sl ) ))
            value = value + s;
    }

    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString tabbableIPLineEdit::strip( const QString& text ) const
{
    QString value( text );

    // nothing to do for empty string
    if ( !value.isEmpty() )
    {
        const QString p( prefix() );
        const int pl = p.length();

        const QString s( suffix() );
        const int sl = s.length();

        // strip prefix
        if (( pl ) && ( p == value.left( pl ) ))
            value = value.right( value.length() - pl );

        // strip suffix
        if (( sl ) && ( s == value.right( sl ) ))
            value = value.left( value.length() - sl );
    }

    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int tabbableIPLineEdit::convertToInt( const QString& text ) const
{
    QString value( strip( text ) );

    // get rid of decimal places
    if ( decimalPlaces() )
        value.replace( decimal(), "" );

    return (value.length() ? value.toInt() : 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tabbableIPLineEdit::forceCursorPosition()
{
    int pos = text().length() - suffix().length();

    if ( 0 == inputPrecision() )
        pos -= decimal().length() - decimalPlaces();
    else if (( 0 < inputPrecision() ) && ( inputPrecision() <= decimalPlaces() ))
        pos -= (decimalPlaces() - inputPrecision());

    // force cursor position to end of numeric entry
    if ( pos >= 0 )
        setCursorPosition( pos );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tabbableIPLineEdit::emitEditingFinished()
{
    LOG_CLASS_INFO( "tabbableIPLineEdit", "emitEditingFinished" );

    // when there is a validator installed the editingFinished() will not be emitted due to all
    // decoration in the string (i.e. its not an integer)... so here we emit manually...
    if ( !_Mybase::validator() )
        return;

    QString input( text() );
    int pos = 0;

    // when input passes validation the signal was emitted by base line edit class
    if ( QValidator::Acceptable == _Mybase::validator()->validate( input, pos ) )
        return;

    LOG_DEBUG( "emitting editing finished signal...", NULL );
    Q_EMIT editingFinished();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

