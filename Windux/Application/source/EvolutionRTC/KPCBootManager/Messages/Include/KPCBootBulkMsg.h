#ifndef KPCBOOTBULKMSG_H_
#define KPCBOOTBULKMSG_H_
#include "EvolMessage.h"
class KPCBootBulkMsg : public EvolMessage
{
  public:
    static const size_t MAX_PAYLOAD_SIZE = 64;

    KPCBootBulkMsg(Hobart::CEvolMsgReceiver *pHobThread = NULL, const unsigned char *pImage = NULL, UINT8 cmd = 0, int frameNum = 0, int size = 0, UINT8 id = 0);
    KPCBootBulkMsg(const char *buffer);
    ~KPCBootBulkMsg();

    int getNumFrames() const { return numFrames; }
    void setNumFrame(int num) { numFrames = num; }

    int getLength() const { return length; }
    void setLength(int x) { length = x; }

    UINT8 getCommand() const { return cmd; }
    void setCommand(UINT8 x) { cmd = x; }

    UINT8 getDestination() const { return destination; }
    void setDestination(UINT8 id) { destination = id; }

    UINT8 getSourceId() const { return sourceId; }
    void setSourceId(UINT8 id) { sourceId = id; }

    UINT8 getData(int index) const { return data[index]; }
    const char *serialize() const;

    void show() const;

  private:
    int numFrames;
    int length;
    UINT8 cmd;
    UINT8 destination;
    UINT8 sourceId;
    unsigned char data[MAX_PAYLOAD_SIZE];
};

#endif /* KPCBOOTBULKMSG_H_ */
