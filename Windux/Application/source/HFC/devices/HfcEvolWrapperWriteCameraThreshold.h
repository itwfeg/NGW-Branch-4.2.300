#ifndef HFCEVOLWRAPPERWRITECAMERATHRESHOLD_H
#define HFCEVOLWRAPPERWRITECAMERATHRESHOLD_H

#include "HfcObject.h"
#include "EvolWrapperWriteCameraThreshold.h"

class HfcEvolWrapperWriteCameraThreshold : public EvolWrapperWriteCameraThreshold, public HfcObject
{
	public:

		HfcEvolWrapperWriteCameraThreshold(const char *data);
		HfcEvolWrapperWriteCameraThreshold(const char *data, int len, int formatType);

		~HfcEvolWrapperWriteCameraThreshold();

		virtual const char *serialize(int format = InternalFormat)const;
};

#endif	//	HFCEVOLWRAPPERWRITECAMERATHRESHOLD_H
