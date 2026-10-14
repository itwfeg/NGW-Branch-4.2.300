/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "OnlineHelp.h"
OnlineHelp * OnlineHelp::getInstance()
{
	if (instance == 0)
	{
		instance = new OnlineHelp();
	}
	return instance;
}

OnlineHelp * OnlineHelp::instance = 0;
class XMLFormat;

XMLFormat OnlineHelp::lookupHelp(StringType helpScreenId, StringType fieldId)
{
}

XMLFormat OnlineHelp::lookupHelp(StringType helpScreenId)
{
}

OnlineHelp::OnlineHelp()
{
}

