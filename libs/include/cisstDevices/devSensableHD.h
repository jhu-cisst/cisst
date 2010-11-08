/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#include <cisstDevices/devConfig.h>

#if defined CISST_DEV_HAS_SENSABLEHD || defined DOXYGEN

#include <cisstMultiTask/mtsTaskFromCallback.h>
#include <cisstParameterTypes.h>

// Always include last
#include <cisstDevices/devExport.h>

// forward declaration for private data
struct devSensableHDDriverData;
struct devSensableHDHandle;

class CISST_EXPORT devSensableHD: public mtsTaskFromCallbackAdapter {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    enum {NB_JOINTS = 6};
    int DeviceCount;

protected:
    // internal data using Sensable data types
    struct DeviceData {
        bool DeviceEnabled;
        bool ForceOutputEnabled;
        
        // local copy of the buttons state as defined by Sensable
        mtsInt Buttons;
        
        // local copy of the position and velocities
        prmPositionCartesianGet PositionCartesian;
        prmVelocityCartesianGet VelocityCartesian;
        prmPositionJointGet PositionJoint;
        vctDynamicVectorRef<double> GimbalJointsRef;
        
        // mtsFunction called to broadcast the event
        mtsFunctionWrite Button1Event, Button2Event;

        prmForceCartesianSet ForceCartesian;

        // local buffer used to store the position as provided
        // by Sensable
        typedef vctFixedSizeMatrix<double, 4, 4, VCT_COL_MAJOR> Frame4x4Type;
        Frame4x4Type Frame4x4;
        vctFixedSizeConstVectorRef<double, 3, Frame4x4Type::ROWSTRIDE> Frame4x4TranslationRef;
        vctFixedSizeConstMatrixRef<double, 3, 3,
                                   Frame4x4Type::ROWSTRIDE, Frame4x4Type::COLSTRIDE> Frame4x4RotationRef;
        
        // added to provide tip position to the frame manager
        mtsFunctionRead PositionFunctionForTransformationManager;

        bool Clutch;

        std::string Name;

        int DeviceNumber;
    };

    vctDynamicVector<DeviceData *> DevicesVector;
    vctDynamicVector<devSensableHDHandle *> DevicesHandleVector;
    devSensableHDDriverData * Driver;
    void SetInterfaces(const std::string & firstDeviceName,
                       const std::string & secondDeviceName,
                       bool firstDeviceForcesEnabled,
                       bool secondDeviceForcesEnabled);
    void SetInterfaces(const std::string & firstDeviceName,
                       const std::string & secondDeviceName,
                       const std::string & thirdDeviceName,
                       const std::string & fourthDeviceName,
                       bool firstDeviceForcesEnabled,
                       bool secondDeviceForcesEnabled,
                       bool thirdDeviceForcesEnabled,
                       bool fourthDeviceForcesEnabled);
    void SetupInterfaces(void);

private:
    /*! Default constructor, will use the default device connected and
      create and interface named "Default Arm" */
    devSensableHD(const std::string & taskName);

public:
    /*! Constructor for a single arm with a user specified name.  The
      name must match the device name as defined by Sensable
      drivers. Force output initially disabled*/
    devSensableHD(const std::string & taskName,
                  const std::string & firstDeviceName);

    /*! Constructor for a single arm with a user specified name.  The
      name must match the device name as defined by Sensable
      drivers. Overloaded to enable/disable force output.*/
    devSensableHD(const std::string & taskName,
                  const std::string & firstDeviceName,
                  bool firstDeviceForcesEnabled);

    /*! Constructor for two arms with a user specified names.  The
      names must match the device names as defined by Sensable
      drivers. Force output initially disabled */
    //@{
    devSensableHD(const std::string & taskName,
                  const std::string & firstDeviceName,
                  const std::string & secondDeviceName);

    devSensableHD(const char * taskName,
                  const char * firstDeviceName,
                  const char * secondDeviceName);
    //@}

    /*! Constructor for two arms with a user specified names.  The
      names must match the device names as defined by Sensable
      drivers. Overloaded to enable/disable force output. */
    devSensableHD(const std::string & taskName,
                  const std::string & firstDeviceName,
                  const std::string & secondDeviceName,
                  bool firstDeviceForcesEnabled,
                  bool secondDeviceForcesEnabled);

    /*! Constructor for four arms with a user specified names.  The
      names must match the device names as defined by Sensable
      drivers. Force output initially disabled */
    //@{
    devSensableHD(const std::string & taskName,
                  const std::string & firstDeviceName,
                  const std::string & secondDeviceName,
                  const std::string & thirdDeviceName,
                  const std::string & fourthDeviceName);

    devSensableHD(const char * taskName,
                  const char * firstDeviceName,
                  const char * secondDeviceName,
                  const char * thirdDeviceName,
                  const char * fourthDeviceName);
    //@}

    /*! Constructor for four arms with a user specified names.  The
      names must match the device names as defined by Sensable
      drivers. Overloaded to enable/disable force output. */
    devSensableHD(const std::string & taskName,
                  const std::string & firstDeviceName,
                  const std::string & secondDeviceName,
                  const std::string & thirdDeviceName,
                  const std::string & fourthDeviceName,
                  bool firstDeviceForcesEnabled,
                  bool secondDeviceForcesEnabled,
                  bool thirdDeviceForcesEnabled,
                  bool fourthDeviceForcesEnabled);


    ~devSensableHD();
    void Configure(const std::string & CMN_UNUSED(filename) = "") {};
	void Create(void *data = 0);
    void Run();
	void Start(void);
	void Kill(void);
    void Cleanup(void) {};
    virtual void UserControl(void) {};
};


CMN_DECLARE_SERVICES_INSTANTIATION(devSensableHD);


#endif // CISST_DEV_HAS_SENSABLEHD

#endif // _devSensableHD_h

