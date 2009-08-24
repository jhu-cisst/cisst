#include <cisstRobot/robTrajectory.h>
#include <cisstRobot/robLinear.h>
#include <cisstRobot/robSigmoid.h>
#include <cisstRobot/robSLERP.h>
#include <cisstRobot/robRnConstant.h>
#include <cisstRobot/robSE3Constant.h>
#include <cisstRobot/robSE3Track.h>
#include <cisstRobot/robFunctionPiecewise.h>

#include <typeinfo>
#include <iostream>
using namespace std;
using namespace cisstRobot;

robMapping::robMapping() {}
robMapping::robMapping( const robDOF& from, const robDOF& to ){
  domain = from;
  codomain = to;
}
  
robTrajectory::robTrajectory(robClock* clock, robDevice* device){ 
  this->clock = clock;
  this->device = device; 
}


void robTrajectory::Clear(){ functions.clear(); }

robError robTrajectory::Insert( robFunction* function, 
				uint64_t indof, 
				uint64_t outdof ){

   // create a mapping from time to joint positions
  robDOF rd1(indof), rd2(outdof);
  robMapping mapping( rd1, rd2 );

  // see if a mapping is already in the table
  std::map< robMapping, robFunction* >::iterator iter =  functions.find(mapping);

  // nope...the mapping is not in the table...add a new one
  if( iter == functions.end() ) { 
    
    // create a new piecewise function
    robFunctionPiecewise* fnpw = new robFunctionPiecewise();
    
    // insert the function in the piecewise function
    fnpw->Insert( function );
    
    // this is used to hold the result from the insertion
    std::pair< std::map<robMapping, robFunction*>::iterator, bool > result;

    // insert the mapping/piecewise function pair
    result = functions.insert( std::make_pair( mapping, fnpw ) );

    // if the insertion happened return right away
    if( result.second == true ){  return SUCCESS;  }
    
    // the insertion didn't work
    else{ 
      cout <<"robTrajectory::Insert: Couldn't insert in the map" << endl;
      return FAILURE;
    }
  }
  
  // the mapping is already in the map
  else{
    
    // check if the existing function is a piecewise function
    robFunctionPiecewise* fnpw = dynamic_cast<robFunctionPiecewise*>(iter->second);
  
    //  nope the function isn't a piecewise function, we can't add the function
    if( fnpw == NULL ) { 
      cout<<"robTrajectory::Insert: Mapping conflict." << endl;
      return FAILURE;
    }

    // the function is a piecewise function so insert the new function in there
    else{
      fnpw->Insert( function );
      return SUCCESS;
    }
  }
}

robError robTrajectory::Linear( real ti, real qi, 
				real tf, real qf, 
				uint64_t dof, 
				bool sticky ){
  // should check that only one Rn dof is set
  return Linear( ti, Rn(1,qi), tf, Rn(1,qf), dof, sticky );
}

robError robTrajectory::Linear( real qi, real qf, 
				real vmax, uint64_t dof, 
				bool sticky ){
  if(clock==NULL){
    cout << "robTrajectory::Linear: No clock" << endl;
    return FAILURE;
  }
  
  real ti = clock->Evaluate();
  real tf = ti + fabs(qf-qi)/fabs(vmax);

  if( Linear( ti, qi, tf, qf, dof, sticky ) == FAILURE ){
    cout << "robTrajectory::Linear: failed to create a linear trajectory" << endl;
    return FAILURE;
  }

  return SUCCESS;
}

robError robTrajectory::Linear( const Rn& qi, 
				const Rn& qf, 
				real vmax, uint64_t dof, bool sticky ){
  // test for vector size
  if( qi.size() != qf.size() ){
    cout << "robTrajectory::Linear: vectors must have the same length" << endl;
    return FAILURE;
  }

  // get the current time
  real ti = clock->Evaluate();

  // find the longuest time
  real tf = -1;
  for(size_t i=0; i<qi.size(); i++){
    real t = ti + fabs(qf[i]-qi[i])/fabs(vmax);
    if( tf < t ) tf = t;
  }

  if( Linear( ti, qi, tf, qf, dof, sticky ) == FAILURE ){
    cout << "robTrajectory::Linear: failed to create a linear trajectory" << endl;
    return FAILURE;
  }
  return SUCCESS;
}
    
