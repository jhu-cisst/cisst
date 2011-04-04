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
  devOSGCamera::Data* data = NULL;
  data = dynamic_cast<devOSGCamera::Data*>( node->getUserData() );

  if( data != NULL )
    { data->GetCamera()->Update(); }

  traverse( node, nv );

}

#if CISST_DEV_HAS_OPENCV22

devOSGCamera::FinalDrawCallback::Data::Data( size_t width, size_t height ) : 
  osg::Referenced(),                                 // referenced object
  visibilityrequest( false ),                        // no request
  rangerequest( false ),                        // no request
  depthrequest( false ),                        // no request
  colorrequest( false ),                        // no request
  rangedata( 3, width*height, VCT_COL_MAJOR ),       // 3xM*N 
  depthimage( height, width, CV_32FC1 ),             // cv::Mat
  rgbimage( height, width, CV_8UC3 ){                // cv::Mat
}

devOSGCamera::FinalDrawCallback::Data::~Data(){
  depthimage.release();
  rgbimage.release();
}

std::list< std::list<devOSGBody*> >
devOSGCamera::FinalDrawCallback::Data::GetVisibilityList() const
{ return visibilitylist; }

vctDynamicMatrix<double>
devOSGCamera::FinalDrawCallback::Data::GetRangeData() const
{ return rangedata; }

cv::Mat 
devOSGCamera::FinalDrawCallback::Data::GetDepthImage() const
{ return depthimage; }

cv::Mat 
devOSGCamera::FinalDrawCallback::Data::GetRGBImage() const
{ return rgbimage; }

void 
devOSGCamera::FinalDrawCallback::Data::SetVisibilityList
( const std::list< std::list<devOSGBody*> >& visibilitylist )
{ this->visibilitylist = visibilitylist; visibilityrequest = false;}

void
devOSGCamera::FinalDrawCallback::Data::SetRangeData
( const vctDynamicMatrix<double>& rangedata )
{ this->rangedata = rangedata; rangerequest = false; }

void 
devOSGCamera::FinalDrawCallback::Data::SetDepthImage
( const cv::Mat& depthimage )
{ this->depthimage = depthimage; depthrequest = false; }

void 
devOSGCamera::FinalDrawCallback::Data::SetRGBImage
( const cv::Mat& rgbimage )
{ this->rgbimage = rgbimage; colorrequest = false; }


// This is called after everything else
// It is used to capture color/depth images from the color/depth buffers
devOSGCamera::FinalDrawCallback::FinalDrawCallback( osg::Camera* camera ){

  // get the viewport size
  const osg::Viewport* viewport    = camera->getViewport();
  osg::Viewport::value_type width  = viewport->width();
  osg::Viewport::value_type height = viewport->height();

  // Create and attach a depth image to the camera
  try{ depthbufferimg = new osg::Image; }
  catch( std::bad_alloc ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to allocate image for depth buffer."
		      << std::endl;
  }
  depthbufferimg->allocateImage(width, height, 1, GL_DEPTH_COMPONENT, GL_FLOAT);
  camera->attach( osg::Camera::DEPTH_BUFFER, depthbufferimg.get(), 0, 0 );

  // Create and attach a color image to the camera
  try{ colorbufferimg = new osg::Image; }
  catch( std::bad_alloc ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to allocate image for color buffer."
		      << std::endl;
  }
  colorbufferimg->allocateImage( width, height, 1, GL_RGB, GL_UNSIGNED_BYTE );
  camera->attach( osg::Camera::COLOR_BUFFER, colorbufferimg.get(), 0, 0 );

  // Create the data for this callback
  osg::ref_ptr< devOSGCamera::FinalDrawCallback::Data > data;
  try{ data = new devOSGCamera::FinalDrawCallback::Data( width, height ); }
  catch( std::bad_alloc ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to create data for final draw callback." 
		      << std::endl;
  }

  setUserData( data.get() );

}

devOSGCamera::FinalDrawCallback::~FinalDrawCallback(){}

