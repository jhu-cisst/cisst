/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
Author(s):	Marcin Balicki
Created on:   2008-09-14

(C) Copyright 2008 Johns Hopkins University (JHU), All Rights
Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*! 
\file 
\brief Actuator State query parameters.
*/


#ifndef _prmActuatorState_h
#define _prmActuatorState_h

// basic includes
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsVector.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>

/*! A container for Controlloer's Actuator states
   Position, Velocity, etc. for each actuator
*/
class CISST_EXPORT prmActuatorState: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    typedef prmActuatorState ThisType;
    typedef mtsGenericObject BaseType;

    typedef unsigned int size_type;

    /*! default constructor - does nothing for now */
    inline prmActuatorState() {}

    prmActuatorState(size_type size);

    void SetSize(size_type size);

    ///*! constructor with all possible parameters */
    //inline prmActuatorState(const prmPosition & position,
    //                           const mtsStateIndex & stateIndex):
    //    PositionMember(position),
    //    StateIndexMember(stateIndex)
    //{}

    /*! destructor */
    virtual ~prmActuatorState() {};

    /*! Set and Get methods for the the position. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(mtsDoubleVec, Position);
    //@}

    /*! Set and Get methods for the the Velocity. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(mtsDoubleVec, Velocity);
    //@}


    /*! Set and Get methods for the the position. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctBoolVec, InMotion);
    //@}

    /*! Set and Get methods for the the position. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctBoolVec, MotorOff);
    //@}


    /*! Set and Get methods for the forward SOFTWARE limits hit. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctBoolVec, SoftFwdLimitHit);
    //@}


    /*! Set and Get methods for the reverse SOFTWARE Limit. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctBoolVec, SoftRevLimitHit);
    //@}

        /*! Set and Get methods for the forward HARDWARE limits hit. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctBoolVec, HardFwdLimitHit);
    //@}

    /*! Set and Get methods for the reverse HARDWARE Limit. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctBoolVec, HardRevLimitHit);
    //@}


    /*! Set and Get methods for the  HomeSwitch . */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctBoolVec, HomeSwitchOn);
    //@}

    /*! Set and Get methods for state of homing variable on the controller. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctBoolVec, IsHomed);
    //@}

    /*! Set and Get methods for the state of the estop button. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(bool, EStopON);
    //@}

public:

    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;


    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const;

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream);

}; 


CMN_DECLARE_SERVICES_INSTANTIATION(prmActuatorState);

#endif // _prmActuatorState_h
