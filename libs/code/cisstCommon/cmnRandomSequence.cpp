/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnRandomSequence.cpp 20 2009-01-08 16:09:57Z adeguet1 $
  
  Author(s):	Ofri Sadowsky
  Created on:	2003-06-09

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnRandomSequence.h>

const cmnRandomSequence::SeedType cmnRandomSequence::DefaultSeed = 1;
const cmnRandomSequence::ElementaryRandomNumber cmnRandomSequence::LowerRandomBound = 0;
const cmnRandomSequence::ElementaryRandomNumber cmnRandomSequence::UpperRandomBound = RAND_MAX;

cmnRandomSequence cmnRandomSequence::RandomInstance;

