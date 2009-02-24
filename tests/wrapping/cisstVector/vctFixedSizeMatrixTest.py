# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
# $Id$
#

# Author: Anton Deguet
# Date: 2007-07-19
#
# (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
# Reserved.

# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

import unittest

import cisstCommonPython

from cisstVectorPython import vctDoubleMat
from cisstVectorPython import vctIntMat

from vctFixedSizeMatrixProxyTestPython import vctFixedSizeMatrixProxyTest

# "d" is used to prefix double matrices and "i" for int matrices The
# letter "l" is added for local values

class FixedSizeMatrixTest(unittest.TestCase):
    def setUp(self):
        """Call before every test case."""
        self.proxy = vctFixedSizeMatrixProxyTest()
        
    def tearDown(self):
        """Call after every test case."""




    def IndividualTests(self, type, rows, cols):

        # Test size and default values
        classPrefix = "self.proxy." + type + str(rows) + "x" + str(cols)
        matrix = eval(classPrefix + "Ref()")
        self.assertEqual(matrix.rows(), rows)
        self.assertEqual(matrix.cols(), cols)
        for r in range(matrix.rows()):
            for c in range(matrix.cols()):
                self.assertEqual(matrix[r][c], r * 100 + c)

        # Add 5 to all elements and check the sum
        sumFromC = eval(classPrefix +  "Add(5)")
        matrix = eval(classPrefix + "Ref()")
        sum = 0;
        for r in range(matrix.rows()):
            for c in range(matrix.cols()):
                sum += matrix[r][c]
        matrix -= 5
        self.assertEqual(sum, sumFromC)

        # Add 5 to all elements on Python copy and then 5 to C++ copy
        copy = eval(classPrefix + "Copy()")
        reference = eval(classPrefix + "Ref()")
        self.assertEqual(reference.size(), copy.size())
        copy += 5
        for r in range(reference.rows()):
            for c in range(reference.cols()):
                self.assertEqual(reference[r][c] + 5, copy[r][c])
        reference += 5
        for r in range(reference.rows()):
            for c in range(reference.cols()):
                self.assertEqual(reference[r][c], copy[r][c])
        reference -= 5

        # Create a new matrix with different values, set by reference and check sum
        matrix = eval("vct" + type + "Mat(rows, cols)")
        sum = 0
        for r in range(matrix.rows()):
            for c in range(matrix.cols()):
                matrix[r][c] = 100 * r + 10 * c
                sum += matrix[r][c]
        self.assertEqual(matrix.SumOfElements(), sum)
        eval(classPrefix + "SetRefAdd10(matrix)")
        self.assertEqual(eval(classPrefix + "SumOfElements()"), sum)
        # matrix should have been modified, check its new sum
        self.assertEqual(matrix.SumOfElements(), sum + matrix.rows() * matrix.cols() * 10)

        # Check that an exception is thrown is sizes don't match
        matrix = eval("vct" + type + "Mat(rows + 2, cols)")
        exception = 0
        try:
            eval(classPrefix + "SetRefAdd10(matrix)")
        except RuntimeError, e:
            exception = 1
        self.assertEqual(exception, 1)

        # Create a new matrix with different values, set by copy and check sum
        matrix = eval("vct" + type + "Mat(rows, cols)")
        sum = 0
        for r in range(matrix.rows()):
            for c in range(matrix.cols()):
                matrix[r][c] = 200 * r + 20 * c
                sum += matrix[r][c]
        self.assertEqual(matrix.SumOfElements(), sum)
        eval(classPrefix + "SetCopy(matrix)")
        self.assertEqual(eval(classPrefix + "SumOfElements()"), sum)

        # Now, modify the C++ and make sure Python matrix is unchanged
        eval(classPrefix + "Add(5)")
        self.assertEqual(matrix.SumOfElements(), sum)

        # Check that an exception is thrown is sizes don't match
        matrix = eval("vct" + type + "Mat(rows + 2, cols)")
        exception = 0
        try:
            eval(classPrefix + "SetCopy(matrix)")
        except RuntimeError, e:
            exception = 1
        self.assertEqual(exception, 1)
            


    def TestDouble2x2(self):
        self.IndividualTests("Double", 2, 2)

    def TestDouble3x3(self):
        self.IndividualTests("Double", 3, 3)
        
    def TestDouble4x4(self):
        self.IndividualTests("Double", 4, 4)


    def TestInt2x2(self):
        self.IndividualTests("Int", 2, 2)

    def TestInt3x3(self):
        self.IndividualTests("Int", 3, 3)

    def TestInt4x4(self):
        self.IndividualTests("Int", 4, 4)

