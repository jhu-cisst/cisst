/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsFaultTypes.cpp 3303 2012-01-02 16:33:23Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2012-02-07

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstMultiTask/mtsFaultTypes.h>

CMN_IMPLEMENT_SERVICES(mtsFaultComponentThreadPeriodicity)

mtsFaultComponentThreadPeriodicity::mtsFaultComponentThreadPeriodicity(void) 
    : mtsFaultBase("Thread Periodicity", 
                   BaseType::LAYER_COMPONENT, 
                   BaseType::FAULT_COMPONENT_FUNCTIONAL)
{
}

mtsFaultComponentThreadPeriodicity::~mtsFaultComponentThreadPeriodicity()
{
}

void mtsFaultComponentThreadPeriodicity::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);
}

void mtsFaultComponentThreadPeriodicity::SerializeRaw(std::ostream & outputStream) const
{
    BaseType::SerializeRaw(outputStream);
}

void mtsFaultComponentThreadPeriodicity::DeSerializeRaw(std::istream & inputStream)
{
    BaseType::DeSerializeRaw(outputStream);
}

