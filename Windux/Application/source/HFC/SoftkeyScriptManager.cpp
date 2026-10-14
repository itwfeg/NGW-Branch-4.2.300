#include "SoftkeyScriptManager.h"
#include "ListSoftkeyScriptsMessage.h"
#include "SoftkeyScriptResultMessage.h"
#include "HobString.h"
#include "HobList.h"
#include "SessionMessage.h"
#include "Debug.h"

#include <time.h>
#if defined( __linux__)
#include <dirent.h>
#endif
#include <cstdint>
#include <random>

// handy timespec comparison
bool operator <(const timespec& lhs, const timespec& rhs)
{
  if (lhs.tv_sec == rhs.tv_sec)
  {
    return lhs.tv_nsec < rhs.tv_nsec;
  }
  else
  {
    return lhs.tv_sec < rhs.tv_sec;
  }
}

// handy pair comparison
bool operator <(const std::pair<timespec, std::string>& lhs, const std::pair<timespec, std::string>& rhs)
{
  return lhs.first < rhs.first;
}

SoftkeyScriptManager::SoftkeyScriptManager()
{
  finished = false;

  if (Debug::isTracing("SoftkeyScriptManager")) Debug::trace("SoftkeyScriptManager", "Starting worker thread");
  workThread = std::thread(&SoftkeyScriptManager::runScripts, this);
}

SoftkeyScriptManager::~SoftkeyScriptManager()
{
  finished = true;
  
  workThread.join();
}

void SoftkeyScriptManager::sendScriptList()
{
  bool traceOn = Debug::isTracing("SoftkeyScriptManager");

  if (traceOn) Debug::trace("SoftkeyScriptManager", "Got request for script list, scanning " + std::string(SOFTKEY_SCRIPT_PATH));

#if defined( __linux__)

	DIR* dir = opendir(SoftkeyScriptManager::SOFTKEY_SCRIPT_PATH);
	struct dirent *entry = NULL;

	hfc::ListSoftkeyScriptsMessage listMsg;

	if (dir != NULL)
	{
		for (entry = readdir(dir); entry != NULL; entry = readdir(dir))
		{
			HobString script = HobString(entry->d_name);

			// only add files if they end in .py
			int l = script.length();
			if (l > 3 && 0 == script.compare(l - 3, 3, ".py"))
			{
				if (traceOn)
				{
					Debug::trace("SoftkeyScriptManager", HobString("Found script: ") + script);
				}

				listMsg.add(script);
			}
		}
	}
	else
	{
		if(traceOn)
		{
			Debug::trace("SoftkeyScriptManager", "Failed to open script directory!");
		}
	}
	
	SessionMessage outputMessage(SoftkeyDev, ListSoftkeyScripts);

	outputMessage.setFormatType(BrowserFormat);
	outputMessage.setHfcObject(&listMsg);
	
  Session* mySession = SessionManager::getInstance()->getPrimaryConsoleSession();
  mySession->send(&outputMessage, Messenger::DEST_UI);

  closedir(dir);
#endif

}

void SoftkeyScriptManager::addScript(std::string scriptName, time_t frequency)
{
  if (scriptName == "") return;

  scriptMutex.lock();

  if (Debug::isTracing("SoftkeyScriptManager"))
  {
    if (scriptFrequency.count(scriptName) > 0)
    {
      Debug::trace("SoftkeyScriptManager", "Not adding " + scriptName + " because it is already running");
    }
    else
    {
      std::string d("Adding script #");

      d += std::to_string(scriptFrequency.size());
      d += " with interval " + std::to_string(frequency);
      d += "ms: " + scriptName;

      Debug::trace("SoftkeyScriptManager", d);
    }
  }

  // convert frequency (milliseconds) to seconds and nanoseconds
  timespec newTime = { frequency / 1000, (frequency % 1000) * 1000000};

  // if we already have the script, update frequency if needed
  if (scriptFrequency.count(scriptName) > 0)
  {
    timespec oldTime = scriptFrequency[scriptName];
    scriptFrequency[scriptName] = newTime < oldTime ? newTime : oldTime;
  }
  else
  {
    scriptResult[scriptName] = std::pair<std::string,int>("no-op\n", 0);

    // add to front of work queue with time of 0 (i.e. always do it next)
    std::pair<timespec, std::string> newScript;
    newScript.first = {0, 0};
    newScript.second = scriptName;
    scriptQueue.push_front(newScript);

    scriptFrequency[scriptName] = newTime;
  }

  scriptMutex.unlock();
}

