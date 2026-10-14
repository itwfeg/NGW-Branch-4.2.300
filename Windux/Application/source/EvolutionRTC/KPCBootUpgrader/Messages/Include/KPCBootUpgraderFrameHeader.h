/*
 * KPCBootUpgraderFrameHeader.h
 *
 *  Created on: Mar 13, 2019
 *      Author: Eric Landes
 */

#ifndef KPCBOOTUPGRADERFRAMEHEADER_H_
#define KPCBOOTUPGRADERFRAMEHEADER_H_

#include "EvolMessage.h"
#include "KpcBootUpgraderEvent.h"


class KPCBootUpgraderFrameHeader: public EvolMessage{
public:
	KPCBootUpgraderFrameHeader(const KPCBootUpgraderFrameHeader&);
	KPCBootUpgraderFrameHeader(const char *buffer);
	KPCBootUpgraderFrameHeader(Hobart::CEvolMsgReceiver *pHobThread);
	~KPCBootUpgraderFrameHeader();

	const char *serialize() const;

	void show();

	bool equals(const KPCBootUpgraderFrameHeader &kpcBootUpgraderFrameHeader) const;

    uint8_t getNumberOfFrames() const;
    void setNumberOfFrames(uint8_t frames);

    uint8_t getFinalFrameLength() const;
    void setFinalFrameLength(uint8_t length);

private:
	uint8_t numberOfFrames;
    uint8_t finalFrameLength;
};

#endif /* KPCBOOTUPGRADERFRAMEHEADER_H_ */
