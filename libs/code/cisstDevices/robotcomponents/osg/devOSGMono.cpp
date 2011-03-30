#include <cisstDevices/robotcomponents/osg/devOSGMono.h>
#include <cisstOSAbstraction/osaSleep.h>

devOSGMono::devOSGMono( const std::string& name, 
			devOSGWorld* world,
			int x, int y, int width, int height,
			double fovy, double aspectRatio,
			double zNear, double zFar,
			const std::string& fnname,
			bool trackball ) :
  devOSGCamera( name, world, fnname, trackball ),
  x( x ),                              // x position
  y( y ),                              // y position
  width( width ),                      // width of images
  height( height ){
  
  // Set the intrinsic paramters
  getCamera()->setProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);

  // Create the view port first since the FinalDrawCallback needs it and we need
  // to create the final callback in the constructor to initialize the SVL stuff
  // right away
  getCamera()->setViewport( new osg::Viewport( x, y, width, height ) );

  // Setup the OpenCV stuff
#if CISST_DEV_HAS_OPENCV22

  // Create a drawing callback. This callback is set to capture depth+color 
  // buffer (true, true)
  osg::ref_ptr<devOSGCamera::FinalDrawCallback> finaldrawcallback;
  try{ finaldrawcallback =  new FinalDrawCallback( getCamera() ); }
  catch( std::bad_alloc& ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to allocate FinalDrawCallback."
		      << std::endl;
  }
  CMN_ASSERT( finaldrawcallback );
  getCamera()->setFinalDrawCallback( finaldrawcallback );

#endif

}

devOSGMono::~devOSGMono(){}

void devOSGMono::Startup(){

  // Create the graphic context traits. The reason why this is here is because
  // Windows somehow requires the context to be allocated within the same thread
  // as the rendering thread. And this method is called within the camera thread
  osg::ref_ptr<osg::GraphicsContext::Traits> traits;
  traits = new osg::GraphicsContext::Traits;
  traits->x = x ;
  traits->y = y ;
  traits->width = width;
  traits->height = height;
  traits->windowDecoration = true;
  traits->doubleBuffer = true;
  traits->sharedContext = 0;
  
  // Get the master camera
  osg::ref_ptr<osg::Camera> camera = getCamera();
  camera->setName( GetName() );

  // Create the graphic context
  osg::ref_ptr<osg::GraphicsContext> gc;
  gc = osg::GraphicsContext::createGraphicsContext( traits.get() );
  camera->setGraphicsContext( gc.get() );

  // Create/Set the drawing buffers
  GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
  camera->setDrawBuffer( buffer );
  camera->setReadBuffer( buffer );
  camera->setClearColor( osg::Vec4d( 0.0, 0.0, 0.0, 0.0 ) );

}

void devOSGMono::Run(){
  devOSGCamera::Run();
}

#if CISST_DEV_HAS_OPENCV22

std::list< std::list< devOSGBody* > > devOSGMono::GetVisibilityList(){

  // get the camera final draw callback
  osg::Camera::DrawCallback* dcb = NULL;
  dcb = getCamera()->getFinalDrawCallback();

  // 
  osg::ref_ptr<osg::Referenced> ref = dcb->getUserData();
  osg::ref_ptr<devOSGCamera::FinalDrawCallback::Data> data = NULL;
  data = dynamic_cast<devOSGCamera::FinalDrawCallback::Data*>( ref.get() );
  if( data != NULL ){
    data->RequestVisibilityList();
    while( data->VisibilityListRequested() ) {osaSleep( 1.0 );}
    return data->GetVisibilityList();
  }
  return std::list< std::list< devOSGBody* > >();
}


vctDynamicMatrix<double> devOSGMono::GetRangeData(){

  // get the camera final draw callback
  osg::Camera::DrawCallback* dcb = NULL;
  dcb = getCamera()->getFinalDrawCallback();
 
  // 
  osg::ref_ptr<osg::Referenced> ref = dcb->getUserData();
  osg::ref_ptr<devOSGCamera::FinalDrawCallback::Data> data = NULL;
  data = dynamic_cast<devOSGCamera::FinalDrawCallback::Data*>( ref.get() );
  if( data != NULL ){
    data->RequestRangeData();
    osaSleep( 1.0 );
    return data->GetRangeData();
  }
  return vctDynamicMatrix<double>();
}

vctDynamicNArray<unsigned char,3> devOSGMono::GetRGBPlanarImage(){

  // get the camera final draw callback
  osg::Camera::DrawCallback* dcb = NULL;
  dcb = getCamera()->getFinalDrawCallback();

  // 
  osg::ref_ptr<osg::Referenced> ref = dcb->getUserData();
  osg::ref_ptr<devOSGCamera::FinalDrawCallback::Data> data = NULL;
  data = dynamic_cast<devOSGCamera::FinalDrawCallback::Data*>( ref.get() );
  if( data != NULL ){
    data->RequestRGBImage();
    osaSleep( 1.0 );
    cv::Mat rgbimage = data->GetRGBImage();
    cv::Size size = rgbimage.size();

    vctDynamicNArray<unsigned char, 3> x;
    x.SetSize( vctDynamicNArray<unsigned char, 3>::nsize_type( size.height, 
							       size.width, 3 ) );
    memcpy( x.Pointer(), 
	    rgbimage.ptr<unsigned char>(), 
	    size.height*size.width*3 );

    return x;
  }
  return vctDynamicNArray<unsigned char, 3>();
}

cv::Mat devOSGMono::GetRGBImage(){

  // get the camera final draw callback
  osg::Camera::DrawCallback* dcb = NULL;
  dcb = getCamera()->getFinalDrawCallback();

  // 
  osg::ref_ptr<osg::Referenced> ref = dcb->getUserData();
  osg::ref_ptr<devOSGCamera::FinalDrawCallback::Data> data = NULL;
  data = dynamic_cast<devOSGCamera::FinalDrawCallback::Data*>( ref.get() );
  if( data != NULL ){
    data->RequestRGBImage();
    osaSleep( 1.0 );
    return data->GetRGBImage();
  }
  return cv::Mat();
}

cv::Mat devOSGMono::GetDepthImage(){
  // get the camera final draw callback
  osg::Camera::DrawCallback* dcb = NULL;
  dcb = getCamera()->getFinalDrawCallback();

  // 
  osg::ref_ptr<osg::Referenced> ref = dcb->getUserData();
  osg::ref_ptr<devOSGCamera::FinalDrawCallback::Data> data = NULL;
  data = dynamic_cast<devOSGCamera::FinalDrawCallback::Data*>( ref.get() );
  if( data != NULL ){
    data->RequestDepthImage();
    osaSleep( 1.0 );
    return data->GetDepthImage();
  }
  return cv::Mat();
}

#endif
