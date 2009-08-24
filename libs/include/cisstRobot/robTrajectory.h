#ifndef _robTrajectory_h
#define _robTrajectory_h

#include <cisstRobot/robClock.h>
#include <cisstRobot/robDevice.h>

#include <cisstRobot/robDOF.h>
#include <cisstRobot/robFunction.h>
#include <map>

namespace cisstRobot{

  //! robMapping defines the spaces used by a function.
  /** 
      This class defines which DOF are used by a function as input and which 
      DOF are used as output
  */
  
  class robMapping{
  private:
    
    robDOF domain;
    robDOF codomain;  

  public:
    
    robMapping();
    robMapping( const robDOF& from, const robDOF& to );

    //! Return the domain DOF
    robDOF From() const { return domain; }
    //! Return the image DOF
    robDOF To() const { return codomain; }
    
    //! used by the map
    friend bool operator < ( const robMapping& mapping1, 
			     const robMapping& mapping2 ){
      if( mapping1.domain == mapping2.domain ) {
	if( mapping1.codomain < mapping2.codomain ) return true;
	else return false;
      }      
      if( mapping1.domain < mapping2.domain ) return true;
      else return false;

    }
  };
  
  //! A generic trajectory
  /**
     A trajectory contains a list of robMapping where each mapping is associated
     with a function. Mappings can be added or manipulatated dynamically and
     taylored to specific behaviors. The trajectory must have a device to generate
     an input. If the trajectories are "time-based", the the device must be a
     clock
   */
  class robTrajectory {
    
  protected:
    
    std::map<robMapping, robFunction*> functions;
    robClock*   clock;
    robDevice*  device;
    
  public:

    //! Create a trajectory with the given devide
    robTrajectory(robClock* clock, robDevice* device=NULL);
    
    //! Delete the functions with the given DOF
    robError Erase( size_t dof );

    //! Insert a function in the trajectory
    /**
       \param function The function to insert
       \param indof The input DOF
       \param outdof The output DOF
    */
    robError Insert( robFunction* function, uint64_t indof, uint64_t outdof );
    
    //! Create a linear 1D trajectory
    /** 
	Create a 1D trajectory between \f$(ti,qi)\f$ and \f$(tf,qf)\f$ for
	the given DOF.
	\param ti The start time of the trajectory
	\param qi The initial value at ti
	\param tf The final time of the trajectory
	\param qf The final value at tf
	\param dof The DOF
	\param sticky Add a constant function at tf
    */
    robError Linear( real ti, real qi, 
		     real tf, real qf, 
		     uint64_t dof, 
		     bool sticky=true );

    //! Create a linear 1D trajectory
    /** 
	Create a 1D trajectory between \f$(ti,qi)\f$ and \f$(tf,qf)\f$ for
	the given DOF.
	\param qi The initial value at ti
	\param qf The final value at tf
	\param vmax The maximum velocity
	\param dof The DOF
	\param sticky Add a constant function at tf
    */
    robError Linear( real qi, real qf, real vmax, uint64_t dof, bool sticky=true );
    
    //! Create a linear nD trajectory
    /** 
	Create a nD trajectory between \f$(ti,qi)\f$ and \f$(tf,qf)\f$ for
	the given DOF. All the DOF are synchronized
	\param ti The start time of the trajectory
	\param qi The initial values at ti
	\param tf The final time of the trajectory
	\param qf The final values at tf
	\param dof The DOF
	\param sticky Add a constant function at tf
    */
    robError Linear( real ti, const Rn& qi, 
		     real tf, const Rn& qf, 
		     uint64_t dof, 
		     bool sticky=true );
    
    //! Create a linear nD trajectory
    /** 
	Create a nD trajectory between \f$(ti,qi)\f$ and \f$(tf,qf)\f$ for
	the given DOF. All the DOF are synchronized
	\param qi The initial values at ti
	\param qf The final values at tf
	\param vmax The maximum velocity for all DOF
	\param dof The DOF
	\param sticky Add a constant function at tf
    */
    robError Linear( const Rn& qi, const Rn& qf, real vmax, 
		     uint64_t dof, bool sticky=true );
    
    
    //! Create a linear Cartesian trajectory
    /** 
	Create a SE3 trajectory between \f$(ti,Rti)\f$ and \f$(tf,Rtf)\f$ for
	the given DOF. The translation is decoupled from the rotation but both
	are synchronized.
	\param ti The start time of the trajectory
	\param Rti The initial values at ti
	\param tf The final time of the trajectory
	\param Rtf The final values at tf
	\param dof The DOF
	\param sticky Add a constant function at tf
    */
    robError Linear( real ti, const SE3& Rti, 
		     real tf, const SE3& Rtf, 
		     uint64_t dof, 
		     bool sticky=true );
    
