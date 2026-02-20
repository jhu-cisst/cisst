/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2026-02-20

  (C) Copyright 2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsCommandLineOptions.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsCollectorFactory.h>
#include <cisstCommon/cmnPath.h>
#include <cstdlib>

mtsCommandLineOptions::mtsCommandLineOptions(void)
{
    this->AddOptionMultipleValues("m", "component-manager",
                                  "JSON files to configure component manager",
                                  cmnCommandLineOptions::OPTIONAL_OPTION, &ManagerConfig);
    this->AddOptionMultipleValues("c", "collection-config",
                                  "json configuration files for data collection using cisstMultiTask state table collector",
                                  cmnCommandLineOptions::OPTIONAL_OPTION, &CollectionConfig);
}

void mtsCommandLineOptions::Apply(void)
{
    mtsManagerLocal * componentManager = mtsManagerLocal::GetInstance();
    if (!ManagerConfig.empty()) {
        if (!componentManager->ConfigureJSON(ManagerConfig)) {
            exit(-1);
        }
    }
    if (!CollectionConfig.empty()) {
        mtsCollectorFactory * collectorFactory = new mtsCollectorFactory("collectors");
        componentManager->AddComponent(collectorFactory);
        for (const auto & config : CollectionConfig) {
            if (!cmnPath::Exists(config)) {
                cmnPath path;
                path.Add(cmnPath::GetWorkingDirectory());
                std::string fullPath = path.Find(config);
                if (fullPath != "") {
                    collectorFactory->Configure(fullPath);
                } else {
                    std::cerr << "mtsCommandLineOptions::Apply: unable to find configuration file \""
                              << config << "\"" << std::endl;
                    exit(-1);
                }
            } else {
                collectorFactory->Configure(config);
            }
        }
        collectorFactory->Connect();
    }
}
