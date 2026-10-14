#include "msgbox.h"
#include <sstream>
#include <string>
#include <iostream>
#include <QDebug>
#include <QtDebug>

#include"msgboxconfig.h"


std::string ftos(float f) {
    std::stringstream ss;
    ss<< f;
    std::string ystr(ss.str());
    return ystr;
}

msgbox::msgbox(QObject *parent)
    :QObject(parent)
{
    init();
}

msgbox::~msgbox()
{
    delete messageList;
}

void msgbox::init(){
    messageList = new QList<message*>();
    createMessageList();
}

void msgbox::slt_MessageUpdated()
{
    updateMessageList();
}

void msgbox::slt_messageAccepted(int id)
{
    for(message* msg: *messageList)
    {
        if(msg->getUniqueId() == id)
        {
            msg->updateMessageFileLastConfirmCompleted();
        }
    }
}

void msgbox::updateMessageList(){
    for(int i = 0; i < messageList->length(); i++){
        delete messageList->at(i);
    }
    this->messageList->clear();
    createMessageList();
}

void msgbox::createMessageList(){
    msgBoxConfig::messageList list = msgBoxConfig::readMessages();
    message * newMsg;
    this->messageList->clear();
    for (const auto & m :list){
        int counter = 0;
        QTime t;
        for (const auto & item : m){
            if(counter == 0)
            {
                newMsg = new message(this);
                newMsg->setIsSystemMessage((bool)item.toInt());
            }
            else if(counter == 1)
            {
                newMsg->setIsDiabled((bool)item.toInt());
            }
            else if(counter == 2)
            {
                newMsg->setHeader(item);
            }
            else if(counter == 3)
            {
                newMsg->setMsg(item);
            }
            else if(counter == 4)
            {
                newMsg->setType(item.toInt());
            }
            else if(counter == 5)
            {
                newMsg->setDayOfWeek(item.toInt());
            }
            else if(counter == 6)
            {
                if(item != ""){
                    QStringList dateList = item.split(";");
                    foreach(QString s, dateList){
                        newMsg->addDates(s.toInt());
                    }
                }
            }
            else if(counter == 7)
            {
                //newMsg->addDates(item.toInt());
            }
            else if(counter == 16)
            {
                newMsg->setRepeatInterval(item.toInt());
            }
            else if(counter == 17)
            {
                newMsg->setUniqueId(item.toInt());
            }
            else if(counter == 18)
            {
                newMsg->setLastAlerted(item.toLongLong());
            }
            else if(counter == 19)
            {
                newMsg->setLastConfirmCompleted(item.toLongLong());
            }
            //time boxes are:
            else if(counter >= 8 && counter <=15)
            {
                if(item != ""){
                    QStringList timeList = item.split(";");
                    foreach(QString s, timeList){
                        newMsg->addTimes(QTime::fromString(s));
                    }
                }
            }
            counter++;
        }
        //start message functions after all info about the message has been added
        newMsg->startTimer();
        connect(newMsg, SIGNAL(sig_DisplayMessage(QString,QString,QString,int)), this, SIGNAL(sig_DisplayMessage(QString,QString,QString,int)));
        this->messageList->append(newMsg);
    }
}
