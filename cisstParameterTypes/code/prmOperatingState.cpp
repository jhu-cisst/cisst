/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:   2019-06-10

  (C) Copyright 2019-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmOperatingState.h>

bool prmOperatingState::ValidCommand(const prmOperatingState::CommandType & command,
                                     prmOperatingState::StateType & newOperatingState,
                                     std::string & humanReadableMessage)
{
    // find supported transitions
    switch (State()) {
    case DISABLED:
        switch (command) {
        case enable:
            newOperatingState = ENABLED;
            return true;
        case disable:
            newOperatingState = DISABLED;
            return true;
        case unhome:
            newOperatingState = State();
            return true;
        default:
            break;
        }
    case ENABLED:
        switch (command) {
        case enable:
            newOperatingState = ENABLED;
            return true;
        case disable:
            newOperatingState = DISABLED;
            return true;
        case pause:
            newOperatingState = PAUSED;
            return true;
        case unhome:
            newOperatingState = State();
            return true;
        case home:
            newOperatingState = State();
            return true;
        default:
            break;
        }
        break;
    case PAUSED:
        switch (command) {
        case disable:
            newOperatingState = DISABLED;
            return true;
        case pause:
            newOperatingState = PAUSED;
            return true;
        case resume:
            newOperatingState = ENABLED;
            return true;
        case unhome:
            newOperatingState = State();
            return true;
        default:
            break;
        }
        break;
    case FAULT:
        switch (command) {
        case disable:
            newOperatingState = DISABLED;
            return true;
        case unhome:
            newOperatingState = State();
            return true;
        default:
            break;
        }
        break;
    case UNDEFINED:
        break;
    }

    // invalid transitions
    newOperatingState = State();
    humanReadableMessage = "command \"" + CommandTypeToString(command)
        + "\" is not supported in state \"" + StateTypeToString(State())
        + (SubState().empty() ? "\"" : (" (" + SubState() + ")\"")) ;
    return false;
}
