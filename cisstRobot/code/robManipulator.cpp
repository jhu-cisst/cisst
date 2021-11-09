/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-    */
/* ex: set filetype=cpp softtabstop=2 shiftwidth=2 tabstop=2 cindent expandtab: */

/*
  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnConstants.h>
#include <cisstRobot/robManipulator.h>

#include <cisstVector/vctQuaternionRotation3.h>
#include <cisstVector/vctFixedSizeMatrix.h>

//#include <cisstNumerical/nmrNetlib.h>

#include <vector>
#include <iostream>

using namespace std;

typedef long int integer;
typedef unsigned long int uinteger;
typedef char *address;
typedef short int shortint;
typedef float real;
typedef double doublereal;
typedef struct { real r, i; } complex;
typedef struct { doublereal r, i; } doublecomplex;
typedef long int logical;
typedef short int shortlogical;
typedef char logical1;
typedef char integer1;
typedef long int flag;
typedef long int ftnlen;
typedef long int ftnint;

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

  void potrf(char *uplo, integer *n, doublereal *a, integer *lda, integer *info);
  void potri(char *uplo, integer *n, doublereal *a, integer *lda, integer *info);

  void symm(char *side, char *uplo, integer *m, integer *n,
            doublereal *alpha, doublereal *a, integer *lda,
            doublereal* b, integer *ldb, doublereal *beta,
            doublereal *c, integer *ldc);
  void gemm(char *transa, char *transb,
            integer *m, integer *n, integer *k,
            doublereal *alpha, doublereal *a, integer *lda,
            doublereal *b, integer *ldb,
            doublereal *beta,  doublereal *c, integer *ldc);
  void symv(char *uplo, integer *n,
            doublereal *alpha, doublereal *a, integer *lda,
            doublereal *x, integer *incx, doublereal *beta,
            doublereal *y, integer *incy);
  void gemv(char *trans, integer *m, integer *n,
            doublereal *alpha, doublereal *a, integer *lda,
            doublereal *x, integer *incx, doublereal *beta,
            doublereal *y, integer *incy);

  void  gesv(integer *N, integer *NRHS,
             doublereal *A, integer *LDA, integer *IPIV,
             doublereal *B, integer *LDB, integer *INFO);
  void gelss(integer* M, integer* N, integer* NRHS,
             doublereal* A, integer* LDA,
             doublereal* B, integer* LDB,
             doublereal* S, doublereal* RCOND, integer* RANK,
             doublereal* WORK, integer* LWORK, integer* INFO );

  doublereal nrm2(integer *N, doublereal* X, integer *INC);

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

robManipulator::robManipulator( const vctFrame4x4<double>& Rtw0 )
  : Jn(NULL),Js(NULL)
{  this->Rtw0 = Rtw0;  }

robManipulator::robManipulator( const std::string& linkfile,
                                const vctFrame4x4<double>& Rtw0 )
  : Jn(NULL),Js(NULL){

  this->Rtw0 = Rtw0;

  if( LoadRobot( linkfile ) == robManipulator::EFAILURE ){
    mLastError = "robManipulator::robManipulator: failed to load the robot configuration";
    CMN_LOG_RUN_ERROR << mLastError << std::endl;
  }
}

robManipulator::robManipulator( const std::vector<robKinematics *> linkParms,
                                const vctFrame4x4<double>& Rtw0 )
  : Jn(NULL),Js(NULL){

  this->Rtw0 = Rtw0;

  if( LoadRobot( linkParms ) == robManipulator::EFAILURE ){
    mLastError = "robManipulator::robManipulator: failed to load the robot configuration";
    CMN_LOG_RUN_ERROR << mLastError << std::endl;
  }
}

robManipulator::~robManipulator()
{
  if( Jn != NULL ){ free_rmatrix(Jn, 0, 0); }
  if( Js != NULL ){ free_rmatrix(Js, 0, 0); }
}

void robManipulator::Attach( robManipulator* tool )
{
  tools.push_back( tool );
}

void robManipulator::DeleteTools()
{
  const ToolsType::iterator end = tools.end();
  ToolsType::iterator tool;
  for (tool = tools.begin(); tool != end; ++tool) {
    delete *tool;
    *tool = NULL;
  }
  tools.clear();
}

robManipulator::Errno robManipulator::LoadRobot( const std::string& filename ){

  if( filename.empty() ){
    mLastError = "robManipulator::LoadRobot: no configuration file";
    CMN_LOG_RUN_ERROR << mLastError << std::endl;
    return robManipulator::EFAILURE;
  }

  std::ifstream ifs;
  ifs.open( filename.data() );
  if(!ifs){
    mLastError = "robManipulator::LoadRobot: couldn't open configuration file "
      + filename;
    CMN_LOG_RUN_ERROR << mLastError << std::endl;
    return robManipulator::EFAILURE;
  }

  size_t N;       // the number of links
  {
    std::string line;
    getline( ifs, line );
    std::istringstream stringstream(line);
    stringstream >> N;
  }

  // read the links (kinematics+dynamics+geometry) from the input
  for( size_t i=0; i<N; i++ ){

    // Read a line from the file
    std::string line;
    getline( ifs, line );
    std::istringstream stringstream(line);

    // Find the type of kinematics convention
    std::string convention;
    stringstream >> convention;

    robKinematics* kinematics = NULL;
    try{ kinematics = robKinematics::Instantiate( convention ); }
    catch( std::bad_alloc& ){
      mLastError = "robManipulator::LoadRobot: failed to allocate a kinematics of type: "
        + convention;
      CMN_LOG_RUN_ERROR << mLastError << std::endl;
      return robManipulator::EFAILURE;
    }

    CMN_ASSERT( kinematics != NULL );
    robLink li( kinematics, robMass() );
    li.Read( stringstream );
    links.push_back( li );

  }

  Js = rmatrix(0, links.size()-1, 0, 5);
  Jn = rmatrix(0, links.size()-1, 0, 5);

  return robManipulator::ESUCCESS;
}

#if CISST_HAS_JSON
robManipulator::Errno robManipulator::LoadRobot(const Json::Value &config)
{
    if (config.isNull()) {
        mLastError = "robManipulator::LoadRobot(json): invalid configuration";
        CMN_LOG_INIT_ERROR << mLastError << std::endl;
        return robManipulator::EFAILURE;
    }

    Json::Value jsonLinks = config["links"];
    if (jsonLinks.isNull()) {
        jsonLinks = config["joints"];
        if (jsonLinks.isNull()) {
            mLastError = "robManipulator::LoadRobot(json): need array \"links\" or \"joints\"";
            CMN_LOG_INIT_ERROR << mLastError << std::endl;
            return robManipulator::EFAILURE;
        }
    }

    const unsigned int numLinks = jsonLinks.size();
    if (numLinks == 0) {
        mLastError = "robManipulator::LoadRobot(json): empty array of links";
        CMN_LOG_INIT_ERROR << mLastError << std::endl;
        return robManipulator::EFAILURE;
    }

    // Try to get convention for the whole chain first
    std::string convention;
    if (!(config["convention"].isNull())) {
        convention = config["convention"].asString();
    }

    // load each link
    for (unsigned int i = 0; i < numLinks; i++) {
        const Json::Value jlink = jsonLinks[i];
        if (jlink.isNull()) {
            // could use c++11 std::to_string
            std::stringstream ss;
            ss << (i + 1);
            mLastError = "robManipulator::LoadRobot(json): can't find link " + ss.str();
            CMN_LOG_INIT_ERROR << mLastError << std::endl;
            return robManipulator::EFAILURE;
        }

        // Convention is defined per link
        if (convention == "") {
            if (jlink["convention"].isNull()) {
                // could use c++11 std::to_string
                std::stringstream ss;
                ss << (i + 1);
                mLastError = "robManipulator::LoadRobot(json): can't find convention for link " + ss.str();
                CMN_LOG_INIT_ERROR << mLastError << std::endl;
                return robManipulator::EFAILURE;
            }
            convention = jlink["convention"].asString();
        }
        robKinematics * kinematics = NULL;
        try {
            kinematics = robKinematics::Instantiate(convention);
        }
        catch( std::bad_alloc& ){
            mLastError = "robManipulator::LoadRobot(json): failed to allocate a kinematics of type: "
              + convention;
            CMN_LOG_RUN_ERROR << mLastError << std::endl;
            return robManipulator::EFAILURE;
        }

        CMN_ASSERT( kinematics != NULL );
        robLink li( kinematics, robMass() );

        li.Read(jlink);
        links.push_back(li);
    }

    Js = rmatrix(0, links.size()-1, 0, 5);
    Jn = rmatrix(0, links.size()-1, 0, 5);

    return robManipulator::ESUCCESS;
}
#endif


robManipulator::Errno robManipulator::LoadRobot(std::vector<robKinematics *> KinParms)
{
  // Number of links
  size_t N = KinParms.size();

  // read the links (kinematics+dynamics+geometry) from the input
  for( size_t i=0; i<N; i++ ){
    robLink li( KinParms[i], robMass() );
    links.push_back( li );
  }

  Js = rmatrix(0, links.size()-1, 0, 5);
  Jn = rmatrix(0, links.size()-1, 0, 5);

  return robManipulator::ESUCCESS;
}

//////////////////////////////////////
//         KINEMATICS
//////////////////////////////////////

void
robManipulator::SetJointLimits(const vctDynamicVector<double> & lowerLimits,
                               const vctDynamicVector<double> & upperLimits)
{
  if ((lowerLimits.size() != links.size())
      || (upperLimits.size() != links.size())) {
    cmnThrow(std::range_error("robManipulator::SetJointLimits: size of inputs doesn't match kinematic chain length"));
  }
  for (size_t i = 0; i < links.size(); i++ ) {
    links[i].GetKinematics()->PositionMin() = lowerLimits.at(i);
    links[i].GetKinematics()->PositionMax() = upperLimits.at(i);
  }
}

void
robManipulator::GetJointLimits(vctDynamicVectorRef<double> lowerLimits,
                               vctDynamicVectorRef<double> upperLimits) const
{
  if ((lowerLimits.size() != links.size())
      || (upperLimits.size() != links.size())) {
    cmnThrow(std::range_error("robManipulator::GetJointLimits: size of placeholders doesn't match kinematic chain length"));
  }
  for (size_t i = 0; i < links.size(); i++ ) {
    lowerLimits.at(i) = links[i].GetKinematics()->PositionMin();
    upperLimits.at(i) = links[i].GetKinematics()->PositionMax();
  }
}

void
robManipulator::GetFTMaximums(vctDynamicVectorRef<double> ftMaximums) const
{
  if (ftMaximums.size() != links.size()) {
    cmnThrow(std::range_error("robManipulator::GetFTMaximums: size of placeholder doesn't match kinematic chain length"));
  }
  for (size_t i = 0; i < links.size(); i++ ) {
    ftMaximums.at(i) = links[i].GetKinematics()->ForceTorqueMax();
  }
}

void
robManipulator::GetJointNames(std::vector<std::string> & names) const
{
  if (names.size() != links.size()) {
    cmnThrow(std::range_error("robManipulator::GetJointNames: size of placeholder doesn't match kinematic chain length"));
  }
  for (size_t i = 0; i < links.size(); i++ ) {
    names.at(i) = links[i].GetKinematics()->Name();
  }
}

void
robManipulator::GetJointTypes(std::vector<robJoint::Type> & types) const
{
  if (types.size() != links.size()) {
    cmnThrow(std::range_error("robManipulator::GetJoinTypes: size of placeholder doesn't match kinematic chain length"));
  }
  for (size_t i = 0; i < links.size(); i++ ) {
    types.at(i) = links[i].GetKinematics()->GetType();

  }
}

vctFrame4x4<double>
robManipulator::ForwardKinematics( const vctDynamicVector<double>& q,
                                   int N ) const
{

  if( N == 0 ) return Rtw0;

  // if N < 0 then we want the end-effector
  if( N < 0 ) N = links.size();

  if( ((int)q.size()) < N ){
    stringstream ss;
    ss << "robManipulator::ForwardKinematics: expected up to " << N
       << " joint positions but received " << q.size();
    cmnThrow(std::range_error(ss.str()));
  }

  // no link? then return the transformation of the base
  if( links.empty() ){
    //CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
    //                << ": Manipulator has no link."
    //                        << std::endl;
    return Rtw0;
  }

  // set the position/orientation of link 0 to the base * its tranformation
  // setting the link's transformation is necessary in order to render the link
  // in opengl
  vctFrame4x4<double> Rtwi = Rtw0*links[0].ForwardKinematics( q[0] );

  // for link 1 to N
  for(int i=1; i<N; i++)
    Rtwi = Rtwi * links[i].ForwardKinematics( q[i] );

  if( (tools.size() == 1) && (N == (int)links.size()) ){
    if( tools[0] != NULL )
      { return Rtwi * tools[0]->ForwardKinematics( q, 0 ); }
  }

  return Rtwi;
}

robManipulator::Errno
robManipulator::InverseKinematics( vctDynamicVector<double>& q,
                                   const vctFrm3& Rts,
                                   double tolerance,
                                   size_t Niterations ){
  vctFrm3 Rtsn( Rts );
  Rtsn.Rotation().NormalizedSelf();
  vctFrame4x4<double> Rts4x4( Rtsn.Rotation(), Rtsn.Translation());
  return InverseKinematics( q, Rts4x4, tolerance, Niterations );
}

robManipulator::Errno
robManipulator::InverseKinematics( vctDynamicVector<double>& q,
                                   const vctFrame4x4<double>& Rts,
                                   double tolerance,
                                   size_t Niterations,
                                   double LAMBDA ){

  if( q.size() != links.size() ){
    stringstream ss;
    ss << "robManipulator::InverseKinematics: expected "
       << links.size() << " joints values but received "
       << q.size();
    mLastError = ss.str();
    CMN_LOG_RUN_ERROR << mLastError << std::endl;
    return robManipulator::EFAILURE;
  }

  if( links.size() == 0 ){
    mLastError = "robManipulator::InverseKinematics: the manipulator has no links";
    CMN_LOG_RUN_ERROR << mLastError << std::endl;
    return robManipulator::EFAILURE;
  }

  // A is a pointer to the 6xN spatial Jacobian
  integer M = 6;                  // The number or rows of matrix A
  integer N = links.size();       // The number of columns of matrix A
  integer NRHS = 1;               // The number of right hand side vectors

  integer LDA = M;                // The leading dimension of the array A.

  // B is a pointer the the N vector containing the solution
  doublereal* B;
  if( N < 6 )
    { B = new doublereal[6]; }   // The N-by-NRHS matrix of
  else
    { B = new doublereal[N]; }

  integer LDB = N;                // The leading dimension of the array B.

  // These values are used for the SVD computation
  integer INFO;                   // The info code
  integer INC = 1;                // The index increment

  doublereal ndq=1;               // norm of the iteration error
  size_t i;                   // the iteration counter
  char TRANSN = 'N';          // "N"ormal
  char TRANST = 'T';          // "T"transpose
  doublereal ALPHA = 1.0;
  doublereal* dq = new doublereal[N];

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
    doublereal e[6] = { dt[0], dt[1], dt[2], dr[0], dr[1], dr[2] };

    //
    for( integer j=0; j<N; j++ )
      { B[j] = 0.0; }
    for( integer j=0; j<6; j++ )
      { B[j] = e[j]; }

    // weights
    doublereal I[6][6] = { { 1.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
                           { 0.0, 1.0, 0.0, 0.0, 0.0, 0.0 },
                           { 0.0, 0.0, 1.0, 0.0, 0.0, 0.0 },
                           { 0.0, 0.0, 0.0, 1.0, 0.0, 0.0 },
                           { 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 },
                           { 0.0, 0.0, 0.0, 0.0, 0.0, 1.0 } };

    // We need to solve dq = J' ( JJ' + lambda I )^-1 e

    // I = JJ' + lambda I
    gemm( &TRANSN, &TRANST, &M, &M, &N,
          &ALPHA,
          &Js[0][0], &LDA,
          &Js[0][0], &LDA,
          &LAMBDA,
          &(I[0][0]), &M );

    // solve B = I\B
    integer IPIV[6];
    LDB=6;
    gesv( &M, &NRHS,
          &(I[0][0]), &LDA,
          &(IPIV[0]),
          &(B[0]), &LDB,
          &INFO );
    // should check the pivots

    // dq = J'B
    doublereal GAMMA = 0.0;
    gemv( &TRANST, &M, &N, &ALPHA,
          &(Js[0][0]), &LDA,
          &(B[0]), &INC,
          &GAMMA,
          dq, &INC );

    // compute the L2 norm of the error
    ndq = nrm2(&N, dq, &INC);

    // update the solution
    for(size_t j=0; j<links.size(); j++) q[j] += dq[j];
  }

  NormalizeAngles(q);

  delete[] B;
  delete[] dq;

  if( i==Niterations ) {
    stringstream ss;
    ss << "robManipulator::InverseKinematics: failed to converge in "
       << i << " iterations";
    mLastError = ss.str();
    return robManipulator::EFAILURE;
  }

  return robManipulator::ESUCCESS;
}

void robManipulator::NormalizeAngles( vctDynamicVector<double> &q )
{
  // normalize joint values
  for (size_t j=0; j<links.size(); j++) {
    if (links[j].GetType() == robJoint::HINGE) {
      q[j] = fmod((double)q[j], (double)2.0*cmnPI);
      if (cmnPI < q[j]) {
        q[j] = q[j] - 2.0*cmnPI;
      } else if (q[j] < -cmnPI) {
        q[j] = q[j] + 2.0 * cmnPI;
      }
    }
  }
}

#if 0
robManipulator::Errno
robManipulator::InverseKinematics( vctDynamicVector<double>& q,
                                   const vctFrame4x4<double>& Rts,
                                   double tolerance,
                                   size_t Niterations ){

  if( q.size() != links.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                      << ": Expected " << links.size() << " joints values. "
                      << " Got " << q.size()
                      << std::endl;
    return robManipulator::EFAILURE;
  }

  if( links.size() == 0 ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                      << ": The manipulator has no links."
                      << std::endl;
    return robManipulator::EFAILURE;
  }

  // A is a pointer to the 6xN spatial Jacobian
  int M = 6;                  // The number of rows of matrix A
  int N = links.size();       // The number of columns of matrix A
  int NHRS = 1;               // The number of right hand side vectors

  double* A = &(Js[0][0]);    // Pointer to the spatial Jacobian
  int LDA = M;                // The leading dimension of the array A.

  // B is a pointer the the N vector containing the solution
  double* B;                  // The N-by-NRHS matrix of right hand side matrix
  int LDB;                    // The leading dimension of the array B.
  if( N < 6 )    { B = new double[6]; LDB = 6; }
  else           { B = new double[N]; LDB = N; }

  // These values are used for the SVD computation
  double* S = new double[M];  // The singular values of A in decreasing order
  double RCOND = -1;          // Use machine precision to determine rank(A)
  int RANK;                   // The effective rank of A
  int LWORK = 128;            // The (safe) size of the workspace
  double WORK[128];           // The workspace
  int INFO;                   // The info code
  int INC = 1;                // The index increment

  double ndq=1;               // norm of the iteration error
  size_t i = 0;               // the iteration counter

  // loop until Niter are executed or the error is bellow the tolerance
  for( i=0; i<Niterations && tolerance<ndq; i++ ){

    // Evaluate the forward kinematics
    vctFrame4x4<double,VCT_ROW_MAJOR> Rt( ForwardKinematics( q ) );
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
    for( int j=0; j<N; j++ )
      { B[j] = 0.0; }

    for( int j=0; j<6; j++ )
      { B[j] = e[j]; }

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

  NormalizeAngles(q);

  delete[] S;
  delete[] B;

  std::cerr << "Nb iter " << i << "/" << Niterations << std::endl;
  if( i==(Niterations-1) ) return robManipulator::EFAILURE;
  else return robManipulator::ESUCCESS;
}
#endif

/*
 * Body manipulator Jacobian
 * Paul IEEE SMC 11(6) 1981
 * BIG FAT WARNIN': The jacobian is in column major (for Fortran)
 */
