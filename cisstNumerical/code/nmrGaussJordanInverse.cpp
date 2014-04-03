/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):	Ofri Sadowsky
  Created on: 2005-09-23

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/
#include <cisstNumerical/nmrGaussJordanInverse.h>
#include <cisstVector/vctFixedSizeMatrix.h>

template<class _elementType, bool _rowMajorIn, bool _rowMajorOut>
void nmrGaussJordanInverse2x2(
    vctFixedSizeMatrix<_elementType, 2, 2, _rowMajorIn> A,
    bool & nonsingular,
    vctFixedSizeMatrix<_elementType, 2, 2, _rowMajorOut> & Ainv,
    const _elementType singularityTolerance)
{
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, 2, 2> result(value_type(0));
    result.Diagonal().SetAll(value_type(1));
    nonsingular = false;

    value_type scale0, scale1;

    int pivot0index = 0;
    vctFixedSizeVector<value_type, 2> absColumn0;
    absColumn0.AbsOf(A.Column(0));
    value_type pivot0value = absColumn0[0];
    if ( absColumn0[1] > pivot0value ) {
        pivot0index = 1;
        pivot0value = absColumn0[1];
    }
    if (pivot0value < singularityTolerance) {
        return;
    }
    if (pivot0index != 0) {
        A.ExchangeRows(0, pivot0index);
        result.ExchangeRows(0, pivot0index);
    }

    value_type denom0 = A.Element(0,0);
    scale1 = A.Element(1,0) / denom0;
    result.Row(1) -= scale1 * result.Row(0);
    A.Row(1) -= scale1 * A.Row(0);

    value_type denom1 = A.Element(1,1);
    if ( (-singularityTolerance < denom1) && (denom1 < singularityTolerance) ) {
        return;
    }
    scale0 = A.Element(0,1) / denom1;
    result.Row(0) -= scale0 * result.Row(1);
    A.Row(0).Y() -= scale0 * A.Row(1).Y();

    result.Row(0).Divide( A.Element(0,0) );
    result.Row(1).Divide( A.Element(1,1) );

    Ainv.Assign(result);
    nonsingular = true;
    return;
}



template<class _elementType, bool _rowMajorIn, bool _rowMajorOut>
void nmrGaussJordanInverse3x3(
    vctFixedSizeMatrix<_elementType, 3, 3, _rowMajorIn> A,
    bool & nonsingular,
    vctFixedSizeMatrix<_elementType, 3, 3, _rowMajorOut> & Ainv,
    const _elementType singularityTolerance)
{
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, 3, 3> result(value_type(0));
    result.Diagonal().SetAll(value_type(1));
    nonsingular = false;

    value_type scale0, scale1, scale2;

    int pivot0index = 0;
    vctFixedSizeVector<value_type, 3> absColumn0;
    absColumn0.AbsOf(A.Column(0));
    value_type pivot0value = absColumn0[0];
    if ( absColumn0[1] > pivot0value ) {
        pivot0index = 1;
        pivot0value = absColumn0[1];
    }
    if ( absColumn0[2] > pivot0value ) {
        pivot0index = 2;
        pivot0value = absColumn0[2];
    }
    if (pivot0value < singularityTolerance) {
        return;
    }
    if (pivot0index != 0) {
        A.ExchangeRows(0, pivot0index);
        result.ExchangeRows(0, pivot0index);
    }

    value_type denom0 = A.Element(0,0);
    scale1 = A.Element(1,0) / denom0;
    scale2 = A.Element(2,0) / denom0;
    result.Row(1) -= scale1 * result.Row(0);
    result.Row(2) -= scale2 * result.Row(0);
    A.Row(1) -= scale1 * A.Row(0);
    A.Row(2) -= scale2 * A.Row(0);

    int pivot1index = 1;
    vctFixedSizeVector<value_type, 2> absColumn1;
    absColumn1.AbsOf( A.Column(1).YZ() );
    value_type pivot1value = absColumn1[0];
    if ( absColumn1[1] > pivot1value ) {
        pivot1index = 2;
        pivot1value = absColumn1[1];
    }
    if (pivot1value < singularityTolerance) {
        return;
    }
    if (pivot1index != 1) {
        A.ExchangeRows(1, pivot1index);
        result.ExchangeRows(1, pivot1index);
    }
    value_type denom1 = A.Element(1,1);
    scale0 = A.Element(0,1) / denom1;
    scale2 = A.Element(2,1) / denom1;
    result.Row(0) -= scale0 * result.Row(1);
    result.Row(2) -= scale2 * result.Row(1);
    A.Row(0).YZ() -= scale0 * A.Row(1).YZ();
    A.Row(2).YZ() -= scale2 * A.Row(1).YZ();

    value_type denom2 = A.Element(2,2);
    if ( (-singularityTolerance < denom2) && (denom2 < singularityTolerance) ) {
        return;
    }
    scale0 = A.Element(0,2) / denom2;
    scale1 = A.Element(1,2) / denom2;
    result.Row(0) -= scale0 * result.Row(2);
    result.Row(1) -= scale1 * result.Row(2);
    A.Row(0).Z() -= scale0 * A.Row(2).Z();
    A.Row(1).Z() -= scale1 * A.Row(2).Z();

    result.Row(0).Divide( A.Element(0,0) );
    result.Row(1).Divide( A.Element(1,1) );
    result.Row(2).Divide( A.Element(2,2) );

    Ainv.Assign(result);
    nonsingular = true;
    return;
}


