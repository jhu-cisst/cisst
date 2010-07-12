#include <cisstRobot/robFunctionSE3.h>

robFunctionSE3::robFunctionSE3( double t1, 
				const vctFrame4x4<double>& Rtw1,
				const vctFixedSizeVector<double,6>& v1w1,
				const vctFixedSizeVector<double,6>& v1dw1d,
				double t2, 
				const vctFrame4x4<double>& Rtw2,
				const vctFixedSizeVector<double,6>& v2w2,
				const vctFixedSizeVector<double,6>& v2dw2d ) : 
  robFunction( t1, t2 ),
  Rtw1( Rtw1 ), v1w1( v1w1 ), v1dw1d( v1dw1d ),
  Rtw2( Rtw2 ), v2w2( v2w2 ), v2dw2d( v2dw2d ){}


void robFunctionSE3::InitialState( vctFrame4x4<double>& Rt,
				   vctFixedSizeVector<double,6>& vw,
				   vctFixedSizeVector<double,6>& vdwd ){
  Rt   = this->Rtw1;
  vw   = this->v1w1;
  vdwd = this->v1dw1d;
}

void robFunctionSE3::FinalState( vctFrame4x4<double>& Rt,
				 vctFixedSizeVector<double,6>& vw,
				 vctFixedSizeVector<double,6>& vdwd ){
  Rt   = this->Rtw2;
  vw   = this->v2w2;
  vdwd = this->v2dw2d;
}


