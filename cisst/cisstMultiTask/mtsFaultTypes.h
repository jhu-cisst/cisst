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

#include <cisstMultiTask/mtsExport.h>

//
//  System Layer
//
//-----------------------------------------------------------------------------
//  System Layer - Faults From Process Layer
//
class CISST_EXPORT mtsFaultSystemFromSubLayer: public mtsFaultBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    std::string TargetProcessName;
    std::string TargetComponentName;

public:
    /*! Constructors and destructor */
    mtsFaultSystemFromSubLayer();
    ~mtsFaultSystemFromSubLayer();

    void SetFaultLocation(void);

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsFaultSystemFromSubLayer);

//
//  Process Layer
//
//-----------------------------------------------------------------------------
//  Process Layer - Faults From Component Layer
//
class CISST_EXPORT mtsFaultProcessFromSubLayer: public mtsFaultBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    std::string TargetProcessName;

public:
    /*! Constructors and destructor */
    mtsFaultProcessFromSubLayer(void);
    mtsFaultProcessFromSubLayer(const std::string & targetProcessName);
    ~mtsFaultProcessFromSubLayer();

    void SetFaultLocation(void);

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsFaultProcessFromSubLayer);

//
//  Component Layer
//
//-----------------------------------------------------------------------------
//  Component Layer - Functional Integrity - Thread Periodicity
//
class CISST_EXPORT mtsFaultComponentThreadPeriodicity: public mtsFaultBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    std::string TargetProcessName;
    std::string TargetComponentName;

public:
    /*! Constructors and destructor */
    mtsFaultComponentThreadPeriodicity(void);
    mtsFaultComponentThreadPeriodicity(const std::string & TargetProcessName,
                                       const std::string & TargetComponentName);
    virtual ~mtsFaultComponentThreadPeriodicity();

    void SetFaultLocation(void);

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsFaultComponentThreadPeriodicity);

//-----------------------------------------------------------------------------
//  Component Layer - Faults From Interface Layer
//
class CISST_EXPORT mtsFaultComponentFromSubLayer: public mtsFaultBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    std::string TargetProcessName;
    std::string TargetComponentName;

public:
    /*! Constructors and destructor */
    mtsFaultComponentFromSubLayer(void);
    mtsFaultComponentFromSubLayer(const std::string & TargetProcessName,
                                  const std::string & TargetComponentName);
    ~mtsFaultComponentFromSubLayer();

    void SetFaultLocation(void);

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsFaultComponentFromSubLayer);

//
//  Interface Layer
//
//-----------------------------------------------------------------------------
//  Interface Layer - Faults From Execution Layer
//
class CISST_EXPORT mtsFaultInterfaceFromSubLayer: public mtsFaultBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    std::string TargetProcessName;
    std::string TargetComponentName;
    std::string TargetInterfaceName;

public:
    /*! Constructors and destructor */
    mtsFaultInterfaceFromSubLayer(void);
    mtsFaultInterfaceFromSubLayer(const std::string & TargetProcessName,
                                  const std::string & TargetComponentName,
                                  const std::string & TargetInterfaceName);
    ~mtsFaultInterfaceFromSubLayer();

    void SetFaultLocation(void);

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsFaultInterfaceFromSubLayer);

#endif // _mtsFaultComponentThreadPeriodicity_h
