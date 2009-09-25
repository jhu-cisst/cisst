#include <cisstRobot/robManipulator.h>

#include <vector>

using namespace cisstRobot;
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

  void potrf(char *uplo, int *n, Real *a, int *lda, int *info);
  void potri(char *uplo, int *n, Real *a, int *lda, int *info);
  void symm(char *side, char *uplo, int *m, int *n,
            Real *alpha, Real *a, int *lda,
            Real* b, int *ldb, Real *beta,
            Real *c, int *ldc);
  void gemm(char *transa, char *transb, 
	    int *m, int *n, int *k,
            Real *alpha, Real *a, int *lda,
                         Real *b, int *ldb, 
	    Real *beta,  Real *c, int *ldc);
  void symv(char *uplo, int *n,
            Real *alpha, Real *a, int *lda,
            Real *x, int *incx, Real *beta,
            Real *y, int *incy);
  void gemv(char *trans, int *m, int *n,
            Real *alpha, Real *a, int *lda,
            Real *x, int *incx, Real *beta,
            Real *y, int *incy);
  void  gesv(int *N, int *NRHS,
             Real *A, int *LDA, int *IPIV,
             Real *B, int *LDB, int *INFO);
  void gelss(int* M, int* N, int* NRHS,
	     Real* A, int* LDA, 
	     Real* B, int* LDB, 
	     Real* S, Real* RCOND, int* RANK, 
	     Real* WORK, int* LWORK, int* INFO );

  Real nrm2(int *N, Real* X, int *INC);

}

#define NR_END 1
#define FREE_ARG char*

Real** rmatrix(long nrl, long nrh, long ncl, long nch){
  long i, nrow=nrh-nrl+1, ncol=nch-ncl+1;
  Real **m;

  m=(Real **)malloc((size_t)((nrow+NR_END)*sizeof(Real*)));
  //if(!m) nrerror("allocation failure 1 in matrix()");
  m += NR_END;
  m -= nrl;

  m[nrl]=(Real *)malloc((size_t)((nrow*ncol+NR_END)*sizeof(Real)));
  //if(!m[nrl]) nrerror("allocation failure 2 in matrix()");
  m[nrl] += NR_END;
  m[nrl] -= ncl;

  for(i=nrl+1; i<=nrh; i++) m[i]=m[i-1]+ncol;

  return m;
}

void free_rmatrix(Real** m, long nrl, long ncl){
  free((FREE_ARG) (m[nrl]+ncl-NR_END));
  free((FREE_ARG) (m+nrl-NR_END));
}

robManipulator::robManipulator( const std::string& linkfile, 
				const std::string& toolfile,
				const SE3& Rtw0,
				Real G, const R3& z0 ){
  this->Rtw0 = Rtw0;
  this->G = G;
  this->z0 = z0;
  tool = NULL;
  
  if( LoadRobot( linkfile ) == FAILURE ){
    cout << "robManipulator::robManipulator: failed to load the robot" << endl;
  }

  if( LoadTool( toolfile ) == FAILURE ){
    cout << "robManipulator::robManipulator: failed to load the tool" << endl;
  }

  ForwardKinematics();
}
				
robError robManipulator::LoadRobot( const string& filename ){
  ifstream ifs;

  ifs.open( filename.data() );
  if(!ifs){
    cout<<"robManipulator::LoadRobot: couln't open file " << filename << endl;
    return FAILURE;
  }

  size_t N;
  ifs >> N;
  for( size_t i=0; i<N; i++ ){
    robLink li;
    ifs >> li;
    links.push_back( li );
  }

  ifs >> N;
  for( size_t i=0; i<N; i++ ){
    robJoint ji;
    ifs >> ji;
    joints.push_back(ji);
  }

  Js = rmatrix(0, joints.size()-1, 0, 5);
  Jn = rmatrix(0, joints.size()-1, 0, 5);

  return SUCCESS;
}

robError robManipulator::LoadTool( const string& filename ){
  return SUCCESS;
}

