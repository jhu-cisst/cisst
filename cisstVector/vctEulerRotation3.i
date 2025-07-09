/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  (C) Copyright 2010-2025 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

// ------------- Euler angle rotations -------------------

#ifndef _vctEulerRotation3_i
#define _vctEulerRotation3_i

%ignore Inverse;
%ignore Equal;
%ignore AlmostEqual;
%ignore Equivalent;
%ignore AlmostEquivalent;
%ignore operator==;

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

#endif // _vctEulerRotation3_i
