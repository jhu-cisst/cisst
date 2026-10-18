/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2010-03-19

  (C) Copyright 2010-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnPortability.h>

#include <cisstMultiTask/mtsInterfaceCommon.h>
#include "cisstCommon/cmnClassRegisterMacros.h"

// If not using MingW, compile as a single file.
#if (CISST_OS != CISST_WINDOWS) || (CISST_COMPILER != CISST_GCC)
#define MTS_CLASS_SERVICES_PART1
#define MTS_CLASS_SERVICES_PART2
#define MTS_CLASS_SERVICES_PART3
#endif

#ifdef MTS_CLASS_SERVICES_PART1

#include <cisstMultiTask/mtsCollectorEvent.h>
CMN_IMPLEMENT_SERVICES_DERIVED(mtsCollectorEvent, mtsCollectorBase)   // derives from mtsTaskFromSignal

#include <cisstMultiTask/mtsCollectorState.h>
CMN_IMPLEMENT_SERVICES_DERIVED(mtsCollectorState, mtsCollectorBase)   // derives from mtsTaskFromSignal

#include <cisstMultiTask/mtsCollectorFactory.h>
CMN_IMPLEMENT_SERVICES(mtsCollectorFactory)

#include <cisstMultiTask/mtsComponent.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsComponentConstructorNameAndInt)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsComponentConstructorNameAndUInt)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsComponentConstructorNameAndLong)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsComponentConstructorNameAndULong)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsComponentConstructorNameAndDouble)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsComponentConstructorNameAndString)
//CMN_IMPLEMENT_SERVICES(mtsComponent)
// mtsComponent: allow dynamic creation with std::string constructor arg
CMN_IMPLEMENT_SERVICES_INTERNAL(mtsComponent, 0, mtsGenericTypes<std::string>::FinalType)
// mtsComponentWithManagement: allow dynamic creation with std::string constructor arg
CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mtsComponentWithManagement, mtsComponent, std::string)

#include <cisstMultiTask/mtsComponentAddLatency.h>
CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mtsComponentAddLatency, mtsTaskPeriodic, mtsTaskPeriodicConstructorArg)

#include <cisstMultiTask/mtsInterfaceProvided.h>
CMN_IMPLEMENT_SERVICES(mtsInterfaceProvided)

#include <cisstMultiTask/mtsInterfaceRequired.h>
CMN_IMPLEMENT_SERVICES(mtsInterfaceRequired)

#include <cisstMultiTask/mtsStateIndex.h>
CMN_IMPLEMENT_SERVICES(mtsStateIndex)

#include <cisstMultiTask/mtsStateTable.h>
CMN_IMPLEMENT_SERVICES(mtsStateTable)
CMN_IMPLEMENT_SERVICES(mtsStateTableIndexRange)

#include <cisstMultiTask/mtsTask.h>
CMN_IMPLEMENT_SERVICES_DERIVED(mtsTask, mtsComponent)

#include <cisstMultiTask/mtsTaskContinuous.h>
CMN_IMPLEMENT_SERVICES_DERIVED(mtsTaskContinuous, mtsTask)
CMN_IMPLEMENT_SERVICES_DERIVED(mtsTaskMain, mtsTaskContinuous)

#include <cisstMultiTask/mtsTaskFromCallback.h>
CMN_IMPLEMENT_SERVICES_DERIVED(mtsTaskFromCallback, mtsTask)
CMN_IMPLEMENT_SERVICES(mtsTaskFromCallbackAdapter)

#include <cisstMultiTask/mtsTaskFromSignal.h>
CMN_IMPLEMENT_SERVICES_DERIVED(mtsTaskFromSignal, mtsTaskContinuous)

#include <cisstMultiTask/mtsTaskPeriodic.h>
CMN_IMPLEMENT_SERVICES_DERIVED(mtsTaskPeriodic, mtsTaskContinuous)

#endif  // MTS_CLASS_SERVICES_PART1

#ifdef MTS_CLASS_SERVICES_PART2

#include <cisstMultiTask/mtsGenericObjectProxy.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDouble)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsFloat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsLong)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsLongLong)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsULong)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsInt)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUInt)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsShort)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUShort)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsChar)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUChar)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsBool)
#if (CISST_OS == CISST_WINDOWS)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsSizeT);
#endif
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsStdString)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsStdStringVecProxy)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsStdDoubleVecProxy)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsStdCharVecProxy)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsStdStringListProxy)

CMN_IMPLEMENT_SERVICES_TEMPLATED(cmnJointTypeProxy)

#endif  // MTS_CLASS_SERVICES_PART2

#ifdef MTS_CLASS_SERVICES_PART3
#include <cisstMultiTask/mtsGenericObjectProxy.h>

#endif // MTS_CLASS_SERVICES_PART3

#ifdef MTS_CLASS_SERVICES_PART2

