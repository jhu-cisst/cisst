# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
# $Id: vctDynamicMatrixTest.py,v 1.13 2007/04/26 20:12:05 anton Exp $
#

# Author: Anton Deguet
# Date: 2005-07-14
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

from cisstCommonPython import cmnTypeTraitsDouble_Tolerance

from cisstVectorPython import vctDoubleMat
from cisstVectorPython import vctDoubleVec
from cisstVectorPython import vctIntMat
from cisstVectorPython import vctIntVec


# "d" is used to prefix double matrices and "i" for int matrices The
# letter "l" is added for local values

class DynamicMatrixTest(unittest.TestCase):

    def setUp(self):
        """Call before every test case."""
        self.dm1 = vctDoubleMat(12, 17)
        self.dm1.Random(-10, 10)
        self.dm2 = vctDoubleMat(12, 17)
        self.dm2.Random(-10, 10)
        self.im1 = vctIntMat(23, 11)
        self.im1.Random(-10, 10)
        self.im2 = vctIntMat(23, 11)
        self.im2.Random(-10, 10)

        
    def tearDown(self):
        """Call after every test case."""


    def assertAlmostEqual(self, value1, value2, tolerance = cmnTypeTraitsDouble_Tolerance()):
        """Assert that two values are equal within given tolerance."""
        self.failUnless((abs(value1 - value2)) <= tolerance)


    def TestConstructorSize(self):
        """Test size."""
        dv = vctDoubleMat(5, 7)
        self.assertEqual(dv.rows(), 5)
        self.assertEqual(dv.cols(), 7)
        iv = vctIntMat(5, 7)
        self.assertEqual(iv.rows(), 5)
        self.assertEqual(iv.cols(), 7)


    def TestMoMiMi(self):
        """Test the MoMiMi based methods"""
        # test the sum
        dlm1 = self.dm1 + self.dm2
        self.assertAlmostEqual(dlm1.SumOfElements(), self.dm1.SumOfElements() + self.dm2.SumOfElements())
        # test the difference
        dlm2 = dlm1 - self.dm2
        self.failUnless(dlm2.AlmostEqual(self.dm1))

        # test the sum
        dlm1.SumOf(self.dm1, self.dm2)
        self.assertAlmostEqual(dlm1.SumOfElements(), self.dm1.SumOfElements() + self.dm2.SumOfElements())
        # test the difference
        dlm2.DifferenceOf(dlm1, self.dm2)
        self.failUnless(dlm2.AlmostEqual(self.dm1))


    def TestMoMiSi(self):
        """Test the MoMiSi based methods"""
        dlm1 = self.dm1 + 5
        self.assertAlmostEqual(dlm1.SumOfElements(), self.dm1.SumOfElements() + 5 * dlm1.size())

        dlm1.SumOf(self.dm1, 5)
        self.assertAlmostEqual(dlm1.SumOfElements(), self.dm1.SumOfElements() + 5 * dlm1.size())

        dlm1 = self.dm1 - 5
        self.assertAlmostEqual(dlm1.SumOfElements(), self.dm1.SumOfElements() - 5 * dlm1.size())

        dlm1.DifferenceOf(self.dm1, 5)
        self.assertAlmostEqual(dlm1.SumOfElements(), self.dm1.SumOfElements() - 5 * dlm1.size())

        dlm1 = self.dm1 * 2
        self.assertAlmostEqual(dlm1.SumOfElements(), self.dm1.SumOfElements() * 2)

        dlm1.ProductOf(self.dm1, 2)
        self.assertAlmostEqual(dlm1.SumOfElements(), self.dm1.SumOfElements() * 2)

        dlm1 = self.dm1 / 2
        self.assertAlmostEqual(dlm1.SumOfElements(), self.dm1.SumOfElements() / 2)

        dlm1.RatioOf(self.dm1, 2)
        self.assertAlmostEqual(dlm1.SumOfElements(), self.dm1.SumOfElements() / 2)

        
    def TestMoSiMi(self):
        """Test the MoSiMi based methods"""
        dlm1 = 5 + self.dm1
        self.assertAlmostEqual(dlm1.SumOfElements(), self.dm1.SumOfElements() + 5 * dlm1.size())

        dlm1.SumOf(5, self.dm1)
        self.assertAlmostEqual(dlm1.SumOfElements(), self.dm1.SumOfElements() + 5 * dlm1.size())

        dlm1 = 5 - self.dm1
        self.assertAlmostEqual(dlm1.SumOfElements(), 5 * dlm1.size() - self.dm1.SumOfElements())

        dlm1.DifferenceOf(5, self.dm1)
        self.assertAlmostEqual(dlm1.SumOfElements(), 5 * dlm1.size() - self.dm1.SumOfElements())

        dlm1 = 2 * self.dm1
        self.assertAlmostEqual(dlm1.SumOfElements(), self.dm1.SumOfElements() * 2)

        dlm1.ProductOf(2, self.dm1)
        self.assertAlmostEqual(dlm1.SumOfElements(), self.dm1.SumOfElements() * 2)

        dlm1 = 2 / self.dm1
        dlm2 = 2 / dlm1
        self.failUnless(dlm2.AlmostEqual(self.dm1))

        dlm1.RatioOf(2, self.dm1)
        dlm2.RatioOf(2, dlm1)
        self.failUnless(dlm2.AlmostEqual(self.dm1))


    def TestSoMi(self):
        product = 1
        sum = 0
        for row in xrange(0, self.dm1.rows()):
            for col in xrange(0, self.dm1.cols()):
                product = product * self.dm1[row][col]
                sum = sum + self.dm1[row][col]
        self.assertAlmostEqual(product, self.dm1.ProductOfElements())
        self.assertAlmostEqual(sum, self.dm1.SumOfElements())


    def TestMioMi(self):
        """ Test the MioMi based methods"""
        dlm1 = vctDoubleMat(self.dm1.rows(), self.dm1.cols())

        dlm1.Assign(self.dm1)
        dlm1 += self.dm2
        dlm1.Subtract(self.dm2)
        self.failUnless(dlm1.AlmostEqual(self.dm1))

        dlm1.Assign(self.dm1)
        dlm1 -= self.dm2
        dlm1.Add(self.dm2)
        self.failUnless(dlm1.AlmostEqual(self.dm1))


    def TestMioSi(self):
        """ Test the MioSi based methods"""
        dlm1 = vctDoubleMat(self.dm1.rows(), self.dm1.cols())

        dlm1.Assign(self.dm1)
        dlm1 += 5
        dlm1.Subtract(5)
        self.failUnless(dlm1.AlmostEqual(self.dm1))

        dlm1.Assign(self.dm1)
        dlm1 -= 5
        dlm1.Add(5)
        self.failUnless(dlm1.AlmostEqual(self.dm1))

        dlm1.Assign(self.dm1)
        dlm1 *= 2
        dlm1.Divide(2)
        self.failUnless(dlm1.AlmostEqual(self.dm1))

        dlm1.Assign(self.dm1)
        dlm1 /= 2
        dlm1.Multiply(2)
        self.failUnless(dlm1.AlmostEqual(self.dm1))


    def TestAssign(self):
        """ Test the Assign methods"""
        dlm1 = vctDoubleMat(self.dm1.rows(), self.dm1.cols())

        dlm1.Assign(self.dm1)
        self.failUnless(dlm1.AlmostEqual(self.dm1))
        
        dlm1.Assign(self.dm1[0][0] + 10)
        self.failUnless(dlm1.SumOfElements() == self.dm1.SumOfElements() + 10)
        
        command = "dlm1.Assign("
        sum = 0
        for index in range(dlm1.rows() * dlm1.cols()):
            command = command + str(index) + ','
            sum = sum + index
        command = command + ')'
        eval(command)
        self.assertAlmostEqual(sum, dlm1.SumOfElements())

        command = "dlm1.Assign("
        for index in range(dlm1.size() + 1):
            command = command + str(index) + ','
        command = command + ')'
        gotException = False
        try:
            eval(command)
        except RuntimeError, e:
            gotException = True
        self.failUnless(gotException)


    def TestMatrixProducts(self):
        """ Test matrix products"""
        dlm1 = vctDoubleMat(self.dm1.cols(), self.dm1.rows())
        dlm1.Random(-10, 10)
        dlm2 = vctDoubleMat(self.dm1.rows(), self.dm1.rows())
        dlm2.ProductOf(self.dm1, dlm1)
        dlm3 = self.dm1 * dlm1
        self.failUnless(dlm2.AlmostEqual(dlm3))


    def TestVectorProducts(self):
        """ Test vector products"""
        dlv1 = vctDoubleVec(self.dm1.cols())
        dlv1.Random(-10, 10)

        dlv2 = self.dm1 * dlv1
        # ckeck the size of result
        self.assertEqual(self.dm1.rows(), dlv2.size())
        # check result
        for i in range(dlv2.size()):
            sum = 0
            for j in range(self.dm1.cols()):
                sum += (self.dm1[i][j] * dlv1[j])
            self.failUnless(abs(sum - dlv2[i]) < 0.1)


