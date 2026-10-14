/**
 * @file marqueetext.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "marqueetext.h"
#include "marqueetextconstants.h"

#include <QPainter>

///////////////////////////////////////////////////////////////////////////////////////////////////
QString marqueeText::text() const
{
    QString result;

    for ( const_iterator i = begin(); i != end(); ++i )
        result += i->text();

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void marqueeText::setText( const QString& value, const QFontMetrics& m )
{
    // destroy current text
    clear();

    // text contains no markup (easy case)
    if ( !value.contains( BLINK_PREFIX, Qt::CaseInsensitive ) )
        push_back( value_type( value, m, false ) );

    // text has markup
    else
    {
        QList<int> prefixOffsets;
        QList<int> suffixOffsets;
        int offset;

        // create list of prefix offsets
        offset = -1;
        while ( -1 != (offset = value.indexOf( BLINK_PREFIX, ++offset, Qt::CaseInsensitive )) )
            prefixOffsets.push_back( offset );

        // create list of suffix offsets
        offset = -1;
        while ( -1 != (offset = value.indexOf( BLINK_SUFFIX, ++offset, Qt::CaseInsensitive )) )
            suffixOffsets.push_back( offset );

        // add text segment into list
        offset = 0;
        for ( int i = 0; (( i < prefixOffsets.size() ) && ( i < suffixOffsets.size() )); ++i )
        {
            // compute offset and length of non blinking text
            int offsetNonBlink = offset;
            int lengthNonBlink = prefixOffsets[i] - offsetNonBlink;

            // add it to list
            if ( lengthNonBlink )
                push_back( value_type( value.mid( offsetNonBlink, lengthNonBlink ), m, false ) );

            // compute offset and length of blinking text
            int offsetBlink = offsetNonBlink + lengthNonBlink + BLINK_PREFIX.length();
            int lengthBlink = suffixOffsets[i] - offsetBlink;

            // add it to list
            if ( lengthBlink )
                push_back( value_type( value.mid( offsetBlink, lengthBlink ), m, true ) );

            // offset of next round
            offset = offsetBlink + lengthBlink + BLINK_SUFFIX.length();
        }

        // add last text segment into list
        if ( offset < value.length() )
            push_back( value_type( value.right( value.length() - offset ), m, false ) );
    }

    textSize_ = calculateTextSize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int marqueeText::holdTime( HoldLocation where ) const
{
    int amount = holdTimes_[where];

    // framed text has an implied hold time and should remain centered
    if (( DuringFrame == where ) && ( Frame == entryBehavior() ))
        ++amount;
    // a cleared marquee has an implied hold time and should remain cleared
    else if (( AfterExit == where ) && ( Clear == exitBehavior() ))
        ++amount;

    return amount;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void marqueeText::paint( QPainter *painter, const QPoint& pos, bool blinkTextVisible ) const
{
    int x = pos.x();

    for ( const_iterator i = begin(); i != end(); ++i )
    {
        QRect rect( QPoint( x, pos.y() ), i->textSize() );

        if (( !i->blinks() ) || ( blinkTextVisible ))
            painter->drawText( rect, i->text() );

        x += rect.width();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void marqueeText::copy( const _Myt& rhs )
{
    // clear the list
    while ( size() )
        pop_front();

    // copy list contents
    for ( const_iterator i = rhs.begin(); i != rhs.end(); ++i )
        push_back( *i );

    // copy properties
    textSize_ = rhs.textSize_;

    entryBehavior_ = rhs.entryBehavior_;
    exitBehavior_ = rhs.exitBehavior_;
    scrollSpeed_ = rhs.scrollSpeed_;

    for ( int i = 0; i < HOLD_TIMES; ++i )
        holdTimes_[i] = rhs.holdTimes_[i];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize marqueeText::calculateTextSize() const
{
    int width = 0;
    int height = 0;

    for ( const_iterator i = begin(); i != end(); ++i )
    {
        QSize size = i->textSize();

        width += size.width();

        if ( size.height() > height )
            height = size.height();
    }

    return QSize( width, height );
}
