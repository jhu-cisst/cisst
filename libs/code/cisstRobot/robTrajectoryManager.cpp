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

#include <cisstRobot/robTrajectoryManager.h>

#include <cisstRobot/robTrajectoryR1.h>
#include <cisstRobot/robTrajectorySO3.h>

#include <cisstRobot/robLinear.h>
#include <cisstRobot/robConstantR1.h>
#include <cisstRobot/robConstantSO3.h>
#include <cisstRobot/robSLERP.h>
#include <cisstRobot/robSigmoid.h>

#include <cisstCommon/cmnLogger.h>

#include <typeinfo>
#include <iostream>

// Initialize the trajectory manager
// This initialize the start time for all trajectory and it also sets
// each initial joint position and each joint maximum velocity and acceleration
// This creates a robTrajectoryR1 for each joint position that is enabled in 
// space and inserts a robConstantR1 function in each of them 
robTrajectoryManager::robTrajectoryManager( const robSpace& space,
					    double t,
					    const vctDynamicVector<double>& q,
					    const vctDynamicVector<double>& qdmax,
					    const vctDynamicVector<double>& qddmax):
  // the domain/codomain of robFunction is not used by the manager
  robFunction(0, 0){

  // Size matters
  if( q.size() != qdmax.size() || q.size() != qddmax.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Vectors have different size. The vectors have:"
		      << " size(q)=" << q.size() 
		      << " size(qd)=" << qdmax.size()
		      << " size(qdd)=" << qddmax.size()
		      << std::endl;
  }

  // Check each joint position basis in the given space
  robSpace::Basis bi = robSpace::Q1;
  for( size_t i=0; i<9; i++ ){

    // if bi is in the space
    if( space.IsEnabled( bi ) ){

      // then create a R1trajectory for bi
      robTrajectoryR1* traj = new robTrajectoryR1( bi, t, qdmax[i], qddmax[i] );

      // and insert a constant value for the trajectory
      robConstantR1* constant = new robConstantR1( bi, q[i], t, FLT_MAX );
      if( traj->Insert( constant, t, FLT_MAX ) != robFunction::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << ": Failed to insert an constant function."
			  << std::endl;
      }
      // insert the trajectory in the list
      trajectories.push_back( traj );
    }
    // next position basis
    bi <<= 1;
  }
}

robTrajectoryManager::robTrajectoryManager( const robSpace& space,
					    double t,
					    const vctFrame4x4<double>& Rt,
					    double vmax, double vdmax,
					    double wmax, double wdmax ) :
  // the domain/codomain of robFunction is not used by the manager
  robFunction(0,0) {

  // list of translation basis
  robSpace::Basis b[3] = {robSpace::TX, robSpace::TY, robSpace::TZ};
  for( size_t i=0; i<3; i++ ){

    // if b[i] is in the space
    if( space.IsEnabled( b[i] ) ){

      // then create a R1 trajectory for b[i]
      robTrajectoryR1* traj = new robTrajectoryR1( b[i], t, vmax, vdmax );
      
      // and initialize the trajectory for b[i] by inserting a constant function
      robConstantR1* constant = new robConstantR1( b[i], Rt[i][3], t, FLT_MAX );
      if( traj->Insert( constant, t, FLT_MAX ) != robFunction::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << ": Failed to insert an constant function."
			  << std::endl;
      }
      // insert the trajectory in the list
      trajectories.push_back( traj );
    }
  }

  // if the space involves orientation
  if( space.IsEnabled( robSpace::ORIENTATION ) ){

    // then create a SO3 trajectory
    robTrajectorySO3* traj = new robTrajectorySO3( t, wmax, wdmax );
    // I hate having to do this...
    vctMatrixRotation3<double> R( Rt[0][0], Rt[0][1], Rt[0][2],
				  Rt[1][0], Rt[1][1], Rt[1][2],
				  Rt[2][0], Rt[2][1], Rt[2][2] );

    // and initialize the trajectory by inserting a constant orientation
    robConstantSO3* cte=new robConstantSO3(robSpace::ORIENTATION, t, R,FLT_MAX);
    if( traj->Insert( cte, t, FLT_MAX ) != robFunction::ESUCCESS ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to insert an constant function."
			<< std::endl;
    }
    // insert the trajectory in the list
    trajectories.push_back( traj );
  }
}


