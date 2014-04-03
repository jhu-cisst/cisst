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

#include <cisstMultiTask/mtsLODMultiplexerStreambuf.h>
#include <cisstMultiTask/mtsManagerLocal.h>

#include <iostream>

mtsLODMultiplexerStreambuf::mtsLODMultiplexerStreambuf(){}

mtsLODMultiplexerStreambuf::~mtsLODMultiplexerStreambuf()
{
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_WINDOWS)
    PerThreadChannelMapType::const_iterator it = PerThreadChannelMap.begin();
    const PerThreadChannelMapType::const_iterator itEnd = PerThreadChannelMap.end();
    for (; it != itEnd; ++it) {
        if (it->second) {
            cmnLogger::GetMultiplexer()->RemoveChannel(it->second);
            delete it->second;
        }
    }
#elif (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_XENOMAI)
    PerThreadChannelType * channel;
    for (size_t i = 0; i < PerThreadChannelContainer.size(); ++i) {
        channel = PerThreadChannelContainer[i].PerThreadChannel;
        if (channel) {
            cmnLogger::GetMultiplexer()->RemoveChannel(channel);
            delete channel;
        }
    }
#endif
}

mtsLODMultiplexerStreambuf::PerThreadChannelType * mtsLODMultiplexerStreambuf::GetThreadChannel(const osaThreadId& threadId)
{
    // if already registered thread, return registered channel
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_WINDOWS)
    PerThreadChannelMapType::const_iterator it = PerThreadChannelMap.find(threadId);
    if (it != PerThreadChannelMap.end()) {
        return it->second;
    }

    PerThreadChannelMapSync.Lock();

#elif (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_XENOMAI)
    // MJ: Vector iteration may take longer than map.
    PerThreadChannelMapSync.Lock();

    for (size_t i = 0; i < PerThreadChannelContainer.size(); ++i) {
        if (PerThreadChannelContainer[i].ThreadId.Equal(threadId)) {
            PerThreadChannelMapSync.Unlock();
            return PerThreadChannelContainer[i].PerThreadChannel;
        }
    }
#endif

    // create new log channel for new thread if not registered yet
    PerThreadChannelType * perThreadChannel = new cmnCallbackStreambuf<char>(mtsManagerLocal::LogDispatcher);

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_WINDOWS)
    PerThreadChannelMapPairType ret =  PerThreadChannelMap.insert(std::make_pair(threadId, perThreadChannel));
    if (!ret.second) { // already registered thread
        delete perThreadChannel;
        it = PerThreadChannelMap.find(threadId);
        CMN_ASSERT(it != PerThreadChannelMap.end());

        PerThreadChannelMapSync.Unlock();

        return it->second;
    }
#elif (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_XENOMAI)
    PerThreadChannelElementType element;
    element.ThreadId = threadId;
    element.PerThreadChannel = perThreadChannel;
    PerThreadChannelContainer.push_back(element);
#endif

    this->AddChannel(perThreadChannel, CMN_LOG_ALLOW_ALL);

    PerThreadChannelMapSync.Unlock();

    return perThreadChannel;
}

std::streamsize mtsLODMultiplexerStreambuf::xsputn(const char * s, std::streamsize n, cmnLogLevel CMN_UNUSED(level))
{
    PerThreadChannelType * perThreadChannel = GetThreadChannel(osaGetCurrentThreadId());
    CMN_ASSERT(perThreadChannel);

    std::streamsize ssize(0);
    //if (level) { // MJ TODO: implement log masking later
        ssize = perThreadChannel->sputn(s, n);
    //}
    return ssize;
}

std::streamsize mtsLODMultiplexerStreambuf::xsputn(const char *s, std::streamsize n)
{
    PerThreadChannelType * perThreadChannel = GetThreadChannel(osaGetCurrentThreadId());
    CMN_ASSERT(perThreadChannel);

    return perThreadChannel->sputn(s, n);
}

int mtsLODMultiplexerStreambuf::sync(void)
{
    PerThreadChannelType * perThreadChannel = GetThreadChannel(osaGetCurrentThreadId());
    CMN_ASSERT(perThreadChannel);

    perThreadChannel->pubsync();

    return 0;
}

mtsLODMultiplexerStreambuf::IntType mtsLODMultiplexerStreambuf::overflow(mtsLODMultiplexerStreambuf::IntType c, cmnLogLevel CMN_UNUSED(level))
{
    // follow the basic_streambuf standard
    if (TraitType::eq_int_type(TraitType::eof(), c)) {
        return (TraitType::not_eof(c));
    }

    PerThreadChannelType * perThreadChannel = GetThreadChannel(osaGetCurrentThreadId());
    CMN_ASSERT(perThreadChannel);

    //if (level) { // MJ TODO: implement log masking later
        perThreadChannel->sputc(TraitType::to_char_type(c));
    //}
 
    return TraitType::not_eof(c);
}

mtsLODMultiplexerStreambuf::IntType mtsLODMultiplexerStreambuf::overflow(mtsLODMultiplexerStreambuf::IntType c)
{
    // follow the basic_streambuf standard
    if (TraitType::eq_int_type(TraitType::eof(), c)) {
        return (TraitType::not_eof(c));
    }

    // multiplexing
    PerThreadChannelType * perThreadChannel = GetThreadChannel(osaGetCurrentThreadId());
    CMN_ASSERT(perThreadChannel);

    perThreadChannel->sputc(TraitType::to_char_type(c));

    return TraitType::not_eof(c);
}
