/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2008-08-21

  (C) Copyright 2008-2015 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Declaration of units and unit conversion methods
  \ingroup cisstCommon

  This file include the definition and implementation of constants and
  global functions used to defines measuring units in the cisst
  libraries.  The internal units of the cisst libraries are meters,
  kilograms, seconds and radians if CISST_USE_SI_UNITS is set to true.
  Otherwise, units are millimeters and grams.  Older versions of cisst
  didn't define CISST_USE_SI_UNITS and the default units were
  millimeters and grams.

  For all units, the cisst libraries provide a constant as well as a
  global function to convert from the internal units to other
  representations.

  <code>
    double distance = 5000 * cmn_m; // 5000 meters
    AnExampleOfFunction(50 * cmn_mm); // 50 millimeters
    std::cout << "distance in cm: " << cmnInternalTo_cm(distance) << std::endl;
  </code>

  \note The use of underscore in symbol names is an exception to the
  cisst naming conventions.
*/
#pragma once

#ifndef _cmnUnits_h
#define _cmnUnits_h

// To determine if we are using SI (m) or millimeters by default
#include <cisstConfig.h>

// Always include last
#include <cisstCommon/cmnExport.h>



/*! \name Length units.  The internal unit is millimeter if
  CISST_USE_SI_UNITS is set to false, meters otherwise. */
//@{

/*! Millimeter constant, use to convert a length in millimeters to
  internal units.  */
#if CISST_USE_SI_UNITS
const double cmn_mm = 0.001;
#else
const double cmn_mm = 1.0;
#endif

/*! Convert a length in internal units to millimeters */
inline double cmnInternalTo_mm(double valueInternalUnits) {
    return valueInternalUnits / cmn_mm;
}

/*! Micrometer constant, use to convert a length in micrometers to
  internal units.  */
const double cmn_um = cmn_mm / 1000.0;

/*! Convert a length in internal units to micrometers */
inline double cmnInternalTo_um(double valueInternalUnits) {
    return valueInternalUnits / cmn_um;
}

/*! Centimeter constant, use to convert a length in centimeters to
  internal units.  */
const double cmn_cm = 10.0 * cmn_mm;

/*! Convert a length in internal units to centimeters */
inline double cmnInternalTo_cm(double valueInternalUnits) {
    return valueInternalUnits / cmn_cm;
}

/*! Meter constant, use to convert a length in meters to
  internal units.  */
#if CISST_USE_SI_UNITS
const double cmn_m = 1.0;
#else
const double cmn_m = 1000.0;
#endif

/*! Convert a length in internal units to meters */
inline double cmnInternalTo_m(double valueInternalUnits) {
    return valueInternalUnits / cmn_m;
}

/*! Kilometer constant, use to convert a length in kilometers to
  internal units.  */
const double cmn_km = 1000.0 * cmn_m;

/*! Convert a length in internal units to kilometers */
inline double cmnInternalTo_km(double valueInternalUnits) {
    return valueInternalUnits / cmn_km;
}
//@}



/*! \name Mass units.  The internal unit is grams if
  CISST_USE_SI_UNITS is set to false, kilograms otherwise. */
//@{

/*! Gram constant, use to convert a mass in grams to
  internal units.  */
#if CISST_USE_SI_UNITS
const double cmn_g = 0.001;
#else
const double cmn_g = 1.0;
#endif

/*! Convert a mass in internal units to grams */
inline double cmnInternalTo_g(double valueInternalUnits) {
    return valueInternalUnits / cmn_g;
}

/*! Milligrams constant, use to convert a mass in milligrams to
  internal units.  */
const double cmn_mg = cmn_g / 1000.0;

/*! Convert a mass in internal units to milligrams */
inline double cmnInternalTo_mg(double valueInternalUnits) {
    return valueInternalUnits / cmn_mg;
}

/*! Microgram constant, use to convert a mass in micrograms to
  internal units.  */
const double cmn_ug = cmn_mg / 1000.0;

/*! Convert a mass in internal units to micrograms */
inline double cmnInternalTo_ug(double valueInternalUnits) {
    return valueInternalUnits / cmn_ug;
}

/*! Centigram constant, use to convert a mass in centigrams to
  internal units.  */
const double cmn_cg = cmn_g / 100.0;

/*! Convert a mass in internal units to centigrams */
inline double cmnInternalTo_cg(double valueInternalUnits) {
    return valueInternalUnits / cmn_cg;
}

/*! Kilogram constant, use to convert a mass in kilograms to
  internal units.  */
const double cmn_kg = 1000.0 * cmn_g;

/*! Convert a mass in internal units to kilograms */
inline double cmnInternalTo_kg(double valueInternalUnits) {
    return valueInternalUnits / cmn_kg;
}
//@}



/*! \name Time units.  The internal unit is seconds. */
//@{

/*! Second constant, use to convert a time in seconds to
  internal units.  */
const double cmn_s = 1.0;

/*! Convert a time in internal units to seconds */
inline double cmnInternalTo_s(double valueInternalUnits) {
    return valueInternalUnits / cmn_s;
}

/*! Milliseconds constant, use to convert a time in milliseconds to
  internal units.  */
const double cmn_ms = cmn_s / 1000.0;

/*! Convert a time in internal units to milliseconds */
inline double cmnInternalTo_ms(double valueInternalUnits) {
    return valueInternalUnits / cmn_ms;
}

/*! Microsecond constant, use to convert a time in microseconds to
  internal units.  */
const double cmn_us = cmn_ms / 1000.0;

/*! Convert a time in internal units to microseconds */
inline double cmnInternalTo_us(double valueInternalUnits) {
    return valueInternalUnits / cmn_us;
}

/*! Nanosecond constant, use to convert a time in nanoseconds to
  internal units.  */
const double cmn_ns = cmn_us / 1000.0;

/*! Convert a time in internal units to nanoseconds */
inline double cmnInternalTo_ns(double valueInternalUnits) {
    return valueInternalUnits / cmn_ns;
}

/*! Minutes constant, use to convert a time in minutes to
  internal units.  */
const double cmn_minute = 60.0 * cmn_s;

/*! Convert a time in internal units to minutes */
inline double cmnInternalTo_minute(double valueInternalUnits) {
    return valueInternalUnits / cmn_minute;
}

/*! Hours constant, use to convert a time in hours to
  internal units.  */
const double cmn_hour = 60.0 * cmn_minute;

/*! Convert a time in internal units to hours */
inline double cmnInternalTo_hour(double valueInternalUnits) {
    return valueInternalUnits / cmn_hour;
}

/*! Days constant, use to convert a time in days to
  internal units.  */
const double cmn_day = 24.0 * cmn_hour;

/*! Convert a time in internal units to days */
inline double cmnInternalTo_day(double valueInternalUnits) {
    return valueInternalUnits / cmn_day;
}

/*! Convert frequency to period */
inline double cmnHzToPeriod(const double frequencyInHz) {
    return cmn_s / frequencyInHz;
}

//@}


#endif // _cmnUnits_h
