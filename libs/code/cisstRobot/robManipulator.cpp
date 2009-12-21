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

#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robManipulator.h>
#include <cisstRobot/robGUI.h>

#include <cisstVector/vctQuaternionRotation3.h>
#include <cisstVector/vctFixedSizeMatrix.h>

#include <vector>
#include <iostream>
using namespace std;

#ifdef SINGLE_PRECISION

#define potrf spotrf_
#define potri spotri_
#define symm  ssymm_
#define gemm  sgemm_
#define symv  ssymv_
#define gemv  sgemv_
#define gesv  sgesv_
#define gelss sgelss_
#define nrm2  snrm2_

#else

#define potrf dpotrf_
#define potri dpotri_
#define symm  dsymm_
#define gemm  dgemm_
#define symv  dsymv_
#define gemv  dgemv_
#define gesv  dgesv_
#define gelss dgelss_
#define nrm2  dnrm2_

#endif

extern "C" {

  void potrf(char *uplo, int *n, double *a, int *lda, int *info);
  void potri(char *uplo, int *n, double *a, int *lda, int *info);
  void symm(char *side, char *uplo, int *m, int *n,
            double *alpha, double *a, int *lda,
            double* b, int *ldb, double *beta,
            double *c, int *ldc);
  void gemm(char *transa, char *transb, 
	    int *m, int *n, int *k,
            double *alpha, double *a, int *lda,
                         double *b, int *ldb, 
	    double *beta,  double *c, int *ldc);
  void symv(char *uplo, int *n,
            double *alpha, double *a, int *lda,
            double *x, int *incx, double *beta,
            double *y, int *incy);
  void gemv(char *trans, int *m, int *n,
            double *alpha, double *a, int *lda,
            double *x, int *incx, double *beta,
            double *y, int *incy);
  void  gesv(int *N, int *NRHS,
             double *A, int *LDA, int *IPIV,
             double *B, int *LDB, int *INFO);
  void gelss(int* M, int* N, int* NRHS,
	     double* A, int* LDA, 
	     double* B, int* LDB, 
	     double* S, double* RCOND, int* RANK, 
	     double* WORK, int* LWORK, int* INFO );

  double nrm2(int *N, double* X, int *INC);

}

#define NR_END 1
#define FREE_ARG char*

double** rmatrix(long nrl, long nrh, long ncl, long nch){
  long i, nrow=nrh-nrl+1, ncol=nch-ncl+1;
  double **m;

  m=(double **)malloc((size_t)((nrow+NR_END)*sizeof(double*)));
  //if(!m) nrerror("allocation failure 1 in matrix()");
  m += NR_END;
  m -= nrl;

  m[nrl]=(double *)malloc((size_t)((nrow*ncol+NR_END)*sizeof(double)));
  //if(!m[nrl]) nrerror("allocation failure 2 in matrix()");
  m[nrl] += NR_END;
  m[nrl] -= ncl;

  for(i=nrl+1; i<=nrh; i++) m[i]=m[i-1]+ncol;

  return m;
}

void free_rmatrix(double** m, long nrl, long ncl){
  free((FREE_ARG) (m[nrl]+ncl-NR_END));
  free((FREE_ARG) (m+nrl-NR_END));
}

robManipulator::robManipulator( const std::string& linkfile, 
				const vctFrame4x4<double,VCT_ROW_MAJOR>& Rtw0 ){
  
  this->tool = NULL;
  this->Rtw0 = Rtw0;
  
  if( LoadRobot( linkfile ) == ERROR ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to load the robot configuration."
		      << std::endl;
  }
}
				
robError robManipulator::LoadRobot( const std::string& filename ){

  if( filename.empty() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " No configuration file!."
		      << std::endl;
    return ERROR;
  }

  std::ifstream ifs;
  ifs.open( filename.data() );
  if(!ifs){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Couln't open configuration file " << filename 
		      << std::endl;
    return ERROR;
  }

  size_t N;       // the number of links
  ifs >> N;
  
  // read the links (kinematics+dynamics+geometry) from the input
  for( size_t i=0; i<N; i++ ){
    robLink li;
    li.Read( ifs );
    links.push_back( li );
  }

  // Insert the geometry in opengl
  for( size_t i=0; i<links.size(); i++)
    robGUI::Insert( &(links[i]) );

  Js = rmatrix(0, links.size()-1, 0, 5);
  Jn = rmatrix(0, links.size()-1, 0, 5);

  return SUCCESS;
}


