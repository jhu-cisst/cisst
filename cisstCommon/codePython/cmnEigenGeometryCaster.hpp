/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author: Brendan Burkhart
  Date: 2026-11-21

  (C) Copyright 2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef CISST_COMMON_CMN_EIGEN_GEOMETRY_CASTER_H
#define CISST_COMMON_CMN_EIGEN_GEOMETRY_CASTER_H

#include <pybind11/eigen.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include <Eigen/Geometry>

namespace py = pybind11;

namespace pybind11 {
namespace detail {

// Generic conversion for Numpy arrays to Eigen transforms
template <typename Transform>
bool loadEigenTransform(handle src, Transform& transform, bool convert)
{
    using Scalar = typename Transform::Scalar;
    py::array array;

    // Let pybind materialize array-like inputs and cast their dtype only when allowed
    if (convert) {
        array = py::array_t<Scalar>::ensure(src);
        if (!array) { return false; }
    } else {
        if (!py::isinstance<py::array>(src)) { return false; }
        array = py::reinterpret_borrow<py::array>(src);
        if (!array.dtype().is(py::dtype::of<Scalar>())) { return false; }
    }

    auto& matrix = transform.matrix();

    // Transform coefficients must be provided as a matrix of the correct siZze
    if (array.ndim() != 2 || array.shape(0) != matrix.rows() || array.shape(1) != matrix.cols()) {
        return false;
    }

    for (ssize_t row = 0; row < matrix.rows(); ++row) {
        for (ssize_t col = 0; col < matrix.cols(); ++col) {
            matrix(row, col) = *static_cast<const Scalar*>(array.data(row, col));
        }
    }

    return true;
}

// Generic conversion for Eigen transforms to Numpy arrays
template <typename Transform>
py::handle castEigenTransform(const Transform& transform,
                              return_value_policy policy,
                              handle parent,
                              bool read_only)
{
    const auto& matrix = transform.matrix();
    using Scalar = typename Transform::Scalar;
    const auto rows = static_cast<ssize_t>(matrix.rows());
    const auto cols = static_cast<ssize_t>(matrix.cols());

    // Keep the parent alive for reference_internal views; otherwise return an owned copy
    if ((policy == return_value_policy::reference_internal) && parent && !parent.is_none()) {
        py::array result(py::dtype::of<Scalar>(),
                         {rows, cols},
                         {static_cast<ssize_t>(sizeof(Scalar) * matrix.rowStride()),
                          static_cast<ssize_t>(sizeof(Scalar) * matrix.colStride())},
                         matrix.data(), parent);
        if (read_only) {
            result.attr("setflags")(false);
        }
        return result.release();
    }

    // Otherwise, we create a new Numpy array and copy the data from the Eigen matrix.
    py::array_t<Scalar> result({rows, cols});
    auto output = result.template mutable_unchecked<2>();
    for (ssize_t row = 0; row < rows; ++row) {
        for (ssize_t col = 0; col < cols; ++col) {
            output(row, col) = matrix(row, col);
        }
    }

    return result.release();
}

template <typename Transform>
struct eigen_transform_type_caster {
    // set up conversion boilerplate
    PYBIND11_TYPE_CASTER(Transform, const_name("np.array"));

    static py::handle cast(
        Transform& transform, return_value_policy policy, handle parent
    ) {
        return castEigenTransform(transform, policy, parent, false);
    }

    static py::handle cast(
        const Transform& transform, return_value_policy policy, handle parent
    ) {
        return castEigenTransform(transform, policy, parent, true);
    }

    static py::handle cast(
        Transform&& transform, return_value_policy, handle
    ) {
        return castEigenTransform(transform, return_value_policy::copy, handle(), false);
    }

    bool load(handle src, bool convert) {
        return loadEigenTransform(src, value, convert);
    }
};

// Instantiate type casters for specific Eigen transform types

template <typename Scalar>
struct type_caster<Eigen::Transform<Scalar, 3, Eigen::Isometry>>
    : eigen_transform_type_caster<Eigen::Transform<Scalar, 3, Eigen::Isometry>> {};

template <typename Scalar>
struct type_caster<Eigen::Transform<Scalar, 3, Eigen::Affine>>
    : eigen_transform_type_caster<Eigen::Transform<Scalar, 3, Eigen::Affine>> {};

template <typename Scalar>
struct type_caster<Eigen::Transform<Scalar, 2, Eigen::Projective>>
    : eigen_transform_type_caster<Eigen::Transform<Scalar, 2, Eigen::Projective>> {};

}  // namespace detail
}  // namespace pybind11

#endif
