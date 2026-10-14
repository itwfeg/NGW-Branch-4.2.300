/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#if 0
#define NO_SPAN_TIME		//	Give me the actual time and not the time difference from startup.
#endif

/****************************************************************************************************
 * Note: There are several versions of trace. the old trace functions are still present in this class
 They will print directly to either the standard output or the trace file. there are two new versions
 of the trace method. both simply append a message to a global message queue. these trace methods
 return as soon as the messages are placed into the message queue. a back ground task is running that
 will simply receive the messages placed into the message queue by the trace command. thus even if
 a task is suspended, its previous trace calls should still be handled properly. that is unless the
 task has completely wiped out the operating system. this class is entirely static. thus all of its
 features are available even if the class has not bee instatiated.

USAGE: start the background debug task by calling startDebugTask. once the task has been started.
the priority of the debug task can be adjusted with the call to changeDebugTaskPriority. then
calls to the two new trace methods can be called.
 ******************************************************************************************************/

// Based on the Debug class from TNP

#include "Trace.h"
#include "HobTokenizer.h"
#include "Integer.h"
#include "Hex.h"
#include "Bool.h"
#include "CommMechanism.h"
#include "thread/thread.h"
#include "util/thread/lock.h"


#define NEW_MSG_SIZE 2048
#define NEW_MAX_MSG (Trace::MAX_MSGS / 1)

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Trace::dataType = 0;
std::string Trace::dataTypeName("Trace");
#endif


using namespace std;


Trace * Trace::_instance_ = 0;
util_thread::Mutex<> Trace::_instanceMutex_ = util_thread::Mutex<>();

/**
  Debug is used to help developers identify and localize problems in the code.

  Debug is preferred over Diagnostic since we can extend the functionality of Diagnostic
  yet only have to worry about excluding 1 class when we ship.

  Debug must be enabled via the Debug.setEnabled(bool) method for the trace()
  and pre/postCondition methods to work.  This allows one to turn debugging on and
  off quickly without having to exclude Debug during a recompile.
  */

void shellTraceShow ()
{
    Trace::getInstance()->show();
}


Trace::Trace(): Hobart::CEvolMsgReceiver("Trace"), traceCategories(), traceAllCategoriesOf()
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    outLevel = 0;
    debugEnabled = false;

	debugStartupDelay = 0;

    traceAll = false;

    appendFile = false;
    debugFile = 0;
	console = false;

    msgTooLargeCount = 0;
    overFlowCount = 0;

    showTiming_ = false;
    listTaskInfo = false;

    basetime_ = Util::CMSGTime::GetTheCurrentTime();
}

Trace::~Trace()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
	ShutdownNicely();
}

Trace *Trace::getInstance()
{
    if ( _instance_ == 0 )
    {
        util_thread::Lock< util_thread::Mutex<> > lock(_instanceMutex_);

        if ( _instance_ == 0 )
            _instance_ = new Trace();
    }

    return _instance_;
}

Trace *Trace::getInstance(Properties& prop)
{
    util_thread::Lock< util_thread::Mutex<> > lock(_instanceMutex_);

    if ( _instance_ == 0 )
    {
        if ( _instance_ == 0 )
            _instance_ = new Trace();
    }

	if ( _instance_ )
		_instance_->load(prop);

    return _instance_;
}