robError robTrajectory::Linear( real ti, const Rn& qi, 
				real tf, const Rn& qf, 
				uint64_t dof, bool sticky ){

  // check that the time is ok
  if( tf < ti ){
    cout << "robTrajectory::Linear: ti must be less than tf" <<endl;
    return FAILURE;
  }

  // check that the vector size match
  if( qi.size() != qf.size() ){
    cout << "robTrajectory::Linear: vectors must have the same length" << endl;
    return FAILURE;
  }

  // check that we need real position and/or velocities and/or accelerations
  dof &= ( robDOF::XPOS | robDOF::TX  | robDOF::TY  | robDOF::TZ |
	   robDOF::XVEL | robDOF::VX  | robDOF::VY  | robDOF::VZ |
	   robDOF::XACC | robDOF::VXD | robDOF::VYD | robDOF::VZD );
  if( !dof ){
    cout << "robTrajectory::Linear: expected real position values" << endl;
    return FAILURE;
  }

  // create a linear function
  robLinear* fnlin = new robLinear(ti, qi, tf, qf);

  // insert the linear function as a mapping F:R1->Rn
  if( Insert(fnlin, robDOF::TIME, dof) == FAILURE ){
    cout << "robTrajectory::Linear: Failed to insert the function." << endl;
    return FAILURE;
  }

  // do we want to hold the position at the end
  if(sticky){
    // create a constant function at time tf
    robRnConstant* fncte = new robRnConstant( qf, tf );
    
    // insert the constant function as a mapping F:R1->Rn
    if( Insert( fncte, robDOF::TIME, dof) == FAILURE ){
      cout << "robTrajectory::Linear: Failed to insert the function." << endl;
      return FAILURE;
    }
  }
  return SUCCESS;
}

robError robTrajectory::Sigmoid( real ti, real qi,
				 real tf, real qf, 
				 uint64_t dof, 
				 bool sticky ){
  // check the time
  if( tf < ti ){
    cout<<"robTrajectory::Sigmoid: initial time is later than final time" <<endl;
    return FAILURE;
  }

  // check that a position DOF is used (should make sure that there's only 1 dof)
  dof &= (robDOF::XPOS|robDOF::XVEL|robDOF::XACC);
  if( !dof ){
    cout<<"robTrajectory::Sigmoid: DOF of sigmoid must be position" <<endl;
    return FAILURE;
  }

  // create the linear function
  robSigmoid* fnsig = new robSigmoid(ti, qi, tf, qf);

  // insert the linear function as a mapping F:R1->Rn
  if( Insert(fnsig, robDOF::TIME, dof) == FAILURE ){
    cout << "robTrajectory::Simoid: Failed to insert the function." << endl;
    return FAILURE;
  }

  // do we want to hold the position at the end
  if(sticky){
    // create a constant function at time tf
    robRnConstant* fncte = new robRnConstant( qf, tf );
    
    // insert the constant function as a mapping F:R1->Rn
    if( Insert( fncte, robDOF::TIME, dof) == FAILURE ){
      cout << "robTrajectory::Sigmoid: Failed to insert the function." << endl;
      return FAILURE;
    }
  }
  return SUCCESS;
}

robError robTrajectory::Sigmoid( real ti, const Rn& qi,
				 real tf, const Rn& qf, 
				 uint64_t dof, 
				 bool sticky ){
  // check that the vector size match
  if( qi.size() != qf.size() ){
    cout << "robTrajectory::Sigmoid: vectors must have the same length" << endl;
    return FAILURE;
  }
  // this part is really ugly
  uint64_t mask = robDOF::X1 | robDOF::X1D | robDOF::X1DD ;
  for( size_t i=0; i<qi.size(); i++ ){

    while( mask != (robDOF::X8 | robDOF::X8D | robDOF::X8DD) ){
      if( mask & dof ){
	if( Sigmoid(ti, qi[i], tf, qf[i], dof & mask, sticky ) == FAILURE ){
	  cout << "robTrajectory::Sigmoid: Failed to create a sigmoid." << endl;
	  return FAILURE;
	}
	mask <<= 1;
	break;
      }
      mask <<= 1;
    }
  }
  return SUCCESS;
}

robError robTrajectory::Translation( real ti, const SE3& Rti,
				     real tf, const SE3& Rtf,
				     uint64_t dof,
				     bool sticky ){
  // check the time
  if( tf < ti ){
    cout<<"robTrajectory::Translation: initial time is later than final" <<endl;
    return FAILURE;
  }

  // check the DOF are for translation and/or derivatives
  dof &= (robDOF::TX |robDOF::TY |robDOF::TZ|
	  robDOF::VX |robDOF::VY |robDOF::VZ|
	  robDOF::VXD|robDOF::VYD|robDOF::VZD );
  if( !dof ){
    cout<<"robTrajectory::Translation: expected translation DOF" <<endl;
    return FAILURE;
  }
  
  Rn Ti(3, Rti[0][3], Rti[1][3], Rti[2][3] );
  Rn Tf(3, Rtf[0][3], Rtf[1][3], Rtf[2][3] );

  if( Linear(ti, Ti, tf, Tf, dof, sticky) == FAILURE ){
    cout << "robTrajectory::Translation: failed to create a linear function"<<endl;
    return FAILURE;
  }
  return SUCCESS;
}

