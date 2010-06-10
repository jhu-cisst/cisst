/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2004-01-23

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

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
float & cmnTypeTraits<float>::ToleranceValue()
{
    static float tolerance = 1.0e-5f;
    return tolerance;
}

template<>
const float cmnTypeTraits<float>::DefaultTolerance = cmnTypeTraits<float>::ToleranceValue();

template<>
std::string cmnTypeTraits<float>::TypeName()
{
    return "float";
}

template<>
float cmnTypeTraits<float>::PlusInfinity()
{
    static const float inf = 1.0f / zeroFloat;
    return inf;
}

template<>
float cmnTypeTraits<float>::MinusInfinity()
{
    static const float inf = -1.0f / zeroFloat;
    return inf;
}

template<>
float cmnTypeTraits<float>::NaN()
{
    static const float nan = zeroFloat / zeroFloat;
    return nan;
}



// ---------- double ----------
static double zeroDouble = 0.0;
template<>
double & cmnTypeTraits<double>::ToleranceValue()
{
    static double tolerance = 1.0e-9;
    return tolerance;
}

template<>
const double cmnTypeTraits<double>::DefaultTolerance = cmnTypeTraits<double>::ToleranceValue();

template<>
std::string cmnTypeTraits<double>::TypeName()
{
    return "double";
}

template<>
double cmnTypeTraits<double>::PlusInfinity()
{
    static const double inf = 1.0 / zeroDouble;
    return inf;
}

template<>
double cmnTypeTraits<double>::MinusInfinity()
{
    static const double inf = -1.0 / zeroDouble;
    return inf;
}

template<>
double cmnTypeTraits<double>::NaN()
{
    static const double nan = zeroDouble / zeroDouble;
    return nan;
}


// ---------- int ----------
template<>
int & cmnTypeTraits<int>::ToleranceValue()
{
    static int tolerance = 0;
    return tolerance;
}

template<>
const int cmnTypeTraits<int>::DefaultTolerance = cmnTypeTraits<int>::ToleranceValue();

template<>
std::string cmnTypeTraits<int>::TypeName()
{
    return "int";
}

template<>
int cmnTypeTraits<int>::PlusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
int cmnTypeTraits<int>::MinusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
int cmnTypeTraits<int>::NaN() {
    // Not meaningful!  Here to avoid link errors
    return 0;
}


// ---------- char ----------
template<>
char & cmnTypeTraits<char>::ToleranceValue()
{
    static char tolerance = 0;
    return tolerance;
}

template<>
const char cmnTypeTraits<char>::DefaultTolerance = cmnTypeTraits<char>::ToleranceValue();

template<>
std::string cmnTypeTraits<char>::TypeName()
{
    return "char";
}

template<>
char cmnTypeTraits<char>::PlusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
char cmnTypeTraits<char>::MinusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
char cmnTypeTraits<char>::NaN() {
    // Not meaningful!  Here to avoid link errors
    return 0;
}


// ---------- short ----------
template<>
short & cmnTypeTraits<short>::ToleranceValue()
{
    static short tolerance = 0;
    return tolerance;
}

template<>
const short cmnTypeTraits<short>::DefaultTolerance = cmnTypeTraits<short>::ToleranceValue();

template<>
std::string cmnTypeTraits<short>::TypeName()
{
    return "short";
}

template<>
short cmnTypeTraits<short>::PlusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
short cmnTypeTraits<short>::MinusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
short cmnTypeTraits<short>::NaN() {
    // Not meaningful!  Here to avoid link errors
    return 0;
}


// ---------- long long ----------
template<>
long long & cmnTypeTraits<long long>::ToleranceValue()
{
    static long long tolerance = 0;
    return tolerance;
}

template<>
const long long cmnTypeTraits<long long>::DefaultTolerance = cmnTypeTraits<long long>::ToleranceValue();

template<>
std::string cmnTypeTraits<long long>::TypeName()
{
    return "long long";
}

template<>
long long cmnTypeTraits<long long>::PlusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
long long cmnTypeTraits<long long>::MinusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
long long cmnTypeTraits<long long>::NaN() {
    // Not meaningful!  Here to avoid link errors
    return 0;
}

// ---------- unsigned long long ----------
template<>
unsigned long long & cmnTypeTraits<unsigned long long>::ToleranceValue()
{
    static unsigned long long tolerance = 0;
    return tolerance;
}

template<>
const unsigned long long cmnTypeTraits<unsigned long long>::DefaultTolerance = cmnTypeTraits<unsigned long long>::ToleranceValue();

template<>
std::string cmnTypeTraits<unsigned long long>::TypeName()
{
    return "unsigned long long";
}

template<>
unsigned long long cmnTypeTraits<unsigned long long>::PlusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
unsigned long long cmnTypeTraits<unsigned long long>::MinusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
unsigned long long cmnTypeTraits<unsigned long long>::NaN() {
    // Not meaningful!  Here to avoid link errors
    return 0;
}

// ---------- unsigned int ----------
template<>
unsigned int & cmnTypeTraits<unsigned int>::ToleranceValue()
{
    static unsigned int tolerance = 0;
    return tolerance;
}

template<>
const unsigned int cmnTypeTraits<unsigned int>::DefaultTolerance = cmnTypeTraits<unsigned int>::ToleranceValue();

template<>
std::string cmnTypeTraits<unsigned int>::TypeName()
{
    return "unsigned int";
}

template<>
unsigned int cmnTypeTraits<unsigned int>::PlusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
unsigned int cmnTypeTraits<unsigned int>::MinusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
unsigned int cmnTypeTraits<unsigned int>::NaN() {
    // Not meaningful!  Here to avoid link errors
    return 0;
}

// ---------- unsigned short ----------
template<>
unsigned short & cmnTypeTraits<unsigned short>::ToleranceValue()
{
    static unsigned short tolerance = 0;
    return tolerance;
}

template<>
const unsigned short cmnTypeTraits<unsigned short>::DefaultTolerance = cmnTypeTraits<unsigned short>::ToleranceValue();

template<>
std::string cmnTypeTraits<unsigned short>::TypeName()
{
    return "unsigned short";
}

template<>
unsigned short cmnTypeTraits<unsigned short>::PlusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
unsigned short cmnTypeTraits<unsigned short>::MinusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
unsigned short cmnTypeTraits<unsigned short>::NaN() {
    // Not meaningful!  Here to avoid link errors
    return 0;
}

// ---------- unsigned char ----------
template<>
unsigned char & cmnTypeTraits<unsigned char>::ToleranceValue()
{
    static unsigned char tolerance = 0;
    return tolerance;
}

template<>
const unsigned char cmnTypeTraits<unsigned char>::DefaultTolerance = cmnTypeTraits<unsigned char>::ToleranceValue();

template<>
std::string cmnTypeTraits<unsigned char>::TypeName()
{
    return "unsigned char";
}

template<>
unsigned char cmnTypeTraits<unsigned char>::PlusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
unsigned char cmnTypeTraits<unsigned char>::MinusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
unsigned char cmnTypeTraits<unsigned char>::NaN() {
    // Not meaningful!  Here to avoid link errors
    return 0;
}


// ---------- bool ----------
template<>
bool & cmnTypeTraits<bool>::ToleranceValue()
{
    static bool tolerance = 0;
    return tolerance;
}

template<>
const bool cmnTypeTraits<bool>::DefaultTolerance = cmnTypeTraits<bool>::ToleranceValue();

template<>
std::string cmnTypeTraits<bool>::TypeName()
{
    return "bool";
}

