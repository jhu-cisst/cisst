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
  \brief Types for dynamic control of output messages.
*/
#pragma once

#ifndef _cmnLODOutputMultiplexer_h
#define _cmnLODOutputMultiplexer_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnMultiplexerStreambufProxy.h>

#include <list>
#include <iostream>

/*! \brief  Types for dynamic control of output messages.

   This includes debugging information, error reporting, state logging etc.
   This file declares class cmnLODOutputMultiplexer. A cmnLODOutputMultiplexer
   inherits the public interface of a generic ostream, and attaches each output
   message with a Level Of Detail (LOD) descriptor.  The LOD descriptor is
   forwarded through a cmnMultiplexerStreambufProxy to a cmnLODMultiplexerStreambuf
   that performs the actual multiplexing.

   Create a collection of ostream objects, and attach them to a
   cmnLODMultiplexerStreambuf.
   Create a cmnLODOutputMultiplexer and assign it with the LOD value and
   the cmnLODMultiplexerStreambuf address.
   Use the standard ostream syntax (operator <<) to stream objects to the multiplexer.

   Example (assume that the object lodMultiplexerStreambuf is
   a cmnLODMultiplexerStreambuf):

     \code
       // The multiple output channels
       ofstream log("logfile.txt");
       windowoutputstream display;  // hypothetical class

       cmnLODOutputMultiplexer multiplexerOutput(&lodMultiplexetStreambuf, CMN_LOG_ALLOW_DEFAULT);

       lodMultiplexerStreambuf.AddChannel(log, CMN_LOG_ALLOW_DEFAULT);
       lodMultiplexerStreambuf.AddChannel(windowoutputstream, CMN_LOG_ALLOW_ALL);

       multiplexerStreambuf << "Hello, world" << endl;  // channel the message only to 'log'
     \endcode

   \sa cmnLODMultiplexerStreambuf.h, cmnMultiplexerStreambufProxy.h,
   C++ documentation of basic_streambuf and iostream
 */
class cmnLODOutputMultiplexer: public std::ostream
{
private:
    /*! Declare the copy constructor private to prevent any use.  No
      implementation provided. */
    cmnLODOutputMultiplexer(const cmnLODOutputMultiplexer & other);

 public:
    typedef char char_type;

    typedef cmnLODMultiplexerStreambuf<char_type> SinkType;

    typedef std::ostream BaseType;

    /*!
      Constructor that initializes base class and stores LOD.
    */
    cmnLODOutputMultiplexer(SinkType * multiplexer, cmnLogLevel level)
        : BaseType(NULL), StreambufProxy(multiplexer, level) {
        if (multiplexer != NULL) {
            init(&StreambufProxy);
        }
    }

    /*! This method simply returns a reference to this object.  It is
      used when a cmnLODOutputMultiplexer is created and immediately
      operated on, to ensure that the object operated on is an l-value
      rather than r-value.  This solves compiler ambiguities and
      errors that lead to incorrect printout.  Use it where the old
      CMN_LOG_HACK would be used.  Normally, one doesn't need to call
      this method in other cases.
    */
    cmnLODOutputMultiplexer & Ref(void)
    {
        return *this;
    }

    /*!
      Returns the Level of Detail.
     */
    cmnLogLevel GetLOD(void) const {
        return StreambufProxy.GetLOD();
    }

    /*!
      Sets the Level of Detail.
     */
    void SetLOD(cmnLogLevel lod) {
        StreambufProxy.SetLOD(lod);
    }


 private:
    cmnMultiplexerStreambufProxy<char_type> StreambufProxy;

};


#endif // _cmnLODOutputMultiplexer_h

