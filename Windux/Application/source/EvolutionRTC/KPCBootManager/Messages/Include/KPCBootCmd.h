/*
 * KPCBootCmd.h
 *
 *  Created on: Mar 13, 2019
 *      Author: Eric Landes
 */

#ifndef KPCBOOTCMD_H_
#define KPCBOOTCMD_H_

#include "EvolMessage.h"
#include "KPCBootGeneric.h"

typedef enum
{
	NO_CMD,
	JUMP,
	INIT_UPGRADE,
	I_HEARD_YOU_NOW_SHUT_UP,
	VERIFY_APP
} BootCmd;

class KPCBootCmd : public EvolMessage
{
public:
	KPCBootCmd(const KPCBootCmd &);
	KPCBootCmd(const char *buffer);
	KPCBootCmd(Hobart::CEvolMsgReceiver *pHobThread, UINT8 destination, UINT8 source);
	~KPCBootCmd();

	const char *serialize() const;

	void show();

	bool equals(const KPCBootCmd &kpcBootCmd) const;

	BootCmd getCommand() const;
	void setCommand(BootCmd newCommand);

	UINT8 getDestination() const { return destination; }
	void setDestination(UINT8 id) { destination = id; }

	UINT8 getSourceId() const { return sourceId; }
	void setSourceId(UINT8 id) { sourceId = id; }

	uint32_t getAddress() const { return address; }
	void setAddress(uint32_t addr) { address = addr; }

private:
	BootCmd command;
	UINT8 destination;
	UINT8 sourceId;
	uint32_t address;
};

#endif /* KPCBOOTCMD_H_ */
