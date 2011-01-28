#include <osg/ref_ptr>
#include <osg/View>
#include <osgDB/WriteFile>

#include <cisstCommon/cmnLogger.h>
#include <cisstDevices/robotcomponents/osg/devOSGStereo.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstStereoVision/svlSampleMatrixTypes.h>

#include <exception>

// this is called during update traversal
// it is used to call the "update" method that updates the position of the 
// stereo rig
void devOSGStereo::UpdateCallback::operator()( osg::Node* node, 
					       osg::NodeVisitor* nv ){
  osg::Referenced* data = node->getUserData();
  devOSGStereo::UserData* userdata;
  userdata = dynamic_cast<devOSGStereo::UserData*>( data );

  if( userdata != NULL ){ userdata->GetStereo()->Update(); }

  traverse( node, nv );

}

// This is called after everything else
// It is used to capture color/depth images from the color/depth buffers
devOSGStereo::FinalDrawCallback::FinalDrawCallback( osg::Camera* camera,
						    bool drawdepth,
						    bool drawcolor ) :
  drawdepth( drawdepth ),
  drawcolor( drawcolor ){

  // get the viewport size
  const osg::Viewport* viewport    = camera->getViewport();
  osg::Viewport::value_type width  = viewport->width();
  osg::Viewport::value_type height = viewport->height();

  // Create and attach a depth image to the camera
  depthbuffer = new osg::Image;
  depthbuffer->allocateImage( width, height, 1, GL_DEPTH_COMPONENT, GL_FLOAT );
  camera->attach( osg::Camera::DEPTH_BUFFER, depthbuffer, 0, 0 );

  // Create a vct image
  depthimage.SetSize( height, width );

  // Create and attach a color image to the camera
  colorbuffer = new osg::Image;
  colorbuffer->allocateImage( width, height, 1, GL_RGB, GL_UNSIGNED_BYTE );
  camera->attach( osg::Camera::COLOR_BUFFER, colorbuffer, 0, 0 );
  
  // Create a vct image
  colorimage.SetSize( height, width*3 );

}

// This is called after each draw
void devOSGStereo::FinalDrawCallback::operator ()( osg::RenderInfo& info )const{

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
      ConvertDepthBuffer( camera );
      break;
      // Convert the color buffer
    case osg::Camera::COLOR_BUFFER:
      ConvertColorBuffer( camera );
      break;
      // nothing else
    default:
      break;
    }

  }
  
}

// Convert the depth buffer to something useful (depth values)
void 
devOSGStereo::FinalDrawCallback::ConvertDepthBuffer
( osg::Camera* camera ) const {

  if( IsDepthBufferEnabled() ){

    // get the viewport size
    const osg::Viewport* viewport = camera->getViewport();
    int width  = (int)viewport->width();
    int height = (int)viewport->height();

    // get the intrinsic parameters of the camera
    double fovy, aspectRatio, Zn, Zf;
    camera->getProjectionMatrixAsPerspective( fovy, aspectRatio, Zn, Zf );
  
    // Convert zbuffer values [0,1] to range data and flip the image vertically
    float* z = (float*)depthbuffer->data();
    float* Z = (float*)depthimage.Pointer();

    int i=0;
    for( int r=height-1; 0<=r; r-- ){
      for( int c=0; c<width; c++ ){
	// forgot where I took this equation
	Z[ i++ ] = Zn*Zf / (Zf - z[ r*width + c ]*(Zf-Zn));
      }
    }

  }

}


void 
devOSGStereo::FinalDrawCallback::ConvertColorBuffer
( osg::Camera* camera ) const {

  if( IsColorBufferEnabled() ){

    // get the viewport size
    const osg::Viewport* viewport = camera->getViewport();
    size_t width  = (size_t)viewport->width();
    size_t height = (size_t)viewport->height();
    
    // copy the color buffer and flip the image vertically
    unsigned char* rgb = (unsigned char*)colorbuffer->data();
    unsigned char* RGB = (unsigned char*)colorimage.Pointer();
    // copy each row
    for( size_t r=0; r<height; r++ ){
      memcpy( RGB + r*width*3,
	      rgb + (height-r-1)*width*3,
	      sizeof(unsigned char)*width*3 );
    }
    
  }

}

devOSGStereo::devOSGStereo( const std::string& name, 
			    devOSGWorld* world,
			    int xoffset, int yoffset, int width, int height,
			    double fovy, double aspectRatio,
			    double zNear, double zFar,
			    double baseline ) :
  mtsTaskContinuous( name ),
  osgViewer::Viewer(),
  x( xoffset ),
  y( yoffset ),
  width( width ),
  height( height ),
  baseline( baseline )
