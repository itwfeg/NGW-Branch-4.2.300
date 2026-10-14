#ifndef UPGRADETRANSFERSTATUS_H
#define UPGRADETRANSFERSTATUS_H

#include "EvolMessage.h"

//class HobThread;

class UpgradeTransferStatus : public EvolMessage
{
    public:
        UpgradeTransferStatus(UINT32 bytesSent = 0, UINT32 totalBytes = 0, Hobart::CEvolMsgReceiver *pHobThread = NULL);
        UpgradeTransferStatus(const char *buffer);
        ~UpgradeTransferStatus();

        const char *serialize() const;
        void show();

        UINT32 getBytesSent();
        UINT32 getBytesSent() const;

        void setBytesSent(UINT32 bytes);

        UINT32 getTotalBytes();
        UINT32 getTotalBytes() const;

        void setTotalBytes(UINT32 bytes);

    private:
        UINT32 bytesSent;               //      How many bytes have been sent so far.
        UINT32 totalBytes;              //      The total number of bytes to be sent.

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif  //UPGRADETRANSFERSTATUS_H