void robTrajectoryManager::Clear(){ trajectories.clear(); }

robTrajectory* 
robTrajectoryManager::FindTrajectory( const robMapping& mapping ){
  // Search of an existing trajectory in the list that matches the mapping
  std::list<robTrajectory*>::iterator traj;
  for( traj=trajectories.begin(); traj!=trajectories.end(); traj++ ){
    if( (**traj) == mapping )
      { return *traj; }
  }
  return NULL;
}

////// CONSTANT VECTOR

robFunction::Errno robTrajectoryManager::Constant( const robSpace& space ){

  // list all the individual basis of the input space into a vector
  // this way we can go through the basis one-by-one
  std::vector<robSpace::Basis> b = space.ListBasis();

  // for each basis in the input space
  for( size_t i=0; i<b.size(); i++ ){

    // search for an existing joint trajectory in the list of trajectories
    // Note: there *must* be a trajectory in the list otherwise the manager
    // was not initialized properly
    robSpace domain( robSpace::TIME );
    // Ensure we only deal with joint positions or Cartesian positions
    robSpace codomain( ( robSpace::JOINTS_POS | robSpace::TRANSLATION ) & b[i] );
    robTrajectory* traj = FindTrajectory( robMapping( domain, codomain ) );
    
    // Ensure the trajectory was found
    if( traj != NULL ){
	
      // Evalute that trajectory at its last point
      robVariable input, output;
      traj->EvaluateLastSegment( input, output );

      double ti, tf, y;
      ti = input.time;                                       // initial time
      tf = FLT_MAX;                                          // final time

      // parse the codomain
      if( b[i] & robSpace::JOINTS_POS )
	{ y = output.q[codomain.JointBasis2ArrayIndex()]; }  // joint position qi
      if( b[i] & robSpace::TX )
	{ y = output.t[0]; }                                 // X position
      if( b[i] & robSpace::TY )
	{ y = output.t[1]; }                                 // Y position
      if( b[i] & robSpace::TZ )
	{ y = output.t[2]; }                                 // Z position
      
      // Evalute that trajectory at its last point
      robConstantR1* constant = new robConstantR1( b[i], ti, y, tf );
      if( traj->Insert( constant, ti, tf ) != robFunction::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << ": Failed to insert a insert constant."
			  << std::endl;
      }
    }
  }
  return robFunction::ESUCCESS;
}

////// LINEAR VECTOR

robFunction::Errno robTrajectoryManager::Linear( const robSpace& space,
						 const double y, 
						 const double ydmax ){

  return Linear( space, 
		 vctDynamicVector<double>(1,y),
		 vctDynamicVector<double>(1,ydmax) );
}

robFunction::Errno 
robTrajectoryManager::Linear( const robSpace& space,
			      const vctDynamicVector<double>& y, 
			      const vctDynamicVector<double>& ydmax ){

  // Size matters
  if( y.size() != ydmax.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Vectors have different size. The vectors have:"
		      << " size(y)=" << y.size() 
		      << " size(ydmax)=" << ydmax.size()
		      << std::endl;
    return robFunction::EFAILURE;
  }

  // list all the individual basis of the input space into a vector
  // this way we can go through the basis one-by-one
  std::vector<robSpace::Basis> b = space.ListBasis();

  // for each basis of the input space
  for( size_t i=0; i<b.size(); i++ ){

    // search for an existing trajectory in the list of trajectories
    // Note: there *must* be a trajectory in the list otherwise the manager
    // was not initialized properly
    robSpace domain( robSpace::TIME );
    // Ensure we only deal with joint positions or Cartesian positions
    robSpace codomain( ( robSpace::JOINTS_POS | robSpace::TRANSLATION ) & b[i] );
    robTrajectory* traj = FindTrajectory( robMapping( domain, codomain ) );
    
    // Ensure the trajectory was found
    if( traj != NULL ){
      
      // Evalute that trajectory at its last point
      robVariable input, output;
      traj->EvaluateLastSegment( input, output );
      
      double ti, tf, yi, yf, yd;// initial and final time, values and velocity
      ti = input.time;          // initial time
      
      // if the codomain is joint position, read the output from q
      if( b[i] & robSpace::JOINTS_POS ){
	yi = output.q[codomain.JointBasis2ArrayIndex()];  // initial position
	yf = y[ codomain.JointBasis2ArrayIndex() ];       // final position
	yd = ydmax[ codomain.JointBasis2ArrayIndex() ];   // max joint vel
      }
      // if the codomain is joint position, read the output from t
      if( b[i] & robSpace::TX ){
	yi = output.t[0];                                 // initial position
	yf = y[0];                                        // final position
	yd = ydmax[0];                                    // max linear vel
      }
      if( b[i] & robSpace::TY ){
	yi = output.t[1];                                 // initial position
	yf = y[1];                                        // final position
	yd = ydmax[1];                                    // max linear vel
      }
      if( b[i] & robSpace::TZ ){
	yi = output.t[2];                                 // initial position
	yf = y[2];                                        // final position
	yd = ydmax[2];                                    // max linear vel
      }
      
      tf = ti + fabs( (yf - yi) / yd );                   // final time

      robLinear* linear = new robLinear( b[i], ti, yi, tf, yf );
      if( traj->Insert( linear, ti, tf ) != robFunction::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << ": Failed to insert a function in the trajectory"
			  << std::endl;
      }
    }
  }

  // hold the position
  //ConstantR1( space );

  return robFunction::ESUCCESS;
}