//////////////////////////////////////
//         KINEMATICS
//////////////////////////////////////

vctFrame4x4<double,VCT_ROW_MAJOR> 
robManipulator::ForwardKinematics( const vctDynamicVector<double>& q ) {

  // q must have at least links.size() values. If q has more values they will
  // be passed to the tool
  if( q.size() < links.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected at least " << links.size() << " values. "
		      << "Got " << q.size()
		      << std::endl;
    return Rtw0;
  }

  // If there no tool (possibly with joint), then the number of values
  // must match the number of joints
  if( tool == NULL && (links.size() < q.size()) ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected exactly " << links.size() << " values. "
		      << " Got " << q.size()
		      << std::endl;
    return Rtw0;
  }

  // no link? then return the transformation of the base
  if( links.empty() ){
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< ": Manipulator with no link."
			<< std::endl;
    return Rtw0;
  }

  // set the position/orientation of link 0 to the base x its tranformation
  // setting the link's transformation is necessary in order to render the link
  // in opengl
  links[0] = Rtw0 * links[0].ForwardKinematics( q[0] );

  // for link 1 to N
  for(size_t i=1; i<links.size(); i++){
    // set the position/orientation of the link
    links[i] = links[i-1] * links[i].ForwardKinematics( q[i] );
  }

  if( tool != NULL ){
    // copy the remaining joints values and give them to the tool
    vctDynamicVector<double> toolq( q.size() - links.size() );
    for( size_t i=0; i<q.size()-links.size(); i++ )
      toolq[i] = q[ links.size() + i ];
    return links.back() * tool->ForwardKinematics( toolq );
  }
  else
    return links.back();
}

vctFrame4x4<double,VCT_ROW_MAJOR> 
robManipulator::ForwardKinematics( const vctDynamicVector<double>& q ) const {

  // q must have at least links.size() values. If q has more values they will
  // be passed to the tool
  if( q.size() < links.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected at least " << links.size() << " values. "
		      << "Got " << q.size()
		      << std::endl;
    return Rtw0;
  }

  // If there's no tool, then the number of values must match the number of 
  // joints
  if( tool == NULL && (links.size() < q.size()) ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected exactly " << links.size() << " values. "
		      << " Got " << q.size()
		      << std::endl;
    return Rtw0;
  }

  // no link? then return the transformation of the base
  if( links.empty() ){
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< ": Manipulator with no link."
			<< std::endl;
    return Rtw0;
  }

  // set the position/orientation of link 0 to the base * its tranformation
  // setting the link's transformation is necessary in order to render the link
  // in opengl
  vctFrame4x4<double,VCT_ROW_MAJOR> Rtwi =Rtw0*links[0].ForwardKinematics(q[0]);

  // for link 1 to N
  for(size_t i=1; i<links.size(); i++)
    Rtwi = Rtwi * links[i].ForwardKinematics( q[i] );

  if( tool != NULL ){
    // copy the remaining joints values and give them to the tool
    vctDynamicVector<double> toolq( q.size() - links.size() );
    for( size_t i=0; i<q.size()-links.size(); i++ )
      toolq[i] = q[ links.size() + i ];
    return links.back() * tool->ForwardKinematics( toolq );
  }
  
  return Rtwi;
}

