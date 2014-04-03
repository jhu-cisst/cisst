/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2008-11-13

  (C) Copyright 2008-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsInterfaceRequiredProxy_h
#define _mtsInterfaceRequiredProxy_h

#include <cisstMultiTask/mtsInterfaceRequired.h>

class mtsFunctionReturnProxyBase;

class mtsInterfaceRequiredProxy: public mtsInterfaceRequired
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    mtsFunctionReturnProxyBase * LastFunction;

public:

    mtsInterfaceRequiredProxy(const std::string & interfaceName, mtsComponent * component,
                              mtsMailBox * mailBox, mtsRequiredType required = MTS_REQUIRED);

    /*! Default destructor. */
    virtual ~mtsInterfaceRequiredProxy();

    void SetLastFunction(mtsFunctionReturnProxyBase * lastFunction);

    void ResetLastFunction(void);

    mtsFunctionReturnProxyBase * GetLastFunction(void) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsInterfaceRequiredProxy)

#endif // _mtsInterfaceRequiredProxy_h
