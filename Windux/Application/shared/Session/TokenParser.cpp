//! TokenParse.cpp
/*! TokenParser parses all strings that need parsed that are in comma separated value list format.
 *  These strings are typically a piece of another message that still need to be parsed. A single
 *  message may receive as an argument a quoted CSV list inside the message itself. For example, in
 *  the flashkey message, there are multiple lists of "names", or "fonts", or "colors", or "graphics"
 *  that will all have to be parsed as well. Currently there are only 2 parsing methods in this class
 *  and one is not yet implemented. Any new parsing messages used anywhere in the UI should be placed
 *  here so they can be used by any class when needed.
 *  Autor: Joe E. Crew
*/

#include <QVector>
#include <QString>
#include <QStringList>
#include "TokenParser.h"

TokenParser::TokenParser()
{
    // from jsCSVParser.asp
}

// IMPORTANT NOTE
//
// USE THIS CLASS OVER MessageParser(). MessageParser() will be busy processing incoming messages so don't use it.
//

//! splitStringTokens()
/*!
 *   The splitStringTokens() does all of the work for returning a vector of elements
 *   \  listOf: incoming CSV string from the Application thru the socket
 *   \  returns: list: a vector containing individual elements\arguments in the string
 *   \  Bugs\Issues: Straight port from Java->Javascript->here. Enhanced where possible.
 */
QVector<QString> TokenParser::splitStringTokens( const QString& listOf )
{
    static const QChar delimiter( 255 );

    // Note:
    // I changed this from simplified() to trimmed() because it was stripping new line
    // characters from within the strings... I believe the intent here is only to remove
    // whitespace from the beginning and end of the string.
    // Note Update:
    // trimmed() will be completely fine.  Like you said the intent here was to only
    // strip whitespace off of the front and end.
    QString temp = listOf.trimmed();

    bool withinTextField = false;

    // replace commas with delimiter
    for ( int i = 0; i < temp.length(); ++i )
    {
        // do not replace commas within a text field
        if ( '\"' == temp[i] )
            withinTextField = !withinTextField;
        else if ( !withinTextField )
        {
            if ( ',' == temp[i] )
                temp[i] = delimiter;
        }
    }

    // split list
    QList<QString> list = temp.split( delimiter );

    // populate result
    QVector<QString> result;

    foreach ( const QString& s, list )
    {
        QString formatted;

        // remove leading and trailing quote
        if (( 2 <= s.length() ) && ( '\"' == s[0] ) && ( '\"' == s[s.length()-1] ))
            formatted = s.mid( 1, s.length()-2 );
        else
            formatted = s;

        // replace double quotes
        formatted.replace( "\"\"", "\"" );

        result.append( formatted );
    }

    return result;
}


//! parseStringTokens()
/*!
 *   The parseStringTokens() does all of the work for returning a vector of elements
 *   \  listOf: incoming CSV string from the Application thru the socket
 *   \ param: numOfTokens - The number of tokens in the list to be parsed.
 *   \  returns: list: a vector containing individual elements\arguments in the string
 *   \  Bugs\Issues: Straight port from Java->Javascript->here. Enhanced where possible.
 *   \ THIS IS THE OLD METHOD - USE THE ONE ABOVE. BUT they both do the same task
 */
