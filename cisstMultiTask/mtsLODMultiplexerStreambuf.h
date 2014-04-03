/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2011-08-04

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _mtsLODMultiplexerStreambuf_h
#define _mtsLODMultiplexerStreambuf_h

#include <cisstCommon/cmnLODMultiplexerStreambuf.h>
#include <cisstCommon/cmnCallbackStreambuf.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaMutex.h>

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_WINDOWS)
#include <map>
#elif (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_XENOMAI)
#include <vector>
#endif

class mtsLODMultiplexerStreambuf : public cmnLODMultiplexerStreambuf<char>
{
public:
    typedef cmnLODMultiplexerStreambuf<char> BaseType;
    typedef std::char_traits<char> TraitType;
    typedef cmnCallbackStreambuf<char> PerThreadChannelType; // per thread
    typedef BaseType::int_type IntType;

protected:
    // MJ: Use std::vector instead of std::map on Mac and Xenomai
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_WINDOWS)
    // Map (osaThread, PerThreadBufferType)
    typedef std::map<osaThreadId, PerThreadChannelType*, osaThreadId> PerThreadChannelMapType;
    typedef std::pair<PerThreadChannelMapType::iterator, bool> PerThreadChannelMapPairType;
    PerThreadChannelMapType PerThreadChannelMap;
#elif (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_XENOMAI)
    typedef struct {
        osaThreadId ThreadId;
        PerThreadChannelType * PerThreadChannel;
    } PerThreadChannelElementType;
    typedef std::vector<PerThreadChannelElementType> PerThreadChannelContainerType;
    PerThreadChannelContainerType PerThreadChannelContainer;
#endif

    /*! To access internal container thread-safely */
    osaMutex PerThreadChannelMapSync;

    // Override these methods for log dispatch based on caller's thread id
    std::streamsize xsputn(const char * s, std::streamsize n, cmnLogLevel level);
    std::streamsize xsputn(const char *s, std::streamsize n);

    IntType overflow(IntType c, cmnLogLevel level);
    IntType overflow(IntType c = mtsLODMultiplexerStreambuf::TraitType::eof());

    int sync(void);

    /*! Get (per-thread) log channel for a thread given. If thread id was not
        registered yet, create new one. */
    PerThreadChannelType * GetThreadChannel(const osaThreadId& threadId);

public:
   mtsLODMultiplexerStreambuf();
   ~mtsLODMultiplexerStreambuf();
};

#endif
