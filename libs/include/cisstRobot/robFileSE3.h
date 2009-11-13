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

#ifndef _robFileSE3_h
#define _robFileSE3_h

#include <fstream>
#include <string>

#include <cisstRobot/robSource.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robFileSE3 : public robSource {
 protected:
  
  std::string filename;
  std::ifstream ifs;
  
 public:
  
  robFileSE3( const std::string& filename );
  ~robFileSE3();
  
  robError Read( robVariables& var );
  robError Write( const robVariables& var );
  
  robError Open();
  robError Close();
    
};

#endif
