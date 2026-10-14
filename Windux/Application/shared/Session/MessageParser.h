//! MesssageParser.cpp
/*!
 *  MesssageParser parses all incoming messages from the Application to the GUI.
 *  The message comes from the application, thru the socket, and into here
 *  for parsing. No other form, function, or class should call this method as
 *  its strict use if for socket message parsing only. If other parsing is
 *  required use the TokenParser class.
 * @Author: Joe E. Crew
 * @bugs: TBD
 * @addtogroup thinClientMessagingCsu
*/

#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include <QObject>
#include <QString>
#include <QVector>

typedef QVector<QString> SessionMessage;

class MessageParser
{
    // from jsListParser.asp
public:

    // These two methods are used STRICTLY by incoming messages coming across the socket form the Application
    // These methods should ONLY ever be used by Session. For a generalized token parser, see TokenParser
    static SessionMessage parseUpdate( const QString& sessionStr );
    static SessionMessage readTokens( const QString& listOf );

};

Q_DECLARE_METATYPE( SessionMessage )

#endif // MESSAGEPARSER_H