#ifdef CISST_STEREOVISION
  ,colorbuffersample( NULL ),
  colorsample( NULL ),
  depthbuffersample( NULL ),
  depthsample( NULL )
#endif
{
  // Add a timeout as it can take time to load the windows
  SetInitializationDelay( 5.0 );

  getCamera()->setProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);

  setSceneData( world );

  // Set the user data to point to this object
  userdata = new devOSGStereo::UserData( this );

  // Set the update callback of the rig
  getCamera()->setUserData( userdata );
  getCamera()->setUpdateCallback( new devOSGStereo::UpdateCallback );

  // Setup the left camera
  {

    osg::ref_ptr<osg::Camera> camera = new osg::Camera;

    osg::ref_ptr<osg::Viewport> viewport;
    viewport = new osg::Viewport( 0, 0, width, height);
    camera->setViewport( viewport.get() );

    // drawing callback
    finaldrawcallbacks[0] = new FinalDrawCallback( camera, true, true );
    camera->setFinalDrawCallback( finaldrawcallbacks[0] );

    // add this slave camera to the viewer, with a shift left of the
    // projection matrix
    addSlave( camera.get(), 
	      osg::Matrixd(),
	      osg::Matrixd::translate( baseline/2.0, 0.0, 0.0 ) );

  }

  // setup the right camera
  {

    osg::ref_ptr<osg::Camera> camera = new osg::Camera;

    osg::ref_ptr<osg::Viewport> viewport;
    viewport = new osg::Viewport( 0, 0, width, height);
    camera->setViewport( viewport.get() );

    // drawing callback
    finaldrawcallbacks[1] = new FinalDrawCallback( camera, true, true );
    camera->setFinalDrawCallback( finaldrawcallbacks[1] );

    // add this slave camera to the viewer, with a shift right of the 
    // projection matrix                                  
    addSlave( camera.get(), 
	      osg::Matrixd(),
	      osg::Matrixd::translate( -baseline/2.0, 0.0, 0.0 ) );

  }

#ifdef CISST_STEREOVISION
  // SVL stuff
  int retval;

  // Create the buffer for the (left) depth image
  try{ depthbuffersample = new svlBufferSample( svlTypeMatrixFloat ); }
  catch( std::bad_alloc& ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to allocate depth buffer sample."
		      << std::endl;
  }

  try{ depthsample = new svlSampleMatrixFloat( false ); }
  catch( std::bad_alloc& ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to allocate depth sample."
		      << std::endl;
  }

  // attach the callback matrices to the sample
  retval = depthsample->SetMatrix( finaldrawcallbacks[0]->GetDepthImage() );
  if( retval != SVL_OK ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to set matrix for left depth image."
		      << std::endl;
  }

  // push the sample in the buffer
  depthbuffersample->Push( depthsample );

  // Create the buffer for the RGB images
  try{ colorbuffersample = new svlBufferSample( svlTypeImageRGBStereo ); }
  catch( std::bad_alloc& ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to allocate color buffer sample."
		      << std::endl;
  }

  try{ colorsample = new svlSampleImageRGBStereo( false ); }
  catch( std::bad_alloc& ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to allocate color sample."
		      << std::endl;
  }

 // attach the callback matrices to the sample
  retval = colorsample->SetMatrix( finaldrawcallbacks[0]->GetColorImage(), 0 );
  if( retval != SVL_OK){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			   << "Failed to set matrix for left color image."
			   << std::endl;
  }
  retval = colorsample->SetMatrix( finaldrawcallbacks[1]->GetColorImage(), 1 );
  if( retval != SVL_OK){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			   << "Failed to set matrix for right color image."
			   << std::endl;
  }

  colorbuffersample->Push( colorsample );

#endif

}

devOSGStereo::~devOSGStereo(){}

void devOSGStereo::Configure( const std::string& CMN_UNUSED( argv ) ) {}

void devOSGStereo::Startup(){

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
  ProcessQueuedCommands();
  frame();
#ifdef CISST_STEREOVISION
  colorbuffersample->Push( colorsample ); 
  depthbuffersample->Push( depthsample );
#endif
}

void devOSGStereo::Cleanup(){}

#ifdef CISST_STEREOVISION
svlBufferSample* devOSGStereo::GetColorBufferSample() const { 
  if( colorbuffersample != NULL ) 
    return colorbuffersample; 
  std::cout << "NULL: " << std::endl;
  return NULL;
}

svlBufferSample* devOSGStereo::GetDepthBufferSample() const { 
  if( depthbuffersample != NULL ) 
    return depthbuffersample; 
  std::cout << "NULL: " << std::endl;
  return NULL;
}
#endif

void devOSGStereo::Update()
{}


