/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ManagerComponentLocal.h 

  Author(s):  Min Yang Jung
  Created on: 2010-09-01

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnConstants.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSleep.h>

#include "ManagerComponentLocal.h"

const std::string CounterOddComponentType  = "CounterOddComponent";
const std::string CounterOddComponentName  = "CounterOddComponentObject";
const std::string CounterEvenComponentType = "CounterEvenComponent";
const std::string CounterEvenComponentName = "CounterEvenComponentObject";

const std::string NameCounterOddInterfaceProvided = "CounterOddInterfaceProvided";
const std::string NameCounterOddInterfaceRequired = "CounterOddInterfaceRequired";
const std::string NameCounterEvenInterfaceProvided = "CounterEvenInterfaceProvided";
const std::string NameCounterEvenInterfaceRequired = "CounterEvenInterfaceRequired";

CMN_IMPLEMENT_SERVICES(ManagerComponentLocal);

ManagerComponentLocal::ManagerComponentLocal(const std::string & componentName, double period):
    mtsTaskPeriodic(componentName, period, false, 1000)
{
    UseSeparateLogFileDefault();

    // Enable manager component services
    mtsInterfaceRequired * required = EnableDynamicComponentManagement();
    if (!required) {
        cmnThrow(std::runtime_error("ManagerComponentLocal constructor: failed to enable dynamic component composition"));
    }
}

