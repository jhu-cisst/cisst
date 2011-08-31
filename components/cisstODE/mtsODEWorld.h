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

#ifndef _mtsODEWorld_h
#define _mtsODEWorld_h

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstODE/cisstODEWorld.h>

class CISST_EXPORT mtsODEWorld : 
  public mtsTaskPeriodic,
  public cisstODEWorld {

 public:

  //! Create a new world
  /**
     Create a new ODE world. This initializes the ODE engine and create a new
     world and a new top level space. It also sets simulation parameters such as
     error reduction parameter (ERP) and constraint force mixing (CFM)
     \param gravity A gravity vector. The defalt value is 
                    \$\begin{bmatrix} 0 & 0 & -9.81 \end{bmatrix}\$.
  */
  mtsODEWorld( const std::string& name,
	       double period,
	       const vctFixedSizeVector<double,3>& gravity = GRAVITY ) :
    mtsTaskPeriodic( name, period, true ),
    cisstODEWorld( period, gravity ){}
  
  //! Destroy the world!
  ~mtsODEWorld(){}

  void Configure( const std::string& CMN_UNUSED( argv ) ){}
  
  void Startup(){}
  void Run(){
    ProcessQueuedCommands();
    Step();
  }
  void Cleanup(){}

};

#endif

