/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

Author(s):  Marcin Balicki
Created on: 2008-08-07

(C) Copyright 2008 Johns Hopkins University (JHU), All Rights
Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _prmActuatorParameters_h
#define _prmActuatorParameters_h

/*!
\file
\brief Holds parameters of an actuator (currently includes current values also)
\note This might not be parameter class (ask anton for his opinion)
*/

#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstMultiTask/mtsStateIndex.h>
#include <cisstMultiTask/mtsMacros.h>
#include <cisstMultiTask/mtsVector.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>


/*! So far all the memebers are public for sake of convenience
Only use getters unless you know what you are doing.

//TODO: add internal size to specify the size of each member in this class
//      so the user does not have to have a
// this is not a great software engineering approach but for now it might be the most usefull
//cisst vector of this class caused compilation errors
//it is possible to resize each memeber so it is better to test each member for size
// this is by contract but does not have to be so...more discussion required

//using mtsVectors here because prm lib does not define Long Vectors
//galil position is in counts...so
*/
class CISST_EXPORT prmActuatorParameters: public mtsGenericObject

{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

private:


public:
    typedef prmActuatorParameters ThisType;
    typedef mtsGenericObject BaseType;
    typedef unsigned int size_type;


    /*!default constructor*/
    prmActuatorParameters() {};
    //initial size
    inline prmActuatorParameters(size_type size) { SetSize(size);};

    void SetSize(size_type size);

    /*!destructor
    */
    virtual ~prmActuatorParameters(){};


    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const;

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream);


    /*! Set and Get methods for actuator settings
    Can use this in order to reset the position
    These should be queried only, but for sake of convenience they have setterse here
    */

    /*! Set and Get methods for postive limit. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(mtsLongVec, ForwardLimit);
    //@}

    /*! Set and Get methods for negative limit. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(mtsLongVec, ReverseLimit);
    //@}

    /*! Set and Get methods for velocity limit, use accelerationMax to accelerate until this limit.
    Can used this in velocity control mode.
    */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(mtsLongVec, MaxVelocity);
    //@}

    /*! Set and Get methods for Acceleration Maximum, this is used as the actual maximum acceleration used for all motion. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(mtsLongVec, MaxAcceleration);
    //@}

    /*! Set and Get methods for Deceleration Maximum, this is used as the actual maximum acceleration used for all motion. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(mtsLongVec, MaxDeceleration);
    //@}

    /*! Set and Get methods for Deceleration Maximum, this is used as the actual maximum acceleration used for all motion. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(mtsLongVec, LimitSwitchDeccelaration);
    //@}

    /*! Set and Get methods for counts to mm conversion  */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, Counts_per_mm);
    //@}

    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(int, AxisSign);
    //@}

    //PID Servo Loop Controll Parameters
    //these are only changed in special cases, so right now there is no accessors for them.


    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, Kp);   //Proportional
    //@}

    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, Kd); //Derivative
    //@}

    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, Ki); //Integral
    //@}

    //@{
    //if IL is positive, Ki works all the time, otherwise IL works only after move is finished
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, IL); //Integrator limit
    //@}


    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, FV); //FeedForward Velocity Parameters
    //@}

    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, FA); //FeedForward Acceleration Parameters
    //@}

    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, TorqueLimit);
    //@}

    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, TorqueLimitPeak);
    //@}

    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, TorqueOffset); //Torque offset parameter, used for break.
    //@}

}; // _prmActuatorParameters_h

CMN_DECLARE_SERVICES_INSTANTIATION(prmActuatorParameters)

#endif
