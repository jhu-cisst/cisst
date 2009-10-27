/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-26

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _serverTask_h
#define _serverTask_h

#include <cisstMultiTask.h>

#include <QObject>


class serverTask : public QObject, public mtsDevice
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    serverTask(const std::string & taskName);
    ~serverTask(void) {};

    void Configure(const std::string & CMN_UNUSED(filename)) {};

protected:
    void Void(void);
    void Write(const mtsDouble & data);
    void QualifiedRead(const mtsDouble & data, mtsDouble & placeHolder) const;
    void SendButtonClickEvent() {
        EventVoid();
    }

    mtsFunctionVoid EventVoid;
    mtsFunctionWrite EventWrite;

    mtsDouble ReadValue;
};

CMN_DECLARE_SERVICES_INSTANTIATION(serverTask);

#endif  // _serverTask_h
