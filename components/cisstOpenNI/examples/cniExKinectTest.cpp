/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Kelleher Guerin and Simon Leonard
  Created on: 2008

  (C) Copyright 2006-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstOpenNI/cisstOpenNI.h>
#include <cisstCommon/cmnGetChar.h>
#include <cisstOSAbstraction/osaSleep.h>

#ifdef _WIN32
#define SAMPLE_CONFIG_PATH "C:/dev/OpenNI/data/SamplesConfig.xml"
#endif
#ifdef __APPLE__ 
#define SAMPLE_CONFIG_PATH "/Developer-old/Kinect/SensorKinect/avin2-SensorKinect-28738dc/OpenNI/Data/SamplesConfig.xml"
#endif

int main(){
  
	cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
	cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
	cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );

	cisstOpenNI kinect(1);
    kinect.Configure(SAMPLE_CONFIG_PATH);
    kinect.InitSkeletons();
	while(true){
        
        // Wait and Update All
        kinect.Update(WAIT_AND_UPDATE_ALL);
        kinect.UpdateUserSkeletons();
		//vctDynamicMatrix<double> depth = kinect.GetDepthImage8bit();
		vctDynamicMatrix<unsigned char> rgb = kinect.GetRGBImage();
		vctDynamicMatrix<double> range = kinect.GetRangeData();
        std::vector<cisstOpenNISkeleton*> skeletons = kinect.UpdateAndGetUserSkeletons();

		osaSleep(10*cmn_ms);
	}

	return 0;
}
