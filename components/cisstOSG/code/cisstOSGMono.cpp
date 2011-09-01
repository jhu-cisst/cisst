#include <cisstOSG/cisstOSGMono.h>
#include <cisstOSAbstraction/osaSleep.h>

cisstOSGMono::cisstOSGMono( cisstOSGWorld* world,
			    int x, int y, int width, int height,
			    double fovy, double aspectRatio,
			    double zNear, double zFar,
			    bool trackball,
			    bool offscreenrendering ) :
  cisstOSGCamera( world, trackball, offscreenrendering ),
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
  osg::ref_ptr<cisstOSGCamera::FinalDrawCallback> finaldrawcallback;
  try{ finaldrawcallback =  new FinalDrawCallback( getCamera() ); }
  catch( std::bad_alloc& ){
    CMN_LOG_RUN_ERROR << "Failed to allocate FinalDrawCallback"
		      << std::endl;
  }
  CMN_ASSERT( finaldrawcallback );
  getCamera()->setFinalDrawCallback( finaldrawcallback );

#endif

}

cisstOSGMono::~cisstOSGMono(){}

void cisstOSGMono::Initialize(){

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
  if( IsOffscreenRenderingEnabled() ){ traits->pbuffer = true; }
  traits->sharedContext = 0;
  
  // Get the master camera
  osg::ref_ptr<osg::Camera> camera = getCamera();
  //camera->setName( GetName() );

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

#if CISST_DEV_HAS_OPENCV22

std::list< std::list< cisstOSGBody* > > cisstOSGMono::GetVisibilityList(){

  // get the camera final draw callback
  osg::Camera::DrawCallback* dcb = NULL;
  dcb = getCamera()->getFinalDrawCallback();

  // 
  osg::ref_ptr<osg::Referenced> ref = dcb->getUserData();
  osg::ref_ptr<cisstOSGCamera::FinalDrawCallback::Data> data = NULL;
  data = dynamic_cast<cisstOSGCamera::FinalDrawCallback::Data*>( ref.get() );
  if( data != NULL ){
    data->RequestVisibilityList();
    while( data->VisibilityListRequested() ) {osaSleep( 1.0 );}
    return data->GetVisibilityList();
  }
  return std::list< std::list< cisstOSGBody* > >();
}


vctDynamicMatrix<double> cisstOSGMono::GetRangeData(){

  // get the camera final draw callback
  osg::Camera::DrawCallback* dcb = NULL;
  dcb = getCamera()->getFinalDrawCallback();
 
  // 
  osg::ref_ptr<osg::Referenced> ref = dcb->getUserData();
  osg::ref_ptr<cisstOSGCamera::FinalDrawCallback::Data> data = NULL;
  data = dynamic_cast<cisstOSGCamera::FinalDrawCallback::Data*>( ref.get() );
  if( data != NULL ){
    data->RequestRangeData();
    osaSleep( 1.0 );
    return data->GetRangeData();
  }
  return vctDynamicMatrix<double>();
}

vctDynamicNArray<unsigned char,3> cisstOSGMono::GetRGBPlanarImage(){

  // get the camera final draw callback
  osg::Camera::DrawCallback* dcb = NULL;
  dcb = getCamera()->getFinalDrawCallback();

  // 
  osg::ref_ptr<osg::Referenced> ref = dcb->getUserData();
  osg::ref_ptr<cisstOSGCamera::FinalDrawCallback::Data> data = NULL;
  data = dynamic_cast<cisstOSGCamera::FinalDrawCallback::Data*>( ref.get() );
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

cv::Mat cisstOSGMono::GetRGBImage(){

  // get the camera final draw callback
  osg::Camera::DrawCallback* dcb = NULL;
  dcb = getCamera()->getFinalDrawCallback();

  // 
  osg::ref_ptr<osg::Referenced> ref = dcb->getUserData();
  osg::ref_ptr<cisstOSGCamera::FinalDrawCallback::Data> data = NULL;
  data = dynamic_cast<cisstOSGCamera::FinalDrawCallback::Data*>( ref.get() );
  if( data != NULL ){
    data->RequestRGBImage();
    osaSleep( 1.0 );
    return data->GetRGBImage();
  }
  return cv::Mat();
}

cv::Mat cisstOSGMono::GetDepthImage(){
  // get the camera final draw callback
  osg::Camera::DrawCallback* dcb = NULL;
  dcb = getCamera()->getFinalDrawCallback();

  // 
  osg::ref_ptr<osg::Referenced> ref = dcb->getUserData();
  osg::ref_ptr<cisstOSGCamera::FinalDrawCallback::Data> data = NULL;
  data = dynamic_cast<cisstOSGCamera::FinalDrawCallback::Data*>( ref.get() );
  if( data != NULL ){
    data->RequestDepthImage();
    osaSleep( 1.0 );
    return data->GetDepthImage();
  }
  return cv::Mat();
}

#endif
