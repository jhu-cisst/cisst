/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsMonitorFilterBasics.h 3034 2011-10-09 01:53:36Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2012-01-07

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

/*!
  \file
  \brief Defines basic types of filters used for fault detection and diagnosis
*/

#ifndef _mtsMonitorFilterBasics_h
#define _mtsMonitorFilterBasics_h

#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsMonitorFilterBase.h>

#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask
*/

//-----------------------------------------------------------------------------
//  Bypass Filter
//
//  Output Y = X
//
class mtsMonitorFilterBypass : public mtsMonitorFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    BaseType::SignalElement::SIGNAL_TYPE SignalType;

public:
    /*! Default constructor is provided only to satisfy the requirement of 
        cmnGenericObject.  DO NOT USE THIS. */
    mtsMonitorFilterBypass();
    mtsMonitorFilterBypass(BaseType::FILTER_TYPE filterType, 
                           const std::string & inputName,
                           BaseType::SignalElement::SIGNAL_TYPE signalType);
    ~mtsMonitorFilterBypass();

    /*! Implements bypass */
    void DoFiltering(bool debug);

    void ToStream(std::ostream & outputStream) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsMonitorFilterBypass);


//-----------------------------------------------------------------------------
//  Trend Velocity Filter (1st-order differentiator)
//
//  Output Y = (x(t+1) - x(t)) / (delta_t)
//
//  delta_t: Period of monitoring component.  Usually sampling rate of 
//           the monitoring component (mtsMonitorComponent).
//  MJ: delta_t can be also calculated as actual time difference between 
//      two samples but this requires samples to contain timestamp information.
//
class mtsMonitorFilterTrendVel : public mtsMonitorFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    BaseType::SignalElement::SIGNAL_TYPE SignalType;
    BaseType::PlaceholderType OldValueScalar;
    BaseType::PlaceholderVectorType OldValueVector;
    double OldTimestamp;
    bool OldValueVectorInitialized;

public:
    /*! Default constructor is provided only to satisfy the requirement of 
        cmnGenericObject.  DO NOT USE THIS. */
    mtsMonitorFilterTrendVel();
    mtsMonitorFilterTrendVel(BaseType::FILTER_TYPE filterType, 
                             const std::string & inputName,
                             BaseType::SignalElement::SIGNAL_TYPE signalType);
    ~mtsMonitorFilterTrendVel();

    /*! Implements 1st-order differentiator */
    void DoFiltering(bool debug);

    void ToStream(std::ostream & outputStream) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsMonitorFilterTrendVel);


//-----------------------------------------------------------------------------
//  Vectorize Filter
//
//  Output Y_n = (X1, X2, ..., Xn)
//
//  Y_n: vector of size n (of type mtsDoubleVec)
//   Xi: i-th scalar input
//
class mtsMonitorFilterVectorize: public mtsMonitorFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    unsigned int InputSize;

public:
    /*! Default constructor is provided only to satisfy the requirement of 
        cmnGenericObject.  DO NOT USE THIS. */
    mtsMonitorFilterVectorize();
    mtsMonitorFilterVectorize(BaseType::FILTER_TYPE filterType, const BaseType::SignalNamesType & inputNames);
    ~mtsMonitorFilterVectorize();

    /*! Convert n scalars into one vector of size n */
    void DoFiltering(bool debug);

    void ToStream(std::ostream & outputStream) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsMonitorFilterVectorize);


//-----------------------------------------------------------------------------
//  Norm Filter
//
//  Output Y = (norm of X)
//
//  Three types of norm are supported: L1 norm, L2 norm, Linfinity norm
//
//  L1 Norm: Weisstein, Eric W. "L1-Norm." From MathWorld--A Wolfram Web Resource. 
//           http://mathworld.wolfram.com/L1-Norm.html
//  L2 Norm: Weisstein, Eric W. "L2-Norm." From MathWorld--A Wolfram Web Resource. 
//           http://mathworld.wolfram.com/L2-Norm.html
//  Linf Norm: Weisstein, Eric W. "Linfty-Norm." From MathWorld--A Wolfram Web Resource. 
//             http://mathworld.wolfram.com/L-Infinity-Norm.html
//
class mtsMonitorFilterNorm: public mtsMonitorFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    typedef enum { L1NORM, L2NORM, LINFNORM } NORM_TYPE;

protected:
    NORM_TYPE NormType;

public:
    /*! Default constructor is provided only to satisfy the requirement of 
        cmnGenericObject.  DO NOT USE THIS. */
    mtsMonitorFilterNorm();
    mtsMonitorFilterNorm(BaseType::FILTER_TYPE filterType, 
                         const std::string & inputName,
                         NORM_TYPE normType);
    ~mtsMonitorFilterNorm();

    /*! Calculate norm of input */
    void DoFiltering(bool debug);

    void ToStream(std::ostream & outputStream) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsMonitorFilterNorm);

#endif // _mtsMonitorFilterBasics_h