robError robTrajectory::Rotation( real ti, const SE3& Rti,
				  real tf, const SE3& Rtf,
				  uint64_t dof,
				  bool sticky ){
  // check the time
  if( tf < ti ){
    cout<<"robTrajectory::Rotation: initial time is later than final" <<endl;
    return FAILURE;
  }

  // check the DOF are for rotation and/or velocities and/or accelerations
  dof &= (robDOF::RX |robDOF::RY |robDOF::RZ|
	  robDOF::WX |robDOF::WY |robDOF::WZ|
	  robDOF::WXD|robDOF::WYD|robDOF::WZD );
  if( !dof ){
    cout<<"robTrajectory::Rotation: expected rotation DOF" <<endl;
    return FAILURE;
  }
  
  // create slerp
  robSLERP* fnslerp = new robSLERP( ti, Rti, tf, Rtf );

  // insert the linear function as a mapping F:R1->Rn
  if( Insert( fnslerp, robDOF::TIME, dof ) == FAILURE ){
    cout << "robTrajectory::Rotation: Failed to insert the function." << endl;
    return FAILURE;
  }

  // do we want to hold the position at the end
  if(sticky){
    // create a constant function at time tf
    robSE3Constant* fncte = new robSE3Constant( Rtf, tf );
    
    // insert the constant function as a mapping F:R1->Rn
    if( Insert( fncte, robDOF::TIME, dof ) == FAILURE ){
      cout << "robTrajectory::Rotation: Failed to insert the function." << endl;
      return FAILURE;
    }
  }
  return SUCCESS;
}
				      
robError robTrajectory::Linear( real ti, const SE3& Rti,
				real tf, const SE3& Rtf,
				uint64_t dof, 
				bool sticky ){

  if( Translation(ti, Rti, tf, Rtf, dof, sticky) == FAILURE ){
    cout << "robTrajectory::Linear: Failed to create a translation" << endl;
    return FAILURE;
  }
  if( Rotation(ti, Rti, tf, Rtf, dof, sticky) == FAILURE ){
    cout << "robTrajectory::Linear: Failed to create a rotation" << endl;
    return FAILURE;
  }

  return SUCCESS;
}

robError robTrajectory::Linear( const SE3& Rtwi, const SE3& Rtwf, 
				real vmax, real wmax,
				uint64_t dof, bool sticky ){
  // ensure the clock is there
  if(clock==NULL){
    cout << "robTrajectory::Linear: No clock" << endl;
    return FAILURE;
  }

  real ti = clock->Evaluate();
 
  // compute the translation time: timet
  R3 pwi = Rtwi.Translation();
  R3 pwf = Rtwf.Translation();
  R3 pif = pwf - pwi;
  real timet = pif.Norm() / fabs(vmax);

  // compute the rotation time: clock
  SE3 Rtiw(Rtwi);
  Rtiw.InverseSelf();
  SE3 Rtif = Rtiw * Rtwf;
  SO3 Rif;
  for(int r=0; r<3; r++) for(int c=0; c<3; c++) Rif[r][c] = Rtif[r][c];
  vctAxisAngleRotation3<real> ut(Rif);
  real timeR = ut.Angle()/ fabs(wmax);

  // compute the final time
  real tf = ti+timet;
  if( timet < timeR )
    tf = ti+timeR;

  // create the motion
  if( Linear(ti, Rtwi, tf, Rtwf, dof, sticky) == FAILURE ){
    cout << "robTrajectory::Linear: Could not create a linear trajectory" << endl;
    return FAILURE;
  }

  return SUCCESS;
}

