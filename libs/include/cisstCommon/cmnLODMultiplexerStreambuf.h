/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Ofri Sadowsky
  Created on: 2002-05-17

  (C) Copyright 2002-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*! \file 
  \brief Type Definitions for dynamic control of output messages.
*/

#ifndef _cmnLODMultiplexerStreambuf_h
#define _cmnLODMultiplexerStreambuf_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLogLoD.h>


#include <list>
#include <algorithm>
#include <fstream>


template<class _element, class _trait = std::char_traits<_element> >
class cmnMultiplexerStreambufProxy;

/*! \brief A Streambuffer class that allows output to multiple
  streambuf objects for Level of Detail information.
  
  Types for dynamic control of output messages. This includes
  debugging information, error reporting, state logging etc.  This
  file declares *class cmnLODMultiplexerStreambuf*. It is a templated
  class derived from the standard library's basic_streambuf, with the
  additional feature that enables channeling the output to multiple
  other streambuf objects, and each output channel is associated with
  a Level Of Detail (LOD) descriptor.  The cmnLODMultiplexerStreambuf
  provides both the standard interface functions of basic_streambuf,
  and a substitute with an extra parameter, which is the output
  LOD. cmnLODMultiplexerStreambuf the extra-parameter methods should
  be called from a streambuf proxy that implements the standard
  basic_streambuf virtual methods, and wraps calls to the multiplexer.
  Note that the cmnLODMultiplexerStreambuf can be used independently
  as an output multiplexer withoud LOD, using the standard
  basic_streambuf interface.
 
 
  Usage:
     Include the module in your application with:
     \#include <cmnLODMultiplexerStreambuf.h>
     
     Add output streambuf channels to the multiplexer using the AddChannel() method.
 
     Remove output channels from the multiplexer using the RemoveChannel() method.
 
     Set the output LOD of a channel using the SetChannelLOD() method.
 
     Create proxy streambuf objects that store their LOD, and pass it to the
     cmnLODMultiplexerStreambuf to do the actual multiplexing.
     The proxy streambufs should be attached to an ostream object, which is then
     used by the client module.
     
     Example of using a cmnLODOutputMultiplexer, which is an ostream that is
     attached to a proxy.  Assume that the object lodMultiplexerStreambuf is
     a cmnLODMultiplexerStreambuf.
 
     \code
        // The multiple output channels
       ofstream log("logfile.txt");
       windowoutputstream display;    // hypothesized class
 
       lodMultiplexerStreambuf.AddChannel(&log.rdbuf(), 5);
       lodMultiplexerStreambuf.AddChannel(&windowoutputstream.rdbuf(), 2);
 
       cmnLODOutputMultiplexer multiplexerOutput(&lodMultiplexetStreambuf, 3);
 
       multiplexerStreambuf << "Hello, world" << endl;  // channel the message only to 'log'
     \endcode

     Notes: 
     -# cmnLODMultiplexerStreambuf does not OWN the output channels. They are created
     and destroyed externally.
     -# The initial implementation uses a list to store the addresses of the output channels.
     There is no guarantee on the order of storage or the order of output channelling.
     -# It is assumed that none of the output channels modifies the data arguments.
     -# It is guaranteed that unique streambuf instances are stored in a single 
     cmnLODMultiplexerStreambuf. The AddChannel() function checks for uniqueness.
     -# cmnLODMultiplexerStreambuf does not buffer data. Instead, whenever at attempt is made
        to use stream::put() on a stream with a multiplexer streambuf, the 
        cmnLODMultiplexerStreambuf::overflow() is called automatically, and it forwards the
        character to the output channels.
  
   \sa C++ manual on basic_ostream and basic_streambuf. cmnOutputMultiplexer.h
 */
template<class _element, class _trait = std::char_traits<_element> >
class cmnLODMultiplexerStreambuf : public std::basic_streambuf<_element, _trait>
{
    friend class cmnMultiplexerStreambufProxy<_element, _trait>;
  
    public:
  
    typedef std::basic_streambuf<_element, _trait> BaseClassType;
    typedef std::basic_streambuf<_element, _trait> ChannelType;
    /*! 
      The type of Level of Detail
     */
    typedef cmnLogLoD LogLoDType;

    typedef std::pair<ChannelType *, LogLoDType> ElementType;
  
    /*! Type of internal data structure storing the channels.
      I chose to use a list for efficiency in output iteration over the
      channels, rather than, for example, a map.
     */
    typedef std::list<ElementType> ChannelContainerType;
	typedef typename ChannelContainerType::iterator IteratorType;
	typedef typename ChannelContainerType::const_iterator ConstIteratorType;
  
