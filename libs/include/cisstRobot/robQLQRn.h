
#ifndef _robQLQRn_h
#define _robQLQRn_h

#include <cisstRobot/robLinearRn.h>
#include <cisstRobot/robQuintic.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robQLQRn : public robFunctionRn {

 private:

  vctDynamicVector<double> qdmax;
  vctDynamicVector<double> qddmax;
  
  //double toffset;
  double tauin;
  double tauout;

  robQuintic*  transitionin;
  robLinearRn* cruise;
  robQuintic*  transitionout;

  double EvaluateTransitionTime( const vctDynamicVector<double>& q1d,
				 const vctDynamicVector<double>& q2d );
    
  void CreateInputTransition( double tau,
			      const vctDynamicVector<double>& q1, 
			      const vctDynamicVector<double>& q1d, 
			      const vctDynamicVector<double>& q1dd );

  void CreateOutputTransition( double tau,
			       const vctDynamicVector<double>& q2, 
			       const vctDynamicVector<double>& q2d, 
			       const vctDynamicVector<double>& q2dd );


 public:

  robQLQRn( const vctDynamicVector<double>& q1,
	    const vctDynamicVector<double>& q2,
	    const vctDynamicVector<double>& qd,
	    const vctDynamicVector<double>& qdd,
	    double t1 = 0.0,
	    bool forceblend = false );

  void Evaluate( double t, 
		 vctDynamicVector<double>& q,
		 vctDynamicVector<double>& qd,
		 vctDynamicVector<double>& qdd );

  void Blend( robFunction* function,
	      const vctDynamicVector<double>& qdmax, 
	      const vctDynamicVector<double>& qddmax );

  void Blend( robFunction* function, double vmax, double vdmax );

};

#endif
