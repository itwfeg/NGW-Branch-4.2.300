/**
 * @file softkeyoverlaymanager.cpp
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Tony Shelton-McGaha
 */

#include "softkeyoverlaymanager.h"
#include "softkeybutton.h"

#include <logmanager.h>

#include "Session.h"
#include <../sessionHandlerHooks/sessionhooks.h>

#include <sstream>
#include <regex>

#include <time.h>

softkeyOverlayManager* softkeyOverlayManager::instance_ = NULL;
QMutex softkeyOverlayManager::instanceMutex_;

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeyOverlayManager* softkeyOverlayManager::instance()
{
  LOG_CLASS_INFO("softkeyOverlayManager", "setResults");

  if ( softkeyOverlayManager::instance_ == NULL )
  {
    QMutexLocker locker( &instanceMutex_ );

    if ( softkeyOverlayManager::instance_ == NULL )
    {
      LOG_WARN("creating new instance", NULL);
      softkeyOverlayManager::instance_ = new softkeyOverlayManager();

      const Session* session_ = sessionHooks::instance()->session();

      connect( session_, SIGNAL( receive(SessionMessage) ), softkeyOverlayManager::instance_, SLOT( setResults(SessionMessage) ) );
    }
  }

  return softkeyOverlayManager::instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyOverlayManager::setResults( const SessionMessage &value )
{

  LOG_CLASS_INFO("softkeyOverlayManager", "setResults");

  static const std::regex RECTANGLE_REGEX( "^(rectangle) [^.](-?[[:digit:]]+)[^.](-?[[:digit:]]+)[^.] ([[:digit:]]+) ([[:digit:]]+) (\\S+)" );
  static const std::regex TEXT_REGEX( "^(text) [^.](-?[[:digit:]]+)[^.](-?[[:digit:]]+)[^.] ([[:digit:]]+) (\\S+) (.+)" );
  static const std::regex FONT_REGEX( "^(font) (.+)" );
  static const std::regex CIRCLE_REGEX( "^(circle) [^.](-?[[:digit:]]+)[^.](-?[[:digit:]]+)[^.] ([[:digit:]]+) (\\S+)" );
  static const std::regex OVAL_REGEX( "^(oval) [^.](-?[[:digit:]]+)[^.](-?[[:digit:]]+)[^.] ([[:digit:]]+) ([[:digit:]]+) (\\S+)" );
  static const std::regex IMAGE_REGEX( "^(image) [^.](-?[[:digit:]]+)[^.](-?[[:digit:]]+)[^.] ([[:digit:]]+) ([[:digit:]]+) (\\S+) ([^.]+)([.]\\S+)" );
  static const std::regex FLASH_REGEX( "^(flash) ([[:digit:]]+) ([[:digit:]]+)" );

  int device = value[0].toInt();
  int event = value[1].toInt();

  if ( SoftkeyDev == device && SoftkeyScriptResult == event )
  {
    LOG_WARN("new results message for", NULL);

    SoftkeyScriptResultMsg m( value );
    int status = m.exitStatus();

    if ( status )
    {
      m.setScriptOutput("text (50,25) 15 #ffffff Error: " + m.scriptName() + "\n");
    }

    QStringList cmd = m.scriptOutput().split("\n");
    std::smatch match;
    QVector<softkeyOverlayCommand> overlayOBJ;

    for (int i = 0; i < cmd.length(); ++i )
    {
      std::string str = cmd[i].toStdString();
      softkeyOverlayCommand overlayCMD;

      if (cmd[i].length() <= 2) continue;

      switch(cmd[i][0].toLatin1())
      {
        case 'r':
          if ( std::regex_search( str, match, RECTANGLE_REGEX ) )
          {
            overlayCMD.type = softkeyOverlayCommand::COMMAND_RECTANGLE;

            std::stringstream ssXPos( match[2] );
            ssXPos >> overlayCMD.xPos;

            std::stringstream ssYPos( match[3] );
            ssYPos >> overlayCMD.yPos;

            std::stringstream ssWSize( match[4] );
            ssWSize >> overlayCMD.wSize;

            std::stringstream ssHSize( match[5] );
            ssHSize >> overlayCMD.hSize;

            overlayCMD.qColor = QString::fromStdString(match[6]);
            overlayCMD.colorBrush = QBrush( QColor(overlayCMD.qColor), Qt::SolidPattern );

            overlayOBJ.push_back( overlayCMD );
          }
          break;

        case 'c':
          if ( std::regex_search( str, match, CIRCLE_REGEX ) )
          {
            overlayCMD.type = softkeyOverlayCommand::COMMAND_CIRCLE;

            std::stringstream ssXPos( match[2] );
            ssXPos >> overlayCMD.xPos;

            std::stringstream ssYPos( match[3] );
            ssYPos >> overlayCMD.yPos;

            std::stringstream ssRadius( match[4] );
            ssRadius >> overlayCMD.radius;

            overlayCMD.qColor = QString::fromStdString(match[5]);
            overlayCMD.colorBrush = QBrush( QColor(overlayCMD.qColor), Qt::SolidPattern );

            overlayOBJ.push_back( overlayCMD );
          }
          break;

        case 't':
          if ( std::regex_search( str, match, TEXT_REGEX ) )
          {
            overlayCMD.type = softkeyOverlayCommand::COMMAND_TEXT;

            std::stringstream ssXPos( match[2] );
            ssXPos >> overlayCMD.xPos;

            std::stringstream ssYPos( match[3] );
            ssYPos >> overlayCMD.yPos;

            std::stringstream ssSize( match[4] );
            ssSize >> overlayCMD.size;

            overlayCMD.qColor = QString::fromStdString(match[5]);
            overlayCMD.colorBrush = QBrush( QColor(overlayCMD.qColor), Qt::SolidPattern );

            overlayCMD.qText = QString::fromStdString(match[6]);

            overlayOBJ.push_back( overlayCMD );
          }
          break;

        case 'i':
          if ( std::regex_search( str, match, IMAGE_REGEX ) )
          {
            overlayCMD.type = softkeyOverlayCommand::COMMAND_IMAGE;

            std::stringstream ssXPos( match[2] );
            ssXPos >> overlayCMD.xPos;

            std::stringstream ssYPos( match[3] );
            ssYPos >> overlayCMD.yPos;

            std::stringstream ssWidth( match[4] );
            ssWidth >> overlayCMD.width;

            std::stringstream ssHeight( match[5] );
            ssHeight >> overlayCMD.height;

            overlayCMD.qImgFit = QString::fromStdString(match[6]);

            overlayCMD.qFilePath = QString::fromStdString( match[7]);

            overlayCMD.qFileType = QString::fromStdString(match[8]);

            overlayOBJ.push_back( overlayCMD );
          }
          break;

        case 'o':
          if ( std::regex_search( str, match, OVAL_REGEX ) )
          {
            overlayCMD.type = softkeyOverlayCommand::COMMAND_OVAL;

            std::stringstream ssXPos( match[2] );
            ssXPos >> overlayCMD.xPos;

            std::stringstream ssYPos( match[3] );
            ssYPos >> overlayCMD.yPos;
            std::stringstream ssWRadius( match[4] );
            ssWRadius >> overlayCMD.wRadius;

            std::stringstream ssHRadius( match[5] );
            ssHRadius >> overlayCMD.hRadius;

            overlayCMD.qColor = QString::fromStdString(match[6]);
            overlayCMD.colorBrush = QBrush( QColor(overlayCMD.qColor), Qt::SolidPattern );

            overlayOBJ.push_back( overlayCMD );
          }
          break;

        case 'f':
          if ( std::regex_search( str, match, FONT_REGEX ) )
          {
            overlayCMD.type = softkeyOverlayCommand::COMMAND_FONT;
            overlayCMD.qFontName = QString::fromStdString(match[2]);

            overlayOBJ.push_back( overlayCMD );
          }
          else if ( std::regex_search( str, match, FLASH_REGEX ) )
          {
            overlayCMD.type = softkeyOverlayCommand::COMMAND_FLASH;

            std::stringstream ssSpeed( match[2] );
            ssSpeed >> overlayCMD.speed;

            std::stringstream ssDuration( match[3] );
            ssDuration >> overlayCMD.duration;

            overlayOBJ.push_back( overlayCMD );
          }
          break;

        default:
          break;
      }
    }

    results_[m.scriptName()] = overlayOBJ;
    emit newData(m.scriptName());
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QVector<softkeyOverlayCommand>& softkeyOverlayManager::operator[]( QString scriptName )
{
  if ( results_[scriptName].count() == 0 )
  {
    results_[scriptName] = QVector<softkeyOverlayCommand>();
  }

  return results_[scriptName];
}
