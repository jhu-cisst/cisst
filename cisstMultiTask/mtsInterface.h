/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Peter Kazanzides, Anton Deguet, Min Yang Jung
  Created on: 2008-11-13

  (C) Copyright 2008-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsInterface_h
#define _mtsInterface_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>


/*!
  \file
  \brief Declaration of mtsInterface
 */


/*!
  \ingroup cisstMultiTask

  Base class for all interfaces.
 */

class CISST_EXPORT mtsInterface: public cmnGenericObject
{
protected:

    /*! A string identifying the 'Name' of this interface. */
    const std::string Name;

    /*! Pointer on the device itself. */
    mtsComponent * Component;

 public:
    /*! Constructor. Sets the name.
        \param interfaceName Name of interface
    */
    mtsInterface(const std::string & interfaceName,
                 mtsComponent * component);

    /*! Default destructor. */
    virtual ~mtsInterface();

    /*! Returns the name of the interface. */
    const std::string & GetName(void) const;

    /*! Returns the name of the this interface as well as the
      component that owns the interface. */
    const std::string GetFullName(void) const;

    /*! Get a const pointer on the component */
    const mtsComponent * GetComponent(void) const;

    /*! Returns name of owner component */
    const std::string & GetComponentName(void) const;
};


#endif // _mtsInterface_h

