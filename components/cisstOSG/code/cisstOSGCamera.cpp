#include <cisstOSG/cisstOSGCamera.h>
#include <osg/MatrixTransform>
#include <osgGA/TrackballManipulator>


#if (CISST_OS == CISST_DARWIN)
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

// This operator is called during update traversal
void cisstOSGCamera::UpdateCallback::operator()( osg::Node* node, 
						 osg::NodeVisitor* nv ){

  cisstOSGCamera::Data* data = NULL;
  data = dynamic_cast<cisstOSGCamera::Data*>( node->getUserData() );

  if( data != NULL )
    { data->GetCamera()->UpdateTransform(); }

  traverse( node, nv );

}

#if CISST_OSG_OPENCV

cisstOSGCamera::FinalDrawCallback::Data::Data( size_t width, size_t height ) : 
  osg::Referenced(),                            // referenced object
  rangerequest( false ),                        // no request
  depthrequest( false ),                        // no request
  colorrequest( false ),                        // no request
  rangedata( 3, width*height, VCT_COL_MAJOR ),  // 3xM*N 
  depthimage( height, width, CV_32FC1 ),        // cv::Mat
  rgbimage( height, width, CV_8UC3 ){           // cv::Mat
}

cisstOSGCamera::FinalDrawCallback::Data::~Data(){
  depthimage.release();
  rgbimage.release();
}

vctDynamicMatrix<double>
cisstOSGCamera::FinalDrawCallback::Data::GetRangeData() const
{ return rangedata; }

cv::Mat 
cisstOSGCamera::FinalDrawCallback::Data::GetDepthImage() const
{ return depthimage; }

cv::Mat 
cisstOSGCamera::FinalDrawCallback::Data::GetRGBImage() const
{ return rgbimage; }

void
cisstOSGCamera::FinalDrawCallback::Data::SetRangeData
( const vctDynamicMatrix<double>& rangedata )
{ this->rangedata = rangedata; rangerequest = false; }

void 
cisstOSGCamera::FinalDrawCallback::Data::SetDepthImage
( const cv::Mat& depthimage )
{ this->depthimage = depthimage; depthrequest = false; }

void 
cisstOSGCamera::FinalDrawCallback::Data::SetRGBImage
( const cv::Mat& rgbimage )
{ this->rgbimage = rgbimage; colorrequest = false; }


// This is called after everything else
// It is used to capture color/depth images from the color/depth buffers
cisstOSGCamera::FinalDrawCallback::FinalDrawCallback( osg::Camera* camera ){

  // get the viewport size
  const osg::Viewport* viewport    = camera->getViewport();
  osg::Viewport::value_type width  = viewport->width();
  osg::Viewport::value_type height = viewport->height();

  // Create and attach a depth image to the camera
  try{ depthbufferimg = new osg::Image; }
  catch( std::bad_alloc ){
    CMN_LOG_RUN_ERROR << " Failed to allocate image for depth buffer."
		      << std::endl;
  }
  depthbufferimg->allocateImage(width, height, 1, GL_DEPTH_COMPONENT, GL_FLOAT);
  camera->attach( osg::Camera::DEPTH_BUFFER, depthbufferimg.get(), 0, 0 );

  // Create and attach a color image to the camera
  try{ colorbufferimg = new osg::Image; }
  catch( std::bad_alloc ){
    CMN_LOG_RUN_ERROR << " Failed to allocate image for color buffer."
		      << std::endl;
  }
  colorbufferimg->allocateImage( width, height, 1, GL_RGB, GL_UNSIGNED_BYTE );
  camera->attach( osg::Camera::COLOR_BUFFER, colorbufferimg.get(), 0, 0 );

  // Create the data for this callback
  osg::ref_ptr< cisstOSGCamera::FinalDrawCallback::Data > data;
  try{ data = new cisstOSGCamera::FinalDrawCallback::Data( width, height ); }
  catch( std::bad_alloc ){
    CMN_LOG_RUN_ERROR << " Failed to create data for final draw callback." 
		      << std::endl;
  }

  setUserData( data.get() );

}

cisstOSGCamera::FinalDrawCallback::~FinalDrawCallback(){}

// This is called after each draw
void cisstOSGCamera::FinalDrawCallback::operator()(osg::RenderInfo& info)const{

  // get the user data
  osg::ref_ptr< const osg::Referenced > ref = getUserData();

  // cast the reference as data
  osg::ref_ptr< const cisstOSGCamera::FinalDrawCallback::Data > data;
  data=dynamic_cast<const cisstOSGCamera::FinalDrawCallback::Data*>(ref.get());

  // 
  if( data.get() != NULL ){

    // get the camera
    osg::Camera* camera = info.getCurrentCamera();

    // get the buffers attached to the cameras
    osg::Camera::BufferAttachmentMap map = camera->getBufferAttachmentMap ();

    // process the buffers attached to the camera
    osg::Camera::BufferAttachmentMap::iterator attachment;
    for( attachment=map.begin(); attachment!=map.end(); attachment++ ){

      // find the kind of buffer
      switch( attachment->first ){

	// A depth buffer is attached to the camera
      case osg::Camera::DEPTH_BUFFER:
	{
	  // Should we convert the buffer?
	  if( data->RangeDataRequested() )
	    { ComputeRangeData( camera ); }

	  if( data->DepthImageRequested() )
	    { ComputeDepthImage( camera ); }
	  
	}
	break;
	
	// A color buffer is attached to the camera
      case osg::Camera::COLOR_BUFFER:
	// Should we convert the buffer?
	if( data->RGBImageRequested() )
	  { ComputeRGBImage( camera ); }
	break;
	
	// nothing else
      default:
	break;
      }
      
    }
  }
}

