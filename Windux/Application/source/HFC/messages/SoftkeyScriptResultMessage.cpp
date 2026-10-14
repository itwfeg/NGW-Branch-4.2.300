#include "SoftkeyScriptResultMessage.h"

#include "Transaction.h"
#include "Configuration.h"
#include "evolution.h"
#include "MessageFormat.h"
#include "Bool.h"
#include "Debug.h"

namespace hfc
{
    SoftkeyScriptResultMessage::SoftkeyScriptResultMessage(HobString scriptName, int exitCode, HobString scriptResult)
    {
        name = scriptName;
        code = exitCode;
        result = scriptResult;
        result.quoteForCSV();
    }

	const char* SoftkeyScriptResultMessage::serialize(int format) const
	{
        serializedData.clear();

        if (format != BrowserFormat)
        {
            // ???
            return serializedData.data();
        }

        serializedData.append(name);
        serializedData.append(BrowserFormatSeparator);
        serializedData.append(std::to_string(code));
        serializedData.append(BrowserFormatSeparator);
        serializedData.append(result);

        return serializedData.data();
	}
}
