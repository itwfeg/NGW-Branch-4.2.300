/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */


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

#include "Debug.h"
#include "Trace.h"
#include "Integer.h"
#include "HobString.h"

int Debug::count = 0;


bool Debug::isRunning()
{
	return Trace::isRunning();
}

bool Debug::isEnabled()
{
    Trace *pTrace = Trace::getInstance();

    return(pTrace->isEnabled());
}


void Debug::setEnabled(bool state, const char *filename)
{
    Trace *pTrace = Trace::getInstance();

    pTrace->setEnabled(state, filename);
}


void Debug::closeDebugFile()
{
    Trace *pTrace = Trace::getInstance();

    pTrace->closeDebugFile();
}


/**
 * Category based tracing and warning.
 * Pass in a unique String, Class or other object that supports a meaningful toString operation.
 * Then when any call to trace or warning that takes a category
 *   (eg. trace(Object category, String description))
 * is called, the trace is only displayed if an addTraceCategory() call was made with the same
 * category object.
 */
#if defined(LINUX_FIX)
void Debug::addTraceCategory(const char *category)
#else
void Debug::addTraceCategory(char *category)
#endif
{
    Trace *pTrace = Trace::getInstance();

    pTrace->addTraceCategory(category);
}


#if defined(LINUX_FIX)
void Debug::addTraceCategory(const std::string category)
#else
void Debug::addTraceCategory(std::string category)
#endif
{
    Trace *pTrace = Trace::getInstance();

    pTrace->addTraceCategory(category);
}


void Debug::addTraceCategory(const HobString& category)
{
    Trace *pTrace = Trace::getInstance();

    pTrace->addTraceCategory(category);
}


#if defined(LINUX_FIX)
void Debug::removeTraceCategory(const char *category)
#else
void Debug::removeTraceCategory(char *category)
#endif
{
    Trace *pTrace = Trace::getInstance();

    pTrace->removeTraceCategory(category);
}


#if defined(LINUX_FIX)
void Debug::removeTraceCategory(const std::string category)
#else
void Debug::removeTraceCategory(std::string category)
#endif
{
    Trace *pTrace = Trace::getInstance();

    pTrace->removeTraceCategory(category);

}


void Debug::removeTraceCategory(const HobString& category)
{
    Trace *pTrace = Trace::getInstance();

    pTrace->removeTraceCategory(category);
}


