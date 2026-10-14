/**
 * @file marqueetextlist.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "marqueetextlist.h"
#include "marqueetextconstants.h"

#include <QPainter>

///////////////////////////////////////////////////////////////////////////////////////////////////
marqueeTextList marqueeTextList::create( const QString& value, const QFontMetrics& m )
{
    QString workingValue( convertMarqueeText( value ).trimmed() );

    // most properties stick until explicitly set
    marqueeText properties;

    // here is our finished product
    marqueeTextList result;

    // after we process an exit behavior and until we process something else we must apply
    // properties to previous text
    bool processingExit = false;

    int from = 0;

    // loop while we have text to process
    while ( workingValue.length() )
    {
        marqueeText::EntryBehavior newEntryBehavior = properties.entryBehavior();
        marqueeText *current = &properties;

        int index = workingValue.indexOf( "<", from );
        bool process = false;

        from = 0;

        if ( index > 0 )
            processingExit = false;

        // no more tag, process the text
        if ( index < 0 )
        {
            process = true;
            index = workingValue.length();
        }

        // handle tag at our index
        else
        {
            if (( processingExit ) && ( result.size() ))
                current = &result.last();

            if ( validateRemoveTag( workingValue, index, HOLD ) )
            {
                if ( processingExit )
                    current->incrementHoldTime( marqueeText::AfterExit );
                else if ( 0 == index )
                    current->incrementHoldTime( marqueeText::BeforeEntry );
                else
                    current->incrementHoldTime( marqueeText::DuringFrame );
            }
            else if ( validateRemoveTag( workingValue, index, SLOW ) )
                current->setScrollSpeed( marqueeText::SCROLL_SPEED_SLOW );
            else if ( validateRemoveTag( workingValue, index, NORMAL ) )
                current->setScrollSpeed( marqueeText::SCROLL_SPEED_NORMAL );
            else if ( validateRemoveTag( workingValue, index, QUICK ) )
                current->setScrollSpeed( marqueeText::SCROLL_SPEED_QUICK );
            else if ( validateRemoveTag( workingValue, index, EXTRA_QUICK ) )
                current->setScrollSpeed( marqueeText::SCROLL_SPEED_EXTRA_QUICK );
            else if ( validateRemoveTag( workingValue, index, SCROLL ) )
            {
                newEntryBehavior = marqueeText::Scroll;
                process = true;
            }
            else if ( validateRemoveTag( workingValue, index, FRAME ) )
            {
                newEntryBehavior = marqueeText::Frame;
                process = true;
            }
            else if ( validateRemoveTag( workingValue, index, ROLL ) )
            {
                current->setExitBehavior( marqueeText::Roll );
                processingExit = true;
                process = true;
            }
            else if ( validateRemoveTag( workingValue, index, CLEAR ) )
            {
                current->setExitBehavior( marqueeText::Clear );
                processingExit = true;
                process = true;
            }
            else
            {
                // no match to any known tag, search from next index
                from = index + 1;
            }
        }

        // process the text
        if ( process )
        {
            QString text = workingValue.left( index ).trimmed();

            workingValue.remove( 0, index );

            // insert the text into our list
            if ( text.length() )
            {
                marqueeText temp( properties );

                temp.setText( text, m );
                result.push_back( temp );
            }

            // reset properties
            properties.setEntryBehavior( newEntryBehavior );
            properties.resetHoldTimes();
        }

        workingValue = workingValue.trimmed();
    }

    // nothing in list, just give back simple list with empty text
    if ( !result.size() )
        result.push_back( properties );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool marqueeTextList::validateRemoveTag( QString& text, int index, const QString& tag )
{
    // check for tag
    if ( 0 == text.mid( index, tag.length() ).compare( tag, Qt::CaseInsensitive ) )
    {
        // remove it
        text.remove( index, tag.length() );

        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString marqueeTextList::convertMarqueeText( const QString& value )
{
    QString converted( value );

    int index;
    int count;

    // some scales use different tag formats, here we will convert...
    converted.replace( "\\S", SCROLL );
    converted.replace( "\\F", FRAME );
    converted.replace( "\\R", ROLL );
    converted.replace( "\\C", CLEAR );
    converted.replace( "\\H", HOLD );

    converted.replace( "\\L", SLOW );
    converted.replace( "\\N", NORMAL );
    converted.replace( "\\Q", QUICK );
    converted.replace( "\\X", EXTRA_QUICK );

    count = 0;

    while ( -1 != (index = converted.indexOf( "\\B" )) )
    {
        if ( 0 == (count%2) )
            converted.replace( index, 2, BLINK_PREFIX );
        else
            converted.replace( index, 2, BLINK_SUFFIX );

        ++count;
    }

    return converted;
}

