#include <cisstCommon/cmnRandomSequence.h>
#include <cisstRobot/robManipulator.h>

#include <iostream>

int main( int argc, char** argv ){

  cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );

  std::ostringstream usage;
  usage  << "Usage: " << std::endl
	 << argv[0] << " robfile [q1 ... qn q1 ... qn]";

  if( argc == 1 ){
    std::cerr << usage.str() << std::endl;
    return -1;
  }

  robManipulator manipulator( argv[1] );

  // parsing from stream
  if( argc == 2 ){ 
    bool eof = false;
    
    while( !eof ){

      vctDynamicVector<double> q( manipulator.links.size(), 0.0 );
      if( q.FromStreamRaw( std::cin ) ){
	vctFrame4x4<double> Rt = manipulator.ForwardKinematics( q );
	std::cout << std::endl;
	std::cout.precision(10);
	Rt.ToStreamRaw( std::cout );
      }
      else
	{ eof = true; }
    }

  }

  // parsing from command line
  else{

    int cnt=2;
    
    while( cnt<argc ){

      vctDynamicVector<double> q( manipulator.links.size(), 0.0 );
      for( size_t i=0; i<q.size() && cnt<argc; i++, cnt++ )
	{ sscanf( argv[cnt], "%lf", &q[i] ); }

      vctFrame4x4<double> Rt = manipulator.ForwardKinematics( q );
      std::cout << std::endl;
      std::cout.precision(10);
      Rt.ToStreamRaw( std::cout );

    }

  }

  return 0;

}
