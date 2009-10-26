#ifndef _robTrajectoryManager_h
#define _robTrajectoryManager_h

#include <cisstRobot/robClock.h>
#include <cisstRobot/robSource.h>
#include <cisstRobot/robTrajectory.h>

#include <map>

//! robMapping defines the spaces used by a function.
/** 
    This class defines which DOF are used by a function as input and which 
    DOF are used as output
*/
class robMapping{
private:
  
  robVariables domain;
  robVariables codomain;  

public:
    
  robMapping();
  robMapping( const robVariables& from, const robVariables& to );
  
  //! Return the domain DOF
  robVariables From() const { return domain; }
  //! Return the image DOF
  robVariables To()   const { return codomain; }
  
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

//! A trajectory manager
/**
   A trajectory contains a list of robMapping where each mapping is associated
   with a function. Mappings can be added or manipulatated dynamically and
   taylored to specific behaviors. The trajectory must have a source to generate
   an input. If the trajectories are "time-based", the the source must be a
   clock
*/
class robTrajectoryManager {
  
protected:
  
  std::map<robMapping, robTrajectory*> trajectories;
  robClock*   clock;
  robSource* source;
  
public:
  
  //! Create a trajectory with the given devide
  robTrajectoryManager(robClock* clock, robSource* source=NULL);
  
  //! Delete the functions with the given DOF
  robError Erase( size_t dof );
  
  //! Insert a function in the manager
  /**
     \param function The function to insert
     \param indof The input variables
     \param outdof The output variables
  */
  robError Insert( robFunction* function, 
		   robVariablesMask indof, 
		   robVariablesMask outdof );
  
  //! Create a linear 1D trajectory
  /** 
      Create a 1D trajectory between \f$(ti,qi)\f$ and \f$(tf,qf)\f$ for
      the given DOF.
      \param ti The start time of the trajectory
      \param qi The initial value at ti
      \param tf The final time of the trajectory
      \param qf The final value at tf
      \param variables A mask of output variables
      \param sticky Add a constant function at tf
  */
  robError Linear( double ti, double qi, 
		   double tf, double qf, 
		   robVariablesMask variables, 
		   bool sticky=true );
  
  //! Create a linear 1D trajectory
  /** 
      Create a 1D trajectory between \f$(ti,qi)\f$ and \f$(tf,qf)\f$ for
      the given DOF.
      \param qi The initial value at ti
      \param qf The final value at tf
      \param vmax The maximum velocity
      \param variables A mask of output variables
      \param sticky Add a constant function at tf
  */
  robError Linear( double qi, double qf, 
		   double vmax, 
		   robVariablesMask variables, 
		   bool sticky=true );
  
  //! Create a linear nD trajectory
  /** 
      Create a nD trajectory between \f$(ti,qi)\f$ and \f$(tf,qf)\f$ for
      the given DOF. All the DOF are synchronized
      \param ti The start time of the trajectory
      \param qi The initial values at ti
      \param tf The final time of the trajectory
      \param qf The final values at tf
      \param variables A mask of output variables
      \param sticky Add a constant function at tf
  */
  robError Linear( double ti, const vctDynamicVector<double>& qi, 
		   double tf, const vctDynamicVector<double>& qf, 
		   robVariablesMask variables, 
		   bool sticky=true );
  
  //! Create a linear nD trajectory
  /** 
      Create a nD trajectory between \f$(ti,qi)\f$ and \f$(tf,qf)\f$ for
      the given DOF. All the DOF are synchronized
      \param qi The initial values at ti
      \param qf The final values at tf
      \param vmax The maximum velocity for all DOF
      \param variables A mask of output variables
      \param sticky Add a constant function at tf
    */
  robError Linear( const vctDynamicVector<double>& qi, 
		   const vctDynamicVector<double>& qf, 
		   double vmax, 
		   robVariablesMask variables, 
		   bool sticky=true );
  
  //! Create a linear Cartesian trajectory
  /** 
      Create a SE3 trajectory between \f$(ti,Rti)\f$ and \f$(tf,Rtf)\f$ for
      the given DOF. The translation is decoupled from the rotation but both
      are synchronized.
      \param ti The start time of the trajectory
      \param Rti The initial values at ti
      \param tf The final time of the trajectory
      \param Rtf The final values at tf
      \param variables A mask of output variables
      \param sticky Add a constant function at tf
  */
  robError Linear( double ti, const vctFrame4x4<double,VCT_ROW_MAJOR>& Rti, 
		   double tf, const vctFrame4x4<double,VCT_ROW_MAJOR>& Rtf, 
		   robVariablesMask variables, 
		   bool sticky=true );
  
  //! Create a linear Cartesian trajectory
  /** 
      Create a SE3 trajectory between \f$Rti\f$ and \f$Rtf\f$ for the given DOF. 
      The translation is decoupled from the rotation but both	are synchronized.
      \param Rti The initial values at ti
      \param Rtf The final values at tf
      \param vmax The maximum linear velocity
      \param wmax The maximum angular velocity
      \param variables A mask of output variables
      \param sticky Add a constant function at tf
  */
  robError Linear( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rti, 
		   const vctFrame4x4<double,VCT_ROW_MAJOR>& Rtf, 
		   double vmax, double wmax,
		   robVariablesMask variables, 
		   bool sticky=true );
  
