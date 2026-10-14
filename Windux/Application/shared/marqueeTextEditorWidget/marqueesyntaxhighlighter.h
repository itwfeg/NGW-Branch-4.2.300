/**
 * @file marqueesyntaxhighlighter.h
 * Syntax highlighter for marquee tags.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup marqueeEditorCsu
 * @{
 */

#ifndef MARQUEESYNTAXHIGHLIGHTER_H
#define MARQUEESYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QTextEdit>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class marqueeSyntaxHighlighter
 * @brief Syntax highlighter for marquee tags.
 */
class marqueeSyntaxHighlighter : public QSyntaxHighlighter
{
    typedef marqueeSyntaxHighlighter _Myt;
    typedef QSyntaxHighlighter _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] parent  parent control
     */
    marqueeSyntaxHighlighter( QObject *parent ) : _Mybase( parent ) {initialize();}

    /**
     * Constructor.
     *
     * @param[in] parent  parent control
     */
    marqueeSyntaxHighlighter( QTextDocument *parent ) : _Mybase( parent ) {initialize();}

    /**
     * Constructor.
     *
     * @param[in] parent  parent control
     */
    marqueeSyntaxHighlighter( QTextEdit *parent ) : _Mybase( parent ) {initialize();}

    /**
     * Destructor.
     */
    virtual	~marqueeSyntaxHighlighter() {}

    /**
     * Highlight syntax blocks within text.
     *
     * @param[in] text  text to highlight
     */
    virtual void highlightBlock( const QString& text );

private:

    QStringList tags_;
    QTextCharFormat tagFormat_;

    /**
     * Initialize variables.
     */
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // MARQUEESYNTAXHIGHLIGHTER_H
