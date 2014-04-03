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

#include <cisstCommon/cmnRandomSequence.h>
#include <cisstRobot/robManipulator.h>

#include <iostream>

int main( int argc, char** argv ){

  cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );

  std::ostringstream usage;
  usage  << "Usage: " << std::endl
	 << argv[0] << " robfile [nsamples] [seed]";


  if( argc != 2 && argc != 3 && argc != 4 ){
    std::cerr << usage.str() << std::endl;
    return -1;
  }

  int nsamples = 1;
  if( argc == 3 || argc == 4 ){
    if( sscanf( argv[2], "%d", &nsamples ) != 1 ){
      std::cerr << usage.str() << std::endl;
      return -1;
    }
    if( nsamples <= 0 ){
      std::cerr << usage.str() << std::endl;
      return -1;
    }
  }

  cmnRandomSequence::SeedType seed = 0;
  if( argc == 4 ){
    if( sscanf( argv[3], "%d", &seed ) != 1 ){
      std::cerr << usage.str() << std::endl;
      return -1;
    }
  }

  cmnRandomSequence& rs = cmnRandomSequence::GetInstance();
  rs.SetSeed( seed );

  robManipulator manipulator( argv[1] );

  for( int i=0; i<nsamples; i++ ){

    vctDynamicVector<double> qi( manipulator.links.size(), 0.0 );

    for( size_t j=0; j<qi.size(); j++ ){
      qi[j] = rs.ExtractRandomDouble( manipulator.links[j].PositionMin(),
				      manipulator.links[j].PositionMax() );
    }
    std::cout << std::endl;
    std::cout.precision(10);
    qi.ToStreamRaw( std::cout );
  }

  return 0;

}
