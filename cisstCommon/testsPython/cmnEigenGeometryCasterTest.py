# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

# Author: Brendan Burkhart
# Date: 2026-11-21
#
# (C) Copyright 2026 Johns Hopkins University (JHU), All Rights Reserved.

# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

import unittest

import numpy as np

import cisstCommonPythonTestPython as caster


class EigenGeometryCasterTest(unittest.TestCase):
    def TestRoundTripAcrossTransformTypesAndScalars(self):
        """Convert array-like inputs and preserve each transform's scalar type."""
        cases = (
            (caster.round_trip_isometry3f, 4, np.float32),
            (caster.round_trip_isometry3d, 4, np.float64),
            (caster.round_trip_affine3f, 4, np.float32),
            (caster.round_trip_affine3d, 4, np.float64),
            (caster.round_trip_projective2f, 3, np.float32),
            (caster.round_trip_projective2d, 3, np.float64),
        )
        for round_trip, size, dtype in cases:
            with self.subTest(dtype=dtype, size=size):
                values = np.arange(size * size, dtype=np.float64).reshape(size, size)
                result = round_trip(values.tolist())
                np.testing.assert_array_equal(result, values.astype(dtype))
                self.assertEqual(result.dtype, dtype)

    def TestProjectiveAcceptsStridedArray(self):
        """Copy values correctly from a non-contiguous NumPy view."""
        values = np.arange(36, dtype=np.float64).reshape(6, 6)[::2, ::2]
        self.assertFalse(values.flags.c_contiguous)
        np.testing.assert_array_equal(caster.round_trip_projective2d(values), values)

    def TestNoConvertRequiresExactArrayDtype(self):
        """Reject array-like and dtype conversions when pybind disables them."""
        values = np.eye(4, dtype=np.float64)
        np.testing.assert_array_equal(caster.round_trip_isometry3d_noconvert(values), values)
        with self.assertRaises(TypeError):
            caster.round_trip_isometry3d_noconvert(values.tolist())
        with self.assertRaises(TypeError):
            caster.round_trip_isometry3d_noconvert(values.astype(np.float32))

    def TestRejectsWrongShape(self):
        """Reject matrices whose dimensions do not match the transform."""
        with self.assertRaises(TypeError):
            caster.round_trip_isometry3d(np.eye(3))