//////////////////////////////////////

//////////////////////////////////////

const std::vector<robLink>& robManipulator::Links() const { return links; }

SE3 robManipulator::PositionOrientation( int i ) const {
  size_t l = ( i < 0 ) ? (links.size()-1) : (size_t)i;

  if( l < links.size() )
    return links[l].PositionOrientation();
  else 
    return SE3();
}

void robManipulator::ForwardKinematics(){
  SE3 Rtwi = Rtw0;

  for(size_t i=0; i<links.size(); i++){
    links[i].SetDOF( joints[i].Position() );
    Rtwi = Rtwi * links[i].ForwardKinematics();
    links[i].PositionOrientation( Rtwi );
  }

  cout.precision(4);                                                 
  cout.setf(std::ios::fixed, std::ios::floatfield);

  JacobianBody();
  JacobianSpatial();

}

robError robManipulator::InverseKinematics( Rn& q, 
					    const SE3& Rts, 
					    Real tol, 
					    size_t Niter ){

  int M = 6;                    // The number or rows of matrix A
  int N = joints.size();        // The number of columns of matrix A
  int NHRS = 1;                 // The number of right hand side vectors

  Real* A = &(Js[0][0]);        // Pointer to the spatial Jacobian
  int LDA = M;                  // The leading dimension of the array A.

  Real* B;                      // The N-by-NRHS matrix of right hand side matrix
  int LDB = N;                  // The leading dimension of the array B.

  Real* S = new Real[M];        // The singular values of A in decreasing order
  Real RCOND = -1;              // Use machine precision to determine rank(A)
  int RANK;                     // The effective rank of A
  int LWORK = 128;              // The (safe) size of the workspace
  Real WORK[128];               // The workspace
  int INFO;                     // The info code
  int INC = 1;                  // The index increment 

  Real ndq=1;             // norm of the iteration
  size_t i;               // the iterator

  mutex.Lock();

  // loop until Niter are executed or the error is bellow the tolerance
  for( i=0; i<Niter && tol<ndq; i++ ){

    if(JointPosition(q) == FAILURE)   // set the joint positions
      cout<<"robManipulator::InverseKinematics: failed to set the joints."<<endl;

    SE3 Rt = PositionOrientation();   // get the forward kinematics

    // compute the translation error
    R3 dt( Rts[0][3]-Rt[0][3],   Rts[1][3]-Rt[1][3],   Rts[2][3]-Rt[2][3] );
    
    // compute the rotation error
    R3 n1( Rt[0][0], Rt[1][0], Rt[2][0] ), n2( Rts[0][0], Rts[1][0], Rts[2][0] );
    R3 o1( Rt[0][1], Rt[1][1], Rt[2][1] ), o2( Rts[0][1], Rts[1][1], Rts[2][1] );
    R3 a1( Rt[0][2], Rt[1][2], Rt[2][2] ), a2( Rts[0][2], Rts[1][2], Rts[2][2] );
    R3 dr = 0.5*( (n1%n2) + (o1%o2) + (a1%a2) );

    // combine both errors in one R^6 vector
    Real e[6] = { dt[0], dt[1], dt[2], dr[0], dr[1], dr[2] };
    B = &e[0];

    // compute the minimum norm solution
    gelss( &M, &N, &NHRS, 
	   A, &LDA, 
	   B, &LDB, 
	   S, &RCOND, &RANK,
	   WORK, &LWORK, &INFO );
    
    // process the errors (if any)
    if(INFO < 0)
      cout << "robManipulator::InverseKinematics: " 
	   << "the i-th argument had an illegal value." << endl;
    if(0 < INFO)
      cout << "robManipulator::InverseKinematics: gelss failed." << endl;
    
    // compute the L2 norm 
    ndq = nrm2(&N, B, &INC);

    // update the solution
    for(size_t j=0; j<joints.size(); j++) q[j] += B[j];
  }
  
  // copy the joint values and 
  for(size_t j=0; j<joints.size(); j++){
    q[j] = fmod((Real)q[j], (Real)2.0*M_PI);
    if(M_PI < q[j])
      q[j] = q[j] - 2.0*M_PI;
  }

  delete[] S;

  mutex.Unlock();

  if( i==Niter ) return FAILURE;
  else return SUCCESS;
}