    //! Create a linear Cartesian trajectory
    /** 
	Create a SE3 trajectory between \f$Rti\f$ and \f$Rtf\f$ for the given DOF. 
	The translation is decoupled from the rotation but both	are synchronized.
	\param Rti The initial values at ti
	\param Rtf The final values at tf
	\param vmax The maximum linear velocity
	\param wmax The maximum angular velocity
	\param dof The DOF
	\param sticky Add a constant function at tf
    */
    robError Linear( const SE3& Rti, const SE3& Rtf, real vmax, real wmax,
		     uint64_t dof, bool sticky=true );
    
    //! Create a linear Cartesian trajectory
    /** 
	Create SE3 trajectories between each element of the vector \f$Rt\f$ for
	the given DOF. The translation is decoupled from the rotation but both
	are synchronized.
	\param Rt The vector of SE3
	\param vmax The maximum linear velocity
	\param wmax The maximum angular velocity
	\param dof The DOF
	\param sticky Add a constant function at the end
    */
    robError Linear( const std::vector<SE3>& Rt, 
		     real vmax, real wmax,
		     uint64_t dof, bool sticky=true );
    
    //! Create a 1D sigmoid trajectory
    /** 
	Create a nD trajectory between \f$(ti,qi)\f$ and \f$(tf,qf)\f$ for
	the given DOF.
	\param ti The start time of the trajectory
	\param yi The initial values at ti
	\param tf The final time of the trajectory
	\param yf The final values at tf
	\param dof The DOF
	\param sticky Add a constant function at tf
    */
    robError Sigmoid( real ti, real yi,
		      real tf, real yf,
		      uint64_t dof, 
		      bool sticky=true );
    
    //! Create a 1D sigmoid trajectory
    /** 
	Create a nD trajectory between \f$(ti,qi)\f$ and \f$(tf,qf)\f$ for
	the given DOF.
	\param yi The initial values at ti
	\param yf The final values at tf
	\param vmax The maximum velocity
	\param dof The DOF
	\param sticky Add a constant function at tf
    */
    robError Sigmoid( real yi, real yf, real vmax, uint64_t dof, bool sticky=true);
    
    //! Create a nD sigmoid trajectory
    /** 
	Create a nD trajectory between \f$(ti,qi)\f$ and \f$(tf,qf)\f$ for
	the given DOF. This creates N 1D sigmoids. All the sigmoids are 
	synchronized.
	\param ti The start time of the trajectory
	\param yi The initial values at ti
	\param tf The final time of the trajectory
	\param yf The final values at tf
	\param dof The DOF
	\param sticky Add a constant function at tf
    */
    robError Sigmoid( real ti, const Rn& yi, 
		      real tf, const Rn& yf, 
		      uint64_t dof, 
		      bool sticky=true );
    
    //! Create a nD sigmoid trajectory
    /** 
	Create a nD trajectory between \f$(ti,qi)\f$ and \f$(tf,qf)\f$ for
	the given DOF. This creates N 1D sigmoids. All the sigmoids are 
	synchronized.
	\param yi The initial values at ti
	\param yf The final values at tf
	\param vmax The maximum velocity for each DOF
	\param dof The DOF
	\param sticky Add a constant function at tf
    */
    robError Sigmoid( const Rn& yi, const Rn& yf, const Rn& vmax, uint64_t dof, 
		      bool sticky=true );
    
    //! Create a linear 3D translation
    /**
	\param ti The start time of the trajectory
	\param Rt The initial values at ti
	\param tf The final time of the trajectory
	\param Rtf The final values at tf
	\param dof The DOF
	\param sticky Add a constant function at tf
    */
    robError Translation( real ti, const SE3& Rti,
			  real tf, const SE3& Rtf,
			  uint64_t dof,
			  bool sticky );
    
    //! Create a linear 3D rotation
    /**
	\param ti The start time of the trajectory
	\param Rt The initial values at ti
	\param tf The final time of the trajectory
	\param Rtf The final values at tf
	\param dof The DOF
	\param sticky Add a constant function at tf
    */
    robError Rotation( real ti, const SE3& Rti,
		       real tf, const SE3& Rtf,
		       uint64_t dof,
		       bool sticky );

    //! Create a position tracking trajectory
    /**
	\param dof The DOF
	\param vmax The maximum linear velocity
	\param vmax The maximum angular velocity
	\param vmax The maximum linear acceleration
    */
    robError SE3Track( uint64_t dof, real vmax=-1, real wmax=-1, real vdmax=-1 );
    
    //! remove all the functions
    void Clear();
    
    //! evaluate the trajectory
    robError Evaluate( robDOF& output );
    
  };
  
}
#endif