void robManipulator::JacobianBody( const vctDynamicVector<double>& q ) const {

  vctFrame4x4<double> U;  // set identity
  if( !tools.empty() ){
    if( tools[0] != NULL ){  U = tools[0]->ForwardKinematics( q ); }
  }

  for(int j=(int)links.size()-1; 0<=j; j--){

    if( links[j].GetConvention() == robKinematics::STANDARD_DH ||  // DH
        links[j].GetConvention() == robKinematics::HAYATI ){       // Hayati
      U = links[j].ForwardKinematics( q[j] ) * U;
    }

    if( links[j].GetType() == robJoint::HINGE ){         // Revolute joint
      // Jn is column major
      Jn[j][0] = U[0][3]*U[1][0] - U[1][3]*U[0][0];
      Jn[j][1] = U[0][3]*U[1][1] - U[1][3]*U[0][1];
      Jn[j][2] = U[0][3]*U[1][2] - U[1][3]*U[0][2];

      Jn[j][3] = U[2][0]; // nz
      Jn[j][4] = U[2][1]; // oz
      Jn[j][5] = U[2][2]; // az

    }

    if( links[j].GetType() == robJoint::SLIDER ){   // Prismatic joint
      // Jn is column major
      Jn[j][0] = U[2][0]; // nz
      Jn[j][1] = U[2][1]; // oz
      Jn[j][2] = U[2][2]; // az

      Jn[j][3] = 0.0;
      Jn[j][4] = 0.0;
      Jn[j][5] = 0.0;

    }

    if( links[j].GetConvention() == robKinematics::MODIFIED_DH )  // Modified DH
      { U = links[j].ForwardKinematics( q[j] ) * U; }

  }
}