robError robTrajectory::Linear( const std::vector<SE3>& Rt, 
				real vmax, real wmax,
				uint64_t dof, bool sticky){

  // ensure the clock is there
  if(clock==NULL){
    cout << "robTrajectory::Linear: No clock" << endl;
    return FAILURE;
  }

  real ti = clock->Evaluate();

  for(size_t i=0; i<Rt.size()-1; i++){
    SE3 Rtwi = Rt[i];
    SE3 Rtwf = Rt[i+1];

    // compute the translation time: timet
    R3 pwi = Rtwi.Translation();
    R3 pwf = Rtwf.Translation();
    R3 pif = pwf - pwi;
    real timet = pif.Norm() / fabs(vmax);
    
    // compute the rotation time: clock
    SE3 Rtiw(Rtwi);
    Rtiw.InverseSelf();
    SE3 Rtif = Rtiw * Rtwf;
    SO3 Rif;
    for(int r=0; r<3; r++) for(int c=0; c<3; c++) Rif[r][c] = Rtif[r][c];
    vctAxisAngleRotation3<real> ut(Rif);
    real timeR = ut.Angle()/fabs(wmax);

    // compute the final time
    real tf = ti+timet;
    if( timet < timeR )
      tf = ti+timeR;

    // create the motion
    if( Linear(ti, Rtwi, tf, Rtwf, dof, sticky) == FAILURE ){
      cout<<"robTrajectory::Linear: Could not create a linear trajectory" << endl;
      return FAILURE;
    }
    ti+=tf;
  }
  return SUCCESS;
}

robError robTrajectory::SE3Track( uint64_t dof, real vmax, real wmax, real vdmax ){

  // check the DOF are for rotation and/or velocities and/or accelerations
  dof &= (robDOF::RX | robDOF::RY | robDOF::RZ |
	  robDOF::TX | robDOF::TY | robDOF::TZ );
  if( !dof ){
    cout<<"robTrajectory::SE3Track: expected Cartesian DOF" <<endl;
    return FAILURE;
  }

  // create slerp
  robSE3Track* fntrack = new robSE3Track( vmax, wmax, vdmax );
  
  // insert the linear function as a mapping F:R1->Rn
  //if( Insert( fntrack, robDOF::TIME | robDOF::CARTESIAN, dof ) == FAILURE ){
  if( Insert( fntrack, dof, dof ) == FAILURE ){
    cout << "robTrajectory::SE3Track: Failed to insert the function." << endl;
    return FAILURE;
  }
  return SUCCESS;
}

robError robTrajectory::Evaluate( robDOF& output ){

  // make sure that the domain is there
  if( clock == NULL ){
    cout<<"robTrajectory::evaluate: no clock."<<endl;
    return FAILURE;
  }

  robDOF fninput( clock->Evaluate() );
  if( device != NULL ){
    device->Generate( fninput );
  }

  // evaluate each function in the maps
  std::map<robMapping, robFunction*>::iterator iter;
  
  for(iter=functions.begin(); iter!=functions.end(); iter++){
    
    robMapping mapping = iter->first;      // the dof mask
    robFunction* function = iter->second;  // the function

    // Handle real outputs
    if( mapping.To().IsReal() ){

      // Evaluate the real function
      robDOF rnoutput( mapping.To().GetDOF() );
      if( function->Evaluate( fninput, rnoutput ) == SUCCESS ){
	output.Set( mapping.To().GetDOF(), rnoutput.x, rnoutput.xd, rnoutput.xdd );
      }
    }

    // Handle Cartesian output
    if( mapping.To().IsCartesian() ){

      // hold results from evaluations
      robDOF se3output( mapping.To().GetDOF() );

      // handle both rotation and translation (i.e. like SE3 tracking)
      if( mapping.To().IsRotation() && mapping.To().IsTranslation () ){

	if( function->Evaluate( fninput, se3output ) == SUCCESS ){
	  output.Set( mapping.To().GetDOF(), 
		      se3output.Rt, 
		      se3output.vw, 
		      se3output.vdwd );
	}

      }

      // handle the rotation function copy directly in dofse3
      else if(mapping.To().IsRotation()){

	if( function->Evaluate( fninput, se3output ) == SUCCESS ){
	  output.Set( mapping.To().GetDOF(), 
		      se3output.Rt, 
		      se3output.vw, 
		      se3output.vdwd );
	}
      }

      // handle the translation function use a Rn and copy the results in dofse3
      else if(mapping.To().IsTranslation()){

	robDOF doft( mapping.To().GetDOF() );
	if( function->Evaluate( fninput, doft ) == SUCCESS ){
	  // copy the Rn values in the SE3 
	  se3output.Rt[0][3] = doft.x[0]; 
	  se3output.Rt[1][3] = doft.x[1]; 
	  se3output.Rt[2][3] = doft.x[2];
	  /*
	  se3output.vw[3]    = doft.xd[3];
	  se3output.vw[4]    = doft.xd[4];
	  se3output.vw[5]    = doft.xd[5];
	  se3output.vdwd[3]  = doft.xdd[3];
	  se3output.vdwd[4]  = doft.xdd[4];
	  se3output.vdwd[5]  = doft.xdd[5];
	  */
	  output.Set( mapping.To().GetDOF(), 
		      se3output.Rt, 
		      se3output.vw, 
		      se3output.vdwd );
	}
      }
    }
  }  
  return SUCCESS;
}
