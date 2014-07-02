

#ifndef _robKinematics_h
#define _robKinematics_h

#include <iostream>

#include <cisstVector/vctFrame4x4.h>
#include <cisstVector/vctMatrixRotation3.h>
#include <cisstRobot/robJoint.h>

#if CISST_HAS_JSON
#include <json/json.h>
#endif

#include <cisstRobot/robExport.h>

class CISST_EXPORT robKinematics : public robJoint {

 public:

  //! Kinematics convention
  /**
   */
  enum Convention
  {
    UNDEFINED,
    STANDARD_DH,
    MODIFIED_DH,
    HAYATI,
    MODIFIED_HAYATI
  };

 private:

  //! The kinematics convention
  robKinematics::Convention convention;

 protected:

  //! Read the parameters of the kinematics convention
  virtual void ReadParameters( std::istream& is ) = 0;
#if CISST_HAS_JSON
  virtual void ReadParameters(const Json::Value &config) = 0;
#endif

  //! Write the parameters of the kinematics convention
  virtual void WriteParameters( std::ostream& os ) const = 0;

 public:

  //! Default constructor
  robKinematics( robKinematics::Convention convention );

  //! Overloaded constructor
  robKinematics( const robJoint& joint , robKinematics::Convention convention );

  virtual ~robKinematics() {}

  //! Return the kinematics convention
  /**
     \return The DH convention: robDHStandard or robDHModified
  */
  robKinematics::Convention GetConvention() const { return convention; }

  //! Read the kinematics parameters and joints parameters
  robKinematics::Errno Read( std::istream& is );
#if CISST_HAS_JSON
  robKinematics::Errno Read(const Json::Value &config);
#endif

  //! Write the kinematics parameters and joints parameters
  robKinematics::Errno Write( std::ostream& os ) const;

  //! Get the position and orientation of the link
  /**
     Returns the position and orientation of the link with respect to the
     proximal link for a given joint vale.
     \param joint The joint associated with the link
     \return The position and orientation associated with the DH parameters
  */
  virtual vctFrame4x4<double> ForwardKinematics( double q ) const = 0;

  //! Get the orientation of the link
  /**
     Returns the orientation of the link with respect to the proximal link for
     a given joint vale.
     \param joint The joint associated with the link
     \return The orientation associated with the DH parameters
  */
  virtual vctMatrixRotation3<double> Orientation( double q ) const = 0;

  //! Return the position of the next (distal) link coordinate frame
  /**
     This method returns the \$XYZ\$ coordinates of the origin of the distal
     link in the coordinate frame of the proximal link. "PStar" is not a good
     name for this but the literature uses \$ \mathbf{p}^* \$ to denote this
     value.
     \return The position of the next coordinate frame wrt to the current
              frame
  */
  virtual vctFixedSizeVector<double,3> PStar() const = 0;

  //! Kinematics objects must be able to clone themselves
  /**
   */
  virtual robKinematics* Clone() const = 0;

  //! Instantiate a kinematic convention by name
  /**
   */
  static robKinematics* Instantiate( const std::string& type );

};

#endif // _robKinematics_h
