/**
 ***************************************************************
 * Module Name            Program Name         Pgm Asm #        *
 * comm.h                     NSP                               *
 * *===============================================================*
 * Copyright (c) 1991, by                                       * Hobart Corporation
 *                                           * 401 W. Market St.                                            *
 * Troy, Ohio 45374                                             *
 * (513) 332-3000                                               *
 * Synopsis:                                                    * None														*
 * Descriptions:                                                *
 * This file contains all the include information necessary to * NSPII Communications.									    *
 * Assumptions:                                                 * None														*
 * Warnings:                                                    * None														*
 * Implementation:                                              * None														*
 * Author(s):                             Creation Date:        * Mark P. Strauser						2/28/91				*
 * ------------------------------------------------------------ *
 * Revision Record:                                             *
 * ------------------------------------------------------------ *
 * Rev #.#   Date      S/W Eng.         Description of Change   *
 */
#ifndef COMM_HEADER_H
#define COMM_HEADER_H



#include "PAL_platformheader.h"


class CommHeader {
	public:
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
		}   PhysicalAddress_t;


		typedef struct LogicalAddress
		{
			UINT16 pad0;
			DeviceAddress address;
		}   LogicalAddress_t;

		typedef UINT32 UniqueAddress;

	public:
		typedef enum CommCode
		{
			LOW_SPEED_TRANSMIT,
			LOW_SPEED_ALLOCATE,
			LOW_SPEED_RECEIVE,
			HIGH_SPEED_TRANSMIT,
			HIGH_SPEED_ALLOCATE,
			HIGH_SPEED_RECEIVE,
			SPI_TRANSMIT,
			SPI_ALLOCATE,
			SPI_RECEIVE
		} CommCode_t;

		typedef enum DeviceType
		{
		    COMM,
		    CONFIG,
		    KEYBOARD,
		    GRAPHICS,
		    NUMERIC,
		    PRINTER,
		    WEIGHER,
		    CLOCK,
		    SYSTEM,
		    USER_INTERFACE,
		    FORM_MANAGER,
		    LABEL_MANAGER,
		    MINI_NET_II, /* SCALE_MASTER, */
		    TIMER,
		    MANUFACTURE,
		    CLA,
		    DISK,
		    HPA,
		    WRAPPER = 0x1d,
		    INVALID_DEVICE
		} DeviceType_t;

		typedef union UltimaSystemAddress {
			PhysicalAddress physical;
			LogicalAddress logical;
			UniqueAddress unique;
		}   UltimaSystemAddress_t;

		CommHeader();
		CommHeader(const char *buffer, bool bigEndian = false);
		~CommHeader();

		UINT16 Serialize(char buffer[], bool bigEndian = false);
		UINT32 getSerializedLength() const;

		void show();

		void setFunctionCode(CommCode funcCode);
		void setFunctionCode(UINT8 funcCode);
		CommCode getFunctionCode();

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

		void setSequence(UINT16 sequence);
		UINT16 getSequence();

		void setMessagePriority(UINT8 messagePriority);
		UINT8 getMessagePriority();

		void setDestinationPriority(UINT8 destinationPriority);
		UINT8 getDestinationPriority();

		void setAllocation(UINT8 allocation);
		UINT8 getAllocation();

		void setResponse(UINT8 response);
		UINT8 getResponse();

		void setStatus(UINT8 status);
		UINT8 getStatus();

		void setMessageLength(UINT16 messageLength);
		UINT16 getMessageLength();

		void setBufferLength(UINT16 bufferLength);
		UINT16 getBufferLength();

		bool isLogicalSource(DeviceType device);

		CommHeader::DeviceType getSourceDeviceType();

	private:
		#define LOGICAL_DESTINATION	 	destination.logical.address
		#define PHYSICAL_DESTINATION 	destination.physical.address
		#define UNIQUE_DESTINATION 	 	destination.unique

		#define LOGICAL_SOURCE     	 	source.logical.address
		#define PHYSICAL_SOURCE      	source.physical.address
		#define UNIQUE_SOURCE      	 	source.unique

		UINT8 functionCode;
		UltimaSystemAddress source;
		UltimaSystemAddress destination;
		UINT16 sequence;
		UINT8 messagePriority;
		UINT8 destinationPriority;
		UINT8 allocation : 1;
		UINT8 response : 1;
		UINT8 reserved : 6;
		UINT8 status;
		UINT16 messageLength;
		UINT16 bufferLength;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif		/* COMM_HEADER_H */
