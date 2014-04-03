/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlTypeCheckers_h
#define _svlTypeCheckers_h


// char
template <class __ValueType>
static bool IsTypeInt8(__ValueType CMN_UNUSED(val)) { return false; }
template <>
inline bool IsTypeInt8<char>(char CMN_UNUSED(val)) { return true; }

// short
template <class __ValueType>
static bool IsTypeInt16(__ValueType CMN_UNUSED(val)) { return false; }
template <>
inline bool IsTypeInt16<short>(short CMN_UNUSED(val)) { return true; }

// int
template <class __ValueType>
static bool IsTypeInt32(__ValueType CMN_UNUSED(val)) { return false; }
template <>
inline bool IsTypeInt32<int>(int CMN_UNUSED(val)) { return true; }

// long long int
template <class __ValueType>
static bool IsTypeInt64(__ValueType CMN_UNUSED(val)) { return false; }
template <>
inline bool IsTypeInt64<long long int>(long long int CMN_UNUSED(val)) { return true; }

// unsigned char
template <class __ValueType>
static bool IsTypeUInt8(__ValueType CMN_UNUSED(val)) { return false; }
template <>
inline bool IsTypeUInt8<unsigned char>(unsigned char CMN_UNUSED(val)) { return true; }

// unsigned short
template <class __ValueType>
static bool IsTypeUInt16(__ValueType CMN_UNUSED(val)) { return false; }
template <>
inline bool IsTypeUInt16<unsigned short>(unsigned short CMN_UNUSED(val)) { return true; }

// unsigned int
template <class __ValueType>
static bool IsTypeUInt32(__ValueType CMN_UNUSED(val)) { return false; }
template <>
inline bool IsTypeUInt32<unsigned int>(unsigned int CMN_UNUSED(val)) { return true; }

// unsigned long long int
template <class __ValueType>
static bool IsTypeUInt64(__ValueType CMN_UNUSED(val)) { return false; }
template <>
inline bool IsTypeUInt64<unsigned long long int>(unsigned long long int CMN_UNUSED(val)) { return true; }

// float
template <class __ValueType>
static bool IsTypeFloat(__ValueType CMN_UNUSED(val)) { return false; }
template <>
inline bool IsTypeFloat<float>(float CMN_UNUSED(val)) { return true; }

// double
template <class __ValueType>
static bool IsTypeDouble(__ValueType CMN_UNUSED(val)) { return false; }
template <>
inline bool IsTypeDouble<double>(double CMN_UNUSED(val)) { return true; }


#define MAX_INT8    127
#define MIN_INT8    -128
#define MAX_INT16   32767
#define MIN_INT16   -32768
#define MAX_INT32   2147483647
#define MIN_INT32   -2147483647
#define MAX_UINT8   255U
#define MIN_UINT8   0U
#define MAX_UINT16  65535U
#define MIN_UINT16  0U
#define MAX_UINT32  4294967295U
#define MIN_UINT32  0U

template <class __ValueType>
static bool GetMaxValue(long long int& max) { max = 0; return false; }
template <>
inline bool GetMaxValue<char          >(long long int& max) { max = MAX_INT8;   return true; }
template <>
inline bool GetMaxValue<short         >(long long int& max) { max = MAX_INT16;  return true; }
template <>
inline bool GetMaxValue<int           >(long long int& max) { max = MAX_INT32;  return true; }
template <>
inline bool GetMaxValue<unsigned char >(long long int& max) { max = MAX_UINT8;  return true; }
template <>
inline bool GetMaxValue<unsigned short>(long long int& max) { max = MAX_UINT16; return true; }
template <>
inline bool GetMaxValue<unsigned int  >(long long int& max) { max = MAX_UINT32; return true; }

template <class __ValueType>
static bool GetMinValue(long long int& min) { min = 0; return false; }
template <>
inline bool GetMinValue<char          >(long long int& min) { min = MIN_INT8;   return true; }
template <>
inline bool GetMinValue<short         >(long long int& min) { min = MIN_INT16;  return true; }
template <>
inline bool GetMinValue<int           >(long long int& min) { min = MIN_INT32;  return true; }
template <>
inline bool GetMinValue<unsigned char >(long long int& min) { min = MIN_UINT8;  return true; }
template <>
inline bool GetMinValue<unsigned short>(long long int& min) { min = MIN_UINT16; return true; }
template <>
inline bool GetMinValue<unsigned int  >(long long int& min) { min = MIN_UINT32; return true; }


#endif // _svlTypeCheckers_h

