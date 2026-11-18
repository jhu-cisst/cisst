/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2026-11-17

  (C) Copyright 2010-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsCallable_h
#define _mtsCallable_h

#include <functional>
#include <type_traits>

#include <cisstMultiTask/mtsExecutionResult.h>
#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>

/*!
  \file
  \brief Type-erased callable types and C++17 marshaling helpers for cisstMultiTask.
*/

using mtsCallableVoid          = std::function<mtsExecutionResult(void)>;
using mtsCallableVoidReturn    = std::function<mtsExecutionResult(mtsGenericObject &)>;
using mtsCallableRead          = std::function<mtsExecutionResult(mtsGenericObject &)>;
using mtsCallableWriteReturn   = std::function<mtsExecutionResult(const mtsGenericObject &, mtsGenericObject &)>;
using mtsCallableQualifiedRead = std::function<mtsExecutionResult(const mtsGenericObject &, mtsGenericObject &)>;

namespace mtsCallableDetail {

template <typename _type>
struct ConstArgAdapter {
    const _type * Ptr = nullptr;
    std::conditional_t<std::is_same_v<_type, mtsGenericObject>, int, _type> Temp;

    bool Bind(const mtsGenericObject & obj) {
        if constexpr (std::is_same_v<_type, mtsGenericObject>) {
            Ptr = &obj;
            return true;
        } else if constexpr (cmnIsDerivedFromTemplated<_type, mtsGenericObjectProxy>::IS_DERIVED) {
            Ptr = dynamic_cast<const _type *>(&obj);
            if (Ptr) {
                return true;
            }
            using RefType = typename _type::RefType;
            const RefType * ref = dynamic_cast<const RefType *>(&obj);
            if (!ref) {
                return false;
            }
            Temp = *ref;
            Ptr = &Temp;
            return true;
        } else {
            Ptr = mtsGenericTypes<_type>::CastArg(obj);
            return Ptr != nullptr;
        }
    }
};

template <typename _type>
struct MutableArgAdapter {
    _type * Ptr = nullptr;
    std::conditional_t<std::is_same_v<_type, mtsGenericObject>, int, _type> Temp;
    mtsGenericObject * RawRef = nullptr;

    bool Bind(mtsGenericObject & obj) {
        if constexpr (std::is_same_v<_type, mtsGenericObject>) {
            Ptr = &obj;
            return true;
        } else if constexpr (cmnIsDerivedFromTemplated<_type, mtsGenericObjectProxy>::IS_DERIVED) {
            Ptr = dynamic_cast<_type *>(&obj);
            if (Ptr) {
                return true;
            }
            using RefType = typename _type::RefType;
            RefType * ref = dynamic_cast<RefType *>(&obj);
            if (!ref) {
                return false;
            }
            RawRef = &obj;
            Ptr = &Temp;
            return true;
        } else {
            Ptr = mtsGenericTypes<_type>::CastArg(obj);
            return Ptr != nullptr;
        }
    }

    void Commit(void) {
        if constexpr (cmnIsDerivedFromTemplated<_type, mtsGenericObjectProxy>::IS_DERIVED) {
            if (RawRef) {
                using RefType = typename _type::RefType;
                *static_cast<RefType *>(RawRef) = Temp;
            }
        }
    }
};

} // namespace mtsCallableDetail

template <class _classType>
inline mtsCallableVoid mtsMakeCallableVoid(void (_classType::*method)(void),
                                           _classType * classInstantiation)
{
    return [method, classInstantiation](void) -> mtsExecutionResult {
        (classInstantiation->*method)();
        return mtsExecutionResult::COMMAND_SUCCEEDED;
    };
}

inline mtsCallableVoid mtsMakeCallableVoid(void (*function)(void))
{
    return [function](void) -> mtsExecutionResult {
        (*function)();
        return mtsExecutionResult::COMMAND_SUCCEEDED;
    };
}

template <class _classType, class _resultType>
inline mtsCallableVoidReturn mtsMakeCallableVoidReturn(void (_classType::*method)(_resultType &),
                                                       _classType * classInstantiation)
{
    return [method, classInstantiation](mtsGenericObject & result) -> mtsExecutionResult {
        mtsCallableDetail::MutableArgAdapter<_resultType> adapter;
        if (!adapter.Bind(result)) {
            return mtsExecutionResult::INVALID_INPUT_TYPE;
        }
        (classInstantiation->*method)(*adapter.Ptr);
        adapter.Commit();
        return mtsExecutionResult::COMMAND_SUCCEEDED;
    };
}