void robManipulator::JacobianSpatial() const {
  /*
   * Get the adjoint matrix to flip the body jacobian to spatial jacobian
   */
  SE3 Rt0n = PositionOrientation();
  Real Ad[6][6];//( this->FK() );

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
  int N = joints.size();// specifies the number  of columns of the matrix Jn
  int K = 6;            // specifies  the number of columns of op( Ad )
  Real ALPHA = 1.0;     // C := alpha*op( A )*op( B ) + beta*C
  Real* A = &Ad[0][0];  // 
  int LDA = 6;          // specifies the first dimension of A 
  Real* B = &Jn[0][0];  // 
  int LDB = 6;          // specifies  the first dimension of B 
  Real BETA = 0.0;      // C := alpha*op( A )*op( B ) + beta*C
  Real* C = &Js[0][0];  // 
  int LDC = 6;          // specifies the first dimension of C

  // Js = Ad * Jn
  gemm(TRANSA, TRANSB,
       &M, &N, &K, 
       &ALPHA, A, &LDA,
               B, &LDB, 
       &BETA,  C, &LDC);
}

/*
 * Body manipulator Jacobian
 * Paul IEEE SMC 11(6) 1981
 * BIG FAT WARNIN': The jacobian is in column major (for Fortran)
 */
void robManipulator::JacobianBody() const {

  SE3 U;   // set identity

  if( tool != NULL ){
    U = tool->ForwardKinematics(); // set to tool if any
  }

  for(int j=(int)joints.size()-1; 0<=j; j--){

    if( links[j].IsDH() ){            // DH convention
      U = links[j].ForwardKinematics() * U;
    }

    if( links[j].IsRevolute() ){     // Revolute joint
      // Jn is column major
      Jn[j][0] = U[0][3]*U[1][0] - U[1][3]*U[0][0];
      Jn[j][1] = U[0][3]*U[1][1] - U[1][3]*U[0][1];
      Jn[j][2] = U[0][3]*U[1][2] - U[1][3]*U[0][2];

      Jn[j][3] = U[2][0]; // nz
      Jn[j][4] = U[2][1]; // oz
      Jn[j][5] = U[2][2]; // az

    }

    if( links[j].IsPrismatic() ){    // Prismatic joint
      // Jn is column major
      Jn[j][0] = U[2][0]; // nz
      Jn[j][1] = U[2][1]; // oz
      Jn[j][2] = U[2][2]; // az

      Jn[j][3] = 0.0;
      Jn[j][4] = 0.0;
      Jn[j][5] = 0.0;

    }

    if( links[j].IsModifiedDH() ){   // Modified DH
      U = links[j].ForwardKinematics() * U;
    }
  }
}

