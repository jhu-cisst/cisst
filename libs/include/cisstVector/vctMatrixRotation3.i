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


#include "cisstVector/vctMatrixRotation3Base.h"



// macro to extend using different types of vectors
%define VCT_MATRIX_ROTATION_BASE_EXTEND_VECTOR_1(owner, subOwner)
%extend vctMatrixRotation3Base<owner > {

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
%define VCT_MATRIX_ROTATION_BASE_EXTEND_VECTOR_2(owner, subOwner1, subOwner2)    
%extend vctMatrixRotation3Base<owner > {

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
%include "cisstVector/vctMatrixRotation3ConstBase.h"
%include "cisstVector/vctMatrixRotation3Base.h"
%template(vctConstMatRot3) vctMatrixRotation3ConstBase<vctDynamicMatrix<double> >;
%template(vctMatRot3) vctMatrixRotation3Base<vctDynamicMatrix<double> >;

// type declarations for SWIG
%{
    typedef vctMatrixRotation3Base<vctDynamicMatrix<double> > vctMatRot3;
%}

typedef vctMatrixRotation3Base<vctDynamicMatrix<double> > vctMatRot3;

%types(vctMatRot3 *);



VCT_MATRIX_ROTATION_BASE_EXTEND_VECTOR_1(vctDynamicMatrix<double>,
                                         vctDynamicVectorOwner<double>);
VCT_MATRIX_ROTATION_BASE_EXTEND_VECTOR_1(vctDynamicMatrix<double>,
                                         vctDynamicVectorRefOwner<double>);

VCT_MATRIX_ROTATION_BASE_EXTEND_VECTOR_2(vctDynamicMatrix<double>,
                                         vctDynamicVectorOwner<double>, vctDynamicVectorOwner<double>);
VCT_MATRIX_ROTATION_BASE_EXTEND_VECTOR_2(vctDynamicMatrix<double>,
                                         vctDynamicVectorOwner<double>, vctDynamicVectorRefOwner<double>);
VCT_MATRIX_ROTATION_BASE_EXTEND_VECTOR_2(vctDynamicMatrix<double>,
                                         vctDynamicVectorRefOwner<double>, vctDynamicVectorOwner<double>);
VCT_MATRIX_ROTATION_BASE_EXTEND_VECTOR_2(vctDynamicMatrix<double>,
                                         vctDynamicVectorRefOwner<double>, vctDynamicVectorRefOwner<double>);


%extend vctMatrixRotation3Base<vctDynamicMatrix<double> > {
    
    vctMatrixRotation3Base<vctDynamicMatrix<double> >(const vctMatrixRotation3Base<vctDynamicMatrix<double> > & matrixRotation)
        throw(std::runtime_error)
    {
        vctMatrixRotation3Base<vctDynamicMatrix<double> > * result =
            new vctMatrixRotation3Base<vctDynamicMatrix<double> >(matrixRotation);
        return result;
    }

    vctMatrixRotation3Base<vctDynamicMatrix<double> >(const vctMatrixRotation3Base<vctDynamicMatrix<double> > & matrixRotation,
                                                      bool normalizeInput)
        throw(std::runtime_error)
    {
        vctMatrixRotation3Base<vctDynamicMatrix<double> > * result =
            new vctMatrixRotation3Base<vctDynamicMatrix<double> >(matrixRotation, normalizeInput);
        return result;
    }
    
    vctMatrixRotation3Base<vctDynamicMatrix<double> >(const vctQuaternionRotation3Base<vctDynamicVector<double> > & quaternionRotation)
    {
        vctMatrixRotation3Base<vctDynamicMatrix<double> > * result =
            new vctMatrixRotation3Base<vctDynamicMatrix<double> >(quaternionRotation);
        return result;
    }

    vctMatrixRotation3Base<vctDynamicMatrix<double> >(const vctQuaternionRotation3Base<vctDynamicVector<double> > & quaternionRotation,
                                                      bool normalizeInput)
    {
        vctMatrixRotation3Base<vctDynamicMatrix<double> > * result =
            new vctMatrixRotation3Base<vctDynamicMatrix<double> >(quaternionRotation, normalizeInput);
        return result;
    }

    inline void Random(void) {
        vctRandom(*self);
    }

    inline void From(const vctRodriguezRotation3Base<vctDynamicVector<double> > & rodriguezRotation) {
        self->From(rodriguezRotation);
    }

    inline void From(const vctQuaternionRotation3Base<vctDynamicVector<double> > & quaternionRotation) {
        self->From(quaternionRotation);
    }


    inline vctMatrixRotation3Base<vctDynamicMatrix<double> >
        ApplyTo(const vctMatrixRotation3Base<vctDynamicMatrix<double> > & input)
        throw (std::runtime_error) 
    {
        return self->ApplyTo(input);
    }
    
    inline vctMatrixRotation3Base<vctDynamicMatrix<double> >
        ApplyInverseTo(const vctMatrixRotation3Base<vctDynamicMatrix<double> > & input)
        throw (std::runtime_error) 
    {
        return self->ApplyInverseTo(input);
    }

    inline void
        ApplyTo(const vctMatrixRotation3Base<vctDynamicMatrix<double> > & input,
                vctMatrixRotation3Base<vctDynamicMatrix<double> > & output)
        throw (std::runtime_error) 
    {
        self->ApplyTo(input, output);
    }
    
    inline void
        ApplyInverseTo(const vctMatrixRotation3Base<vctDynamicMatrix<double> > & input,
                       vctMatrixRotation3Base<vctDynamicMatrix<double> > & output)
        throw (std::runtime_error) 
    {
        self->ApplyInverseTo(input, output);
    }

}

