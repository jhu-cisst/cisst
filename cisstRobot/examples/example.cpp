/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Zihan Chen
  Created on: 2013-08-14

  (C) Copyright 2013-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <iostream>
#include <fstream>
#include <cisstCommon/cmnPath.h>
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstRobot/robManipulator.h>

#if CISST_HAS_JSON
#include <json/json.h>
#endif


int main(int CMN_UNUSED(argc), char ** CMN_UNUSED(argv))
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cerr, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);

    cmnPath path;
    std::cout << path.GetWorkingDirectory() << std::endl;

#if CISST_HAS_JSON

    // open *.rob.json config file
    std::string fileName("irb6600.rob.json");
    std::ifstream robotConfigFile(fileName.c_str());

    // load config file with JSON parser
    Json::Reader jreader;
    Json::Value  robotConfig;
    bool rc = jreader.parse(robotConfigFile, robotConfig);
    if (!rc) {
        std::cerr << jreader.getFormattedErrorMessages() << std::endl;
        return -1;
    }

    // robManipulator with .rob file
    robManipulator robotRob;
    robotRob.LoadRobot(robotConfig);

    // robManipulator with .rob.json file
    robManipulator robotJson;
    robotJson.LoadRobot(robotConfig);

    // joint position all zero
    vctDoubleVec q(robotRob.links.size(), 0.0);

    std::cout << "JSON: " << robotJson.ForwardKinematics(q) << std::endl
              << "Rob:  " << robotRob.ForwardKinematics(q) << std::endl;

#endif
}