void 
cisstOSGCamera::FinalDrawCallback::ComputeDepthImage
( osg::Camera* camera ) const {

  // remove the const
  osg::Referenced* ref = const_cast< osg::Referenced* >( getUserData() );
  // cast as callback data
  cisstOSGCamera::FinalDrawCallback::Data* data = NULL;
  data = dynamic_cast< cisstOSGCamera::FinalDrawCallback::Data* >( ref );

  // ensure that the casting worked
  if( data != NULL ){

    // get the viewport size
    const osg::Viewport* viewport = camera->getViewport();
    int width = viewport->width();
    int height = viewport->height();

    // get the intrinsic parameters of the camera
    double fovy, aspectRatio, Zn, Zf;
    camera->getProjectionMatrixAsPerspective( fovy, aspectRatio, Zn, Zf );

    // the z buffer values source
    float* z = (float*)depthbufferimg->data();

    // the depth image destination
    cv::Mat depthimage( height, width, CV_32FC1 );
    float* Z = NULL;
    if( depthimage.isContinuous() )
      { Z = depthimage.ptr<float>(); }

    CMN_ASSERT( Z != NULL );
    size_t i=0;
    // convert zbuffer values [0,1] to depth values + flip the image vertically
    for( int r=height-1; 0<=r; r-- ){
      for( int c=0; c<width; c++ ){
	// forgot where I took this equation
	Z[ i++ ] = Zn*Zf / (Zf - z[ r*width + c ]*(Zf-Zn));
      }
    }
    
    data->SetDepthImage( depthimage );

  }
}

// Convert the depth buffer to range data
void 
cisstOSGCamera::FinalDrawCallback::ComputeRangeData
( osg::Camera* camera ) const {

  // remove the const
  osg::Referenced* ref = const_cast< osg::Referenced* >( getUserData() );
  // cast as callback data
  cisstOSGCamera::FinalDrawCallback::Data* data = NULL;
  data = dynamic_cast< cisstOSGCamera::FinalDrawCallback::Data* >( ref );

  // ensure that the casting worked
  if( data != NULL ){

    // get the viewport size
    const osg::Viewport* viewport = camera->getViewport();
    size_t width = viewport->width();
    size_t height = viewport->height();

    // This is used by glutUnProject
    GLint view[4];
    view[0] = (int)viewport->x();
    view[1] = (int)viewport->y();
    view[2] = width;
    view[3] = height;
  
    // Create a 3xN range data destination matrix.
    // [ x1 ... xN ]
    // [ y1 ... yN ]
    // [ z1 ... zN ]
    // VCT_COL_MAJOR is used because we will write in the following order
    // x1, y1, z1, x2, y2, z2, ..., xN, yN, zZ
    vctDynamicMatrix<double> rangedata( 3, width*height, VCT_COL_MAJOR );
    double* XYZ = rangedata.Pointer();

    // get the intrinsic parameters of the camera
    double fovy, aspectRatio, Zn, Zf;
    camera->getProjectionMatrixAsPerspective( fovy, aspectRatio, Zn, Zf );
  
    osg::Matrixd modelm = camera->getViewMatrix();
    osg::Matrixd projm = camera->getProjectionMatrix();

    //for( int y=0; y<height; y++ ){
    for( int y=height-1; 0<=y; y-- ){
      for( size_t x=0; x<width; x++ ){
	GLdouble X, Y, Z;
	float* d = (float*)depthbufferimg->data( x, y );
	gluUnProject( x, y, *d, modelm.ptr(), projm.ptr(), view, &X, &Y, &Z );
	//gluUnProject( x, y, *d, &model[0][0], &proj[0][0], view, &X, &Y, &Z );
	// rangedata is 4xN column major
	*XYZ++ = X;
	*XYZ++ = Y;
	*XYZ++ = Z;
      }
    }
   
    data->SetRangeData( rangedata );
  }

}

