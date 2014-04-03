/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky
  Created on:	2002-04-18

  (C) Copyright 2002-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Declaration of cmnOutputMultiplexer class.
 */
#pragma once

#ifndef _cmnOutputMultiplexer_h
#define _cmnOutputMultiplexer_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLODMultiplexerStreambuf.h>

#include <list>
#include <iostream>

#include <cisstCommon/cmnExport.h>


/*!
  \brief Creates a collection of objects that channels output to multiple sinks.

  Types for dynamic control of output messages. This includes
  debugging information, error reporting, state logging etc.  This
  file declares *class cmnOutputMultiplexer*. a cmnOutputMultiplexer
  inherits the public interface of a generic ostream, and channels the
  output to multiple sinks.


  Usage:
 	  Include the module in your application with:
 	  \#include "cmnOutputMultiplexer.h"

     Create a collection of ostream objects, and attach them to a cmnOutputMultiplexer.
     Use the standrard ostream syntax (operator <<) to stream objects to the multiplexer.

     \code
       ofstream log("logfile.txt");
       windowoutputstream display;  // hypothetical class

       cmnOutputMultiplexer multiplexer;
       multiplexer.AddChannel(&log);
       multiplexer.AddChannel(&windowoutputstream);

       multiplexer << "Hello, world" << endl;  // channel the message ot all streams.
     \endcode

     Notes:
     -# cmnOutputMultiplexer does not OWN the output streams. They are created
     and destroyed externally.
     -# The initial implementation uses a list to store the addresses of the output channels.
     There is no guarantee on the order of storage or the order of output channelling.
     -# It is assumed that none of the output channels modifies the data arguments.
     -# The multiplexer does not buffer any output. There is no implementation for seekp()
        and tellp().
     -# It is guaranteed that at most one instance of an ostream object is stored as a channel
        in a single multiplexer. The AddChannel() function checks for uniqueness.
*/
class CISST_EXPORT cmnOutputMultiplexer : public std::ostream
{
 public:
    typedef char char_type;
    typedef std::ostream ChannelType;

    /*! Type of internal data structure storing the channels. */
    typedef std::list<ChannelType *> ChannelContainerType;

    /*! Default constructor - initialize base class. */
    cmnOutputMultiplexer() : ChannelType(&m_Streambuf) {}

    /*! Add an output channel. See notes above.
      \param channel A pointer to the output channel to be added.
     */
    ChannelType & AddChannel(ChannelType * channel);

    /*! Remove an output channel.
      \param channel A pointer to the output channel to be removed. No change occurs if
      the pointer is not on the list of channels for this multiplexer.
      \return channel Returns *this, The output channel.
     */
    ChannelType & RemoveChannel(ChannelType * channel);


    /*! Enable access to the channel storage, without addition or removal of channels.
      Elements of the container can be accessed using the standard const_iterator
      interfaces.  Note that the channels themselves are non-const, so individual
      manipulation of each is enabled.
     */
    const ChannelContainerType & GetChannels() const {
        return m_ChannelContainer;
    }

 protected:
    ChannelContainerType m_ChannelContainer;

    cmnLODMultiplexerStreambuf<char_type> m_Streambuf;
};


#endif // _cmnOutputMultiplexer_h

