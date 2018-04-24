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

class DynamicNArrayTypemapsTest(unittest.TestCase):

    types = cisstVectorTypemapsTestTypes.vctDynamicNArrayTypemapsTest_types

    ###########################################################################
    #   SET UP function                                                       #
    ###########################################################################

    def setUp(self):
        pass


    ###########################################################################
    #   HELPER FUNCTIONS - Used by the functions in this .py file             #
    ###########################################################################

    # Assuming the CObject is already set, defines the type used for
    # indices and sizes
    def SetSizeType(self):
        if (self.CObject.sizeOfSizes() == 4):
            self.size_type = numpy.uint32
        else:
            self.size_type = numpy.uint64
        return self.size_type


    # Given a dimensionality, returns a vector of random sizes for an ndarray
    # of that dimension
    def HelperRandSizes(self, ndim):
        # TODO: Limit sizes to be something other than [1, 10]; possibly [3, 7]
        sizes = numpy.random.randint(1, high = 10, size = ndim)
        sizes = tuple(sizes)
        return sizes


    # Converts a given metaindex `index' on an array of shape `shape' to a
    # tuple index
    def HelperMetaIndexToTuple(self, index, shape):
        indexList = []
        for j in shape[::-1]:
            r = index % j
            index //= j
            indexList.append(r)
        indexList.reverse()
        indexTuple = tuple(indexList)
        return indexTuple


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
        sizes = self.HelperRandSizes(self.CObject.Dim())
        goodvar = numpy.ones(sizes, dtype=self.dtype)
        function(goodvar, 0)


    # Tests that the typemap throws an exception if the data type isn't int
    def StdTestThrowUnlessDataType(self, function):
        # Give an array of floats; expect an exception
        exceptionOccurred = False
        try:
            sizes = self.HelperRandSizes(self.CObject.Dim())
            if (self.dtype != numpy.float64):
                badvar = numpy.ones(sizes, dtype=numpy.float64)
            else:
                badvar = numpy.ones(sizes, dtype=self.size_type)
            function(badvar, 0)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give an int; expect no exception
        sizes = self.HelperRandSizes(self.CObject.Dim())
        goodvar = numpy.ones(sizes, dtype=self.dtype)
        function(goodvar, 0)


    # Tests that the typemap throws an exception if the array isn't of the
    # correct dimension
    def StdTestThrowUnlessDimensionN(self, function):
        # Give a (n-1)D array; expect an exception
        exceptionOccurred = False
        try:
            sizes = self.HelperRandSizes(self.CObject.Dim() - 1)
            badvar = numpy.ones(sizes, dtype=self.dtype)
            function(badvar, 0)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give an n-D array; expect no exception
        sizes = self.HelperRandSizes(self.CObject.Dim())
        goodvar = numpy.ones(sizes, dtype=self.dtype)
        function(goodvar, 0)


    # Tests that the typemap throws an exception if the array isn't writable
    def StdTestThrowUnlessWritable(self, function):
        # Give a non-writable array; expect an exception
        exceptionOccurred = False
        try:
            sizes = self.HelperRandSizes(self.CObject.Dim())
            badvar = numpy.ones(sizes, dtype=self.dtype)
            badvar.setflags(write=False)
            function(badvar, 0)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give a writable array; expect no exception
        sizes = self.HelperRandSizes(self.CObject.Dim())
        goodvar = numpy.ones(sizes, dtype=self.dtype)
        function(goodvar, 0)


    def StdTestThrowUnlessOwnsData(self, function):
        # Give a non-memory owning array; expect an exception
        exceptionOccurred = False
        try:
            sizes = self.HelperRandSizes(self.CObject.Dim())
            temp = numpy.ones(sizes, dtype=self.dtype)
            badvar = temp[:]
            function(badvar, 0)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give a memory-owning array; expect no exception
        sizes = self.HelperRandSizes(self.CObject.Dim())
        goodvar = numpy.ones(sizes, dtype=self.dtype)
        function(goodvar, 0)


    def StdTestThrowUnlessNotReferenced(self, function):
        # Give an array with a reference on it; expect an exception
        exceptionOccurred = False
        try:
            sizes = self.HelperRandSizes(self.CObject.Dim())
            badvar = numpy.ones(sizes, dtype=self.dtype)
            temp = badvar
            function(badvar, 0)
        except:
            exceptionOccurred = True
        assert(exceptionOccurred)

        # Give an array with no references; expect no exception
        sizes = self.HelperRandSizes(self.CObject.Dim())
        goodvar = numpy.ones(sizes, dtype=self.dtype)
        function(goodvar, 0)


    def StdTestThrowUnlessReturnedNArrayIsWritable(self, function):
        # Expect the returned array to be writable
        sizes = self.HelperRandSizes(self.CObject.Dim())
        sizes = numpy.array(sizes, dtype=self.size_type)
        v = function(sizes)
        assert(v.flags['WRITEABLE'] == True)


    def StdTestThrowUnlessReturnedNArrayIsNonWritable(self, function):
        # Expect the returned array to be non-writable
        sizes = self.HelperRandSizes(self.CObject.Dim())
        sizes = numpy.array(sizes, dtype=self.size_type)
        v = function(sizes)
        assert(v.flags['WRITEABLE'] == False)


    # Test if the C object reads the vector correctly
    def SpecTestThrowUnlessReadsCorrectly(self, function):
        ndim = self.CObject.Dim()

        sizes = self.HelperRandSizes(ndim)
        vNew = numpy.random.random_sample(sizes)
        vNew = numpy.floor(vNew * 100)
        vNew = numpy.array(vNew, dtype=self.dtype)
        vOld = copy.deepcopy(vNew)

        vShape = numpy.array(sizes)
        vSize = vNew.size

        function(vNew, 0)

        cShape = numpy.ones(ndim, dtype=self.size_type)
        self.CObject.sizes(cShape)

        assert((cShape == vShape).all())
        assert((vNew.shape == vShape).all())
        for i in range(vSize):
            indexTuple = self.HelperMetaIndexToTuple(i, vShape)

            # Test if the C object read the vector correctly
            assert(self.CObject.GetItem(i) == vOld[indexTuple])
            # Test that the C object did not modify the vector
            assert(vNew[indexTuple] == vOld[indexTuple])


    # Test if the C object reads and modifies the vector correctly
    def SpecTestThrowUnlessReadsWritesCorrectly(self, function):
        ndim = self.CObject.Dim()

        sizes = self.HelperRandSizes(ndim)
        vNew = numpy.random.random_sample(sizes)
        vNew = numpy.floor(vNew * 100)
        vNew = numpy.array(vNew, dtype=self.dtype)
        vOld = copy.deepcopy(vNew)

        vShape = numpy.array(sizes)
        vSize = vNew.size

        function(vNew, 0)

        cShape = numpy.ones(ndim, dtype=self.size_type)
        self.CObject.sizes(cShape)

        assert((cShape == vShape).all())
        assert((vNew.shape == vShape).all())
        for i in range(vSize):
            indexTuple = self.HelperMetaIndexToTuple(i, vShape)

            # Test if the C object read the vector correctly
            assert(self.CObject.GetItem(i) == vOld[indexTuple])
            # Test if the C object modified the vector correctly
            assert(vNew[indexTuple] == vOld[indexTuple] + 1)


    # Test if the C object resizes the vector correctly
    def SpecTestThrowUnlessResizesCorrectly(self, function):
        ndim = self.CObject.Dim()

        sizes = self.HelperRandSizes(ndim)
        vNew = numpy.random.random_sample(sizes)
        vNew = numpy.floor(vNew * 100)
        vNew = numpy.array(vNew, dtype=self.dtype)
        vOld = copy.deepcopy(vNew)

        vShape = numpy.array(sizes)
        vSize = vNew.size

        SIZE_FACTOR = 3

        function(vNew, SIZE_FACTOR)

        cShape = numpy.ones(ndim, dtype=self.size_type)
        self.CObject.sizes(cShape)

        assert((cShape == vShape).all())
        vShapeNew = numpy.array(vNew.shape)
        assert((vShapeNew == vShape * SIZE_FACTOR).all())
        for i in range(vNew.size):
            indexTuple = self.HelperMetaIndexToTuple(i, vShape)

            # Test if the C object resized the vector correctly
            assert(vNew[indexTuple] == 17)


    # Test if the C object returns a good vector
    def SpecTestThrowUnlessReceivesCorrectNArray(self, function):
        ndim = self.CObject.Dim()
        sizes = self.HelperRandSizes(self.CObject.Dim())
        sizes = numpy.array(sizes, dtype=self.size_type)

        v = function(sizes)
        vShape = v.shape
        vSize = v.size

        for i in range(ndim):
            assert(vShape[i] == sizes[i]);

        for i in range(vSize):
            indexTuple = self.HelperMetaIndexToTuple(i, vShape)
            assert(self.CObject.GetItem(i) == v[indexTuple])


    ###########################################################################
    #   TEST FUNCTIONS - Put any new unit test here!  These are the unit      #
    #   tests that test the typemaps.  One test per typemap!                  #
    ###########################################################################

    def Test_in_argout_vctDynamicNArray_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()
            self.SetSizeType()

            function = self.CObject.in_argout_vctDynamicNArray_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimensionN(function)
            self.StdTestThrowUnlessWritable(function)
            self.StdTestThrowUnlessOwnsData(function)
            self.StdTestThrowUnlessNotReferenced(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsWritesCorrectly(function)
            self.SpecTestThrowUnlessResizesCorrectly(function)


    def Test_in_vctDynamicNArrayRef(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()
            self.SetSizeType()

            function = self.CObject.in_vctDynamicNArrayRef

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimensionN(function)
            self.StdTestThrowUnlessWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsWritesCorrectly(function)


    def Test_in_vctDynamicConstNArrayRef(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()
            self.SetSizeType()

            function = self.CObject.in_vctDynamicConstNArrayRef

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimensionN(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsCorrectly(function)


    def Test_in_argout_const_vctDynamicConstNArrayRef_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()
            self.SetSizeType()

            function = self.CObject.in_argout_const_vctDynamicConstNArrayRef_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimensionN(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsCorrectly(function)


    def Test_in_argout_const_vctDynamicNArrayRef_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()
            self.SetSizeType()

            function = self.CObject.in_argout_const_vctDynamicNArrayRef_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimensionN(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsCorrectly(function)


    def Test_in_vctDynamicNArray(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()
            self.SetSizeType()

            function = self.CObject.in_vctDynamicNArray

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimensionN(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsCorrectly(function)


    def Test_in_argout_const_vctDynamicNArray_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()
            self.SetSizeType()

            function = self.CObject.in_argout_const_vctDynamicNArray_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessIsArray(function)
            self.StdTestThrowUnlessDataType(function)
            self.StdTestThrowUnlessDimensionN(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReadsCorrectly(function)


    def Test_out_vctDynamicNArray(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()
            self.SetSizeType()

            function = self.CObject.out_vctDynamicNArray

            # Perform battery of standard tests
            self.StdTestThrowUnlessReturnedNArrayIsWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReceivesCorrectNArray(function)


    def Test_out_vctDynamicNArray_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()
            self.SetSizeType()

            function = self.CObject.out_vctDynamicNArray_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessReturnedNArrayIsWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReceivesCorrectNArray(function)


    def Test_out_const_vctDynamicNArray_ref(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()
            self.SetSizeType()

            function = self.CObject.out_const_vctDynamicNArray_ref

            # Perform battery of standard tests
            self.StdTestThrowUnlessReturnedNArrayIsNonWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReceivesCorrectNArray(function)


    def Test_out_vctDynamicNArrayRef(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()
            self.SetSizeType()

            function = self.CObject.out_vctDynamicNArrayRef

            # Perform battery of standard tests
            self.StdTestThrowUnlessReturnedNArrayIsWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReceivesCorrectNArray(function)


    def Test_out_vctDynamicConstNArrayRef(self):
        for (dtype, CObject) in self.types.items():
            self.dtype = dtype
            self.CObject = CObject()
            self.SetSizeType()

            function = self.CObject.out_vctDynamicConstNArrayRef

            # Perform battery of standard tests
            self.StdTestThrowUnlessReturnedNArrayIsNonWritable(function)

            # Perform specialized tests
            self.SpecTestThrowUnlessReceivesCorrectNArray(function)
