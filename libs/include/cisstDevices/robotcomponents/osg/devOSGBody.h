
/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _devOSGBody_h
#define _devOSGBody_h

#include <osg/Geometry>
#include <osg/Switch>
#include <osg/MatrixTransform>

#include <cisstVector/vctFrame4x4.h>
#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsTransformationTypes.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devOSGBody : 
  public mtsComponent,
  public osg::MatrixTransform {

 protected:

  // Callback stuff

  // This is to store a pointer to the body
  class UserData : public osg::Referenced {
  private:
    osg::ref_ptr<devOSGBody> body;
  public:
    UserData( devOSGBody* body ) : body( body ){}
    devOSGBody* GetBody() { return body; }
  };
  osg::ref_ptr<UserData> userdata;

  // This is used to update the position of the body
  class TransformCallback : public osg::NodeCallback {    
  public:
    TransformCallback(){}
    void operator()( osg::Node* node, osg::NodeVisitor* nv );
  };

  // This is used to update the position of the body
  class SwitchCallback : public osg::NodeCallback {    
  public:
    SwitchCallback(){}
    void operator()( osg::Node* node, osg::NodeVisitor* nv );
  };

  //! This method is called from the transform callback
  virtual void Transform();

  //! This method is called from the switch callback
  virtual void Switch();

  //! Set the OSG matrix
  /**
     Convert the vctFrame4x4 matrix to the osg transform matrix
  */
  void SetMatrix( const vctFrame4x4<double>& Rt );

  // Get the transformation from somewhere else
  mtsFunctionRead ReadTransformation;
  vctFrame4x4<double> Rt_body;

  // Get the switch status from somewhere else
  mtsFunctionRead ReadSwitch;
  bool switch_body;

  // The switch
  osg::ref_ptr< osg::Switch > osgswitch;
  
  // A vector of geometries
  std::vector<osg::Geometry*> osggeometries;


  void CreateInterface( const std::string& transformfn, 
			const std::string& switchfn );
  void ReadModel( const std::string& fname );
  void Read3DData( const vctDynamicMatrix<double>& pc );

 public: 

  //! OSG Body constructor
  /**
     Create a OSG body component. The body will add a required interface *if*
     a function name is passed. In this case the interface is called 
     "Transformation" and the body will read the function at each update 
     traversal.
     \param name The name of the body/component
     \param Rt The initial transformation of the body
     \param model The file name of a 3D model
     \param world The OSG world the body belongs to
     \param transformfn The name of a MTS read command the body will connect
  */
  devOSGBody( const std::string& name, 
	      const vctFrame4x4<double>& Rt,
	      const std::string& model,
	      devOSGWorld* world,
	      const std::string& transformfn = "",
	      const std::string& switchfn = "" );

  //! OSG Body constructor
  /**
     Create a OSG body component. The body will add a required interface *if*
     a function name is passed. In this case the interface is called 
     "Transformation" and the body will read the function at each update 
     traversal.
     \param name The name of the body/component
     \param Rt The initial transformation of the body
     \param model The file name of a 3D model
     \param world The OSG world the body belongs to
     \param transformfn The name of a MTS read command the body will connect
  */
  devOSGBody( const std::string& name, 
	      const vctFrm3& Rt,
	      const std::string& model,
	      devOSGWorld* world,
	      const std::string& transformfn = "",
	      const std::string& switchfn = "" );

  //! Construcor for 3D point cloud
  devOSGBody( const std::string& name, 
              const vctFrm3& Rt,
	      const vctDynamicMatrix<double>& pc,
	      devOSGWorld* world,
              const std::string& transformfn = "",
	      const std::string& switchfn = "" );

  ~devOSGBody();

  //! Set the transform of the body
  void SetTransform( const vctFrame4x4<double>& Rt );

  //! Set the switch of the body
  void SetSwitch( bool onoff );
  
};

#endif
