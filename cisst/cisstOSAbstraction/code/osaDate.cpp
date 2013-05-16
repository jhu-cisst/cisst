#include "../osaDate.h"


std::string osaDate::toString()
{
	std::stringstream ss;
    
	ss<<year<<"/"<<month<<"/"<<day<<" "<<hour<<":"<<minute<<":"<<second;

	
	return ss.str();
}
