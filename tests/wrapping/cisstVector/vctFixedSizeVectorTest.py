# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
# $Id$
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


