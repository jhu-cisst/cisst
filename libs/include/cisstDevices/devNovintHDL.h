/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: devNovintHDL.h 683 2009-08-14 21:40:14Z gsevinc1 $

  Author(s): Gorkem Sevinc, Anton Deguet
  Created on: 2009-09-04

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _devNovintHDL_h
#define _devNovintHDL_h

#include <cisstDevices/devConfig.h>

//#if defined CISST_DEV_HAS_NOVINTHDL || defined DOXYGEN
#include <cisstMultiTask/mtsTaskFromCallback.h>
#include <cisstParameterTypes.h>

// Always include last
#include <cisstDevices/devExport.h>


// forward declaration for private data
struct devNovintHDLDriverData;
struct devNovintHDLHandle;

class CISST_EXPORT devNovintHDL: public mtsTaskFromCallbackAdapter {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 10);

public:
    enum {NB_JOINTS = 3};
    int DeviceCount;

protected:
    // internal data using Novint data types
    struct DeviceData {
        bool DeviceEnabled;

        // local copy of the buttons state as defined by Sensable
        //mtsInt Buttons;
        //bool ButtonPressed;
        mtsInt  Buttons;
        
        // local copy of the position and velocities
        prmPositionCartesianGet PositionCartesian;
        
        // mtsFunction called to broadcast the event
        mtsFunctionWrite Button1Event, Button2Event, Button3Event, Button4Event;

        prmForceCartesianSet ForceCartesian;

        bool Clutch;

        std::string Name;

        int DeviceNumber;
    };

    vctDynamicVector<DeviceData *> DevicesVector;
    vctDynamicVector<devNovintHDLHandle *> DevicesHandleVector;
    devNovintHDLDriverData * Driver;

    void SetInterfaces(const std::string & firstDeviceName,
                       const std::string & secondDeviceName);
    
    void SetInterfaces(const std::string & firstDeviceName,
                       const std::string & secondDeviceName,
                       const std::string & thirdDeviceName,
                       const std::string & fourthDeviceName);
    
    void SetupInterfaces(void);

private:
    /*! Default constructor, will use the default device connected and
      create and interface named "Default Arm" */
    devNovintHDL(const std::string & taskName);

public:
    /*! Constructor for a single arm with a user specified name.  The
      name must match the device name as defined by Sensable
      drivers. Force output initially disabled*/
    devNovintHDL(const std::string & taskName,
                 const std::string & firstDeviceName);

    /*! Constructor for two arms with a user specified names.  The
      names must match the device names as defined by Sensable
      drivers. Force output initially disabled */
    //@{
    devNovintHDL(const std::string & taskName,
                  const std::string & firstDeviceName,
                  const std::string & secondDeviceName);

    devNovintHDL(const char * taskName,
                  const char * firstDeviceName,
                  const char * secondDeviceName);
    //@}

    /*! Constructor for four arms with a user specified names.  The
      names must match the device names as defined by Sensable
      drivers. Force output initially disabled */
    //@{
    devNovintHDL(const std::string & taskName,
                  const std::string & firstDeviceName,
                  const std::string & secondDeviceName,
                  const std::string & thirdDeviceName,
                  const std::string & fourthDeviceName);

    devNovintHDL(const char * taskName,
                  const char * firstDeviceName,
                  const char * secondDeviceName,
                  const char * thirdDeviceName,
                  const char * fourthDeviceName);
    //@}

    ~devNovintHDL();
    void Configure(const std::string & CMN_UNUSED(filename) = "") {};
	void Create(void *data = 0);
    void Run();
	void Start(void);
	void Kill(void);
    void Cleanup(void) {};
    virtual void UserControl(void) {};
};


CMN_DECLARE_SERVICES_INSTANTIATION(devNovintHDL);


//#endif // CISST_DEV_HAS_NOVINTHDL

#endif // _devNovintHDL_h

