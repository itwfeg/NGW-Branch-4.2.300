#ifndef SOFTKEYSCRIPTRESULTMESSAGE_H
#define SOFTKEYSCRIPTRESULTMESSAGE_H

#include "HfcObject.h"
#include "HobString.h"


namespace hfc
{
	class SoftkeyScriptResultMessage : public HfcObject
	{
		public:
			SoftkeyScriptResultMessage(HobString scriptName, int exitCode, HobString scriptResult);

			const char* serialize(int format) const;

		private:
			HobString name;
            int code;
            HobString result;
	};
}

#endif /* SOFTKEYSCRIPTRESULTMESSAGE_H */
