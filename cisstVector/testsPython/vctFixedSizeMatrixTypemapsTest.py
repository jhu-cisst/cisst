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

class FixedSizeMatrixTypemapsTest(unittest.TestCase):

    types = cisstVectorTypemapsTestTypes.vctFixedSizeMatrixTypemapsTest_types

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
            function(badvar)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give an array; expect no exception
        shape = tuple(self.CObject.sizes())
        goodvar = numpy.ones(shape, dtype=self.dtype)
        function(goodvar)


    # Tests that the typemap throws an exception if the data type isn't int
    def StdTestThrowUnlessDataType(self, function):
        shape = tuple(self.CObject.sizes())

        # Give an array of floats; expect an exception
        exceptionOccurred = False
        try:
            if (self.dtype != numpy.float64):
                badvar = numpy.ones(shape, dtype=numpy.float64)
            else:
                badvar = numpy.ones(shape, dtype=numpy.int32)
            function(badvar)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give an int; expect no exception
        goodvar = numpy.ones(shape, dtype=self.dtype)
        function(goodvar)


    # Tests that the typemap throws an exception if the array isn't 2D
    def StdTestThrowUnlessDimension2(self, function):
        # Give a 1D array; expect an exception
        exceptionOccurred = False
        try:
            badvar = numpy.ones(10, dtype=self.dtype)
            function(badvar)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give a 2D array; expect no exception
        shape = tuple(self.CObject.sizes())
        goodvar = numpy.ones(shape, dtype=self.dtype)
        function(goodvar)


    # Tests that the typemap throws an exception if there is a size mismatch
    def StdTestThrowUnlessCorrectMatrixSize(self, function):
        shape = tuple(self.CObject.sizes())
        shapePlusOne = (shape[0] + 1, shape[1] + 1)

        # Give a `rows+1, cols+1' array; expect an exception
        exceptionOccurred = False
        try:
            badvar = numpy.ones(shapePlusOne, dtype=self.dtype)
            function(badvar)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give a `rows, cols' array; expect no exception
        goodvar = numpy.ones(shape, dtype=self.dtype)
        function(goodvar)


    # Tests that the typemap throws an exception if the array isn't writable
    def StdTestThrowUnlessWritable(self, function):
        shape = tuple(self.CObject.sizes())

        # Give a non-writable array; expect an exception
        exceptionOccurred = False
        try:
            badvar = numpy.ones(shape, dtype=self.dtype)
            badvar.setflags(write=False)
            function(badvar)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give a writable array; expect no exception
        goodvar = numpy.ones(shape, dtype=self.dtype)
        function(goodvar)


    def StdTestThrowUnlessReturnedMatrixIsWritable(self, function):
        # Expect the returned array to be writable
        v = function()
        assert(v.flags['WRITEABLE'] == True)


    def StdTestThrowUnlessReturnedMatrixIsNonWritable(self, function):
        # Expect the returned array to be non-writable
        v = function()
        assert(v.flags['WRITEABLE'] == False)


    # Test if the C object reads the vector correctly
    def SpecTestThrowUnlessReadsCorrectly(self, function):
        shape = tuple(self.CObject.sizes())

        vNew = numpy.random.random_sample(shape)
        vNew = numpy.floor(vNew * 100)
        vNew = numpy.array(vNew, dtype=self.dtype)
        vOld = copy.deepcopy(vNew)
        rows = vNew.shape[0]
        cols = vNew.shape[1]
        function(vNew)

        assert(self.CObject.rows() == rows and self.CObject.cols() == cols)
        assert(vNew.shape[0] == rows and vNew.shape[1] == cols)
        for r in range(rows):
            for c in range(cols):
                # Test if the C object read the vector correctly
                assert(self.CObject.GetItem(r,c) == vOld[r,c])
                # Test that the C object did not modify the vector
                assert(vNew[r,c] == vOld[r,c])


    # Test if the C object reads and modifies the vector correctly
    def SpecTestThrowUnlessReadsWritesCorrectly(self, function):
        shape = tuple(self.CObject.sizes())

        vNew = numpy.random.random_sample(shape)
        vNew = numpy.floor(vNew * 100)
        vNew = numpy.array(vNew, dtype=self.dtype)
        vOld = copy.deepcopy(vNew)
        rows = vNew.shape[0]
        cols = vNew.shape[1]
        function(vNew)

        assert(self.CObject.rows() == rows and self.CObject.cols() == cols)
        assert(vNew.shape[0] == rows and vNew.shape[1] == cols)
        for r in range(rows):
            for c in range(cols):
                # Test if the C object read the vector correctly
                assert(self.CObject.GetItem(r,c) == vOld[r,c])
                # Test if the C object modified the vector correctly
                assert(vNew[r,c] == vOld[r,c] + 1)


    # Test if the C object returns a good vector
    def SpecTestThrowUnlessReceivesCorrectMatrix(self, function):
        v = function()

        rows = self.CObject.rows()
        cols = self.CObject.cols()
        assert(v.shape[0] == rows and v.shape[1] == cols)
        for r in range(rows):
            for c in range(cols):
                assert(self.CObject.GetItem(r,c) == v[r,c])


    ###########################################################################
    #   TEST FUNCTIONS - Put any new unit test here!  These are the unit      #
    #   tests that test the typemaps.  One test per typemap!                  #
    ###########################################################################

    def Test_in_vctFixedSizeMatrix(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.in_vctFixedSizeMatrix

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimension2(function)
            self.StdTestThrowUnlessCorrectMatrixSize(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsCorrectly(function)


    def Test_out_vctFixedSizeMatrix(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.out_vctFixedSizeMatrix

            # Perform battery of standard tests
            self.StdTestThrowUnlessReturnedMatrixIsWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReceivesCorrectMatrix(function)


    def Test_in_argout_vctFixedSizeMatrix_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.in_argout_vctFixedSizeMatrix_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimension2(function)
            self.StdTestThrowUnlessCorrectMatrixSize(function)
            self.StdTestThrowUnlessWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsWritesCorrectly(function)


    def Test_out_vctFixedSizeMatrix_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.out_vctFixedSizeMatrix_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessReturnedMatrixIsWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReceivesCorrectMatrix(function)


    def Test_in_argout_const_vctFixedSizeMatrix_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.in_argout_const_vctFixedSizeMatrix_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimension2(function)
            self.StdTestThrowUnlessCorrectMatrixSize(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsCorrectly(function)


    def Test_out_const_vctFixedSizeMatrix_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.out_const_vctFixedSizeMatrix_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessReturnedMatrixIsNonWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReceivesCorrectMatrix(function)