Rn robManipulator::RNE(const R6& fext){
  R3 w(0.0), wd(0.0), v(0.0), vd(0.0), vdhat(0.0);
  std::vector<R3> N( joints.size(), R3(0.0) );//total moment exerted on each link
  std::vector<R3> F( joints.size(), R3(0.0) );//total force exerted on each link
  Rn            tau( joints.size(), 0.0 );    // torques

  vd = G*z0;                                  // acceleration of link 0

  // Forward recursion
  for(size_t i=0; i<joints.size(); i++){

    Real m   = links[i].Mass();
    R3 s     = links[i].COM();                    // center of mass
    MOIT I   = links[i].MOI();                    // Moment of inertia
    SO3 A    = links[i].Rotation().InverseSelf(); // (i-1)Ai
    R3 ps    = links[i].PStar();                  // position of distal link
    Real qd  = joints[i].Velocity();              // joint velocity
    Real qdd = joints[i].Acceleration();          // joint acceleration

    wd = A*( wd + (z0*qdd) + (w%(z0*qd)) );  // rotational acceleration wrt cf i
    w  = A*( w  + (z0*qd) );                 // rotational velocity
    vd = (wd%ps) + (w%(w%ps)) + A*vd;        // linear acceleration

    vdhat = (wd%s) + (w%(w%s)) + vd;         // 
    F[i] = m*vdhat;                          // force
    N[i] = (I*wd) + (w%(I*w));               // moment
  }

  R3 f( fext[0], fext[1], fext[2] );         // ext force exerted on last link
  R3 n( fext[3], fext[4], fext[5] );         // ext moment exterted on last link

  // Backward recursion
  for(int i=(int)joints.size()-1; 0<=i; i--){//
    SO3 A;
    R3 ps = links[i].PStar();                // 
    R3 s  = links[i].COM();

    if(i != (int)joints.size()-1)            // 
      A = links[i+1].Rotation();             // 

    f = A*f + F[i];                          // force exterted on i by i-1
    n = A*n + (ps%f) + (s%F[i]) + N[i];      // moment externed on i by i-1
    A = links[i].Rotation().InverseSelf();   // 
    if (links[i].IsRevolute() )
      tau[i] = n*(A*z0);                     // 
    if( links[i].IsPrismatic() )
      tau[i] = f*(A*z0);                     // 
  }

  return tau;
}

Rn robManipulator::InertiaSubroutine( size_t idx ){
  R3 w(0.0), wd(0.0), v(0.0), vd(0.0), vdhat(0.0);
  std::vector<R3> N( joints.size(), R3(0.0) ); // reserve enough space
  std::vector<R3> F( joints.size(), R3(0.0) ); //

  // Forward recursion
  for(size_t i=0; i<joints.size(); i++){

    Real m   = links[i].Mass();                   // mass
    R3 s     = links[i].COM();                    // center of mass
    MOIT I   = links[i].MOI();                    // moment of inertia
    SO3 A    = links[i].Rotation().InverseSelf(); // (i-1)Ai
    R3 ps    = links[i].PStar();                  // 
    Real qd  = 0.0;                               // 0 velocity
    Real qdd = ( i == idx ) ? 1 : 0;              // 0/1 acceleration

    wd = A*( wd + (z0*qdd) + (w%(z0*qd)) );  // rotational acceleration
    w  = A*( w  + (z0*qd) );                 // rotational velocity
    vd = (wd%ps) + (w%(w%ps)) + A*vd;        // linear acceleration

    vdhat = (wd%s) + (w%(w%s)) + vd;
    F[i] = m*vdhat;                          // force
    N[i] = (I*wd) + (w%(I*w));               // moment
  }

  R3 f(0.0), n(0.0);
  Rn tau( joints.size(), 0.0 );

  // Backward recursion
  for(int i=(int)joints.size()-1; 0<=i; i--){
    SO3 A;
    R3 ps = links[i].PStar();
    R3 s  = links[i].COM();

    if(i != (int)joints.size()-1)
      A = links[i+1].Rotation();

    f = A*f + F[i];
    n = A*n + (ps%f) + (s%F[i]) + N[i];  // ask me for this simplification
    A = links[i].Rotation().Inverse();

    if (links[i].IsRevolute() )
      tau[i] = n*(A*z0);                     // 
    if( links[i].IsPrismatic() )
      tau[i] = f*(A*z0);                     // 

  }

  return tau;
}

