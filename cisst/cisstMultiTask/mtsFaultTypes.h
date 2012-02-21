/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsFaultTypes.h 3034 2011-10-09 01:53:36Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2012-02-07

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

/*!
  \file
  \brief Declaration of various types of faults
  \ingroup cisstMultiTask
 */


#ifndef _mtsFaultTypes_h
#define _mtsFaultTypes_h

#include <cisstMultiTask/mtsFaultBase.h>

//-----------------------------------------------------------------------------
//  System Layer
//
//  System Layer - Faults From Process Layer
class mtsFaultSystemFromSubLayer: public mtsFaultBase
{
public:
    mtsFaultSystemFromSubLayer();
};

//-----------------------------------------------------------------------------
//  Process Layer
//
//  Process Layer - Faults From Component Layer
class mtsFaultProcessFromSubLayer: public mtsFaultBase
{
public:
    mtsFaultProcessFromSubLayer(const std::string & targetProcessName);
};

//-----------------------------------------------------------------------------
//  Component Layer
//
//  Component Layer - Functional Integrity - Thread Periodicity
class mtsFaultComponentThreadPeriodicity: public mtsFaultBase
{
public:
    mtsFaultComponentThreadPeriodicity(const std::string & targetProcessName,
                                       const std::string & targetComponentName);
};

//  Component Layer - Faults From Interface Layer
class mtsFaultComponentFromSubLayer: public mtsFaultBase
{
public:
    mtsFaultComponentFromSubLayer(const std::string & TargetProcessName,
                                  const std::string & TargetComponentName);
};

//-----------------------------------------------------------------------------
//  Interface Layer
//
//  Interface Layer - Faults From Execution Layer
class mtsFaultInterfaceFromSubLayer: public mtsFaultBase
{
public:
    mtsFaultInterfaceFromSubLayer(const std::string & TargetProcessName,
                                  const std::string & TargetComponentName,
                                  const std::string & TargetInterfaceName);
};

#endif // _mtsFaultComponentThreadPeriodicity_h
