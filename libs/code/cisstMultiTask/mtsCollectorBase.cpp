/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung, Anton Deguet
  Created on: 2009-03-20

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsCollectorBase.h>
#include <cisstOSAbstraction/osaSleep.h>

CMN_IMPLEMENT_SERVICES(mtsCollectorBase)

unsigned int mtsCollectorBase::CollectorCount;
mtsTaskManager * mtsCollectorBase::TaskManager;

//-------------------------------------------------------
//	Constructor, Destructor, and Initializer
//-------------------------------------------------------
mtsCollectorBase::mtsCollectorBase(const std::string & collectorName,
                                   const CollectorFileFormat fileFormat)
    :
    mtsTaskFromSignal(collectorName),
    FileFormat(fileFormat)
{
    ++CollectorCount;

    if (TaskManager == 0) {
        TaskManager = mtsTaskManager::GetInstance();
    }

    // add a control interface to start and stop the data collection
    this->ControlInterface = AddProvidedInterface("Control");
    if (this->ControlInterface) {
        // commands controlling the output
        ControlInterface->AddCommandVoid(&mtsCollectorBase::SetOutputToDefault, this, "SetOutputToDefault");
        // start/stop commands
        ControlInterface->AddCommandVoid(&mtsCollectorBase::StartCollectionCommand, this, "StartCollection");
        ControlInterface->AddCommandWrite(&mtsCollectorBase::StartCollectionInCommand, this, "StartCollectionIn");
        ControlInterface->AddCommandVoid(&mtsCollectorBase::StopCollectionCommand, this, "StopCollection");
        ControlInterface->AddCommandWrite(&mtsCollectorBase::StopCollectionInCommand, this, "StopCollectionIn");
    }
    Init();
}


mtsCollectorBase::~mtsCollectorBase()
{
    --CollectorCount;
    CMN_LOG_CLASS_INIT_VERBOSE << "destructor: collector " << GetName() << " ends." << std::endl;
}


void mtsCollectorBase::Init()
{
    Status = COLLECTOR_STOP;
    ClearTaskMap();
}


void mtsCollectorBase::Cleanup(void)
{
    ClearTaskMap();
}


//-------------------------------------------------------
//	Miscellaneous Functions
//-------------------------------------------------------
void mtsCollectorBase::ClearTaskMap(void)
{
    if (!TaskMap.empty()) {
        TaskMapType::iterator itr = TaskMap.begin();
        SignalMapType::iterator _itr;
        for (; itr != TaskMap.end(); ++itr) {
            itr->second->clear();
            delete itr->second;
        }
        TaskMap.clear();
    }
}
