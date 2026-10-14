
#include "UsbManager.h"
#include "IUsbCodeUpdater.h"


class AtmelSambaUpdater : public IUsbCodeUpdater
{
	public:

	    AtmelSambaUpdater(USBD_NODE_ID usbNodeId);
	    ~AtmelSambaUpdater();

	protected:
		void handleVersionManagerDevEvent(VersionManagerEvent verManagerEvent, const char *recvBuffer);
		void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );

	private:
	    // number of simultaneously connected AT91 devices
	    static const int MAX_NUM_AT91_DEVICES=10;

	    // Assume Target is a SAM7S256 (SAM7S-EK)
	    // Otherwise, modify that part by yourself, it will be a good training! ;-)

	    //static const unsigned int SAM7S256_DBGU_CIDR=0x270B0940;

	    // various constants required for AT91 calls
	    static const unsigned int AT91_CMD_ADDR_VENDOR_ID=0x108000;
	    static const unsigned int AT91_CMD_ADDR_PRODUCT_ID=0x108002;
	    static const unsigned int AT91_FIRMWARE_START_ADDRESS=0x108200;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

