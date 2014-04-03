/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2008-01-04

  (C) Copyright 2002-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*! \file 
  \brief Definitions of osaThreadedLogFile and osaThreadedLogFileStreambuf
*/

#ifndef _osaThreadedLogFile_h
#define _osaThreadedLogFile_h

#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaMutex.h>

#include <list>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>


/*! \brief
  
   \sa C++ manual on basic_ostream and basic_streambuf, cmnLogger
 */
template<class _element, class _trait = std::char_traits<_element> >
class osaThreadedLogFileStreambuf: public std::basic_streambuf<_element, _trait>
{
public:
  
    typedef std::basic_streambuf<_element, _trait> BaseClassType;

    typedef std::basic_streambuf<_element, _trait> ChannelType;

    typedef std::pair<osaThreadId, ChannelType *> ElementType;
  
    /*! Type of internal data structure storing the channels. */
    typedef std::list<ElementType> ChannelContainerType;
	typedef typename ChannelContainerType::iterator iterator;
	typedef typename ChannelContainerType::const_iterator const_iterator;
  
    /*!  Create the stream buf with the file name prefix.  Does
      nothing else than storing the file name prefix.
     */
    osaThreadedLogFileStreambuf(const std::string & filePrefix):
        FilePrefix(filePrefix),
        Mutex()
    {}

    /*! Create a file for the current thread and add its buffer to the list of channels. */
    ChannelType * AddChannelForThread(const osaThreadId & threadId);


// Here we provide basic_streambuf namesame methods for multiplexing.
// This part is declared 'protected' following the declarations of 
// basic_streambuf. See basic_streambuf documentation for more
// details.
protected:
    typedef typename std::basic_streambuf<_element, _trait>::int_type int_type;
  
    /*! Override the basic_streambuf sync for the current file
      output. */
    virtual int sync();
  
    /*! Override the basic_streambuf xsputn for the current file
      output. */
    virtual std::streamsize xsputn(const _element *s, std::streamsize n);
  
    /*! Override the basic_streambuf overflow. overflow() is called
      when sputc() discovers it does not have space in the storage
      buffer. In our case, it's always. See more on it in the
      basic_streambuf documentation.
     */
    virtual int_type overflow(int_type c = _trait::eof());
  
private:
    /*! Prefix used to create new files */
    std::string FilePrefix;

    /*! Mutex used to protect ChannelContainer. */
    osaMutex Mutex;

    /*! The actual container that stores channel addresses. */
    ChannelContainerType ChannelContainer;
  
    /*! Find a channel in the container and return the container's
      iterator for the element with that channel.
     */
    iterator FindChannel(const osaThreadId & threadId);
};



template<class _element, class _trait>
int osaThreadedLogFileStreambuf<_element, _trait>::sync(void)
{
    const osaThreadId threadId = osaGetCurrentThreadId();
    Mutex.Lock();
    iterator it = FindChannel(threadId);
    if (it != ChannelContainer.end()) {
        Mutex.Unlock();
        return it->second->pubsync();
    }
    ChannelType * channel = AddChannelForThread(threadId);
    Mutex.Unlock();
    return channel->pubsync();
}


template<class _element, class _trait>
std::streamsize
osaThreadedLogFileStreambuf<_element, _trait>::xsputn(const _element *s, std::streamsize n)
{
    const osaThreadId threadId = osaGetCurrentThreadId();
    Mutex.Lock();
    const iterator it = FindChannel(threadId);
    if (it != ChannelContainer.end()) {
        Mutex.Unlock();
        return it->second->sputn(s, n);
    }
    ChannelType * channel = AddChannelForThread(threadId);
    Mutex.Unlock();
    return channel->sputn(s, n);
}


template<class _element, class _trait>
typename osaThreadedLogFileStreambuf<_element, _trait>::int_type 
osaThreadedLogFileStreambuf<_element, _trait>::overflow(int_type c)
{
    // follow the basic_streambuf standard
    if (_trait::eq_int_type(_trait::eof(), c))
        return (_trait::not_eof(c));

    const osaThreadId threadId = osaGetCurrentThreadId();
    Mutex.Lock();
    const iterator it = FindChannel(threadId);
    if (it != ChannelContainer.end()) {
        Mutex.Unlock();
        return it->second->sputc( _trait::to_char_type(c) );
    }
    ChannelType * channel = AddChannelForThread(threadId);
    Mutex.Unlock();
    return channel->sputc( _trait::to_char_type(c) );
}


template<class _element, class _trait>
typename osaThreadedLogFileStreambuf<_element, _trait>::iterator
osaThreadedLogFileStreambuf<_element, _trait>::FindChannel(const osaThreadId & threadId)
{
    iterator it = ChannelContainer.begin();
    while (it != ChannelContainer.end()) {
        if ((*it).first == threadId)
            break;
        ++it;
    }
    return it;
}


template<class _element, class _trait>
typename osaThreadedLogFileStreambuf<_element, _trait>::ChannelType *
osaThreadedLogFileStreambuf<_element, _trait>::AddChannelForThread(const osaThreadId & threadId)
{
    std::stringstream fileName;
    fileName << this->FilePrefix << this->ChannelContainer.size() << ".txt";
    std::ofstream * newFile = new std::ofstream(fileName.str().c_str()); 
    ChannelContainer.push_back(ElementType(threadId, newFile->rdbuf()));
    return newFile->rdbuf();
}




class osaThreadedLogFile: public std::ostream
{
private:
    osaThreadedLogFileStreambuf<char> Streambuf;

public:
    osaThreadedLogFile(const std::string & filePrefix):
        std::ostream(&Streambuf),
        Streambuf(filePrefix)
    {}

    virtual std::basic_streambuf<char> * rdbuf(void) {
        return &Streambuf;
    }
};


#endif 