robError 
robManipulator::InverseKinematics( vctDynamicVector<double>& q, 
				   const vctFrame4x4<double,VCT_ROW_MAJOR>& Rts,
				   double tolerance, 
				   size_t Niterations ){

  if( q.size() != links.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected " << links.size() << " joints values. "
		      << " Got " << q.size() 
		      << std::endl;
    return ERROR;
  }

  if( links.size() == 0 ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": The manipulator has no links."
		      << std::endl;
    return ERROR;
  }

  // A is a pointer to the 6xN spatial Jacobian
  int M = 6;                  // The number or rows of matrix A
  int N = links.size();      // The number of columns of matrix A
  int NHRS = 1;               // The number of right hand side vectors

  double* A = &(Js[0][0]);    // Pointer to the spatial Jacobian
  int LDA = M;                // The leading dimension of the array A.

  // B is a pointer the the N vector containing the solution
  double* B;                  // The N-by-NRHS matrix of right hand side matrix
  int LDB = N;                // The leading dimension of the array B.

  // These values are used for the SVD computation
  double* S = new double[M];  // The singular values of A in decreasing order
  double RCOND = -1;          // Use machine precision to determine rank(A)
  int RANK;                   // The effective rank of A
  int LWORK = 128;            // The (safe) size of the workspace
  double WORK[128];           // The workspace
  int INFO;                   // The info code
  int INC = 1;                // The index increment 

  double ndq=1;               // norm of the iteration error
  size_t i;                   // the iteration counter

  // loop until Niter are executed or the error is bellow the tolerance
  for( i=0; i<Niterations && tolerance<ndq; i++ ){

    // Evaluate the forward kinematics
    vctFrame4x4<double,VCT_ROW_MAJOR> Rt = ForwardKinematics( q );
    // Evaluate the spatial Jacobian (also evaluate the forward kin)
    JacobianSpatial( q );

    // compute the translation error
    vctFixedSizeVector<double,3> dt( Rts[0][3]-Rt[0][3],   
				     Rts[1][3]-Rt[1][3],  
				     Rts[2][3]-Rt[2][3] );
    
    // compute the orientation error 
    // first build the [ n o a ] vectors
    vctFixedSizeVector<double,3> n1( Rt[0][0],  Rt[1][0],  Rt[2][0] );
    vctFixedSizeVector<double,3> o1( Rt[0][1],  Rt[1][1],  Rt[2][1] );
    vctFixedSizeVector<double,3> a1( Rt[0][2],  Rt[1][2],  Rt[2][2] );
    vctFixedSizeVector<double,3> n2( Rts[0][0], Rts[1][0], Rts[2][0] );
    vctFixedSizeVector<double,3> o2( Rts[0][1], Rts[1][1], Rts[2][1] );
    vctFixedSizeVector<double,3> a2( Rts[0][2], Rts[1][2], Rts[2][2] );

    // This is the orientation error
    vctFixedSizeVector<double,3> dr = 0.5*( (n1%n2) + (o1%o2) + (a1%a2) );

    // combine both errors in one R^6 vector
    double e[6] = { dt[0], dt[1], dt[2], dr[0], dr[1], dr[2] };
    // get a pointer
    B = &e[0];

    // compute the minimum norm solution
    gelss( &M, &N, &NHRS,       // 6xN matrix
	   A, &LDA,             // Jacobian matrix
	   B, &LDB,             // error vector
	   S, &RCOND, &RANK,    // SVD parameters
	   WORK, &LWORK, &INFO );
    
    // process the errors (if any)
    if(INFO < 0)
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": the i-th argument of gelss is illegal." 
			<< std::endl;
    if(0 < INFO)
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": gelss failed to converge."
			<< std::endl;
    
    // compute the L2 norm of the error
    ndq = nrm2(&N, B, &INC);

    // update the solution
    for(size_t j=0; j<links.size(); j++) q[j] += B[j];
  }
  
  // copy the joint values and 
  for(size_t j=0; j<links.size(); j++){
    q[j] = fmod((double)q[j], (double)2.0*M_PI);
    if(M_PI < q[j])
      q[j] = q[j] - 2.0*M_PI;
  }

  delete[] S;

  if( i==Niterations ) return ERROR;
  else return SUCCESS;
}

/*
 * Body manipulator Jacobian
 * Paul IEEE SMC 11(6) 1981
 * BIG FAT WARNIN': The jacobian is in column major (for Fortran)
 */
