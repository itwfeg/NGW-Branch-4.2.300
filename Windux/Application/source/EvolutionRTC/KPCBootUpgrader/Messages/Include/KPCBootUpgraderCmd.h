/*
 * KPCBootCmd.h
 *
 *  Created on: Mar 13, 2019
 *      Author: Eric Landes
 */

#ifndef KPCBOOTUPGRADERCMD_H_
#define KPCBOOTUPGRADERCMD_H_

#include "EvolMessage.h"

typedef enum
{
  NO_UPGRADER_CMD,
  INIT_BOOT_UPGRADE,
  VERIFY_BOOT,
  KILL_UPGRADER
}BootUpgraderCmd;

class KPCBootUpgraderCmd: public EvolMessage{
public:
	KPCBootUpgraderCmd(const KPCBootUpgraderCmd&);
	KPCBootUpgraderCmd(const char *buffer);
	KPCBootUpgraderCmd(Hobart::CEvolMsgReceiver *pHobThread, UINT8 destination, UINT8 source);
	~KPCBootUpgraderCmd();

	const char *serialize() const;

	void show();

	bool equals(const KPCBootUpgraderCmd &kpcBootUpgraderCmd) const;

	BootUpgraderCmd getCommand() const;
	void setCommand(BootUpgraderCmd newCommand);

	UINT8 getDestination() const { return destination; }
  void setDestination(UINT8 id) { destination = id; }

  UINT8 getSourceId() const { return sourceId; }
  void setSourceId(UINT8 id) { sourceId = id; }

private:
	BootUpgraderCmd command;
	UINT8 destination;
    UINT8 sourceId;
};

#endif /* KPCBOOTUPGRADERCMD_H_ */
