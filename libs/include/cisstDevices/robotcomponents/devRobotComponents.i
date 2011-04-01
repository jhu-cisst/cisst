
#if CISST_DEV_HAS_OSG 

%include "std_list.i"
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
  import_array()   /* Initial function for NumPy */
%}

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


namespace std {
  %template(ContactList) list<devODEContact>;
}

#endif

#endif


