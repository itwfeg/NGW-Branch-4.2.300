#include "KPCBootBulkMsg.h"
#include "KpcBootEvent.h"
#include "HobString.h"
#include "Debug.h"
#include "Integer.h"
#include "Hex.h"

KPCBootBulkMsg::KPCBootBulkMsg(Hobart::CEvolMsgReceiver *pHobThread, const unsigned char *pImage, UINT8 cmd, int frameNum, int size, UINT8 id) : EvolMessage(KPCBootloader, KpcBootBulkMsg , pHobThread)
{
    numFrames = frameNum;
    length = size;
    destination = id;
    sourceId = 255;
    this->cmd = cmd;

    Debug::trace("KPCBootBulkMsg", "Constructor()  length: " + Integer::toString(length) + "number of frames:" + Integer::toString(frameNum));
    int payloadSize = 0;
    if (length != 0)
    {
        payloadSize = ((numFrames - 1) * 8) + length;
    }
    else
    {
        payloadSize = numFrames * 8;
    }

    for (int i = 0; i < payloadSize; i++)
    {
        data[i] = *pImage++;
    }
}

KPCBootBulkMsg::KPCBootBulkMsg(const char *buffer) : EvolMessage(buffer)
{
    EvolMessage::serialize();
    int j = getSerializedLength();

    char *pChar = (char *)&numFrames;
    for (unsigned int i = 0; i < sizeof(numFrames); i++)
    {
        *pChar++ = buffer[j++];
    }

    pChar = (char *)&length;
    for (unsigned int i = 0; i < sizeof(length); i++)
    {
        *pChar++ = buffer[j++];
    }

    pChar = (char *)&cmd;
    for (unsigned int i = 0; i < sizeof(cmd); i++)
    {
        *pChar++ = buffer[j++];
    }

    pChar = (char *)&destination;
    for (unsigned int i = 0; i < sizeof(destination); i++)
    {
        *pChar++ = buffer[j++];
    }

    pChar = (char *)&sourceId;
    for (unsigned int i = 0; i < sizeof(sourceId); i++)
    {
        *pChar++ = buffer[j++];
    }

    int payloadSize = (numFrames * 8) + length;
    for (int i = 0; i < payloadSize; i++)
    {
        data[i] = buffer[j++];
    }
}

KPCBootBulkMsg::~KPCBootBulkMsg()
{
}

const char *KPCBootBulkMsg::serialize() const
{
    EvolMessage::serialize();

    char *pChar = (char *)&numFrames;
    for (unsigned int i = 0; i < sizeof(numFrames); i++)
    {
        serializedData.append(1, pChar[i]);
    }

    pChar = (char *)&length;
    for (unsigned int i = 0; i < sizeof(length); i++)
    {
        serializedData.append(1, pChar[i]);
    }

    pChar = (char *)&cmd;
    for (unsigned int i = 0; i < sizeof(cmd); i++)
    {
        serializedData.append(1, pChar[i]);
    }

    pChar = (char *)&destination;
    for (unsigned int i = 0; i < sizeof(destination); i++)
    {
        serializedData.append(1, pChar[i]);
    }

    pChar = (char *)&sourceId;
    for (unsigned int i = 0; i < sizeof(sourceId); i++)
    {
        serializedData.append(1, pChar[i]);
    }

    int payloadSize = (numFrames * 8) + length;
    for (int i = 0; i < payloadSize; i++)
    {
        serializedData.append(1, data[i]);
    }

    std::string s;
    for (int i = 0; i < payloadSize; i++)
    {
        s += Hex::toString(data[i]);
    }
    Debug::trace("KPCBootBulkMsg", "serializedData: " + s);

    return serializedData.data();
}

void KPCBootBulkMsg::show() const
{
    bool isTracing = Debug::isTracing("KPCBootBulkMsg");
    if (isTracing == false)
    {
        return;
    }
    Debug::trace("KPCBootBulkMsg", "KPCBootBulkMsg: ");

    EvolMessage::show();

    Debug::trace("KPCBootBulkMsg",
                 HobString("Number of Frames: ") + HobString(Integer::toString(numFrames)));

    Debug::trace("KPCBootBulkMsg",
                 HobString("Length: ") + HobString(Integer::toString(length)));

    Debug::trace("KPCBootBulkMsg",
                 HobString("Command: ") + HobString(Integer::toString(cmd)));
    
    Debug::trace("KPCBootBulkMsg",
                 HobString("Destination: ") + HobString(Integer::toString(destination)));
    Debug::trace("KPCBootBulkMsg",
                 HobString("Source: ") + HobString(Integer::toString(sourceId)));

    std::string s;
    int payloadSize = (numFrames * 8) + length;
    for (int i = 0; i < payloadSize; i++)
    {
        s += Hex::toString(data[i]);
    }
    Debug::trace("KPCBootBulkMsg", "serializedData: " + s);


    if (isTracing == false)
    {
        Debug::removeTraceCategory("KPCBootBulkMsg");
    }
}