template <class _classType, class _resultType>
inline mtsCallableRead mtsMakeCallableRead(bool (_classType::*method)(_resultType &) const,
                                           _classType * classInstantiation)
{
    return [method, classInstantiation](mtsGenericObject & result) -> mtsExecutionResult {
        mtsCallableDetail::MutableArgAdapter<_resultType> adapter;
        if (!adapter.Bind(result)) {
            return mtsExecutionResult::INVALID_INPUT_TYPE;
        }
        const bool ok = (classInstantiation->*method)(*adapter.Ptr);
        adapter.Commit();
        return ok ? mtsExecutionResult::COMMAND_SUCCEEDED : mtsExecutionResult::METHOD_OR_FUNCTION_FAILED;
    };
}

template <class _classType, class _resultType>
inline mtsCallableRead mtsMakeCallableRead(void (_classType::*method)(_resultType &) const,
                                           _classType * classInstantiation)
{
    return [method, classInstantiation](mtsGenericObject & result) -> mtsExecutionResult {
        mtsCallableDetail::MutableArgAdapter<_resultType> adapter;
        if (!adapter.Bind(result)) {
            return mtsExecutionResult::INVALID_INPUT_TYPE;
        }
        (classInstantiation->*method)(*adapter.Ptr);
        adapter.Commit();
        return mtsExecutionResult::COMMAND_SUCCEEDED;
    };
}

template <class _classType, class _argumentType, class _resultType>
inline mtsCallableWriteReturn mtsMakeCallableWriteReturn(void (_classType::*method)(const _argumentType &, _resultType &),
                                                         _classType * classInstantiation)
{
    return [method, classInstantiation](const mtsGenericObject & argument, mtsGenericObject & result) -> mtsExecutionResult {
        mtsCallableDetail::ConstArgAdapter<_argumentType> argAdapter;
        if (!argAdapter.Bind(argument)) {
            return mtsExecutionResult::INVALID_INPUT_TYPE;
        }
        mtsCallableDetail::MutableArgAdapter<_resultType> resAdapter;
        if (!resAdapter.Bind(result)) {
            return mtsExecutionResult::INVALID_INPUT_TYPE;
        }
        (classInstantiation->*method)(*argAdapter.Ptr, *resAdapter.Ptr);
        resAdapter.Commit();
        return mtsExecutionResult::COMMAND_SUCCEEDED;
    };
}

template <class _classType, class _argumentType, class _resultType>
inline mtsCallableQualifiedRead mtsMakeCallableQualifiedRead(bool (_classType::*method)(const _argumentType &, _resultType &) const,
                                                             _classType * classInstantiation)
{
    return [method, classInstantiation](const mtsGenericObject & argument, mtsGenericObject & result) -> mtsExecutionResult {
        mtsCallableDetail::ConstArgAdapter<_argumentType> argAdapter;
        if (!argAdapter.Bind(argument)) {
            return mtsExecutionResult::INVALID_INPUT_TYPE;
        }
        mtsCallableDetail::MutableArgAdapter<_resultType> resAdapter;
        if (!resAdapter.Bind(result)) {
            return mtsExecutionResult::INVALID_INPUT_TYPE;
        }
        const bool ok = (classInstantiation->*method)(*argAdapter.Ptr, *resAdapter.Ptr);
        resAdapter.Commit();
        return ok ? mtsExecutionResult::COMMAND_SUCCEEDED : mtsExecutionResult::METHOD_OR_FUNCTION_FAILED;
    };
}

template <class _classType, class _argumentType, class _resultType>
inline mtsCallableQualifiedRead mtsMakeCallableQualifiedRead(void (_classType::*method)(const _argumentType &, _resultType &) const,
                                                             _classType * classInstantiation)
{
    return [method, classInstantiation](const mtsGenericObject & argument, mtsGenericObject & result) -> mtsExecutionResult {
        mtsCallableDetail::ConstArgAdapter<_argumentType> argAdapter;
        if (!argAdapter.Bind(argument)) {
            return mtsExecutionResult::INVALID_INPUT_TYPE;
        }
        mtsCallableDetail::MutableArgAdapter<_resultType> resAdapter;
        if (!resAdapter.Bind(result)) {
            return mtsExecutionResult::INVALID_INPUT_TYPE;
        }
        (classInstantiation->*method)(*argAdapter.Ptr, *resAdapter.Ptr);
        resAdapter.Commit();
        return mtsExecutionResult::COMMAND_SUCCEEDED;
    };
}

#endif // _mtsCallable_h