void robManipulator::JacobianBody( const vctDynamicVector<double>& q ) const {

  vctFrame4x4<double,VCT_ROW_MAJOR> U;  // set identity

  if( tool != NULL ){
    //U = tool->ForwardKinematics( 0.0 ); // set to tool if any
  }

  for(int j=(int)links.size()-1; 0<=j; j--){

    if( links[j].DHConvention() == robDHStandard ){      // DH convention
      U = links[j].ForwardKinematics( q[j] ) * U;
    }

    if( links[j].JointType() == robJointHinge ){         // Revolute joint
      // Jn is column major
      Jn[j][0] = U[0][3]*U[1][0] - U[1][3]*U[0][0];
      Jn[j][1] = U[0][3]*U[1][1] - U[1][3]*U[0][1];
      Jn[j][2] = U[0][3]*U[1][2] - U[1][3]*U[0][2];

      Jn[j][3] = U[2][0]; // nz
      Jn[j][4] = U[2][1]; // oz
      Jn[j][5] = U[2][2]; // az

    }

    if( links[j].JointType() == robJointSlider ){   // Prismatic joint
      // Jn is column major
      Jn[j][0] = U[2][0]; // nz
      Jn[j][1] = U[2][1]; // oz
      Jn[j][2] = U[2][2]; // az

      Jn[j][3] = 0.0;
      Jn[j][4] = 0.0;
      Jn[j][5] = 0.0;

    }

    if( links[j].DHConvention() == robDHModified){  // Modified DH
      U = links[j].ForwardKinematics( q[j] ) * U;
    }
  }
}

void robManipulator::JacobianSpatial( const vctDynamicVector<double>& q ) const {
  
  JacobianBody( q );

  /*
   * Get the adjoint matrix to flip the body jacobian to spatial jacobian
   */
  vctFrame4x4<double,VCT_ROW_MAJOR> Rt0n = ForwardKinematics( q );
  double Ad[6][6];//( this->FK() );

  // Build the adjoint matrix
  // upper left block
  Ad[0][0] = Rt0n[0][0];     Ad[0][1] = Rt0n[0][1];     Ad[0][2] = Rt0n[0][2];
  Ad[1][0] = Rt0n[1][0];     Ad[1][1] = Rt0n[1][1];     Ad[1][2] = Rt0n[1][2];
  Ad[2][0] = Rt0n[2][0];     Ad[2][1] = Rt0n[2][1];     Ad[2][2] = Rt0n[2][2];

  // upper right block
  Ad[0][3] = -Rt0n[2][3]*Rt0n[1][0] + Rt0n[1][3]*Rt0n[2][0];
  Ad[0][4] = -Rt0n[2][3]*Rt0n[1][1] + Rt0n[1][3]*Rt0n[2][1];
  Ad[0][5] = -Rt0n[2][3]*Rt0n[1][2] + Rt0n[1][3]*Rt0n[2][2];

  Ad[1][3] =  Rt0n[2][3]*Rt0n[0][0] - Rt0n[0][3]*Rt0n[2][0];
  Ad[1][4] =  Rt0n[2][3]*Rt0n[0][1] - Rt0n[0][3]*Rt0n[2][1];
  Ad[1][5] =  Rt0n[2][3]*Rt0n[0][2] - Rt0n[0][3]*Rt0n[2][2];

  Ad[2][3] = -Rt0n[1][3]*Rt0n[0][0] + Rt0n[0][3]*Rt0n[1][0];
  Ad[2][4] = -Rt0n[1][3]*Rt0n[0][1] + Rt0n[0][3]*Rt0n[1][1];
  Ad[2][5] = -Rt0n[1][3]*Rt0n[0][2] + Rt0n[0][3]*Rt0n[1][2];

  // lower left block
  Ad[3][0] = 0.0;            Ad[3][1] = 0.0;            Ad[3][2] = 0.0; 
  Ad[4][0] = 0.0;            Ad[4][1] = 0.0;            Ad[4][2] = 0.0; 
  Ad[5][0] = 0.0;            Ad[5][1] = 0.0;            Ad[5][2] = 0.0; 

  // lower right block
  Ad[3][3] = Rt0n[0][0];     Ad[3][4] = Rt0n[0][1];     Ad[3][5] = Rt0n[0][2];
  Ad[4][3] = Rt0n[1][0];     Ad[4][4] = Rt0n[1][1];     Ad[4][5] = Rt0n[1][2];
  Ad[5][3] = Rt0n[2][0];     Ad[5][4] = Rt0n[2][1];     Ad[5][5] = Rt0n[2][2];

  char TRANSA[] = "T";  // op(Ad): transpose of Ad coz it's row major
  char TRANSB[] = "N";  // op(Jn) 
  int M = 6;            // specifies  the number  of rows  of op( Ad )
  int N = links.size();// specifies the number  of columns of the matrix Jn
  int K = 6;            // specifies  the number of columns of op( Ad )
  double ALPHA = 1.0;     // C := alpha*op( A )*op( B ) + beta*C
  double* A = &Ad[0][0];  // 
  int LDA = 6;          // specifies the first dimension of A 
  double* B = &Jn[0][0];  // 
  int LDB = 6;          // specifies  the first dimension of B 
  double BETA = 0.0;      // C := alpha*op( A )*op( B ) + beta*C
  double* C = &Js[0][0];  // 
  int LDC = 6;          // specifies the first dimension of C

  // Js = Ad * Jn
  gemm(TRANSA, TRANSB,
       &M, &N, &K, 
       &ALPHA, A, &LDA,
               B, &LDB, 
       &BETA,  C, &LDC);
}