void Trace::load(Properties& prop)
{
    Util::CLockCriticalSection lock( traceCS );

	if ( _instance_->traceCategories.size() )
		_instance_->traceCategories.clear();
	if ( _instance_->traceAllCategoriesOf.size() )
		_instance_->traceAllCategoriesOf.clear();


    string debugFileName = "";
    const string* s_prop;
    const vector<string>* properties;
    bool enabled = false;

    s_prop = prop.getProperty("debug.traceLevel");
    if ( s_prop && s_prop->size() )
        _instance_->setTraceLevel(Integer::valueOf(*s_prop));

    s_prop = prop.getProperty("debug.showTiming");
    if ( s_prop && s_prop->size() )
        _instance_->setShowTiming(Bool::valueOf(*s_prop));

    s_prop = prop.getProperty("debug.enableTaskInfo");
    if ( s_prop && s_prop->size() )
        _instance_->enableTaskInfo(Bool::valueOf(*s_prop));

    s_prop = prop.getProperty("debug.traceAll");
    if ( s_prop && s_prop->size() )
        _instance_->setTraceAllCategories(Bool::valueOf(*s_prop));

    s_prop = prop.getProperty("debug.startupDelay");
    if ( s_prop && s_prop->size() )
        _instance_->setDebugStartupDelay(Integer::valueOf(*s_prop));

    properties = prop.getMultiProperty("debugTraceCategories");
    if ( properties && properties->size() )
    {
        vector<string>::const_iterator iter = properties->begin();
        while ( iter != properties->end() )
            _instance_->addTraceCategory(*iter++);
    }

    properties = prop.getMultiProperty("debug.traceAllCategories");
    if ( properties && properties->size() )
    {
        vector<string>::const_iterator iter = properties->begin();
        while ( iter != properties->end() )
            _instance_->setTraceAllCategoriesOf(*iter++);
    }


    s_prop = prop.getProperty("debug.appendFile");
    if ( s_prop && s_prop->size() )
        _instance_->setAppendFile(Bool::valueOf(*s_prop));

    s_prop = prop.getProperty("debugSetEnabledFile");
    if ( s_prop && s_prop->size() )
	{
		enabled = true;
        debugFileName = *s_prop;
	}

	s_prop = prop.getProperty("debugSetEnabledConsole");
	if (s_prop && s_prop->size())
	{
		enabled = true;
		_instance_->setConsoleOutput(Bool::valueOf(*s_prop));
	}

	s_prop = prop.getProperty("debugSetEnabled");
	if ( s_prop && s_prop->size())
		enabled |= Bool::valueOf(*s_prop);

	_instance_->setEnabled(enabled, debugFileName);
}


void Trace::enableTaskInfo(bool enableTaskInfo)
{
    listTaskInfo = enableTaskInfo;
}

bool Trace::isTaskInfoEnabled()
{
    return (listTaskInfo);
}

void Trace::setTraceAllCategories(bool state)
{
    traceAll = state;
}

void Trace::setEnabled(bool state, const char *filename)
{
    debugEnabled = state;
    closeDebugFile();

    if (state)
    {
        if (filename && strlen(filename))
        {
            if (appendFile)
            {
				printf("\nTrace - setEnabled - filename=%s - append", filename);
                debugFile = new ofstream(filename, ios_base::app);
                *debugFile << "Trace - setEnabled - filename=" << filename << " - append" << endl;

            }
            else
            {
				printf("\nTrace - setEnabled - filename=%s - truncate", filename);
                debugFile = new ofstream(filename, ios_base::out);
                *debugFile << "Trace - setEnabled - filename=" << filename << " - truncate" << endl;
            }
        }
    }
}

void Trace::setEnabled(bool state, const string &filename)
{
    setEnabled(state, filename.c_str());
}


bool Trace::isEnabled()
{
    return debugEnabled;
}

void Trace::closeDebugFile()
{
    Util::CLockCriticalSection lock( traceCS );
    if (debugFile)
    {
        ((ofstream *)debugFile)->close();
        delete debugFile;
        debugFile = 0;
    }
}


/**
 * Category based tracing and warning.
 * Pass in a unique String, Class or other object that supports a meaningful toString operation.
 * Then when any call to trace or warning that takes a category
 *   (eg. trace(Object category, String description))
 * is called, the trace is only displayed if an addTraceCategory() call was made with the same
 * category object.
 */

void Trace::addTraceCategory(const char *category)
{
    addTraceCategory(HobString(category));
}

void Trace::addTraceCategory(const string &category)
{
    addTraceCategory(HobString(category));
}

void Trace::addTraceCategory(const HobString &category)
{
    Util::CLockCriticalSection lock( traceCS );
    if (!isTracing(category))
        traceCategories.push_back(category);
}

