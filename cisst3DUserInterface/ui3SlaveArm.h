/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2009-04-03

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _ui3SlaveArm_h
#define _ui3SlaveArm_h

#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstParameterTypes/prmForwardDeclarations.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3CursorBase.h>

// Always include last!
#include <cisst3DUserInterface/ui3Export.h>

/*!
  Defines a master arm with cursor and callbacks
*/
class CISST_EXPORT ui3SlaveArm: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class ui3Manager;

public:

    /*!
     Constructor
    */
    ui3SlaveArm(const std::string & name);

    /*!
     Destructor
    */
    virtual ~ui3SlaveArm();

    virtual bool SetInput(mtsDevice * positionDevice, const std::string & positionInterface);

    virtual bool SetInput(const std::string & positionDevice, const std::string & positionInterface);
    
    virtual bool SetTransformation(const vctFrm3 & transformation = vctFrm3::Identity(),
                                   double scale = 1.0);

    virtual void GetCartesianPosition(prmPositionCartesianGet & position);

protected:

    // arm name
    std::string Name;
    
    // transformation between inputs and scene
    vctFrm3 Transformation;
    double Scale;
    
    // function to read from the device
    mtsFunctionRead GetCartesianPositionFunction;

    // ui3Manager used
    ui3Manager * Manager;

    inline bool SetManager(ui3Manager * manager) {
        this->Manager = manager;
        return true;
    }

};


CMN_DECLARE_SERVICES_INSTANTIATION(ui3SlaveArm)


#endif // _ui3SlaveArm_h
