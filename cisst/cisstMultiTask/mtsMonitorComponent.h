/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsMonitorComponent.h 3034 2011-10-09 01:53:36Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2011-12-29

  (C) Copyright 2004-2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

/*!
  \file
  \brief Defines fault monitor
*/

#ifndef _mtsMonitorComponent_h
#define _mtsMonitorComponent_h

#include <cisstCommon/cmnNamedMap.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>

#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class implements an internal component that monitors other component(s) 
  in the same process.

    Requirements:
        - Can be deployed at run-time using dynamic component composition services
        - Can monitor more than one components simultaneously
        - Can spwan internal processing thread(s) if monitoring overhead increases (self-monitoring)
        - Can support flexible deployment of different kinds of "filters"
          1) Basic filters
             . Bypass
             . Sampling
             . Average
             . Median
             . Std
             . Min, Max
             . Trend velocity (dx/dt)
             . Trend acceleration (d/dt)(dx/dt)
             . Thresholding
          2) Advanced filters
             . LPF, HPF
             . FFT
             . Wavelet
             . Statistical distribution
          3) User-supplied filters
        - Can support cascaded filters, i.e., filters can be combined

    Inputs:
        - Raw data collected from target component(s)

    Outputs:
        - Feature vectors
*/

class CISST_EXPORT mtsMonitorComponent : public mtsTaskPeriodic
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    class TargetComponent {
    public:
        // Name of target component
        std::string Name;
        // Required interface to connect to the target component
        mtsInterfaceRequired * InterfaceRequired;
        // Function to read state variables from target component
        mtsFunctionRead GetPeriod;
        // Placeholders to register state variable to the monitoring state table of this
        // component (i.e., working copy of Period of the target component)
        double Period;
    };

    /*! List of TargetComponent structure */
    typedef cmnNamedMap<TargetComponent> TargetComponentsType;
    TargetComponentsType * TargetComponents;

    /*! Fetch new values from each target component */
    void UpdateFilters(void);

    /*! Print list of target components */
    void PrintTargetComponents(void);

public:
    mtsMonitorComponent();
    ~mtsMonitorComponent();

    /*! Add and remove target component to monitor */
    bool AddTargetComponent(mtsTaskPeriodic * task); // MJ: task name can be used instead
    bool RemoveTargetComponent(const std::string & taskName);

    void Configure(const std::string & CMN_UNUSED(filename) = ""){}
    void Startup(void) {};
    void Run(void);
    void Cleanup(void) {};

    //  Getters
    /*! Return the name of required interface to access a given task's state table */
    const std::string GetNameOfStateTableAccessInterface(const std::string & taskName) const;

    /*! Return the name of this component */
    static const std::string & GetNameOfMonitorComponent(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsMonitorComponent)


#endif // _mtsMonitorComponent_h

