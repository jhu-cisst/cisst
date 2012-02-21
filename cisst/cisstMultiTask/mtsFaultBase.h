/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsFaultBase.h 3034 2011-10-09 01:53:36Z adeguet1 $

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
  \brief Declaration of the base class for various types of faults
 */


/*!
  \ingroup cisstMultiTask

  This class implements the base class for various types of faults defined by the 
  layered approach that identifies systematic faults of component-based software 
  systems [1].  According to this model, the cisst framework is decomposed into the 
  following five layers:

    - System    : Top-level view of the whole system.  Consists of processes.
    - Process   : Process-level view, defined as process boundary.  Consists of components.
    - Component : Component-level view.  Focuses on each component's context integrity.
    - Interface : Interface-level view.  Defines connections between components (and thus
                  component composition as well).
    - Execution : Bottom-level of the system.  Defines and executes actual functions,
                  dispatches and handles events, and exchanges data.

  References:

  -# M. Y. Jung, "A Layered Approach for Identifying Systematic Faults of Component-based 
  Software Systems," In 16th International Workshop on Component-Oriented Programming, 
  Boulder, Colorado, USA, 2011.
*/

#ifndef _mtsFaultBase_h
#define _mtsFaultBase_h

#include <cisstMultiTask/mtsGenericObject.h>

#include <cisstMultiTask/mtsExport.h>

//
// mtsFaultLocation
//
class CISST_EXPORT mtsFaultLocation: public mtsGenericObject 
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    std::string Process;
    std::string Component;
    std::string Interface;
    std::string Command;
    std::string Function;
    std::string EventGenerator;
    std::string EventHandler;

    mtsFaultLocation() : mtsGenericObject() {} 
    ~mtsFaultLocation() {}

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsFaultLocation);


//
// mtsFaultBase
//
class CISST_EXPORT mtsFaultBase: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    typedef enum { 
        LAYER_SYSTEM, 
        LAYER_PROCESS, 
        LAYER_COMPONENT, 
        LAYER_INTERFACE, 
        LAYER_EXECUTION,
        LAYER_INVALID
    } LayerTypes;

    typedef enum {
        // INVALID
        FAULT_INVALID                        = 0x00000,
        // System layer
        FAULT_SYSTEM_PROCESS                 = 0x00001,
        FAULT_SYSTEM_CONNECTION              = 0x00002,
        FAULT_SYSTEM_NETWORK                 = 0x00004,
        FAULT_SYSTEM_FROM_PROCESS_LAYER      = 0x00008,
        FAULT_SYSTEM_MASK                    = 0x0000F,
        // Process layer
        FAULT_PROCESS_COMPONENT              = 0x00010,
        FAULT_PROCESS_FROM_COMPONENT_LAYER   = 0x00020,
        FAULT_PROCESS_MASK                   = 0x000F0,
        // Component layer
        FAULT_COMPONENT_FUNCTIONAL           = 0x00100,
        FAULT_COMPONENT_NONFUNCTIONAL        = 0x00200,
        FAULT_COMPONENT_FROM_INTERFACE_LAYER = 0x00400,
        FAULT_COMPONENT_MASK                 = 0x00F00,
        // Interface layer
        FAULT_INTERFACE_ONCONNECT            = 0x01000,
        FAULT_INTERFACE_POSTCONNECT          = 0x02000,
        FAULT_INTERFACE_FROM_EXECUTION_LAYER = 0x04000,
        FAULT_INTERFACE_MASK                 = 0x0F000,
        // Execution layer
        FAULT_EXECUTION_PERFORMANCE          = 0x10000,
        FAULT_EXECUTION_NETWORK              = 0x20000,
        FAULT_EXECUTION_INVALID_PAYLOAD      = 0x40000,
        FAULT_EXECUTION_MASK                 = 0xF0000
    } FaultTypes;

protected:
    typedef mtsFaultBase BaseType;

    /*! Basic information about fault */
    std::string      FaultName;
    LayerTypes       FaultLayer;

    /*! Location and timestamp of fault (for fault isolation) */
    mtsFaultLocation FaultLocation;
    double           FaultTimestamp;

    /*! Type of fault and degree (magnitude) of fault (for fault identification and diagnosis) */
    FaultTypes       FaultType;
    double           FaultMagnitude;

public:
    /*! Constructors and destructor */
    mtsFaultBase();
    mtsFaultBase(const std::string & faultName, FaultTypes faultType);
    virtual ~mtsFaultBase();

    /*! Fault isolation */
    mtsFaultLocation & GetFaultLocation(void);
    void               SetFaultTimestamp(double timestamp);
    double             GetFaultTimestamp(void);

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsFaultBase);

#endif // _mtsFaultBase_h
