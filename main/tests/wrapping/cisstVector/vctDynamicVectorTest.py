# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
# $Id: vctDynamicVectorTest.py 75 2009-02-24 16:47:20Z adeguet1 $
#

# Author: Anton Deguet
# Date: 2005-04-08
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

from cisstVectorPython import vctDoubleVec
from cisstVectorPython import vctIntVec
from cisstVectorPython import vctDoubleMat
from cisstVectorPython import vctIntMat
from cisstVectorPython import vctShortVec
from cisstVectorPython import vctLongVec
from cisstVectorPython import vctBoolVec

# "d" is used to prefix double vectors and "i" for int vectors
# The letter "l" is added for local values

class DynamicVectorTest(unittest.TestCase):
    
    def setUp(self):
        """Call before every test case."""
        self.dv1 = vctDoubleVec(10)
        self.dv1.Random(1, 10)
        self.dv2 = vctDoubleVec(10)
        self.dv2.Random(1, 10)
        self.iv1 = vctIntVec(10)
        self.iv1.Random(1, 10)
        self.iv2 = vctIntVec(10)
        self.iv2.Random(1, 10)

        self.sv1 = vctShortVec(10)
        self.sv1.Random(1, 10)
        self.sv2 = vctShortVec(10)
        self.sv2.Random(1, 10)
        self.lv1 = vctLongVec(10)
        self.lv1.Random(1, 10)
        self.lv2 = vctLongVec(10)
        self.lv2.Random(1, 10)
        self.bv1 = vctBoolVec(10)
        self.bv2 = vctBoolVec(10)
        for index in xrange(0, self.dv1.size()):
           if self.dv1[index] > 5:
              self.bv1[index] = 1
           else:
              self.bv1[index] = 0
         
           if self.dv2[index] > 5:
              self.bv2[index] = 1
           else :
              self.bv2[index] = 0
   

        
    def tearDown(self):
        """Call after every test case."""


    def assertAlmostEqual(self, value1, value2, tolerance = cmnTypeTraitsDouble_Tolerance()):
        """Assert that two values are equal within given tolerance."""
        self.failUnless((abs(value1 - value2)) <= tolerance)


    def TestConstructorSize(self):
        """Test size."""
        dv = vctDoubleVec(5)
        self.assertEqual(dv.size(), 5)
        iv = vctIntVec(5)
        self.assertEqual(iv.size(), 5)
        sv = vctShortVec(5)
        self.assertEqual(sv.size(), 5)
        lv = vctLongVec(5)
        self.assertEqual(lv.size(), 5)
        bv = vctBoolVec(5)
        self.assertEqual(bv.size(), 5)


    def TestVoViVi(self):
        """Test the VoViVi based methods"""
        # test the sum
        dlv1 = self.dv1 + self.dv2
        self.assertAlmostEqual(dlv1.SumOfElements(), self.dv1.SumOfElements() + self.dv2.SumOfElements())
        # test the difference
        dlv2 = dlv1 - self.dv2
        self.failUnless(dlv2.AlmostEqual(self.dv1))

        # test the sum
        dlv1.SumOf(self.dv1, self.dv2)
        self.assertAlmostEqual(dlv1.SumOfElements(), self.dv1.SumOfElements() + self.dv2.SumOfElements(), cmnTypeTraitsDouble_Tolerance())
        # test the difference
        dlv2.DifferenceOf(dlv1, self.dv2)
        self.failUnless(dlv2.AlmostEqual(self.dv1))
        

    def TestVoViSi(self):
        """Test the VoViSi based methods"""
        dlv1 = self.dv1 + 5
        self.assertAlmostEqual(dlv1.SumOfElements(), self.dv1.SumOfElements() + 5 * dlv1.size())
     
        dlv1.SumOf(self.dv1, 5)
        self.assertAlmostEqual(dlv1.SumOfElements(), self.dv1.SumOfElements() + 5 * dlv1.size())

        dlv1 = self.dv1 - 5
        self.assertAlmostEqual(dlv1.SumOfElements(), self.dv1.SumOfElements() - 5 * dlv1.size())

        dlv1.DifferenceOf(self.dv1, 5)
        self.assertAlmostEqual(dlv1.SumOfElements(), self.dv1.SumOfElements() - 5 * dlv1.size())

        dlv1 = self.dv1 * 2
        self.assertAlmostEqual(dlv1.SumOfElements(), self.dv1.SumOfElements() * 2)

        dlv1.ProductOf(self.dv1, 2)
        self.assertAlmostEqual(dlv1.SumOfElements(), self.dv1.SumOfElements() * 2)

        dlv1 = self.dv1 / 2
        self.assertAlmostEqual(dlv1.SumOfElements(), self.dv1.SumOfElements() / 2)

        dlv1.RatioOf(self.dv1, 2)
        self.assertAlmostEqual(dlv1.SumOfElements(), self.dv1.SumOfElements() / 2)

      
    def TestVoSiVi(self):
        """Test the VoViSi based methods"""
        dlv1 = 5 + self.dv1
        self.assertAlmostEqual(dlv1.SumOfElements(), self.dv1.SumOfElements() + 5 * dlv1.size())

        dlv1.SumOf(5, self.dv1)
        self.assertAlmostEqual(dlv1.SumOfElements(), self.dv1.SumOfElements() + 5 * dlv1.size())

        dlv1 = 5 - self.dv1
        self.assertAlmostEqual(dlv1.SumOfElements(), 5 * dlv1.size() - self.dv1.SumOfElements())

        dlv1.DifferenceOf(5, self.dv1)
        self.assertAlmostEqual(dlv1.SumOfElements(), 5 * dlv1.size() - self.dv1.SumOfElements())

        dlv1 = 2 * self.dv1
        self.assertAlmostEqual(dlv1.SumOfElements(), self.dv1.SumOfElements() * 2)

        dlv1.ProductOf(2, self.dv1)
        self.assertAlmostEqual(dlv1.SumOfElements(), self.dv1.SumOfElements() * 2)

        dlv1 = 2 / self.dv1
        dlv2 = 2 / dlv1
        self.failUnless(dlv2.AlmostEqual(self.dv1))

        dlv1.RatioOf(2, self.dv1)
        dlv2.RatioOf(2, dlv1)
        self.failUnless(dlv2.AlmostEqual(self.dv1))


    def TestSoVi(self):
        product = 1
        sum = 0
        for index in xrange(0, self.dv1.size()):
            product = product * self.dv1[index]
            sum = sum + self.dv1[index]
        self.assertAlmostEqual(product, self.dv1.ProductOfElements())
        self.assertAlmostEqual(sum, self.dv1.SumOfElements())
        
    
    def TestSoViVi(self):
        dlv1 = vctDoubleVec(self.dv1.size())
        dlv2 = vctDoubleVec(self.dv1.size())
        dlv1.SumOf(self.dv1, 1)
        if self.dv1.size() > 0 :
           dlv1[0] = self.dv1[0]
        
        self.failUnless(dlv1.GreaterOrEqual(self.dv1))
        self.failUnless(not (dlv1.Greater(self.dv1)))
        self.failUnless(self.dv1.LesserOrEqual(dlv1))
        self.failUnless(not (self.dv1.Lesser(dlv1)))

        dlv1.Add(1)
        self.failUnless(dlv1.NotEqual(self.dv1))
        self.failUnless(dlv1 != self.dv1)
        self.failUnless(dlv1.Greater(self.dv1))
        self.failUnless(self.dv1.Lesser(dlv1))

        dlv1.Assign(self.dv1)
        self.failUnless(dlv1.Equal(self.dv1))
        self.failUnless(dlv1.AlmostEqual(self.dv1))
        self.failUnless(dlv1 == self.dv1)
        self.failUnless(dlv1.GreaterOrEqual(self.dv1))
        self.failUnless(dlv1.LesserOrEqual(self.dv1))
        
        self.failUnless(not (dlv1.NotEqual(self.dv1)))
        self.failUnless(not (dlv1 != self.dv1))
        self.failUnless(not (dlv1.Greater(self.dv1)))
        self.failUnless(not (dlv1.Lesser(self.dv1)))

        
        
    def TestVioVi(self):
        """ Test the VioVi based methods"""
        dlv1 = vctDoubleVec(self.dv1.size())

        dlv1.Assign(self.dv1)
        dlv1 += self.dv2
        dlv1.Subtract(self.dv2)
        self.failUnless(dlv1 == self.dv1)

        dlv1.Assign(self.dv1)
        dlv1 -= self.dv2
        dlv1.Add(self.dv2)
        self.failUnless(dlv1 == self.dv1)

        
    def TestVioSi(self):
        """ Test the VioSi based methods"""
        dlv1 = vctDoubleVec(self.dv1.size())

        dlv1.Assign(self.dv1)
        dlv1 += 5
        dlv1.Subtract(5)
        self.failUnless(dlv1 == self.dv1)

        dlv1.Assign(self.dv1)
        dlv1 -= 5
        dlv1.Add(5)
        self.failUnless(dlv1 == self.dv1)

        dlv1.Assign(self.dv1)
        dlv1 *= 2
        dlv1.Divide(2)
        self.failUnless(dlv1 == self.dv1)

        dlv1.Assign(self.dv1)
        dlv1 /= 2
        dlv1.Multiply(2)
        self.failUnless(dlv1 == self.dv1)


    def TestAssign(self):
        """ Test the Assign methods"""
        dlv1 = vctDoubleVec(self.dv1.size())

        dlv1.Assign(self.dv1)
        self.failUnless(dlv1 == self.dv1)
        
        dlv1.Assign(self.dv1[0] + 10)
        self.failUnless(dlv1.SumOfElements() == self.dv1.SumOfElements() + 10)
        
        command = "dlv1.Assign("
        sum = 0
        for index in xrange(0, dlv1.size()):
            command = command + str(index) + ','
            sum = sum + index
        command = command + ')'
        eval(command)
        self.failUnless(sum == dlv1.SumOfElements())

        command = "dlv1.Assign("
        for index in xrange(0, dlv1.size() + 1):
            command = command + str(index) + ','
        command = command + ')'
        gotException = False
        try:
            eval(command)
        except RuntimeError, e:
            gotException = True
        self.failUnless(gotException)

        # Test boolean Assign
        blv1 = vctBoolVec( self.bv1.size())
        blv1.Assign(self.bv1)
        matches = 0
        for i in xrange(0,blv1.size()):
          if (blv1[i] and self.bv1[i]) or ( not blv1[i] and not self.bv1[i]):
               matches = matches + 1
        self.failUnless( matches == blv1.size())

        command = "blv1.Assign("
        for index in xrange(0, blv1.size() + 1):
            command = command + str(index) + ','
        command = command + ')'
        gotException = False
        try:
            eval(command)
        except RuntimeError, e:
            gotException = True
        self.failUnless(gotException)


    def TestCrossProduct(self):
        """ Test the Cross Product methods"""
        dlv1 = vctDoubleVec(3)
        dlv2 = vctDoubleVec(3)
        dlv3 = vctDoubleVec(3)
        dlv1.Random(0, 1)
        dlv2.Random(0, 1)
        dlv3.CrossProductOf(dlv1, dlv2)
        self.assertAlmostEqual(dlv1.DotProduct(dlv3), 0)
        self.assertAlmostEqual(dlv2 * dlv3, 0)
        

    def TestMatrixVectorProduct(self):
        """ Test the matrix * vector methods"""
        dlm1 = vctDoubleMat(self.dv1.size(), self.dv1.size())
        dlm1.SetAll(0)
        dlm1.Diagonal().SetAll(1)

        dlv1 = dlm1 * self.dv1
        self.failUnless(dlv1.AlmostEqual(self.dv1))
        dlv1.ProductOf(dlm1, self.dv1)
        self.failUnless(dlv1.AlmostEqual(self.dv1))

        dlv2 = self.dv1 * dlm1
        self.failUnless(dlv2.AlmostEqual(self.dv1))
        dlv2.ProductOf(self.dv1, dlm1)
        self.failUnless(dlv2.AlmostEqual(self.dv1))


    def TestDotProduct(self):
        dlv1 = vctDoubleVec(3)
        dlv2 = vctDoubleVec(3)
        
        dlv1.Random(0, 1)
        dlv2.Random(0, 1)

        result = 0
        for index in xrange(0, dlv1.size()):
            result = result + (dlv1[index] * dlv2[index])

        self.assertAlmostEqual(result, dlv1.DotProduct(dlv2))
        self.assertAlmostEqual(result, dlv2.DotProduct(dlv1))
        self.assertAlmostEqual(result, dlv1 * dlv2)
        self.assertAlmostEqual(result, dlv2 * dlv1)

