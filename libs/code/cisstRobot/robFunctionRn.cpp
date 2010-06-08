#include <cisstRobot/robFunctionRn.h>

robFunctionRn::robFunctionRn( double t1, double y1, double y1d, double y1dd, 
			      double t2, double y2, double y2d, double y2dd ):
  robFunction( t1, t2 ),
  y1  ( vctDynamicVector<double>( 1, y1   ) ),
  y1d ( vctDynamicVector<double>( 1, y1d  ) ),
  y1dd( vctDynamicVector<double>( 1, y1dd ) ),
  y2  ( vctDynamicVector<double>( 1, y2   ) ),
  y2d ( vctDynamicVector<double>( 1, y2d  ) ),
  y2dd( vctDynamicVector<double>( 1, y2dd ) ){}

robFunctionRn::robFunctionRn( double t1, 
			      const vctFixedSizeVector<double,3>& p1, 
			      const vctFixedSizeVector<double,3>& v1, 
			      const vctFixedSizeVector<double,3>& v1d, 
			      double t2, 
			      const vctFixedSizeVector<double,3>& p2,
			      const vctFixedSizeVector<double,3>& v2,
			      const vctFixedSizeVector<double,3>& v2d ):
  robFunction( t1, t2 ),
  y1  ( vctDynamicVector<double>( 3,  p1[0],  p1[1],  p1[2] ) ),
  y1d ( vctDynamicVector<double>( 3,  v1[0],  v1[1],  v1[2] ) ),
  y1dd( vctDynamicVector<double>( 3, v1d[0], v1d[1], v1d[2] ) ),
  y2  ( vctDynamicVector<double>( 3,  p2[0],  p2[1],  p2[2] ) ),
  y2d ( vctDynamicVector<double>( 3,  v2[0],  v2[1],  v2[2] ) ),
  y2dd( vctDynamicVector<double>( 3, v2d[0], v2d[1], v2d[2] ) ){}

robFunctionRn::robFunctionRn( double t1, 
			      const vctDynamicVector<double>& y1, 
			      const vctDynamicVector<double>& y1d, 
			      const vctDynamicVector<double>& y1dd, 
			      double t2, 
			      const vctDynamicVector<double>& y2,
			      const vctDynamicVector<double>& y2d,
			      const vctDynamicVector<double>& y2dd ): 
  robFunction( t1, t2 ),
  y1( y1 ), y1d( y1d ), y1dd( y1dd ),
  y2( y2 ), y2d( y2d ), y2dd( y2dd ){}

