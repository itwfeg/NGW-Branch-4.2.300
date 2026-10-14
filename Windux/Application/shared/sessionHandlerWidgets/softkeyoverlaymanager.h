/**
 * @file softkeyoverlaymanager.h
 * Softkey overlay singleton.
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Tony Shelton-McGaha
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SOFTKEYOVERLAYMANAGER_H
#define SOFTKEYOVERLAYMANAGER_H

#include "Messages.h"
#include <QBrush>


struct softkeyOverlayCommand
{
  enum commandType { COMMAND_RECTANGLE, COMMAND_CIRCLE, COMMAND_TEXT, COMMAND_IMAGE, COMMAND_OVAL, COMMAND_FONT, COMMAND_FLASH } type;

  int xPos, yPos, wSize, hSize, wRadius, hRadius, size, radius, width, height, speed, duration;

  QString qColor, qText, qFontName, qFilePath, qFileType, qImgFit;
  QBrush colorBrush;
};

class softkeyOverlayManager : public QObject
{
  Q_OBJECT

  public:
  
    /**
     * Deleted Copy Constructor.
     *
     * @param[in] softkeyOverlayManager& class instance
     */  
    softkeyOverlayManager( const softkeyOverlayManager& ) = delete;

    /**
     * single instance getter.
     *
     * @returns single instance
     */
    static softkeyOverlayManager* instance();

    /**
     * script name & output getter
     *
     * @returns map of results
     */
    QMap<QString,QVector<softkeyOverlayCommand>> results(){ return results_; }

    /**
     * Overloads subscript operator.
     *
     * @param QString script name
     */
    QVector<softkeyOverlayCommand>& operator[](QString);

  public slots:
  
    /**
     * Parses Session Message.
     */
    void setResults(const SessionMessage& );

  private:
  
     /**
     * Constructor.
     *
     * creates single instance
     */
    softkeyOverlayManager(){ };

    static softkeyOverlayManager* instance_; // contains single instance

    static QMutex instanceMutex_;

    QMap<QString,QVector<softkeyOverlayCommand>> results_;

  signals:
     /**
     * emits new script data.
     *
     * @param[in] QString command string
     */
    void newData(const QString&);
};

#endif // SOFTKEYOVERLAYMANAGER_H
