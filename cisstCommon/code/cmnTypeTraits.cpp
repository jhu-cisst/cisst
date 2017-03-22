/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2004-01-23

  (C) Copyright 2004-2016 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnTypeTraits.h>


// ---------- float ----------
static float zeroFloat = 0.0f;
template<>
float & cmnTypeTraits<float>::ToleranceValue(void)
{
    static float tolerance = 1.0e-5f;
    return tolerance;
}

template<>
const float cmnTypeTraits<float>::DefaultTolerance = cmnTypeTraits<float>::ToleranceValue();

template<>
std::string cmnTypeTraits<float>::TypeName(void)
{
    return "float";
}

template<>
float cmnTypeTraits<float>::PlusInfinity(void)
{
    static const float inf = 1.0f / zeroFloat;
    return inf;
}

template<>
float cmnTypeTraits<float>::MinusInfinity(void)
{
    static const float inf = -1.0f / zeroFloat;
    return inf;
}

template<>
float cmnTypeTraits<float>::NaN(void)
{
    static const float nan = zeroFloat / zeroFloat;
    return nan;
}



// ---------- double ----------
static double zeroDouble = 0.0;
template<>
double & cmnTypeTraits<double>::ToleranceValue(void)
{
    static double tolerance = 1.0e-9;
    return tolerance;
}

template<>
const double cmnTypeTraits<double>::DefaultTolerance = cmnTypeTraits<double>::ToleranceValue();

template<>
std::string cmnTypeTraits<double>::TypeName(void)
{
    return "double";
}

template<>
double cmnTypeTraits<double>::PlusInfinity(void)
{
    static const double inf = 1.0 / zeroDouble;
    return inf;
}

template<>
double cmnTypeTraits<double>::MinusInfinity(void)
{
    static const double inf = -1.0 / zeroDouble;
    return inf;
}

template<>
double cmnTypeTraits<double>::NaN(void)
{
    static const double nan = zeroDouble / zeroDouble;
    return nan;
}


// ---------- long int ----------
template<>
long int & cmnTypeTraits<long int>::ToleranceValue(void)
{
    static long int tolerance = 0;
    return tolerance;
}

template<>
const long int cmnTypeTraits<long int>::DefaultTolerance = cmnTypeTraits<long int>::ToleranceValue();

template<>
std::string cmnTypeTraits<long int>::TypeName(void)
{
    return "long";
}

template<>
long int cmnTypeTraits<long int>::PlusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
long int cmnTypeTraits<long int>::MinusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
long int cmnTypeTraits<long int>::NaN(void)
{
    // Not meaningful!  Here to avoid link errors
    return 0;
}

// ---------- int ----------
template<>
int & cmnTypeTraits<int>::ToleranceValue(void)
{
    static int tolerance = 0;
    return tolerance;
}

template<>
const int cmnTypeTraits<int>::DefaultTolerance = cmnTypeTraits<int>::ToleranceValue();

template<>
std::string cmnTypeTraits<int>::TypeName(void)
{
    return "int";
}

template<>
int cmnTypeTraits<int>::PlusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
int cmnTypeTraits<int>::MinusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
int cmnTypeTraits<int>::NaN(void)
{
    // Not meaningful!  Here to avoid link errors
    return 0;
}


// ---------- char ----------
template<>
char & cmnTypeTraits<char>::ToleranceValue(void)
{
    static char tolerance = 0;
    return tolerance;
}

template<>
const char cmnTypeTraits<char>::DefaultTolerance = cmnTypeTraits<char>::ToleranceValue();

template<>
std::string cmnTypeTraits<char>::TypeName(void)
{
    return "char";
}

template<>
char cmnTypeTraits<char>::PlusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
char cmnTypeTraits<char>::MinusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
char cmnTypeTraits<char>::NaN(void)
{
    // Not meaningful!  Here to avoid link errors
    return 0;
}


// ---------- short ----------
template<>
short & cmnTypeTraits<short>::ToleranceValue(void)
{
    static short tolerance = 0;
    return tolerance;
}

template<>
const short cmnTypeTraits<short>::DefaultTolerance = cmnTypeTraits<short>::ToleranceValue();

template<>
std::string cmnTypeTraits<short>::TypeName(void)
{
    return "short";
}

template<>
short cmnTypeTraits<short>::PlusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
short cmnTypeTraits<short>::MinusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
short cmnTypeTraits<short>::NaN(void)
{
    // Not meaningful!  Here to avoid link errors
    return 0;
}


// ---------- long long ----------
template<>
long long & cmnTypeTraits<long long>::ToleranceValue(void)
{
    static long long tolerance = 0;
    return tolerance;
}

template<>
const long long cmnTypeTraits<long long>::DefaultTolerance = cmnTypeTraits<long long>::ToleranceValue();

