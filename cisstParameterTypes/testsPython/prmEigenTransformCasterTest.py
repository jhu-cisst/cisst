# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

import unittest

import numpy as np

import cisstCommonPython
import cisstMultiTaskPython
import cisstParameterTypesPython as prm


class EigenTransformCasterTest(unittest.TestCase):
    def TestGeneratedIsometrySetGetAndReference(self):
        """Exercise the Isometry caster through generated prmPositionCartesianSet bindings."""
        transform = np.eye(4, dtype=np.float32)
        transform[:3, 3] = [1.25, -2.5, 3.75]
        component = prm.prmPositionCartesianSet()

        component.SetGoal(transform)
        expected = transform.astype(np.float64)
        np.testing.assert_array_equal(component.GetGoal(), expected)

        value = component.GetGoal()
        value[0, 3] = 99.0
        self.assertEqual(component.GetGoal()[0, 3], expected[0, 3])

        reference = component.Goal()
        self.assertFalse(reference.flags.writeable)
        np.testing.assert_array_equal(reference, expected)
