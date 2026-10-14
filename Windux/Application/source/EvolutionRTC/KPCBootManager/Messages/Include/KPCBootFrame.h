/*
 * KPCBootFrame.h
 *
 *  Created on: Mar 13, 2019
 *      Author: Eric Landes
 */

#ifndef KPCBOOTFRAME_H_
#define KPCBOOTFRAME_H_

#include "KPCBootGeneric.h"

class KPCBootFrame: public EvolMessage{
public:
	KPCBootFrame(const KPCBootFrame&);
	KPCBootFrame(const char *buffer);
	KPCBootFrame(Hobart::CEvolMsgReceiver *pHobThread, KPCBootEvent event);
	~KPCBootFrame();

	const char *serialize() const;

	void show();

	bool equals(const KPCBootFrame &kpcBootFrame) const;

	const unsigned char* getFrameBuffer() const {
		return framebuffer;
	}

	void setFramebuffer(const unsigned char* buff) {
        memcpy(&framebuffer, buff, sizeof(framebuffer));
	}

private:
	unsigned char framebuffer[8];
};

#endif /* KPCBOOTFRAME_H_ */