Rn robManipulator::CCG(){
  R3 w(0.0), wd(0.0), v(0.0), vd(0.0), vdhat(0.0);
  R3 n(0.0), f(0.0);
  std::vector<R3> N( joints.size(), R3(0.0) );
  std::vector<R3> F( joints.size(), R3(0.0) );
  Rn            ccg( joints.size(), 0.0 );

  vd = G*z0;

  for(size_t i=0; i<joints.size(); i++){
    Real m  = links[i].Mass();
    R3 s    = links[i].COM();
    MOIT I  = links[i].MOI();
    SO3 A   = links[i].Rotation().InverseSelf();
    R3 ps   = links[i].PStar();
    Real qd = joints[i].Velocity();

    wd = A*( wd + (w%(z0*qd)) );
    w  = A*( w  + (   z0*qd)  );
    vd = (wd%ps) + (w%(w%ps)) + A*vd;

    vdhat = (wd%s) + (w%(w%s)) + vd;
    F[i] =  m*vdhat;
    N[i] = (I*wd) + (w%(I*w));
  }

  for(int i=(int)links.size()-1; 0<=i; i--){
    SO3 A;
    R3 ps = links[i].PStar();
    R3 s  = links[i].COM();

    if(i != (int)links.size()-1)
      A = links[i+1].Rotation();

    f = A*f + F[i];
    n = A*n + (ps%f) + (s%F[i]) + N[i];
    A = links[i].Rotation().Inverse();

    if (links[i].IsRevolute() )
      ccg[i] = n*(A*z0);                     // 
    if( links[i].IsPrismatic() )
      ccg[i] = f*(A*z0);                     // 
  }
  return ccg;
}

/*
 * Use to compute the linear anc rotational accelerations
 */
R6 robManipulator::Acceleration(){
  R3 w(0.0), wd(0.0), vd(0.0);

  for(size_t i=0; i<joints.size(); i++){

    SO3 A      = links[i].Rotation().InverseSelf();
    R3 ps      = links[i].PStar();
    Real qd    = joints[i].Velocity();
    Real qdd   = joints[i].Acceleration();

    wd = A*( wd + (z0*qdd) + (w%(z0*qd)) );
    w  = A*( w  + (z0*qd ) );
    vd = (wd%ps) + (w%(w%ps)) + A*vd;
  }

  return R6(vd[0], vd[1], vd[2], wd[0], wd[1], wd[2]);
}

R6 robManipulator::BiasAcceleration(){
  R3 w, wd, vd;

  for(size_t i=0; i<joints.size(); i++){

    SO3 A      = links[i].Rotation().InverseSelf();
    R3 ps      = links[i].PStar();
    Real qd    = joints[i].Velocity();

    wd = A*( wd + ( w%(z0*qd) ) );
    w  = A*( w  + (    z0*qd  ) );
    vd = (wd%ps) + (w%(w%ps)) + A*vd;
  }

  return R6(vd[0], vd[1], vd[2], wd[0], wd[1], wd[2]);
}

// A is column major!
void robManipulator::JSinertia(Real **A) {
  for(size_t c=0; c<joints.size(); c++){
    Rn h = InertiaSubroutine( c );
    for( size_t r=0; r<joints.size(); r++ )
      A[c][r] = h[r];
  }
}