void Trace::removeTraceCategory(const char *category)
{
    removeTraceCategory(HobString(category));
}

void Trace::removeTraceCategory(const string &category)
{
    removeTraceCategory(HobString(category));
}

void Trace::removeTraceCategory(const HobString &category)
{
    Util::CLockCriticalSection lock( traceCS );

    if (!traceCategories.empty())
    {
        vector<string>::iterator pos;
        pos = traceCategories.begin();
        while (pos != traceCategories.end())
        {
            if (*pos == category)
            {
                traceCategories.erase(pos);
                break;
            }
            pos++;
        }
    }
}

/**
 * A method that allows us to pass in an entry from a Properties instance
 * in the form of "catagory1,catagory2,catagory3,..."
 *
 * Example cfg file entry:
 *
 * debugCatagories=HobFieldControl,HobTreeControl,HobListControl,HobDS,HobQueryDataSet
 *
 * The above would then be parsed and the rhs would be passed in to this method.
 */
void Trace::addTraceCategories(HobString s)
{
    HobTokenizer tok(&s, " ,");
    HobString rwPart;
    while (tok.next(&rwPart) != -1)
    {
        addTraceCategory(rwPart);
    }
}

/**
 * Returns true if the catagory is currently being traced.
 */
bool Trace::isTracing(const HobString& category)
{
	if ( !debugEnabled )
		return false;

	Util::CLockCriticalSection lock( traceCS );

	if ( traceAll )
		return true;
	else if ( matchesTraceAllCategoriesOf( category ) )
		return true;

    if ( !traceCategories.empty() )
    {
        vector<string>::iterator pos( traceCategories.begin() );

        while ( pos != traceCategories.end() )
        {
            if ( category == *pos )
                return true;

            ++pos;
        }
    }

    return false;
}


void Trace::trace(const HobString& category, const HobString& description)
{
    if ( !debugEnabled )
        return;

    HobString final;

    {
		Util::CLockCriticalSection lock( traceCS );

		if ( !isTracing( category ) )
			return;

		HobString hTaskInfo;

        if ( listTaskInfo )
            hTaskInfo = "][" + Hex::toString(util_thread::Thread<>::getCurrentThreadID()) + "] ";
        else
            hTaskInfo = "] ";

        if ( showTiming_ )
        {
            char timeString[16];

#ifdef NO_SPAN_TIME
            Util::CMSGTime theTime( Util::CMSGTime::GetTheCurrentTime() );
            sprintf( timeString, "][%d:%.2d:%.2d:%.3d", theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), theTime.GetMilliseconds() );
#else
            Util::CMSGTimeSpan span( Util::CMSGTime::GetTheCurrentTime() - basetime_ );
            sprintf( timeString, "][%d:%.2d:%.2d:%.3d", span.GetHourPart(), span.GetMinutePart(), span.GetSecondPart(), span.GetMillisecondPart() );
#endif
            final = "[" + category + timeString + hTaskInfo + description;
        }
        else
        {
            final = "[" + category + hTaskInfo + description;
        }
    }

	addMessage( final );
	notifyTraceListeners( final.c_str(), final.length() );
}


void Trace::trace(int level, const HobString& description)
{
	if ( !debugEnabled )
        return;

	HobString final;

	{
		Util::CLockCriticalSection lock( traceCS );

		if ( outLevel < level )
			return;

		const HobString hLevel( Integer::toString(level) );
		HobString hTaskInfo;

		if ( listTaskInfo )
			hTaskInfo = "][" + Hex::toString( util_thread::Thread<>::getCurrentThreadID() ) + "] ";
		else
			hTaskInfo = "] ";

        if ( showTiming_ )
        {
            char timeString[16];

#ifdef NO_SPAN_TIME
            Util::CMSGTime theTime( Util::CMSGTime::GetTheCurrentTime(); )
            sprintf( timeString, "][%d:%.2d:%.2d:%.3d", theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), theTime.GetMilliseconds() );
#else
            Util::CMSGTimeSpan span( Util::CMSGTime::GetTheCurrentTime() - basetime_ );
            sprintf( timeString, "][%d:%.2d:%.2d:%.3d", span.GetHourPart(), span.GetMinutePart(), span.GetSecondPart(), span.GetMillisecondPart() );
#endif

            final = "[" + hLevel + timeString + hTaskInfo + description;
        }
        else
        {
            final = "[" + hLevel + hTaskInfo + description;
        }
	}

	addMessage( final );
	notifyTraceListeners( final.c_str(), final.length() );
}