template<class _elementType, bool _rowMajorIn, bool _rowMajorOut>
void nmrGaussJordanInverse4x4(
    vctFixedSizeMatrix<_elementType, 4, 4, _rowMajorIn> A,
    bool & nonsingular,
    vctFixedSizeMatrix<_elementType, 4, 4, _rowMajorOut> & Ainv,
    const _elementType singularityTolerance)
{
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, 4, 4> result(value_type(0));
    result.Diagonal().SetAll(value_type(1));
    nonsingular = false;

    value_type scale0, scale1, scale2 ,scale3;

    int pivot0index = 0;
    vctFixedSizeVector<value_type, 4> absColumn0;
    absColumn0.AbsOf(A.Column(0));
    value_type pivot0value = absColumn0[0];
    if ( absColumn0[1] > pivot0value ) {
        pivot0index = 1;
        pivot0value = absColumn0[1];
    }
    if ( absColumn0[2] > pivot0value ) {
        pivot0index = 2;
        pivot0value = absColumn0[2];
    }
    if ( absColumn0[3] > pivot0value ) {
        pivot0index = 3;
        pivot0value = absColumn0[3];
    }
    if (pivot0value < singularityTolerance) {
        return;
    }
    if (pivot0index != 0) {
        A.ExchangeRows(0, pivot0index);
        result.ExchangeRows(0, pivot0index);
    }

    value_type denom0 = A.Element(0,0);
    scale1 = A.Element(1,0) / denom0;
    scale2 = A.Element(2,0) / denom0;
    scale3 = A.Element(3,0) / denom0;
    result.Row(1) -= scale1 * result.Row(0);
    result.Row(2) -= scale2 * result.Row(0);
    result.Row(3) -= scale3 * result.Row(0);
    A.Row(1) -= scale1 * A.Row(0);
    A.Row(2) -= scale2 * A.Row(0);
    A.Row(3) -= scale3 * A.Row(0);

    int pivot1index = 1;
    vctFixedSizeVector<value_type, 3> absColumn1;
    absColumn1.AbsOf( A.Column(1).YZW() );
    value_type pivot1value = absColumn1[0];
    if ( absColumn1[1] > pivot1value ) {
        pivot1index = 2;
        pivot1value = absColumn1[1];
    }
    if (absColumn1[2] > pivot1value) {
        pivot1index = 3;
        pivot1value = absColumn1[2];
    }
    if (pivot1value < singularityTolerance) {
        return;
    }
    if (pivot1index != 1) {
        A.ExchangeRows(1, pivot1index);
        result.ExchangeRows(1, pivot1index);
    }
    value_type denom1 = A.Element(1,1);
    scale0 = A.Element(0,1) / denom1;
    scale2 = A.Element(2,1) / denom1;
    scale3 = A.Element(3,1) / denom1;
    result.Row(0) -= scale0 * result.Row(1);
    result.Row(2) -= scale2 * result.Row(1);
    result.Row(3) -= scale3 * result.Row(1);
    A.Row(0).YZW() -= scale0 * A.Row(1).YZW();
    A.Row(2).YZW() -= scale2 * A.Row(1).YZW();
    A.Row(3).YZW() -= scale3 * A.Row(1).YZW();

    int pivot2index = 2;
    vctFixedSizeVector<value_type, 2> absColumn2;
    absColumn2.AbsOf( A.Column(2).ZW());
    value_type pivot2value = absColumn2[0];
    if (absColumn2[1] > pivot2value) {
        pivot2index = 3;
        pivot2value = absColumn2[1];
    }
    if (pivot2value < singularityTolerance) {
        return;
    }
    if (pivot2index != 2) {
        A.ExchangeRows(2, pivot2index);
        result.ExchangeRows(2, pivot2index);
    }
    value_type denom2 = A.Element(2,2);
    scale0 = A.Element(0,2) / denom2;
    scale1 = A.Element(1,2) / denom2;
    scale3 = A.Element(3,2) / denom2;
    result.Row(0) -= scale0 * result.Row(2);
    result.Row(1) -= scale1 * result.Row(2);
    result.Row(3) -= scale3 * result.Row(2);
    A.Row(0).ZW() -= scale0 * A.Row(2).ZW();
    A.Row(1).ZW() -= scale1 * A.Row(2).ZW();
    A.Row(3).ZW() -= scale3 * A.Row(2).ZW();

    if ( (-singularityTolerance < A.Element(3,3)) && (A.Element(3,3) < singularityTolerance) ) {
        return;
    }
    value_type denom3 = A.Element(3,3);
    scale0 = A.Element(0,3) / denom3;
    scale1 = A.Element(1,3) / denom3;
    scale2 = A.Element(2,3) / denom3;
    result.Row(0) -= scale0 * result.Row(3);
    result.Row(1) -= scale1 * result.Row(3);
    result.Row(2) -= scale2 * result.Row(3);
    A.Row(0).W() -= scale0 * A.Row(3).W();
    A.Row(1).W() -= scale1 * A.Row(3).W();
    A.Row(2).W() -= scale2 * A.Row(3).W();

    result.Row(0).Divide( A.Element(0,0) );
    result.Row(1).Divide( A.Element(1,1) );
    result.Row(2).Divide( A.Element(2,2) );
    result.Row(3).Divide( A.Element(3,3) );

    Ainv.Assign(result);
    nonsingular = true;
    return;
}

