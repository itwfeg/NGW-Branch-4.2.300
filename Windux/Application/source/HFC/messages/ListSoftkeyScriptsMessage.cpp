#include "ListSoftkeyScriptsMessage.h"

#include "Transaction.h"
#include "Configuration.h"
#include "evolution.h"
#include "MessageFormat.h"
#include "Bool.h"
#include "Debug.h"

namespace hfc
{
	ListSoftkeyScriptsMessage::ListSoftkeyScriptsMessage(HobList s)
	{
        scripts = s;
	}

    void ListSoftkeyScriptsMessage::add(HobString script)
    {
        scripts.push_back(script);
    }

	const char* ListSoftkeyScriptsMessage::serialize(int format) const
	{
        serializedData.clear();
        
        // do we want to quote the script names?
        // I think we can expect them to be reasonable
        //output.setQuotingElements(true);

        serializedData.append(scripts.serialize(format));

        return serializedData.data();
	}
}
