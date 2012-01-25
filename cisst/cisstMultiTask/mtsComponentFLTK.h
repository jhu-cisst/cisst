/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsComponentFLTK_h
#define _mtsComponentFLTK_h

/*!
  \file
  \brief Declaration of mtsComponentFLTK
*/


/*!
  \ingroup cisstMultiTask

  mtsComponentFLTK is a templated class to facilitate creation of components that use FLTK.
  It is templated by a BaseClass, which typically contains the FLTK GUI code. This class is
  not instantiated within the cisstMultiTask library, but is available for use by application
  components. For an example of use, see cisstMultiTaskPeriodicTask (FLTK implementation).

  The use of a templated class to derive from the FLTK GUI class is a bit odd, as a more obvious
  solution would be for mtsComponentFLTK to be a (non-templated) base class, and then to derive
  the FLTK GUI class from it. But, this solution does not require the FLTK GUI class to use
  CISST_EXPORT and CMN_DECLARE_SERVICES, which are a bit difficult to incorporate when using the
  FLTK fluid designer. An alternate design is to create a generic "wrapper" template (perhaps
  in cisstCommon) to add the CISST_EXPORT and CMN_DECLARE_SERVICES to an existing class. Then,
  the rest of mtsComponentFLTK can become a non-templated base class.
*/

#include <cisstCommon/cmnClassRegisterMacros.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <FL/Fl.H>

template <class BaseClass>
class CISST_EXPORT mtsComponentFLTK : public BaseClass {

    typedef mtsComponentFLTK<BaseClass> ThisType;
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_ALL)

public:

    mtsComponentFLTK(const std::string &name) : BaseClass(name)
    {
        mtsInterfaceRequired *interfaceRequired = this->AddInterfaceRequired("Dispatcher", MTS_OPTIONAL);
        if (interfaceRequired) {
            interfaceRequired->AddEventHandlerVoid(&mtsComponentFLTK<BaseClass>::CheckFLTK, this,
                                                   "RunEvent", MTS_EVENT_NOT_QUEUED);
        }
        this->Thread.CreateFromThreadId(mtsManagerLocal::GetInstance()->GetMainThreadId());
    }

    ~mtsComponentFLTK() {}

    static void IdleCallback(void *data)
    {
        ThisType * whichUI = reinterpret_cast<ThisType *>(data);
        whichUI->DoCallback();  // calls Run method
    }

    void Startup(void)
    {
        BaseClass::Startup();
        Fl::add_idle(mtsComponentFLTK<BaseClass>::IdleCallback, this);
    }

    void Run(void)
    {
        BaseClass::Run();
    }

    void Cleanup(void)
    {
        BaseClass::Cleanup();
        Fl::remove_idle(mtsComponentFLTK<BaseClass>::IdleCallback, this);
    }

    static int StartRunLoop(void)
    {
        return Fl::run(); 
    }

    void CheckFLTK(void)
    {
        this->DoCallback();
        Fl::check();
    }
};

#endif // _mtsComponentFLTK_h
