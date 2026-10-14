#include "KPCBootFrame.h"
#include "HobString.h"
#include "Debug.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterGeneric::dataType = 0;
std::string PrinterGeneric::dataTypeName("KPCBootFrame");
#endif

KPCBootFrame::KPCBootFrame(const KPCBootFrame& newBFrame) : EvolMessage(newBFrame){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	memcpy(framebuffer,newBFrame.getFrameBuffer(), sizeof(framebuffer));
}

KPCBootFrame::KPCBootFrame(const char *buffer) : EvolMessage(buffer){
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

KPCBootFrame::KPCBootFrame(Hobart::CEvolMsgReceiver *pHobThread, KPCBootEvent event) : EvolMessage( KPCBootloader, event, pHobThread ){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	memset(framebuffer, 0, sizeof(framebuffer));
}


KPCBootFrame::~KPCBootFrame()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *KPCBootFrame::serialize() const
{

	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &framebuffer;
	for (unsigned int i = 0; i < sizeof(framebuffer); i++) {
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void KPCBootFrame::show(){
	bool isTracing = Debug::isTracing("KPCBootFrame");

	if (isTracing == false) {
		return;
	}
	Debug::trace("KPCBootFrame", "KPCBootFrame: ");

	EvolMessage::show();

    std::string hob("");
    for(int i = 0; i < sizeof(framebuffer); i++)
    hob.append(Integer::toString(framebuffer[0]));
	Debug::trace("KPCBootFrame",
			HobString("FameBuffer: ") + HobString(hob));


	if (isTracing == false) {
		Debug::removeTraceCategory("KPCBootFrame");
	}
}