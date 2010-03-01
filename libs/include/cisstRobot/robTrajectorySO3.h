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

#ifndef _robTrajectorySO3_h
#define _robTrajectorySO3_h

#include <cisstRobot/robTrajectory.h>
#include <cisstRobot/robExport.h>

//! Trajectory is a r1->SO3
class CISST_EXPORT robTrajectorySO3 : public robTrajectory {

 private:

  double wmax;                             // magnitude or max angular vel.
  double wdmax;                            // magnitude or max angular acc.
  
 protected:

  double TransitionTime( const robTrajectory::Segment& si, 
			 const robTrajectory::Segment& sj );

  robTrajectory::Segment GenerateBlender( const robTrajectory::Segment& si,
					  const robTrajectory::Segment& sj );

public:
  
  robTrajectorySO3( double t, double wmax, double wdmax );

};

#endif