void Trace::trace(const HobString& category, int level, const HobString& description)
{
	static int entryNumber( 0 );

	if ( !debugEnabled )
		return;

    HobString final;

    {
		Util::CLockCriticalSection lock( traceCS );

		if (( outLevel < level ) || ( !isTracing( category ) ))
			return;

		const HobString entryNumStr( Integer::toString( ++entryNumber ) );
        const HobString hLevel( Integer::toString( level ) );
		HobString hTaskInfo;

        if ( listTaskInfo )
            hTaskInfo = "][" + Hex::toString(util_thread::Thread<>::getCurrentThreadID()) + "] ";
        else
            hTaskInfo = "] ";

        if ( showTiming_ )
        {
            char timeString[16];

#ifdef NO_SPAN_TIME
            Util::CMSGTime theTime( Util::CMSGTime::GetTheCurrentTime() );
            sprintf( timeString, "][%d:%.2d:%.2d:%.3d", theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), theTime.GetMilliseconds() );
#else
            Util::CMSGTimeSpan span( Util::CMSGTime::GetTheCurrentTime() - basetime_ );
            sprintf( timeString, "][%d:%.2d:%.2d:%.3d", span.GetHourPart(), span.GetMinutePart(), span.GetSecondPart(), span.GetMillisecondPart() );
#endif

            final = "[" + entryNumStr + "]" + "[" + category + "." + hLevel + timeString + hTaskInfo  + description;
        }
        else
        {
            final = "[" + entryNumStr + "]" + "[" + category + "." + hLevel + hTaskInfo + description;
        }
    }

	addMessage( final );
	notifyTraceListeners( final.c_str(), final.length() );
}

void Trace::trace(const HobString& category, int level, const HobString& fileName,
        const HobString& functionName, int lineNumber, const HobString& description)
{
    if ( !debugEnabled )
        return;

    HobString final;

    {
		Util::CLockCriticalSection lock( traceCS );

		if (( outLevel < level ) || ( !isTracing( category ) ))
			return;

		const HobString hLineNo( Integer::toString(lineNumber) );
		const HobString hLevel( Integer::toString(level) );
		const HobString first( "["  + fileName + ":" + hLineNo + "][" );
		const HobString second( functionName + "][" );
		HobString hTaskInfo;

        if ( listTaskInfo )
            hTaskInfo = "][" + HobString(Hex::toString(util_thread::Thread<>::getCurrentThreadID())) + "] ";
        else
            hTaskInfo = "] ";

        if ( showTiming_ )
        {
            char timeString[16];

#ifdef NO_SPAN_TIME
            Util::CMSGTime theTime( Util::CMSGTime::GetTheCurrentTime() );
            sprintf( timeString, "][%d:%.2d:%.2d:%.3d", theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), theTime.GetMilliseconds() );
#else
            Util::CMSGTimeSpan span( Util::CMSGTime::GetTheCurrentTime() - basetime_ );
            sprintf( timeString, "][%d:%.2d:%.2d:%.3d", span.GetHourPart(), span.GetMinutePart(), span.GetSecondPart(), span.GetMillisecondPart() );
#endif
            final = first + second + category + "." + hLevel + timeString + hTaskInfo + description;
        }
        else
        {
            final = first + second + category + "." + hLevel + hTaskInfo + description;
        }
    }

	addMessage( final );
	notifyTraceListeners( final.c_str(), final.length() );
}

