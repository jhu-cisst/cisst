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

class CISST_EXPORT mtsFaultBase: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    typedef enum {
        // System layer
        FAULT_SYSTEM_PROCESS,
        FAULT_SYSTEM_CONNECTION,
        FAULT_SYSTEM_NETWORK,
        FAULT_SYSTEM_FROM_PROCESS_LAYER,
        // Process layer
        FAULT_PROCESS_COMPONENT,
        FAULT_PROCESS_FROM_COMPONENT_LAYER,
        // Component layer
        FAULT_COMPONENT_STRUCTURAL,
        FAULT_COMPONENT_NONSTRUCTURAL,
        FAULT_COMPONENT_FROM_INTERFACE_LAYER,
        // Interface layer
        FAULT_INTERFACE_ONCONNECT,
        FAULT_INTERFACE_POSTCONNECT,
        // Execution layer
        FAULT_EXECUTION_PERFORMANCE,
        FAULT_EXECUTION_NETWORK,
        FAULT_EXECUTION_INVALID_PAYLOAD,
        // INVALID
        FAULT_INVALID
    } FaultTypes;

    typedef enum { INVALID, SYSTEM, PROCESS, COMPONENT, INTERFACE, EXECUTION } LayerTypes;

    typedef struct _ElementNames {
        std::string Process;
        std::string Component;
        std::string Interface;
        std::string Command;
        std::string Function;
        std::string EventGenerator;
        std::string EventHandler;
    } ElementNamesType;

protected:
    /*! Location of fault and its timestamp (for fault isolation) */
    LayerTypes       Layer;
    ElementNamesType ElementNames;
    double           Timestamp;

    /*! Type of fault and degree (magnitude) of fault (for fault identification and diagnosis) */
    FaultTypes Type;
    double     Magnitude;

public:
    // MJ TODO: Add getter/setter/more constructors

    /*! Constructors and destructor */
    mtsFaultBase(void);
    virtual ~mtsFaultBase();

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsFaultBase);

#endif // _mtsFaultBase_h
