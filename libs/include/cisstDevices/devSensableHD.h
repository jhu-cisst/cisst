/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: devSensableHD.h,v 1.7 2008/10/21 20:35:48 anton Exp $

  Author(s): Anton Deguet
  Created on: 2008-04-04

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _devSensableHD_h
#define _devSensableHD_h

#include <cisstMultiTask/mtsTaskFromCallback.h>
#include <cisstMultiTask/mtsStateData.h>
#include <cisstParameterTypes.h>

// Always include last
#include <cisstDevices/devExport.h>

// forward declaration for private data
struct devSensableHDDeviceData;
struct devSensableHDDriverData;

class CISST_EXPORT devSensableHD: public mtsTaskFromCallbackAdapter {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 10);

public:
    enum {NB_JOINTS = 6};

    typedef std::map<std::string, devSensableHDDeviceData *> DevicesMapType;

protected:
    // internal data using Sensable data types
    DevicesMapType Devices;
    devSensableHDDriverData * Driver;
    void SetupInterfaces(void);

public:
    /*! Default constructor, will use the default device connected and
      create and interface named "Default Arm" */
    devSensableHD(const std::string & taskName);

    /*! Constructor for a single arm with a user specified name.  The
      name must match the device name as defined by Sensable
      drivers. */
    devSensableHD(const std::string & taskName,
                  const std::string & firstDeviceName);

    /*! Constructor for two arms with a user specified names.  The
      names must match the device names as defined by Sensable
      drivers. */
    devSensableHD(const std::string & taskName,
                  const std::string & firstDeviceName,
                  const std::string & secondDeviceName);

    ~devSensableHD();
    void Configure(const std::string & CMN_UNUSED(filename) = "") {};
	void Create(void *data = 0);
    void Run();
	void Start(void);
	void Kill(void);
    void Cleanup(void) {};
};


CMN_DECLARE_SERVICES_INSTANTIATION(devSensableHD);


#endif // _devSensableHD_h

