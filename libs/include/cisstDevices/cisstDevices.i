/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Anton Deguet
  Created on:   2008-01-17

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


%module cisstDevicesPython


%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "std_pair.i"
%include "std_streambuf.i"
%include "std_iostream.i"

%include "swigrun.i"

%import "cisstConfig.h"
%import "cisstDevices/devConfig.h"

%import "cisstCommon/cisstCommon.i"
%import "cisstVector/cisstVector.i"
%import "cisstMultiTask/cisstMultiTask.i"
%import "cisstParameterTypes/cisstParameterTypes.i"

%init %{
    import_array() // numpy initialization
%}

%header %{
#include <cisstDevices/devPython.h>
%}

// Generate parameter documentation for IRE
%feature("autodoc", "1");

%rename(__str__) ToString;
%ignore *::ToStream;
%ignore operator<<;
%ignore operator=;

%ignore *::operator[]; // We define __setitem__ and __getitem__

#define CISST_EXPORT
#define CISST_DEPRECATED

// Wrap devices
#if CISST_HAS_XML
    %include "cisstDevices/devNDISerial.h"
    #if CISST_DEV_HAS_MICRONTRACKER
        %include "cisstDevices/devMicronTracker.h"
    #endif
#endif

%include "cisstDevices/devKeyboard.h"

#if CISST_DEV_HAS_ROBOTCOMPONENTS

  #if CISST_DEV_HAS_OSG 

    %header %{
      #include <cisstVector/vctTransformationTypes.h>
      #include <cisstDevices/devPython.h>

      #include <cisstDevices/robotcomponents/devRobotComponent.h>

      #include <cisstDevices/robotcomponents/osg/devOSGWorld.h>
      #include <cisstDevices/robotcomponents/osg/devOSGCamera.h>
      #include <cisstDevices/robotcomponents/osg/devOSGMono.h>
      #include <cisstDevices/robotcomponents/osg/devOSGStereo.h>
      #include <cisstDevices/robotcomponents/osg/devOSGBody.h>

     %}

     %include "cisstDevices/robotcomponents/osg/devOSGWorld.h"
     %include "cisstDevices/robotcomponents/osg/devOSGCamera.h"
     %include "cisstDevices/robotcomponents/osg/devOSGMono.h"
     %include "cisstDevices/robotcomponents/osg/devOSGStereo.h"
     %include "cisstDevices/robotcomponents/osg/devOSGBody.h"

    #if CISST_DEV_HAS_ODE

      %header %{
        #include <cisstVector/vctTransformationTypes.h>
        #include <cisstDevices/devPython.h>

        #include <cisstDevices/robotcomponents/devRobotComponent.h>

        #include <cisstDevices/robotcomponents/osg/devOSGWorld.h>
        #include <cisstDevices/robotcomponents/osg/devOSGCamera.h>
        #include <cisstDevices/robotcomponents/osg/devOSGMono.h>
        #include <cisstDevices/robotcomponents/osg/devOSGStereo.h>
        #include <cisstDevices/robotcomponents/osg/devOSGBody.h>

        #include <cisstDevices/robotcomponents/ode/devODEWorld.h>
        #include <cisstDevices/robotcomponents/ode/devODEBody.h>
      %}

      %include "cisstDevices/robotcomponents/osg/devOSGWorld.h"
      %include "cisstDevices/robotcomponents/osg/devOSGCamera.h"
      %include "cisstDevices/robotcomponents/osg/devOSGMono.h"
      %include "cisstDevices/robotcomponents/osg/devOSGStereo.h"
      %include "cisstDevices/robotcomponents/osg/devOSGBody.h"

      %include "cisstDevices/robotcomponents/ode/devODEWorld.h"
      %include "cisstDevices/robotcomponents/ode/devODEBody.h"

    #endif
  
  #endif

#endif


