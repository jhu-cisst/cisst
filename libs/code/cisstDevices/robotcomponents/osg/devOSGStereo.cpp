#include <osg/ref_ptr>
#include <osg/View>
#include <osgDB/WriteFile>

#include <cisstCommon/cmnLogger.h>
#include <cisstDevices/robotcomponents/osg/devOSGStereo.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstStereoVision/svlSampleMatrixTypes.h>

// this is called during update traversal
void devOSGStereo::UpdateCallback::operator()( osg::Node* node, 
					       osg::NodeVisitor* nv ){
  osg::Referenced* data = node->getUserData();
  devOSGStereo::UserData* userdata;
  userdata = dynamic_cast<devOSGStereo::UserData*>( data );

  if( userdata != NULL ){ userdata->GetStereo()->Update(); }

  traverse( node, nv );

}

// This is called after everything else
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

    /*
    std::string s(camera->getName()+"depth");
    std::ofstream ofs( s.data() );
    for( int r=0; r<height; r++ ){
      for( int c=0; c<width; c++ ){
	ofs << depthimage[r][c] << " ";
      }
      ofs << std::endl;
    }
    ofs.close();
    */
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
    
    std::string s(camera->getName()+"color");
    /*
    std::ofstream ofs( s.data() );
    for( size_t r=0; r<height; r++ ){
      for( size_t c=0; c<width*3; c++ ){
	ofs << (int)colorimage[r][c] << " ";
      }
      ofs << std::endl;
    }
    ofs.close();
    */
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

  getCamera()->setProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);

  setSceneData( world );

  // Set callback stuff
  userdata = new devOSGStereo::UserData( this );
  getCamera()->setUserData( userdata );
  getCamera()->setUpdateCallback( new devOSGStereo::UpdateCallback );

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

    osg::ref_ptr<osg::GraphicsContext> gc;
    gc = osg::GraphicsContext::createGraphicsContext( traits.get() );

    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    camera->setName( GetName() + "LEFT" );
    camera->setGraphicsContext( gc.get() );
    camera->setViewport( new osg::Viewport( 0, 0, width, height ) );

    GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
    camera->setDrawBuffer( buffer );
    camera->setReadBuffer( buffer );
    camera->setClearColor( osg::Vec4d( 0.0, 0.0, 0.0, 0.0 ) );

    // drawing callback
    finaldrawcallbacks[0] = new FinalDrawCallback( camera, true, true );
    camera->setFinalDrawCallback( finaldrawcallbacks[0] );

    // add this slave camera to the viewer, with a shift left of the
    // projection matrix
    addSlave( camera.get(), 
	      osg::Matrixd(),
	      osg::Matrixd::translate( baseline/2.0, 0.0, 0.0 ) );

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

    osg::ref_ptr<osg::GraphicsContext> gc;
    gc = osg::GraphicsContext::createGraphicsContext(traits.get());

    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    camera->setName( GetName() + "RIGHT" );
    camera->setGraphicsContext(gc.get());
    camera->setViewport( new osg::Viewport( 0, 0, width, height) );

    GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
    camera->setDrawBuffer(buffer);
    camera->setReadBuffer(buffer);
    camera->setClearColor( osg::Vec4d( 0.0, 0.0, 0.0, 0.0 ) );

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
  depthbuffersample = new svlBufferSample( svlTypeMatrixFloat );
  depthsample       = new svlSampleMatrixFloat( false );

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
  colorbuffersample = new svlBufferSample( svlTypeImageRGBStereo );
  colorsample       = new svlSampleImageRGBStereo( false );

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
svlBufferSample* devOSGStereo::GetColorBufferSample() const 
{ return colorbuffersample; }

svlBufferSample* devOSGStereo::GetDepthBufferSample() const 
{ return depthbuffersample; }
#endif

void devOSGStereo::Update()
{}