bool robManipulator::JacobianBody(const vctDynamicVector<double>& q,
                                  vctDynamicMatrix<double>& J) const
{
  JacobianBody(q);
  if ((J.rows() != 6) && (J.cols() != links.size()))
    return false;
  for (size_t r = 0; r < 6; r++)
    for (size_t c = 0; c < links.size(); c++)
      J.Element(r,c) = Jn[c][r];
  return true;
}

void robManipulator::JacobianSpatial( const vctDynamicVector<double>& q ) const{

  JacobianBody( q );

  /*
   * Get the adjoint matrix to flip the body jacobian to spatial jacobian
   */
  vctFrame4x4<double> Rt0n = ForwardKinematics( q );
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
  integer M = 6;            // specifies  the number  of rows  of op( Ad )
  integer N = links.size();// specifies the number  of columns of the matrix Jn
  integer K = 6;            // specifies  the number of columns of op( Ad )
  doublereal ALPHA = 1.0;     // C := alpha*op( A )*op( B ) + beta*C
  doublereal* A = &Ad[0][0];  //
  integer LDA = 6;          // specifies the first dimension of A
  doublereal* B = &Jn[0][0];  //
  integer LDB = 6;          // specifies  the first dimension of B
  doublereal BETA = 0.0;      // C := alpha*op( A )*op( B ) + beta*C
  doublereal* C = &Js[0][0];  //
  integer LDC = 6;          // specifies the first dimension of C

  // Js = Ad * Jn
  gemm(TRANSA, TRANSB,
       &M, &N, &K,
       &ALPHA, A, &LDA,
               B, &LDB,
       &BETA,  C, &LDC);
}