robFunction::Errno 
robTrajectoryManager::Linear(const robSpace& space,
			     const std::vector<vctDynamicVector<double> >& y, 
			     const std::vector<vctDynamicVector<double> >& ydmax){
  // size matters
  if( y.size() != ydmax.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Vectors have different size. The vectors have:"
		      << " size(y)=" << y.size() 
		      << " size(ydmax)=" << ydmax.size()
		      << std::endl;
    return robFunction::EFAILURE;
  }

  // for each trajectory segment, insert a linear segment
  for( size_t i=0; i<y.size(); i++ )
    { Linear( space, y[i], ydmax[i] ); }

  return robFunction::ESUCCESS;
}

////// SIGMOID VECTOR

robFunction::Errno robTrajectoryManager::Sigmoid( const robSpace& space,
						  const double y, 
						  const double ydmax ){
  return Linear( space, 
		 vctDynamicVector<double>(1,y),
		 vctDynamicVector<double>(1,ydmax) );
}

robFunction::Errno
robTrajectoryManager::Sigmoid( const robSpace& space,
			       const vctDynamicVector<double>& y, 
			       const vctDynamicVector<double>& ydmax ){
  // Size matters
  if( y.size() != ydmax.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Vectors have different size. The vectors have:"
		      << " size(y)=" << y.size() 
		      << " size(ydmax)=" << ydmax.size()
		      << std::endl;
    return robFunction::EFAILURE;
  }

  // list all the individual basis of the input space into a vector
  // this way we can go through the basis one-by-one
  std::vector<robSpace::Basis> b = space.ListBasis();

  // for each basis of the input space
  for( size_t i=0; i<b.size(); i++ ){

    // search for an existing trajectory in the list of trajectories
    // Note: there *must* be a trajectory in the list otherwise the manager
    // was not initialized properly
    robSpace domain( robSpace::TIME );
    // Ensure we only deal with joint positions or Cartesian positions
    robSpace codomain( ( robSpace::JOINTS_POS | robSpace::TRANSLATION ) & b[i] );
    robTrajectory* traj = FindTrajectory( robMapping( domain, codomain ) );
    
    // Ensure the trajectory was found
    if( traj != NULL ){
      
      // Evalute that trajectory at its last point
      robVariable input, output;
      traj->EvaluateLastSegment( input, output );
      
      double ti, tf, yi, yf, yd;// initial and final time, values and velocity
      ti = input.time;          // initial time
      
      // if the codomain is joint position, read the output from q
      if( b[i] & robSpace::JOINTS_POS ){
	yi = output.q[codomain.JointBasis2ArrayIndex()];  // initial position
	yf = y[ codomain.JointBasis2ArrayIndex() ];       // final position
	yd = ydmax[ codomain.JointBasis2ArrayIndex() ];   // max joint vel
      }
      // if the codomain is joint position, read the output from t
      if( b[i] & robSpace::TX ){
	yi = output.t[0];                                 // initial position
	yf = y[0];                                        // final position
	yd = ydmax[0];                                    // max linear vel
      }
      if( b[i] & robSpace::TY ){
	yi = output.t[1];                                 // initial position
	yf = y[1];                                        // final position
	yd = ydmax[0];                                    // max linear vel
      }
      if( b[i] & robSpace::TZ ){
	yi = output.t[2];                                 // initial position
	yf = y[2];                                        // final position
	yd = ydmax[0];                                    // max linear vel
      }
      
      robSigmoid* sigmoid = new robSigmoid( b[i], ti, yi, tf, yd );
      tf = sigmoid->FinalTime();                          // final time
      
      if( traj->Insert( sigmoid, ti, tf ) != robFunction::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << ": Failed to insert a function in the trajectory"
			  << std::endl;
      }
    }
  }

  // hold the position
  //ConstantR1( space );

  return robFunction::ESUCCESS;
}

