#ifndef UPGRADEPERIPHERAL_H
#define UPGRADEPERIPHERAL_H

#include "EvolMessage.h"

class UpgradePeripheral : public EvolMessage
{
    public:
        UpgradePeripheral(UINT8 event, EvolDeviceType device = UnknownDevice, UINT16 subType = 0, Hobart::CEvolMsgReceiver *pHobThread = NULL);
        UpgradePeripheral(const char *buffer);
        ~UpgradePeripheral();

        const char *serialize() const;
        void show();

        EvolDeviceType getDevice();
        EvolDeviceType getDevice() const;

        void setDevice(EvolDeviceType dev);

        UINT16 getSubType();
        UINT16 getSubType() const;

        void setSubType(UINT16 type);

    private:
        EvolDeviceType device;
        UINT16 subType;                         //      i.e. WrapperKind

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif  //UPGRADEPERIPHERAL_H
