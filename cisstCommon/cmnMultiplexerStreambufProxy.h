/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ofri Sadowsky
  Created on: 2002-05-20

  (C) Copyright 2002-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*! \file
  \brief Description of types for dynamic control of output messages.
*/
#pragma once

#ifndef _cmnMultiplexerStreambufProxy_h
#define _cmnMultiplexerStreambufProxy_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLODMultiplexerStreambuf.h>

#include <list>
#include <algorithm>

/*!
  \brief Types for dynamic control of output messages.

  This includes debugging information,
  error reporting, state logging etc.
  This file declares *class cmnMultiplexerStreambufProxy*. It is a templated class
  derived from the standard library's basic_streambuf. But is stores a Level Of
  Detail (LOD) descriptor that applies to each message that's output through it.
  The LOD descriptor is transferred to a cmnLODMultiplexerStreambuf object,
  that stores a list of LODed output channels, and decides which ones will
  be actually used.
  The proxy is implemented by overriding basic_streambuf output functions
  xsputn(), overflow() and sync().


   Usage:
         Include the module in your application with:
         \#include <cmnMultiplexerStreambufProxy.h>

     Attach a cmnMultiplexerStreambufProxy object with an ostream. The output functions
     << , put(), write() etc will operate directly on the cmnMultiplexerStreambufProxy.

     Example of using a cmnLODOutputMultiplexer, which is an ostream that is
     attached to a proxy.  Assume that the object lodMultiplexerStreambuf is
     a cmnLODMultiplexerStreambuf.

     \code
     // The multiple output channels
     ofstream log("logfile.txt");
     windowoutputstream display;    // hypothesized class

     lodMultiplexerStreambuf.AddChannel(log.rdbuf(), CMN_LOG_ALLOW_DEFAULT);
     lodMultiplexerStreambuf.AddChannel(windowoutputstream.rdbuf(), CMN_LOG_ALLOW_ERRORS);

     cmnLODMultiplexer multiplexerOutput(&lodMultiplexetStreambuf, CMN_LOG_LEVEL_INIT_DEBUG);

     multiplexerStreambuf << "Hello, world" << endl;  // channel the message only to 'log'
     \endcode

     Notes:
     -# It is assumed that none of the output channels modifies the data arguments.
     -# cmnMultiplexerStreambufProxy does not buffer data. Instead, whenever at attempt is
        made to use stream::put() on a stream with a multiplexer streambuf, the
        cmnMultiplexerStreambuf::overflow() is called automatically, and it forwards the
        character to the output channels.


   \sa C++ manual on basic_ostream and basic_streambuf. cmnOutputMultiplexer.h and cmnLODMultiplexerStreambuf.h
 */
template<class _element, class _trait>
class cmnMultiplexerStreambufProxy : public std::basic_streambuf<_element, _trait>
{
private:
    /*! Declare the copy constructor private to prevent any use.  No
        implementation provided. */
    cmnMultiplexerStreambufProxy(const cmnMultiplexerStreambufProxy<_element, _trait> & other);

 public:

    typedef cmnLODMultiplexerStreambuf<_element, _trait> ChannelType;
    typedef typename std::basic_streambuf<_element, _trait>::int_type int_type;

    /*! Constructor: initialize the true output multiplexer and the current LOD. */
    cmnMultiplexerStreambufProxy(ChannelType *output, cmnLogLevel level)
        : OutputChannel(output), LogLevel(level)
        {}

    /*! Returns the Level of Detail. */
    cmnLogLevel GetLOD(void) const {
        return LogLevel;
    }

    /*! Sets the Level of Detail. */
    void SetLOD(cmnLogLevel level) {
        LogLevel = level;
    }

    /*! Returns a pointer to the output multiplexer. */
    ChannelType * GetOutput(void) const {
        return OutputChannel;
    }

    // Here we override the basic_streambuf methods for multiplexing.
    // This part is declared 'protected' following the declarations of
    // basic_streambuf. See basic_streambuf documentation for more
    // details.
 protected:

    /*! Override the basic_streambuf xsputn to do the multiplexing. */
    virtual std::streamsize xsputn(const _element * s, std::streamsize n);

    /*! Override the basic_streambuf sync for multiplexing. */
    virtual int sync(void);

    /*! Override the basic_streambuf overflow for
      multiplexing. overflow() is called when sputc() discovers it does
      not have space in the storage buffer. In our case, it's
      always. See more on it in the basic_streambuf documentation. */
    virtual int_type overflow(int_type c = _trait::eof());


 private:
    ChannelType * OutputChannel;
    cmnLogLevel LogLevel;

};


//********************************
// Template method implementation
//********************************


/*! Override the basic_streambuf xsputn to do the multiplexing. */
template <class _element, class _trait>
std::streamsize cmnMultiplexerStreambufProxy<_element, _trait>::xsputn(const _element *s, std::streamsize n)
{
    return OutputChannel->xsputn(s, n, LogLevel);
}


/*! Override the basic_streambuf sync for multiplexing. */
template <class _element, class _trait>
int cmnMultiplexerStreambufProxy<_element, _trait>::sync()
{
    return OutputChannel->sync();
}


/*! Override the basic_streambuf overflow for multiplexing. overflow() is called when
 * sputc() discovers it does not have space in the storage buffer. In our case,
 * it's always. See more on it in the basic_streambuf documentation.
 */
template<class _element, class _trait>
typename cmnMultiplexerStreambufProxy<_element, _trait>::int_type
cmnMultiplexerStreambufProxy<_element, _trait>::overflow(int_type c)
{
    return OutputChannel->overflow(c, LogLevel);
}


#endif  // _cmnMultiplexerStreambufProxy_h