// Ac is column major!
void robManipulator::OSinertia(Real Ac[6][6]){
  char SIDE = 'R';                // dsymm C := alpha*B*A + beta*C,
  char UPLO = 'L';                // lower triangular (upper triangular for CM)
  char TRANST = 'T';
  char TRANSN = 'N';
  Real ALPHA = 1.0;
  Real BETA = 0.0;                //
  int NEQS = 6;                   // num rows of the jacobian
  int NJOINTS = joints.size();    // The order of matrix A
  int INFO;
                                  // The inertia matrix
  Real** A   = rmatrix(0, joints.size()-1, 0, joints.size()-1);
  int LDA = joints.size();        // The leading dimension of A
  Real** JAi = rmatrix(0, joints.size()-1, 0, 5);
  int LDJAi = 6;
  int LDJ = 6;
  
  for(size_t r=0; r<joints.size(); r++)
    for(size_t c=0; c<joints.size(); c++)
      A[r][c] = 0.0;

  JSinertia(A);         // compute the NxN joint space inertia matrix

  // A^-1
  // Cholesky factorization of the symmetric positive definite matrix A
  // A = L  * L**T
  potrf(&UPLO, &NJOINTS, &A[0][0], &LDA, &INFO);
  if(INFO<0)
    std::cout<<"OSinertia::dpotrf1: The " << INFO << " argument is illegal.\n";
  else if(0<INFO)
    std::cout<<"OSinertia::dpotrf1: The matrix A is not positive definite.\n";

  // invert A
  //
  potri(&UPLO, &NJOINTS, &A[0][0], &LDA, &INFO);
  if(INFO<0)
    std::cout<<"OSinertia::dpotri1: The " << INFO << " argument is illegal.\n";
  else if(0<INFO)
    std::cout<<"OSinertia::dpotri1: The matrix A is singular.\n";

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
    std::cout<<"OSinertia::dpotrf2: The " << INFO << " argument is illegal.\n";
  else if(0<INFO)
    std::cout<<"OSinertia::dpotrf2: The matrix is not positive definite.\n";

  // invert
  potri(&UPLO, &NEQS, &Ac[0][0], &LDAc, &INFO);
  if(INFO<0)
    std::cout<<"OSinertia::dpotri2: The " << INFO << " argument is illegal.\n";
  else if(0<INFO)
    std::cout<<"OSinertia::dpotri2: The matrix is singular.\n";

  //for( size_t i=0; i<6; i++ )
  //for( size_t j=i; j<6; j++ )
  //Ac[j][i] = Ac[i][j];

  free_rmatrix(   A, 0, 0 );
  free_rmatrix( JAi, 0, 0 );
}