//////////////////////////////////////
//         DYNAMICS
//////////////////////////////////////

vctDynamicVector<double> 
robManipulator::RNE( const vctDynamicVector<double>& q,
		     const vctDynamicVector<double>& qd,
		     const vctDynamicVector<double>& qdd,
		     const vctFixedSizeVector<double,6>& fext,
		     double g ) const {

  vctFixedSizeVector<double,3> w    (0.0); // angular velocity
  vctFixedSizeVector<double,3> wd   (0.0); // angular acceleration
  vctFixedSizeVector<double,3> v    (0.0); // linear velocity
  vctFixedSizeVector<double,3> vd   (0.0); // linear acceleration
  vctFixedSizeVector<double,3> vdhat(0.0); 

  //total moment exerted on each link
  std::vector<vctFixedSizeVector<double,3> > N(links.size(),
					       vctFixedSizeVector<double,3>(0));
  //total force exerted on each link 
  std::vector<vctFixedSizeVector<double,3> > F(links.size(),
					       vctFixedSizeVector<double,3>(0));
  // torques
  vctDynamicVector<double>                  tau(links.size(), 0.0);

  // The axis pointing "up"
  vctFixedSizeVector<double,3> z0(0.0, 0.0, 1.0);

  // acceleration of link 0
  // extract the rotation of the base and map the vector [0 0 1] in the robot
  // coordinate frame
  vctMatrixRotation3<double,VCT_ROW_MAJOR> R(Rtw0[0][0], Rtw0[0][1],Rtw0[0][2],
					     Rtw0[1][0], Rtw0[1][1],Rtw0[1][2],
					     Rtw0[2][0], Rtw0[2][1],Rtw0[2][2]);
  vd = R.Transpose() * z0 * g;

  // Forward recursion
  for(size_t i=0; i<links.size(); i++){

    double                                       m; // mass
    vctFixedSizeVector<double,3>                 s; // center of mass
    vctFixedSizeMatrix<double,3,3,VCT_ROW_MAJOR> I; // moment of inertia
    vctMatrixRotation3<double,VCT_ROW_MAJOR>     A; // iA(i-1)
    vctFixedSizeVector<double,3>                ps; // distal link

    m  = links[i].Mass();
    s  = links[i].CenterOfMass();
    I  = links[i].MomentOfInertia();

    A  = links[i].Orientation( q[i] ).InverseSelf();
    ps = links[i].PStar();

    wd = A*( wd + (z0*qdd[i]) + (w%(z0*qd[i])) ); // angular acceleration wrt i
    w  = A*( w  + (z0*qd[i]) );                   // angular velocity
    vd = (wd%ps) + (w%(w%ps)) + A*vd;             // linear acceleration

    vdhat = (wd%s) + (w%(w%s)) + vd;              // 
    F[i] = m*vdhat;                               // total force
    N[i] = (I*wd) + (w%(I*w));                    // total moment
  }

  // external force applied on the TCP
  vctFixedSizeVector<double,3> f( fext[0], fext[1], fext[2] );
  // external moment applied on the TCP 
  vctFixedSizeVector<double,3> n( fext[3], fext[4], fext[5] );

  // Backward recursion
  for(int i=(int)links.size()-1; 0<=i; i--){
    vctMatrixRotation3<double,VCT_ROW_MAJOR> A;
    vctFixedSizeVector<double,3> ps = links[i].PStar();
    vctFixedSizeVector<double,3> s  = links[i].CenterOfMass();

    if(i != (int)links.size()-1)              // 
      A = links[i+1].Orientation( q[i+1] );    // 

    f = A*f + F[i];                            // force exterted on i by i-1
    n = A*n + (ps%f) + (s%F[i]) + N[i];        // moment externed on i by i-1
    A = links[i].Orientation(q[i]).InverseSelf(); // 

    if (links[i].JointType() == robJointHinge )
      tau[i] = n*(A*z0);                       // 
    if( links[i].JointType() == robJointSlider )
      tau[i] = f*(A*z0);                       // 

  }

  return tau;
}

