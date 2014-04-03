# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
#

# Author: Anton Deguet
# Date: 2005-08-14
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

# from cisstCommonPython import cmnTypeTraitsDouble
# from cisstVectorPython import vctDoubleMat
# from cisstVectorPython import vctDoubleVec
# from cisstNumericalPython import nmrSVDDynamicData
# from cisstNumericalPython import nmrSVD


# class SVDTest(unittest.TestCase):
#     def setUp(self):
#         """Call before every test case."""
#         self.dm1 = vctDoubleMat(5, 7)
#         self.dm1.Random(-10, 10)
#         self.dm2 = vctDoubleMat(7, 5)
#         self.dm2.Random(-10, 10)
        
#     def tearDown(self):
#         """Call after every test case."""
        
#     def TestSVD(self):
#         """Test SVD."""
#         svdData = nmrSVDDynamicData(self.dm1)
#         input = vctDoubleMat(self.dm1)
#         nmrSVD(self.dm1, svdData)
#         U = svdData.U()
#         Vt = svdData.Vt()
#         vectorS = svdData.S()
#         S = vctDoubleMat(input.rows(), input.cols())
#         S.SetAll(0.0)
#         S.Diagonal().Assign(vectorS)
#         SVt = vctDoubleMat(S.rows(), Vt.cols())
#         SVt.ProductOf(S, Vt)
#         USVt = vctDoubleMat(U.rows(), SVt.cols())
#         USVt.ProductOf(U, SVt)
#         error = (input - USVt).LinfNorm()
#         self.failUnless(error < cmnTypeTraitsDouble.Tolerance())

