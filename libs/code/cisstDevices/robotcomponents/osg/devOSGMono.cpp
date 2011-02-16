#include <cisstDevices/robotcomponents/osg/devOSGMono.h>

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
  try{ finaldrawcallback =  new FinalDrawCallback( getCamera(), true, true ); }
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

vctDynamicMatrix<double> devOSGMono::GetRangeData() const{
  // get the camera final draw callback
  const osg::Camera::DrawCallback* dcb = NULL;
  dcb = getCamera()->getFinalDrawCallback();

  // cast
  const devOSGCamera::FinalDrawCallback* finaldrawcallback = NULL;
  finaldrawcallback=dynamic_cast<const devOSGCamera::FinalDrawCallback*>(dcb);

  CMN_ASSERT( finaldrawcallback != NULL );
  return finaldrawcallback->GetRangeData();
}

cv::Mat devOSGMono::GetRGBImage() const{
  // get the camera final draw callback
  const osg::Camera::DrawCallback* dcb = NULL;
  dcb = getCamera()->getFinalDrawCallback();

  // cast
  const devOSGCamera::FinalDrawCallback* finaldrawcallback = NULL;
  finaldrawcallback=dynamic_cast<const devOSGCamera::FinalDrawCallback*>(dcb);

  CMN_ASSERT( finaldrawcallback != NULL );
  return finaldrawcallback->GetRGBImage();
}

vctDynamicNArray<unsigned char,3> devOSGMono::GetRGBPlanarImage() const{
  // get the camera final draw callback
  const osg::Camera::DrawCallback* dcb = NULL;
  dcb = getCamera()->getFinalDrawCallback();

  // cast
  const devOSGCamera::FinalDrawCallback* finaldrawcallback = NULL;
  finaldrawcallback=dynamic_cast<const devOSGCamera::FinalDrawCallback*>(dcb);

  CMN_ASSERT( finaldrawcallback != NULL );
  const cv::Mat& rgbimage = finaldrawcallback->GetRGBImage();

  cv::Size size = rgbimage.size();
  vctDynamicNArray<unsigned char, 3> x;
  x.SetSize( vctDynamicNArray<unsigned char, 3>::nsize_type( size.height, size.width, 3 ) );
  memcpy( x.Pointer(), rgbimage.ptr<unsigned char>(), size.height*size.width*3 );

  return x;
}

vctDynamicMatrix<unsigned char> devOSGMono::GetRGBPixelImage() const{
  // get the camera final draw callback
  const osg::Camera::DrawCallback* dcb = NULL;
  dcb = getCamera()->getFinalDrawCallback();

  // cast
  const devOSGCamera::FinalDrawCallback* finaldrawcallback = NULL;
  finaldrawcallback=dynamic_cast<const devOSGCamera::FinalDrawCallback*>(dcb);

  CMN_ASSERT( finaldrawcallback != NULL );
  const cv::Mat& rgbimage = finaldrawcallback->GetRGBImage();

  cv::Size size = rgbimage.size();
  vctDynamicMatrix<unsigned char> x( size.height, size.width*3 );
  memcpy( x.Pointer(), rgbimage.ptr<unsigned char>(), size.height*size.width*3 );

  return x;
}
#endif
