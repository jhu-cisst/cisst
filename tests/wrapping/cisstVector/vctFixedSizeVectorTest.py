# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
# $Id: vctFixedSizeVectorTest.py,v 1.10 2007/04/26 20:12:05 anton Exp $
#

# Author: Anton Deguet
# Date: 2006-06-21
#
# (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
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

from cisstVectorPython import vctDoubleVec
from cisstVectorPython import vctIntVec

from vctFixedSizeVectorProxyTestPython import vctFixedSizeVectorProxyTest

# "d" is used to prefix double vectors and "i" for int vectors
# The letter "l" is added for local values

class FixedSizeVectorTest(unittest.TestCase):
    def setUp(self):
        """Call before every test case."""
        self.proxy = vctFixedSizeVectorProxyTest()
        
    def tearDown(self):
        """Call after every test case."""




    def IndividualTests(self, type, size):

        # Test size and default values
        vector = eval("self.proxy." + type + str(size) + "Ref()")
        self.assertEqual(vector.size(), size)
        for i in range(vector.size()):
            self.assertEqual(vector[i], i)

        # Add 5 to all elements and check the sum
        sumFromC = eval("self.proxy." + type + str(size) +  "Add(5)")
        vector = eval("self.proxy." + type + str(size) + "Ref()")
        sum = 0;
        for i in range(vector.size()):
            sum += vector[i]
        vector -= 5
        self.assertEqual(sum, sumFromC)

        # Add 5 to all elements on Python copy and then 5 to C++ copy
        copy = eval("self.proxy." + type + str(size) + "Copy()")
        reference = eval("self.proxy." + type + str(size) + "Ref()")
        self.assertEqual(reference.size(), copy.size())
        copy += 5
        for i in range(reference.size()):
            self.assertEqual(reference[i] + 5, copy[i])
        reference += 5
        for i in range(reference.size()):
            self.assertEqual(reference[i], copy[i])
        reference -= 5

        # Create a new vector with different values, set by reference and check sum
        vector = eval("vct" + type + "Vec(size)")
        sum = 0
        for i in range(size):
            vector[i] = 2 * (size - i)
            sum += vector[i]
        self.assertEqual(vector.SumOfElements(), sum)
        eval("self.proxy." + type + str(size) + "SetRefAdd10(vector)")
        self.assertEqual(eval("self.proxy." + type + str(size) + "SumOfElements()"), sum)
        # vector should have been modified, check its new sum
        self.assertEqual(vector.SumOfElements(), sum + vector.size() * 10)
        
        # Check that an exception is thrown is sizes don't match
        vector = eval("vct" + type + "Vec(size + 2)")
        exception = 0
        try:
            eval("self.proxy." + type + str(size) + "SetRefAdd10(vector)")
        except RuntimeError, e:
            exception = 1
        self.assertEqual(exception, 1)

        # Create a new vector with different values, set by copy and check sum
        vector = eval("vct" + type + "Vec(size)")
        sum = 0
        for i in range(size):
            vector[i] = 3 * (size - i)
            sum += vector[i]
        self.assertEqual(vector.SumOfElements(), sum)
        eval("self.proxy." + type + str(size) + "SetCopy(vector)")
        self.assertEqual(eval("self.proxy." + type + str(size) + "SumOfElements()"), sum)

        # Now, modify the C++ and make sure Python vector is unchanged
        eval("self.proxy." + type + str(size) + "Add(5)")
        self.assertEqual(vector.SumOfElements(), sum)

        # Check that an exception is thrown is sizes don't match
        vector = eval("vct" + type + "Vec(size + 2)")
        exception = 0
        try:
            eval("self.proxy." + type + str(size) + "SetCopy(vector)")
        except RuntimeError, e:
            exception = 1
        self.assertEqual(exception, 1)
            


    def TestDouble2(self):
        self.IndividualTests("Double", 2)

    def TestDouble3(self):
        self.IndividualTests("Double", 3)
        
    def TestDouble4(self):
        self.IndividualTests("Double", 4)

    def TestDouble5(self):
        self.IndividualTests("Double", 5)

    def TestDouble6(self):
        self.IndividualTests("Double", 6)



    def TestInt2(self):
        self.IndividualTests("Int", 2)

    def TestInt3(self):
        self.IndividualTests("Int", 3)

    def TestInt4(self):
        self.IndividualTests("Int", 4)

    def TestInt5(self):
        self.IndividualTests("Int", 5)

    def TestInt6(self):
        self.IndividualTests("Int", 6)



#
# $Log: vctFixedSizeVectorTest.py,v $
# Revision 1.10  2007/04/26 20:12:05  anton
# All files in tests: Applied new license text, separate copyright and
# updated dates, added standard header where missing.
#
# Revision 1.9  2006/11/20 20:33:53  anton
# Licensing: Applied new license to tests.
#
# Revision 1.8  2006/05/03 01:47:37  anton
# cisstVector wrapper tests: Renamed test classes to avoid name conflicts
# and avoid import *.  All test cases are now visible and running.
#
# Revision 1.7  2005/09/26 16:36:05  anton
# cisst: Added modelines for emacs and vi (python), see #164.
#
# Revision 1.6  2005/09/24 00:09:30  anton
# wrapping: All cisstVector modules require to link with/load cisstCommon.
# This was not the case so far because cisstVector was only relying on inline
# features of cisstCommon.
#
# Revision 1.5  2005/09/06 16:27:22  anton
# wrapping tests: Added license.
#
# Revision 1.4  2005/07/22 20:08:18  anton
# Fixed size containers: Added tests for reference inputs.
#
# Revision 1.3  2005/06/24 01:48:58  anton
# vctFixedSizeVectorTest: Renamed class, was copy/pasted from vctDynamicVector.
#
# Revision 1.2  2005/06/23 21:41:49  anton
# cisstVector wrapping:  Extensive testing of fixed size vectors.
#
# Revision 1.1  2005/06/23 15:19:04  anton
# cisstVector wrapping tests: Added the possibility to wrap some C++ classes
# used for further Python testing.  This is useful to test the typemaps which
# convert fixed size vectors back and forth to dynamic vectors.
#
#
