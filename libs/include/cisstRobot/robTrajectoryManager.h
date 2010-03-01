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

#ifndef _robTrajectoryManager_h
#define _robTrajectoryManager_h

#include <list>

#include <cisstRobot/robTrajectory.h>
#include <cisstRobot/robFunction.h>
#include <cisstRobot/robExport.h>

//! A trajectory manager
/**
   A trajectory contains a list of mappings. Mappings can be added or changed
   dynamically. Each mapping represents a function that acts on one or several
   degrees of freedom.
*/
class CISST_EXPORT robTrajectoryManager : public robFunction {

 protected:
  
  std::list<robTrajectory*> trajectories;
  
  //!
  robTrajectory* FindTrajectory( const robMapping& mapping );

 public:

  //! Create a trajectory manager with initial joint space configuration
  /**
  */
  robTrajectoryManager( const robSpace& space,
			double t,
			const vctDynamicVector<double>& q,
			const vctDynamicVector<double>& qdmax,
			const vctDynamicVector<double>& qddmax);
  
  //! Create a trajectory manager with initial Cartesian space configuration
  /**
  */
  robTrajectoryManager( const robSpace& space,
			double t,
			const vctFrame4x4<double>& Rt,
			double vmax, double vdmax,
			double wmax, double wdmax );
  
  //! Remove all the functions
  void Clear();

  ///// CONSTANT /////
  //! Create a constant trajectory segment
  /**
     Create a constant \f$R^1\f$ trajectory for the given space. The constant 
     value is determined from the last motion segment and that the constant
     lasts to infinity.
     \param space The space for this trajectory
     \return robFunction::ESUCCESS if the segment was created properly.
   */
  robFunction::Errno Constant( const robSpace& space );

  ///// LINEAR VECTOR /////

  //! Create a \f$R^1\f$ linear trajectory segment
  /**
     Create a \f$R^1\f$ trajectory for the given space. It is assumed that the 
     space contains only one basis.
     \param space The space for this trajectory
     \param y The final values
     \param ydmax The maximum velocity for the basis
     \return robFunction::ESUCCESS if the segment was created properly.
  */
  robFunction::Errno Linear( const robSpace& space,
			     const double y, 
			     const double ydmax );

  //! Create a \f$R^n\f$ linear trajectory segment
  /**
     Create a \f$R^n\f$ trajectory for the given space. The velocity of each 
     basis is specified in ydmax. For joint space, the vectors \f$y\f$ and 
     \f$ydmax\f$ must have the same length and the vectors must be sufficiently
     large to contain all the basis in the space. For example, if robSpace::Q7 is
     in the space, then the vectors must be at least 7 long. For Cartesian 
     positions then \f$y \in R^3\f$ and \f$ydmax \in R^3\f$.
     \param space The space for this trajectory
     \param y The final values
     \param ydmax The maximum velocity for each basis
     \return robFunction::ESUCCESS if the segment was created properly.
  */
  robFunction::Errno Linear( const robSpace& space,
			     const vctDynamicVector<double>& y, 
			     const vctDynamicVector<double>& ydmax );

  //! Create a sequence of \f$R^n\f$ linear trajectory segments
  /**
     Create a sequence of \f$R^n\f$ motion segments for the given space.  The 
     velocity of each basis for each segment is specified in ydmax. For joint 
     space, the vectors \f$y\f$ and \f$ydmax\f$ must have the same length and 
     each vector must be sufficiently large to contain all the basis in the 
     space. For example, if robSpace::Q7 is in the space, then the vectors must 
     be at least 7 long. For Cartesian positions, each segment \f$i\f$ 
     \f$y_i \in R^3\f$ and \f$ydmax_i \in R^3\f$.
     \param space The space for this trajectory
     \param y The final values at the end of each segment
     \param ydmax The maximum velocity for each basis during each segment
     \return robFunction::ESUCCESS if the segment was created properly.
  */
  robFunction::Errno Linear(const robSpace& space,
			    const std::vector<vctDynamicVector<double> >& y, 
			    const std::vector<vctDynamicVector<double> >& ydmax);


  ///// SIGMOID VECTORS /////
  
  //! Create a \f$R^1\f$ sigmoid trajectory segment
  /**
     Create a \f$R^1\f$ trajectory for the given space. It is assumed that the 
     space contains only one basis.
     \param space The space for this trajectory
     \param y The final values
     \param ydmax The maximum velocity for the basis
     \return robFunction::ESUCCESS if the segment was created properly.
  */
  robFunction::Errno Sigmoid( const robSpace& space,
			      const double y, 
			      const double ydmax );

  //! Create a \f$R^n\f$ sigmoid trajectory segment
  /**
     Create a \f$R^n\f$ trajectory for the given space. The velocity of each 
     basis is specified in ydmax. For joint space, the vectors \f$y\f$ and 
     \f$ydmax\f$ must have the same length and the vectors must be sufficiently
     large to contain all the basis in the space. For example, if robSpace::Q7 is
     in the space, then the vectors must be at least 7 long. For Cartesian 
     positions then \f$y \in R^3\f$ and \f$ydmax \in R^3\f$.
     \param space The space for this trajectory
     \param y The final values
     \param ydmax The maximum velocity for each basis
     \return robFunction::ESUCCESS if the segment was created properly.
  */
  robFunction::Errno Sigmoid( const robSpace& space,
			      const vctDynamicVector<double>& y, 
			      const vctDynamicVector<double>& ydmax );

