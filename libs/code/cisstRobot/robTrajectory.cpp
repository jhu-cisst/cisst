#include <cisstRobot/robTrajectory.h>
#include <cisstRobot/robLinear.h>
#include <cisstRobot/robSigmoid.h>
#include <cisstRobot/robSLERP.h>
#include <cisstRobot/robRnConstant.h>
#include <cisstRobot/robSE3Constant.h>
#include <cisstRobot/robFunctionPiecewise.h>

#include <typeinfo>
#include <iostream>
using namespace std;
using namespace cisstRobot;

robMapping::robMapping() { };
robMapping::robMapping( const robDOF& from, const robDOF& to ){
  domain = from;
  codomain = to;
}
  
// 
void robTrajectory::Clear(){ functions.clear(); }

robError robTrajectory::Insert( robFunction* function, 
				uint64_t indof, uint64_t outdof ){

  // see if a mapping is already in the table
  // create a mapping from time to joint positions
  robMapping mapping( robDOF( robDOF::TIME ), robDOF( outdof ) );

  // see if a mapping is already in the table
  std::map<robMapping, robFunction*>::iterator iter = functions.find(mapping);

  //  std::map<robMapping, robFunction*>::iterator iter = functions.find(mapping);
  // nope...the mapping isn't in the table...add a new one
  if( iter == functions.end() ) { 
    robFunctionPiecewise* fnpw = new robFunctionPiecewise();
    fnpw->Insert( function );

    // this is used to hold the result from the insertion
    std::pair< std::map<robMapping, robFunction*>::iterator, bool > result;

    // insert the (mapping, function) pair
    result = functions.insert( std::make_pair( mapping, fnpw ) );

    // if the insertion happened return right away
    if( result.second == true ){  return SUCCESS;  }
    // the insertion didn't work
    else{ 
      cout <<"robTrajectory::Insert: Couldn't insert in the map" << endl;
      return FAILURE;
    }
  }

  else{
    // check if the existing function is a piecewise function
    robFunctionPiecewise* fnpw=dynamic_cast<robFunctionPiecewise*>(iter->second);
    
    // nope the function isn't a piecewise function...
    if( fnpw == NULL ) { 
      cout<<"robTrajectory::Insert: mapping conflict." << endl;
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
  return Linear( ti, Rn(1,qi), tf, Rn(1,qf), dof, sticky );
}

robError robTrajectory::Linear( real ti, const Rn& qi, 
				real tf, const Rn& qf, 
				uint64_t dof, 
				bool sticky ){
  
  if( tf < ti ){
    cout << "robTrajectory::Linear: ti must be less than tf" <<endl;
    return FAILURE;
  }
  if( qi.size() != qf.size() ){
    cout << "robTrajectory::Linear: vectors must have the same length" << endl;
    return FAILURE;
  }
  if( !(dof & robDOF::XPOS) ) {
    cout << "robTrajectory::Linear: trajectory is not for positions DOF" << endl;
    return FAILURE;
  }

  // create a mapping from time to joint positions
  robMapping mapping( robDOF( robDOF::TIME ), robDOF( dof & robDOF::XPOS ) );

  // see if a mapping is already in the table
  std::map<robMapping, robFunction*>::iterator iter = functions.find(mapping);

  // nope...the mapping isn't in the table...add a new one
  if( iter == functions.end() ) { 

    // create the linear function
    robLinear* fnlin = new robLinear(ti, qi, tf, qf);
    robFunctionPiecewise* fnpw = new robFunctionPiecewise();
    fnpw->Insert( fnlin );

    // do we want to hold the position at the end
    if(sticky){
      robRnConstant* fncte = new robRnConstant( qf, tf );
      fnpw->Insert( fncte );
    }

    // this is used to hold the result from the insertion
    std::pair< std::map<robMapping, robFunction*>::iterator, bool > result;

    // insert the (mapping, function) pair
    result = functions.insert( std::make_pair( mapping, fnpw ) );

    // if the insertion happened return right away
    if( result.second == true ){  return SUCCESS;  }

    // the insertion didn't work
    else{ 
      cout <<"robTrajectory::Linear: Couldn't insert in the map" << endl;
      return FAILURE;
    }
  }

  // the mapping is already in the table
  else{

    // check if the existing function is a piecewise function
    robFunctionPiecewise* fnpw=dynamic_cast<robFunctionPiecewise*>(iter->second);

    // nope the function isn't a piecewise function...
    if( fnpw == NULL ) { 
      cout<<"robTrajectory::Linear: mapping conflict." << endl;
      return FAILURE;
    }
    
    // the function is a piecewise function so insert the new function in there
    else{
      // create the linear function
      robLinear* fnlin = new robLinear(ti, qi, tf, qf);
      fnpw->Insert( fnlin );

      // do we want to hold the position at the end
      if(sticky){
	robRnConstant* fncte = new robRnConstant( qf, tf );
	fnpw->Insert( fncte );
      }

      return SUCCESS;
    }
  }
}


robError robTrajectory::Sigmoid( real ti, real qi,
				 real tf, real qf, 
				 uint64_t dof, 
				 bool sticky ){

  if( tf < ti ){
    cout<<"robTrajectory::sigmoid: initial time is later than final time" <<endl;
    return FAILURE;
  }
  if( !(dof & robDOF::XPOS) ){
    cout<<"robTrajectory::sigmoid: DOF of sigmoid must be position" <<endl;
    return FAILURE;
  }

  // create a mapping from time to joint positions
  robMapping mapping( robDOF( robDOF::TIME ), robDOF( dof & robDOF::XPOS ) );

  // see if a mapping is already in the table
  std::map<robMapping, robFunction*>::iterator iter = functions.find(mapping);

  // nope...the mapping isn't in the table...add a new one
  if( iter == functions.end() ) { 

    // create the linear function
    robSigmoid* fnsig = new robSigmoid(ti, qi, tf, qf);
    robFunctionPiecewise* fnpw = new robFunctionPiecewise();
    fnpw->Insert( fnsig );
    // do we want to hold the position at the end
    if(sticky){
      robRnConstant* fncte = new robRnConstant( qf, tf );
      fnpw->Insert( fncte );
    }

    // this is used to hold the result from the insertion
    std::pair< std::map<robMapping, robFunction*>::iterator, bool > result;

    // insert the (mapping, function) pair
    result = functions.insert( std::make_pair( mapping, fnpw ) );

    // if the insertion happened return right away
    if( result.second == true ){  return SUCCESS;  }

    // the insertion didn't work
    else{ 
      cout <<"robTrajectory::sigmoid: Couldn't insert in the map" << endl;
      return FAILURE;
    }

  }

  // the mapping is already in the table
  else{

    // check if the existing function is a piecewise function
    robFunctionPiecewise* fnpw=dynamic_cast<robFunctionPiecewise*>(iter->second);

    // nope the function isn't a piecewise function...
    if( fnpw == NULL ) { 
      cout<<"robTrajectory::sigmoid: mapping conflict." << endl;
      return FAILURE;
    }
    
    // the function is a piecewise function so insert the new function in there
    else{

      // create the linear function
      robSigmoid* fnsig = new robSigmoid(ti, qi, tf, qf);
      fnpw->Insert( fnsig );

      // do we want to hold the position at the end
      if(sticky){
	robRnConstant* fncte = new robRnConstant( qf, tf );
	fnpw->Insert( fncte );
      }

      return SUCCESS;
    }
  }
}

robError robTrajectory::Translation( real ti, const SE3& Rti,
				     real tf, const SE3& Rtf,
				     uint64_t dof,
				     bool sticky ){
  
  if( tf < ti ){
    cout<<"robTrajectory::translation: initial time is later than final" <<endl;
    return FAILURE;
  }

  if( !(dof & (robDOF::TX|robDOF::TY|robDOF::TZ)) ){
    cout<<"robTrajectory::translation: expected translation DOF" <<endl;
    return FAILURE;
  }

  // create a mapping from time to joint positions
  robMapping mapping( robDOF( robDOF::TIME ), 
		      robDOF( dof & (robDOF::TX|robDOF::TY|robDOF::TZ) ) );

  // see if this mapping is already in the table
  std::map<robMapping, robFunction*>::iterator iter = functions.find(mapping);

  R3 Ti = Rti.Translation();
  R3 Tf = Rtf.Translation();

  // nope...the mapping isn't in the table...add a new one
  if( iter == functions.end() ) { 

    // create the linear function
    robLinear* fnlin = new robLinear( ti, Rn(3, Ti[0], Ti[1], Ti[2]), 
				      tf, Rn(3, Tf[0], Tf[1], Tf[2]) );
    robFunctionPiecewise* fnpw = new robFunctionPiecewise();
    fnpw->Insert( fnlin );

    // do we want to hold the position at the end
    if(sticky){
      robRnConstant* fncte = new robRnConstant( Rn(3, Tf[0], Tf[1], Tf[2]), tf );
      fnpw->Insert( fncte );
    }

    // this is used to hold the result from the insertion
    std::pair< std::map<robMapping, robFunction*>::iterator, bool > result;

    // insert the (mapping, function) pair in the table
    result = functions.insert( std::make_pair( mapping, fnpw ) );

    // if the insertion happened return right away
    if( result.second == true ){  return SUCCESS;  }

    // the insertion didn't work
    else{ 
      cout <<"robTrajectory::translation: Couldn't insert in the map" << endl;
      return FAILURE;
    }

  }

  // the mapping is already in the table
  else{

    // check if the existing function is a piecewise function
    robFunctionPiecewise* fnpw=dynamic_cast<robFunctionPiecewise*>(iter->second);

    // nope the function isn't a piecewise function...
    if( fnpw == NULL ) { 
      cout<<"robTrajectory::translation: mapping conflict." << endl;
      return FAILURE;
    }
    
    // the function is a piecewise function so insert the new function in there
    else{

      // create the linear function
      robLinear* fnlin = new robLinear(ti, Rn(3, Ti[0], Ti[1], Ti[2]), 
				       tf, Rn(3, Tf[0], Tf[1], Tf[2]) );
      fnpw->Insert( fnlin );

      // do we want to hold the position at the end
      if(sticky){
	robRnConstant* fncte = new robRnConstant(Rn(3, Tf[0], Tf[1], Tf[2]), tf);
	fnpw->Insert( fncte );
      }

      return SUCCESS;
    }
  }
}

robError robTrajectory::Rotation( real ti, const SE3& Rti,
				  real tf, const SE3& Rtf,
				  uint64_t dof,
				  bool sticky ){

  if( tf < ti ){
    cout<<"robTrajectory::rotation: initial time is later than final" <<endl;
    return FAILURE;
  }
  if( !(dof & (robDOF::RX|robDOF::RY|robDOF::RZ)) ){
    cout<<"robTrajectory::rotation: expected rotation DOF" <<endl;
    return FAILURE;
  }
  
  // create a mapping from time to joint positions
  robMapping mapping( robDOF( robDOF::TIME ), 
		      robDOF( dof & (robDOF::RX|robDOF::RY|robDOF::RZ) ) );
  
  // see if this mapping is already in the table
  std::map<robMapping, robFunction*>::iterator iter = functions.find(mapping);

  // nope...the mapping isn't in the table...add a new one
  if( iter == functions.end() ) { 

    // create the linear function
    robSLERP* fnslerp = new robSLERP( ti, Rti, tf, Rtf );
    robFunctionPiecewise* fnpw = new robFunctionPiecewise();
    fnpw->Insert( fnslerp );

    // do we want to hold the position at the end
    if(sticky){
      robSE3Constant* fncte = new robSE3Constant( Rtf, tf );
      fnpw->Insert( fncte );
    }

    // this is used to hold the result from the insertion
    std::pair< std::map<robMapping, robFunction*>::iterator, bool > result;

    // insert the (mapping, function) pair in the table
    result = functions.insert( std::make_pair( mapping, fnpw ) );

    // if the insertion happened return right away
    if( result.second == true ){  return SUCCESS;  }

    // the insertion didn't work
    else{ 
      cout <<"robTrajectory::rotation: Couldn't insert in the map" << endl;
      return FAILURE;
    }

  }

  // the mapping is already in the table
  else{

    // check if the existing function is a piecewise function
    robFunctionPiecewise* fnpw=dynamic_cast<robFunctionPiecewise*>(iter->second);

    // nope the function isn't a piecewise function...
    if( fnpw == NULL ) { 
      cout<<"robTrajectory::rotation: mapping conflict." << endl;
      return FAILURE;
    }
    
    // the function is a piecewise function so insert the new function in there
    else{

      // create the linear function
      robSLERP* fnslerp = new robSLERP( ti, Rti, tf, Rtf );
      fnpw->Insert( fnslerp );

      // do we want to hold the position at the end
      if(sticky){
	robSE3Constant* fncte = new robSE3Constant( Rtf, tf );
	fnpw->Insert( fncte );
      }

      return SUCCESS;
    }

  }
}
				      
robError robTrajectory::Linear( real ti, const SE3& Rti,
				real tf, const SE3& Rtf,
				uint64_t dof, 
				bool sticky ){
  
  Translation(ti, Rti, tf, Rtf, dof, sticky);
  Rotation(ti, Rti, tf, Rtf, dof, sticky);
  return SUCCESS;
}
  
robError robTrajectory::Evaluate( robDOF& output ){

  // make sure that the domain is there
  if( device == NULL ){
    cout<<"robTrajectory::evaluate: NULL device."<<endl;
    return FAILURE;
  }

  // invoke the domain to get some input for the functions
  robDOFRn fninput;
  fninput = device->generate();

  // evaluate each function in the maps
  std::map<robMapping, robFunction*>::iterator iter;

  for(iter=functions.begin(); iter!=functions.end(); iter++){

    robMapping mapping = iter->first;          // the dof mask
    robFunction* function = iter->second;  // the function

    if( mapping.To().IsReal() ){
      try{
	robDOFRn& fnoutput = dynamic_cast<robDOFRn&>(output);
	robDOFRn dofrn;
	dofrn.Set( robDOF::XPOS );
	function->Evaluate( fninput, dofrn );
	fnoutput.x = dofrn.x;
	fnoutput.xd = dofrn.xd;
	fnoutput.xdd = dofrn.xdd;
      }
      catch(std::bad_cast){
	cout<< "robTrajectory::evaluate: expected robDOFRn parameter" << endl;
	return FAILURE;
      }
    }

    if( mapping.To().IsCartesian() ){
      try{
	robDOFSE3& fnoutput = dynamic_cast<robDOFSE3&>(output);

	if(mapping.To().IsTranslation()){
	  robDOFRn doft;
	  doft.Set( robDOF::TX | robDOF::TY | robDOF::TZ );
	  if( function->Evaluate( fninput, doft ) == SUCCESS ){
	    fnoutput.Rt[0][3] = doft.x.at(0);
	    fnoutput.Rt[1][3] = doft.x.at(1);
	    fnoutput.Rt[2][3] = doft.x.at(2);
	    fnoutput.vw[0]    = doft.xd.at(0);
	    fnoutput.vw[1]    = doft.xd.at(1);
	    fnoutput.vw[2]    = doft.xd.at(2);
	    fnoutput.vdwd[0]  = doft.xdd.at(0);
	    fnoutput.vdwd[1]  = doft.xdd.at(1);
	    fnoutput.vdwd[2]  = doft.xdd.at(2);
	  }
	}
	if(mapping.To().IsRotation()){
	  robDOFSE3 dofR;
	  dofR.Set( robDOF::RX | robDOF::RY | robDOF::RZ );
	  if( function->Evaluate( fninput, dofR ) == SUCCESS ){
	    fnoutput.Rt.Rotation() = dofR.Rt.Rotation();
	    fnoutput.vw[3]    = dofR.vw[3];
	    fnoutput.vw[4]    = dofR.vw[4];
	    fnoutput.vw[5]    = dofR.vw[5];
	    fnoutput.vdwd[3]  = dofR.vdwd[3];
	    fnoutput.vdwd[4]  = dofR.vdwd[4];
	    fnoutput.vdwd[5]  = dofR.vdwd[5];
	  }
	}
      }
      catch(std::bad_cast){
	cout<< "robTrajectory::evaluate: expected robDOFSE3 parameter" << endl;
	return FAILURE;
      }
    }
  }
  
  return SUCCESS;
}
