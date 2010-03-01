#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robMapping.h>

robMapping::robMapping(){}

robMapping::robMapping( const robSpace& domain, const robSpace& codomain ){
  this->domain = domain;      // set the mapping domain
  this->codomain = codomain;  // set the mapping codomain
}

robSpace robMapping::Domain()   const { return domain; }
robSpace robMapping::Codomain() const { return codomain; }


