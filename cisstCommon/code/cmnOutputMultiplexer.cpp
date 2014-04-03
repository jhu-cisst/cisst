/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ofri Sadowsky
  Created on: 2002-04-18

  (C) Copyright 2002-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <algorithm>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnOutputMultiplexer.h>


// Add an output channel. See notes above.
// Parameters:
//   channel    - a pointer to the output channel to be added.
//
// Return value: *this
std::ostream & cmnOutputMultiplexer::AddChannel(ChannelType * channel)
{
	ChannelContainerType::iterator it = std::find(m_ChannelContainer.begin(), m_ChannelContainer.end(),
                                                  channel);

    if (it == m_ChannelContainer.end()) {
        m_ChannelContainer.insert(it, channel);
        m_Streambuf.AddChannel(channel->rdbuf(), CMN_LOG_ALLOW_NONE);
    }

    return (*this);
}


// Remove an output channel.
// Parameters:
//   channel    - a pointer to the output channel to be removed. No change occurs if
//                the pointer is not on the list of channels for this multiplexer
//
// Return value: *this
std::ostream & cmnOutputMultiplexer::RemoveChannel(ChannelType * channel)
{
    m_ChannelContainer.remove(channel);
    m_Streambuf.RemoveChannel(channel->rdbuf());
    return (*this);
}
