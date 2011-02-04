#include <osgGA/TrackballManipulator>
#include <cisstDevices/robotcomponents/osg/devOSGStereo.h>

devOSGStereo::devOSGStereo( const std::string& name, 
			    devOSGWorld* world,
			    int x, int y, int width, int height,
			    double fovy, double aspectRatio,
			    double zNear, double zFar,
			    double baseline, 
			    const std::string& fnname,
			    bool trackball ) :
  devOSGCamera( name, world, fnname, trackball ),
  x( x ),                              // x position
  y( y ),                              // y position
  width( width ),                      // width of images
  height( height ),
  baseline( baseline )
#if CISST_SVL_HAS_OPENCV2
  ,depthsample( NULL ),
  colorsample( NULL )
#endif
{

  // Set the intrinsic paramters
  getCamera()->setProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);

  // Setup the left camera
  {
    // Create a new (slave) camera
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;

    // Create the view port. Again, the reason why the viewport is created here
    // is because the SVL stuff in the final draw callback needs to be created
    // during the constructor
    camera->setViewport( new osg::Viewport( 0, 0, width, height) );

    // add this slave camera to the viewer, with a shift left of the
    // projection matrix
    addSlave( camera.get(), 
	      osg::Matrixd(),
	      osg::Matrixd::translate( baseline/2.0, 0.0, 0.0 ) );

    // Only do drawing callback if SVL + OpenCV is enabled
#if CISST_SVL_HAS_OPENCV2

    // OSG stuff

    // Create a drawing callback. This callback is set to capture depth+color 
    // buffer (true, true)
    osg::ref_ptr<devOSGCamera::FinalDrawCallback> finaldrawcallback;
    try{ finaldrawcallback =  new FinalDrawCallback( camera, true, true ); }
    catch( std::bad_alloc& ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< "Failed to allocate FinalDrawCallback."
			<< std::endl;
    }
    CMN_ASSERT( finaldrawcallback );
    camera->setFinalDrawCallback( finaldrawcallback );
    

    // SVL stuff

    // Create the buffer for the (left) depth image. The stereo rig only uses
    // only one depth buffer and it's for the left camera.
    try{ depthbuffersample = new svlBufferSample( svlTypeMatrixFloat ); }
    catch( std::bad_alloc& ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< "Failed to allocate depth buffer sample."
			<< std::endl;
    }

    // Create the sample of the the (left) depth buffer
    try{ depthsample = new svlSampleMatrixFloat( false ); }
    catch( std::bad_alloc& ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< "Failed to allocate depth sample."
			<< std::endl;
    }

    int retval = !SVL_OK;
    
    // attach the callback matrices to the (left) depth sample
    retval = depthsample->SetMatrix( finaldrawcallback->GetvctDepthImage() );
    if( retval != SVL_OK ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< "Failed to set matrix for left depth image."
			<< std::endl;
    }
    
    // push the depth sample in the buffer
    depthbuffersample->Push( depthsample );
    

    // Create the buffer for the RGB stereo images. This only needs to be
    // created once since both cameras will use the same buffer
    try{ colorbuffersample = new svlBufferSample( svlTypeImageRGBStereo ); }
    catch( std::bad_alloc& ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< "Failed to allocate color buffer sample."
			<< std::endl;
    }

    // Create the RGB stereo sample. This only needs to be created once since
    // both cameras will use the same sample (it's a "stereo" sample.
    try{ colorsample = new svlSampleImageRGBStereo( false ); }
    catch( std::bad_alloc& ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< "Failed to allocate svlSampleImageRGBStereo."
			<< std::endl;
    }
    
    // Attach the left camera to the left sample. The "0" is for the "left"
    // camera of the sample
    retval = colorsample->SetMatrix( finaldrawcallback->GetvctColorImage(), 0);
    if( retval != SVL_OK){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< "Failed to set matrix for left color image."
			<< std::endl;
    }

#endif // CISST_SVL_HAS_OPENCV2 for the left camera

  }

  // setup the right camera
  {
    // Create a new (slave) camera
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;

    // Create the view port. Again, the reason why the viewport is created here
    // is because the SVL stuff in the final draw callback needs to be created
    // during the constructor
    camera->setViewport( new osg::Viewport( 0, 0, width, height) );
    
    // add this slave camera to the viewer, with a shift right of the 
    // projection matrix                                  
    addSlave( camera.get(), 
	      osg::Matrixd(),
	      osg::Matrixd::translate( -baseline/2.0, 0.0, 0.0 ) );

    // Only do drawing callback if SVL + OpenCV is enabled
#if CISST_SVL_HAS_OPENCV2

    // OSG stuff

    // Create a drawing callback. This callback is set to capture depth+color 
    // buffer (true, true)
    osg::ref_ptr<devOSGCamera::FinalDrawCallback> finaldrawcallback;
    try{ finaldrawcallback =  new FinalDrawCallback( camera, true, true ); }
    catch( std::bad_alloc& ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< "Failed to allocate FinalDrawCallback."
			<< std::endl;
    }
    CMN_ASSERT( finaldrawcallback );
    camera->setFinalDrawCallback( finaldrawcallback );
    

    // SVL stuff
    
    int retval = !SVL_OK;

    // The stereo buffer and samples have been created above when creating the
    // left camera. So all that we need to do here is to attach the right color
    // image to the stereo sample and push the sample in the buffer
    retval = colorsample->SetMatrix( finaldrawcallback->GetvctColorImage(), 1 );
    if( retval != SVL_OK){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< "Failed to set matrix for right color image."
			<< std::endl;
    }
    
    colorbuffersample->Push( colorsample );

#endif // CISST_SVL_HAS_OPENCV2 for the right camera

  }

}