  //! Create a sequence of \f$R^n\f$ sigmoid trajectory segments
  /**
     Create a sequence of \f$R^n\f$ motion segments for the given space.  The 
     velocity of each basis for each segment is specified in ydmax. For joint 
     space, the vectors \f$y\f$ and \f$ydmax\f$ must have the same length and 
     each vector must be sufficiently large to contain all the basis in the 
     space. For example, if robSpace::Q7 is in the space, then the vectors must 
     be at least 7 long. For Cartesian positions, each segment \f$i\f$ 
     \f$y_i \in R^3\f$ and \f$ydmax_i \in R^3\f$.
     \param space The space for this trajectory
     \param y The final values at the end of each segment
     \param ydmax The maximum velocity for each basis during each segment
     \return robFunction::ESUCCESS if the segment was created properly.
  */
  robFunction::Errno Sigmoid(const robSpace& space,
			     const std::vector<vctDynamicVector<double> >& y, 
			     const std::vector<vctDynamicVector<double> >& ydmax);

  ///// LINEAR CARTESIAN /////

  //! Create a linear translation segment
  /**
     Create a \f$R^3\f$ trajectory in Cartesian space. The linear velocity is
     specified by vmax.
     \param space The space for this trajectory. This should be a combinaison of
            robSpace::TX, robSpace::TY and robSpace::TZ.
     \param t The final Cartesian position
     \param vmax The magnitude of the linear velocity
     \return robFunction::ESUCCESS if the segment was created properly.
  */
  robFunction::Errno Linear( const robSpace& space,
			     const vctFixedSizeVector<double,3>& t,
			     double vmax );

  //! Create a SLERP segment
  /**
     Create a \f$SO(3)\f$ trajectory in Cartesian space. The angular velocity is
     specified by wmax.
     \param space The space for this trajectory. This should be 
                  robSpace::ORIENTATION
     \param R The final Cartesian orientation
     \param wmax The magnitude of the angular velocity
     \return robFunction::ESUCCESS if the segment was created properly.
  */
  robFunction::Errno Linear( const robSpace& space,
			     const vctMatrixRotation3<double>& R,
			     double wmax );
     
  //! Create a \f$SE(3)\f$ linear trajectory segment
  /**
     Create a \f$SE(3)\f$ trajectory for the given space. The linear velocity
     is specified by vmax and the angular velocity is specified by wmax. 
     \param space The space for this trajectory
     \param Rt The final position and orientation
     \param vmax The magnitude of the linear velocity
     \param wmax The magnitude of the angular velocity
     \return robFunction::ESUCCESS if the segment was created properly.
  */
  robFunction::Errno Linear( const robSpace& space,
			     const vctFrame4x4<double>& Rt, 
			     double vmax, 
			     double wmax );
  
  //! Create a sequence of \f$SE(3)\f$ linear trajectory segments
  /**
     Create a sequence of \f$SE(3)\f$ motion segments for the given space.  The 
     liinear velocity is specified by vmax and the angular velocity is specified 
     by wmax.          
     \param space The space for this trajectory
     \param Rt The final position and orientation
     \param vmax The magnitude of the linear velocity
     \param wmax The magnitude of the angular velocity
     \return robFunction::ESUCCESS if the segment was created properly.
  */
  robFunction::Errno Linear( const robSpace& space,
			     const std::vector< vctFrame4x4<double> >& Rt, 
			     const std::vector<double>& vmax, 
			     const std::vector<double>& wmax );
    

  ///// SIGMOID CARTESIAN /////

  //! Create a sigmoid translation segment
  /**
     Create a \f$R^3\f$ trajectory in Cartesian space. The linear velocity is
     specified by vmax.
     \param space The space for this trajectory. This should be a combinaison of
            robSpace::TX, robSpace::TY and robSpace::TZ.
     \param t The final Cartesian position
     \param vmax The magnitude of the linear velocity
     \return robFunction::ESUCCESS if the segment was created properly.
  */
  robFunction::Errno Sigmoid( const robSpace& space,
			      const vctFixedSizeVector<double,3>& t,
			      double vmax );

  //! Create a \f$SE(3)\f$ sigmoid trajectory segment
  /**
     Create a \f$SE(3)\f$ trajectory for the given space. The linear velocity
     is specified by vmax and the angular velocity is specified by wmax. 
     \param space The space for this trajectory
     \param Rt The final position and orientation
     \param vmax The magnitude of the linear velocity
     \param wmax The magnitude of the angular velocity
     \return robFunction::ESUCCESS if the segment was created properly.
  */
  robFunction::Errno Sigmoid( const robSpace& space,
			      const vctFrame4x4<double>& Rt, 
			      double vmax, 
			      double wmax );
  
  //! Create a sequence of \f$SE(3)\f$ sigmoid trajectory segments
  /**
     Create a sequence of \f$SE(3)\f$ motion segments for the given space.  The 
     liinear velocity is specified by vmax and the angular velocity is specified 
     by wmax.          
     \param space The space for this trajectory
     \param Rt The final position and orientation
     \param vmax The magnitude of the linear velocity
     \param wmax The magnitude of the angular velocity
     \return robFunction::ESUCCESS if the segment was created properly.
  */
  robFunction::Errno Sigmoid( const robSpace& space,
			      const std::vector< vctFrame4x4<double> >& Rt, 
			      const std::vector<double>& vmax, 
			      const std::vector<double>& wmax );
    
  //////////////

  //! Not used
  robFunction::Context GetContext( const robVariable& input ) const; 

  //! Evaluate the trajectory
  robFunction::Errno Evaluate( const robVariable& input, robVariable& output );
  
};

#endif
