/**
 * @file numericlineedit.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "numericlineedit.h"

#include <QIntValidator>
#include <QKeyEvent>
#include <QMutex>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
numericLineEdit::numericLineEdit( QWidget *parent ) :
    _Mybase( parent ),
    decimalPlaces_( 0 )
{
    setObjectName( "hobartNumericLineEdit" );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
numericLineEdit::numericLineEdit( int decimalPlaces, QWidget *parent ) :
    _Mybase( parent ),
    decimalPlaces_( decimalPlaces )
{
    setObjectName( "hobartNumericLineEdit" );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
numericLineEdit::~numericLineEdit()
{
    if ( mutex_ )
        delete mutex_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void numericLineEdit::fromInt( int value )
{
    LOG_CLASS_INFO( "numericLineEdit", "fromInt" );

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

        // FT-1105: do not allow division by zero
        if ( 0 == split )
            split = 1;

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
void numericLineEdit::fromSessionField( const QString& value )
{
    LOG_CLASS_INFO( "numericLineEdit", "fromSessionField" );

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
void numericLineEdit::fromSessionField( const QString& value, no_numeric_validation_t )
{
    LOG_CLASS_INFO( "numericLineEdit", "fromSessionField" );

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
bool numericLineEdit::inputLeadingZeros() const
{
    return (( leadingZeros_ ) && ( inputWrapping() ) && ( !decimalPlaces() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void numericLineEdit::setDecimal( const QString& value )
{
    LOG_CLASS_INFO( "numericLineEdit", "setDecimal" );

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
void numericLineEdit::setDecimalPlaces( int value )
{
    LOG_CLASS_INFO( "numericLineEdit", "setDecimal" );

    // nothing to do
    if ( value == decimalPlaces_ )
        return;

    // update validator top to reflect new top
    if ( validator_ )
    {
        QIntValidator *v( const_cast<QIntValidator*>( validator_ ) );
        int oldValue( decimalPlaces_ );

        // more decimal places
        while ( oldValue < value )
        {
            int oldTop( v->top() );
            int newTop( oldTop * 10 );

            if ( 9 == (oldTop % 10))
                newTop += 9;

            // FT-273: make sure we are increasing top
            if ( oldTop < newTop )
                v->setTop( newTop );

            ++oldValue;
        }

        // less decimal places
        while ( value < oldValue )
        {
            int oldTop( v->top() );
            int newTop( oldTop / 10 );

            // HA-96: make sure we are decreasing top
            if ( newTop < oldTop  )
                v->setTop( newTop );

            --oldValue;
        }
    }

    // FT-1105: limit how many decimal places are allowed
    if ( MAX_DECIMAL_PLACES < value )
        value = MAX_DECIMAL_PLACES;

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
void numericLineEdit::setPrefix( const QString& value )
{
    LOG_CLASS_INFO( "numericLineEdit", "setPrefix" );

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
void numericLineEdit::setSuffix( const QString& value )
{
    LOG_CLASS_INFO( "numericLineEdit", "setSuffix" );

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
void numericLineEdit::setValidator( const QIntValidator *value )
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
const QIntValidator *numericLineEdit::validator() const
{
    if ( !userTextInput_ )
        return NULL;

    return validator_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void numericLineEdit::resetInputTruncationFlag()
{
    firstEntry_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int numericLineEdit::toInt() const
{
    return convertToInt( text() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString numericLineEdit::toSessionField() const
{
    QString value( strip( text() ) );

    value.replace( decimal(), "." );

    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void numericLineEdit::setText( const QString& value )
{
    LOG_CLASS_INFO( "numericLineEdit", "setText" );

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
bool numericLineEdit::event( QEvent *e )
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

            // denied!
            if ( !isValidKeyEvent )
                return true;
        }
    }

    // invoke base method
    return _Mybase::event( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void numericLineEdit::focusInEvent( QFocusEvent *event )
{
    LOG_CLASS_INFO( "numericLineEdit", "focusInEvent" );

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
void numericLineEdit::focusOutEvent( QFocusEvent *event )
{
    LOG_CLASS_INFO( "numericLineEdit", "focusOutEvent" );

    LOG_TRACE( "focus out event 0x%08x [%s]", this, qPrintable( objectName() ) );

    // call base method
    _Mybase::focusOutEvent( event );

    // we are no longer focused
    firstEntry_ = false;

    // editing finished
    emitEditingFinished();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void numericLineEdit::keyPressEvent( QKeyEvent *event )
{
    LOG_CLASS_INFO( "numericLineEdit", "keyPressEvent" );

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
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void numericLineEdit::onCursorPositionChanged( int oldPos, int newPos )
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
void numericLineEdit::onTextChanged( const QString& text )
{
    LOG_CLASS_INFO( "numericLineEdit", "onTextChanged" );

    LOG_TRACE( "on text changed '%s'", qPrintable( text ) );

    // nothing to do for empty string
    if ( text.isEmpty() )
        return;

    if ( !mutex_->tryLock() )
        return;

    const QIntValidator *intValidator( validator() );

    QString textToConvert( text );

    // when allowed leading zeros set a fixed length format
    if ( inputLeadingZeros() )
    {
        fixedLengthFormat_ = textToConvert.length();

        // HA-14 show leading zeros
        if ( intValidator )
        {
            QString top( QString::number( intValidator->top() ) );

            fixedLengthFormat_ = qMax( fixedLengthFormat_, top.length() );
        }
    }

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
    if (( inputWrapping() ) && ( intValidator ))
    {
        QString top( QString::number( intValidator->top() ) );

        if ( top.length() < value.length() )
            value = value.right( top.length() );

        fixedLengthFormat_ = qMin( fixedLengthFormat_, top.length() );
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
void numericLineEdit::initialize()
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
QString numericLineEdit::format() const
{
    QString numberFormat( "%d" );

    if ( fixedLengthFormat_ )
        numberFormat.insert( 1, "0" + QString::number( fixedLengthFormat_ ) );

    if ( decimalPlaces() )
        numberFormat += decimal() + "%0" + QString::number( decimalPlaces() ) + "d";

    return numberFormat;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString numericLineEdit::decorate( const QString& text ) const
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
QString numericLineEdit::strip( const QString& text ) const
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
int numericLineEdit::convertToInt( const QString& text ) const
{
    QString value( strip( text ) );

    // get rid of decimal places
    if ( decimalPlaces() )
        value.replace( decimal(), "" );

    return (value.length() ? value.toInt() : 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void numericLineEdit::forceCursorPosition()
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
void numericLineEdit::emitEditingFinished()
{
    LOG_CLASS_INFO( "numericLineEdit", "emitEditingFinished" );

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
