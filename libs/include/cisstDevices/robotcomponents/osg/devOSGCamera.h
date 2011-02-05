

#ifndef _devOSGCamera_h
#define _devOSGCamera_h

#include <osgViewer/Viewer>

#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>

#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstMultiTask/mtsTransformationTypes.h>

#include <cisstStereoVision.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devOSGCamera : 

  public mtsTaskContinuous,
  public osgViewer::Viewer {

 protected:

  //! User data for the camera
  /** 
      This class stores a pointer to a camera object. This pointer is is used
      during traversals to capture/process images and update the
      orientation/position of the camera.
  */
  class UserData : public osg::Referenced {
  private:
    //! Pointer to a camera object
    osg::ref_ptr<devOSGCamera> camera;
  public:
    //! Default constructor.
    UserData( devOSGCamera* camera ) : camera( camera ){}
    //! Get the pointer to the camera
    devOSGCamera* GetCamera() { return camera; }
  };
  


  //! Update Callback
  /**
     This callback is used to update the position/orientation of the camera
     during the update traversal.     
  */
  class UpdateCallback : public osg::NodeCallback {
    //! Callback operator
    /**
       This operator is called during the update traversal. It's purpose is to
       update the position/orientation of the camera by calling the Update
       method.
     */
    void operator()( osg::Node* node, osg::NodeVisitor* );
  };


  //! Update the position and orientation of the camera
  /**
     This method can be called to update the position and orientation of
     the camera. It is called from the update callback during the update 
     traversal.
  */
  void Update();

  //! Set the camera position and orientation
  /**
     This method sets the position and orientation of the OSG camera.
     \param Rt A 4x4 frame providing the position and orientation
  */
  void SetMatrix( const vctFrame4x4<double>& Rt );

  //! Get the transformation
  /**
     This MTS function reads the position and orientation of the camera.
     It is called by the Update method during the update traversal.
  */
  mtsFunctionRead ReadTransformation;

    
  // Only enable this if SVL+OpenCV is enabled
#if CISST_SVL_HAS_OPENCV2
    
  //! Final drawing callback
  /**
     This class is used to capture/process images during the final drawing
     callback traversal.
  */
  class FinalDrawCallback : public osg::Camera::DrawCallback {
    
  private:
    
    //! OSG image containing the depth buffer
    osg::ref_ptr<osg::Image> depthbuffer;
    
    //! OSG image containing the color buffer
    osg::ref_ptr<osg::Image> colorbuffer;
    
    //! Image containing depth information of a scene
    cv::Mat                   cvDepthImage;
    vctDynamicMatrix<float>  vctDepthImage;
    
    //! RGB image
    cv::Mat                           cvColorImage;
    vctDynamicMatrix<unsigned char>  vctColorImage;
    
    //! Callback operator
    /**
       This callback is called during the final drawing traversal. This 
       operator calls the Capture method to grab frames from the depth and 
       color buffers.
    */
    virtual void operator () ( osg::RenderInfo& ) const;
    
    //! Convert the depth buffer to a depth image
    void ConvertDepthBuffer( osg::Camera* camera ) const;
    
    //! Convert the color buffer to a color image
    void ConvertColorBuffer( osg::Camera* camera ) const;
    
    //! Is depth buffer process
    bool capturedepth;
    
    //! Process depth buffer?
    bool IsDepthBufferEnabled() const { return capturedepth; }
    
    //! Is color buffer process
    bool capturecolor;
    
    //! Process color buffer?
    bool IsColorBufferEnabled() const { return capturecolor; }
    
  public:
    
    //! Default constructor
    /**
       Constructor for a final drawing callback object. This object will
       determine the operation performed at each frame rendering.
       \param camera A pointer to an OSG camera
       \param capturedepth Read and convert depth buffer
       \param capturecolor Read and convert color buffer
    */
    FinalDrawCallback( osg::Camera* camera, 
		       bool capturedepth = false, 
		       bool capturecolor = false );
    
    ~FinalDrawCallback();
    
    //! Get the depth image of the camera
    /**
       Return the depth image attached to the camera. Note that this image is
       not updated if the callback does not capture the depth buffer.
       \return A pointer to the depth image
    */
    const cv::Mat&                  GetCVDepthImage()  const
    { return cvDepthImage; }
       vctDynamicMatrix<float>*        GetvctDepthImage()
    { return &vctDepthImage; }
    
    //! Get the color image of the camera
    /**
       Return the color image attached to the camera. Note that this image is
       not updated if the callback does not capture the color buffer.
       \return A reference to the color image
    */
    const cv::Mat&                   GetCVColorImage() const 
    {  return cvColorImage; }
    vctDynamicMatrix<unsigned char>& GetvctColorImage()
    { return vctColorImage; }
    
  };

#endif

 public : 

  //! Create an OSG viewer (actually it's a OSG viewer)
  /**
     Create an OSG viewer wrapped in a MTS continuous task. The camera also
     creates an MTS required interface called "Transformation" if a function
     name is provided. This function is used to update the position of the
     camera at each update traversal.
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
     \param fnname The name of a MTS read command the body will connect
     \param trackball Create the default trackball
  */
  devOSGCamera( const std::string& name,
		devOSGWorld* world,
		const std::string& fnname = "",
		bool trackball = true );

  ~devOSGCamera();

  void Configure( const std::string& = "" ){}
  void Startup(){}
  void Run();
  void Cleanup(){}

  //! SVL stuff
#if CISST_SVL_HAS_OPENCV2

 protected:

  svlBufferSample*   depthbuffersample;
  svlBufferSample*   colorbuffersample;

 public:

  svlBufferSample* GetDepthBufferSample() const { return depthbuffersample; }
  svlBufferSample* GetColorBufferSample() const { return colorbuffersample; }

  //virtual const cv::Mat& GetDepthImage( size_t idx = 0 ) const = 0;
  //virtual const cv::Mat& GetColorImage( size_t idx = 0 ) const = 0;

#endif

};

#endif