QVector<QString> TokenParser::parseStringTokens( int numOfTokens, const QString& listOf )
{
    QVector<QString> list(numOfTokens);

    bool newField  = true;
    bool loopIt = true;

    int index = 0;
    int newIndex = 0;
    int lastIndex = 0;
    int tmpIndex  = 0;
    int length = listOf.length();
    int ctr = 0;

    QString c = "";
    QString str = "";

    // We will execute one iteration in this loopIt for each data element in the listOf String

    // The following while loops are to skip past any initial whitespace, i.e., spaces and tabs
    // not really sure if leading whitespace is an issue anymore. Was it ever an issue???? Remove this?
    while(index < length)
    {
        c = listOf.at(index);
        str = "";

        /*
        if( c  == " "  || c == "\t" )
        {
            ++index;
            while( index < length )
            {
                c = listOf.at(index);
                if( c != " " && c != "\t")
                    break;

                ++index;
            }
        }
        */

        /* USE THIS LINE, and IF it tests out, completely delete ABOVE IF loop */
        listOf.simplified();

        // Determine end of field and store listOf
        if( index < length)
        {
            // if we are not at the end of the listOf string, then c points to the next character
            if( c == "\"")                                          // did we find a quote? Remember to escape it
            {
                // if the first character is a quote then the entire token is quoted
                ++index;                                            // skip the arguments beginning quote

                // check to find out if this is an empty token, quite common actually
                if ( index < length && (listOf.at(index) != '\"' || (index + 1 < length && listOf.at(index + 1) == '\"')) )
                {
                    // If this is not an empty field (""), then look for the end of the field replacing any double double-quote with a double-quote.
                    loopIt = true;
                    do
                    {
                        newIndex = listOf.indexOf('\"', index);
                        if( newIndex  != -1)
                        {
                            if ( newIndex + 1 < length )
                            {
                                if ( listOf.at(newIndex + 1) == '\"' )
                                    ++newIndex;                     // Add 1 of the 2 consecutive doublequotes.
                                else
                                    loopIt = false;
                            }
                        }
                        else                                    // Houston we have a problem, but ignore it
                            newIndex = length;                  // This will cause that index be 1 past length, which is ok

                        tmpIndex = newIndex - index;
                        str = listOf.mid(index, tmpIndex);
                        index = newIndex + 1;

                    }while( index < length && loopIt );
                }

                // Add Element
                list.append(str);
                ctr++;
                newField = false;

                // skip anything until the next comma or the end of the listOf
                if ( index < length )
                {
                    newIndex = listOf.indexOf(',', index);
                    if ( newIndex != -1 )
                    {
                        index = newIndex + 1;
                        newField = true;
                    }
                    else
                        index = length;
                }

            }
            else
            {
                lastIndex = length - 1;

                // There are no quote characters, therefore look for the next comma.
                newIndex = listOf.indexOf(',', index);

                if ( newIndex != -1 )
                {
                    lastIndex = newIndex - 1;
                    newField = true;
                }
                else
                {
                    newIndex = length - 1;
                    newField = false;
                }


                // Skip whitespace
                if ( index <= lastIndex )
                {
                    c = listOf.at(lastIndex);
                    if ( c == " " || c == "\t" )
                    {
                        --lastIndex;
                        while ( index < lastIndex )
                        {
                            c = listOf.at(lastIndex);
                            if ( c != " " && c != "\t" )
                                break;

                            --lastIndex;
                        }
                    }
                }

                // Add element
                lastIndex  = lastIndex - index;
                str = listOf.mid(index, lastIndex + 1 );          // BRUTALLY TEST THIS
                list.append(str);
                ctr++;

                index = newIndex + 1;
            }
        }
    }

    // newField is used to indicate whether there is a new data element (due
    // to the presence of a comma, the field separator) that has not been
    // processed yet and still needs processing. If not processed above, it
    // will be processed here.
    if( newField )
        list.append("");

    return list;
}



// In the Javascript code there are many instances of the parseMe() method scattered everywhere
// It all does the same thing but its created different in different places. Sometimes theArray
// is passed in, and then returned, other times just the string itself is passed in, sometimes
// the length is passed in, sometimes not. In the end it all does the same thing. So I am going
// to make one general method, passing in just the string, and returning a vector with the list,
// if possible. Depending on how I can translate this method, I may have to pass in the length
// because the length will be known about the list and the vector can be initialized right up
// front. Hmmmmmm, the more I think about it, the better that actually sounds.
/*
[ all of the possibilities are listed to cover them all ]
function parseMe(arrLen, opStr, theArray)
function parseMe(arrayLen, opStr)
function parseMe(theStr, theArray)

{
    var tmpStr = "";
    var findComma = 0;
    var parseStr = "";
    var didThis = 0;

    for(var i = 0; i < arrLen; i++)
    {
        findComma = opStr.indexOf(",");
        parseStr = opStr.substring(0,findComma);
        theArray[i] = parseStr;
        opStr = opStr.substring(findComma + 1, opStr.length);
        didThis = 1;
    }

    if(didThis)
        theArray[i-1] = opStr;
    return theArray;
}
*/
