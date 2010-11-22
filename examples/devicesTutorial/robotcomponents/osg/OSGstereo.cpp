

// Need to include these first otherwise there's a mess with
//  #defines in cisstConfig.h
#include <osgViewer/CompositeViewer>
#include <osgGA/TrackballManipulator>
#include <osgGA/StateSetManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>
#include <cisstDevices/robotcomponents/osg/devOSGBody.h>

#include <cisstVector/vctMatrixRotation3.h>
#include <cisstVector/vctFixedSizeVector.h>

#include <cisstCommon/cmnGetChar.h>

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
  
  // Create a viewer
  osgViewer::CompositeViewer viewer;
  
  // Create the camera
  osg::Camera* leftcam = new osg::Camera();
  // set black background
  leftcam->setClearColor( osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
  // set the projection matrix: viewing algle, ratio, near, far
  leftcam->setProjectionMatrixAsPerspective( 55, 512.0/480.0, 0.01, 10.0 );
  // set the position/orientation of the camera
  leftcam->setViewMatrixAsLookAt(  osg::Vec3d( 2, -0.1, 2 ),
				   osg::Vec3d( 0, -0.1, 0 ),
				   osg::Vec3d( 0,  0, 1 ) ); 

  // create a view for the left camera
  osgViewer::View* leftview = new osgViewer::View();
  leftview->setCamera( leftcam );
  leftview->setSceneData( world );
  leftview->setUpViewInWindow( 0, 0, 512, 480 );
  viewer.addView( leftview );

  // perform the same for the right camera
  osg::Camera* rightcam = new osg::Camera();
  rightcam->setClearColor( osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
  rightcam->setProjectionMatrixAsPerspective( 55, 512.0/480.0, 0.01, 10.0 );
  rightcam->setViewMatrixAsLookAt(  osg::Vec3d( 2, 0.1, 2 ),
				    osg::Vec3d( 0, 0.1, 0 ),
				    osg::Vec3d( 0, 0, 1 ) ); 
  osgViewer::View* rightview = new osgViewer::View();
  rightview->setCamera( rightcam );
  rightview->setSceneData( world );
  rightview->setUpViewInWindow( 512, 0, 512, 480 );
  viewer.addView( rightview );    

  // run the viewer
  while(!viewer.done())
    { viewer.frame(); }

  return 0;

}
