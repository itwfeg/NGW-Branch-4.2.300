/*
 * KPCBootUpgraderStatus.h
 *
 *  Created on: Mar 15, 2019
 *      Author: hobart
 */

#ifndef KPCBOOTUPGRADERSTATUS_H_
#define KPCBOOTUPGRADERSTATUS_H_

#include "KPCBootUpgraderGeneric.h"
#include "Integer.h"
#include "ULong.h"


class KPCBootUpgraderStatus: public EvolMessage{
public:

	typedef enum BootUpgraderStatusCode
	{
		BOOT_NO_STATUS,
	    BOOT_INIT,
	    BOOT_READY,
	    BOOT_UPGRADING,
	    BOOT_FAULT,
	    BOOT_VERIFY,
	    BOOT_DONE
	}BootUpgraderStatusCode;


	typedef enum BootUpgraderFaults
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
	}BootUpgraderFaults;


	KPCBootUpgraderStatus(const KPCBootUpgraderStatus&);
	KPCBootUpgraderStatus(const char *buffer);
	KPCBootUpgraderStatus(Hobart::CEvolMsgReceiver *pHobThread);
	~KPCBootUpgraderStatus();

	const char *serialize() const;

	void show();

	bool equals(const KPCBootUpgraderStatus &kpcBootCmd) const;

	unsigned long getId() const;
	void setId(unsigned long);

	BootUpgraderStatusCode getStatusCode() const;
	void setStatusCode(BootUpgraderStatusCode newStat);

	BootUpgraderFaults getBootUpgraderFaults() const;
	void setBootUpgraderFaults(BootUpgraderFaults newFault);

	unsigned long getFaultInfo() const;
	void setFaultInfo(unsigned long);


private:
	unsigned long		pid;
	BootUpgraderStatusCode 		status;
	BootUpgraderFaults 	fault;
	unsigned long 		faultInfo;
};

#endif /* KPCBOOTUPGRADERSTATUS_H_ */
