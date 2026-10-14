/*
 * KPCBootFrame.h
 *
 *  Created on: Mar 13, 2019
 *      Author: Eric Landes
 */

#ifndef KPCBOOTUPGRADERFRAME_H_
#define KPCBOOTUPGRADERFRAME_H_

#include "KpcBootUpgraderEvent.h"
#include "EvolMessage.h"

class KPCBootUpgraderFrame: public EvolMessage{
public:
	KPCBootUpgraderFrame(const KPCBootUpgraderFrame&);
	KPCBootUpgraderFrame(const char *buffer);
	KPCBootUpgraderFrame(Hobart::CEvolMsgReceiver *pHobThread, KPCBootUpgraderEvent event);
	~KPCBootUpgraderFrame();

	const char *serialize() const;

	void show();

	bool equals(const KPCBootUpgraderFrame &kpcBootUpgraderFrame) const;

	const unsigned char* getFrameBuffer() const {
		return framebuffer;
	}

	void setFramebuffer(const unsigned char* buff) {
        memcpy(&framebuffer, buff, sizeof(framebuffer));
	}

private:
	unsigned char framebuffer[8];
};

#endif /* KPCBOOTUPGRADERFRAME_H_ */