robFunction::Errno 
robTrajectoryManager::Sigmoid(const robSpace& space,
			      const std::vector<vctDynamicVector<double> >& y, 
			      const std::vector<vctDynamicVector<double> >& ydmax){
  // size matters
  if( y.size() != ydmax.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Vectors have different size. The vectors have:"
		      << " size(y)=" << y.size() 
		      << " size(ydmax)=" << ydmax.size()
		      << std::endl;
    return robFunction::EFAILURE;
  }
  // for each trajectory segment, insert a linear segment
  for( size_t i=0; i<y.size(); i++ )
    { Sigmoid( space, y[i], ydmax[i] ); }

  return robFunction::ESUCCESS;
}


/////// LINEAR CARTESIAN

robFunction::Errno robTrajectoryManager::Linear( const robSpace& space,
						 const vctFrame4x4<double>& Rt, 
						 double vmax, 
						 double wmax ){
  // if translation is enable...insert a linear translation
  if( space.IsEnabled( robSpace::TRANSLATION ) )
    {  Linear( space, Rt.Translation(), vmax ); }
  // if orientation is enabled...insert a linear rotation
  if( space.IsEnabled( robSpace::ORIENTATION ) ){
    vctMatrixRotation3<double> R( Rt[0][0], Rt[0][1], Rt[0][2],
				  Rt[1][0], Rt[1][1], Rt[1][2],
				  Rt[2][0], Rt[2][1], Rt[2][2] );
    Linear( space, R, wmax );
  }
  return robFunction::ESUCCESS;
}

robFunction::Errno 
robTrajectoryManager::Linear( const robSpace& space,
			      const vctFixedSizeVector<double,3>& t,
			      double vmax ){
  // if translation along x is enable...insert a linear translation along x
  if( space.IsEnabled( robSpace::TX ) )
    {  Linear( robSpace( robSpace::TX ), 
	       vctDynamicVector<double>(3,t[0]),
	       vctDynamicVector<double>(3,vmax) ); }
  // if translation along y is enable...insert a linear translation along y
  if( space.IsEnabled( robSpace::TY ) )
    {  Linear( robSpace( robSpace::TY ),
	       vctDynamicVector<double>(3,t[1]),
	       vctDynamicVector<double>(3,vmax) ); }
  // if translation along z is enable...insert a linear translation along z
  if( space.IsEnabled( robSpace::TZ ) )
    {  Linear( robSpace( robSpace::TZ ),
	       vctDynamicVector<double>(3,t[2]),
	       vctDynamicVector<double>(3,vmax) ); }
  return robFunction::ESUCCESS;
}

robFunction::Errno 
robTrajectoryManager::Linear( const robSpace& space,
			      const vctMatrixRotation3<double>& R,
			      double wmax ){

  if( space.IsEnabled( robSpace::ORIENTATION ) ){ 

    // search for an existing trajectory in the list of trajectories
    // Note: there *must* be a trajectory in the list otherwise the manager
    // was not initialized properly
    robSpace domain( robSpace::TIME );
    robSpace codomain( robSpace::ORIENTATION );
    robTrajectory* traj = FindTrajectory( robMapping( domain, codomain ) );

    // Ensure the trajectory was found
    if( traj != NULL ){
      
      // Evalute that trajectory at its last point
      robVariable input, output;
      traj->EvaluateLastSegment( input, output );

      double ti = input.time;                     // initial time
      vctMatrixRotation3<double> Rwi = output.R;  // initial orientation
      
      vctMatrixRotation3<double> Rwf = R;         // final orientation
      vctMatrixRotation3<double> Riw, Rif;        // relative orientations
      Riw.InverseOf( Rwi );
      Rif = Riw * Rwf;
      vctAxisAngleRotation3<double> rif( Rif );
      double tf = ti + rif.Angle() / wmax;        // final time

      robSLERP* slerp = new robSLERP( ti, Rwi, tf, Rwf );
      if( traj->Insert( slerp, ti, tf ) != robFunction::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << ": Failed to insert a function in the trajectory"
			  << std::endl;
      }
    }
  }
  return robFunction::ESUCCESS;  
}

