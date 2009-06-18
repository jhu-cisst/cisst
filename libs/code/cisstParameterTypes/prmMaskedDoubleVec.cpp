/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
Author(s):	Marcin Balicki
Created on:   2009-06-12

(C) Copyright 2009 Johns Hopkins University (JHU), All Rights
Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmMaskedDoubleVec.h>

CMN_IMPLEMENT_SERVICES(prmMaskedDoubleVec);

prmMaskedDoubleVec::~prmMaskedDoubleVec()
{
}

prmMaskedDoubleVec::prmMaskedDoubleVec(size_type size)
{
    SetSize(size);
  
}
//Shall this print both values and mask or just the masked elements?
void prmMaskedDoubleVec::ToStream(std::ostream & outputStream) const
{
    outputStream << ValueMember<<" : "<<MaskMember;         
}

/*! Set vector same size for each element*/ 
void prmMaskedDoubleVec::SetSize(size_type size){
    MaskMember.SetSize(size);
    ValueMember.SetSize(size);  
    //set some default values
    ValueMember.Zeros();
    MaskMember.Zeros();
}