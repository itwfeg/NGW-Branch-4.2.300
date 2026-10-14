//! MesssageParser.cpp
/*! 
 *  MesssageParser parses all incoming messages from the Application to the GUI.
 *  The message comes from the application, thru the socket, and into here
 *  for parsing. No other form, function, or class should call this method as
 *  its strict use if for socket message parsing only. If other parsing is
 *  required use the TokenParser class.
 *  Autor: Joe E. Crew
*/

#include <QStringBuilder>
#include <QStringList>

#include "MessageParser.h"


SessionMessage victor;


//! parseUpdate()
/*! 
    The parseUpdate() receives a CSV string that has been received from the socket
    and returns a vector as individual elements of that string.
    \ param: applicationStr - incoming CSV string from the Application thru the socket
    \ returns: a vector containing individual elements\arguments in the string
    \ Bugs\Issues: Straight port from Java->Javascript->here. Enhanced where possible.
*/
SessionMessage MessageParser::parseUpdate( const QString& applicationStr )
{
    victor.clear();                                                 // clean left over victors

    if(applicationStr.size() > 0)
    {
        // MAY HAVE TO SLAP A SEMI4 HERE TO PROTECT VICTOR
        // TODO TEST: test address of victor, and then test to see
        // if the address is the same in handleMessage().
        // RESULTS: Did the address test, the address here for victor
        // is different than the address of the returned victor in Session

        victor = readTokens(applicationStr);                        // parse the message
    }
    return victor;
}

// This method is used strictly by the parseUpdate() method above. I need to perform the same
// functionality as this method for items that are a quoted list in a method, but I don't want
// to take the chance of using readTokens() when an incoming message may come in as well. So I
// am going to copy readTokens(), and then let all other methods use it instead of this one.
//! readTokens()
/*! 
    The readTokens() does all of the work for returning a vector of elements
    \  listOf: incoming CSV string from the Application thru the socket
    \  returns: list: a vector containing individual elements\arguments in the string
    \  Bugs\Issues: Straight port from Java->Javascript->here. Enhanced where possible.
*/
SessionMessage MessageParser::readTokens( const QString& listOf )
{
    static const QChar delimiter( 255 );

    // Note:
    // Originally this code was calling simplified(). I found an issue where we were removing
    // embedded cr+lf (i.e. "1 quart\n2 fl oz") when we should not have been. Due to this I have
    // changed the code to trimmed() which is what was really intended.
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
    SessionMessage result;

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

//
// Old code is here, possibly remove in the future?
//
/*
    QVector<QString> list;

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
        c = listOf.at(index);               //str = new QString();  ///// This is from Javascript, why would I need this NEW?
        str = "";

//        if( c  == " "  || c == "\t" )
//        {
//            ++index;
//            while( index < length )
//            {
//                c = listOf.at(index);
//                if( c != " " && c != "\t")
//                    break;
//
//                ++index;
//            }
//        }


        // IF THIS TESTS OUT, DELETE THE IF LOOP ABOVE
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


                // Skip whitespace until next element??
                // CHECK AND SEE WHAT THIS DOES
                // Can we replace the next loop with the next line?
                // listOf.simplified();
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
*/


