/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Rajesh Kumar, Anton Deguet
  Created on:   2008-03-03

  (C) Copyright 2007-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstParameterTypes/prmTransformationFixed.h>


/*! Destructor.  The frame is also removed from the frame manager.
  The children of this frame will inherit its parent when the frame is
  removed.
*/
prmTransformationFixed::~prmTransformationFixed() 
{
    //nothing for now. 
}

