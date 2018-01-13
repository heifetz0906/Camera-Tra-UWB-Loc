#ifndef gdprint_h__
#define gdprint_h__

/*************************************************************************************************************
Provides following macros:

  DPRINT(tag, ...)          // Send ... to debug print with tag
  GDPRINT(...)              // Send ... to debug print with default tag for qcvGraph, i.e., "qcvGraph"

Debug print can be supressed . To suppress DPRINT and GDPRINT, define macro:
  DPRINT_TARGET
     0           DPRINT is suppressed as NOP, with zero code and run-time cost.
     1           DPRINT uses default target.
                     Android: logcat
                     Windows: std output
                     Linux:   std output
     2           DPRINT always uses std output
  If DPRINT_TARGET is not defined, it defaults to 1.

Usage example 1:
  #include <gdprint.h>
  DPRINT("mytag", "this is a message %d\n", 123);
  GDPRINT("this is a qcvGraph message %d\n", 456);

Usage example 2: (File scope DPRINT)
  #include <gdprint.h>
  #ifdef FDPRINT
    #undef FDPRINT
  #endif
  #define FDPRINT(...) DPRINT(__FILE__, __VA_ARGS__)
  FDPRINT("this is a file scope message %d\n", 123);

Usage example 3:
  #define DPRINT_TARGET 2          // or use compiler option for macro
  #include <gdprint.h>
  DPRINT("mytag", "this is a message %d\n", 123);   // always prints to stdout.

*************************************************************************************************************/

#define DPRINT(tag, ...)   
#define GDPRINT(...)


/*
/////////////////////////////////////////////////////////////////////////////////////////
// DPRINT_TARGET__ is normalized version of DPRINT_TARGET
#ifdef DPRINT_TARGET__
  #undef DPRINT_TARGET__
#endif
#ifdef DPRINT_TARGET
  #define DPRINT_TARGET__  DPRINT_TARGET
#else
  #define DPRINT_TARGET__  1
#endif 

/////////////////////////////////////////////////////////////////////////////////////////
// DPRINT
#ifdef DPRINT
    #undef DPRINT
#endif
#if DPRINT_TARGET__ == 0
    // suppress DPRINT
    #define DPRINT(tag, ...)  
#elif DPRINT_TARGET__ == 2
    // to std output
    #include <cstdio>
    // tag is however gone. sorry.
    #define DPRINT(tag, ...)  std::printf(__VA_ARGS__)
#elif DPRINT_TARGET__ == 1
    // default target
    #ifdef __ANDROID__        
        #include <android/log.h>
        #define DPRINT(tag, ...)  __android_log_print(ANDROID_LOG_DEBUG, tag, __VA_ARGS__)
    #else
        #include <cstdio>
        // tag is however gone. sorry.
        #define DPRINT(tag, ...)  std::printf(__VA_ARGS__)
    #endif
#else
    #error DPRINT_TARGET must be one of 0(suppressed), 1(default target), and 2(stdout).
#endif


/////////////////////////////////////////////////////////////////////////////////////////
// GDPRINT
#ifdef GDPRINT
    #undef GDPRINT
#endif
#define GDPRINT(...)   DPRINT("qcvGraph", __VA_ARGS__)


////////////////////////////////////////////////////////////////////////
//PRINTF
#ifndef _WIN32
#include <android/log.h>
//==============================================================================
// Declarations
//==============================================================================
#define DPRINTF(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define IPRINTF(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define EPRINTF(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define PRINTF	DPRINTF
#else
#define PRINTF	printf
#endif
*/


/////////////////////////////////////////////////////////////////////////////////////////
#endif //gdprint_h__
