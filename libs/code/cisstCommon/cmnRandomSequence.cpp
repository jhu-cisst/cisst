/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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

void cmnRandomSequence::ExtractRandomPermutation(const size_t length, unsigned int * array)
{
    size_t i;
    for (i = 0; i < length; ++i)
        array[i] = ExtractRandomInt(0, length);
    
    unsigned int next;
    unsigned int tmp;
    for (i = 0; i < length; ++i) {
        next = ExtractRandomInt(i, length);
        tmp = array[i];
        array[i] = array[next];
        array[next] = tmp;
    }
}


void cmnRandomSequence::ExtractRandomPermutation(const size_t length, unsigned long int * array)
{
    size_t i;
    for (i = 0; i < length; ++i)
        array[i] = ExtractRandomUnsignedLong(0, length);
    
    unsigned long long next;
    unsigned long long tmp;
    for (i = 0; i < length; ++i) {
        next = ExtractRandomUnsignedLong(i, length);
        tmp = array[i];
        array[i] = array[next];
        array[next] = tmp;
    }
}
