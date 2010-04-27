#include <cisstDevices/trajectories/devLinearR3.h>
#include <cisstDevices/trajectories/devSLERP.h>
#include <cisstDevices/devExport.h>

#ifndef _devLinearSE3_h
#define _devLinearSE3_h

class CISST_EXPORT devLinearSE3 : public devLinearR3, public devSLERP {

 public:

  devLinearSE3( const std::string& TaskName, 
		const std::string& InputFunctionNameR3,
		const std::string& InputFunctionNameSO3,
		double period, 
		bool enabled,
		const vctFrame4x4<double>& Rtw1,
		double vmax, 
		double wmax ) :
    devLinearR3( TaskName+"R3", 
		 InputFunctionNameR3, 
		 period, 
		 enabled,
		 Rtw1.Translation(),
		 vmax ),
    devSLERP( TaskName+"SO3", 
	      InputFunctionNameSO3, 
	      period, 
	      enabled,
	      vctQuaternionRotation3<double>
	      (vctMatrixRotation3<double>(Rtw1[0][0], Rtw1[0][1], Rtw1[0][2],
					  Rtw1[1][0], Rtw1[1][1], Rtw1[1][2], 
					  Rtw1[2][0], Rtw1[2][1], Rtw1[2][2])),
	      wmax ){}

  ~devLinearSE3(){}
  
};

#endif
