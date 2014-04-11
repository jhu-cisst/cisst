/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#error File deprecated

#ifndef _mtsInterfaceProvidedOrOutput_h
#define _mtsInterfaceProvidedOrOutput_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegisterMacros.h>
#include <cisstCommon/cmnNamedMap.h>
#include <cisstCommon/cmnPortability.h>

#include <cisstMultiTask/mtsCommandBase.h>
#include <cisstMultiTask/mtsCommandVoid.h>
#include <cisstMultiTask/mtsCommandRead.h>
#include <cisstMultiTask/mtsCommandWrite.h>
#include <cisstMultiTask/mtsCommandQualifiedRead.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsMulticastCommandWrite.h>
#include <cisstMultiTask/mtsMulticastCommandVoid.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionWrite.h>
#include <cisstMultiTask/mtsStateTable.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \file
  \brief Declaration of mtsInterfaceProvidedOrOutput
  \ingroup cisstMultiTask

  \sa mtsInterfaceProvided

 */
class CISST_EXPORT mtsInterfaceProvidedOrOutput: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 protected:

    /*! A string identifying the 'Name' of the interface. */
    const std::string Name;

    /*! Pointer on the component itself. */
    mtsComponent * Component;

    /*! Default constructor. Does nothing, should not be used. */
    mtsInterfaceProvidedOrOutput(void) {}

 public:

    /*! Constructor. Sets the name. */
    mtsInterfaceProvidedOrOutput(const std::string & interfaceName,
                                 mtsComponent * component);

    /*! Default destructor. Does nothing. */
    virtual ~mtsInterfaceProvidedOrOutput() {}

    /*! Returns the name of the interface. */
    const std::string & GetName(void) const;

    /*! Returns the name of the this interface as well as the
      component that owns the interface. */
    const std::string GetFullName(void) const;

    /*! Get a const pointer on the component */
    const mtsComponent * GetComponent(void) const;

    /*! Cleanup method */
    virtual void Cleanup(void);
};




CMN_DECLARE_SERVICES_INSTANTIATION(mtsInterfaceProvidedOrOutput)


#endif // _mtsInterfaceProvidedOrOutput_h

