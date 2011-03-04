#include <osgGA/TrackballManipulator>

#include <cisstDevices/robotcomponents/osg/devOSGCamera.h>
#if (CISST_OS == CISST_DARWIN)
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <cisstMultiTask/mtsTransformationTypes.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

// This operator is called during update traversal
void devOSGCamera::UpdateCallback::operator()( osg::Node* node, 
					       osg::NodeVisitor* nv ){
  osg::Referenced* data = node->getUserData();
  devOSGCamera::UserData* userdata;
  userdata = dynamic_cast<devOSGCamera::UserData*>( data );

  if( userdata != NULL )
    { userdata->GetCamera()->Update(); }

  traverse( node, nv );

}

#if CISST_DEV_HAS_OPENCV22

// This is called after everything else
// It is used to capture color/depth images from the color/depth buffers
devOSGCamera::FinalDrawCallback::FinalDrawCallback( osg::Camera* camera,
						    bool capturedepth,
						    bool capturecolor ) :
  colorbufferrequest( false ),
  depthbufferrequest( false ),
  capturedepth( capturedepth ),
  capturecolor( capturecolor ){

  // get the viewport size
  const osg::Viewport* viewport    = camera->getViewport();
  osg::Viewport::value_type width  = viewport->width();
  osg::Viewport::value_type height = viewport->height();

  // Create and attach a depth image to the camera
  depthbuffer = new osg::Image;
  depthbuffer->allocateImage( width, height, 1, GL_DEPTH_COMPONENT, GL_FLOAT );
  camera->attach( osg::Camera::DEPTH_BUFFER, depthbuffer.get(), 0, 0 );

  // must be col major
  rangedata.SetSize( 3, width*height, VCT_COL_MAJOR );

  // Create and attach a color image to the camera
  colorbuffer = new osg::Image;
  colorbuffer->allocateImage( width, height, 1, GL_RGB, GL_UNSIGNED_BYTE );
  camera->attach( osg::Camera::COLOR_BUFFER, colorbuffer.get(), 0, 0 );
  
  // Create a OpenCV image
  rgbimage.create( height, width, CV_8UC3 );   // RGB image

}

devOSGCamera::FinalDrawCallback::~FinalDrawCallback(){
  rgbimage.release();
}

// This is called after each draw
void devOSGCamera::FinalDrawCallback::operator ()( osg::RenderInfo& info )const{

  // get the camera
  osg::Camera* camera = info.getCurrentCamera();

  // get the buffers attached to the cameras
  osg::Camera::BufferAttachmentMap map = camera->getBufferAttachmentMap ();

  // process the buffers
  osg::Camera::BufferAttachmentMap::iterator attachment;
  for( attachment=map.begin(); attachment!=map.end(); attachment++ ){

    // find the kind of buffer
    switch( attachment->first ){
      // Convert the depth buffer
    case osg::Camera::DEPTH_BUFFER:
      if( IsDepthBufferRequested() ){
	ConvertDepthBuffer( camera );
      }
      break;
      // Convert the color buffer
    case osg::Camera::COLOR_BUFFER:
      if( IsColorBufferRequested() ){
	ConvertColorBuffer( camera );
      }
      break;
      // nothing else
    default:
      break;
    }

  }

}

// Convert the depth buffer to something useful (depth values)
void 
devOSGCamera::FinalDrawCallback::ConvertDepthBuffer
( osg::Camera* camera ) const {

  
  // Should we care?
  if( IsDepthBufferEnabled() ){
    
    // get the viewport size
    const osg::Viewport* viewport = camera->getViewport();

    size_t width = viewport->width();
    size_t height = viewport->height();

    double* XYZ = const_cast<double*>( rangedata.Pointer() );

    GLint view[4];
    view[0] = (int)viewport->x();
    view[1] = (int)viewport->y();
    view[2] = width;
    view[3] = height;

    // Compute the depth image
    // get the intrinsic parameters of the camera
    double fovy, aspectRatio, Zn, Zf;
    camera->getProjectionMatrixAsPerspective( fovy, aspectRatio, Zn, Zf );
  
    // Convert zbuffer values [0,1] to range data and flip the image vertically
    float* z = (float*)depthbuffer->data();

    float* Z = NULL;
    if( depthimage.isContinuous() )
      // const_cast = lame!
      { Z = const_cast<float*>( depthimage.ptr<float>() ); }

    CMN_ASSERT( Z != NULL );
    int i=0;
    for( int r=height-1; 0<=r; r-- ){
      for( int c=0; c<width; c++ ){
	// forgot where I took this equation
	Z[ i++ ] = Zn*Zf / (Zf - z[ r*width + c ]*(Zf-Zn));
      }
    }



    // Compute range data

    //GLdouble model[4][4];
    //glGetDoublev(GL_MODELVIEW_MATRIX, &model[0][0]);

    //GLdouble proj[4][4];
    //glGetDoublev(GL_PROJECTION_MATRIX, &proj[0][0]);
    
    osg::Matrixd modelm = camera->getViewMatrix();
    osg::Matrixd projm = camera->getProjectionMatrix();
    
    for( size_t x=0; x<width; x++ ){
      for( size_t y=0; y<height; y++ ){
	GLdouble X, Y, Z;
	float* d = (float*)depthbuffer->data( x, y );
	gluUnProject( x, y, *d, modelm.ptr(), projm.ptr(), view, &X, &Y, &Z );
	//gluUnProject( x, y, *d, &model[0][0], &proj[0][0], view, &X, &Y, &Z );
	// rangedata is 4xN column major
	*XYZ++ = X;
	*XYZ++ = Y;
	*XYZ++ = Z;
      }
    }

  }
}

