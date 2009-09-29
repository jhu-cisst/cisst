/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctAxisAngleRotation3.i,v 1.5 2007/04/26 19:33:58 anton Exp $

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


#include "cisstVector/vctAxisAngleRotation3.h"


// instantiate and extend the end-user class
%include "cisstVector/vctAxisAngleRotation3.h"
%template(vctAxAnRot3) vctAxisAngleRotation3<double>;

%extend vctAxisAngleRotation3<double> {

    vctAxisAngleRotation3<double>(const vctDynamicConstVectorBase<vctDynamicVectorOwner<double> , double> & axis,
                                  double angle) throw(std::runtime_error) {
        vctAxisAngleRotation3<double> * result =
            new vctAxisAngleRotation3<double>(axis, angle);
        return result;
    }

    vctAxisAngleRotation3<double>(const vctDynamicConstVectorBase<vctDynamicVectorRefOwner<double> , double> & axis,
                                  double angle) throw(std::runtime_error) {
        vctAxisAngleRotation3<double> * result =
            new vctAxisAngleRotation3<double>(axis, angle);
        return result;
    }

    inline void From(const vctQuaternionRotation3Base<vctDynamicVector<double> > & quaternionRotation) {
        self->From(quaternionRotation);
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
    typedef vctAxisAngleRotation3<double> vctAxAnRot3;
%}

typedef vctAxisAngleRotation3<double> vctAxAnRot3;

%types(vctAxAnRot3 *);