vctDynamicVector<double> 
robManipulator::CCG( const vctDynamicVector<double>& q,
		     const vctDynamicVector<double>& qd ) const {
  if( q.size() != qd.size() ){
    std::cerr << "robManipulator::CCG: vectors must have the same size." 
	      << std::endl;
    return vctDynamicVector<double>();
  }

  return RNE( q,           // call Newton-Euler with only the joints 
	      qd,          // positions and the joints velocities
	      vctDynamicVector<double>( q.size(), 0.0 ) );
}

vctFixedSizeVector<double,6> 
robManipulator::BiasAcceleration( const vctDynamicVector<double>& q,
				  const vctDynamicVector<double>& qd ) const {

  vctFixedSizeVector<double,3> w (0.0); // angular velocity
  vctFixedSizeVector<double,3> wd(0.0); // angular acceleration
  vctFixedSizeVector<double,3> vd(0.0); // linear velocity

  vctFixedSizeVector<double,3> z0(0.0, 0.0, 1.0);

  for(size_t i=0; i<links.size(); i++){

    vctMatrixRotation3<double,VCT_ROW_MAJOR> A;    // iA(i-1)
    vctFixedSizeVector<double,3>            ps;

    A  = links[i].Orientation( q[i] ).InverseSelf();
    ps = links[i].PStar();                         // distal

    wd = A*( wd + ( w%(z0*qd[i]) ) );
    w  = A*( w  + (    z0*qd[i]  ) );
    vd = (wd%ps) + (w%(w%ps)) + A*vd;
  }

  return vctFixedSizeVector<double,6>(vd[0], vd[1], vd[2], wd[0], wd[1], wd[2]);
}

// A is column major!
void 
robManipulator::JSinertia(double **A,
			  const vctDynamicVector<double>& q) const{

  if( q.size() != links.size() ){
    std::cerr << "robManipulator::JSinertia: Expected " << links.size()
	      << " values. Got " << q.size()
	      << std::endl;
    return;
  }

  for(size_t c=0; c<links.size(); c++){
    vctDynamicVector<double> qd( q.size(), 0.0 ); // velocities to zero
    vctDynamicVector<double> qdd(q.size(), 0.0 ); // accelerations to zero
    vctFixedSizeVector<double,6> fext(0.0);
    qdd[c] = 1.0;                                 // ith acceleration to 1

    vctDynamicVector<double> h = RNE( q, qd, qdd, fext, 0  );
    for( size_t r=0; r<links.size(); r++ )
      A[c][r] = h[r];
  }

}