void 
devOSGCamera::FinalDrawCallback::ConvertColorBuffer
( osg::Camera* camera ) const{
  
  // Should we care?
  if( IsColorBufferEnabled() ){
    
    // get the viewport size
    const osg::Viewport* viewport = camera->getViewport();
    size_t width  = (size_t)viewport->width();
    size_t height = (size_t)viewport->height();
    
    // copy the color buffer and flip the image vertically
    unsigned char* rgb = (unsigned char*)colorbuffer->data();
    unsigned char* RGB = NULL;
    if( rgbimage.isContinuous() )
      { RGB = const_cast<unsigned char*>( rgbimage.ptr<unsigned char>() ); } 
  
    CMN_ASSERT( RGB != NULL );

    // The format is BGR and flipped vertically
    for( size_t R=0; R<height; R++ ){
      for( size_t C=0; C<width; C++ ){
	RGB[ R*width*3 + C*3 + 0 ] = rgb[ (height-R-1)*width*3 + C*3 + 2]; 
	RGB[ R*width*3 + C*3 + 1 ] = rgb[ (height-R-1)*width*3 + C*3 + 1]; 
	RGB[ R*width*3 + C*3 + 2 ] = rgb[ (height-R-1)*width*3 + C*3 + 0]; 
      }
    }

    // use this line to dump a test image
    //cv::imwrite( "rgb.bmp", cvColorImage );
  }

}

#endif

devOSGCamera::devOSGCamera( const std::string& name, 
			    devOSGWorld* world,
			    const std::string& fnname,
			    bool trackball ) :  mtsTaskContinuous( name ),
  osgViewer::Viewer(){

  // Add a timeout as it can take time to load the windows
  SetInitializationDelay( 15.0 );

  // Set the scene
  setSceneData( world );

  // Set the user data to point to this object
  // WARNING: Duno if passing "this" in a construstor is kosher
  getCamera()->setUserData( new devOSGCamera::UserData( this ) );

  // update callback
  getCamera()->setUpdateCallback( new devOSGCamera::UpdateCallback() );

  // MTS stuff
  // This interface is used to change the position/orientation of the camera
  if( !fnname.empty() ){    mtsInterfaceRequired* required;
    required = AddInterfaceRequired( "Transformation", MTS_OPTIONAL );
    if( required != NULL )
      { required->AddFunction( fnname, ReadTransformation ); }
  }

  // Create default trackball and light
  if( trackball ){
    // Add+configure the trackball of the camera
    setCameraManipulator( new osgGA::TrackballManipulator );

    getCameraManipulator()->setHomePosition( osg::Vec3d( 0,0,1 ),
					     osg::Vec3d( 0,0,0 ),
					     osg::Vec3d( 0,1,0 ) );
    home();

    // add a bit more light
    osg::ref_ptr<osg::Light> light = new osg::Light;
    light->setAmbient( osg::Vec4( .7, .7, .7, 1 ) );
    setLight( light );
  }

}

devOSGCamera::~devOSGCamera(){}

void devOSGCamera::Run(){
  ProcessQueuedCommands();
  frame();
}

// Set the position/orientation of the camera
void devOSGCamera::SetMatrix( const vctFrame4x4<double>& Rt ){
  // Set OSG transformation
  getCamera()->setViewMatrix( osg::Matrix( Rt[0][0], Rt[1][0], Rt[2][0], 0.0,
					   Rt[0][1], Rt[1][1], Rt[2][1], 0.0,
					   Rt[0][2], Rt[1][2], Rt[2][2], 0.0,
					   Rt[0][3], Rt[1][3], Rt[2][3], 1.0 ));
}

void devOSGCamera::Update(){
  // Get the transformation if possible
  if( ReadTransformation.IsValid() ){
    mtsDoubleFrm4x4 Rt;
    ReadTransformation( Rt );
    SetMatrix( vctFrame4x4<double>(Rt) );
  }
}


/*
// Convert the depth buffer to something useful (depth values)
void 
devOSGCamera::FinalDrawCallback::ConvertDepthBuffer
( osg::Camera* camera ) const {

  
  // Should we care?
  if( IsDepthBufferEnabled() ){
    
  }

}

*/