devOSGStereo::~devOSGStereo(){

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

void devOSGStereo::Startup(){

  // Create the graphic context traits. The reason why this is here is because
  // Windows somehow requires the context to be allocated within the same thread
  // as the rendering thread. And this method is called within the camera thread


  {

    osg::ref_ptr<osg::GraphicsContext::Traits> traits;
    traits = new osg::GraphicsContext::Traits;
    traits->x = x + 0;
    traits->y = y + 0;
    traits->width = width;
    traits->height = height;
    traits->windowDecoration = true;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;

    // slave "0" is the first slave. In this case the "left" camera
    osg::View::Slave& slave = getSlave(0);
    osg::Camera* camera = slave._camera.get();

    osg::ref_ptr<osg::GraphicsContext> gc;
    gc = osg::GraphicsContext::createGraphicsContext( traits.get() );
    camera->setName( GetName() + "LEFT" );
    camera->setGraphicsContext( gc.get() );

    GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
    camera->setDrawBuffer( buffer );
    camera->setReadBuffer( buffer );
    camera->setClearColor( osg::Vec4d( 0.0, 0.0, 0.0, 0.0 ) );

  }

  {

    osg::ref_ptr<osg::GraphicsContext::Traits> traits;
    traits = new osg::GraphicsContext::Traits;
    traits->x = x + width;
    traits->y = y + 0;
    traits->width = width;
    traits->height = height;
    traits->windowDecoration = true;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;

    // slave "1" is the second slave. In this case the "right" camera
    osg::View::Slave& slave = getSlave(1);
    osg::Camera* camera = slave._camera.get();

    osg::ref_ptr<osg::GraphicsContext> gc;
    gc = osg::GraphicsContext::createGraphicsContext( traits.get() );
    camera->setName( GetName() + "RIGHT" );
    camera->setGraphicsContext( gc.get() );

    GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
    camera->setDrawBuffer(buffer);
    camera->setReadBuffer(buffer);
    camera->setClearColor( osg::Vec4d( 0.0, 0.0, 0.0, 0.0 ) );

  }

}

void devOSGStereo::Run(){
  devOSGCamera::Run();
#if CISST_SVL_HAS_OPENCV2
  colorbuffersample->Push( colorsample ); 
  depthbuffersample->Push( depthsample );
#endif
}

#if CISST_SVL_HAS_OPENCV2

const cv::Mat& devOSGStereo::GetDepthImage( size_t ) const{

  // Get the left slave
  const osg::View::Slave& slave = getSlave(0);
  osg::Camera* camera = slave._camera.get();

  // get the camera final draw callback
  const osg::Camera::DrawCallback* dcb = NULL;
  dcb = camera->getFinalDrawCallback();

  // cast
  const devOSGCamera::FinalDrawCallback* finaldrawcallback = NULL;
  finaldrawcallback=dynamic_cast<const devOSGCamera::FinalDrawCallback*>(dcb);

  CMN_ASSERT( finaldrawcallback != NULL );
  return finaldrawcallback->GetCVDepthImage();
}

const cv::Mat& devOSGStereo::GetColorImage( size_t idx ) const{

  // Get the left/right slave
  const osg::View::Slave& slave = getSlave( idx );
  osg::Camera* camera = slave._camera.get();

  // get the camera final draw callback
  const osg::Camera::DrawCallback* dcb = NULL;
  dcb = camera->getFinalDrawCallback();

  // cast
  const devOSGCamera::FinalDrawCallback* finaldrawcallback = NULL;
  finaldrawcallback=dynamic_cast<const devOSGCamera::FinalDrawCallback*>(dcb);

  CMN_ASSERT( finaldrawcallback != NULL );
  return finaldrawcallback->GetCVColorImage();

}

#endif