// Instantiate the function for double matrices in any storage order
template
void nmrGaussJordanInverse2x2(vctFixedSizeMatrix<double, 2, 2, VCT_ROW_MAJOR>, bool &,
                              vctFixedSizeMatrix<double, 2, 2, VCT_ROW_MAJOR> &,
                              const double);

template
void nmrGaussJordanInverse2x2(vctFixedSizeMatrix<double, 2, 2, VCT_ROW_MAJOR>, bool &,
                              vctFixedSizeMatrix<double, 2, 2, VCT_COL_MAJOR> &,
                              const double);

template
void nmrGaussJordanInverse2x2(vctFixedSizeMatrix<double, 2, 2, VCT_COL_MAJOR>, bool &,
                              vctFixedSizeMatrix<double, 2, 2, VCT_ROW_MAJOR> &,
                              const double);

template
void nmrGaussJordanInverse2x2(vctFixedSizeMatrix<double, 2, 2, VCT_COL_MAJOR>, bool &,
                              vctFixedSizeMatrix<double, 2, 2, VCT_COL_MAJOR> &,
                              const double);

template
void nmrGaussJordanInverse3x3(vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR>, bool &,
                              vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> &,
                              const double);

template
void nmrGaussJordanInverse3x3(vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR>, bool &,
                              vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> &,
                              const double);