void SoftkeyScriptManager::runScripts()
{
  bool traceOn = Debug::isTracing("SoftkeyScriptManager");
  std::random_device rd;
  std::mt19937 mt(rd());

#if defined( __linux__)
  while (!finished)
  {
    scriptMutex.lock();

    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    // find the first script that's due before now (if any)
    auto nextScript = std::find_if(scriptQueue.begin(),
                                   scriptQueue.end(),
                                   [now](std::pair<timespec, std::string> s)
                                   { return s.first < now; });

    if (nextScript != scriptQueue.end())
    {
      bool firstRun = nextScript->first.tv_sec == 0 && nextScript->first.tv_nsec == 0;
      std::string scriptName = nextScript->second;
      scriptQueue.erase(nextScript);
      scriptMutex.unlock();

      if (traceOn)
      {
        Debug::trace("SoftkeyScriptManager", "Running script " + scriptName);
      }

      std::string path = "nice -n " + std::to_string(SoftkeyScriptManager::SCRIPT_NICE_LEVEL) + " python3 " + std::string(SOFTKEY_SCRIPT_PATH) + scriptName + " 2>/dev/null";

      FILE* pipe = popen(path.c_str(), "r");

      if (pipe != NULL)
      {
        std::array<char, 1024> buffer;
        std::string scriptOutput;
        int scriptExit;
        bool sendUpdate = false;

        while (fgets(buffer.data(), 1024, pipe) != NULL)
        {
          scriptOutput += buffer.data();
        }

        scriptExit = pclose(pipe);
        scriptExit = WEXITSTATUS(scriptExit);

        if (traceOn && scriptExit != 0)
        {
          Debug::trace("SoftkeyScriptManager", "Nonzero return value for script " + scriptName);
        }

        scriptMutex.lock();

        if (scriptResult[scriptName].first != scriptOutput || scriptResult[scriptName].second != scriptExit)
        {
          scriptResult[scriptName] = std::pair<std::string, int>(scriptOutput, scriptExit);
          sendUpdate = true;
        }
        else if (traceOn)
        {
          Debug::trace("SoftkeyScriptManager", "No change in script " + scriptName + " output, not sending results");
        }

        scriptMutex.unlock();

        if (sendUpdate)
        {
          hfc::SoftkeyScriptResultMessage msg(scriptName, scriptExit, scriptOutput);
          SessionMessage outputMessage(SoftkeyDev, SoftkeyScriptResult);

          outputMessage.setFormatType(BrowserFormat);
          outputMessage.setHfcObject(&msg);

          Session* mySession = SessionManager::getInstance()->getPrimaryConsoleSession();
          mySession->send(&outputMessage, Messenger::DEST_UI);
        }
      }
      else if (traceOn)
      {
        Debug::trace("SoftkeyScriptManager", "Failed to start " + scriptName);
      }

      scriptMutex.lock();

      // get time for next run of this script (now + frequency)
      clock_gettime(CLOCK_MONOTONIC, &now);

      timespec nextTime = {now.tv_sec + scriptFrequency[scriptName].tv_sec, 0 };
      uint64_t nextTimeNano = now.tv_nsec + scriptFrequency[scriptName].tv_nsec;

      if (nextTimeNano > 1000000000)
      {
        nextTimeNano -= 1000000000;
        nextTime.tv_sec++;
      }

      nextTime.tv_nsec = nextTimeNano;

      // if we're running this script for the first time, pick a random offset
      // hopefully this spreads out when scripts will run
      if (firstRun)
      {
        std::uniform_int_distribution<uint64_t> dist(1, scriptFrequency[scriptName].tv_sec);
        int offset = dist(mt);

        if (traceOn)
        {
          Debug::trace("SoftkeyScriptManager", "Running " + scriptName + " for first time, randomly offsetting by " + std::to_string(offset) + " seconds");
        }

        nextTime.tv_sec += offset;
      }

      // add back to queue
      scriptQueue.push_back(std::pair<timespec, std::string>(nextTime, scriptName));
    }

    // find earliest script in the queue
    auto minScript = std::min_element(scriptQueue.begin(), scriptQueue.end());
    timespec nextSleep;

    now.tv_sec += MAX_SLEEP_TIME;

    // if no scripts, or they're off in the future, sleep max
    if (minScript == scriptQueue.end() || now < minScript->first)
    {
      nextSleep = {MAX_SLEEP_TIME, 0};
    }
    else
    {
      nextSleep = MIN_SLEEP_TIME;
    }

    scriptMutex.unlock();
    nanosleep(&nextSleep, NULL);
  }
#endif
}
