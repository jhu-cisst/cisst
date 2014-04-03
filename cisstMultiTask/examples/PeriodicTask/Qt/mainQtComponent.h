/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet, Ali Uneri, Peter Kazanzides
  Created on: 2009-10-22

  (C) Copyright 2009-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mainQtComponent_h
#define _mainQtComponent_h

#include <cisstMultiTask/mtsComponent.h>

#ifdef mtsExPeriodicTaskGuiQt_EXPORTS
#define CISST_THIS_LIBRARY_AS_DLL
#endif
#include <cisstCommon/cmnExportMacros.h>
#undef CISST_THIS_LIBRARY_AS_DLL

// Currently hard-coded, but could be added as constructor parameter
const unsigned int NumSineTasks = 2;

class QWidget;
class mtsCollectorQtWidget;
class displayQtComponent;

class CISST_EXPORT mainQtComponent: public mtsComponent
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_LOD_RUN_ERROR);

    QWidget * mainWidget;
    mtsCollectorQtWidget * collectorQtWidget;

    displayQtComponent * display[NumSineTasks];

 public:

    mainQtComponent(const std::string & name);
    ~mainQtComponent();

    void Startup(void);

    mtsCollectorQtWidget * GetCollectorQtWidget(void) const
    { return collectorQtWidget; }

};

CMN_DECLARE_SERVICES_INSTANTIATION(mainQtComponent);

#endif  // _mainQtComponent_h