template
void nmrGaussJordanInverse3x3(vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR>, bool &,
                              vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> &,
                              const double);

template
void nmrGaussJordanInverse3x3(vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR>, bool &,
                              vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> &,
                              const double);

template
void nmrGaussJordanInverse4x4(vctFixedSizeMatrix<double, 4, 4, VCT_ROW_MAJOR>, bool &,
                              vctFixedSizeMatrix<double, 4, 4, VCT_ROW_MAJOR> &,
                              const double);

template
void nmrGaussJordanInverse4x4(vctFixedSizeMatrix<double, 4, 4, VCT_ROW_MAJOR>, bool &,
                              vctFixedSizeMatrix<double, 4, 4, VCT_COL_MAJOR> &,
                              const double);

template
void nmrGaussJordanInverse4x4(vctFixedSizeMatrix<double, 4, 4, VCT_COL_MAJOR>, bool &,
                              vctFixedSizeMatrix<double, 4, 4, VCT_ROW_MAJOR> &,
                              const double);

template
void nmrGaussJordanInverse4x4(vctFixedSizeMatrix<double, 4, 4, VCT_COL_MAJOR>, bool &,
                              vctFixedSizeMatrix<double, 4, 4, VCT_COL_MAJOR> &,
                              const double);

// Instantiate the function for float matrices in any storage order
template
void nmrGaussJordanInverse2x2(vctFixedSizeMatrix<float, 2, 2, VCT_ROW_MAJOR>, bool &,
                              vctFixedSizeMatrix<float, 2, 2, VCT_ROW_MAJOR> &,
                              const float);

template
void nmrGaussJordanInverse2x2(vctFixedSizeMatrix<float, 2, 2, VCT_ROW_MAJOR>, bool &,
                              vctFixedSizeMatrix<float, 2, 2, VCT_COL_MAJOR> &,
                              const float);

template
void nmrGaussJordanInverse2x2(vctFixedSizeMatrix<float, 2, 2, VCT_COL_MAJOR>, bool &,
                              vctFixedSizeMatrix<float, 2, 2, VCT_ROW_MAJOR> &,
                              const float);

template
void nmrGaussJordanInverse2x2(vctFixedSizeMatrix<float, 2, 2, VCT_COL_MAJOR>, bool &,
                              vctFixedSizeMatrix<float, 2, 2, VCT_COL_MAJOR> &,
                              const float);

template
void nmrGaussJordanInverse3x3(vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR>, bool &,
                              vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> &,
                              const float);

template
void nmrGaussJordanInverse3x3(vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR>, bool &,
                              vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR> &,
                              const float);

template
void nmrGaussJordanInverse3x3(vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR>, bool &,
                              vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> &,
                              const float);

template
void nmrGaussJordanInverse3x3(vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR>, bool &,
                              vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR> &,
                              const float);

template
void nmrGaussJordanInverse4x4(vctFixedSizeMatrix<float, 4, 4, VCT_ROW_MAJOR>, bool &,
                              vctFixedSizeMatrix<float, 4, 4, VCT_ROW_MAJOR> &,
                              const float);

template
void nmrGaussJordanInverse4x4(vctFixedSizeMatrix<float, 4, 4, VCT_ROW_MAJOR>, bool &,
                              vctFixedSizeMatrix<float, 4, 4, VCT_COL_MAJOR> &,
                              const float);

template
void nmrGaussJordanInverse4x4(vctFixedSizeMatrix<float, 4, 4, VCT_COL_MAJOR>, bool &,
                              vctFixedSizeMatrix<float, 4, 4, VCT_ROW_MAJOR> &,
                              const float);

template
void nmrGaussJordanInverse4x4(vctFixedSizeMatrix<float, 4, 4, VCT_COL_MAJOR>, bool &,
                              vctFixedSizeMatrix<float, 4, 4, VCT_COL_MAJOR> &,
                              const float);

