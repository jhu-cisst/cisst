/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cniExKinectTest.cpp 2916 2011-09-14 02:35:29Z sleonar7 $

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
#define SAMPLE_CONFIG_PATH "/Users/vagvoba/Code/Kinect/avin2-SensorKinect-2d13967/OpenNI/Data/SamplesConfig.xml"
#endif
#ifdef linux // This is defined by gcc
#define SAMPLE_CONFIG_PATH "/etc/openni/SamplesConfig.xml"
#endif

int main( int argc, char** argv ){
  
	cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
	cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
	cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );
    
    int numusers = 0;
    if( argc == 2 ){ sscanf( argv[1], "%d", &numusers ); }

	cisstOpenNI kinect( numusers );
    kinect.Configure( SAMPLE_CONFIG_PATH );
    if( 0 < numusers ){ kinect.InitSkeletons(); }

	while( true ){
        
        // Wait and Update All
        kinect.Update( WAIT_AND_UPDATE_ALL );
        if( 0 < numusers ){ kinect.UpdateUserSkeletons(); }

        {
            vctDynamicMatrix<unsigned char> rgb;
            if( kinect.GetRGBImage( rgb ) != cisstOpenNI::ESUCCESS ){
                CMN_LOG_RUN_ERROR << "Failed to get RGB image" << std::endl;
                return -1;
            }
            std::ofstream ofs( "rgb" );
            for( size_t r=0; r<rgb.rows(); r++ ){
                for( size_t c=0; c<rgb.cols(); c++ )
                    { ofs << (int)rgb[r][c] << " "; }
                ofs << std::endl;
            }
            ofs.close();
        }

        {
            vctDynamicMatrix<double> depth;
            if( kinect.GetDepthImageRaw( depth ) != cisstOpenNI::ESUCCESS ){
                CMN_LOG_RUN_ERROR << "Failed to get RGB image" << std::endl;
                return -1;
            }
            std::ofstream ofs( "depth" );
            ofs << depth;
            ofs.close();
        }
            
        {
            vctDynamicMatrix<double> range;
            std::vector< vctFixedSizeVector<unsigned short, 2> > pixels;
            if( kinect.GetRangeData(range, pixels) != cisstOpenNI::ESUCCESS ){
                CMN_LOG_RUN_ERROR << "Failed to get RGB image" << std::endl;
                return -1;
            }
            std::ofstream ofs( "range" );
            ofs << range;
            ofs.close();
        }
            
        if( 0 < numusers )
            { std::vector<cisstOpenNISkeleton*> skeletons = kinect.UpdateAndGetUserSkeletons(); }

        std::cerr << "*";
	}

	return 0;
}