void Trace::trace(const char * category, int level, const char * fileName,
        const char *className, const char *functionName, int lineNumber,
        const char *description)
{

    trace (HobString (category), level, HobString (fileName), HobString (className), HobString(functionName),
            lineNumber, HobString(description));
}

void Trace::trace(const HobString& category, int level, const HobString& fileName,
        const HobString& className, const HobString& functionName, int lineNumber,
        const HobString& description)
{
    if ( !debugEnabled )
        return;

    HobString final;

    {
		Util::CLockCriticalSection lock( traceCS );

		if (( outLevel < level ) || ( !isTracing( category ) ))
			return;

		const HobString hLineNo( Integer::toString(lineNumber) );
		const HobString hLevel( Integer::toString(level) );
		const HobString first( "["  + fileName + ":" + hLineNo + "][" );
		const HobString second( className + "::" + functionName + "][" );
		HobString hTaskInfo;

        if ( listTaskInfo )
            hTaskInfo = "][" + HobString(Hex::toString(util_thread::Thread<>::getCurrentThreadID())) + "] ";
        else
            hTaskInfo = "] ";

        if ( showTiming_ )
        {
            char timeString[16];

#ifdef NO_SPAN_TIME
            Util::CMSGTime theTime( Util::CMSGTime::GetTheCurrentTime() );
            sprintf( timeString, "][%d:%.2d:%.2d:%.3d", theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), theTime.GetMilliseconds() );
#else
            Util::CMSGTimeSpan span( Util::CMSGTime::GetTheCurrentTime() - basetime_ );
            sprintf( timeString, "][%d:%.2d:%.2d:%.3d", span.GetHourPart(), span.GetMinutePart(), span.GetSecondPart(), span.GetMillisecondPart() );
#endif
            final = first + second + category + "." + hLevel + timeString + hTaskInfo + description;
        }
        else
        {
            final = first + second + category + "." + hLevel + hTaskInfo + description;
        }
    }

    addMessage( final );
    notifyTraceListeners( final.c_str(), final.length() );
}

void Trace::addMessage(const HobString& msg)
{
    //maybe need to send to Session Manager
    send ( msg.c_str(), (uint32_t)msg.length() ); // Hobart::CEvolMsgReceiver::send()
}

void Trace::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
    if (debugFile)
    {
        *debugFile <<  msg->Data() << endl << flush;
    }
    else
    {
        PAL_absclass::PAL_OutputDbgStrA( msg->CStr());
#if !defined ( __linux__ )
        PAL_absclass::PAL_OutputDbgStrA( "\r\n" );
#endif
    }

	if (console)
	{
		cout << msg->Data() << endl;
	}
}

void Trace::show()
{
    //cout << "Trace stats: overflow counts: " <<  overFlowCount << endl << "too large counts: " << msgTooLargeCount << endl << "messages in queue: " << pThreadQ->numMsgs() << endl;
}

/**
 * Set the minimum threshold for trace and warning output.
 *         0 is highest level
 *   +maxint is lowest level
 *
 * Setting this level to -1 will effectively turn off traces and warnings.
 * NOTE: this does not effect direct access to 'out', nor does it effect output in the
 *       checking functions. Use enableOutput() for that.
 */
void Trace::setTraceLevel(int level)
{
    outLevel = level;
}

int Trace::getTraceLevel()
{
    return outLevel;
}

/**
 * Add this string to the traceAllCategoriesOf vector.
 */
void Trace::setTraceAllCategoriesOf(const char *category)
{
    setTraceAllCategoriesOf(HobString(category));
}

/**
 * Add this string to the traceAllCategoriesOf vector.
 */
void Trace::setTraceAllCategoriesOf(const string &category)
{
    setTraceAllCategoriesOf(HobString(category));
}

/**
 * Add this string to the traceAllCategoriesOf vector.
 */