// Ac is column major!
void robManipulator::OSinertia(double Ac[6][6], 
			       const vctDynamicVector<double>& q) const {
  char SIDE = 'R';                // dsymm C := alpha*B*A + beta*C,
  char UPLO = 'L';                // lower triangular (upper triangular for CM)
  char TRANST = 'T';
  char TRANSN = 'N';
  double ALPHA = 1.0;
  double BETA = 0.0;                //
  int NEQS = 6;                   // num rows of the jacobian
  int NJOINTS = links.size();    // The order of matrix A
  int INFO;
                                  // The inertia matrix
  double** A   = rmatrix(0, links.size()-1, 0, links.size()-1);
  int LDA = links.size();        // The leading dimension of A
  double** JAi = rmatrix(0, links.size()-1, 0, 5);
  int LDJAi = 6;
  int LDJ = 6;
  
  for(size_t r=0; r<links.size(); r++)
    for(size_t c=0; c<links.size(); c++)
      A[r][c] = 0.0;

  JSinertia(A, q);         // compute the NxN joint space inertia matrix

  // A^-1
  // Cholesky factorization of the symmetric positive definite matrix A
  // A = L  * L**T
  potrf(&UPLO, &NJOINTS, &A[0][0], &LDA, &INFO);
  if(INFO<0)
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< ": The " << INFO << "th argument to potrf is illegal."
			<< std::endl;
  else if(0<INFO)
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< ": The matrix for potrf is not positive definite." 
			<< std::endl;

  // invert A
  //
  potri(&UPLO, &NJOINTS, &A[0][0], &LDA, &INFO);
  if(INFO<0)
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< ": The " << INFO << "th argument to potri is illegal."
			<< std::endl;
  else if(0<INFO)
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< ": The matrix passed to potri is singular."
			<< std::endl;

  JacobianBody( q );

  // J*A^-1 : a 6xN matrix column major
  // C := alpha*B*A + beta*C,
  symm(&SIDE, &UPLO, &NEQS, &NJOINTS,
       &ALPHA, &A [0][0], &LDA, 
               &Jn[0][0], &LDJ,
       //&Jn[0][0], &LDJ,
       &BETA, &JAi[0][0], &LDJAi);

  // J*A^-1*J'
  // C := alpha*op( A )*op( B ) + beta*C
  int LDAc = 6;
  gemm(&TRANSN, &TRANST, &NEQS, &NEQS, &NJOINTS,
       &ALPHA, &JAi[0][0], &LDJAi, 
               &Jn[0][0],  &LDJ,
       //&Jn[0][0],  &LDJ,
       &BETA,  &Ac[0][0],  &LDAc);

  // (J*A^-1*J')^-1
  // Factorize the symmetric matrix
  // A = L  * L**T
  potrf(&UPLO, &NEQS, &Ac[0][0], &LDAc, &INFO);
  if(INFO<0)
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< ": The " << INFO << "th argument to potrf is illegal."
			<< std::endl;
  else if(0<INFO)
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< ": The matrix for potrf is not positive definite."
			<< std::endl;

  // invert
  potri(&UPLO, &NEQS, &Ac[0][0], &LDAc, &INFO);
  if(INFO<0)
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS 
			<< "The " << INFO << "th argument to potri is illegal."
			<< std::endl;
  else if(0<INFO)
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< "The matrix passed to potri is singular."
			<< std::endl;

  free_rmatrix(   A, 0, 0 );
  free_rmatrix( JAi, 0, 0 );
}

