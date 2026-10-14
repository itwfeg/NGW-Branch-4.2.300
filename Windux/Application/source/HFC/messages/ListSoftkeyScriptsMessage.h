#ifndef LISTSOFTKEYSCRIPTSMESSAGE_H
#define LISTSOFTKEYSCRIPTSMESSAGE_H

#include "HfcObject.h"
#include "HobList.h"


namespace hfc
{
	class ListSoftkeyScriptsMessage : public HfcObject
	{
		public:
			ListSoftkeyScriptsMessage() {};
			ListSoftkeyScriptsMessage(HobList s);

			~ListSoftkeyScriptsMessage() {};

			const char* serialize(int format) const;
			void add(HobString script);

		private:
			HobList scripts;
	};
}

#endif /* LISTSOFTKEYSCRIPTSMESSAGE_H */
