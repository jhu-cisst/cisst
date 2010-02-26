/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2005-08-19

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include "cisstVector/vctQuaternionRotation3Base.h"


// macro to extend using different types of vectors
%define VCT_QUATERNION_ROTATION_BASE_EXTEND_VECTOR_1(owner, subOwner)
%extend vctQuaternionRotation3Base<owner > {

    inline vctDouble3
        ApplyTo(const vctDynamicConstVectorBase<subOwner, value_type>& input)
        throw (std::runtime_error) 
        {
            return self->ApplyTo(input);
        }

    inline vctDouble3
        ApplyInverseTo(const vctDynamicConstVectorBase<subOwner, value_type>& input)
        throw (std::runtime_error) 
        {
            return self->ApplyInverseTo(input);
        }

    inline vctDouble3
        __mul__(const vctDynamicConstVectorBase<subOwner, value_type>& input)
        throw (std::runtime_error) 
        {
            return self->ApplyTo(input);
        }
}
%enddef



// macro to extend using different types of vectors
%define VCT_QUATERNION_ROTATION_BASE_EXTEND_VECTOR_2(owner, subOwner1, subOwner2)    
%extend vctQuaternionRotation3Base<owner > {

    inline void ApplyTo(const vctDynamicConstVectorBase<subOwner1, value_type>& input,
                        vctDynamicVectorBase<subOwner2, value_type> & output)
        throw (std::runtime_error) 
        {
            self->ApplyTo(input, output);
        }

    inline void ApplyInverseTo(const vctDynamicConstVectorBase<subOwner1, value_type>& input,
                               vctDynamicVectorBase<subOwner2, value_type> & output)
        throw (std::runtime_error) 
        {
            self->ApplyInverseTo(input, output);
        }
}
%enddef


// instantiate the templated base class
%include "cisstVector/vctQuaternionRotation3Base.h"
%template(vctQuatRot3) vctQuaternionRotation3Base<vctDynamicVector<double> >;

VCT_QUATERNION_ROTATION_BASE_EXTEND_VECTOR_1(vctDynamicVector<double>,
                                             vctDynamicVectorOwner<double>);
VCT_QUATERNION_ROTATION_BASE_EXTEND_VECTOR_1(vctDynamicVector<double>,
                                             vctDynamicVectorOwner<double>);

VCT_QUATERNION_ROTATION_BASE_EXTEND_VECTOR_2(vctDynamicVector<double>,
                                             vctDynamicVectorOwner<double>, vctDynamicVectorOwner<double>);
VCT_QUATERNION_ROTATION_BASE_EXTEND_VECTOR_2(vctDynamicVector<double>,
                                             vctDynamicVectorOwner<double>, vctDynamicVectorRefOwner<double>);
VCT_QUATERNION_ROTATION_BASE_EXTEND_VECTOR_2(vctDynamicVector<double>,
                                             vctDynamicVectorRefOwner<double>, vctDynamicVectorOwner<double>);
VCT_QUATERNION_ROTATION_BASE_EXTEND_VECTOR_2(vctDynamicVector<double>,
                                             vctDynamicVectorRefOwner<double>, vctDynamicVectorRefOwner<double>);



%extend vctQuaternionRotation3Base<vctDynamicVector<double> > {

    vctQuaternionRotation3Base<vctDynamicVector<double> >(const vctMatrixRotation3Base<vctDynamicMatrix<double> > & matrixRotation)
        throw(std::runtime_error)
    {
        vctQuaternionRotation3Base<vctDynamicVector<double> > * result =
            new vctQuaternionRotation3Base<vctDynamicVector<double> >(matrixRotation);
        return result;
    }
    
    vctQuaternionRotation3Base<vctDynamicVector<double> >(const vctMatrixRotation3Base<vctDynamicMatrix<double> > & matrixRotation,
                                                          bool normalizeInput)
    {
        vctQuaternionRotation3Base<vctDynamicVector<double> > * result =
            new vctQuaternionRotation3Base<vctDynamicVector<double> >(matrixRotation, normalizeInput);
        return result;
    }

    inline void Random(void) {
        vctRandom(*self);
    }

    inline void From(const vctAxisAngleRotation3<double> & axisAngleRotation) {
        self->From(axisAngleRotation);
    }
    
    inline void From(const vctRodriguezRotation3Base<vctDynamicVector<double> > & rodriguezRotation) {
        self->From(rodriguezRotation);
    }

    inline void From(const vctMatrixRotation3Base<vctDynamicMatrix<double> > & matrixRotation) {
        self->From(matrixRotation);
    }
}


// type declarations for SWIG
%{
    typedef vctQuaternionRotation3Base<vctDynamicVector<double> > vctQuatRot3;
%}

typedef vctQuaternionRotation3Base<vctDynamicVector<double> > vctQuatRot3;

%types(vctQuatRot3 *);

