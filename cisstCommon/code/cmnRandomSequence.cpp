/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on: 2003-06-09

  (C) Copyright 2003-2013 Johns Hopkins University (JHU), All Rights
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

void cmnRandomSequence::ExtractRandomPermutation(const size_t length, size_t * array)
{
    size_t i;
    for (i = 0; i < length; ++i) {
        array[i] = i;
    }

    size_t next;
    size_t tmp;
    for (i = 0; i < length; ++i) {
        next = ExtractRandomSizeT(i, length);
        tmp = array[i];
        array[i] = array[next];
        array[next] = tmp;
    }
}