vctDynamicVector<double> 
robManipulator::InverseDynamics( const vctDynamicVector<double>& q,
				 const vctDynamicVector<double>& qd,
				 const vctFixedSizeVector<double,6>& vdwd )const{

  char UPLO = 'L';
  int NEQS = 6;
  int INC = 1;
  char TRANST = 'T';
  int NJOINTS = links.size();

  double ALPHA =  1.0;      //
  double BETA  =  0.0;      // 

  double Ac[6][6];          // OS inertia matrix (lower half)
  int LDAc = 6;             // 1st dimension of Ac
  OSinertia(Ac,q);          // compute OS inertia matrix

  int LDJ = 6;              // 1st dimention of Jn

  vctFixedSizeVector<double,6> h = BiasAcceleration(q,qd); // h = Jdqd
  double hv[6] = {h[0], h[1], h[2], h[3], h[4], h[5]};     // make an array of h

  // Ac*h
  // y := alpha*A*x + beta*y,
  double Ach[6];                                       // symmetric matrix*vector
  symv(&UPLO, &NEQS,
       &ALPHA, &Ac[0][0], &LDAc, 
       hv, &INC,
       &BETA,  &Ach[0], &INC);

  // J'*Ac*h
  // y := alpha*A'*x + beta*y,
  double* JTAch = new double[links.size()];
  gemv(&TRANST, &NEQS, &NJOINTS,
       &ALPHA, &Jn[0][0], &LDJ, 
       Ach, &INC,
       &BETA, JTAch, &INC);

  // make sure there's a force, otherwise skip this and just consider ccg
  double* JTAcF = new double[links.size()];
  for(size_t i=0; i<links.size(); i++) JTAcF[i] = 0;
  if(0.0 < vdwd.Norm()){

    // Ac*F
    double Fv[6] = {vdwd[0], vdwd[1], vdwd[2], vdwd[3], vdwd[4], vdwd[5]};
    double AcF[6];
    symv(&UPLO,  &NEQS,
         &ALPHA, &Ac[0][0], &LDAc, 
	 Fv, &INC,
         &BETA, AcF, &INC);

    // J'*Ac*F
    gemv(&TRANST, &NEQS, &NJOINTS,
         &ALPHA, &Jn[0][0], &LDJ, 
	 AcF, &INC,
         &BETA, JTAcF, &INC);
  }

  vctDynamicVector<double> ccg = CCG(q, qd);         // compute the coriolis+grav

  vctDynamicVector<double> trq( links.size(), 0.0 );// reserve enough elements
  for(size_t i=0; i<links.size(); i++)
    trq[i] = JTAcF[i] + ccg[i] - JTAch[i];

  delete[] JTAcF;
  delete[] JTAch;

  return trq;
}

vctDynamicVector<double> 
robManipulator::InverseDynamics( const vctDynamicVector<double>& q,
				 const vctDynamicVector<double>& qd,
				 const vctDynamicVector<double>& qdd ) const {

  vctDynamicVector<double> ccg = CCG(q, qd);

  double* Inertterm  = new double[ links.size() ];
  for( size_t i=0; i<links.size(); i++ ) 
    Inertterm [i] = 0;

  // Ensure there's an acceleration. Orthewise only consider ccg
  if( 0.0 < qdd.Norm() ){

    char LOW = 'L';
    int N = links.size();;
    int LDA = links.size();
    int INC = 1;
    double ALPHA = 1.0;
    double BETA = 0.0;

    double** A = rmatrix(0, links.size()-1, 0, links.size()-1);
    JSinertia(A,q);

    // must copy to an array for symv
    double* qddv = new double[ links.size() ];
    for( size_t i=0; i<links.size(); i++ ) qddv[i] = qdd[i];

    symv(&LOW, &N,
         &ALPHA, &A[0][0], &LDA, 
	 qddv, &INC,
         &BETA, Inertterm, &INC);

    free_rmatrix(A, 0, 0);
    delete[] qddv;
  }

  // add inertia+coriolis+grav
  vctDynamicVector<double> trq( links.size(), 0.0 );
  for( size_t i=0; i<links.size(); i++ )
    trq[i] = Inertterm[i] + ccg[i];

  delete[] Inertterm;

  return trq;
}

void robManipulator::Print() const {

  for(size_t i=0; i<links.size(); i++){
    std::cout << "Link " << i << ": " << std::endl << links[i] << std::endl;
  }

}

/*
robError robManipulator::JointPosition( const vctDynamicVector<double>& q ){
  
  if( q.size() == links.size() ){
    for(size_t i=0; i<links.size(); i++)
      joints[i].Position( q[i] );
    ForwardKinematics();
    return SUCCESS;
  }
  else{
    std::cout << "robManipulator::JointPosition: Got " << q.size() 
	 << " joints, expected " << links.size() << std::endl;
    return ERROR;
  }
}
*/
/*
vctDynamicVector<double>       robManipulator::JointPosition( ) {
  vctDynamicVector<double> q(q.size(), 0.0);
  for(size_t i=0; i<links.size(); i++)
    q[i] = joints[i].Position();
  return q;
}
*/
/*
robError JointForceTorque( const vctDynamicVector<double>& q );
vctDynamicVector<double>       JointForceTorque( ) const;

robError ActuatorForceTorque( const vctDynamicVector<double>& q );
vctDynamicVector<double>       ActuatorForceTorque( ) const;
*/
