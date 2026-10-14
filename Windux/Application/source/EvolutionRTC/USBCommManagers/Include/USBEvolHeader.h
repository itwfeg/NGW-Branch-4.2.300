#ifndef USBEVOLHEADER_H
#define USBEVOLHEADER_H



#include "PAL_platformheader.h"


class USBEvolHeader {
	private:
		/****************************************************************
		 * These device names were ported back to Ultima for Qunatum	*
		 * in an effort to keep all of the devices between products		*
		 * lines compatible.											*
		 */

		typedef UINT16 DeviceAddress;

		typedef struct PhysicalAddress
		{
			DeviceAddress address;
			UINT16 pad0;
		} PhysicalAddress_t;


		typedef struct LogicalAddress
		{
			UINT16 pad0;
			DeviceAddress address;
		} LogicalAddress_t;

		typedef UINT32 UniqueAddress;

		typedef union UltimaSystemAddress {
			PhysicalAddress physical;
			LogicalAddress logical;
			UniqueAddress unique;
		} UltimaSystemAddress_t;

	public:

		static const int SIZE = 10;

		typedef enum DeviceType
		{
		    COMM = 0x00,
		    PRINTER = 0x05,
		    WEIGHER = 0x06,
		    SYSTEM = 0x08,
		    USER_INTERFACE = 0x09,
		    CLA = 0x0F,
		    WRAPPER = 0x1D,
			UFW_PRINTER = 0x22,
			EVOL_PRINTER = 0x24,
			ESC_NUMERIC = 0x25,
			ESC_MARQUEE = 0x26,
		    INVALID_DEVICE
		} DeviceType_t;

		USBEvolHeader();
		USBEvolHeader(UltimaSystemAddress source,
					  UltimaSystemAddress destination,
					  UINT16 messageLength);
		USBEvolHeader(const char *buffer, bool bigEndian);
		virtual ~USBEvolHeader();

		UINT16 Serialize(char buffer[], bool bigEndian = false);
		UINT32 getSerializedLength() const;

		void show();

		void setSourceUniqueAddress(UINT32 uniAddr);
		UINT32 getSourceUniqueAddress();
		void setSourceLogicalAddress(UINT16 logAddr);
		UINT16 getSourceLogicalAddress();
		void setSourcePhysicalAddress(UINT16 phyAddr);
		UINT16 getSourcePhysicalAddress();

		void setDestinationUniqueAddress(UINT32 uniAddr);
		UINT32 getDestinationUniqueAddress();
		void setDestinationLogicalAddress(UINT16 logAddr);
		UINT16 getDestinationLogicalAddress();
		void setDestinationPhysicalAddress(UINT16 phyAddr);
		UINT16 getDestinationPhysicalAddress();

		UINT16 getMessageLength();
		void setMessageLength(UINT16 messageLength);

	private:
		#define LOGICAL_DESTINATION	 	destination.logical.address
		#define PHYSICAL_DESTINATION 	destination.physical.address
		#define UNIQUE_DESTINATION 	 	destination.unique

		#define LOGICAL_SOURCE     	 	source.logical.address
		#define PHYSICAL_SOURCE      	source.physical.address
		#define UNIQUE_SOURCE      	 	source.unique

		UltimaSystemAddress source;
		UltimaSystemAddress destination;

		UINT16 messageLength;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif		/* USBEVOLHEADER_H */
