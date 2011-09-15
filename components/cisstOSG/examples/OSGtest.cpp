#include <cisstOSG/cisstOSGWorld.h>
#include <cisstOSG/cisstOSGMono.h>
#include <cisstOSG/cisstOSGBody.h>

int main( int, char** argv ){

  cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );

  osg::ref_ptr< cisstOSGWorld > world = new cisstOSGWorld;
  
  // Create a camera
  int x = 0, y = 0;
  int width = 320, height = 240;
  double Znear = 0.1, Zfar = 10.0;
  osg::ref_ptr< cisstOSGMono > camera;
  camera = new cisstOSGMono( world,
			     x, y, width, height,
			     55.0, ((double)width)/((double)height),
			     Znear, Zfar,
			     false );
  camera->Initialize();
  
  // Create the objects
  std::string path( CISST_SOURCE_ROOT"/libs/etc/cisstRobot/objects/" );

  vctFrame4x4<double> Rt(  vctMatrixRotation3<double>(),
			   vctFixedSizeVector<double,3>( 0.0, 0.0, 0.5 ) );

  osg::ref_ptr< cisstOSGBody > hubble;
  hubble = new cisstOSGBody( path+"hst.3ds", world, Rt );

  osg::ref_ptr< cisstOSGBody > background;
  background = new cisstOSGBody( path+"background.3ds", world, 
  				 vctFrame4x4<double>() );
  
  std::cout << "ESC to quit" << std::endl;

  // animate and render
  double theta=1.0;
#if CISST_OSG_OPENCV

  cv::namedWindow( "RGB", CV_WINDOW_AUTOSIZE );
  cv::namedWindow( "Depth", CV_WINDOW_AUTOSIZE );

#endif

  while( !camera->done() ){

    // rotate hubble
    vctFixedSizeVector<double,3> u( 0.0, 0.0, 1.0 );
    vctAxisAngleRotation3<double> Rwh( u, theta );
    vctFrame4x4<double> Rtwh(  Rwh,
			       vctFixedSizeVector<double,3>( 0.0, 0.0, 0.5 ) );
    hubble->SetTransform( Rtwh );

    // zoom out the camera
    vctFrame4x4<double> Rtwc( vctMatrixRotation3<double>(),
			      vctFixedSizeVector<double,3>(0.0, 0.0, theta ));
    camera->SetTransform( Rtwc );

    camera->frame();

#if CISST_OSG_OPENCV

    cv::Mat rgb, depth;
    camera->GetRGBImage( rgb );
    camera->GetDepthImage( depth );
    cv::imshow( "RGB", rgb );
    cv::waitKey( 30 );
    cv::imshow( "Depth", depth );
    cv::waitKey( 30 );

#endif

    theta += 0.001;
  }

  return 0;

}
