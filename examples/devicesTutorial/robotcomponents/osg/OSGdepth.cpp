

// Need to include these first otherwise there's a mess with
//  #defines in cisstConfig.h
#include <osgViewer/Viewer>
#include <osgDB/WriteFile>
#include <osgGA/TrackballManipulator>

#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>
#include <cisstDevices/robotcomponents/osg/devOSGBody.h>

#include <cisstVector/vctMatrixRotation3.h>

// This is a callback class to capture buffers of the viewer
class CallBack : public osg::Camera::DrawCallback {
private:

  std::string name;

  // This is called after each draw to read an image
  virtual void operator () (osg::RenderInfo& ) const{ 
    img->readPixels( 0,   0, 512, 480, GL_RGB, GL_UNSIGNED_BYTE );
    // flip the image upsize down
    img->flipVertical();
    osgDB::writeImageFile( *img, name );
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
  //devOSGBody* hubble;
  //hubble = new devOSGBody( "libs/etc/cisstRobot/objects/hst.3ds", world, Rt );

  // Load another object
  devOSGBody* background;
  background = new devOSGBody( "background",
			       vctFrame4x4<double>(),
			       "libs/etc/cisstRobot/objects/background.3ds",	
			       world );
			       


  // Create the OpenGL projection matrix. This is made to match camera 
  // intrinsic parameters.
  osgViewer::Viewer viewer;
  viewer.setSceneData( world );

  // Set the callback to read images after each draw
  // CallBack* callback = new CallBack("depth.bmp");
  // viewer.getCamera()->setFinalDrawCallback( callback );
  viewer.getCamera()->setClearColor( osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
  viewer.getCamera()->setViewMatrixAsLookAt( osg::Vec3d( 0, 0, 10 ), 
					     osg::Vec3d( 0, 0, 0 ),
					     osg::Vec3d( 0, 1, 0 ) );
  //viewer.setCameraManipulator( new osgGA::TrackballManipulator );
  
  osg::ref_ptr<osg::Image> depth = new osg::Image;
  //osg::ref_ptr<osg::Image> color = new osg::Image;
  //color->allocateImage( 640, 480, 1, GL_RGB,            GL_UNSIGNED_BYTE ); 
  depth->allocateImage( 640, 480, 1, GL_DEPTH_COMPONENT, GL_FLOAT );
  //viewer.getCamera()->attach( osg::Camera::COLOR_BUFFER, color.get(), 0, 0 );
  viewer.getCamera()->attach( osg::Camera::DEPTH_BUFFER, depth.get(), 0, 0 );

  viewer.setUpViewInWindow( 0, 0, 640, 480 );



  viewer.frame();
  
  //float Z[ 640*480 ];
  std::ofstream Z;
  while( !viewer.done() ){
    double fovy, aspectRatio, Zn,Zf;
    bool retval;
    retval = viewer.getCamera()->getProjectionMatrixAsPerspective ( fovy,
								    aspectRatio,
								    Zn,
								    Zf );

    float* z = (float*)depth->data();

    Z.open( "depth" );
    for( int r=480-1; 0<=r; r-- ){
      for( int c=0; c<640; c++ ){
	Z << Zn*Zf / (Zf - z[ r*640 + c ]*(Zf-Zn)) << " ";
      }
    }
    Z.close();

    viewer.frame();
  }

}
