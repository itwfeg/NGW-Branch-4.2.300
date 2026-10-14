#include <map>
#include <utility>
#include <chrono>
#include <mutex>

#include "evolution.h"
#include "Session.h"
#include "SessionManager.h"
#include "Softkey.h"
#include "SoftkeyDevEvent.h"

// singleton; see https://stackoverflow.com/a/1008289 for explanation

class SoftkeyScriptManager
{
	public:
		// directory containing softkey scripts available to run
		const char* SOFTKEY_SCRIPT_PATH = "/usr/local/hobart/user/softkeyscripts/";

    // nice level to run scripts at (stay nicer than scaleui or it gets laggy)
		const int SCRIPT_NICE_LEVEL = 15;

		// max amount of seconds to sleep if no scripts are due soon
    const int MAX_SLEEP_TIME = 1;

		// min amount of time to sleep (0.1s)
		const timespec MIN_SLEEP_TIME = {0, 100000000};

		static SoftkeyScriptManager& getInstance()
		{
			static SoftkeyScriptManager instance;
			return instance;
		}
		
		void sendScriptList();
		void addScript(std::string scriptName, time_t frequency);

	private:
		std::mutex scriptMutex;

		std::thread workThread;

		std::map<std::string, timespec> scriptFrequency;
		std::map<std::string, std::pair<std::string, int>> scriptResult;

		std::list<std::pair<timespec, std::string>> scriptQueue;

		bool finished;

		void runScripts();

		// private constructor/destructor for singleton
		SoftkeyScriptManager();
		~SoftkeyScriptManager();

	public:
		// don't allow assignment or copy constructor for singleton
		SoftkeyScriptManager(SoftkeyScriptManager const&) = delete;
		void operator=(SoftkeyScriptManager const&) = delete;
};
