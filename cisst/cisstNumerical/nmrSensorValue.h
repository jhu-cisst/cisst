#ifndef _nmrSensorValue_h
#define _nmrSensorValue_h


/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 $Id: $
 
 Author(s):  Paul Wilkening
 Created on:
 
 (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.
 
 --- begin cisst license - do not edit ---
 
 This software is provided "as is" under an open source license, with
 no warranty.  The complete license can be found in license.txt and
 http://www.cisst.org/cisst/license.txt.
 
 --- end cisst license ---
 */

#include <cisstCommon/cmnGenericObject.h> 
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctDynamicMatrixTypes.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>

//! This is a pointer to a vector containing the latest values of a sensor, e.g. a force sensor
/*! \brief nmrSensorValue: A class that makes passing sensor values into nmrVFData computations easier
 */
class nmrSensorValue : public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_VERBOSE);

public:

	/*! Constructor
	*/ 
	nmrSensorValue(){};	 
	/*! Constructor
	\param v A pointer to a vector of sensor values
	*/
	nmrSensorValue(vctDynamicVector<double> * v);
	//! Gets the sensor values. 
	/*! getValues
	\return vctDynamicVector<double> The sensor values
	*/
	vctDynamicVector<double> getValues();

private:

	//! Pointer to a vector of sensor values
	vctDynamicVector<double> * values;

};

CMN_DECLARE_SERVICES_INSTANTIATION(nmrSensorValue);

#endif