bool robManipulator::JacobianSpatial(const vctDynamicVector<double>& q,
                                     vctDynamicMatrix<double>& J) const
{
  JacobianSpatial(q);
  if ((J.rows() != 6) && (J.cols() != links.size()))
    return false;
  for (size_t r = 0; r < 6; r++)
    for (size_t c = 0; c < links.size(); c++)
      J.Element(r,c) = Js[c][r];
  return true;
}

//////////////////////////////////////
//         DYNAMICS
//////////////////////////////////////

vctDynamicVector<double>
robManipulator::RNE( const vctDynamicVector<double>& q,
                     const vctDynamicVector<double>& qd,
                     const vctDynamicVector<double>& qdd,
                     const vctFixedSizeVector<double,6>& fext,
                     const double g) const {

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
  vctDynamicVector<double> tau(links.size(), 0.0);

  // The axis pointing "up"
  vctFixedSizeVector<double,3> z0(0.0, 0.0, 1.0);

  // acceleration of link 0
  // extract the rotation of the base and map the vector [0 0 1] in the robot
  // coordinate frame
  vctMatrixRotation3<double> R( Rtw0[0][0], Rtw0[0][1], Rtw0[0][2],
                                Rtw0[1][0], Rtw0[1][1], Rtw0[1][2],
                                Rtw0[2][0], Rtw0[2][1], Rtw0[2][2] );
  vd = R.Transpose() * z0 * g;

  // Forward recursion
  for(size_t i=0; i<links.size(); i++){

    double                          m; // mass
    vctFixedSizeVector<double,3>    s; // center of mass
    vctFixedSizeMatrix<double,3,3>  I; // moment of inertia
    vctMatrixRotation3<double>      A; // iA(i-1)
    vctFixedSizeVector<double,3>   ps; // distal link

    const robMass & massData = links[i].MassData();
    m  = massData.Mass();
    s  = massData.CenterOfMass();
    I  = massData.MomentOfInertia();

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
    vctMatrixRotation3<double>   A;
    vctFixedSizeVector<double,3> ps = links[i].PStar();
    vctFixedSizeVector<double,3> s  = links[i].MassData().CenterOfMass();

    if(i != (int)links.size()-1)              //
      A = links[i+1].Orientation( q[i+1] );    //

    f = A*f + F[i];                            // force exterted on i by i-1
    n = A*n + (ps%f) + (s%F[i]) + N[i];        // moment externed on i by i-1
    A = links[i].Orientation(q[i]).InverseSelf(); //

    if (links[i].GetType() == robJoint::HINGE )
      tau[i] = n*(A*z0);                       //
    if( links[i].GetType() == robJoint::SLIDER )
      tau[i] = f*(A*z0);                       //

  }

  return tau;
}

