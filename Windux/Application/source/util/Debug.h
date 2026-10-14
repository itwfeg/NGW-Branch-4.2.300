/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

// Based on the Debug class from TNP

#ifndef DEBUG_H
#define DEBUG_H

#include "PAL_platformheader.h"

#if     defined( __linux__ )
#include <stdint.h>
#endif
#include <fstream>
#include <string>
#include <vector>

class HobString;

/**
  Debug is used to help developers identify and localize problems in the code.

  Debug is preferred over Diagnostic since we can extend the functionality of Diagnostic
  yet only have to worry about excluding 1 class when we ship.

  Debug must be enabled via the Debug.setEnabled(bool) method for the trace()
  and pre/postCondition methods to work.  This allows one to turn debugging on and
  off quickly without having to exclude Debug during a recompile.
  */
class Debug
{
    public:

        static bool isEnabled();
        static void setEnabled(bool state, const char *filename = (char *)NULL);

        static void closeDebugFile();
        /**
         * A method that allows us to pass in an entry from a Properties instance
         * in the form of "catagory1,catagory2,catagory3,..."
         *
         * Example cfg file entry:
         * debugCatagories=HobFieldControl,HobTreeControl,HobListControl,HobDS,HobQueryDataSet
         *
         * The above would then be parsed and the rhs would be passed in to this method.
         */
        static void addTraceCategories(HobString s);


        /**
         * Returns true if the catagory is currently being traced.
         */
        static bool isTracing(const char* category);
        static bool isTracing(const std::string& category);
        static bool isTracing(const HobString& category);


        /**
         * Set the minimum threshold for trace and warning output.
         *         0 is highest level
         *   +maxint is lowest level
         *
         * Setting this level to -1 will effectively turn off traces and warnings.
         * NOTE: this does not effect direct access to 'out', nor does it effect output in the
         *       checking functions. Use enableOutput() for that.
         */
        static int getTraceLevel();
        static void setTraceLevel(int level);


        /**
         * Category based tracing and warning.
         * Pass in a unique String, Class or other object that supports a meaningful toString operation.
         * Then when any call to trace or warning that takes a category
         *   (eg. trace(Object category, String description))
         * is called, the trace is only displayed if an addTraceCategory() call was made with the same
         * category object.
         */
#if defined(LINUX_FIX)
        static void addTraceCategory(const char *category);
        static void addTraceCategory(const std::string category);
#else
        static void addTraceCategory(char *category);
        static void addTraceCategory(std::string category);
#endif
        static void addTraceCategory(const HobString& category);
#if defined(LINUX_FIX)
        static void removeTraceCategory(const char *category);
        static void removeTraceCategory(const std::string category);
#else
        static void removeTraceCategory(char *category);
        static void removeTraceCategory(std::string category);
#endif
        static void removeTraceCategory(const HobString& category);

        static void setTraceAllCategories(bool state);

        /**
         * Output a trace if the threashold level is high enough and general output is enabled
         */
        static void trace(int level, const HobString& description);

        /**
         * Output a trace if the category is enabled and general output is enabled
         */
        static void trace(const char *category, const char *description);
        static void trace(const char *category, const std::string& description);
        static void trace(const char *category, const HobString& description);
        static void trace(const std::string& category, const std::string& description);
        static void trace(const HobString& category, const HobString& description);

        /**
         * Output a trace if the category is enabled and general output is enabled
         */
        static void trace(const HobString& category, int level, const HobString& description);
        static void trace(const char * category, int level, const std::string& description);
        static void trace(const std::string& category, int level, const std::string& description);
        static void trace(const HobString& category, int level, const HobString& fileName,
                const HobString& className, const HobString& functionName,
                int lineNumber, const HobString& description);
        static void trace(const char * category, int level, const char * fileName,
                const char *className, const char *functionName, int lineNumber,
                const char *description);

        /**
         * print a message to the diagnostic out stream
         */
        static void println(const HobString& message);

        /**
         * print a message to the diagnostic out stream
         */
        static void print(const HobString& message);

        /**
         * print a message to the diagnostic out stream preceded
         * by a line number (incremented count) and a tab character.
         */
        static void printlnc(const HobString& message);

        static void setTraceAllCategoriesOf(char *category);
        static void setTraceAllCategoriesOf(std::string category);
        static void setTraceAllCategoriesOf(const HobString &category);
        static void clearTraceAllCategoriesOf(char *category);
        static void clearTraceAllCategoriesOf(std::string category);
        static void clearTraceAllCategoriesOf(const HobString& category);

        static bool isRunning();

    private:

        static int count;
};

#if     defined( WIN32 )
std::wstring GetHexString( const char * str, unsigned __int32 length );
#elif   defined( __linux__ )
std::wstring GetHexString( const char * str, uint32_t length );
#endif
#ifdef _DEBUG
#define DBGASSERT(a) if (!(a)) DebugBreak()
#else
#define DBGASSERT
#endif
#endif // DEBUG_H
