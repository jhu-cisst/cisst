

// Need to include these first otherwise there's a mess with
//  #defines in cisstConfig.h
#include <osgViewer/CompositeViewer>
#include <osgDB/WriteFile>
#include <osgGA/TrackballManipulator>

#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>
#include <cisstDevices/robotcomponents/osg/devOSGBody.h>

#include <cisstStereoVision.h>


// This is a callback class to capture buffers of the viewer
class CallBack : public osg::Camera::DrawCallback {
private:

  std::string name;

  // This is called after each draw to read an image
  virtual void operator () (osg::RenderInfo& ) const{ 
    img->readPixels( 0,   0, 512, 480, GL_RGB, GL_UNSIGNED_BYTE );
    // flip the image upsize down
    img->flipVertical();
    //osgDB::writeImageFile( *img, name );
  }

public:

  // OSG images
  osg::ref_ptr<osg::Image> img;

  CallBack( const std::string& name ) : osg::Camera::DrawCallback(), name(name)
  { img  = new osg::Image; }

};

int main(){

  // Create the OSG World
  devOSGWorld* world;
  world = new devOSGWorld;

  // Create a rotation/translation
  vctFrame4x4<double> Rt( vctMatrixRotation3<double>(),
			  vctFixedSizeVector<double,3>(0.0, 0.0, 0.3) );
  // Load an object and shift it to Rt
  devOSGBody* hubble;
  hubble = new devOSGBody( "libs/etc/cisstRobot/objects/hst.3ds", world, Rt );

  // Load another object
  devOSGBody* background;
  background = new devOSGBody( "libs/etc/cisstRobot/objects/background.3ds",	
			       world, 
			       vctFrame4x4<double>() );


  // Create the OpenGL projection matrix. This is made to match camera 
  // intrinsic parameters.
  unsigned int width = 512;         // image width
  unsigned int height = 480;        // image height
  double fc1 = 1000, fc2 = 1000;    // x-y scalings
  double o1 = 256, o2 = 240;        // optical center
  double far = 10.0, near = 0.01;   // near/far planes
  osg::Matrix projection( 2*fc1/width,  0.0,           0.0, 0.0,
			  0.0,          2*fc2/height,  0.0, 0.0,
			  2*o1/width-1, 2*o2/height-1,-(far+near)/(far-near),-1,
			  0.0,          0.0,          -2*far*near/(far-near),0);

  // Create a viewer. Composite because we're cool
  osgViewer::CompositeViewer viewer;

  // add some ambience
  osg::Light* light = new osg::Light;
  light->setAmbient( osg::Vec4(1, 1, 1, 1 ) );
  
  // Create the left camera
  osg::Camera* leftcam = new osg::Camera();
  // set black background
  leftcam->setClearColor( osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
  // set the projection matrix
  leftcam->setProjectionMatrix( projection );
  // set the view matrix
  leftcam->setViewMatrixAsLookAt(  osg::Vec3d( 1.5, -0.05, 0.2 ),
				   osg::Vec3d(  0, -0.05, 0.2 ),
				   osg::Vec3d(  0,  0, 1 ) ); 

  // Set the callback to read images after each draw
  CallBack* leftcallback = new CallBack("left.bmp");
  leftcam->setFinalDrawCallback( leftcallback );

  // create a view for the left camera
  osgViewer::View* leftview = new osgViewer::View();
  // set the camera
  leftview->setCamera( leftcam );
  // set the scene
  leftview->setSceneData( world );
  // set the light
  leftview->setLight( light );
  // create a window
  leftview->setUpViewInWindow( 0, 0, 512, 480 );
  // add the view to the viewer
  viewer.addView( leftview );

  // perform the same for the right camera
  osg::Camera* rightcam = new osg::Camera();
  rightcam->setClearColor( osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
  rightcam->setProjectionMatrix( projection );
  rightcam->setViewMatrixAsLookAt(  osg::Vec3d( 1.5, 0.05, .2 ),
				    osg::Vec3d(  0, 0.05, .2 ),
				    osg::Vec3d(  0, 0, 1 ) ); 

  // Set the callback to read images after each draw
  CallBack* rightcallback = new CallBack("right.bmp");
  rightcam->setFinalDrawCallback( rightcallback );

  osgViewer::View* rightview = new osgViewer::View();
  rightview->setCamera( rightcam );
  rightview->setSceneData( world );
  rightview->setLight( light );
  rightview->setUpViewInWindow( 512, 0, 512, 480 );
  viewer.addView( rightview );    

  // Now do SVL stuff
  int mindisparity = 0;
  int maxdisparity = 300;

  svlInitialize();

  svlCameraGeometry geometry;
  svlRect roi;

  svlStreamManager stream(2);
  svlFilterSourceBuffer source;
  svlFilterComputationalStereo stereo;
  svlFilterStreamTypeConverter converter(svlTypeMatrixFloat, svlTypeImageRGB);
  svlFilterImageWindow window;
  
  // setup the source
  // create a RBG stereo buffer
  svlBufferSample buffersample( svlTypeImageRGBStereo );
  svlSampleImageRGBStereo sample;
  sample.SetSize( 512, 480 );
  buffersample.Push( &sample );
  source.SetBuffer( buffersample );
  source.SetTargetFrequency(10.0);
  
  geometry.SetIntrinsics( fc1, fc2, o1, o2,
			  0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			  SVL_LEFT);
  geometry.SetIntrinsics( fc1, fc2, o1, o2,
			  0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			  SVL_RIGHT);
  geometry.SetExtrinsics(0.0, 0.0, 0.0,
			 0.0, 0.0, 0.0,
			 SVL_LEFT);
  geometry.SetExtrinsics(0.0, 0.0, 0.0,
			 10.0, 0.0, 0.0, // baseline = 10.0 mm
			 SVL_RIGHT);
  stereo.SetCameraGeometry(geometry);
  
  roi.Assign(5, 5, width - 100, height - 5);
  stereo.SetROI(roi);
  
  int smoothness = 100;
  int blocksize = 5;
  bool subpixel_precision = false;
  bool xcheck = false;
  stereo.SetCrossCheck(xcheck);
  stereo.SetSubpixelPrecision(subpixel_precision);
  stereo.SetDisparityRange(mindisparity, maxdisparity);
  stereo.SetScalingFactor(0);
  stereo.SetBlockSize(blocksize);
  stereo.SetQuickSearchRadius(maxdisparity);
  stereo.SetSmoothnessFactor(smoothness);
  stereo.SetTemporalFiltering(0);
  stereo.SetSpatialFiltering(0);
  
  // setup converter
  converter.SetScaling(255.0f / maxdisparity);

  // chain filters to pipeline
  stream.SetSourceFilter(&source);
  source.GetOutput()->Connect(stereo.GetInput());
  stereo.GetOutput()->Connect(converter.GetInput());
  converter.GetOutput()->Connect(window.GetInput());
  
  if (stream.Play() != SVL_OK){
    std::cout << "ERROR" << std::endl;
    exit(0);
  }

  size_t j=0;
  while( !viewer.done() ){

    // render
    viewer.frame();

    // give a break to the viewer
    if( 1 < j ){
      memcpy( sample.GetUCharPointer( SVL_LEFT ), leftcallback->img->data(), 
	      512*480*3 );
      memcpy( sample.GetUCharPointer( SVL_RIGHT ), rightcallback->img->data(),
	      512*480*3 );
      buffersample.Push( &sample );
    }
    j++;
  }

  getchar();
  
  return 0;

}
