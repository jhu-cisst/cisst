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

#include "clientTask.h"

CMN_IMPLEMENT_SERVICES(clientTask);


clientTask::clientTask(const std::string & taskName) :
    mtsDevice(taskName)
{
    mtsRequiredInterface * required = AddRequiredInterface("Required");
    if (required) {
        required->AddFunction("Void", Server.Void);
        required->AddFunction("Write", Server.Write);
//        required->AddFunction("Read", Server.Read);
        required->AddFunction("QualifiedRead", Server.QualifiedRead);
        required->AddEventHandlerVoid(&clientTask::EventVoidHandler, this, "EventVoid");
        required->AddEventHandlerWrite(&clientTask::EventWriteHandler, this, "EventWrite");
    }
}


void clientTask::EventVoidHandler(void)
{
}


void clientTask::EventWriteHandler(const mtsDouble & value)
{
}
