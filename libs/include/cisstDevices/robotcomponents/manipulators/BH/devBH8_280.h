

#ifndef _devBH8_280
#define _devBH8_280

#include <cisstDevices/manipulators/devManipulator.h>

class CISST_EXPORT devBH8_280 : public devManipulator {

 public:
  
  devBH8_280( const std::string& taskname, 
	      double period, 
	      const std::string& serialdev );
  
  vctDynamicVector<double> Read() = 0;
  void Write( const vctDynamicVector<double>& values ) = 0;

};

#endif
