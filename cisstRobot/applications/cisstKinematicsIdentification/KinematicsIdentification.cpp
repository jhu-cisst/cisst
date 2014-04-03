/*

  Author(s):  Simon Leonard
  Created on: 2010

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---

*/

#include <cisstRobot/robManipulator.h>
#include <cisstRobot/robDH.h>
#include <cisstRobot/robModifiedDH.h>
#include <cisstRobot/robHayati.h>
#include <cisstNumerical/nmrLSSolver.h>

void AugmentSystem( vctDynamicMatrix<double>& J,
		    const vctDynamicMatrix<double>& Ji ){

  size_t M = J.rows();
  size_t N = Ji.cols();

  J.resize( M+6, N );

  for( size_t i=0; i<6; i++ ){
    for( size_t j=0; j<N; j++ ){
      J[M+i][j] = Ji[i][j];
    }
  }

}

void AugmentMeasurements( vctDynamicVector<double>& vw,
			  const vctFrame4x4<double>& vwi ){

  size_t N = vw.size();
  vw.resize( N+6 );

  vw[N+0] = vwi[0][3];
  vw[N+1] = vwi[1][3];
  vw[N+2] = vwi[2][3];
  vw[N+3] = (vwi[2][1] - vwi[1][2])/2.0;
  vw[N+4] = (vwi[0][2] - vwi[2][0])/2.0;
  vw[N+5] = (vwi[1][0] - vwi[0][1])/2.0;

}

extern "C" {
  void dgelss_( CISSTNETLIB_INTEGER* M,
		CISSTNETLIB_INTEGER* N,
		CISSTNETLIB_INTEGER* NRHS,
		CISSTNETLIB_DOUBLE* A,
		CISSTNETLIB_INTEGER* LDA,
		CISSTNETLIB_DOUBLE* B,
		CISSTNETLIB_INTEGER* LDB,
		CISSTNETLIB_DOUBLE* S,
		CISSTNETLIB_DOUBLE* RCOND,
		CISSTNETLIB_INTEGER* RANK,
		CISSTNETLIB_DOUBLE* WORK,
		CISSTNETLIB_INTEGER* LWORK,
		CISSTNETLIB_INTEGER* INFO );
}

// special sauce
vctDynamicVector<double> SolveLSMinNorm( const vctDynamicMatrix<double>& a,
					 const vctDynamicVector<double>& b ){

  CISSTNETLIB_INTEGER M = a.rows();
  CISSTNETLIB_INTEGER N = a.cols();
  CISSTNETLIB_INTEGER NRHS = 1;

  CISSTNETLIB_DOUBLE* A = (CISSTNETLIB_DOUBLE*)a.Pointer();
  CISSTNETLIB_INTEGER LDA = M;

  // ensure that B is large enough for either M or N
  CISSTNETLIB_DOUBLE* B=NULL;
  CISSTNETLIB_INTEGER LDB;
  if( N < M )    { B = new double[M]; LDB = M; }
  else           { B = new double[N]; LDB = N; }
  for( int i=0; i<N; i++ )    { B[i] = 0.0; }
  for( int i=0; i<M; i++ )    { B[i] = b[i]; }

  CISSTNETLIB_DOUBLE* S = new double[M];
  CISSTNETLIB_DOUBLE RCOND = 1e-6;       // drop singular values threshold
  CISSTNETLIB_INTEGER RANK;

  CISSTNETLIB_DOUBLE  WORK[512];         // fudged size.
  CISSTNETLIB_INTEGER LWORK = 512;       // fudged size

  CISSTNETLIB_INTEGER INFO;

  dgelss_( &M, &N, &NRHS,       // 6xN matrix
	   A, &LDA,             // Jacobian matrix
	   B, &LDB,             // error vector
	   S, &RCOND, &RANK,    // SVD parameters
	   WORK, &LWORK, &INFO );

  if( INFO < 0 )
    { std::cerr << "The " << -INFO << "th argument has an illegal value\n"; }
  if( 0 < INFO )
    { std::cerr << "SVD failed to converge\n"; }

  vctDynamicVector<double> solution( N, 0.0 );
  for( int i=0; i<N; i++ )
    { solution[i] = B[i]; }

  delete[] S;
  delete[] B;

  return solution;

}