  //! Create a linear Cartesian trajectory
  /** 
      Create SE3 trajectories between each element of the vector \f$Rt\f$ for
      the given DOF. The translation is decoupled from the rotation but both
      are synchronized.
      \param Rt The vector of vctFrame4x4<double,VCT_ROW_MAJOR>
      \param vmax The maximum linear velocity
      \param wmax The maximum angular velocity
      \param variables A mask of output variables
      \param sticky Add a constant function at the end
  */
  robError Linear( const std::vector< vctFrame4x4<double,VCT_ROW_MAJOR> >& Rt, 
		   double vmax, double wmax,
		   robVariablesMask variables, 
		   bool sticky=true );
  
  //! Create a 1D sigmoid trajectory
  /** 
      Create a nD trajectory between \f$(ti,qi)\f$ and \f$(tf,qf)\f$ for
      the given DOF.
      \param ti The start time of the trajectory
      \param yi The initial values at ti
      \param tf The final time of the trajectory
      \param yf The final values at tf
      \param variables A mask of output variables
      \param sticky Add a constant function at tf
  */
  robError Sigmoid( double ti, double yi,
		    double tf, double yf,
		    robVariablesMask variables, 
		    bool sticky=true );
  
  //! Create a 1D sigmoid trajectory
  /** 
      Create a nD trajectory between \f$(ti,qi)\f$ and \f$(tf,qf)\f$ for
      the given DOF.
      \param yi The initial values at ti
      \param yf The final values at tf
      \param vmax The maximum velocity
      \param variables A mask of output variables
      \param sticky Add a constant function at tf
  */
  robError Sigmoid( double yi, double yf, 
		    double vmax, 
		    robVariablesMask variables, 
		    bool sticky=true);
  
  //! Create a nD sigmoid trajectory
  /** 
      Create a nD trajectory between \f$(ti,qi)\f$ and \f$(tf,qf)\f$ for
      the given DOF. This creates N 1D sigmoids. All the sigmoids are 
      synchronized.
      \param ti The start time of the trajectory
      \param yi The initial values at ti
      \param tf The final time of the trajectory
      \param yf The final values at tf
      \param variables A mask of output variables
      \param sticky Add a constant function at tf
  */
  robError Sigmoid( double ti, const vctDynamicVector<double>& yi, 
		    double tf, const vctDynamicVector<double>& yf, 
		    robVariablesMask variables, 
		    bool sticky=true );
  
  //! Create a nD sigmoid trajectory
  /** 
      Create a nD trajectory between \f$(ti,qi)\f$ and \f$(tf,qf)\f$ for
      the given DOF. This creates N 1D sigmoids. All the sigmoids are 
      synchronized.
      \param yi The initial values at ti
      \param yf The final values at tf
      \param vmax The maximum velocity for each DOF
      \param variables A mask of output variables
      \param sticky Add a constant function at tf
  */
  robError Sigmoid( const vctDynamicVector<double>& yi, 
		    const vctDynamicVector<double>& yf, 
		    const vctDynamicVector<double>& vmax, 
		    robVariablesMask variables, 
		    bool sticky=true );
  
  //! Create a linear 3D translation
  /**
     \param ti The start time of the trajectory
     \param Rt The initial values at ti
     \param tf The final time of the trajectory
     \param Rtf The final values at tf
     \param variables A mask of output variables
     \param sticky Add a constant function at tf
  */
  robError Translation( double ti, const vctFrame4x4<double,VCT_ROW_MAJOR>& Rti,
			double tf, const vctFrame4x4<double,VCT_ROW_MAJOR>& Rtf,
			robVariablesMask variables,
			bool sticky=true );
  
  //! Create a linear 3D rotation
  /**
     \param ti The start time of the trajectory
     \param Rt The initial values at ti
     \param tf The final time of the trajectory
     \param Rtf The final values at tf
     \para variables A mask of output variables
     \param sticky Add a constant function at tf
  */
  robError Rotation( double ti, const vctFrame4x4<double,VCT_ROW_MAJOR>& Rti,
		     double tf, const vctFrame4x4<double,VCT_ROW_MAJOR>& Rtf,
		     robVariablesMask variables,
		     bool sticky=true );

  //! Create a position tracking trajectory
  /**
     \param variables A mask of output variables
     \param vmax The maximum linear velocity
     \param vmax The maximum angular velocity
     \param vmax The maximum linear acceleration
  */
  robError TrackSE3( robVariablesMask variables, 
		     double vmax = -1, 
		     double wmax = -1, 
		     double vdmax= -1 );
  
  //! Remove all the functions
  void Clear();
  
  //! Evaluate the trajectory
  robError Evaluate( robVariables& output );
  
};

#endif
