# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
# Author(s):  Daniel Li, Anton Deguet
# Created on: 2009-05-20
#
# (C) Copyright 2009-2018 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---
#

#######################
# PLACEHOLDER STRINGS TO LOOK FOR:
#
# TODO          todo
#######################

# TODO: If I have time, Document why self.CObject[i] works and check which typemap(s) used

import copy
import numpy
import unittest

import cisstVectorTypemapsTestTypes

class DynamicVectorTypemapsTest(unittest.TestCase):

    types = cisstVectorTypemapsTestTypes.vctDynamicVectorTypemapsTest_types

    ###########################################################################
    #   SET UP function                                                       #
    ###########################################################################

    def setUp(self):
        pass

    ###########################################################################
    #   STANDARD TESTS - These are the library of tests that will be called   #
    #   by the test functions.                                                #
    ###########################################################################

    # Tests that the typemap throws an exception if the argument isn't an array
    def StdTestThrowUnlessIsArray(self, function):
        # Give a non-array; expect an exception
        exceptionOccurred = False
        try:
            badvar = 0.0
            function(badvar, 0)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give an array; expect no exception
        goodvar = numpy.ones(10, dtype=self.dtype)
        function(goodvar, 0)


    # Tests that the typemap throws an exception if the data type isn't int
    def StdTestThrowUnlessDataType(self, function):
        # Give an array of floats; expect an exception
        exceptionOccurred = False
        try:
            if (self.dtype != numpy.float64):
                badvar = numpy.ones(10, dtype=numpy.float64)
            else:
                badvar = numpy.ones(10, dtype=numpy.int32)
            function(badvar, 0)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give an int; expect no exception
        goodvar = numpy.ones(10, dtype=self.dtype)
        function(goodvar, 0)


    # Tests that the typemap throws an exception if the array isn't 1D
    def StdTestThrowUnlessDimension1(self, function):
        # Give a 2D array; expect an exception
        exceptionOccurred = False
        try:
            badvar = numpy.array([[1, 2, 3], [4, 5, 6]])
            function(badvar, 0)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give a 1D array; expect no exception
        goodvar = numpy.ones(10, dtype=self.dtype)
        function(goodvar, 0)


    # Tests that the typemap throws an exception if the array isn't writable
    def StdTestThrowUnlessWritable(self, function):
        # Give a non-writable array; expect an exception
        exceptionOccurred = False
        try:
            badvar = numpy.ones(10, dtype=self.dtype)
            badvar.setflags(write=False)
            function(badvar, 0)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give a writable array; expect no exception
        goodvar = numpy.ones(10, dtype=self.dtype)
        function(goodvar, 0)


    def StdTestThrowUnlessOwnsData(self, function):
        # Give a non-memory owning array; expect an exception
        exceptionOccurred = False
        try:
            temp = numpy.ones(10, dtype=self.dtype)
            badvar = temp[:]
            function(badvar, 0)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give a memory-owning array; expect no exception
        goodvar = numpy.ones(10, dtype=self.dtype)
        function(goodvar, 0)


    def StdTestThrowUnlessNotReferenced(self, function):
        # Give an array with a reference on it; expect an exception
        exceptionOccurred = False
        try:
            badvar = numpy.ones(10, dtype=self.dtype)
            temp = badvar
            function(badvar, 0)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give an array with no references; expect no exception
        goodvar = numpy.ones(10, dtype=self.dtype)
        function(goodvar, 0)


    def StdTestThrowUnlessReturnedVectorIsWritable(self, function):
        # Expect the returned array to be writable
        SIZE = 10
        v = function(SIZE)
        assert(v.flags['WRITEABLE'] == True)


    def StdTestThrowUnlessReturnedVectorIsNonWritable(self, function):
        # Expect the returned array to be non-writable
        SIZE = 10
        v = function(SIZE)
        assert(v.flags['WRITEABLE'] == False)


    # Test if the C object reads the vector correctly
    def SpecTestThrowUnlessReadsCorrectly(self, function):
        vNew = numpy.random.rand(10)
        vNew = numpy.floor(vNew * 100)
        vNew = numpy.array(vNew, dtype=self.dtype)
        vOld = copy.deepcopy(vNew)
        size = vNew.size
        function(vNew, 0)

        assert(self.CObject.size() == size)
        assert(vNew.size == size)
        for i in range(size):
            # Test if the C object read the vector correctly
            assert(self.CObject[i] == vOld[i])
            # Test that the C object did not modify the vector
            assert(vNew[i] == vOld[i])


    # Test if the C object reads and modifies the vector correctly
    def SpecTestThrowUnlessReadsWritesCorrectly(self, function):
        vNew = numpy.random.rand(10)
        vNew = numpy.floor(vNew * 100)
        vNew = numpy.array(vNew, dtype=self.dtype)
        vOld = copy.deepcopy(vNew)
        size = vNew.size
        function(vNew, 0)

        assert(self.CObject.size() == size)
        assert(vNew.size == size)
        for i in range(size):
            # Test if the C object read the vector correctly
            assert(self.CObject[i] == vOld[i])
            # Test if the C object modified the vector correctly
            assert(vNew[i] == vOld[i] + 1)


    # Test if the C object reads, modifies, and resizes the vector correctly
    def SpecTestThrowUnlessReadsWritesResizesCorrectly(self, function):
        vNew = numpy.random.rand(10)
        vNew = numpy.floor(vNew * 100)
        vNew = numpy.array(vNew, dtype=self.dtype)
        vOld = copy.deepcopy(vNew)
        size = vNew.size
        SIZE_FACTOR = 3
        function(vNew, SIZE_FACTOR)

        assert(self.CObject.size() == size)
        assert(vNew.size == size * SIZE_FACTOR)
        for i in range(size):
            # Test if the C object read the vector correctly
            assert(self.CObject[i] == vOld[i])
            # Test if the C object modified the vector correctly
            assert(vNew[i] == vOld[i] + 1)
            # Test if the C object resized the vector correctly
            for j in range(SIZE_FACTOR):
                assert(vOld[i] + 1 == vNew[i + size*j])


    # Test if the C object returns a good vector
    def SpecTestThrowUnlessReceivesCorrectVector(self, function):
        SIZE = 10
        v = function(SIZE)

        assert(v.size == SIZE)
        for i in range(SIZE):
            assert(self.CObject[i] == v[i])


    ###########################################################################
    #   TEST FUNCTIONS - Put any new unit test here!  These are the unit      #
    #   tests that test the typemaps.  One test per typemap!                  #
    ###########################################################################

    def Test_in_argout_vctDynamicVector_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.in_argout_vctDynamicVector_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimension1(function)
            self.StdTestThrowUnlessWritable(function)
            self.StdTestThrowUnlessOwnsData(function)
            self.StdTestThrowUnlessNotReferenced(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsWritesCorrectly(function)
            self.SpecTestThrowUnlessReadsWritesResizesCorrectly(function)


    def Test_in_vctDynamicVectorRef(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.in_vctDynamicVectorRef

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimension1(function)
            self.StdTestThrowUnlessWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsWritesCorrectly(function)


    def Test_in_vctDynamicConstVectorRef(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.in_vctDynamicConstVectorRef

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimension1(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsCorrectly(function)


    def Test_in_argout_const_vctDynamicConstVectorRef_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.in_argout_const_vctDynamicConstVectorRef_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimension1(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsCorrectly(function)


    def Test_in_argout_const_vctDynamicVectorRef_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.in_argout_const_vctDynamicVectorRef_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimension1(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsCorrectly(function)


    def Test_in_vctDynamicVector(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.in_vctDynamicVector

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimension1(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsCorrectly(function)


    def Test_in_argout_const_vctDynamicVector_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.in_argout_const_vctDynamicVector_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimension1(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsCorrectly(function)


    def Test_out_vctDynamicVector(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.out_vctDynamicVector

            # Perform battery of standard tests
            self.StdTestThrowUnlessReturnedVectorIsWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReceivesCorrectVector(function)


    def Test_out_vctDynamicVector_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.out_vctDynamicVector_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessReturnedVectorIsWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReceivesCorrectVector(function)


    def Test_out_const_vctDynamicVector_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.out_const_vctDynamicVector_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessReturnedVectorIsNonWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReceivesCorrectVector(function)


    def Test_out_vctDynamicVectorRef(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.out_vctDynamicVectorRef

            # Perform battery of standard tests
            self.StdTestThrowUnlessReturnedVectorIsWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReceivesCorrectVector(function)


    def Test_out_vctDynamicConstVectorRef(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.out_vctDynamicConstVectorRef

            # Perform battery of standard tests
            self.StdTestThrowUnlessReturnedVectorIsNonWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReceivesCorrectVector(function)
