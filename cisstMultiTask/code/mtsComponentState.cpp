/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2013-10-31

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsComponentState.h>

const mtsComponentState & mtsComponentState::operator = (const Enum & value)
{
    this->SetState(value);
    return *this;
}

bool mtsComponentState::operator == (const mtsComponentState & state) const {
    return (this->State() == state.State());
}

bool mtsComponentState::operator != (const mtsComponentState & state) const {
    return !(*this == state);
}

bool mtsComponentState::operator >= (const mtsComponentState & state) const {
    return (this->State() >= state.State());
}

bool mtsComponentState::operator > (const mtsComponentState & state) const {
    return (this->State() > state.State());
}

bool mtsComponentState::operator <= (const mtsComponentState & state) const {
    return (this->State() <= state.State());
}

bool mtsComponentState::operator < (const mtsComponentState & state) const {
    return (this->State() < state.State());
}
