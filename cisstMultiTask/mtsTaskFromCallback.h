/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Peter Kazanzides
  Created on: 2008-09-10

  (C) Copyright 2008-2017 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines a task created from a callback.
*/

#ifndef _mtsTaskFromCallback_h
#define _mtsTaskFromCallback_h

#include <cisstCommon/cmnPortability.h>
#include <cisstMultiTask/mtsTask.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

#ifndef MTS_TASK_CALLBACK_CONVENTION
#define MTS_TASK_CALLBACK_CONVENTION
#endif

/*!
  \ingroup cisstMultiTask

  This is a task that has its thread provided by a callback from an
  external piece of software.

  If the external package accepts a user-supplied 'void *' as the first
  parameter, then mtsTaskFromCallbackAdapter can be used; this class provides
  an adapter that expects the 'void *' parameter to contain a pointer to an instance
  of the mtsTaskFromCallbackAdapter class.

  The mtsTaskFromCallback class can be used in all cases.
  The derived class must provide a callback function (e.g., a static function)
  and implement an alternate method to get the 'this' pointer.
  The user-created callback function should call RunInternal and return the data
  (if any).

  One suggestion is to make the derived class a Singleton. The derived class should
  contain a (static) CustomCallbackAdapter  function that has the correct signature.
  For example, if we create a derived class called MyCallbackClass:

     static int MyCallbackClass::CustomCallbackAdapter(char *a, int b) {
         MyCallbackClass *obj = MyCallbackClass::GetInstance();
         // Avoid re-entrancy problems by creating a CallbackParms object on the
         // stack and passing it to RunInternal, which already contains a software
         // guard to prevent re-entrant calls.
         // Alternatively, the data can be copied to class members, which can then
         // be accessed inside the Run method. In this case, the CustomCallbackAdapter
         // should handle the re-entrancy problems (e.g., if CustomCallbackAdapter is
         // called again before the last call has finished, it could overwrite the previous
         // values stored in the class members).
         MyCallbackClass::CallbackParms data(a,b);
         void *ret = RunInternal(static_cast<void *>(data));
         // Run method can use data.a and data.b and set the return value
         // by calling mtsTask::SetThreadReturnValue.
         return static_cast<int>(ret);
     }
*/

class CISST_EXPORT mtsTaskFromCallback : public mtsTask
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    typedef mtsTask BaseType;

    /************************ Protected task data  *********************/
    bool inRunInternal;

    /********************* Methods that call user methods *****************/

    /*! The member function that is passed as 'start routine' argument for
      thread creation. */
    void *RunInternal(void* argument);

    /*! The member funtion that is executed as soon as the thread gets created.
      It does some housekeeping before the user code can be executed. */
    void StartupInternal(void);

public:
    /********************* Task constructor and destructor *****************/

    /*! Create a task with name 'name' and set the state table size.  The
        execution of this task will rely on a callback from an external thread.

        \param name The name of the task
        \param sizeStateTable The history size of the state table

        \note See note in mtsTask regarding length of string name.

        \sa mtsTask, mtsTaskContinuous, mtsTaskPeriodic, mtsTaskFromSignal

     */
    mtsTaskFromCallback(const std::string & name,
                        unsigned int sizeStateTable = 256):
        mtsTask(name, sizeStateTable), inRunInternal(false) {}

    mtsTaskFromCallback(const mtsTaskConstructorArg & arg):
        mtsTask(arg.Name, arg.StateTableSize), inRunInternal(false) {}

    /*! Default Destructor. */
    virtual ~mtsTaskFromCallback() {}

    /********************* Methods to change task status *****************/
    /* (use Kill method from base class)                                 */

    /* Create a new thread (if needed). */
    void Create(void *data = 0);

    /*! Start/resume execution of the task */
    void Start(void);

    /*! Suspend the execution of the task */
    void Suspend(void);

    /*! End the task */
    void Kill(void);

    /*! Hook for calling RunInternal */
    virtual void *DoCallback(void *data = 0) {
        // The user Run() method should call SetThreadReturnValue.
        return RunInternal(data);
    }
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsTaskFromCallback)

/*!
  This class provides a callback adapter that can be used for
  systems that accept a user-supplied 'void *' that gets passed
  to the user-supplied callback. This enables the standard trick
  of passing the "this" pointer to the callback function. To be
  safe, we put the "this" pointer in a data structure that has
  a magic number (signature) that we can test. */

// PK: This class should be in a separate file, but I would like to
//     find a better name first.

class CISST_EXPORT mtsTaskFromCallbackAdapter : public mtsTaskFromCallback
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    typedef mtsTaskFromCallback BaseType;

    enum SignatureType {INVALID = 0, VALID = 0x3242 };
    struct CallbackData {
        SignatureType signature;
        mtsTaskFromCallback* obj;
        CallbackData() : signature(INVALID), obj(0) {}
        ~CallbackData() {}
    };
    CallbackData callbackData;

public:

    inline mtsTaskFromCallbackAdapter(const std::string & name,
                                      unsigned int sizeStateTable = 256):
        mtsTaskFromCallback(name, sizeStateTable),
        callbackData() {}

    inline mtsTaskFromCallbackAdapter(const mtsTaskConstructorArg &arg):
        mtsTaskFromCallback(arg),
        callbackData() {}

    ~mtsTaskFromCallbackAdapter() {}

    /*! This is a callback function that can be used with the external package,
        assuming that the package accepts a user-supplied 'void *' as the first
        parameter. This parameter must be obtained by calling GetCallbackParameter.

        \note The MTS_TASK_CALLBACK_CONVENTION is a hack to allow the calling
         convention to be specified (e.g., __stdcall) by defining this in
         the source code before including this header file.
         Perhaps there is a better way!
     */
    template <class _callbackReturnType>
    static _callbackReturnType MTS_TASK_CALLBACK_CONVENTION CallbackAdapter(void *obj);

    /*! This returns a callback parameter (as a 'void *') that can be supplied to
        an external package as the user data for the callback. The external package
        should pass this back as the first parameter to the callback function. */
    virtual void *GetCallbackParameter() {
        callbackData.signature = VALID;
        callbackData.obj = this;
        return static_cast<void*>(&callbackData);
    }

};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsTaskFromCallbackAdapter)

template <class _callbackReturnType>
_callbackReturnType MTS_TASK_CALLBACK_CONVENTION mtsTaskFromCallbackAdapter::CallbackAdapter(void *obj)
{
    if (!obj) {
        CMN_LOG_INIT_ERROR << "CallbackAdapter got null pointer" << std::endl;
        return _callbackReturnType();
    }
    CallbackData* _this = static_cast<CallbackData*>(obj);
    if (_this->signature != VALID) {
        CMN_LOG_INIT_ERROR << "CallbackAdapter received invalid signature: " << _this->signature << std::endl;
        return _callbackReturnType();
    }
    void *ret =_this->obj->DoCallback(0);
    // Following won't compile if _callbackReturnType is 'void'.
    // TBD:  workaround using a type traits approach.
    return *static_cast<_callbackReturnType *>(ret);
}


#endif // _mtsTaskFromCallback_h
