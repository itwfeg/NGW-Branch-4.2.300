#include "KPCBootUpgraderBulkMsg.h"
#include "KpcBootUpgraderEvent.h"
#include "HobString.h"
#include "Debug.h"
#include "Integer.h"
#include "Hex.h"

KPCBootUpgraderBulkMsg::KPCBootUpgraderBulkMsg(Hobart::CEvolMsgReceiver *pHobThread, const unsigned char *pImage, UINT8 cmd, int frameNum, int size, UINT8 id) : EvolMessage(KPCBootUpdater, KpcBootUpgraderBulkMsg , pHobThread)
{
    numFrames = frameNum;
    length = size;
    destination = id;
    sourceId = 255;
    this->cmd = cmd;

    Debug::trace("KPCBootUpgraderBulkMsg", "Constructor()  length: " + Integer::toString(length) + "number of frames:" + Integer::toString(frameNum));
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

KPCBootUpgraderBulkMsg::KPCBootUpgraderBulkMsg(const char *buffer) : EvolMessage(buffer)
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

KPCBootUpgraderBulkMsg::~KPCBootUpgraderBulkMsg()
{
}

const char *KPCBootUpgraderBulkMsg::serialize() const
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
    Debug::trace("KPCBootUpgraderBulkMsg", "serializedData: " + s);

    return serializedData.data();
}

void KPCBootUpgraderBulkMsg::show() const
{
    bool isTracing = Debug::isTracing("KPCBootUpgraderBulkMsg");
    if (isTracing == false)
    {
        return;
    }
    Debug::trace("KPCBootUpgraderBulkMsg", "KPCBootUpgraderBulkMsg: ");

    EvolMessage::show();

    Debug::trace("KPCBootUpgraderBulkMsg",
                 HobString("Number of Frames: ") + HobString(Integer::toString(numFrames)));

    Debug::trace("KPCBootUpgraderBulkMsg",
                 HobString("Length: ") + HobString(Integer::toString(length)));

    Debug::trace("KPCBootUpgraderBulkMsg",
                 HobString("Command: ") + HobString(Integer::toString(cmd)));
    
    Debug::trace("KPCBootUpgraderBulkMsg",
                 HobString("Destination: ") + HobString(Integer::toString(destination)));
    Debug::trace("KPCBootUpgraderBulkMsg",
                 HobString("Source: ") + HobString(Integer::toString(sourceId)));

    std::string s;
    int payloadSize = (numFrames * 8) + length;
    for (int i = 0; i < payloadSize; i++)
    {
        s += Hex::toString(data[i]);
    }
    Debug::trace("KPCBootUpgraderBulkMsg", "serializedData: " + s);


    if (isTracing == false)
    {
        Debug::removeTraceCategory("KPCBootUpgraderBulkMsg");
    }
}
