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

#ifndef _robTrajectoryR1_h
#define _robTrajectoryR1_h

#include <cisstRobot/robTrajectory.h>
#include <cisstRobot/robQuintic.h>
#include <cisstRobot/robExport.h>

//! Trajectory is a r1->rn
class CISST_EXPORT robTrajectoryR1 : public robTrajectory {

 private:
  
  double  xdmax;   // the maximum velocity
  double xddmax;   // the maximum acceleration

 protected:

  double TransitionTime( const robTrajectory::Segment& si, 
			 const robTrajectory::Segment& sj );

  robTrajectory::Segment GenerateBlender( const robTrajectory::Segment& si,
					  const robTrajectory::Segment& sj );

public:
  
  robTrajectoryR1( robSpace::Basis codomain, 
		   double t, 
		   double xdmax, 
		   double xddmax );

};

#endif
