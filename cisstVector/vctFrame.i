/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2010-01-10

  (C) Copyright 2010-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#ifndef _vctFrame_i
#define _vctFrame_i

#include "cisstVector/vctFrameBase.h"

%ignore Inverse;
%ignore ApplyTo;
%ignore ApplyInverseTo;
%ignore Equal;
%ignore AlmostEqual;
%ignore Equivalent;
%ignore AlmostEquivalent;
%ignore ProductOf;
%ignore operator==;

// instantiate the templated base class
%include "cisstVector/vctFrameBase.h"
%include "cisstVector/vctTransformationTypes.h"

// ------------- types used for vctFrm3 -------------------

// to get access to the translation data member
%include "cisstVector/vctDynamicVectorTypemaps.i"
%apply vctDynamicVector         {vctFixedSizeVector< vctMatrixRotation3< double,VCT_ROW_MAJOR >::value_type,vctFrameBase< vctMatrixRotation3< double,VCT_ROW_MAJOR > >::DIMENSION >};
%apply vctDynamicVector &       {vctFixedSizeVector< vctMatrixRotation3< double,VCT_ROW_MAJOR >::value_type,vctFrameBase< vctMatrixRotation3< double,VCT_ROW_MAJOR > >::DIMENSION > &};
%apply const vctDynamicVector & {const vctFixedSizeVector< vctMatrixRotation3< double,VCT_ROW_MAJOR >::value_type,vctFrameBase< vctMatrixRotation3< double,VCT_ROW_MAJOR > >::DIMENSION > &};

// to get access to the rotation data member
%include "cisstVector/vctDynamicMatrixTypemaps.i"
%apply vctDynamicMatrix         {vctFrameBase< vctMatrixRotation3< double,VCT_ROW_MAJOR > >::RotationType};
%apply vctDynamicMatrix &       {vctFrameBase< vctMatrixRotation3< double,VCT_ROW_MAJOR > >::RotationType &};
%apply const vctDynamicMatrix & {const vctFrameBase< vctMatrixRotation3< double,VCT_ROW_MAJOR > >::RotationType &};


%template(vctFrm3) vctFrameBase<vctRot3 >;

// type declarations for SWIG
%{
    typedef vctFrameBase<vctRot3> vctFrm3;
%}

typedef vctFrameBase<vctRot3 > vctFrm3;

%types(vctFrm3 *);


// ------------- types used for vctFrm4x4 -------------------
%apply vctDynamicMatrix         {vctFrame4x4<double>};
%apply vctDynamicMatrix &       {vctFrame4x4<double> &};
%apply const vctDynamicMatrix & {const vctFrame4x4<double> &};
%apply vctDynamicMatrix         {vctFrame4x4<double, VCT_ROW_MAJOR >};
%apply vctDynamicMatrix &       {vctFrame4x4<double, VCT_ROW_MAJOR > &};
%apply const vctDynamicMatrix & {const vctFrame4x4<double, VCT_ROW_MAJOR > &};

// ------------- Euler angle rotations -------------------
%include "cisstVector/vctEulerRotation3.h"

// Extend the class to add From(vctDynamicMatrix<double>), which due to the typemaps
// will accept numpy arrays.
%extend vctEulerRotation3 {
    ThisType & From(const vctDynamicMatrix<double> &matrixRot) throw(std::runtime_error) {
        vctMatRot3 temp;
        // Should be a better way to do this
        temp.From(matrixRot[0][0], matrixRot[0][1], matrixRot[0][2],
                  matrixRot[1][0], matrixRot[1][1], matrixRot[1][2],
                  matrixRot[2][0], matrixRot[2][1], matrixRot[2][2]);
        return $self->From(temp);
    }
    ThisType & FromNormalized(const vctDynamicMatrix<double> &matrixRot) {
        vctMatRot3 temp;
        // Should be a better way to do this
        temp.FromNormalized(matrixRot[0][0], matrixRot[0][1], matrixRot[0][2],
                            matrixRot[1][0], matrixRot[1][1], matrixRot[1][2],
                            matrixRot[2][0], matrixRot[2][1], matrixRot[2][2]);
        return $self->FromNormalized(temp);
    }
    ThisType & FromRaw(const vctDynamicMatrix<double> &matrixRot) {
        vctMatRot3 temp;
        // Should be a better way to do this
        temp.FromRaw(matrixRot[0][0], matrixRot[0][1], matrixRot[0][2],
                     matrixRot[1][0], matrixRot[1][1], matrixRot[1][2],
                     matrixRot[2][0], matrixRot[2][1], matrixRot[2][2]);
        return $self->FromRaw(temp);
    }
}

%template(vctEulerZYZRotation3) vctEulerRotation3<vctEulerRotation3Order::ZYZ >;
%template(vctEulerZYXRotation3) vctEulerRotation3<vctEulerRotation3Order::ZYX >;

#endif // _vctFrame_i
