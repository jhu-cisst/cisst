/*
  Author(s): Simon Leonard
  Created on: Dec 02 2009

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsODEBH_h
#define _mtsODEBH_h

#include <cisstODE/mtsODEManipulatorTask.h>
#include <cisstODE/cisstODEBH.h>
#include <cisstODE/cisstODEExport.h>

class CISST_EXPORT mtsODEBH : public mtsODEManipulatorTask {

 public:

  mtsODEBH( const std::string& name,
	    double period,
	    osaCPUMask cpumask,
	    int priority,
	    const std::string& palmmodel,
	    const std::string& metacarpalmodel,
	    const std::string& proximalmodel,
	    const std::string& intermediatemodel,
	    cisstODEWorld* world,
	    const vctFrame4x4<double>& Rtw0,
	    const std::string& f1f2filename,
	    const std::string& f3filename ) :
    mtsODEManipulatorTask( name, period, 
			   new cisstODEBH( palmmodel,
					   metacarpalmodel,
					   proximalmodel,
					   intermediatemodel,
					   world,
					   Rtw0,
					   f1f2filename,
					   f3filename ),
			   cpumask, priority ){}
  
  // main constructor
  mtsODEBH( const std::string& name,
	    double period,
	    osaCPUMask cpumask,
	    int priority,
	    const std::string& palmmodel,
	    const std::string& metacarpalmodel,
	    const std::string& proximalmodel,
	    const std::string& intermediatemodel,
	    cisstODEWorld* world,
	    const vctFrm3& Rtw0,
	    const std::string& f1f2filename,
	    const std::string& f3filename ) :
    mtsODEManipulatorTask( name, period, 
			   new cisstODEBH( palmmodel,
					   metacarpalmodel,
					   proximalmodel,
					   intermediatemodel,
					   world,
					   Rtw0,
					   f1f2filename,
					   f3filename ),
			   cpumask, priority ){}

};

#endif