#
# $Log: vctDynamicMatrixTest.py,v $
# Revision 1.13  2007/04/26 20:12:05  anton
# All files in tests: Applied new license text, separate copyright and
# updated dates, added standard header where missing.
#
# Revision 1.12  2006/11/20 20:33:53  anton
# Licensing: Applied new license to tests.
#
# Revision 1.11  2006/10/13 20:04:56  anton
# vctDynamic{Vector,Matrix}.py: Added tolerance to handle different numerical
# precision between C++ and Python computation.
#
# Revision 1.10  2006/08/03 19:22:27  anton
# cisstVector Python: Added tests for SumOfElements() and ProductOfElements()
# See ticket #229.
#
# Revision 1.9  2006/06/13 12:08:57  anton
# vctDynamicXyz Python tests: Updates based on Mai's contribution.
#
# Revision 1.8  2006/05/03 01:47:37  anton
# cisstVector wrapper tests: Renamed test classes to avoid name conflicts
# and avoid import *.  All test cases are now visible and running.
#
# Revision 1.7  2005/09/26 16:36:05  anton
# cisst: Added modelines for emacs and vi (python), see #164.
#
# Revision 1.6  2005/09/13 15:48:33  anton
# cisstVector wrapping tests: More tests ...
#
# Revision 1.5  2005/09/06 16:27:22  anton
# wrapping tests: Added license.
#
# Revision 1.4  2005/08/15 06:01:49  anton
# vctDynamicMatrixTest.py: Added test for matrix * matrix.  This test fails
# which seems to indicate a bug in the wrappers.
#
# Revision 1.3  2005/07/19 19:52:06  anton
# vctDynamicMatrixTest.py: For matrix * vector product, add some tolerance.
#
# Revision 1.2  2005/07/19 19:21:32  anton
# vctDynamicMatrixTest.py: Added tests for matrix * vector.
#
# Revision 1.1  2005/07/14 21:43:34  anton
# vctDynamicMatrixTest.py: Added basic tests.
#
