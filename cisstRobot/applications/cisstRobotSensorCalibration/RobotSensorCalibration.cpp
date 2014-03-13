#include <cisstNumerical/nmrSymmetricEigenProblem.h>
#include <cisstNumerical/nmrLSMinNorm.h>
#include <cisstNumerical/nmrInverse.h>
#include <cisstVector/vctFrame4x4.h>
#include <cisstVector/vctMatrixRotation3.h>
#include <cisstCommon/cmnRandomSequence.h>

/**
   Solves AX = XB.
   Algorithm based on least-squares solution from F. Park IEEE T.Rob 1994.

   It takes two input files Afile and Bfile. These files contain a sequence of
   vctFrame4x4.
*/

int main( int argc, char** argv ){

  cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );

  std::ostringstream usage;
  usage  << "Usage: " << std::endl << argv[0] << " Afile Bfile" << std::endl 
	 << "Afile and Bfile are formated using vctFrame4x4<T>::ToStreamRaw" 
	 << std::endl;
  
  if( argc != 3 ){
    std::cerr << usage.str() << std::endl;
    return -1;
  }

  std::ifstream Afile( argv[1] );
  if( !Afile ){
    std::cerr << argv[0] << ": cannot find file " << argv[1] << std::endl;
    return -1;
  }

  std::vector< vctFrame4x4<double> > stdA;
  while( !Afile.eof() ){
    vctFrame4x4<double> Rt;
    if( Rt.FromStreamRaw( Afile ) )
      { stdA.push_back( Rt ); }
    else
      break;
  }

  std::ifstream Bfile( argv[2] );
  if( !Bfile ){
    std::cerr << argv[0] << ": cannot find file " << argv[2] << std::endl;
    return -1;
  }

  std::vector< vctFrame4x4<double> > stdB;
  while( !Bfile.eof() ){
    vctFrame4x4<double> Rt;
    if( Rt.FromStreamRaw( Bfile ) )
      { stdB.push_back( Rt ); }
    else
      break;
  }
  
  if( stdA.size() != stdB.size() ){
    std::cout << "Found " << stdA.size() << " transformations in " << argv[1]
	      << " and " << stdB.size() << " transformations in " << argv[2]
	      << std::endl;
    return -1;
  }

  std::vector< vctDynamicMatrix<double> > stdra;
  for( unsigned int i=0; i<stdA.size(); i++ ){
    vctFixedSizeMatrix<double,3,3> Ra( stdA[i].Rotation() );
    double theta = acos( (Ra.Trace()-1.0) / 2.0 );
    vctFixedSizeMatrix<double,3,3> ra;
    ra = theta * (Ra - Ra.Transpose()) / ( 2.0*sin(theta) );
    vctDynamicMatrix<double> wa( 3, 1, 0.0, VCT_COL_MAJOR );
    wa[0][0] = ra(2,1);
    wa[1][0] = ra(0,2);
    wa[2][0] = ra(1,0);
    stdra.push_back( wa );
  }

  std::vector< vctDynamicMatrix<double> > stdrb;
  for( size_t i=0; i<stdB.size(); i++ ){
    vctFixedSizeMatrix<double,3,3> Rb( stdB[i].Rotation() );
    double theta = acos( (Rb.Trace()-1.0) / 2.0 );
    vctFixedSizeMatrix<double,3,3> rb;
    rb = theta * (Rb - Rb.Transpose()) / ( 2.0*sin(theta) );
    vctDynamicMatrix<double> wb( 3, 1, 0.0, VCT_COL_MAJOR );
    wb[0][0] = rb(2,1);
    wb[1][0] = rb(0,2);
    wb[2][0] = rb(1,0);
    stdrb.push_back( wb );
  }

  vctDynamicMatrix<double> M( 3, 3, 0.0, VCT_COL_MAJOR );
  for( size_t i=0; i<stdA.size(); i++ )
    { M = M + stdrb[i] * stdra[i].Transpose(); }
  
  vctDynamicMatrix<double> MtM( 3, 3, 0.0, VCT_COL_MAJOR );
  vctDynamicMatrix<double> tmp = M.Transpose() * M;
  for( size_t r=0; r<MtM.rows(); r++ ){
    for( size_t c=0; c<MtM.cols(); c++ ){
      MtM[r][c] = tmp[r][c];
    }
  }

  vctDynamicMatrix<double> V( 3, 3, 0.0, VCT_COL_MAJOR );
  vctDynamicVector<double> d( 3, 0.0 );
  nmrSymmetricEigenProblem( MtM, d, V );
  
  vctDynamicMatrix<double> Dsqrtinv( 3, 3, 0.0, VCT_COL_MAJOR );
  Dsqrtinv[0][0] = 1.0 / sqrt( d[0] );
  Dsqrtinv[1][1] = 1.0 / sqrt( d[1] );
  Dsqrtinv[2][2] = 1.0 / sqrt( d[2] );

  vctDynamicMatrix<double> Vinv( V );
  nmrInverse( Vinv );

  vctDynamicMatrix<double> r = V * Dsqrtinv * Vinv * M.Transpose();
  
  vctMatrixRotation3<double> R( r[0][0], r[0][1], r[0][2], 
				r[1][0], r[1][1], r[1][2], 
				r[2][0], r[2][1], r[2][2], 
				VCT_NORMALIZE );

  vctDynamicMatrix<double> E( stdA.size()*3, 3, 0.0, VCT_COL_MAJOR );
  vctDynamicMatrix<double> e( stdA.size()*3, 1, 0.0, VCT_COL_MAJOR );
  vctFixedSizeMatrix<double,3,3> I( 0.0 );
  I[0][0] = 1.0; I[1][1] = 1.0; I[2][2] = 1.0; 

  for( size_t i=0; i<stdA.size(); i++ ){

    vctFixedSizeMatrix<double,3,3> Ra( stdA[i].Rotation() );
    vctFixedSizeMatrix<double,3,3> dE = I - Ra;
    vctFixedSizeVector<double,3> ta = stdA[i].Translation();
    vctFixedSizeVector<double,3> tb = R * stdB[i].Translation();
    vctFixedSizeVector<double,3> de = ta - tb;

    for( size_t r=0; r<3; r++ ){
      for( size_t c=0; c<3; c++ ){
	E[i*3+r][c] = dE[r][c];
	e[i*3+r][0] = de[r];
      }
    }
  }

  vctDynamicMatrix<double> T = nmrLSMinNorm( E, e );
  vctFixedSizeVector<double,3> t( T[0][0], T[1][0], T[2][0] );
  vctFrame4x4<double> X( R, t );
  
  std::cout << "Solution: " << std::endl;
  std::cout << X << std::endl;
  
  std::cout << "Errors:" << std::endl;
  for( size_t i=0; i<stdA.size(); i++ )
    { std::cout << (stdA[i] * X) - ( X * stdB[i] ) << std::endl << std::endl; }
  
  return 0;
}
