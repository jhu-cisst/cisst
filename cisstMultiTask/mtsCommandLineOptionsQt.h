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

#ifndef _mtsCommandLineOptionsQt_h
#define _mtsCommandLineOptionsQt_h

#include <cisstConfig.h>
#include <cisstMultiTask/mtsCommandLineOptions.h>
#include <cisstMultiTask/mtsExportQt.h>

class CISST_EXPORT mtsCommandLineOptionsQt: public mtsCommandLineOptions {
public:
    mtsCommandLineOptionsQt(const std::string & options = "mcDSMW");
    ~mtsCommandLineOptionsQt() override {}
    void Apply(void) override;

    std::string QtStyle;
#if CISST_HAS_QTNODES
    bool ComponentViewer;
#endif
    bool LoggerWidget;
};

#endif // _mtsCommandLineOptionsQt_h
