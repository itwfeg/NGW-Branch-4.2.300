/**
 * @file marqueesyntaxhighlighter.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "marqueesyntaxhighlighter.h"

#include <marqueetextconstants.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
void marqueeSyntaxHighlighter::highlightBlock( const QString& text )
{
    // for each tag setup the format
    foreach ( const QString& tag, tags_ )
        for ( int pos = text.indexOf( tag ); pos >= 0; pos = text.indexOf( tag, ++pos ) )
            setFormat( pos, tag.length(), tagFormat_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void marqueeSyntaxHighlighter::initialize()
{
    // list of all known tags
    tags_.push_back( SCROLL );
    tags_.push_back( FRAME );
    tags_.push_back( ROLL );
    tags_.push_back( CLEAR );
    tags_.push_back( HOLD );
    tags_.push_back( SLOW );
    tags_.push_back( NORMAL );
    tags_.push_back( QUICK );
    tags_.push_back( EXTRA_QUICK );
    tags_.push_back( BLINK_PREFIX );
    tags_.push_back( BLINK_SUFFIX );

    // setup tag highlighting
    tagFormat_.setBackground( Qt::black );
    tagFormat_.setForeground( Qt::white );
    tagFormat_.setVerticalAlignment( QTextCharFormat::AlignSubScript );
    tagFormat_.setFontWeight( QFont::Bold );
}
