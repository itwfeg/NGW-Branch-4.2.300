//! TokenParse.h
/*! TokenParser parses all strings that need parsed that are in comma separated value list format.
 *  These strings are typically a piece of another message that still need to be parsed. A single
 *  message may receive as an argument a quoted CSV list inside the message itself. For example, in
 *  the flashkey message, there are multiple lists of "names", or "fonts", or "colors", or "graphics"
 *  that will all have to be parsed as well. Currently there are only 2 parsing methods in this class
 *  and one is not yet implemented. Any new parsing messages used anywhere in the UI should be placed
 *  here so they can be used by any class when needed.
 * @Author: Joe E. Crew
 * @bugs: TBD
 * @addtogroup thinClientMessagingCsu
*/

#ifndef TOKENPARSER_H
#define TOKENPARSER_H

#include <QVector>
#include <QString>

class TokenParser
{
    // from jsCSVParser.asp
public:
    TokenParser();

    /**
     * Method to split strings based on commas. This method is used to split strings that have already been
     * placed into single elements, but their single element is also a string list. USE THIS PARSER OVER
     * parseStringTokens(). It has been optimized for QT over the parseStringTokens() ported version
     * @param[in] listOf  listOf is the string list to be parsed.
     * @returns  none
     */
    static QVector<QString> splitStringTokens( const QString& listOf );

    /** parseStringTokens()
     *
     *   The parseStringTokens() does all of the work for returning a vector of elements
     *   @param[in] listOf is the incoming CSV string argument string to be split
     *   @param[in] numOfTokens - The number of tokens in the list to be parsed.
     *   @returns list is a vector containing individual elements\arguments in the string
     *   \ THIS IS THE OLD METHOD - USE THE ONE ABOVE. BUT they both do the same task
     */
    static QVector<QString> parseStringTokens( int numOfTokens, const QString& listOf );

    /** parseStringTokens()
     *
     *   The parseStringTokens() is NOT YET IMPLEMENTED
     *   @param[in] none
     *   @returns none
     */
    static QVector<QString> parseMe(const QString& tokenStr );
};

#endif // TOKENPARSER_H