// This is called after each draw
void devOSGCamera::FinalDrawCallback::operator()( osg::RenderInfo& info ) const{

  // get the user data
  osg::ref_ptr< const osg::Referenced > ref = getUserData();

  // cast the reference as data
  osg::ref_ptr< const devOSGCamera::FinalDrawCallback::Data > data;
  data = dynamic_cast<const devOSGCamera::FinalDrawCallback::Data*>(ref.get());

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
	  
	  if( data->VisibilityListRequested() )
	    { ComputeVisibilityList( camera ); }
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

static bool CompareVisibilityList( const std::list< devOSGBody* >& l1,
				   const std::list< devOSGBody* >& l2 ){
  std::list< devOSGBody* >::const_iterator b1 = l1.begin();
  std::list< devOSGBody* >::const_iterator b2 = l2.begin();
  while( (b1!=l1.end()) && (b2!=l2.end()) ){
    if( *b1 < *b2 ) return true;
    if( *b2 < *b1 ) return false;
    b1++;
    b2++;
  }
  if( l1.size() < l2.size() ) return true;
  else return false;
}

void
devOSGCamera::FinalDrawCallback::ComputeVisibilityList
( osg::Camera* camera ) const {

  // remove the const
  osg::Referenced* ref = const_cast< osg::Referenced* >( getUserData() );
  // cast as callback data
  devOSGCamera::FinalDrawCallback::Data* data = NULL;
  data = dynamic_cast< devOSGCamera::FinalDrawCallback::Data* >( ref );

  // ensure that the casting worked
  if( data != NULL ){

    // get the viewport size
    const osg::Viewport* viewport = camera->getViewport();
    size_t width = viewport->width();
    size_t height = viewport->height();

    std::list< std::list< devOSGBody* > > visibilitylist;

    // For each pixel in the image
    for( size_t r=0; r<height; r++ ){
      for( size_t c=0; c<width; c++ ){

	std::list< devOSGBody* > visibility;

	double x = (2.0 * c ) / width  - 1.0;
	double y = (2.0 * r ) / height - 1.0;
	double dx( .05 ), dy( .05 );

	// create a picker
	osg::ref_ptr< osgUtil::PolytopeIntersector> pi;
	pi = new osgUtil::PolytopeIntersector( osgUtil::Intersector::PROJECTION,
					       x-dx, y-dy, x+dx, y+dy );

	// and an intersection visitor
	osgUtil::IntersectionVisitor iv( pi );

	// run the visitor on the camera
	camera->accept( iv );
	
	// any intersection found?
	if( pi->containsIntersections() ){

	  // loop over all the intersections
	  osgUtil::PolytopeIntersector::Intersections::const_iterator iter;
      
	  for( iter =pi->getIntersections().begin(); 
	       iter!=pi->getIntersections().end();
	       iter++ ){
	    
	    // get the nodepath for this intersection
	    const osg::NodePath& nodePath = iter->nodePath;
	    unsigned int idx = nodePath.size();
	    
	    // for all the nodes along the path
	    while(idx--){
	      
	      // cast the node as a osg body
	      devOSGBody* body = dynamic_cast< devOSGBody* >( nodePath[ idx ] );
	      
	      // if successfull then add the body to the list
	      if( body != NULL )
		{ visibility.push_back( body ); }
	    }
	  }
	  visibility.unique();
	  visibilitylist.push_back( visibility );

	  visibilitylist.sort( CompareVisibilityList );
	  visibilitylist.unique();

	}
      }
    }
    data->SetVisibilityList( visibilitylist );
  }
  
}

void 
devOSGCamera::FinalDrawCallback::ComputeDepthImage
( osg::Camera* camera ) const {

  // remove the const
  osg::Referenced* ref = const_cast< osg::Referenced* >( getUserData() );
  // cast as callback data
  devOSGCamera::FinalDrawCallback::Data* data = NULL;
  data = dynamic_cast< devOSGCamera::FinalDrawCallback::Data* >( ref );

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
devOSGCamera::FinalDrawCallback::ComputeRangeData
( osg::Camera* camera ) const {

  // remove the const
  osg::Referenced* ref = const_cast< osg::Referenced* >( getUserData() );
  // cast as callback data
  devOSGCamera::FinalDrawCallback::Data* data = NULL;
  data = dynamic_cast< devOSGCamera::FinalDrawCallback::Data* >( ref );

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

    for( size_t x=0; x<width; x++ ){
      for( size_t y=0; y<height; y++ ){
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
devOSGCamera::FinalDrawCallback::ComputeRGBImage
( osg::Camera* camera ) const{
  
  // remove the const
  osg::Referenced* ref = const_cast< osg::Referenced* >( getUserData() );
  // cast as callback data
  devOSGCamera::FinalDrawCallback::Data* data = NULL;
  data = dynamic_cast< devOSGCamera::FinalDrawCallback::Data* >( ref );

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
  getCamera()->setUserData( new devOSGCamera::Data( this ) );

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
