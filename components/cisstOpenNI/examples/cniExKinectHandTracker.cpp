/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cniExKinectHandTracker.cpp 2378 2011-03-14 04:27:41Z kguerin1 $

  Author(s):  Kelleher Guerin
  Created on: 2011

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

int main(){
  
	cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
	cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
	cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );

	cisstOpenNI kinect;
    kinect.Configure( "C:/dev/OpenNI/data/SamplesConfig.xml" );

	while(true){
        
        // Wait and Update All
        kinect.UpdateAll();
		vctDynamicMatrix<double> depth = kinect.GetDepthImage8bit();
		vctDynamicMatrix<unsigned char> rgb = kinect.GetRGBImage();
		vctDynamicMatrix<double> range = kinect.GetRangeData();
        std::vector<cisstOpenNISkeleton*> skeletons = kinect.UpdateAndGetUserSkeletons();

		osaSleep(10*cmn_ms);
	}

	kinect.CleanupExit();
}
