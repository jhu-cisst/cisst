
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

#ifndef _cisstOSGBody_h
#define _cisstOSGBody_h

#include <osg/Group>
#include <osg/Switch>
#include <osg/MatrixTransform>

#include <cisstVector/vctTransformationTypes.h>

#include <cisstOSG/cisstOSGWorld.h>
#include <cisstOSG/cisstOSGExport.h>

class CISST_EXPORT cisstOSGBody : public osg::Group {

 public:

  enum Switch{ SWITCH_OFF, SWITCH_ON };

 protected:

  // Callback stuff

  // This is to store a pointer to the body
  class UserData : public osg::Referenced {
  private:
    osg::ref_ptr<cisstOSGBody> body;
  public:
    UserData( cisstOSGBody* body ) : body( body ){}
    cisstOSGBody* GetBody() { return body; }
  };
  osg::ref_ptr<UserData> userdata;

  // This is used to update the position of the body
  class TransformCallback : public osg::NodeCallback {    
  public:
    TransformCallback(){}
    void operator()( osg::Node* node, osg::NodeVisitor* nv );
  };
  // The transform callback
  osg::ref_ptr<TransformCallback> transformcallback;

  //! This method is called from the transform callback
  virtual void UpdateTransform();

  // The vct transform
  vctFrame4x4<double> transform;

  // The osg transform
  osg::ref_ptr<osg::MatrixTransform> osgtransform;


  
  // This is used to update the position of the body
  class SwitchCallback : public osg::NodeCallback {    
  public:
    SwitchCallback(){}
    void operator()( osg::Node* node, osg::NodeVisitor* nv );
  };
  // The switch callback
  osg::ref_ptr<SwitchCallback> switchcallback;

  //! This method is called from the switch callback
  virtual void UpdateSwitch();

  // The switch
  Switch onoff;

  // The switch
  osg::ref_ptr< osg::Switch> osgswitch;



  // I/O
  void ReadModel( const std::string& fname );

  void Read3DData( const vctDynamicMatrix<double>& pc,
		   const vctFixedSizeVector<unsigned char,3>& RGB = 
		   vctFixedSizeVector<unsigned char,3>( 200, 200, 200 ) );

 public: 

  static const vctFixedSizeVector<unsigned char,3> RGBDEFAULT;

  //! OSG Body constructor
  /**
     Create a OSG body component. The body will add a required interface *if*
     a function name is passed.
     \param model The file name of a 3D model
     \param world The OSG world the body belongs to
     \param Rt The initial transformation of the body
  */
  cisstOSGBody( const std::string& model,
		cisstOSGWorld* world,
		const vctFrame4x4<double>& Rt );

  //! OSG Body constructor
  /**
     Create a OSG body component. The body will add a required interface *if*
     a function name is passed.
     \param model The file name of a 3D model
     \param world The OSG world the body belongs to
     \param Rt The initial transformation of the body
  */
  cisstOSGBody( const std::string& model,
		cisstOSGWorld* world,
		const vctFrm3& Rt );

  //! Construcor for 3D point cloud
  cisstOSGBody( const vctDynamicMatrix<double>& pointcloud,
		cisstOSGWorld* world,
	        const vctFrm3& Rt,
		const vctFixedSizeVector<unsigned char,3>& rgb=RGBDEFAULT );

  ~cisstOSGBody();

  void Initialize();

  //! Set the transform of the body
  void SetTransform( const vctFrame4x4<double>& Rt );
  void SetTransform( const vctFrm3& Rt );

  //! Set the switch of the body
  void SwitchOn();
  void SwitchOff();
  
  void SetModeLine();
  void SetModePoint();
  void SetModeFill();
};

#endif
