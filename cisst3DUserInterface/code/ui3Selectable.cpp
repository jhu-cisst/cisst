/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on:	2008-06-10

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisst3DUserInterface/ui3Selectable.h>
#include <cisst3DUserInterface/ui3VisibleList.h>

#include <cisstOSAbstraction/osaSleep.h>

ui3Selectable::ui3Selectable(const std::string & name):
    BaseType(name),
    SelectedMember(false),
    MasterArm(0),
    ActivatedMember(false)
{
}


void ui3Selectable::ResetOverallIntention(void)
{
    this->OverallIntention = 0.0;
}


double ui3Selectable::UpdateOverallIntention(double intention)
{
    if (intention > this->OverallIntention) {
        this->OverallIntention = intention;
    }
    return this->OverallIntention;
}


void ui3Selectable::SetActivated(bool activated)
{
    this->ActivatedMember = activated;
    if (activated) {
        this->Show();
    } else {
        this->Hide();
    }
}