template<>
std::string cmnTypeTraits<long long>::TypeName(void)
{
    return "long long";
}

template<>
long long cmnTypeTraits<long long>::PlusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
long long cmnTypeTraits<long long>::MinusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
long long cmnTypeTraits<long long>::NaN(void)
{
    // Not meaningful!  Here to avoid link errors
    return 0;
}

// ---------- unsigned long long ----------
template<>
unsigned long long & cmnTypeTraits<unsigned long long>::ToleranceValue(void)
{
    static unsigned long long tolerance = 0;
    return tolerance;
}

template<>
const unsigned long long cmnTypeTraits<unsigned long long>::DefaultTolerance = cmnTypeTraits<unsigned long long>::ToleranceValue();

template<>
std::string cmnTypeTraits<unsigned long long>::TypeName(void)
{
    return "unsigned long long";
}

template<>
unsigned long long cmnTypeTraits<unsigned long long>::PlusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
unsigned long long cmnTypeTraits<unsigned long long>::MinusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
unsigned long long cmnTypeTraits<unsigned long long>::NaN(void)
{
    // Not meaningful!  Here to avoid link errors
    return 0;
}

// ---------- unsigned long int ----------
template<>
unsigned long int & cmnTypeTraits<unsigned long int>::ToleranceValue(void)
{
    static unsigned long int tolerance = 0;
    return tolerance;
}

template<>
const unsigned long int cmnTypeTraits<unsigned long int>::DefaultTolerance = cmnTypeTraits<unsigned long int>::ToleranceValue();

template<>
std::string cmnTypeTraits<unsigned long int>::TypeName(void)
{
    return "unsigned long";
}

template<>
unsigned long int cmnTypeTraits<unsigned long int>::PlusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
unsigned long int cmnTypeTraits<unsigned long int>::MinusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
unsigned long int cmnTypeTraits<unsigned long int>::NaN(void)
{
    // Not meaningful!  Here to avoid link errors
    return 0;
}

// ---------- unsigned int ----------
template<>
unsigned int & cmnTypeTraits<unsigned int>::ToleranceValue(void)
{
    static unsigned int tolerance = 0;
    return tolerance;
}

template<>
const unsigned int cmnTypeTraits<unsigned int>::DefaultTolerance = cmnTypeTraits<unsigned int>::ToleranceValue();

template<>
std::string cmnTypeTraits<unsigned int>::TypeName(void)
{
    return "unsigned int";
}

template<>
unsigned int cmnTypeTraits<unsigned int>::PlusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
unsigned int cmnTypeTraits<unsigned int>::MinusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
unsigned int cmnTypeTraits<unsigned int>::NaN(void)
{
    // Not meaningful!  Here to avoid link errors
    return 0;
}

// ---------- unsigned short ----------
template<>
unsigned short & cmnTypeTraits<unsigned short>::ToleranceValue(void)
{
    static unsigned short tolerance = 0;
    return tolerance;
}

template<>
const unsigned short cmnTypeTraits<unsigned short>::DefaultTolerance = cmnTypeTraits<unsigned short>::ToleranceValue();

template<>
std::string cmnTypeTraits<unsigned short>::TypeName(void)
{
    return "unsigned short";
}

template<>
unsigned short cmnTypeTraits<unsigned short>::PlusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
unsigned short cmnTypeTraits<unsigned short>::MinusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
unsigned short cmnTypeTraits<unsigned short>::NaN(void)
{
    // Not meaningful!  Here to avoid link errors
    return 0;
}

// ---------- unsigned char ----------
template<>
unsigned char & cmnTypeTraits<unsigned char>::ToleranceValue(void)
{
    static unsigned char tolerance = 0;
    return tolerance;
}

template<>
const unsigned char cmnTypeTraits<unsigned char>::DefaultTolerance = cmnTypeTraits<unsigned char>::ToleranceValue();

template<>
std::string cmnTypeTraits<unsigned char>::TypeName(void)
{
    return "unsigned char";
}

template<>
unsigned char cmnTypeTraits<unsigned char>::PlusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
unsigned char cmnTypeTraits<unsigned char>::MinusInfinity(void)
{
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
unsigned char cmnTypeTraits<unsigned char>::NaN(void)
{
    // Not meaningful!  Here to avoid link errors
    return 0;
}


// ---------- bool ----------
template<>
bool & cmnTypeTraits<bool>::ToleranceValue(void)
{
    static bool tolerance = 0;
    return tolerance;
}

template<>
const bool cmnTypeTraits<bool>::DefaultTolerance = cmnTypeTraits<bool>::ToleranceValue();

template<>
std::string cmnTypeTraits<bool>::TypeName(void)
{
    return "bool";
}

