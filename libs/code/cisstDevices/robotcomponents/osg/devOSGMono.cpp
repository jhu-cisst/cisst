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
  height( height )
#if CISST_SVL_HAS_OPENCV2
  ,depthsample( NULL ),
  colorsample( NULL )
#endif
{
  
  // Set the intrinsic paramters
  getCamera()->setProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);

  // Create the view port first since the FinalDrawCallback needs it and we need
  // to create the final callback in the constructor to initialize the SVL stuff
  // right away
  getCamera()->setViewport( new osg::Viewport( x, y, width, height ) );

  // Setup the SVL stuff
#if CISST_SVL_HAS_OPENCV2

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

  // Create the SVL depth buffer sample for the image
  try{ depthbuffersample = new svlBufferSample( svlTypeMatrixFloat ); }
  catch( std::bad_alloc& ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to allocate float svlBufferSampple." 
		      << std::endl;
  }

  // Create the SVL depth sample (float matrix)
  try{ depthsample = new svlSampleMatrixFloat( false ); }
  catch( std::bad_alloc& ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to allocate svlSampleMatrixFloat." 
		      << std::endl;
  }

  int retval = !SVL_OK;

  // Attach the sample matrix to the SVL buffer
  retval = depthsample->SetMatrix( finaldrawcallback->GetvctDepthImage() );
  if( retval != SVL_OK ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to set buffer of the depth buffer sample." 
		      << std::endl;
  }

  // push the sample in the buffer
  depthbuffersample->Push( depthsample );

  // Create the SVL color buffer sample for the RGB images
  try{ colorbuffersample = new svlBufferSample( svlTypeImageRGB ); }
  catch( std::bad_alloc& ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to allocate RGB svlBufferSampple." 
		      << std::endl;
  }

  // Create the RGB depth sample
  try{ colorsample = new svlSampleImageRGB( false ); }
  catch( std::bad_alloc& ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to allocate svlSampleImageRGB." 
		      << std::endl;
  }

  // Attach the RGB sample to the buffer
  retval = colorsample->SetMatrix( finaldrawcallback->GetvctColorImage(), 0);
  if( retval != SVL_OK){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			   << "Failed to set matrix for left color image."
			   << std::endl;
  }
  
  colorbuffersample->Push( colorsample );

#endif

}

devOSGMono::~devOSGMono(){

#if CISST_SVL_HAS_OPENCV2
  
  if( depthsample != NULL ){
    delete depthsample;
    depthsample = NULL;
  }
  
  if( colorsample != NULL ){
    delete colorsample; 
    colorsample = NULL;
  }

#endif

}

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
#if CISST_SVL_HAS_OPENCV2
  colorbuffersample->Push( colorsample ); 
  depthbuffersample->Push( depthsample );
#endif

}

#if CISST_SVL_HAS_OPENCV2

cv::Mat devOSGMono::GetDepthImage( size_t ) const{
  // get the camera final draw callback
  const osg::Camera::DrawCallback* dcb = NULL;
  dcb = getCamera()->getFinalDrawCallback();

  // cast
  const devOSGCamera::FinalDrawCallback* finaldrawcallback = NULL;
  finaldrawcallback=dynamic_cast<const devOSGCamera::FinalDrawCallback*>(dcb);

  CMN_ASSERT( finaldrawcallback != NULL );
  return finaldrawcallback->GetCVDepthImage();
}

cv::Mat devOSGMono::GetColorImage( size_t ) const{
  // get the camera final draw callback
  const osg::Camera::DrawCallback* dcb = NULL;
  dcb = getCamera()->getFinalDrawCallback();

  // cast
  const devOSGCamera::FinalDrawCallback* finaldrawcallback = NULL;
  finaldrawcallback=dynamic_cast<const devOSGCamera::FinalDrawCallback*>(dcb);

  CMN_ASSERT( finaldrawcallback != NULL );
  return finaldrawcallback->GetCVColorImage();
}

#endif
