# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
# $Id: vctMatrixRotation3Test.py,v 1.6 2008/03/13 20:46:46 anton Exp $
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


# # "v" is for vector and "mr" is for matrix rotation
# # The letter "l" is added for local values

# class MatrixRotation3Test(unittest.TestCase):
    
#     def setUp(self):
#         """Call before every test case."""
#         self.v1 = vctDoubleVec(3)
#         self.v1.Random(-10, 10)
#         self.mr1 = vctMatRot3()
#         self.mr1.Random()

        
#     def tearDown(self):
#         """Call after every test case."""


#     def assertApproximates(self, first, second, tolerance, msg=None):
#         """Assert equality with tolerance (feature added to unittest)"""
#         if abs(first - second) > tolerance:
#             raise FailTest, (msg or "%s ~== %s" % (first, second))


#     def TestConstructor(self):
#         """Test the default constructor"""
#         mr2 = vctMatRot3()
#         self.failUnless(mr2.IsNormalized())
#         self.assertApproximates(mr2[0].Norm(), 1.0, 0.01)
#         self.assertApproximates(mr2[1].Norm(), 1.0, 0.01)
#         self.assertApproximates(mr2[2].Norm(), 1.0, 0.01)


#     def TestRandom(self):
#         """Test the default constructor"""
#         mr2 = vctMatRot3()
#         mr2.Random()
#         self.failUnless(mr2.IsNormalized())
#         self.assertApproximates(mr2[0].Norm(), 1.0, 0.01)
#         self.assertApproximates(mr2[1].Norm(), 1.0, 0.01)
#         self.assertApproximates(mr2[2].Norm(), 1.0, 0.01)


#     def TestApplyToVector(self):
#         """Apply to a vector."""
#         # Apply(in, out)
#         v2 = vctDoubleVec(3)
#         self.mr1.ApplyTo(self.v1, v2)
#         v3 = vctDoubleVec(3)
#         self.mr1.ApplyInverseTo(v2, v3)
#         self.failUnless(self.v1.AlmostEqual(v3))
#         # out = Apply(in)
#         v4 = self.mr1.ApplyTo(self.v1)
#         self.failUnless(v2.AlmostEqual(v4))
#         v5 = self.mr1.ApplyInverseTo(v4)
#         self.failUnless(v5.AlmostEqual(self.v1))


#     def TestProductWithVector(self):
#         """Apply to a vector."""
#         v2 = self.mr1 * self.v1
#         mr1i = self.mr1.Inverse()
#         v3 = mr1i * v2
#         self.failUnless(self.v1.AlmostEqual(v3))


#     def TestApplyToMatRot(self):
#         """Apply to a rotation matrix."""
#         # Apply(in, out)
#         mr2 = vctMatRot3()
#         mr2.Random()
#         mr3 = vctMatRot3()
#         self.mr1.ApplyTo(mr2, mr3)
#         mr4 = vctMatRot3()
#         self.mr1.ApplyInverseTo(mr3, mr4)
#         self.failUnless(mr2.AlmostEqual(mr4))
#         # out = Apply(in)
#         mr5 = self.mr1.ApplyTo(mr2)
#         self.failUnless(mr5.AlmostEqual(mr3))
#         mr6 = self.mr1.ApplyInverseTo(mr5)
#         self.failUnless(mr6.AlmostEqual(mr2))


#     def TestProductWithMatRot(self):
#         """Apply to a rotation matrix."""
#         mr2 = vctMatRot3()
#         mr2.Random()
#         mr3 = self.mr1 * mr2
#         mr1i = self.mr1.Inverse()
#         mr4 = mr1i * mr3
#         self.failUnless(mr4.AlmostEqual(mr2))


#     def TestConversionQuatRot3(self):
#         """Test conversion to and from quaternion rotation"""
#         # Using default ctor and From
#         qr1 = vctQuatRot3()
#         qr1.From(self.mr1)
#         mr2 = vctMatRot3()
#         mr2.From(qr1)
#         self.failUnless(mr2.AlmostEqual(self.mr1))
#         # Using ctor with conversion
#         qr2 = vctQuatRot3(self.mr1)
#         mr3 = vctMatRot3(qr2)
#         self.failUnless(mr3.AlmostEqual(self.mr1))


#
# $Log: vctMatrixRotation3Test.py,v $
# Revision 1.6  2008/03/13 20:46:46  anton
# cisstVector wrapper tests: Removed tests on transformations.
#
# Revision 1.5  2007/04/26 20:12:05  anton
# All files in tests: Applied new license text, separate copyright and
# updated dates, added standard header where missing.
#
# Revision 1.4  2006/11/20 20:33:53  anton
# Licensing: Applied new license to tests.
#
# Revision 1.3  2006/05/03 01:47:37  anton
# cisstVector wrapper tests: Renamed test classes to avoid name conflicts
# and avoid import *.  All test cases are now visible and running.
#
# Revision 1.2  2005/10/08 20:30:31  anton
# vctMatrixRotation3Test.py: Renamed class, added method for conversion to
# vctQuatRot3.
#
# Revision 1.1  2005/10/07 09:36:57  anton
# cisstVector wrapper tests: Added preliminary tests for vctMatRot3.
#
#