void Trace::setTraceAllCategoriesOf(const HobString &category)
{

    Util::CLockCriticalSection lock( traceCS );

    HobString setCategory = category;
#if defined(LINUX_FIX)
    size_t len = setCategory.length();
#else
    int len = setCategory.length();
#endif
    if (setCategory.at(len - 1) != '.')
        setCategory.append(".");

    if (!matchesTraceAllCategoriesOf(setCategory))
        traceAllCategoriesOf.push_back(setCategory);
}

/**
 * Remove this string from the traceAllCategoriesOf vector.
 */
void Trace::clearTraceAllCategoriesOf(const char *category)
{
    clearTraceAllCategoriesOf(HobString(category));
}

/**
 * Remove this string from the traceAllCategoriesOf vector.
 */
void Trace::clearTraceAllCategoriesOf(const string &category)
{
    clearTraceAllCategoriesOf(HobString(category));
}

/**
 * Remove this string from the traceAllCategoriesOf vector.
 */
void Trace::clearTraceAllCategoriesOf(const HobString& category)
{

    Util::CLockCriticalSection lock( traceCS );

    if (!traceAllCategoriesOf.empty())
    {
        HobString clearCategory = category;
#if defined(LINUX_FIX)
        size_t len = clearCategory.length();
#else
        int len = clearCategory.length();
#endif
        if (clearCategory.at(len - 1) != '.')
            clearCategory.append(".");

        vector<string>::iterator pos;
        pos = traceAllCategoriesOf.begin();
        while (pos != traceAllCategoriesOf.end())
        {
            if (*pos == clearCategory)
            {
                traceAllCategoriesOf.erase(pos);
                break;
            }
            pos++;
        }
    }
}

/**
 * Checks to see if the string is in the traceAllCategoriesOf vector.
 * traceAllCategoriesOf allows us to turn on a trace category and all
 * of its subcategories. For example, if "Product" is contained in
 * traceAllCategoriesOf, all trace categories that begin with "Product"
 * will immediately be turned on.
 * (Product, Product.Graphic, Product.Class, etc. - All will be enabled.)
 */
bool Trace::matchesTraceAllCategoriesOf(const HobString &category)
{
    Util::CLockCriticalSection lock( traceCS );

    if (!traceAllCategoriesOf.empty())
    {
        HobString setCategory = category;
        if ( setCategory.at(setCategory.length() - 1) != '.' )
            setCategory.append(".");

        vector<string>::iterator pos = traceAllCategoriesOf.begin();
        while ( pos != traceAllCategoriesOf.end() )
        {
            if ( setCategory.find(*pos, 0) == 0 )
                return(true);

            pos++;
        }
    }

    return(false);
}

void Trace::addTraceListener(CommMechanism *listener)
{
    Util::CLockCriticalSection lock( traceCS );

    if (!listener)
        return;

    bool shouldAdd = true;

    if (!traceListeners.empty())
    {
        vector<CommMechanism *>::iterator iter;
        iter = find(traceListeners.begin(), traceListeners.end(), listener);
        if (iter != traceListeners.end())
            shouldAdd = false;
    }

    if (shouldAdd)
        traceListeners.push_back(listener);
}

void Trace::removeTraceListener(CommMechanism *listener)
{
    Util::CLockCriticalSection lock( traceCS );

    if (!listener)
        return;

    if (!traceListeners.empty())
    {
        vector<CommMechanism *>::iterator iter;
        iter = find(traceListeners.begin(), traceListeners.end(), listener);
        if (iter != traceListeners.end())
            traceListeners.erase(iter);
    }
}

#if defined(LINUX_FIX)
void Trace::notifyTraceListeners(const char *msg, size_t msgLength)
#else
void Trace::notifyTraceListeners(const char *msg, unsigned int msgLength)
#endif
{
    Util::CLockCriticalSection lock( traceCS );
    if (!traceListeners.empty())
    {
        string data = "100,100,";
        data.append(msg, msgLength);
#if defined(LINUX_FIX)
        for (size_t i = 0; i < traceListeners.size(); i++)
#else
            for (unsigned int i = 0; i < traceListeners.size(); i++)
#endif
                traceListeners[i]->cmSend(data.c_str(), (int)data.length());
    }
}
