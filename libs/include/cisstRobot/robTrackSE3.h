/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robTrackSE3_h
#define _robTrackSE3_h

#include <cisstRobot/robFunction.h>

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctMatrixRotation3.h>
#include <cisstVector/vctFrame4x4.h>

#include <cisstRobot/robExport.h>

class CISST_EXPORT robTrackSE3 : public robFunction{

 private:

  robFunction *txblender;
  robFunction *tyblender;
  robFunction *tzblender;
  robFunction *so3blender;
    
  vctMatrixRotation3<double,VCT_ROW_MAJOR> Rw0;  // the old orientation
  vctMatrixRotation3<double,VCT_ROW_MAJOR> Rw1;  // the current orientation
  vctMatrixRotation3<double,VCT_ROW_MAJOR> Rw2;  // the future orientation
  vctFixedSizeVector<double,3> tw0;              // the old position
  vctFixedSizeVector<double,3> tw1;              // the current position
  vctFixedSizeVector<double,3> tw2;              // the future position
  vctFixedSizeVector<double,3> v1, v1d, v2, v2d; // linear velocity/acceleration
  vctFixedSizeVector<double,3> w1, w1d, w2, w2d; // angular velocity/acceleration
  
  double t0, t1, t2;// old, current and future time;
  
  double vmax;
  double vdmax;
  double wmax;
  double wdmax;
  
  //uint32_t numwp;
  unsigned long int numwp;
  
  robError Evaluate(const robVariables& input);
  
 public:
  
  robTrackSE3(double vmax=-1, double wmax=-1, double vdmax=-1, double wdmax=-1);
  
  robDomainAttribute IsDefinedFor( const robVariables& input ) const;
  
  robError Evaluate( const robVariables& input, robVariables& output );
  
};

#endif

