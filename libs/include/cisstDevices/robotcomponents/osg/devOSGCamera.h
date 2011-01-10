

#ifndef _devOSGCamera_h
#define _devOSGCamera_h

#include <osgViewer/Viewer>

#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>

#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstMultiTask/mtsTransformationTypes.h>

#include <cisstDevices/devExport.h>

class CISST_EXPORT devOSGCamera : 

  public mtsTaskContinuous,

  public osgViewer::Viewer {

 private:


  // this is to store a pointer to a camera object
  class UserData : public osg::Referenced {
  private:
    osg::ref_ptr<devOSGCamera> camera;
  public:
    UserData( devOSGCamera* camera ) : camera( camera ){}
    devOSGCamera* GetCamera() { return camera; }
  };
  osg::ref_ptr<UserData> userdata;

  // This is an update callback 
  class UpdateCallback : public osg::NodeCallback {
    // This is called after each update traversal
    void operator()( osg::Node* node, osg::NodeVisitor* );
  };
  osg::ref_ptr<UpdateCallback> callback;

  // This method is called from the callback
  void Update();

  // Set the OSG matrix
  void SetMatrix( const vctFrame4x4<double>& Rt );

  // Get the transformation from somewhere
  mtsFunctionRead ReadTransformation;

  // This is a callback class to capture the depth buffer of the camera
  class FinalDrawCallback : public osg::Camera::DrawCallback {
  private:
    virtual void operator () ( osg::RenderInfo& ) const;
  public:
  };
  void Capture();

  osg::ref_ptr<osg::Image> zbuffer;
  osg::ref_ptr<osg::Image> colorbuffer;

  vctDynamicMatrix<float>          depthbuffer;
  vctDynamicMatrix<unsigned char>  rgbbuffer;

  int width;
  int height;

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
  */
  devOSGCamera( const std::string& name,
		devOSGWorld* world,
		int x, int y, int width, int height,
		double fovy, double aspectRatio,
		double zNear, double zFar,
		const std::string& fnname = "" );

  ~devOSGCamera( );

  void Configure( const std::string& argv = "" );

  void Startup();
  void Run();
  void Cleanup();

};

#endif
