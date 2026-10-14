/*
 * KPCBootStatus.h
 *
 *  Created on: Mar 15, 2019
 *      Author: hobart
 */

#ifndef KPCBOOTSTATUS_H_
#define KPCBOOTSTATUS_H_

#include "KPCBootGeneric.h"
#include "Integer.h"
#include "ULong.h"



class KPCBootStatus: public EvolMessage{
public:

	typedef enum BootStatusCode
	{
		BOOT_NO_STATUS,
	    BOOT_INIT,
	    BOOT_READY,
	    BOOT_UPGRADING,
	    BOOT_FAULT,
	    BOOT_VERIFY,
	    BOOT_DONE
	}BootStatusCode;


	typedef enum BootloaderFaults
	{
	    NOFAULT,
	    TIMEOUT,
	    WRITE_FAILURE,
	    CHECKSUM,
	    UNKNOWN_RECORD_TYPE,
	    ERASE_FAILURE_APP,
	    ERASE_FAILURE_BOOT,
	    COMPONENT_NOT_SET,
	    IMAGE_MISMATCH,
	    IMAGE_CHECKSUM_MISMATCH,
	    MODEL_TYPE_ERROR,
	    WRITE_ADDRESS_OUT_OF_RANGE,
	    FRAME_OUT_OF_RANGE
	}BootloaderFaults;


	KPCBootStatus(const KPCBootStatus&);
	KPCBootStatus(const char *buffer);
	KPCBootStatus(Hobart::CEvolMsgReceiver *pHobThread);
	~KPCBootStatus();

	const char *serialize() const;

	void show() const;

	bool equals(const KPCBootStatus &kpcBootCmd) const;

	unsigned long getId() const;
	void setId(unsigned long);

	BootStatusCode getStatusCode() const;
	void setStatusCode(BootStatusCode newStat);

	BootloaderFaults getBootloaderFaults() const;
	void setBootloaderFaults(BootloaderFaults newFault);

	unsigned long getFaultInfo() const;
	void setFaultInfo(unsigned long);


private:
	unsigned long		pid;
	BootStatusCode 		status;
	BootloaderFaults 	fault;
	unsigned long 		faultInfo;
};

#endif /* KPCBOOTSTATUS_H_ */
