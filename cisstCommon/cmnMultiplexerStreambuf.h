/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ofri Sadowsky
  Created on: 2002-04-19

  (C) Copyright 2002-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*! \file
   \brief Types for dynamic control of output messages
 */
#pragma once

#ifndef _cmnMultiplexerStreambuf_h
#define _cmnMultiplexerStreambuf_h

#include <cisstCommon/cmnPortability.h>

#include <list>
#include <algorithm>

/*!
  \brief A Streambuffer class that allows output to multiple streambuf objects.

  Types for dynamic control of output messages. This includes
  debugging information, error reporting, state logging etc.  This
  file declares *class cmnMultiplexerStreambuf*. It is a templated
  class derived from the standard library's basic_streambuf, with the
  main additional feature that enables channeling the output to
  multiple other streambuf objects.  The multiplexing is implemented
  by overriding basic_streambuf output functions xsputn(), overflow()
  and sync().


  Usage:
  Include the module in your application with:
  \#include "cmnMultiplexerStreambuf.h"

  Add output streambuf channels to the multiplexer using the
  AddChannel() method.

  Remove output channels from the multiplexer using the
  RemoveChannel() method.

  Attach a cmnMultiplexerStreambuf object with an ostream. The output
  functions << , put(), write() etc will operate directly on the
  cmnMultiplexerStreambuf.

  Example of using a cmnOutputMultiplexer, which is an ostream using a
  cmnMultiplexerStrembuf (an output stream with multiplexer
  streambuf):

  \code
  ofstream log("logfile.txt");
  windowoutputstream display;   // hypothesized class

  cmnOutputMultiplexer multiplexer;
  multiplexer.AddChannel(&log);
  multiplexer.AddChannel(&windowoutputstream);

  multiplexer << "Hello, world" << endl;  // channel the message to all streams.
  \endcode

  Notes:
  -# cmnMultiplexerStreambuf does not OWN the output channels. They are created
  and destroyed externally.
  -# The initial implementation uses a list to store the addresses of the output channels.
  There is no guarantee on the order of storage or the order of output channelling.
  -# It is assumed that none of the output channels modifies the data arguments.
  -# It is guaranteed that unique streambuf instances are stored in a single
  cmnMultiplexerStreambuf. The AddChannel() function checks for uniqueness.
  -# cmnMultiplexerStreambuf does not buffer data. Instead, whenever at attempt is made
  to use stream::put() on a stream with a multiplexer streambuf, the
  cmnMultiplexerStreambuf::overflow() is called automatically, and it forwards the
  character to the output channels.

   \sa C++ manual on basic_ostream and basic_streambuf. cmnOutputMultiplexer.h
*/
template<class _element, class _trait = std::char_traits<_element> >
class cmnMultiplexerStreambuf : public std::basic_streambuf<_element, _trait>
{
    public:

    typedef std::basic_streambuf<_element, _trait> ChannelType;
    typedef typename std::basic_streambuf<_element, _trait>::int_type int_type;

    /*!
     * Type of internal data structure storing the channels.
     */
    typedef std::list<ChannelType *> ChannelContainerType;

    /*!
     * Constructor: currently empty.
     */
    cmnMultiplexerStreambuf()
    {}

    /*! Add an output channel. See notes above.
     * \param channel A pointer to the output channel to be added.
     */
    void AddChannel(ChannelType *channel);

    /*! Remove an output channel.
     * \param channel A pointer to the output channel to be removed. No change occurs if
     * the pointer is not on the list of channels for this multiplexer.
     *
     */
    void RemoveChannel(ChannelType * channel);

    /*! Enable access to the channel storage, without addition or removal of channels.
     * Elements of the container can be accessed using the standard const_iterator
     * interfaces.  Note that the channels themselves are non-const, so individual
     * manipulation of each is enabled.
     */
    const ChannelContainerType & GetChannels() const
    {
        return m_ChannelContainer;
    }


    // Here we override the basic_streambuf methods for multiplexing.
    // This part is declared 'protected' following the declarations of
    // basic_streambuf. See basic_streambuf documentation for more
    // details.
    protected:

    /*! Override the basic_streambuf xsputn to do the multiplexing. */
    virtual std::streamsize xsputn(const _element *s, std::streamsize n);

    /*! Override the basic_streambuf sync for multiplexing. */
    virtual int sync();

    /*! Override the basic_streambuf overflow for multiplexing. overflow() is called when
      sputc() discovers it does not have space in the storage buffer. In our case,
      it's always. See more on it in the basic_streambuf documentation.
    */
    virtual int_type overflow(int_type c = _trait::eof());

    private:
    /*! The actual container that stores channel addresses.*/
    ChannelContainerType m_ChannelContainer;

};


//********************************
// Template method implementation
//********************************


template<class _element, class _trait>
void cmnMultiplexerStreambuf<_element, _trait>::AddChannel(ChannelType * channel)
{
    typename ChannelContainerType::iterator it = find(m_ChannelContainer.begin(), m_ChannelContainer.end(),
                                                      channel);

    if (it == m_ChannelContainer.end()) {
        m_ChannelContainer.insert(it, channel);
    }
}


template<class _element, class _trait>
void cmnMultiplexerStreambuf<_element, _trait>::RemoveChannel(ChannelType * channel)
{
    m_ChannelContainer.remove(channel);
}


template<class _element, class _trait>
std::streamsize cmnMultiplexerStreambuf<_element, _trait>::xsputn(const _element *s, std::streamsize n)
{
    std::streamsize ssize;
    typename ChannelContainerType::iterator it;
    for (it = m_ChannelContainer.begin(); it != m_ChannelContainer.end(); it++) {
        ssize = (*it)->sputn(s, n);
    }
    return ssize;
}


template<class _element, class _trait>
int cmnMultiplexerStreambuf<_element, _trait>::sync()
{
    typename ChannelContainerType::iterator it;
    // synchronize all the channels
    for (it = m_ChannelContainer.begin(); it != m_ChannelContainer.end(); it++) {
        (*it)->pubsync();
    }
    return 0;
}


template<class _element, class _trait>
typename cmnMultiplexerStreambuf<_element, _trait>::int_type
cmnMultiplexerStreambuf<_element, _trait>::overflow(int_type c)
{
    // follow the basic_streambuf standard
    if (_trait::eq_int_type(_trait::eof(), c))
        return (_trait::not_eof(c));

    typename ChannelContainerType::iterator it;

    // multiplexing
    for (it = m_ChannelContainer.begin(); it != m_ChannelContainer.end(); it++) {
        (*it)->sputc(_trait::to_char_type(c));
    }

    // follow the basic_streambuf standard
    return _trait::not_eof(c);
}


#endif // _cmnMultiplexerStreambuf_h

