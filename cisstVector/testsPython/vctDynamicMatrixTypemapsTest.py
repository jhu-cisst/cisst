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

class DynamicMatrixTypemapsTest(unittest.TestCase):

    types = cisstVectorTypemapsTestTypes.vctDynamicMatrixTypemapsTest_types

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
        goodvar = numpy.ones((5, 7), dtype=self.dtype)
        function(goodvar, 0)


    # Tests that the typemap throws an exception if the data type isn't int
    def StdTestThrowUnlessDataType(self, function):
        # Give an array of floats; expect an exception
        exceptionOccurred = False
        try:
            if (self.dtype != numpy.float64):
                badvar = numpy.ones((5, 7), dtype=numpy.float64)
            else:
                badvar = numpy.ones((5, 7), dtype=numpy.int32)
            function(badvar, 0)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give an int; expect no exception
        goodvar = numpy.ones((5, 7), dtype=self.dtype)
        function(goodvar, 0)


    # Tests that the typemap throws an exception if the array isn't 2D
    def StdTestThrowUnlessDimension2(self, function):
        # Give a 1D array; expect an exception
        exceptionOccurred = False
        try:
            badvar = numpy.ones(10, dtype=self.dtype)
            function(badvar, 0)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give a 2D array; expect no exception
        goodvar = numpy.ones((5, 7), dtype=self.dtype)
        function(goodvar, 0)


    # Tests that the typemap throws an exception if the array isn't writable
    def StdTestThrowUnlessWritable(self, function):
        # Give a non-writable array; expect an exception
        exceptionOccurred = False
        try:
            badvar = numpy.ones((5, 7), dtype=self.dtype)
            badvar.setflags(write=False)
            function(badvar, 0)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give a writable array; expect no exception
        goodvar = numpy.ones((5, 7), dtype=self.dtype)
        function(goodvar, 0)


    def StdTestThrowUnlessOwnsData(self, function):
        # Give a non-memory owning array; expect an exception
        exceptionOccurred = False
        try:
            temp = numpy.ones((5, 7), dtype=self.dtype)
            badvar = temp[:]
            function(badvar, 0)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give a memory-owning array; expect no exception
        goodvar = numpy.ones((5, 7), dtype=self.dtype)
        function(goodvar, 0)


    def StdTestThrowUnlessNotReferenced(self, function):
        # Give an array with a reference on it; expect an exception
        exceptionOccurred = False
        try:
            badvar = numpy.ones((5, 7), dtype=self.dtype)
            temp = badvar
            function(badvar, 0)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give an array with no references; expect no exception
        goodvar = numpy.ones((5, 7), dtype=self.dtype)
        function(goodvar, 0)


    def StdTestThrowUnlessReturnedMatrixIsWritable(self, function):
        # Expect the returned array to be writable
        ROWS = 5
        COLS = 7
        v = function(ROWS, COLS)
        assert(v.flags['WRITEABLE'] == True)


    def StdTestThrowUnlessReturnedMatrixIsNonWritable(self, function):
        # Expect the returned array to be non-writable
        ROWS = 5
        COLS = 7
        v = function(ROWS, COLS)
        assert(v.flags['WRITEABLE'] == False)


    # Test if the C object reads the vector correctly
    def SpecTestThrowUnlessReadsCorrectly(self, function):
        STORAGE_ORDER = ['C', 'F']

        for stord in STORAGE_ORDER:
            vNew = numpy.random.rand(5, 7)
            vNew = numpy.floor(vNew * 100)
            vNew = numpy.array(vNew, dtype=self.dtype, order=stord)
            vOld = copy.deepcopy(vNew)
            vOld = numpy.array(vOld, order=stord)
            rows = vNew.shape[0]
            cols = vNew.shape[1]
            function(vNew, 0)

            assert(self.CObject.rows() == rows and self.CObject.cols() == cols)
            assert(vNew.shape[0] == rows and vNew.shape[1] == cols)
            assert(self.CObject.StorageOrder() == stord)
            for r in range(rows):
                for c in range(cols):
                    # Test if the C object read the vector correctly
                    assert(self.CObject.GetItem(r,c) == vOld[r,c])
                    # Test that the C object did not modify the vector
                    assert(vNew[r,c] == vOld[r,c])


    # Test if the C object reads and modifies the vector correctly
    def SpecTestThrowUnlessReadsWritesCorrectly(self, function):
        STORAGE_ORDER = ['C', 'F']

        for stord in STORAGE_ORDER:
            vNew = numpy.random.rand(5, 7)
            vNew = numpy.floor(vNew * 100)
            vNew = numpy.array(vNew, dtype=self.dtype, order=stord)
            vOld = copy.deepcopy(vNew)
            vOld = numpy.array(vOld, order=stord)
            rows = vNew.shape[0]
            cols = vNew.shape[1]
            function(vNew, 0)

            assert(self.CObject.rows() == rows and self.CObject.cols() == cols)
            assert(vNew.shape[0] == rows and vNew.shape[1] == cols)
            assert(self.CObject.StorageOrder() == stord)
            for r in range(rows):
                for c in range(cols):
                    # Test if the C object read the vector correctly
                    assert(self.CObject.GetItem(r,c) == vOld[r,c])
                    # Test if the C object modified the vector correctly
                    assert(vNew[r,c] == vOld[r,c] + 1)


    # Test if the C object reads, modifies, and resizes the vector correctly
    def SpecTestThrowUnlessReadsWritesResizesCorrectly(self, function):
        STORAGE_ORDER = ['C', 'F']

        for stord in STORAGE_ORDER:
            vNew = numpy.random.rand(5, 7)
            vNew = numpy.floor(vNew * 100)
            vNew = numpy.array(vNew, dtype=self.dtype, order=stord)
            vOld = copy.deepcopy(vNew)
            vOld = numpy.array(vOld, order=stord)
            rows = vNew.shape[0]
            cols = vNew.shape[1]
            SIZE_FACTOR = 3
            function(vNew, SIZE_FACTOR)

            assert(self.CObject.rows() == rows and self.CObject.cols() == cols)
            assert(vNew.shape[0] == rows * SIZE_FACTOR and vNew.shape[1] == cols * SIZE_FACTOR)
            assert(self.CObject.StorageOrder() == stord)
            for r in range(rows):
                for c in range(cols):
                    # Test if the C object read the vector correctly
                    assert(self.CObject.GetItem(r,c) == vOld[r,c])
                    # Test if the C object modified the vector correctly
                    assert(vNew[r,c] == vOld[r,c] + 1)
                    # Test if the C object resized the vector correctly
                    for r2 in range(SIZE_FACTOR):
                        for c2 in range(SIZE_FACTOR):
                            assert(vOld[r,c] + 1 == vNew[r + rows*r2, c + cols*c2])


    # Test if the C object returns a good vector
    def SpecTestThrowUnlessReceivesCorrectMatrix(self, function):
        ROWS = 5
        COLS = 7
        v = function(ROWS, COLS)

        assert(v.shape[0] == ROWS and v.shape[1] == COLS)
        for r in range(ROWS):
            for c in range(COLS):
                assert(self.CObject.GetItem(r,c) == v[r,c])


    ###########################################################################
    #   TEST FUNCTIONS - Put any new unit test here!  These are the unit      #
    #   tests that test the typemaps.  One test per typemap!                  #
    ###########################################################################

    def Test_in_argout_vctDynamicMatrix_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.in_argout_vctDynamicMatrix_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimension2(function)
            self.StdTestThrowUnlessWritable(function)
            self.StdTestThrowUnlessOwnsData(function)
            self.StdTestThrowUnlessNotReferenced(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsWritesCorrectly(function)
            self.SpecTestThrowUnlessReadsWritesResizesCorrectly(function)


    def Test_in_vctDynamicMatrixRef(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.in_vctDynamicMatrixRef

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimension2(function)
            self.StdTestThrowUnlessWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsWritesCorrectly(function)


    def Test_in_vctDynamicConstMatrixRef(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.in_vctDynamicConstMatrixRef

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimension2(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsCorrectly(function)


    def Test_in_argout_const_vctDynamicConstMatrixRef_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.in_argout_const_vctDynamicConstMatrixRef_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimension2(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsCorrectly(function)


    def Test_in_argout_const_vctDynamicMatrixRef_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.in_argout_const_vctDynamicMatrixRef_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimension2(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsCorrectly(function)


    def Test_in_vctDynamicMatrix(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.in_vctDynamicMatrix

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimension2(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsCorrectly(function)


    def Test_in_argout_const_vctDynamicMatrix_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.in_argout_const_vctDynamicMatrix_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimension2(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsCorrectly(function)


    def Test_out_vctDynamicMatrix(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.out_vctDynamicMatrix

            # Perform battery of standard tests
            self.StdTestThrowUnlessReturnedMatrixIsWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReceivesCorrectMatrix(function)


    def Test_out_vctDynamicMatrix_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.out_vctDynamicMatrix_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessReturnedMatrixIsWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReceivesCorrectMatrix(function)


    def Test_out_const_vctDynamicMatrix_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.out_const_vctDynamicMatrix_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessReturnedMatrixIsNonWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReceivesCorrectMatrix(function)


    def Test_out_vctDynamicMatrixRef(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.out_vctDynamicMatrixRef

            # Perform battery of standard tests
            self.StdTestThrowUnlessReturnedMatrixIsWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReceivesCorrectMatrix(function)


    def Test_out_vctDynamicConstMatrixRef(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()

            function = self.CObject.out_vctDynamicConstMatrixRef

            # Perform battery of standard tests
            self.StdTestThrowUnlessReturnedMatrixIsNonWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReceivesCorrectMatrix(function)
