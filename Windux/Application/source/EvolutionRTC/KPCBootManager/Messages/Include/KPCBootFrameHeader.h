/*
 * KPCBootFrame.h
 *
 *  Created on: Mar 13, 2019
 *      Author: Eric Landes
 */

#ifndef KPCBOOTFRAMEHEADER_H_
#define KPCBOOTFRAMEHEADER_H_

#include "EvolMessage.h"


class KPCBootFrameHeader: public EvolMessage{
public:
	KPCBootFrameHeader(const KPCBootFrameHeader&);
	KPCBootFrameHeader(const char *buffer);
	KPCBootFrameHeader(Hobart::CEvolMsgReceiver *pHobThread);
	~KPCBootFrameHeader();

	const char *serialize() const;

	void show();

	bool equals(const KPCBootFrameHeader &kpcBootFrameHeader) const;

    uint8_t getNumberOfFrames() const;
    void setNumberOfFrames(uint8_t frames);

    uint8_t getFinalFrameLength() const;
    void setFinalFrameLength(uint8_t length);

private:
	uint8_t numberOfFrames;
    uint8_t finalFrameLength;
};

#endif /* KPCBOOTFRAMEHEADER_H_ */
