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

#include <cisstRobot/robManipulator.h>
#include <cisstDevices/glut/devGLUT.h>
#include <cisstDevices/glut/devGLUTManipulator.h>

devGLUTManipulator::devGLUTManipulator( const std::string& devname,
					double period,
					const std::string& robotfn,
					const vctFrame4x4<double>& Rtw0,
					const vctDynamicVector<double>& qinit,
					const std::vector<std::string>& geomsfn,
					const std::string& basefn ) :

  devManipulator( devname, period, qinit.size() ),
  robManipulator( robotfn, Rtw0 ),
  base(NULL){

  if( !basefn.empty() ){
    base = new devMeshTriangular();
    base->LoadOBJ( basefn );
    devGLUT::Register( base );
  }

  for( size_t i=0; i<links.size(); i++ ){
    devMeshTriangular* mesh = new devMeshTriangular();
    mesh->LoadOBJ( geomsfn[i] );
    devGLUT::Register( mesh );
    meshes.push_back( mesh );
   }

  Write( qinit );

}

vctDynamicVector<double> devGLUTManipulator::Read()
{ return q; }

void devGLUTManipulator::Write( const vctDynamicVector<double>& q ){ 
  this->q = q;

  //if( q.size() == meshes.size() ){
    for( size_t i=0; i<meshes.size(); i++ ){
      vctFrame4x4<double> Rtwi = ForwardKinematics( q, i+1 );
      meshes[i]->SetPositionOrientation( Rtwi );
    }
    //}
  //std::cout << "size( meshes ) = " << meshes.size()
  //	    << " size( q ) = " << q.size() << std::endl;
}