void 
cisstOSGCamera::FinalDrawCallback::ComputeRGBImage
( osg::Camera* camera ) const{
  
  // remove the const
  osg::Referenced* ref = const_cast< osg::Referenced* >( getUserData() );
  // cast as callback data
  cisstOSGCamera::FinalDrawCallback::Data* data = NULL;
  data = dynamic_cast< cisstOSGCamera::FinalDrawCallback::Data* >( ref );

  // ensure that the casting worked
  if( data != NULL ){

    // get the viewport size
    const osg::Viewport* viewport = camera->getViewport();
    size_t width  = (size_t)viewport->width();
    size_t height = (size_t)viewport->height();
    
    // the rgb source
    unsigned char* rgb = (unsigned char*)colorbufferimg->data();

    // the rgb destination
    cv::Mat rgbimage( height, width, CV_8UC3 );
    unsigned char* RGB = NULL;
    if( rgbimage.isContinuous() )
      { RGB = rgbimage.ptr<unsigned char>(); } 
    
    CMN_ASSERT( RGB != NULL );
    
    // The format is BGR and flipped vertically
    for( size_t R=0; R<height; R++ ){
      for( size_t C=0; C<width; C++ ){
	RGB[ R*width*3 + C*3 + 0 ] = rgb[ (height-R-1)*width*3 + C*3 + 2]; 
	RGB[ R*width*3 + C*3 + 1 ] = rgb[ (height-R-1)*width*3 + C*3 + 1]; 
	RGB[ R*width*3 + C*3 + 2 ] = rgb[ (height-R-1)*width*3 + C*3 + 0]; 
      }
    }

    data->SetRGBImage( rgbimage );
    
  }
  // use this line to dump a test image
  //cv::imwrite( "rgb.bmp", cvColorImage );
}

#endif

cisstOSGCamera::cisstOSGCamera( cisstOSGWorld* world,
				bool trackball,
				bool offscreenrendering ) : 
  //mtsTaskContinuous( name ),
  osgViewer::Viewer(),
  offscreenrendering( offscreenrendering ){
  
  // Add a timeout as it can take time to load the windows
  //SetInitializationDelay( 15.0 );
  
  // Set the scene
  setSceneData( world );

  // Set the user data to point to this object
  getCamera()->setUserData( new cisstOSGCamera::Data( this ) );
    
  // update callback
  getCamera()->setUpdateCallback( new cisstOSGCamera::UpdateCallback() );

  osgtransform.makeLookAt( osg::Vec3d( 0,0,1 ),
			   osg::Vec3d( 0,0,0 ),
			   osg::Vec3d( 0,1,0 ) );
  getCamera()->setViewMatrix( osgtransform );

  // Create default trackball and light
  if( trackball ){
    // Add+configure the trackball of the camera
    setCameraManipulator( new osgGA::TrackballManipulator );
    getCameraManipulator()->setHomePosition( osg::Vec3d( 0,0,1 ),
					     osg::Vec3d( 0,0,0 ),
					     osg::Vec3d( 0,1,0 ) );
    home();
  }

  /*
  osg::StateSet* state = world->getOrCreateStateSet();
  state->setMode( GL_LIGHTING, osg::StateAttribute::ON );
  state->setMode( GL_LIGHT0, osg::StateAttribute::ON );
  state->setMode( GL_LIGHT1, osg::StateAttribute::ON );
  
  // Create the Light and set its properties.
  osg::ref_ptr<osg::Light> light = new osg::Light;
  light->setAmbient( osg::Vec4( .8, .8, .8, 1.0 ) );
  
  osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
  osg::Matrix m;
  m.makeTranslate( osg::Vec3( 0.0, 0.0, 2.0 ) );
  mt->setMatrix( m );
  
  // Add the Light to a LightSource. Add the LightSource and
  // MatrixTransform to the scene graph.
  osg::ref_ptr<osg::LightSource> ls = new osg::LightSource;
  world->addChild( mt.get() );
  mt->addChild( ls.get() );
  ls->setLight( light.get() );    
  */
}

cisstOSGCamera::~cisstOSGCamera(){}

// This is called from the OSG update callback thread
void cisstOSGCamera::UpdateTransform()
{ 
  getCamera()->setViewMatrix( osgtransform );
}

// This is called from the MTS thread
void cisstOSGCamera::SetTransform( const vctFrame4x4<double>& Rt ){
  vctFrame4x4<double> Rti( Rt );
  Rti.InverseSelf();
  vctQuaternionRotation3<double> Ri( Rti.Rotation() );

  osgtransform.setRotate( osg::Quat ( Ri[0], Ri[1], Ri[2], Ri[3] ) );
  osgtransform.setTrans( osg::Vec3d( Rti[0][3], Rti[1][3], Rti[2][3] ) );
}

//
void cisstOSGCamera::SetTransform( const vctFrm3& Rt )
{ SetTransform( vctFrame4x4<double>( Rt.Rotation(), Rt.Translation() ) ); }

vctFrm3 cisstOSGCamera::GetTransform() const {

  osg::Matrixd Rt;
  if( getCameraManipulator()!= NULL )
    Rt = getCameraManipulator()->getMatrix();
  else
    Rt = getCamera()->getViewMatrix();

  vctMatrixRotation3<double> R( Rt(0, 0), Rt(1, 0), Rt(2, 0),
				Rt(0, 1), Rt(1, 1), Rt(2, 1),
				Rt(0, 2), Rt(1, 2), Rt(2, 2) );
  vctFixedSizeVector<double,3> t( Rt(3, 0), Rt(3, 1), Rt(3, 2) );

  return vctFrm3( R, t );
}



