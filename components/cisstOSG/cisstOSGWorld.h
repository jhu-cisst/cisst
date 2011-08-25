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

#ifndef _cisstOSGWorld_h
#define _cisstOSGWorld_h

#include <osg/Group>
#include <cisstOSG/cisstOSGExport.h>

class CISST_EXPORT cisstOSGWorld : public osg::Group {
 public:
  cisstOSGWorld();
  ~cisstOSGWorld(){}
};

#endif
