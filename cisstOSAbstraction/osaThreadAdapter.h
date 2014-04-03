/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s): Ankur Kapoor, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines a thread adapter 
  \ingroup cisstOSAbstraction
*/

#ifndef _osaThreadAdapter_h
#define _osaThreadAdapter_h

#include <cisstCommon/cmnPortability.h>

/*!
  \ingroup cisstOSAbstraction

  This struct is used as base class for all callback adapters.  It
  just declares and initializes the adapter's member data, so we don't
  have to do this separately for each single adapter.
*/
template<class _objectType, class _callBackMethodType, class _userDataType>
struct osaCallBackBase {
    /*! Typedef for osaCallBackBase */
    typedef osaCallBackBase osaCallBackBaseType;

    /*! Constructor. Stores the pointer to member function of object
      class _objectType and the user object of class _userDataType.

     \param obj The pointer to the object containing the member
     function to be called.

     \param callBackFunction The pointer to the member function to be
     called.

     \param userData The user data to be passed to the callback member
     function.
    */
    osaCallBackBase(_objectType* obj, _callBackMethodType callBackFunction,
                    _userDataType userData)
        : Obj(obj), CallBackFunction(callBackFunction), UserData(userData) {}


    /*! The pointer to the object containing the member function to be called. */
    _objectType* Obj;

    /*! The pointer to the member function to be called. */
    _callBackMethodType CallBackFunction;

    /*! A copy of the user data to be passed to the callback member function. */
    _userDataType UserData;
};


/*!
  \ingroup cisstOSAbstraction

  Adapter for callback functions with one user defined argument and
  adapter instance created on the heap. The adapter instance is
  automatically destroyed with delete after processing the
  callback. This is used for CreateThread() R MyCallback( A1 arg1 ) is
  redirected to [virtual] R O::MyMemberCallback( U UserData );
  PKAZ: Clean up the above comment.
 */
template<class _objectType, class _userDataType,
         class _callBackReturnType, class _callBackArgumentType >
struct osaHeapCallBack: public osaCallBackBase<_objectType, _callBackReturnType(_objectType::*)(_userDataType), _userDataType> {

    /*! Creates an object that can be passed as argument of the 'start routine'. */
    static osaHeapCallBack* Create(_objectType* obj,
                                   _callBackReturnType (_objectType::*callBackFunction)(_userDataType),
                                   _userDataType userData) {
        return new osaHeapCallBack(obj, callBackFunction, userData);
    }

    typedef osaCallBackBase<_objectType, _callBackReturnType(_objectType::*)(_userDataType), _userDataType> BaseType;

    /*! The static function to be passed as the 'start routine'
      argument of functions such as pthread_create. We use the
      argument to the 'start routine' to send an object that packs the
      member function to be called, along with the receiver object and
      user data.
    
      \param obj This object must be of type _callBackArgumentType,
      which is the same as that of the 'start routine' prototype defined
      by the platform.
g
      \returns The result of the callback method which must be of type
      _callBackReturnType type, which is the same as the return type of
      the 'start routine' defined by the platform. */
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
    static void * CallbackAndDestroy(_callBackArgumentType obj) {
        osaHeapCallBack* _this = (osaHeapCallBack*) obj;
        _callBackReturnType result = (_this->Obj->*_this->CallBackFunction)(_this->UserData);
        delete _this;
        return (void*)result;
    }
#elif (CISST_OS == CISST_WINDOWS)
    // To avoid including <windows.h>, use the unsigned long and __stdcall rather than
    //    DWORD and WINAPI, respectively. This is slightly risky, since <windows.h> could
    //    change, but that is not likely.
    static unsigned long __stdcall CallbackAndDestroy(_callBackArgumentType obj) {
        osaHeapCallBack* _this = (osaHeapCallBack*) obj;
        _callBackReturnType result = (_this->Obj->*_this->CallBackFunction)(_this->UserData );
        delete _this;
#if (CISST_COMPILER == CISST_GCC)
        return (unsigned long)(unsigned long long)result;  // For MingW64
#else
        return (unsigned long)result;
#endif
    }
#endif


protected:
    /*! Constructor is protected to prevent instances on the stack. */
    osaHeapCallBack( _objectType* obj,
                     _callBackReturnType (_objectType::*callBackFunction)(_userDataType),
                     _userDataType userData )
        : BaseType(obj, callBackFunction , userData) {}
};


#endif // _osaThreadAdapter_h

