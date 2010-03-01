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

#ifndef _robTrajectory_h
#define _robTrajectory_h

#include <list>

#include <cisstRobot/robFunction.h>
#include <cisstRobot/robExport.h>

//! A trajectory interface
/**
   robTrajectory is an abstract function defining a common interface for time-
   based trajectories. This class must be overloaded and specialized to a given 
   codomain. As mentioned, the domain of a trajectory is time and the codomain
   must be defined by the overloaded class. For exmaple, robTrajectoryRn defines
   time-based joint trajectories and robTrajectorySE3 defines time-based 
   Cartesian trajectories. The main service provided by robTrajectory is to 
   manages a list of motion segments that can be evaluated. Each motion segment
   must be consistend with the codomain of the trajectory. For example, if you 
   define a trajectory for Q1, then every motion segment must contain a function
   with Q1 as its codomain. Every class derived from robTrajectory must implement
   a method to generate a motion segment, a method to determine the transition
   time between two motion segments and a blending method.
*/
class CISST_EXPORT robTrajectory : public robFunction {

 protected:

  //! Define a motion segment.
  struct Segment{
    double ti;                        // Initial time of the segment
    double tf;                        // Final time of the segment
    robFunction* function;            // The function used by the segment

    Segment( double ti, double tf, robFunction* function ) :
      ti(ti), tf(tf), function(function) {}
  };
  
 private:

  double t; // the time of the last evaluation of the trajectory

  //! The trajectory segments
  std::list< robTrajectory::Segment > segments;

 protected:

  virtual 
    robTrajectory::Segment  GenerateBlender(const robTrajectory::Segment& si,
					    const robTrajectory::Segment& sj)=0;
  
 public:
  
  //! Default constructor
  robTrajectory( robSpace::Basis codomain, double t );
  
  //
  robFunction::Errno EvaluateLastSegment( robVariable& input,
					  robVariable& output ) const;

  //! Insert a function
  /**
     This inserts a new function in the piecewise function. The function is
     inserted at the begining of a queue and thus its domain has higher priority
     than the functions after.
     \param ti The initial time for which the function is defined
     \param tf The final time for which the function is defined
     \param function The new function to be added
  */
  robFunction::Errno Insert( robFunction* function, double ti, double tf );

  //! Return the context of the function
  /**
     Call this method to find if the trajectory has a function defined for an
     input.
     \param[in] input The input variable
     \return CDEFINED if the function is defined for the input. CUNDEFINED if 
             the function is undefined.
  */
  robFunction::Context GetContext( const robVariable& input ) const ;
    
  //! Evaluate the function for the given input
  /**
     This querries all the functions starting by the most recently inserted.
     If a function is defined for the input, it is evaluated. The function also
     perform blending if it determines that an other function is ramping up
     or ramping down. Finally, this method performs cleaning of the functions.
     Functions that are expired or that have a lower priority are removed.
     \param[in] input The input to the function. The input domain must be 
                      robSpace::TIME
     \param[out] output The output of the function. The domain of the output
                        will be set to the codomain of the function
     \return robFunction::ESUCCESS if no error occured. robFunction::EUNDEFINED
             if the function is not defined for the input.
  */
  robFunction::Errno Evaluate( const robVariable& input, robVariable& output );
  
};

#endif
