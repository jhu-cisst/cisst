
#include <cisstOpenNI/cisstOpenNI.h>
#include <cisstDevices/robotcomponents/osg/devOSGMono.h>
#include <cisstCommon/cmnGetChar.h>

int main(){
  
  cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );

  cisstOpenNI kinect( "/etc/openni/SamplesConfig.xml" );

  vctDynamicMatrix<double> depth = kinect.GetDepthImage();
  vctDynamicMatrix<unsigned char> rgb = kinect.GetRGBImage();
  vctDynamicMatrix<double> range = kinect.GetRangeData();

  std::cout << "ENTER to exit." << std::endl;
  cmnGetChar();

  return 0;
}
