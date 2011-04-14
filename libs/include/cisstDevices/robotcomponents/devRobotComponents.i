
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

  #include <cisstOSAbstraction/osaCPUAffinity.h>
  #include <cisstRobot/robManipulator.h>

  #include <cisstDevices/robotcomponents/devRobotComponent.h>
  #include <cisstDevices/robotcomponents/manipulators/devManipulator.h>

  #include <cisstDevices/robotcomponents/trajectories/devSetPoints.h>
  #include <cisstDevices/robotcomponents/trajectories/devTrajectory.h>
  #include <cisstDevices/robotcomponents/trajectories/devLinearRn.h>

%}

%include "cisstOSAbstraction/osaCPUAffinity.h"
%include "cisstRobot/robManipulator.h"

%include "cisstDevices/robotcomponents/devRobotComponent.h"
%include "cisstDevices/robotcomponents/manipulators/devManipulator.h"

%include "cisstDevices/robotcomponents/trajectories/devSetPoints.h"
%include "cisstDevices/robotcomponents/trajectories/devTrajectory.h"
%include "cisstDevices/robotcomponents/trajectories/devLinearRn.h"


#if CISST_DEV_HAS_OSG 

%header %{
  #include <cisstVector/vctTransformationTypes.h>
  #include <cisstDevices/devPython.h>
  
  #include <cisstDevices/robotcomponents/osg/devOSGWorld.h>
  #include <cisstDevices/robotcomponents/osg/devOSGCamera.h>
  #include <cisstDevices/robotcomponents/osg/devOSGMono.h>
  #include <cisstDevices/robotcomponents/osg/devOSGStereo.h>
  #include <cisstDevices/robotcomponents/osg/devOSGBody.h>
  #include <cisstDevices/robotcomponents/osg/devOSGManipulator.h>
%}

%include "cisstDevices/robotcomponents/osg/devOSGWorld.h"
%include "cisstDevices/robotcomponents/osg/devOSGCamera.h"
%include "cisstDevices/robotcomponents/osg/devOSGMono.h"
%include "cisstDevices/robotcomponents/osg/devOSGStereo.h"
%include "cisstDevices/robotcomponents/osg/devOSGBody.h"
%include "cisstDevices/robotcomponents/osg/devOSGManipulator.h"

#if CISST_DEV_HAS_ODE

%header %{
  #include <cisstDevices/robotcomponents/ode/devODEWorld.h>
  #include <cisstDevices/robotcomponents/ode/devODEBody.h>
  #include <cisstDevices/robotcomponents/ode/devODEManipulator.h>
  #include <cisstDevices/robotcomponents/ode/devODEBH.h>
%}

%include "cisstDevices/robotcomponents/ode/devODEWorld.h"
%include "cisstDevices/robotcomponents/ode/devODEBody.h"
%include "cisstDevices/robotcomponents/ode/devODEManipulator.h"
%include "cisstDevices/robotcomponents/ode/devODEBH.h"

namespace std {
  %template(ContactList) list<devODEContact>;
  %template(Visibility) list<devOSGBody*>;
  %template(VisibilityList) list< list<devOSGBody*> >;
}

#endif

#endif

namespace std{
  %template(SetPointsList) list< vctDynamicVector<double> >;
}