void Debug::setTraceAllCategories(bool state)
{
    Trace *pTrace = Trace::getInstance();

    pTrace->setTraceAllCategories(state);
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
void Debug::addTraceCategories(HobString s)
{
    Trace *pTrace = Trace::getInstance();

    pTrace->addTraceCategories(s);
}


/**
 * Returns true if the catagory is currently being traced.
 */
bool Debug::isTracing(const char* category)
{
    return Debug::isTracing(HobString(category));
}


bool Debug::isTracing(const std::string& category)
{
    return Debug::isTracing(HobString(category));
}


bool Debug::isTracing(const HobString& category)
{
    Trace *pTrace = Trace::getInstance();

    return (pTrace->isTracing(category));
}


/**
 * Output a trace if the threashold level is high enough and general output is enabled
 */
void Debug::trace(int level, const HobString& description)
{
    Trace::getInstance()->trace(level, description);
}


/**
 * Output a trace if the category is enabled and general output is enabled
 */
void Debug::trace(const char *category, const char *description)
{
	HobString c( category );
	HobString d( description );

    trace( c, d );
}


void Debug::trace(const char *category, const std::string& description)
{
	HobString c( category );
	HobString d( description );

    trace( c, d );
}


void Debug::trace(const char *category, const HobString& description)
{
	HobString c( category );

    trace( c, description );
}


void Debug::trace(const std::string& category, const std::string& description)
{
	HobString c( category );
	HobString d( description );

    trace( c, d );
}


void Debug::trace(const HobString& category, const HobString& description)
{
    Trace::getInstance()->trace( category, description );
}


/**
 * Output a trace if the category is enabled and general output is enabled
 */
void Debug::trace(const HobString& category, int level, const HobString& description)
{
    Trace::getInstance()->trace(category, level, description);
}


void Debug::trace(const char * category, int level, const std::string& description)
{
    Trace::getInstance()->trace(category, level, description);
}


void Debug::trace(const std::string& category, int level, const std::string& description)
{
    Trace::getInstance()->trace(category, level, description);
}


void Debug::trace(const HobString& category, int level, const HobString& fileName,
        const HobString& className, const HobString& functionName, int lineNumber,
        const HobString& description)
{
    Trace::getInstance()->trace(category, level, fileName, className, functionName, lineNumber, description);
}


void Debug::trace(const char * category, int level, const char * fileName,
        const char *className, const char *functionName, int lineNumber,
        const char *description)
{
    Trace::getInstance()->trace(category, level, fileName, className, functionName, lineNumber, description);
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
void Debug::setTraceLevel(int level)
{
    Trace *pTrace = Trace::getInstance();

    pTrace->setTraceLevel(level);

}


int Debug::getTraceLevel()
{
    Trace *pTrace = Trace::getInstance();

    return (pTrace->getTraceLevel());

}


/**
 * print a message to the diagnostic out stream
 */
void Debug::println(const HobString& UNUSED(message))
{
    //cout << message << endl;
}


/**
 * print a message to the diagnostic out stream
 */
void Debug::print(const HobString& UNUSED(message))
{
    // cout << message;
}


/**
 * print a message to the diagnostic out stream preceded
 * by a line number (incremented count) and a tab character.
 */
void Debug::printlnc(const HobString& UNUSED(message))
{
    //cout << ++count << "\t" << message << endl;
}


/**
 * Add this string to the traceAllCategoriesOf vector in Trace.
 */
void Debug::setTraceAllCategoriesOf(char *category)
{
    Trace *pTrace = Trace::getInstance();

    pTrace->setTraceAllCategoriesOf(category);
}


/**
 * Add this string to the traceAllCategoriesOf vector in Trace.
 */
void Debug::setTraceAllCategoriesOf(std::string category)
{
    Trace *pTrace = Trace::getInstance();

    pTrace->setTraceAllCategoriesOf(category);
}


/**
 * Add this string to the traceAllCategoriesOf vector in Trace.
 */
void Debug::setTraceAllCategoriesOf(const HobString& category)
{
    Trace *pTrace = Trace::getInstance();

    pTrace->setTraceAllCategoriesOf(category);
}


/**
 * Remove this string from the traceAllCategoriesOf vector in Trace.
 */
void Debug::clearTraceAllCategoriesOf(char *category)
{
    Trace *pTrace = Trace::getInstance();

    pTrace->clearTraceAllCategoriesOf(category);
}


/**
 * Remove this string from the traceAllCategoriesOf vector in Trace.
 */
void Debug::clearTraceAllCategoriesOf(std::string category)
{
    Trace *pTrace = Trace::getInstance();

    pTrace->clearTraceAllCategoriesOf(category);
}


/**
 * Remove this string from the traceAllCategoriesOf vector in Trace.
 */
void Debug::clearTraceAllCategoriesOf(const HobString& category)
{
    Trace *pTrace = Trace::getInstance();

    pTrace->clearTraceAllCategoriesOf(category);
}

// TODO fix this
//std::wstring GetHexString( const char * str, unsigned __int32 length )
std::wstring GetHexString( const char * str, DWORD length )
{
    std::wstring msg = L"\r\n\t";
    const char * ptr = str;
    wchar_t s[10];
    for ( DWORD i = 0; i < length; i++ )
    {
        if ( i > 0 && i % 20 == 0 )
            msg += L"\r\n\t";
        else if ( i > 0 && i %10 == 0 )
            msg += L". ";

#if defined ( __linux__ )
        swprintf( s, 10, L"%02X ", ( unsigned char ) *ptr );
#else
        wsprintf( s, L"%02X ", ( unsigned char ) *ptr );
#endif
        ptr++;
        msg += s;
    }

    msg += L"\r\n";

    return msg;
}