void ManagerComponentLocal::Run(void) 
{
    ProcessQueuedCommands();

    static double lastTick = 0;
    static int count = 0;

#if 0
    if (++count == 2) {
        std::vector<std::string> processes, components, interfacesRequired, interfacesProvided;
        if (osaGetTime() - lastTick > 5.0) {
            std::cout << "==================================== Processes" << std::endl;
            if (ManagerComponentServices->RequestGetNamesOfProcesses(processes)) {
                for (size_t i = 0; i < processes.size(); ++i) {
                    std::cout << processes[i] << std::endl;
                }
            }

            std::cout << std::endl << "==================================== Components" << std::endl;
            for (size_t i = 0; i < processes.size(); ++i) {
                if (ManagerComponentServices->RequestGetNamesOfComponents(processes[i], components)) {
                    for (size_t j = 0; j < components.size(); ++j) {
                        std::cout << processes[i] << " - " << components[j] << std::endl;
                    }
                }
            }

            std::cout << std::endl << "==================================== Interfaces" << std::endl;
            for (size_t i = 0; i < processes.size(); ++i) {
                for (size_t j = 0; j < components.size(); ++j) {
                    std::cout << processes[i] << "." << components[j] << std::endl;
                    std::cout << "\tRequired: " << std::endl;
                    if (ManagerComponentServices->RequestGetNamesOfInterfaces(processes[i], components[j], interfacesRequired, interfacesProvided)) {
                        for (size_t k = 0; k < interfacesRequired.size(); ++k) {
                            std::cout << "\t\t" << interfacesRequired[k] << std::endl;
                        }
                        std::cout << std::endl;
                        std::cout << "\tProvided: " << std::endl;
                        for (size_t k = 0; k < interfacesProvided.size(); ++k) {
                            std::cout << "\t\t" << interfacesProvided[k] << std::endl;
                        }
                        std::cout << std::endl;
                    }
                }
            }

            std::cout << std::endl << "==================================== Connections" << std::endl;
			std::vector<mtsDescriptionConnection> connections;
            if (ManagerComponentServices->RequestGetListOfConnections(connections)) {
                for (size_t i = 0; i < connections.size(); ++i) {
                    std::cout << connections[i] << std::endl;
                }
            }

            std::cout << std::endl << std::endl;
            std::flush(std::cout);

            lastTick = osaGetTime();
        }
    }
#endif

#if 1
    if (++count == 5) {
        //
        // Create the two components: odd counter and even counter
        //
        std::cout << std::endl << "Creating counter components....." << std::endl;

        std::cout << "> " << CounterOddComponentType << ", " << CounterOddComponentName << ": ";
        if (!ManagerComponentServices->RequestComponentCreate(CounterOddComponentType, CounterOddComponentName)) {
            std::cout << "failure" << std::endl;
        } else {
            std::cout << "success" << std::endl;
        }

        std::cout << "> " << CounterEvenComponentType << ", " << CounterEvenComponentName << ": ";
        if (!ManagerComponentServices->RequestComponentCreate(CounterEvenComponentType, CounterEvenComponentName)) {
            std::cout << "failure" << std::endl;
        } else {
            std::cout << "success" << std::endl;
        }

        // MJ: needs to be replaced with blocking command with return value
        std::cout << std::endl << "Wait for 5 seconds for \"Component Connect\"...." << std::endl;
        osaSleep(5.0);

        //
        // Connect the two components
        //
        std::cout << std::endl << "Connecting counter components....." << std::endl;
        std::cout << "> Connection 1: ";
        if (!ManagerComponentServices->RequestComponentConnect(CounterOddComponentName, NameCounterOddInterfaceRequired, 
            CounterEvenComponentName, NameCounterEvenInterfaceProvided))
        {
            std::cout << "failure" << std::endl;
        } else {
            std::cout << "success" << std::endl;
        }

        std::cout << "> Connection 2: ";
        if (!ManagerComponentServices->RequestComponentConnect(CounterEvenComponentName, NameCounterEvenInterfaceRequired,
            CounterOddComponentName, NameCounterOddInterfaceProvided))
        {
            std::cout << "failure" << std::endl;
        } else {
            std::cout << "success" << std::endl;
        }

        // MJ: needs to be replaced with blocking command with return value
        std::cout << std::endl << "Wait for 5 seconds for \"Component Start\"...." << std::endl;
        osaSleep(5.0);

        //
        // Start the two components
        //
        std::cout << std::endl << "Starting counter components....." << std::endl;
        std::cout << "> " << CounterOddComponentName << ": ";
        if (!ManagerComponentServices->RequestComponentStart(CounterOddComponentName)) {
            std::cout << "failure" << std::endl;
        } else {
            std::cout << "success" << std::endl;
        }

        std::cout << "> " << CounterEvenComponentName << ": ";
        if (!ManagerComponentServices->RequestComponentStart(CounterEvenComponentName)) {
            std::cout << "failure" << std::endl;
        } else {
            std::cout << "success" << std::endl;
        }

        std::cout << std::endl << "Wait for 5 seconds for \"Component Stop\"...." << std::endl;
        osaSleep(5.0);

        //
        // Stop the two components
        //
        std::cout << std::endl << "Stopping counter components....." << std::endl;
        std::cout << "> " << CounterOddComponentName << ": ";
        if (!ManagerComponentServices->RequestComponentStop(CounterOddComponentName)) {
            std::cout << "failure" << std::endl;
        } else {
            std::cout << "success" << std::endl;
        }

        std::cout << "> " << CounterEvenComponentName << ": ";
        if (!ManagerComponentServices->RequestComponentStop(CounterEvenComponentName)) {
            std::cout << "failure" << std::endl;
        } else {
            std::cout << "success" << std::endl;
        }

        std::cout << std::endl << "Wait for 5 seconds for \"Component Resume\"...." << std::endl;
        osaSleep(5.0);

        //
        // Resume the two components
        //
        std::cout << std::endl << "Resuming counter components....." << std::endl;
        std::cout << "> " << CounterOddComponentName << ": ";
        if (!ManagerComponentServices->RequestComponentResume(CounterOddComponentName)) {
            std::cout << "failure" << std::endl;
        } else {
            std::cout << "success" << std::endl;
        }

        std::cout << "> " << CounterEvenComponentName << ": ";
        if (!ManagerComponentServices->RequestComponentResume(CounterEvenComponentName)) {
            std::cout << "failure" << std::endl;
        } else {
            std::cout << "success" << std::endl;
        }
    }
#endif
}
