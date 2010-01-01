/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: main.cpp 963 2009-11-13 18:55:48Z adeguet1 $

  Author(s):  Anton Deguet
  Created on: 2009-11-16

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnLoggerQWidget.h>

#include <cisstCommon/cmnPath.h>
#include <cisstCommon/cmnGenericObjectProxy.h>

#include <QApplication>


int main(int argc, char *argv[])
{
    // create a couple of dummy objects with LoD to force linker to use these symbols
    cmnPath path;
    cmnInt anInt;
    cmnDouble aDouble;

    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);

    // create a Qt user interface
    QApplication application(argc, argv);

    // create a widget to control the logger
    cmnLoggerQWidget loggerQWidget;

    loggerQWidget.setupUi();

    // run Qt user interface
    application.exec();

    return 0;
}