    /*! 
      Create Multiplexer with a default output filestream
      \param fileStream Default Filestream
     */
    cmnLODMultiplexerStreambuf(std::ofstream & fileStream) {
        this->AddChannel(fileStream.rdbuf(), CMN_LOG_LOD_VERY_VERBOSE);
    }

    /*!
      Constructor: currently empty.
     */
    cmnLODMultiplexerStreambuf()
    {}


    /*! Add an output channel. See notes above.
      \param  channel A pointer to the output channel to be added.
      \param  lod Level Of Detail for the channel.
      \return true: if the channel was added successfully; false: if the channel was not added,
      e.g. if it is already in the container. Note that in this case, we don't change the LOD value.
     */
    bool AddChannel(ChannelType * channel, LogLoDType lod);


    /*! Add a channel defined by an output stream.  This methods
      relies on the existence of a rdbuf() method and calls
      AddChannel(ChannelType, LoDtype).
      
      \param outstream Output stream providing the rdbuf.
      \param lod Level of Detail for the stream.
    */
    inline bool AddChannel(std::ostream & outstream, LogLoDType lod) {
        return AddChannel(outstream.rdbuf(), lod);
    }
  
  
    /*!
      Remove an output channel.
      
      \param channel A pointer to the output channel to be removed. No
      change occurs if the pointer is not on the list of channels for
      this multiplexer.
     */
    void RemoveChannel(ChannelType *channel);


    /*! Remove an output channel.  This methods relies on the
      existence of a rdbuf() method and calls
      RemoveChannel(ChannelType).
      
      \param outstream Output stream providing the rdbuf.
    */
    inline void RemoveChannel(std::ostream& outstream) {
        RemoveChannel(outstream.rdbuf());
    }
  

    /*!
      Set the output LOD of a channel 
      \param channel A pointer to the output channel to be added.
      \param lod Level Of Detail for the channel
      \return true: if the LOD value was changed successfully; false:  otherwise, e.g. if the 
      channel is not in the container.
    */
    bool SetChannelLOD(ChannelType * channel, LogLoDType lod);

  
    /*!
      Find the output LOD of a channel
      \param channel: a pointer to the output channel to be added.
      \param lod: Level Of Detail for the channel
      \return true: if the channel was found in the container; false: if the channel is not 
      in the container.
    */
    bool GetChannelLOD(const ChannelType * channel, LogLoDType & lod) const;
  
    /*!  Enable access to the channel storage, without addition or
      removal of channels.  Elements of the container can be accessed
      using the standard const_iterator interfaces.  Note that the
      channels themselves are non-const, so individual manipulation of
      each is enabled.
      
      \return ChannelContainerType 
    */
    const ChannelContainerType & GetChannels() const {
        return m_ChannelContainer;
    }
  
    // Here we provide basic_streambuf namesame methods for multiplexing.
    // This part is declared 'protected' following the declarations of 
    // basic_streambuf. See basic_streambuf documentation for more
    // details.
    protected:
    typedef typename std::basic_streambuf<_element, _trait>::int_type int_type;
  
    /*! 
      Multiplexed and LODed version of basic_streambuf xsputn.
     */
    virtual std::streamsize xsputn(const _element * s, std::streamsize n, LogLoDType lod);
  
    /*! Override the basic_streambuf sync for multiplexing. Note that in this
      one we sync() all the channels, regardless of the LOD.
     */
    virtual int sync();
  
    /*! Multiplexed and LODed version of basic_streambuf overflow for multiplexing. 
      overflow() is called when sputc() discovers it does not have space in the 
      storage buffer. In our case, it's always. See more on it in the 
      basic_streambuf documentation.
     */
    virtual int_type overflow(int_type c, LogLoDType lod);
  
    /*! Override the basic_streambuf xsputn to do the multiplexing
     */
    virtual std::streamsize xsputn(const _element *s, std::streamsize n);
  
    /*! Override the basic_streambuf overflow for multiplexing. overflow() is called when
      sputc() discovers it does not have space in the storage buffer. In our case, 
      it's always. See more on it in the basic_streambuf documentation.
     */
    virtual int_type overflow(int_type c = _trait::eof());
  
    private:
    /*! The actual container that stores channel addresses.
     */
    ChannelContainerType m_ChannelContainer;
  
    /*! Find a channel in the container and return the container's iterator
      for the element with that channel.
     */
    IteratorType FindChannel(const ChannelType *channel);
  
    /*! Find a channel in the container and return the container's iterator
      for the element with that channel.
     */
    ConstIteratorType FindChannel(const ChannelType *channel) const;
};



//********************************
// Template method implementation
//********************************


