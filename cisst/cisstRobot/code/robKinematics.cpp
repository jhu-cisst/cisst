#include <algorithm>  // for transform
#include <cctype>     // toupper

#include <cisstRobot/robKinematics.h>

#include <cisstRobot/robDH.h>
#include <cisstRobot/robModifiedDH.h>
#include <cisstRobot/robHayati.h>

robKinematics::robKinematics( robKinematics::Convention convention ) :
  convention( convention ) {}

robKinematics::robKinematics( const robJoint& joint,
			      robKinematics::Convention convention ) : 
  robJoint( joint ),
  convention( convention ){}

robKinematics* robKinematics::Instantiate( const std::string& type ){

  std::string convention( type );
  std::transform( convention.begin(),
		  convention.end(),
		  convention.begin(),
		  ::toupper );
  
  if( convention.compare( "STANDARD" ) == 0 )
    { return (robKinematics*) new robDH; }

  if( convention.compare( "MODIFIED" ) == 0 )
    { return (robKinematics*) new robModifiedDH; }

  if( convention.compare( "HAYATI" ) == 0 )
    { return (robKinematics*) new robHayati; }

  return NULL;
}

robKinematics::Errno robKinematics::Read( std::istream& is ){
  ReadParameters( is );
  return robJoint::Read( is );
}

#if CISST_HAS_JSON
robKinematics::Errno robKinematics::Read(const Json::Value &config)
{
    ReadParameters(config);
    return robJoint::Read(config);
}
#endif

robKinematics::Errno robKinematics::Write( std::ostream& os ) const {
  WriteParameters( os );
  return robJoint::Write( os );
}

