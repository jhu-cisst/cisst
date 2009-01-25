# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
# $Id: vctQuaternionRotation3Test.py,v 1.6 2008/03/13 20:46:46 anton Exp $
#

# Author: Anton Deguet
# Date: 2005-10-07
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

# import unittest

# from cisstVectorPython import vctDoubleVec
# from cisstVectorPython import vctMatRot3
# from cisstVectorPython import vctQuatRot3


# # "v" is for vector and "qr" is for quaternion rotation
# # The letter "l" is added for local values

# class QuaternionRotation3Test(unittest.TestCase):
    
#     def setUp(self):
#         """Call before every test case."""
#         self.v1 = vctDoubleVec(3)
#         self.v1.Random(-10, 10)
#         self.qr1 = vctQuatRot3()
#         self.qr1.Random()

        
#     def tearDown(self):
#         """Call after every test case."""


#     def assertApproximates(self, first, second, tolerance, msg=None):
#         """Assert equality with tolerance (feature added to unittest)"""
#         if abs(first - second) > tolerance:
#             raise FailTest, (msg or "%s ~== %s" % (first, second))


#     def TestConstructor(self):
#         """Test the default constructor"""
#         qr2 = vctQuatRot3()
#         self.failUnless(qr2.IsNormalized())


#     def TestRandom(self):
#         """Test the default constructor"""
#         qr2 = vctQuatRot3()
#         qr2.Random()
#         self.failUnless(qr2.IsNormalized())


#     def TestApplyToVector(self):
#         """Apply to a vector."""
#         # Apply(in, out)
#         v2 = vctDoubleVec(3)
#         self.qr1.ApplyTo(self.v1, v2)
#         v3 = vctDoubleVec(3)
#         self.qr1.ApplyInverseTo(v2, v3)
#         self.failUnless(self.v1.AlmostEqual(v3))
#         # out = Apply(in)
#         v4 = self.qr1.ApplyTo(self.v1)
#         self.failUnless(v2.AlmostEqual(v4))
#         v5 = self.qr1.ApplyInverseTo(v4)
#         self.failUnless(v5.AlmostEqual(self.v1))


#     def TestProductWithVector(self):
#         """Apply to a vector."""
#         v2 = self.qr1 * self.v1
#         qr1i = self.qr1.Inverse()
#         v3 = qr1i * v2
#         self.failUnless(self.v1.AlmostEqual(v3))


#     def TestApplyToQuatRot(self):
#         """Apply to a rotation quaternion."""
#         # Apply(in, out)
#         qr2 = vctQuatRot3()
#         qr2.Random()
#         qr3 = vctQuatRot3()
#         self.qr1.ApplyTo(qr2, qr3)
#         qr4 = vctQuatRot3()
#         self.qr1.ApplyInverseTo(qr3, qr4)
#         self.failUnless(qr2.AlmostEquivalent(qr4))
#         # out = Apply(in)
#         qr5 = self.qr1.ApplyTo(qr2)
#         self.failUnless(qr5.AlmostEquivalent(qr3))
#         qr6 = self.qr1.ApplyInverseTo(qr5)
#         self.failUnless(qr6.AlmostEquivalent(qr2))


#     def TestProductWithQuatRot(self):
#         """Apply to a rotation quaternion."""
#         qr2 = vctQuatRot3()
#         qr2.Random()
#         qr3 = self.qr1 * qr2
#         qr1i = self.qr1.Inverse()
#         qr4 = qr1i * qr3
#         self.failUnless(qr4.AlmostEquivalent(qr2))


#     def TestConversionMatRot3(self):
#         """Test conversion to and from quaternion rotation"""
#         # Using default ctor and From
#         mr1 = vctMatRot3()
#         mr1.From(self.qr1)
#         qr2 = vctQuatRot3()
#         qr2.From(mr1)
#         self.failUnless(qr2.AlmostEquivalent(self.qr1))
#         # Using ctor with conversion
#         mr2 = vctMatRot3(self.qr1)
#         qr3 = vctQuatRot3(mr2)
#         self.failUnless(qr3.AlmostEquivalent(self.qr1))


