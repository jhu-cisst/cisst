

#ifndef _devOSGStereo_h
#define _devOSGStereo_h

#include <osgViewer/Viewer>

#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>

#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstMultiTask/mtsTransformationTypes.h>

#include <cisstStereoVision.h>
#include <cisstDevices/devExport.h>

#define CISST_STEREOVISION

class CISST_EXPORT devOSGStereo : 

  public mtsTaskContinuous,
  public osgViewer::Viewer {

 private:

  // this is to store a pointer to a stereo object
  class UserData : public osg::Referenced {
  private:
    osg::ref_ptr<devOSGStereo> stereo;
  public:
    UserData( devOSGStereo* stereo ) : stereo( stereo ){}
    devOSGStereo* GetStereo() { return stereo; }
  };
  osg::ref_ptr<UserData> userdata;


  
  // This is an update callback 
  class UpdateCallback : public osg::NodeCallback {
    // This is called after each update traversal
    void operator()( osg::Node* node, osg::NodeVisitor* );
  };
  osg::ref_ptr<UpdateCallback> callback;

  // this is called to update the position of the stereo
  void Update();


  // This is a callback class to capture the depth buffer of the stereo
  class FinalDrawCallback : public osg::Camera::DrawCallback{ 
  private:
    // These 2 images are "attached" to the camera
    osg::ref_ptr<osg::Image> depthbuffer;
    osg::ref_ptr<osg::Image> colorbuffer;    
    // These 2 images are for human consumption
    vctDynamicMatrix<float>         depthimage;
    vctDynamicMatrix<unsigned char> colorimage;

    // This is called at each iteration
    virtual void operator () ( osg::RenderInfo& ) const;

    void ConvertDepthBuffer( osg::Camera* camera ) const;
    void ConvertColorBuffer( osg::Camera* camera ) const;

    bool IsDepthBufferEnabled() const { return drawdepth; }
    bool IsColorBufferEnabled() const { return drawcolor; }

    bool drawdepth;
    bool drawcolor;

  public:

    FinalDrawCallback( osg::Camera* camera, 
		       bool drawdepth = false, 
		       bool drawcolor = false );


    vctDynamicMatrix<float>* GetDepthImage()
    { return &depthimage; }

    const vctDynamicMatrix<float>* GetDepthImage() const
    { return &depthimage; }

    vctDynamicMatrix<unsigned char>& GetColorImage()
    { return colorimage; }

    const vctDynamicMatrix<unsigned char>& GetColorImage() const 
    { return colorimage; }

  };
  // Two draw callbacks. One for each camera
  osg::ref_ptr<FinalDrawCallback> finaldrawcallbacks[2];

  int x, y;
  int width;
  int height;
  double baseline;

#ifdef CISST_STEREOVISION
  // svl
  svlBufferSample* colorbuffersample;
  svlSampleImageRGBStereo* colorsample;

  svlBufferSample* depthbuffersample;
  svlSampleMatrixFloat* depthsample;
#endif

 public : 
  
  //! Create a stereo OSG viewer (actually it's a OSG viewer)
  /**
     Create an stereo OSG viewer wrapped in a MTS continuous task. The stereo 
     also creates an MTS required interface called "Transformation" if a 
     function name is provided. This function is used to update the position of 
     the camera at each update traversal.
     \param name The name of the camera
     \param world The world the camera belongs to
     \param x The X offset of the camera window
     \param y The Y offset of the camera window
     \param width The width of the camera image
     \param height The height of the camera image
     \param fovy The field of view angle
     \param aspectRatio The aspect ratio of the camera
     \param zNear The near buffer distance
     \param zFar  The far buffer distance
     \param baseline The base line between the two cameras
  */
  devOSGStereo( const std::string& name,
		devOSGWorld* world,
		int x, int y, int width, int height,
		double fovy, double aspectRatio,
		double zNear, double zFar,
		double baseline );
  ~devOSGStereo( );

  void Configure( const std::string& argv = "" );

  void Startup();
  void Run();
  void Cleanup();

#ifdef CISST_STEREOVISION
  svlBufferSample* GetDepthBufferSample() const ;
  svlBufferSample* GetColorBufferSample() const ;
#endif

};

#endif