robFunction::Errno 
robTrajectoryManager::Linear( const robSpace& space,
			      const std::vector< vctFrame4x4<double> >& Rt, 
			      const std::vector<double>& vmax, 
			      const std::vector<double>& wmax ){
  
  if( Rt.size() != vmax.size() || Rt.size() != wmax.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Vectors must have the same length" 
		      << std::endl;
    return robFunction::EFAILURE;
  }

  for( size_t i=0; i<Rt.size(); i++ )
    {  Linear( space, Rt[i], vmax[i], wmax[i] );  }

  return robFunction::ESUCCESS;
}

/////// SIGMOID CARTESIAN

robFunction::Errno robTrajectoryManager::Sigmoid( const robSpace& space,
						  const vctFrame4x4<double>& Rt, 
						  double vmax, 
						  double wmax ){

  // if translation is enable...insert a sigmoid translation
  if( space.IsEnabled( robSpace::TRANSLATION ) )
    {  Sigmoid( space, Rt.Translation(), vmax ); }

  // if orientation is enabled...insert a linear rotation
  if( space.IsEnabled( robSpace::ORIENTATION ) ){ 
    vctMatrixRotation3<double> R( Rt[0][0], Rt[0][1], Rt[0][2],
				  Rt[1][0], Rt[1][1], Rt[1][2],
				  Rt[2][0], Rt[2][1], Rt[2][2] );
    Linear( space, R, wmax );
  }

  return robFunction::ESUCCESS;
}

robFunction::Errno 
robTrajectoryManager::Sigmoid( const robSpace& space,
			       const vctFixedSizeVector<double,3>& t,
			       double vmax ){
  // if translation along x is enable...insert a linear translation along x
  if( space.IsEnabled( robSpace::TX ) )
    {  Sigmoid( robSpace( robSpace::TX ), 
	       vctDynamicVector<double>(3,t[0]),
	       vctDynamicVector<double>(3,vmax) ); }
  // if translation along y is enable...insert a linear translation along y
  if( space.IsEnabled( robSpace::TY ) )
    {  Sigmoid( robSpace( robSpace::TY ), 
	       vctDynamicVector<double>(3,t[1]),
	       vctDynamicVector<double>(3,vmax) ); }
  // if translation along z is enable...insert a linear translation along z
  if( space.IsEnabled( robSpace::TZ ) )
    {  Sigmoid( robSpace( robSpace::TZ ), 
	       vctDynamicVector<double>(3,t[2]),
	       vctDynamicVector<double>(3,vmax) ); }
  return robFunction::ESUCCESS;
}


robFunction::Errno 
robTrajectoryManager::Sigmoid( const robSpace& space,
			       const std::vector< vctFrame4x4<double> >& Rt, 
			       const std::vector<double>& vmax, 
			       const std::vector<double>& wmax ){
  
  if( Rt.size() != vmax.size() || Rt.size() != wmax.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Vectors must have the same length" 
		      << std::endl;
    return robFunction::EFAILURE;
  }

  for( size_t i=0; i<Rt.size(); i++ )
    {  Sigmoid( space, Rt[i], vmax[i], wmax[i] );  }

  return robFunction::ESUCCESS;
}

/////// EVALUATE

robFunction::Context 
robTrajectoryManager::GetContext( const robVariable& ) const
{ return robFunction::CDEFINED; }


robFunction::Errno robTrajectoryManager::Evaluate( const robVariable& input,
						   robVariable& output ){

  // clean slate
  output.Clear();

  // evaluate each function in the maps
  std::list<robTrajectory*>::iterator traj;
  for( traj=trajectories.begin(); traj!=trajectories.end(); traj++ ){
    if( (*traj)->Evaluate( input, output ) != robFunction::ESUCCESS ){
    }
  }

  return robFunction::ESUCCESS;
}
