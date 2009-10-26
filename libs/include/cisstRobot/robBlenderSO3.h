#ifndef _robBlenderSO3_h
#define _robBlenderSO3_h

#include <cisstVector/vctMatrixRotation3.h>

#include <cisstRobot/robFunction.h>

class robBlenderSO3 : public robFunction {
private:

  // Taylor's blender
  double T1, T2, ti;
  double tauA, tauB;
  vctMatrixRotation3<double,VCT_ROW_MAJOR> Rw1;
  vctAxisAngleRotation3<double> n1t1;
  vctAxisAngleRotation3<double> n2t2;
  
public:
  
  //! Build a blender for Real->SO3 mappings based on Taylor's paper
  /**
     \param T1 The length (time) of the first segment
     \param T2 The length (time) of the second segment
     \param R0 The initial orientation
     \param vctFixedSizeVector<double,1> The midpoint orientation
     \param vctFixedSizeVector<double,2> The final orientation
  */
  robBlenderSO3( double ti, 
		 double T1, double T2, 
		 const vctMatrixRotation3<double,VCT_ROW_MAJOR>& R0, 
		 const vctMatrixRotation3<double,VCT_ROW_MAJOR>& R1,
		 const vctMatrixRotation3<double,VCT_ROW_MAJOR>& R2,
		 double tA=0.01,
		 double tB=0.01 );
  
  //! Build a blender for Real->SO3 mappings based on Lloyd's paper
  /**
     \param x1 The time at the start of the blending
     \param R0 The 1st orientation at t=x1
     \param vctFixedSizeVector<double,1> The 2nd orientation at t=x1
     \param w0w The 1st velocity at t=x1
  */
  robBlenderSO3( double x1, 
		 const vctMatrixRotation3<double,VCT_ROW_MAJOR>& R0, 
		 const vctMatrixRotation3<double,VCT_ROW_MAJOR>& R1, 
		 const vctFixedSizeVector<double,3>& w0w );
  
  //! Return true if the function is defined for the given input
  robDomainAttribute IsDefinedFor( const robVariables& x ) const; 
  
  //! evaluate the mapping
  robError Evaluate( const robVariables& x, robVariables& y );  
  
};

#endif

  /*
  // RCCL blender
  vctMatrixRotation3<double,VCT_ROW_MAJOR> Rw0;           // rotation from final frame to initial frame
  vctMatrixRotation3<double,VCT_ROW_MAJOR> R10;          // rotation from final frame to initial frame
  double w0;                                   // initial angular velocity
  vctFixedSizeVector<double,3> uv;             // axis for RV
  vctAxisAngleRotation3<double> uctc;
  
  double xmin, xmax;
    
  vctMatrixRotation3<double,VCT_ROW_MAJOR> Rb1( double h );
  vctMatrixRotation3<double,VCT_ROW_MAJOR> R0b( double h );
  */