Rn robManipulator::InverseDynamics( const R6& vdwd ){

  char UPLO = 'L';
  int NEQS = 6;
  int INC = 1;
  char TRANST = 'T';
  int NJOINTS = joints.size();

  Real ALPHA =  1.0;        //
  Real BETA  =  0.0;        // 

  Real Ac[6][6];            // OS inertia matrix (lower half)
  int LDAc = 6;             // 1st dimension of Ac
  OSinertia(Ac);            // compute OS inertia matrix

  int LDJ = 6;              // 1st dimention of Jn

  R6 h = BiasAcceleration();                         // h = Jdqd
  Real hv[6] = {h[0], h[1], h[2], h[3], h[4], h[5]}; // make an array of h

  // Ac*h
  // y := alpha*A*x + beta*y,
  Real Ach[6];                                       // symmetric matrix*vector
  symv(&UPLO, &NEQS,
       &ALPHA, &Ac[0][0], &LDAc, 
       hv, &INC,
       &BETA,  &Ach[0], &INC);

  // J'*Ac*h
  // y := alpha*A'*x + beta*y,
  Real* JTAch = new Real[joints.size()];
  gemv(&TRANST, &NEQS, &NJOINTS,
       &ALPHA, &Jn[0][0], &LDJ, 
       Ach, &INC,
       &BETA, JTAch, &INC);

  // make sure there's a force, otherwise skip this and just consider ccg
  Real* JTAcF = new Real[joints.size()];
  for(size_t i=0; i<joints.size(); i++) JTAcF[i] = 0;
  if(0.0 < vdwd.Norm()){

    // Ac*F
    Real Fv[6] = {vdwd[0], vdwd[1], vdwd[2], vdwd[3], vdwd[4], vdwd[5]};
    Real AcF[6];
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

  Rn ccg = CCG();                         // compute the coriolis+grav

  Rn trq( joints.size(), 0.0 );           // reserve enough elements
  for(size_t i=0; i<joints.size(); i++)
    trq[i] = JTAcF[i] + ccg[i] - JTAch[i];

  delete[] JTAcF;
  delete[] JTAch;

  return trq;
}

Rn robManipulator::InverseDynamics( const Rn& qdd ){

  Rn ccg = CCG();

  Real* Inertterm  = new Real[ joints.size() ];
  for( size_t i=0; i<joints.size(); i++ ) 
    Inertterm [i] = 0;

  // make sure there's an acceleration. othewise only consider ccg

  if( 0.0 < qdd.Norm() ){

    char LOW = 'L';
    int N = joints.size();;
    int LDA = joints.size();
    int INC = 1;
    Real ALPHA = 1.0;
    Real BETA = 0.0;

    Real** A = rmatrix(0, joints.size()-1, 0, joints.size()-1);
    JSinertia(A);

    // must copy to an array for symv
    Real* qddv = new Real[ joints.size() ];
    for( size_t i=0; i<joints.size(); i++ ) qddv[i] = qdd[i];

    symv(&LOW, &N,
         &ALPHA, &A[0][0], &LDA, 
	 qddv, &INC,
         &BETA, Inertterm, &INC);

    free_rmatrix(A, 0, 0);
    delete[] qddv;
  }

  // add inertia+coriolis+grav
  Rn trq( joints.size(), 0.0 );
  for( size_t i=0; i<joints.size(); i++ )
    trq[i] = Inertterm[i] + ccg[i];

  delete[] Inertterm;

  return trq;
}

robError robManipulator::Apos2Jpos( Real dt ){
  if( actuators.size() == joints.size() ){
    for(size_t i=0; i<joints.size(); i++)
      joints[i].Position( actuators[i]->Position(), dt );
  }
  else{
    cout << "robManipulator::Apos2Jpos: "
	 << "number of actuators must equal the number of joints." << endl;
  }

  return SUCCESS;
}

robError robManipulator::AFT2JFT( ){
  if( actuators.size() == joints.size() ){
    for(size_t i=0; i<joints.size(); i++)
      joints[i].FT( actuators[i]->FT() );
  }
  else{
    cout << "robManipulator::AFT2JFT: "
	 << "number of actuators must equal the number of joints." << endl;
  }

  return SUCCESS;
}

robError robManipulator::Jpos2Apos( ){
  if( actuators.size() == joints.size() ){
    for(size_t i=0; i<actuators.size(); i++)
      actuators[i]->Position( joints[i].Position() );
  }
  else{
    cout << "robManipulator::Jpos2Apos: "
	 << "number of actuators must equal the number of joints." << endl;
  }

  return SUCCESS;
}

robError robManipulator::JFT2AFT( ){
  if( actuators.size() == joints.size() ){
    for(size_t i=0; i<actuators.size(); i++)
      actuators[i]->FT( joints[i].FT() );    
  }
  else{
    cout << "robManipulator::JFT2AFT: "
	 << "number of actuators must equal the number of joints." << endl;
  }

  return SUCCESS;
}

void robManipulator::Print() const {

  for(size_t i=0; i<links.size(); i++){
    std::cout << "Link " << i << ": " << endl << links[i] << endl;
  }

  for(size_t i=0; i<joints.size(); i++){
    std::cout << "Joint " << i << ": " << endl << joints[i] << endl;
  }

  for(size_t i=0; i<actuators.size(); i++){
    std::cout << "Actuator " << i << ": " << std::endl;
    actuators[i]->Print();
    std::cout << std::endl;
  }

}

robError robManipulator::JointPosition( const Rn& q ){
  
  if( q.size() == joints.size() ){
    for(size_t i=0; i<joints.size(); i++)
      joints[i].Position( q[i] );
    ForwardKinematics();
    return SUCCESS;
  }
  else{
    cout << "robManipulator::JointPosition: Got " << q.size() 
	 << " joints, expected " << joints.size() << endl;
    return FAILURE;
  }
}

Rn       robManipulator::JointPosition( ) {
  Rn q(q.size(), 0.0);
  for(size_t i=0; i<joints.size(); i++)
    q[i] = joints[i].Position();
  return q;
}
    /*
robError JointForceTorque( const Rn& q );
Rn       JointForceTorque( ) const;

robError ActuatorForceTorque( const Rn& q );
Rn       ActuatorForceTorque( ) const;
    */
