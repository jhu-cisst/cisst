/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsFaultDetectorThresholding.h 3034 2011-10-09 01:53:36Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2012-02-07

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

/*!
  \file
  \brief Defines the base class for fault detection and diagnosis

  \ingroup cisstMultiTask
*/

#ifndef _mtsFaultDetectorThresholding_h
#define _mtsFaultDetectorThresholding_h

#include <cisstMultiTask/mtsFaultDetectorBase.h>

class mtsFaultDetectorThresholding: public mtsFaultDetectorBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    typedef mtsFaultDetectorBase BaseType;

protected:
    double SumOfSamples;
    double SumOfSquaredSamples;
    size_t SampleCount;

    /*! Sampled std */
    double Sigma;
    /*! Expected mean */
    double Mean;
    /*! K-sigma */
    double K;
    /*! Upper control limit (UCL) and lower control limit (LCL) */
    double UCL, LCL;

public:
    /*! Constructors and destructor */
    mtsFaultDetectorThresholding();
    mtsFaultDetectorThresholding(const std::string & signalName, 
                                 double average, 
                                 size_t samplingCount = 500, 
                                 size_t k = 3);
    ~mtsFaultDetectorThresholding();

    /*! See base class */
    void CheckFault(bool debug = false);

    void ToStream(std::ostream & outputStream) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsFaultDetectorThresholding);

#endif // _mtsFaultDetectorThresholding_h_h