vctDynamicVector<double>
robManipulator::RNE_MDH( const vctDynamicVector<double>& q,
                         const vctDynamicVector<double>& qd,
                         const vctDynamicVector<double>& qdd,
                         const vctFixedSizeVector<double,6>& fext,
                         double g) const {
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
  vctDynamicVector<double> tau(links.size(), 0.0);

  // The axis pointing "up"
  vctFixedSizeVector<double,3> z0(0.0, 0.0, 1.0);

  // acceleration of link 0
  // extract the rotation of the base and map the vector [0 0 1] in the robot
  // coordinate frame
  vctMatrixRotation3<double> R( Rtw0[0][0], Rtw0[0][1], Rtw0[0][2],
                                Rtw0[1][0], Rtw0[1][1], Rtw0[1][2],
                                Rtw0[2][0], Rtw0[2][1], Rtw0[2][2] );
  vd = z0 * g;

  // Forward recursion
  for(size_t i=0; i<links.size(); i++){

    double                          m; // mass
    vctFixedSizeVector<double,3>    s; // center of mass
    vctFixedSizeMatrix<double,3,3>  I; // moment of inertia
    vctMatrixRotation3<double>      A; // iA(i-1)
    vctFixedSizeVector<double,3>   ps; // distal link

    const robMass & massData = links[i].MassData();
    m  = massData.Mass();
    s  = massData.CenterOfMass();
    I  = massData.MomentOfInertia();

    if( i==0 ){
      A  = R*links[i].Orientation( q[i] );
      A = A.InverseSelf();
    }
    else
      A  = links[i].Orientation( q[i] ).InverseSelf();

    ps = links[i].PStar();

    if (links[i].GetType() == robJoint::HINGE ){
      w  = A*w  + (z0*qd[i]) ;                      // angular velocity
      wd = A*wd + (z0*qdd[i]) + ((A*w)%(z0*qd[i])); // angular acceleration wrt i
      vd = A*((wd%ps) + (w%(w%ps)) + vd);           // linear acceleration
    }
    if( links[i].GetType() == robJoint::SLIDER ){
      vd = A*( (wd%ps) + (w%(w%ps)) + vd ) + 2.0*((A*w)%(z0*qd(i))) + z0*qdd(i);
      w = A*w;
      wd = A*wd;
    }
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
    vctMatrixRotation3<double>   A;
    vctFixedSizeVector<double,3> ps(0.0, 0.0, 0.0);
    vctFixedSizeVector<double,3> s  = links[i].MassData().CenterOfMass();

    if(i != (int)links.size()-1){              //
      A = links[i+1].Orientation( q[i+1] );    //
      ps = links[i+1].PStar();
    }

    n = A*n + (s%F[i]) + (ps%(A*f)) + N[i];    // moment externed on i by i-1
    f = A*f + F[i];                            // force exterted on i by i-1

    if (links[i].GetType() == robJoint::HINGE )
      tau[i] = n*(z0);                       //
    if( links[i].GetType() == robJoint::SLIDER )
      tau[i] = f*(z0);                       //

  }

  return tau;
}

vctDynamicVector<double>
robManipulator::CCG( const vctDynamicVector<double>& q,
                     const vctDynamicVector<double>& qd,
                     double g ) const
{
  if( q.size() != qd.size() ){
    cmnThrow(std::range_error("robManipulator::CCG: size of q and qd don't match"));
  }

  return RNE( q,           // call Newton-Euler with only the joints positions
              qd,          // and the joints velocities
              vctDynamicVector<double>( q.size(), 0.0 ), // assumes zero velocity
              vctFixedSizeVector<double,6>(0.0), // no external forces
              g );
}

vctDynamicVector<double>
robManipulator::CCG_MDH( const vctDynamicVector<double>& q,
                         const vctDynamicVector<double>& qd,
                         double g ) const
{
  if( q.size() != qd.size() ){
    cmnThrow(std::range_error("robManipulator::CCG_MDH: size of q and qd don't match"));
  }

  return RNE_MDH( q,           // call Newton-Euler with only the joints positions
                  qd,          // and the joints velocities
                  vctDynamicVector<double>( q.size(), 0.0 ), // assumes zero velocity
                  vctFixedSizeVector<double,6>(0.0), // no external forces
                  g );
}

vctFixedSizeVector<double,6>
robManipulator::BiasAcceleration( const vctDynamicVector<double>& q,
                                  const vctDynamicVector<double>& qd ) const {

  vctFixedSizeVector<double,3> w (0.0); // angular velocity
  vctFixedSizeVector<double,3> wd(0.0); // angular acceleration
  vctFixedSizeVector<double,3> vd(0.0); // linear velocity

  vctFixedSizeVector<double,3> z0(0.0, 0.0, 1.0);

  for(size_t i=0; i<links.size(); i++){

    vctMatrixRotation3<double>     A;    // iA(i-1)
    vctFixedSizeVector<double,3>  ps;

    A  = links[i].Orientation( q[i] ).InverseSelf();
    ps = links[i].PStar();                         // distal

    wd = A*( wd + ( w%(z0*qd[i]) ) );
    w  = A*( w  + (    z0*qd[i]  ) );
    vd = (wd%ps) + (w%(w%ps)) + A*vd;
  }

  return vctFixedSizeVector<double,6>(vd[0], vd[1], vd[2], wd[0], wd[1], wd[2]);
}