int main( int argc, char** argv ){

  cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );

  std::ostringstream usage;
  usage  << "Usage: " << std::endl
	 << argv[0] << " robfile qfile se3file";

  if( argc != 4 ){
    std::cerr << usage.str() << std::endl;
    return -1;
  }

  // load the manipulator
  robManipulator manipulator( argv[1] );


  // load the joints
  std::vector< vctDynamicVector<double> > Q;
  std::ifstream qfile( argv[2] );
  if( !qfile ){
    std::cerr << argv[0] << ": cannot find file " << argv[2] << std::endl;
    return -1;
  }

  while( !qfile.eof() ){
    vctDynamicVector<double> q( manipulator.links.size(), 0.0 );
    if( q.FromStreamRaw( qfile ) )
      { Q.push_back( q ); }
    else
      break;
  }


  std::vector< vctFrame4x4<double> > RT;
  std::ifstream se3file( argv[3] );
  if( !se3file ){
    std::cerr << argv[0] << ": cannot find file " << argv[3] << std::endl;
    return -1;
  }

  while( !se3file.eof() ){
    vctFrame4x4<double> Rt;
    if( Rt.FromStreamRaw( se3file ) )
      { RT.push_back( Rt ); }
    else
      break;
  }

  if( Q.size() != RT.size() ){
    std::cerr << argv[0] << ": Got "
	      << Q.size() << " joint vectors and "
	      << RT.size() << " measurement"
	      << std::endl;
    return -1;
  }

  double tolerance = 1e-5;
  double best_error = 1000000.0;

  // some phony increment
  vctDynamicVector<double> dx( 1, 1.0 );

  while( tolerance < dx.Norm() ){

    if (best_error > dx.Norm()) {
      CMN_LOG_RUN_DEBUG << "best error updated: " << dx.Norm() << std::endl;
      best_error = dx.Norm();
    }

    // these must be column major
    vctDynamicMatrix<double> J(0,0,VCT_COL_MAJOR);
    vctDynamicVector<double> vw;

    for( size_t i=0; i<Q.size(); i++ ){

      // Add the equations
      AugmentSystem( J, manipulator.JacobianKinematicsIdentification( Q[i],
								      0.001) );

      // Forward kinematics from computed model
      vctFrame4x4<double> Rt = manipulator.ForwardKinematics( Q[i] );

      // Forward kinematics from the measurement
      vctFrame4x4<double> Rt_ = RT[i];

      // difference: measurement - computed
      vctFixedSizeMatrix<double,4,4> dT;
      for( size_t r=0; r<4; r++ ){
	for( size_t c=0; c<4; c++ ){
	  dT[r][c] = Rt_[r][c] - Rt[r][c];
	}
      }

      Rt.InverseSelf();

      // Add the measurements
      AugmentMeasurements( vw, Rt * dT );

    }

    // solve the system
    dx = SolveLSMinNorm( J, vw );

    // update the parameters. This is messy...
    for( size_t i=0, cnt=0; i<manipulator.links.size(); i++ ){

      // fetch the kinematics of the ith link
      robKinematics* ki = manipulator.links[i].GetKinematics();

      switch( ki->GetConvention() ){
      case robKinematics::STANDARD_DH:
	{
	  robDH* dh = dynamic_cast<robDH*>( ki );
	  dh->SetRotationX(    dh->GetRotationX()    + dx[cnt++] );
	  dh->SetTranslationX( dh->GetTranslationX() + dx[cnt++] );
	  dh->SetRotationZ(    dh->GetRotationZ()    + dx[cnt++] );
	  dh->SetTranslationZ( dh->GetTranslationZ() + dx[cnt++] );
	}
	break;


      case robKinematics::MODIFIED_DH:
	{
	  robModifiedDH* mdh = dynamic_cast<robModifiedDH*>( ki );
	  mdh->SetRotationX(    mdh->GetRotationX()    + dx[cnt++] );
	  mdh->SetTranslationX( mdh->GetTranslationX() + dx[cnt++] );
	  mdh->SetRotationZ(    mdh->GetRotationZ()    + dx[cnt++] );
	  mdh->SetTranslationZ( mdh->GetTranslationZ() + dx[cnt++] );
	}
	break;

      case robKinematics::HAYATI:
	{
	  robHayati* h = dynamic_cast<robHayati*>( ki );

	  switch( h->GetType() ){
	  case robJoint::HINGE:
	    {
	      h->SetRotationX(    h->GetRotationX()    + dx[cnt++] );
	      h->SetRotationY(    h->GetRotationY()    + dx[cnt++] );
	      h->SetRotationZ(    h->GetRotationZ()    + dx[cnt++] );
	      h->SetTranslationX( h->GetTranslationX() + dx[cnt++] );
	    }
	    break;
	  case robJoint::SLIDER:
	    {
	      h->SetRotationX(    h->GetRotationX()    + dx[cnt++] );
	      h->SetRotationY(    h->GetRotationY()    + dx[cnt++] );
	      h->SetTranslationZ( h->GetTranslationZ() + dx[cnt++] );
	    } // case robJoint::SLIDER:
	    break;

	  default:
	    break;

	  } // switch( h->GetType() )

	} // case robKinematics::HAYATI
	break;

      default:
	break;

      }  // switch( ki->GetConvention() )

    } // for

  }

  std::cout.precision(4);
  std::cout.setf( std::ios::fixed );
  manipulator.PrintKinematics( std::cout );

  return 0;
}
