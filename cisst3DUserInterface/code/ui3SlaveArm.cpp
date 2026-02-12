/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on:	2008-05-23

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisst3DUserInterface/ui3SlaveArm.h>

#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisst3DUserInterface/ui3Manager.h>


CMN_IMPLEMENT_SERVICES(ui3SlaveArm)


ui3SlaveArm::ui3SlaveArm(const std::string & name):
    Name(name)
{
}

ui3SlaveArm::~ui3SlaveArm()
{
}


bool ui3SlaveArm::SetInput(mtsDevice * positionDevice, const std::string & positionInterface)
{
    return this->SetInput(positionDevice->GetName(), positionInterface);
}


bool ui3SlaveArm::SetInput(const std::string & positionDevice, const std::string & positionInterface)
{
    if (this->Manager == 0) {
        CMN_LOG_CLASS_INIT_ERROR << "SetInput: can not setup input for master arm \""
                                 << this->Name << "\" before adding it to a ui3Manager"
                                 << std::endl;
        return false;
    }

    // add required interface for master arm to Manager
    mtsInterfaceRequired * requiredInterface;

    // setup master arm required interface 
    requiredInterface = this->Manager->AddInterfaceRequired(this->Name);
    if (requiredInterface) {
        // bound the mtsFunction to the command provided by the interface 
        requiredInterface->AddFunction("measured_cp", this->GetCartesianPositionFunction, MTS_REQUIRED);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "SetInput: failed to add \""
                                 << this->Name
                                 << "\" interface, are you trying to set this arm twice?"
                                 << std::endl;
        return false;
    }
    // connect the master device to the master required interface
    this->Manager->ComponentManager->Connect(this->Manager->GetName(), this->Name,
                                             positionDevice, positionInterface);
    return true;
}


bool ui3SlaveArm::SetTransformation(const vctFrm3 & transformation,
                                     double scale)
{
    // keep the transformation and scale
    this->Transformation.Assign(transformation);
    this->Scale = scale;
    return true;
}


void ui3SlaveArm::GetCartesianPosition(prmPositionCartesianGet & position)
{
    vctFrm3 temporary;
    this->GetCartesianPositionFunction(position);
    this->Transformation.ApplyTo(position.Position(), temporary);
    position.Position().Assign(temporary);
    position.Position().Translation().Multiply(this->Scale);
}