template<class _element, class _trait>
bool cmnLODMultiplexerStreambuf<_element, _trait>::AddChannel(ChannelType * channel, LogLoDType lod)
{
    IteratorType it = FindChannel(channel);

    if (it == m_ChannelContainer.end()) {
        m_ChannelContainer.insert(it, ElementType(channel, lod));
        return true;
    }
    return false;
}


template<class _element, class _trait>
void cmnLODMultiplexerStreambuf<_element, _trait>::RemoveChannel(ChannelType * channel)
{
    IteratorType it = FindChannel(channel);

    if (it != m_ChannelContainer.end()) {
        m_ChannelContainer.erase(it);
    }
}


template<class _element, class _trait>
bool cmnLODMultiplexerStreambuf<_element, _trait>::SetChannelLOD(ChannelType * channel, LogLoDType lod)
{
    IteratorType it = FindChannel(channel);
    if (it != m_ChannelContainer.end()) {
        (*it).second = lod;
        return true;
    }
    return false;
}


template<class _element, class _trait>
bool cmnLODMultiplexerStreambuf<_element, _trait>::GetChannelLOD(const ChannelType * channel, LogLoDType & lod) const
{
    ConstIteratorType it = FindChannel(channel);
    if (it != m_ChannelContainer.end()) {
        lod = (*it).second;
        return true;
    }
    return false;
}


template<class _element, class _trait>
std::streamsize cmnLODMultiplexerStreambuf<_element, _trait>::xsputn(const _element * s, std::streamsize n, LogLoDType lod)
{
    std::streamsize ssize(0);
    IteratorType it;
    for (it = m_ChannelContainer.begin(); it != m_ChannelContainer.end(); it++) {
        if (lod <= (*it).second)
            ssize = ((*it).first)->sputn(s, n);
    }
    return ssize;
}


template<class _element, class _trait>
int cmnLODMultiplexerStreambuf<_element, _trait>::sync()
{
    IteratorType it;
    // synchronize all the channels
    for (it = m_ChannelContainer.begin(); it != m_ChannelContainer.end(); it++) {
        ((*it).first)->pubsync();
    }
    return 0;
}


template<class _element, class _trait>
typename cmnLODMultiplexerStreambuf<_element, _trait>::int_type 
cmnLODMultiplexerStreambuf<_element, _trait>::overflow(int_type c, LogLoDType lod)
{
    // follow the basic_streambuf standard
    if (_trait::eq_int_type(_trait::eof(), c))
        return (_trait::not_eof(c));

    IteratorType it;

    // multiplexing
    for (it = m_ChannelContainer.begin(); it != m_ChannelContainer.end(); it++) {
        if (lod <= (*it).second)
            ((*it).first)->sputc( _trait::to_char_type(c) );
    }

    // follow the basic_streambuf standard
    return _trait::not_eof(c);
}


template<class _element, class _trait>
std::streamsize cmnLODMultiplexerStreambuf<_element, _trait>::xsputn(const _element *s, std::streamsize n)
{
    std::streamsize ssize(0);
    IteratorType it;
    for (it = m_ChannelContainer.begin(); it != m_ChannelContainer.end(); it++) {
        ssize = ((*it).first)->sputn(s, n);
    }
    return ssize;
}


template<class _element, class _trait>
typename cmnLODMultiplexerStreambuf<_element, _trait>::int_type 
cmnLODMultiplexerStreambuf<_element, _trait>::overflow(int_type c)
{
    // follow the basic_streambuf standard
    if (_trait::eq_int_type(_trait::eof(), c))
        return (_trait::not_eof(c));

    IteratorType it;

    // multiplexing
    for (it = m_ChannelContainer.begin(); it != m_ChannelContainer.end(); it++) {
        ((*it).first)->sputc( _trait::to_char_type(c) );
    }

    // follow the basic_streambuf standard
    return _trait::not_eof(c);
}


template<class _element, class _trait>
typename cmnLODMultiplexerStreambuf<_element, _trait>::IteratorType
cmnLODMultiplexerStreambuf<_element, _trait>::FindChannel(const ChannelType *channel)
{
    IteratorType it = m_ChannelContainer.begin();
    while (it != m_ChannelContainer.end()) {
        if ((*it).first == channel)
            break;
        ++it;
    }
    return it;
}


template<class _element, class _trait>
typename cmnLODMultiplexerStreambuf<_element, _trait>::ConstIteratorType 
cmnLODMultiplexerStreambuf<_element, _trait>::FindChannel(const ChannelType *channel) const
{
    ConstIteratorType it = m_ChannelContainer.begin();
    while (it != m_ChannelContainer.end()) {
        if ((*it).first == channel)
            break;
        ++it;
    }
    return it;
}


#endif 