// A is column major!
vctDynamicMatrix<double>
robManipulator::JSinertia( const vctDynamicVector<double>& q ) const {

  if( q.size() != links.size() ){
    stringstream ss;
    ss << "robManipulator::JSinertia: expected "
       << links.size() << " joints values but received "
       << q.size();
    cmnThrow(std::range_error(ss.str()));
  }

  vctDynamicMatrix<double> A( links.size(), links.size(), 0.0 );

  for(size_t c=0; c<q.size(); c++){
    vctDynamicVector<double> qd( q.size(), 0.0 );  // velocities to zero
    vctDynamicVector<double> qdd( q.size(), 0.0 ); // accelerations to zero
    vctFixedSizeVector<double,6> fext(0.0);
    qdd[c] = 1.0;                                  // ith acceleration to 1

    vctDynamicVector<double> h = RNE( q, qd, qdd, fext, 0.0  );
    for( size_t r=0; r<links.size(); r++ )
      { A[c][r] = h[r]; }
  }

  return A;

}

void robManipulator::JSinertia( double **A,
                                const vctDynamicVector<double>& q ) const {

  if( q.size() != links.size() ){
    stringstream ss;
    ss << "robManipulator::JSinertia: expected "
       << links.size() << " joints values but received "
       << q.size();
    cmnThrow(std::range_error(ss.str()));
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
void robManipulator::OSinertia( double Ac[6][6],
                                const vctDynamicVector<double>& q ) const {
  char SIDE = 'R';                // dsymm C := alpha*B*A + beta*C,
  char UPLO = 'L';                // lower triangular (upper triangular for CM)
  char TRANST = 'T';
  char TRANSN = 'N';
  doublereal ALPHA = 1.0;
  doublereal BETA = 0.0;                //
  integer NEQS = 6;                   // num rows of the jacobian
  integer NJOINTS = links.size();    // The order of matrix A
  integer INFO;
                                  // The inertia matrix
  doublereal** A   = rmatrix(0, links.size()-1, 0, links.size()-1);
  integer LDA = links.size();        // The leading dimension of A
  doublereal** JAi = rmatrix(0, links.size()-1, 0, 5);
  integer LDJAi = 6;
  integer LDJ = 6;

  for(size_t r=0; r<links.size(); r++)
    for(size_t c=0; c<links.size(); c++)
      A[r][c] = 0.0;

  JSinertia(A, q);         // compute the NxN joint space inertia matrix

  // A^-1
  // Cholesky factorization of the symmetric positive definite matrix A
  // A = L  * L**T
  potrf(&UPLO, &NJOINTS, &A[0][0], &LDA, &INFO);
  if(INFO<0) {
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                      << ": The " << INFO << "th argument to potrf is illegal."
                      << std::endl;
  }
  else if(0<INFO) {
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                      << ": The matrix for potrf is not positive definite."
                      << std::endl;
  }

  // invert A
  //
  potri(&UPLO, &NJOINTS, &A[0][0], &LDA, &INFO);
  if(INFO<0) {
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                      << ": The " << INFO << "th argument to potri is illegal."
                      << std::endl;
  }
  else if(0<INFO) {
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                      << ": The matrix passed to potri is singular."
                      << std::endl;
  }

  for( size_t c=0; c<links.size(); c++ )
    for( size_t r=c; r<links.size(); r++ )
      A[r][c] = A[c][r];

  JacobianBody( q );

  // J*A^-1 : a 6xN matrix column major
  // C := alpha*B*A + beta*C,
  symm(&SIDE, &UPLO, &NEQS, &NJOINTS,
       &ALPHA, &A [0][0], &LDA,
       &Jn[0][0], &LDJ,
       &BETA, &JAi[0][0], &LDJAi);

  // J*A^-1*J'
  // C := alpha*op( A )*op( B ) + beta*C
  integer LDAc = 6;
  gemm(&TRANSN, &TRANST, &NEQS, &NEQS, &NJOINTS,
       &ALPHA, &JAi[0][0], &LDJAi,
       &Jn[0][0],  &LDJ,
       &BETA,  &Ac[0][0],  &LDAc);
  /*
  // (J*A^-1*J')^-1
  // Factorize the symmetric matrix
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
  */

  doublereal I[6][6];
  integer LDI = 6;
  for( int r=0; r<6; r++ )
    for( int c=0; c<6; c++ )
      I[r][c] = 0.0;
  for( int r=0; r<6; r++ )
    I[r][r] = 1.0;

  doublereal S[6];                // The singular values of A in decreasing order
  doublereal RCOND = -1;          // Use machine precision to determine rank(A)
  integer RANK;                   // The effective rank of A
  integer LWORK = 128;            // The (safe) size of the workspace
  doublereal WORK[128];           // The workspace

  // compute the minimum norm solution
  gelss( &NEQS, &NEQS, &NEQS,
         &Ac[0][0], &LDI,           //
         &I[0][0],  &LDI,           // error vector
         S, &RCOND, &RANK,          // SVD parameters
         WORK, &LWORK, &INFO );

  for( int r=0; r<6; r++ )
    for( int c=0; c<6; c++ )
      Ac[r][c] = I[r][c];

  free_rmatrix(   A, 0, 0 );
  free_rmatrix( JAi, 0, 0 );
}

vctDynamicVector<double>
robManipulator::InverseDynamics(const vctDynamicVector<double>& q,
                                const vctDynamicVector<double>& qd,
                                const vctFixedSizeVector<double,6>& vdwd )const{


  doublereal* JTAcF = new doublereal[links.size()];
  doublereal* JTAch = new doublereal[links.size()];
  for(size_t i=0; i<links.size(); i++) JTAcF[i] = 0.0;
  for(size_t i=0; i<links.size(); i++) JTAch[i] = 0.0;

  // make sure there's a force, otherwise skip this and just consider ccg
  if(0.0 < vdwd.Norm()){

    char UPLO = 'L';
    integer NEQS = 6;
    integer INC = 1;
    char TRANST = 'T';
    integer NJOINTS = links.size();

    doublereal ALPHA =  1.0;      //
    doublereal BETA  =  0.0;      //

    doublereal Ac[6][6];          // OS inertia matrix
    integer LDAc = 6;             // 1st dimension of Ac
    OSinertia( Ac, q );       // compute OS inertia matrix

    integer LDJ = 6;              // 1st dimention of Jn

    vctFixedSizeVector<double,6> h;
    h = BiasAcceleration( q, qd ); // h = Jdqd

    doublereal hv[6];            // make an array of h
    for( int i=0; i<6; i++ ) { hv[i] = h[i]; }

    // Ac*h
    // y := alpha*A*x + beta*y,
    doublereal Ach[6];                                  // symmetric matrix*vector
    symv( &UPLO, &NEQS,
          &ALPHA,
          &Ac[0][0], &LDAc,
          &hv[0],    &INC,
          &BETA,
          &Ach[0],   &INC );

    // J'*Ac*h
    // y := alpha*A'*x + beta*y,
    gemv( &TRANST, &NEQS, &NJOINTS,
          &ALPHA,
          &Jn[0][0], &LDJ,
          &Ach[0],   &INC,
          &BETA,
          &JTAch[0], &INC );

    // Ac*F
    doublereal Fv[6];            // make an array of vdwd
    for( int i=0; i<6; i++ ) { Fv[i] = vdwd[i]; }

    doublereal AcF[6];
    symv( &UPLO, &NEQS,
          &ALPHA,
          &Ac[0][0], &LDAc,
          &Fv[0],    &INC,
          &BETA,
          &AcF[0],   &INC );

    // J'*Ac*F
    gemv(&TRANST, &NEQS, &NJOINTS,
         &ALPHA,
         &Jn[0][0], &LDJ,
         &AcF[0], &INC,
         &BETA,
         &JTAcF[0], &INC);

  }

  vctDynamicVector<double> ccg = CCG(q, qd);        // compute the coriolis+grav
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

  doublereal* Inertterm  = new doublereal[ links.size() ];
  for( size_t i=0; i<links.size(); i++ )
    Inertterm [i] = 0;

  // Ensure there's an acceleration. Orthewise only consider ccg
  if( 0.0 < qdd.Norm() ){

    char LOW = 'L';
    integer N = links.size();
    integer LDA = links.size();
    integer INC = 1;
    doublereal ALPHA = 1.0;
    doublereal BETA = 0.0;

    doublereal** A = rmatrix(0, links.size()-1, 0, links.size()-1);
    JSinertia(A,q);

    // must copy to an array for symv
    doublereal* qddv = new doublereal[ links.size() ];
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

#include <cisstRobot/robDH.h>
#include <cisstRobot/robModifiedDH.h>
#include <cisstRobot/robHayati.h>

vctFixedSizeMatrix<double,4,4>
robManipulator::SE3Difference( const vctFrame4x4<double>& Rt1,
                               const vctFrame4x4<double>& Rt2 ) const {

  vctFixedSizeMatrix<double,4,4> dRt(0.0);

  for( size_t r=0; r<4; r++ ){
    for( size_t c=0; c<4; c++ ){
      dRt[r][c] = Rt1[r][c] - Rt2[r][c];
    }
  }
  return dRt;
}

void
robManipulator::AddIdentificationColumn
( vctDynamicMatrix<double>& J,
  vctFixedSizeMatrix<double,4,4>& delRt ) const{

  size_t c = J.cols();
  J.resize( 6, c+1 );
  J[0][c] = delRt[0][3];   // dx
  J[1][c] = delRt[1][3];   // dy
  J[2][c] = delRt[2][3];   // dz

  J[3][c] = (delRt[2][1] - delRt[1][2])/2.0;  // average wx
  J[4][c] = (delRt[0][2] - delRt[2][0])/2.0;  // average wy
  J[5][c] = (delRt[1][0] - delRt[0][1])/2.0;  // average wz

}

vctDynamicMatrix<double>
robManipulator::JacobianKinematicsIdentification
( const vctDynamicVector<double>& q, double epsilon ) const {

  // the identification jacobian
  vctDynamicMatrix<double> J(0,0,VCT_COL_MAJOR);

  // the forward kinematics at q
  vctFrame4x4<double> Rt = ForwardKinematics( q );
  vctFrame4x4<double> Rt4x4i( Rt );
  Rt4x4i.InverseSelf();

  // We need a 4x4 matrix because we will mutiply with twists matrices
  vctFixedSizeMatrix<double,4,4> Rti( Rt4x4i );

  // for each link
  for( size_t i=0; i<links.size(); i++ ){

    // get the kinematics parameters
    robKinematics* ki = links[i].GetKinematics();

    // for each parameter
    switch( ki->GetConvention() ){

    case robKinematics::STANDARD_DH:
      {
        robDH* dh = dynamic_cast<robDH*>( ki );

        {
          double old = dh->GetRotationX();
          dh->SetRotationX( old + epsilon );
          vctFrame4x4<double> Rt_ = ForwardKinematics( q );
          dh->SetRotationX( old );
          vctFixedSizeMatrix<double,4,4> delRt;
          delRt = Rti*SE3Difference( Rt_, Rt )/epsilon;
          AddIdentificationColumn( J, delRt );
        }

        {
          double old = dh->GetTranslationX();
          dh->SetTranslationX( old + epsilon );
          vctFrame4x4<double> Rt_ = ForwardKinematics( q );
          dh->SetTranslationX( old );
          vctFixedSizeMatrix<double,4,4> delRt;
          delRt = Rti*SE3Difference( Rt_, Rt )/epsilon;
          AddIdentificationColumn( J, delRt );
        }

        {
          double old = dh->GetRotationZ();
          dh->SetRotationZ( old + epsilon );
          vctFrame4x4<double> Rt_ = ForwardKinematics( q );
          dh->SetRotationZ( old );
          vctFixedSizeMatrix<double,4,4> delRt;
          delRt = Rti*SE3Difference( Rt_, Rt )/epsilon;
          AddIdentificationColumn( J, delRt );
        }

        {
          double old = dh->GetTranslationZ();
          dh->SetTranslationZ( old + epsilon );
          vctFrame4x4<double> Rt_ = ForwardKinematics( q );
          dh->SetTranslationZ( old );
          vctFixedSizeMatrix<double,4,4> delRt;
          delRt = Rti*SE3Difference( Rt_, Rt )/epsilon;
          AddIdentificationColumn( J, delRt );
        }

      }
      break;

    case robKinematics::MODIFIED_DH:
      {
        robModifiedDH* mdh = dynamic_cast<robModifiedDH*>( ki );

        {
          double old = mdh->GetRotationX();
          mdh->SetRotationX( old + epsilon );
          vctFrame4x4<double> Rt_ = ForwardKinematics( q );
          mdh->SetRotationX( old );
          vctFixedSizeMatrix<double,4,4> delRt;
          delRt = Rti*SE3Difference( Rt_, Rt )/epsilon;
          AddIdentificationColumn( J, delRt );
        }

        {
          double old = mdh->GetTranslationX();
          mdh->SetTranslationX( old + epsilon );
          vctFrame4x4<double> Rt_ = ForwardKinematics( q );
          mdh->SetTranslationX( old );
          vctFixedSizeMatrix<double,4,4> delRt;
          delRt = Rti*SE3Difference( Rt_, Rt )/epsilon;
          AddIdentificationColumn( J, delRt );
        }

        {
          double old = mdh->GetRotationZ();
          mdh->SetRotationZ( old + epsilon );
          vctFrame4x4<double> Rt_ = ForwardKinematics( q );
          mdh->SetRotationZ( old );
          vctFixedSizeMatrix<double,4,4> delRt;
          delRt = Rti*SE3Difference( Rt_, Rt )/epsilon;
          AddIdentificationColumn( J, delRt );
        }

        {
          double old = mdh->GetTranslationZ();
          mdh->SetTranslationZ( old + epsilon );
          vctFrame4x4<double> Rt_ = ForwardKinematics( q );
          mdh->SetTranslationZ( old );
          vctFixedSizeMatrix<double,4,4> delRt;
          delRt = Rti*SE3Difference( Rt_, Rt )/epsilon;
          AddIdentificationColumn( J, delRt );
        }

      }
      break;

    case robKinematics::HAYATI:
      {
        robHayati* h = dynamic_cast<robHayati*>( ki );

        switch( h->GetType() ){
        case robJoint::HINGE:
          {

            {
              double old = h->GetRotationX();
              h->SetRotationX( old + epsilon );
              vctFrame4x4<double> Rt_ = ForwardKinematics( q );
              h->SetRotationX( old );
              vctFixedSizeMatrix<double,4,4> delRt;
              delRt = Rti*SE3Difference( Rt_,Rt )/epsilon;
              AddIdentificationColumn( J, delRt );
            }

            {
              double old = h->GetRotationY();
              h->SetRotationY( old + epsilon );
              vctFrame4x4<double> Rt_ = ForwardKinematics( q );
              h->SetRotationY( old );
              vctFixedSizeMatrix<double,4,4> delRt;
              delRt = Rti*SE3Difference( Rt_, Rt )/epsilon;
              AddIdentificationColumn( J, delRt );
            }

            {
              double old = h->GetRotationZ();
              h->SetRotationZ( old + epsilon );
              vctFrame4x4<double> Rt_ = ForwardKinematics( q );
              h->SetRotationZ( old );
              vctFixedSizeMatrix<double,4,4> delRt;
              delRt = Rti*SE3Difference( Rt_, Rt )/epsilon;
              AddIdentificationColumn( J, delRt );
            }

            {
              double old = h->GetTranslationX();
              h->SetTranslationX( old + epsilon );
              vctFrame4x4<double> Rt_ = ForwardKinematics( q );
              h->SetTranslationX( old );
              vctFixedSizeMatrix<double,4,4> delRt;
              delRt = Rti*SE3Difference( Rt_, Rt )/epsilon;
              AddIdentificationColumn( J, delRt );
            }

          }
          break;

        case robJoint::SLIDER:
          {

            {
              double old = h->GetRotationX();
              h->SetRotationX( old + epsilon );
              vctFrame4x4<double> Rt_ = ForwardKinematics( q );
              h->SetRotationX( old );
              vctFixedSizeMatrix<double,4,4> delRt;
              delRt = Rti*SE3Difference( Rt_, Rt )/epsilon;
              AddIdentificationColumn( J, delRt );
            }

            {
              double old = h->GetRotationY();
              h->SetRotationY( old + epsilon );
              vctFrame4x4<double> Rt_ = ForwardKinematics( q );
              h->SetRotationY( old );
              vctFixedSizeMatrix<double,4,4> delRt;
              delRt = Rti*SE3Difference( Rt_, Rt )/epsilon;
              AddIdentificationColumn( J, delRt );
            }

            {
              double old = h->GetTranslationZ();
              h->SetTranslationZ( old + epsilon );
              vctFrame4x4<double> Rt_ = ForwardKinematics( q );
              h->SetTranslationZ( old );
              vctFixedSizeMatrix<double,4,4> delRt;
              delRt = Rti*SE3Difference( Rt_,Rt )/epsilon;
              AddIdentificationColumn( J, delRt );
            }

          }
          break;

        default:
          CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                            << "Unsupported Hayati joint." << std::endl;
        }

      }
      break;

    default:
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                        << "Unsupported kinematic convention." << std::endl;

    }

  }

  return J;
}


void robManipulator::PrintKinematics( std::ostream& os ) const {

  for( size_t i=0; i<links.size(); i++ ){
    links[i].GetKinematics()->Write( os );
    os << std::endl;
  }

}


robManipulator::Errno robManipulator::Truncate(const size_t linksToKeep)
{
  // not enough links
  if (linksToKeep > links.size()) {
    stringstream ss;
    ss << "robManipulator::Truncate: can't truncate to "
       << linksToKeep << " since the manipulator has only "
       << links.size() << " links";
    mLastError = ss.str();
    CMN_LOG_RUN_ERROR << mLastError << std::endl;
    return robManipulator::EFAILURE;
  }

  // no change
  if (linksToKeep == links.size()) {
    return robManipulator::ESUCCESS;
  }

  // remove links
  links.resize(linksToKeep);

  // free existing jacobians
  if( Jn != NULL ){ free_rmatrix(Jn, 0, 0); }
  if( Js != NULL ){ free_rmatrix(Js, 0, 0); }

  // allocate new memory for jacobians
  if (linksToKeep != 0) {
    Js = rmatrix(0, links.size()-1, 0, 5);
    Jn = rmatrix(0, links.size()-1, 0, 5);
  } else {
    Js = NULL;
    Jn = NULL;
  }

  return robManipulator::ESUCCESS;
}

bool robManipulator::ClampJointValueAndUpdateError(const size_t link,
                                                   double & value,
                                                   const double & tolerance)
{
  const double qMax = links[link].GetKinematics()->PositionMax();
  const double qMin = links[link].GetKinematics()->PositionMin();
  if (value > qMax) {
    // only set error if not within tolerance
    if (value > (qMax + tolerance)) {
      value = qMax;
      stringstream ss;
      ss << link;
      mLastError = "robManipulator: position clamped to upper joint limit " + ss.str();
      return true;
    }
    // clamp anyway
    value = qMax;
    return false;
  } else if (value < qMin) {
    if (value < (qMin - tolerance)) {
      value = qMin;
      stringstream ss;
      ss << link;
      mLastError = "robManipulator: position clamped to lower joint limit " + ss.str();
      return true;
    }
    value = qMin;
    return false;
  }
  return false;
}