// Various Eigen types
typedef mtsGenericObjectProxy<Eigen::Vector<double, 1>> eigenvec1d;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenvec1d)
typedef mtsGenericObjectProxy<Eigen::Vector<double, 2>> eigenvec2d;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenvec2d)
typedef mtsGenericObjectProxy<Eigen::Vector<double, 3>> eigenvec3d;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenvec3d)
typedef mtsGenericObjectProxy<Eigen::Vector<double, 4>> eigenvec4d;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenvec4d)
typedef mtsGenericObjectProxy<Eigen::Vector<double, 5>> eigenvec5d;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenvec5d)
typedef mtsGenericObjectProxy<Eigen::Vector<double, 6>> eigenvec6d;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenvec6d)
typedef mtsGenericObjectProxy<Eigen::VectorXd> eigenvectorxd;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenvectorxd)

typedef mtsGenericObjectProxy<Eigen::Quaterniond> eigenquaterniond;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenquaterniond)
typedef mtsGenericObjectProxy<Eigen::AngleAxisd> eigenangleaxisd;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenangleaxisd)
typedef mtsGenericObjectProxy<Eigen::Affine3d> eigenaffine3d;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenaffine3d)
typedef mtsGenericObjectProxy<Eigen::Isometry3d> eigenisometry3d;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenisometry3d)
typedef mtsGenericObjectProxy<Eigen::Matrix2d> eigenmatrix2d;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenmatrix2d)
typedef mtsGenericObjectProxy<Eigen::Matrix3d> eigenmatrix3d;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenmatrix3d)
typedef mtsGenericObjectProxy<Eigen::MatrixXd> eigenmatrixXd;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenmatrixXd)
typedef mtsGenericObjectProxy<Eigen::Matrix<double, 6, Eigen::Dynamic>> eigenmatrix6Xd;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenmatrix6Xd)

typedef mtsGenericObjectProxy<Eigen::ArrayXd> eigenarrayXd;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenarrayXd);
typedef mtsGenericObjectProxy<Eigen::Array2d> eigenarray2d;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenarray2d);
typedef mtsGenericObjectProxy<Eigen::Array3d> eigenarray3d;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenarray3d);
typedef mtsGenericObjectProxy<Eigen::Array4d> eigenarray4d;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenarray4d);
typedef mtsGenericObjectProxy<Eigen::Array<double, 5, 1>> eigenarray5d;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenarray5d);
typedef mtsGenericObjectProxy<Eigen::Array<double, 6, 1>> eigenarray6d;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenarray6d);

typedef mtsGenericObjectProxy<Eigen::ArrayX<bool>> eigenarrayXb;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenarrayXb);
typedef mtsGenericObjectProxy<Eigen::Array2<bool>> eigenarray2b;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenarray2b);
typedef mtsGenericObjectProxy<Eigen::Array3<bool>> eigenarray3b;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenarray3b);
typedef mtsGenericObjectProxy<Eigen::Array4<bool>> eigenarray4b;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenarray4b);
typedef mtsGenericObjectProxy<Eigen::Array<bool, 5, 1>> eigenarray5b;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenarray5b);
typedef mtsGenericObjectProxy<Eigen::Array<bool, 6, 1>> eigenarray6b;
CMN_IMPLEMENT_SERVICES_TEMPLATED(eigenarray6b);

#endif  // MTS_CLASS_SERVICES_PART2

#ifdef MTS_CLASS_SERVICES_PART1

#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsManagerLocal.h>
CMN_IMPLEMENT_SERVICES(mtsManagerLocal)

#include <cisstMultiTask/mtsInterfaceCommon.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsInterfaceProvidedDescriptionProxy);
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsInterfaceRequiredDescriptionProxy);

#include <cisstMultiTask/mtsComponentState.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsComponentStateProxy);

#include <cisstMultiTask/mtsExecutionResult.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsExecutionResultProxy);

#include <cisstMultiTask/mtsParameterTypes.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionComponentProxy);
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionComponentVecProxy);

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionComponentClassProxy);
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionComponentClassVecProxy);

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionInterfaceProxy);

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionInterfaceFullNameProxy);
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionInterfaceFullNameVecProxy);

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionConnectionProxy);
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionConnectionVecProxy);

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsComponentStatusControlProxy);

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsComponentStateChangeProxy);

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsEndUserInterfaceArgProxy);

CMN_IMPLEMENT_SERVICES(mtsEventHandlerList);

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionLoadLibraryProxy);

CMN_IMPLEMENT_SERVICES(mtsLogMessage);

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsMessageProxy);

CMN_IMPLEMENT_SERVICES(mtsComponentConstructorArg);
CMN_IMPLEMENT_SERVICES(mtsTaskConstructorArg);
CMN_IMPLEMENT_SERVICES(mtsTaskContinuousConstructorArg);
CMN_IMPLEMENT_SERVICES(mtsTaskPeriodicConstructorArg);

#include <cisstMultiTask/mtsComponentViewer.h>
CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mtsComponentViewer, mtsTaskFromSignal, std::string)

#include <cisstMultiTask/mtsSocketProxyClient.h>
CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mtsSocketProxyClient, mtsTaskContinuous, mtsSocketProxyClientConstructorArg)

#include <cisstMultiTask/mtsSocketProxyServer.h>
CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mtsSocketProxyServer, mtsTaskContinuous, mtsSocketProxyServerConstructorArg)

#endif  // MTS_CLASS_SERVICES_PART1
