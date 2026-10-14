#include "KPCBootUpgraderFrame.h"
#include "HobString.h"
#include "Debug.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterGeneric::dataType = 0;
std::string PrinterGeneric::dataTypeName("KPCBootUpgraderFrame");
#endif

KPCBootUpgraderFrame::KPCBootUpgraderFrame(const KPCBootUpgraderFrame& newBFrame) : EvolMessage(newBFrame){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	memcpy(framebuffer,newBFrame.getFrameBuffer(), sizeof(framebuffer));
}

KPCBootUpgraderFrame::KPCBootUpgraderFrame(const char *buffer) : EvolMessage(buffer){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &framebuffer;
	for (unsigned int i = 0; i < sizeof(framebuffer); i++)
	{
		*pChar++ = buffer[j++];
	}

}

KPCBootUpgraderFrame::KPCBootUpgraderFrame(Hobart::CEvolMsgReceiver *pHobThread, KPCBootUpgraderEvent event) : EvolMessage( KPCBootUpdater, event, pHobThread ){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	memset(framebuffer, 0, sizeof(framebuffer));
}


KPCBootUpgraderFrame::~KPCBootUpgraderFrame()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *KPCBootUpgraderFrame::serialize() const
{

	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &framebuffer;
	for (unsigned int i = 0; i < sizeof(framebuffer); i++) {
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void KPCBootUpgraderFrame::show(){
	bool isTracing = Debug::isTracing("KPCBootUpgraderFrame");

	if (isTracing == false) {
		return;
	}
	Debug::trace("KPCBootUpgraderFrame", "KPCBootUpgraderFrame: ");

	EvolMessage::show();

    std::string hob("");
    for(int i = 0; i < sizeof(framebuffer); i++)
    hob.append(Integer::toString(framebuffer[0]));
	Debug::trace("KPCBootUpgraderFrame",
			HobString("FameBuffer: ") + HobString(hob));


	if (isTracing == false) {
		Debug::removeTraceCategory("KPCBootUpgraderFrame");
	}
}