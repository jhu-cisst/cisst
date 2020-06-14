/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  (C) Copyright 2008-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _prmGainParameters_h
#define _prmGainParameters_h

/*!
 *\file
 *\brief PID gain parameters (used for LoPoMoCo and MEI)
*/

#include <cisstMultiTask/mtsGenericObject.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>


/*! So far all the memebers are public for sake of convenience
Only use getters unless you know what you are doing.
*/

class CISST_EXPORT prmGainParameters: public mtsGenericObject {
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    typedef mtsGenericObject    BaseType;

    int AxisNumber;
    double PGain;
    double IGain;
    double DGain;
    double AccelerationFF;
    double VelocityFF;
    double ILimit;
    double ErrorLimit;
    double Offset;
    double DACLimit;
    double OScale;
    double FrictionFF;
    prmGainParameters():
        mtsGenericObject(),
        AxisNumber(0),
        PGain(0),
        IGain(0),
        DGain(0),
        AccelerationFF(0),
        VelocityFF(0),
        ILimit(0),
        ErrorLimit(0),
        Offset(0),
        DACLimit(0),
        OScale(0),
        FrictionFF(0) {
    }

    prmGainParameters(const prmGainParameters & that):
        mtsGenericObject(that)
    {
        Assign(that);
    }

    ~prmGainParameters(){}

    void Assign(const prmGainParameters &that) {
        AxisNumber = that.AxisNumber;
        PGain = that.PGain; IGain = that.IGain; DGain = that.DGain;
        AccelerationFF = that.AccelerationFF; VelocityFF = that.VelocityFF;
        ILimit = that.ILimit; Offset = that.Offset;
        DACLimit = that.DACLimit; OScale = that.OScale;
        FrictionFF = that.FrictionFF;
        ErrorLimit = that.ErrorLimit;
    }
    prmGainParameters & operator= (const prmGainParameters &that) { this->Assign(that); return *this; }

    std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }
    virtual void ToStream(std::ostream &out) const {
        out << AxisNumber
        << ", " << PGain
        << ", " << IGain
        << ", " << DGain
        << ", " << AccelerationFF
        << ", " << VelocityFF
        << ", " << ILimit
        << ", " << ErrorLimit
        << ", " << Offset
        << ", " << DACLimit
        << ", " << OScale
        << ", " << FrictionFF;
    }

    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const {


        BaseType::SerializeRaw(outputStream);

        cmnSerializeRaw(outputStream,AxisNumber);
        cmnSerializeRaw(outputStream,PGain);
        cmnSerializeRaw(outputStream,IGain);
        cmnSerializeRaw(outputStream,DGain);
        cmnSerializeRaw(outputStream,AccelerationFF);
        cmnSerializeRaw(outputStream,VelocityFF);
        cmnSerializeRaw(outputStream,ILimit);
        cmnSerializeRaw(outputStream,ErrorLimit);
        cmnSerializeRaw(outputStream,Offset);
        cmnSerializeRaw(outputStream,DACLimit);
        cmnSerializeRaw(outputStream,OScale);
        cmnSerializeRaw(outputStream,FrictionFF);

    }

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream) {

        BaseType::DeSerializeRaw(inputStream);

        cmnDeSerializeRaw(inputStream,AxisNumber);
        cmnDeSerializeRaw(inputStream,PGain);
        cmnDeSerializeRaw(inputStream,IGain);
        cmnDeSerializeRaw(inputStream,DGain);
        cmnDeSerializeRaw(inputStream,AccelerationFF);
        cmnDeSerializeRaw(inputStream,VelocityFF);
        cmnDeSerializeRaw(inputStream,ILimit);
        cmnDeSerializeRaw(inputStream,ErrorLimit);
        cmnDeSerializeRaw(inputStream,Offset);
        cmnDeSerializeRaw(inputStream,DACLimit);
        cmnDeSerializeRaw(inputStream,OScale);
        cmnDeSerializeRaw(inputStream,FrictionFF);

        }

};

CMN_DECLARE_SERVICES_INSTANTIATION(prmGainParameters)
#endif
