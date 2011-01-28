#include <osg/ref_ptr>
#include <osg/View>
#include <osgDB/WriteFile>

#include <cisstDevices/robotcomponents/osg/devOSGCamera.h>

#include <cisstMultiTask/mtsInterfaceRequired.h>

// this is called during update traversal
void devOSGCamera::UpdateCallback::operator()( osg::Node* node, 
					       osg::NodeVisitor* nv ){
  osg::Referenced* data = node->getUserData();
  devOSGCamera::UserData* userdata;
  userdata = dynamic_cast<devOSGCamera::UserData*>( data );

  if( userdata != NULL )
    { userdata->GetCamera()->Update(); }

  traverse( node, nv );

}

// This is called after each draw
void devOSGCamera::FinalDrawCallback::operator ()(osg::RenderInfo& info) const{ 

  
  osg::Referenced* data = info.getView()->getCamera()->getUserData();
  devOSGCamera::UserData* userdata;
  userdata = dynamic_cast<devOSGCamera::UserData*>( data );

  if( userdata != NULL )
    { userdata->GetCamera()->Capture(); }

}

devOSGCamera::devOSGCamera( const std::string& name, 
			    devOSGWorld* world,
			    int x, int y, int width, int height,
			    double fovy, double aspectRatio,
			    double zNear, double zFar,
			    const std::string& fnname ) :
  mtsTaskContinuous( name ),
  osgViewer::Viewer(),                 // viewer
  x( x ),                              // x position
  y( y ),                              // y position
  width( width ),                      // width of images
  height( height )
#ifdef CISST_STEREOVISION
  ,colorbuffersample( NULL ),
  colorsample( NULL ),
  depthbuffersample( NULL ),
  depthsample( NULL )
#endif
{                    // height of images

  osg::ref_ptr< osg::Camera > camera = getCamera();

  camera->setClearColor( osg::Vec4( 0, 0, 0, 0 ) );
  camera->setProjectionMatrixAsPerspective( fovy,aspectRatio, zNear, zFar );

  setSceneData( world );
  //setUpViewInWindow( x, y, width, height );

  // Set callback stuff
  userdata = new devOSGCamera::UserData( this );
  camera->setUserData( userdata );
  camera->setUpdateCallback( new devOSGCamera::UpdateCallback );

  // drawing callback
  camera->setFinalDrawCallback( new FinalDrawCallback );

  // create a depth image
  zbuffer = new osg::Image;
  zbuffer->allocateImage( width, height, 1, GL_DEPTH_COMPONENT, GL_FLOAT );
  camera->attach( osg::Camera::DEPTH_BUFFER, zbuffer, 0, 0 );

  depthbuffer.SetSize( height, width );  // this is a MxN matrix

  // create a color image
  colorbuffer = new osg::Image;
  colorbuffer->allocateImage( width, height, 1, GL_RGB, GL_UNSIGNED_BYTE );
  camera->attach( osg::Camera::COLOR_BUFFER, colorbuffer, 0, 0 );
  
  rgbbuffer.SetSize( height, width*3 );  // this is a MxN*3 matrix

  // MTS stuff
  if( !fnname.empty() ){
    mtsInterfaceRequired* required;
    required = AddInterfaceRequired( "Transformation", MTS_OPTIONAL );
    if( required != NULL )
      { required->AddFunction( fnname, ReadTransformation ); }

  }

}

devOSGCamera::~devOSGCamera(){
}

void devOSGCamera::Configure( const std::string& CMN_UNUSED( argv ) ) {}

void devOSGCamera::Startup(){
  // The window must be created in the same thread as frame()
  setUpViewInWindow( x, y, width, height );
  /*
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
  colorbuffersample = new svlBufferSample( svlTypeImageRGB );
  colorsample       = new svlSampleImageRGB( false );

  // attach the callback matrices to the sample
  retval = colorsample->SetMatrix( finaldrawcallbacks->GetColorImage(), 0 );
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
  */
}

void devOSGCamera::Run(){
  ProcessQueuedCommands();
  frame();
}

void devOSGCamera::Cleanup(){}

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

void devOSGCamera::Capture(){

  // Convert zbuffer values [0,1] to range data (and flip the image vertically)
  float* z = (float*)zbuffer->data();
  float* Z = depthbuffer.Pointer();
  double fovy, aspectRatio, Zn, Zf;
  getCamera()->getProjectionMatrixAsPerspective( fovy, aspectRatio, Zn, Zf );

  int i=0;
  for( int r=height-1; 0<=r; r-- ){
    for( int c=0; c<width; c++ ){
      Z[ i++ ] = Zn*Zf / (Zf - z[ r*width + c ]*(Zf-Zn));
    }
  }

  // copy the color buffer (and flip the image vertically)
  unsigned char* rgb = (unsigned char*)colorbuffer->data();
  unsigned char* RGB = (unsigned char*)rgbbuffer.Pointer();
  for( int r=0; r<height; r++ ){
    memcpy( RGB + r*width*3,
	    rgb + (height-r-1)*width*3,
	    sizeof(unsigned char)*width*3 );
  }

}
