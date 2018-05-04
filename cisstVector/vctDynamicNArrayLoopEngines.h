/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Daniel Li, Ofri Sadowsky, Anton Deguet
  Created on: 2006-07-05

  (C) Copyright 2006-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctDynamicNArrayLoopEngines_h
#define _vctDynamicNArrayLoopEngines_h

/*!
  \file
  \brief Declaration of vctDynamicNArrayLoopEngines
 */

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnThrow.h>

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctContainerTraits.h>
#include <cisstVector/vctDynamicCompactLoopEngines.h>

/*!
  \brief Container class for the dynamic nArray engines.

  \sa SoNi SoNiNi SoNiSi NoNiNi NoNiSi NoSiNi NioSi NioNi NoNi Nio NioSiNi MinAndMax
*/
template <vct::size_type _dimension>
class vctDynamicNArrayLoopEngines
{
public:
    /* define types */
    typedef vct::size_type size_type;
    typedef vct::stride_type stride_type;
    typedef vct::difference_type difference_type;
    typedef vct::index_type index_type;

    VCT_NARRAY_TRAITS_TYPEDEFS(_dimension);

    /*! Helper function to throw an exception whenever sizes mismatch.
      This enforces that a standard message is sent. */
    inline static void ThrowSizeMismatchException(void) CISST_THROW(std::runtime_error)
    {
        cmnThrow(std::runtime_error("vctDynamicNArrayLoopEngines: Sizes of nArrays don't match"));
    }


    /*! Helper function to calculate the strides to next dimension. */
    inline static void CalculateSTND(nstride_type & stnd,
                                     const nsize_type & sizes,
                                     const nstride_type & strides)
    {
        // set up iterators
        typename nsize_type::const_iterator sizesIter = sizes.begin();
        typename nstride_type::const_iterator stridesIter = strides.begin();
        typename nstride_type::iterator stndIter = stnd.begin();
        const typename nstride_type::const_iterator stndIterEnd = stnd.end();

        *stndIter = 0;
        ++sizesIter;
        ++stridesIter;
        ++stndIter;

        stride_type skippedStrides;
        for (;
             stndIter != stndIterEnd;
             ++sizesIter, ++stridesIter, ++stndIter)
        {
            skippedStrides = static_cast<stride_type>(*sizesIter) * (*stridesIter);
            *stndIter = *(stridesIter-1) - skippedStrides;
        }
    }


    /*! Helper function to calculate the pointer offsets to the next dimension. */
    inline static void CalculateOTND(nstride_type & otnd,
                                     const nstride_type & strides,
                                     const nstride_type & stnd)
    {
        // set up iterators
        stride_type previousOTND = *(strides.rbegin());
        typename nstride_type::const_reverse_iterator stndIter = stnd.rbegin();
        typename nstride_type::reverse_iterator otndIter = otnd.rbegin();
        const typename nstride_type::const_reverse_iterator otnd_rend = otnd.rend();

        *otndIter = previousOTND;
        ++otndIter;

        for (;
             otndIter != otnd_rend;
             ++otndIter, ++stndIter)
        {
            *otndIter = *stndIter + previousOTND;
            previousOTND = *otndIter;
        }
    }


    /*! Helper function to calculate the initial targets. */
    template <class _elementType>
    inline static void InitializeTargets(vctFixedSizeVector<const _elementType *, _dimension> & targets,
                                         const nsize_type & sizes,
                                         const nstride_type & strides,
                                         const _elementType * basePtr)
    {
        typedef _elementType value_type;

        // set up iterators
        typename nsize_type::const_iterator sizesIter = sizes.begin();
        typename nstride_type::const_iterator stridesIter = strides.begin();
        typename vctFixedSizeVector<const value_type *, _dimension>::iterator targetsIter = targets.begin();
        const typename vctFixedSizeVector<const value_type *, _dimension>::const_iterator targetsIterEnd = targets.end();

        stride_type offset;
        for (;
             targetsIter != targetsIterEnd;
             ++targetsIter, ++sizesIter, ++stridesIter)
        {
            offset = static_cast<stride_type>(*sizesIter) * (*stridesIter);
            *targetsIter = basePtr + offset;
        }
    }


    /*! Helper function to synchronize the given nArray's current pointer
      with the master nArray's current pointer. */
    template <class _elementType>
    inline static void SyncCurrentPointer(const _elementType * & currentPointer,
                                          const nstride_type & otnd,
                                          difference_type numberOfWrappedDimensions)
    {
        const typename nstride_type::const_reverse_iterator otndBegin = otnd.rbegin();
        currentPointer += otndBegin[numberOfWrappedDimensions];
    }


    /*! Helper function to increment the current pointer and any necessary
      target pointers. */
    template <class _elementType, class _pointerType>
    inline static dimension_type IncrementPointers(vctFixedSizeVector<const _elementType *, _dimension> & targets,
                                                    _pointerType & currentPointer,
                                                    const nstride_type & strides,
                                                    const nstride_type & stnd)
    {
        typedef _elementType value_type;

        // set up iterators
        typename vctFixedSizeVector<const value_type *, _dimension>::reverse_iterator targetsIter = targets.rbegin();
        const typename vctFixedSizeVector<const value_type *, _dimension>::const_reverse_iterator targets_rbeg = targets.rbegin();
        // typename vctFixedSizeVector<const value_type *, _dimension>::reverse_iterator targets_innerIter;
        typename nstride_type::const_reverse_iterator stridesIter = strides.rbegin();
        // const typename nstride_type::const_reverse_iterator strides_rend = strides.rend();
        typename nstride_type::const_reverse_iterator stndIter = stnd.rbegin();
        // typename nstride_type::const_reverse_iterator stnd_innerIter;
        dimension_type numberOfWrappedDimensions = 0;

        //* Below is Ofri's code
        // Consider the following structure.
        //
        // initial target setup :    // performed before engine loop is begun, outside of this function
        //       currPtr = base;
        //       for each k
        //           target[k] = currPtr + size[k] * stride[k];  // This should be invariant after every dimension wrap
        //           stnd[k] = stride[k-1] - size[k] * stride[k];   // This is constant through the run
        //
        // wrap(nw)  :      // nw is the number of dimensions wrapped
        //       currPtr += sum_{i =(d-nw)}^{d-1}( stnd[i] );  // obsolete if currPtr already determined; see below
        //       for k = (d-nw) to (d-1)
        //           target[k] = currPtr + size[k] * stride[k];
        //
        // check_wrap :     // this updates currentPtr (see above) and finds out nw (number of wrapped dimensions)
        //       k = d-1;
        //       nw = 0;
        //       currPtr += stride[k];
        //       while (currPtr == target[k]) {
        //           currPtr += stnd[k];
        //           --k;
        //           ++nw;
        //       }
        //       return nw;
        //
        // NOTE:  The invariant condition above and the initialization of stnd leads to
        //       size[k] * stride[k] = stride[k-1] - stnd[k];

        currentPointer += *stridesIter;
        while (currentPointer == *targetsIter) {
            currentPointer += *stndIter;
            ++targetsIter;
            ++stndIter;
            ++numberOfWrappedDimensions;
            if (numberOfWrappedDimensions == _dimension)
                return numberOfWrappedDimensions;
        }

        if (numberOfWrappedDimensions == 0)
            return numberOfWrappedDimensions;

        stridesIter += numberOfWrappedDimensions;
        difference_type targetOffset;
        do {
            --targetsIter;
            --stndIter;
            targetOffset = *stridesIter - *stndIter;
            *targetsIter = currentPointer + targetOffset;
            --stridesIter;
        } while (targetsIter != targets_rbeg);
        return numberOfWrappedDimensions;
    }


    template <class _incrementalOperationType, class _elementOperationType>
    class SoNi
    {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;

        template <class _inputNArrayType>
        static OutputType Run(const _inputNArrayType & inputNArray)
        {
            typedef _inputNArrayType InputNArrayType;
            typedef typename InputNArrayType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owners
            const InputOwnerType & inputOwner = inputNArray.Owner();

            // if compact
            if (inputOwner.IsCompact()) {
                return vctDynamicCompactLoopEngines::SoCi<_incrementalOperationType, _elementOperationType>::Run(inputOwner);
            } else {
                // declare all variables used for inputOwner
                const nsize_type & inputSizes = inputOwner.sizes();
                const nstride_type & inputStrides = inputOwner.strides();
                nstride_type inputSTND;
                vctFixedSizeVector<InputPointerType, _dimension> inputTargets;
                InputPointerType inputPointer = inputOwner.Pointer();

                dimension_type numberOfWrappedDimensions = 0;
                const dimension_type maxWrappedDimensions = inputOwner.dimension();

                CalculateSTND(inputSTND, inputSizes, inputStrides);
                InitializeTargets(inputTargets, inputSizes, inputStrides, inputPointer);

                OutputType incrementalResult = _incrementalOperationType::NeutralElement();

                while (numberOfWrappedDimensions != maxWrappedDimensions) {
                    incrementalResult =
                        _incrementalOperationType::Operate(incrementalResult,
                                                           _elementOperationType::Operate(*inputPointer) );

                    numberOfWrappedDimensions =
                        IncrementPointers(inputTargets, inputPointer, inputStrides, inputSTND);
                }
                return incrementalResult;
            }
        }   // Run method
    };  // SoNi class


    template <class _incrementalOperationType, class _elementOperationType>
    class SoNiNi
    {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;

        template <class _input1NArrayType, class _input2NArrayType>
        static OutputType Run(const _input1NArrayType & input1NArray,
                              const _input2NArrayType & input2NArray)
        {
            typedef _input1NArrayType Input1NArrayType;
            typedef typename Input1NArrayType::OwnerType Input1OwnerType;
            typedef typename Input1OwnerType::const_pointer Input1PointerType;

            typedef _input2NArrayType Input2NArrayType;
            typedef typename Input2NArrayType::OwnerType Input2OwnerType;
            typedef typename Input2OwnerType::const_pointer Input2PointerType;

            // retrieve owners
            const Input1OwnerType & input1Owner = input1NArray.Owner();
            const Input2OwnerType & input2Owner = input2NArray.Owner();

            // check sizes
            const nsize_type & input1Sizes = input1Owner.sizes();
            const nsize_type & input2Sizes = input2Owner.sizes();
            if (input1Sizes.NotEqual(input2Sizes)) {
                ThrowSizeMismatchException();
            }

            // if compact and same strides
            const nstride_type & input1Strides = input1Owner.strides();
            const nstride_type & input2Strides = input2Owner.strides();

            if (input1Owner.IsCompact() && input2Owner.IsCompact()
                && (input1Owner.strides() == input2Owner.strides())) {
                return vctDynamicCompactLoopEngines::SoCiCi<_incrementalOperationType, _elementOperationType>::Run(input1Owner, input2Owner);
            } else {
                // otherwise
                // declare all variables used for input1Owner
                nstride_type input1STND;
                vctFixedSizeVector<Input1PointerType, _dimension> input1Targets;
                Input1PointerType input1Pointer = input1Owner.Pointer();

                // declare all variables used for input2Owner
                nstride_type input2STND;
                nstride_type input2OTND;
                Input2PointerType input2Pointer = input2Owner.Pointer();

                dimension_type numberOfWrappedDimensions = 0;
                const dimension_type maxWrappedDimensions = input1Owner.dimension();

                CalculateSTND(input1STND, input1Sizes, input1Strides);
                CalculateSTND(input2STND, input2Sizes, input2Strides);
                CalculateOTND(input2OTND, input2Strides, input2STND);
                InitializeTargets(input1Targets, input1Sizes, input1Strides, input1Pointer);

                OutputType incrementalResult = _incrementalOperationType::NeutralElement();

                while (numberOfWrappedDimensions != maxWrappedDimensions) {
                    incrementalResult =
                        _incrementalOperationType::Operate(incrementalResult,
                                                           _elementOperationType::Operate(*input1Pointer, *input2Pointer) );

                    numberOfWrappedDimensions =
                        IncrementPointers(input1Targets, input1Pointer, input1Strides, input1STND);

                    SyncCurrentPointer(input2Pointer, input2OTND, numberOfWrappedDimensions);
                }
                return incrementalResult;
            }
        }   // Run method
    };  // SoNiNi class


    template <class _incrementalOperationType, class _elementOperationType>
    class SoNiSi
    {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;

        template <class _inputNArrayType, class _inputScalarType>
        static OutputType Run(const _inputNArrayType & inputNArray,
                              const _inputScalarType inputScalar)
        {
            typedef _inputNArrayType InputNArrayType;
            typedef typename InputNArrayType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owners
            const InputOwnerType & inputOwner = inputNArray.Owner();

            // if compact
            if (inputOwner.IsCompact()) {
                return vctDynamicCompactLoopEngines::SoCiSi<_incrementalOperationType, _elementOperationType>::Run(inputOwner, inputScalar);
            } else {
                // declare all variables used for inputOwner
                const nsize_type & inputSizes = inputOwner.sizes();
                const nstride_type & inputStrides = inputOwner.strides();
                nstride_type inputSTND;
                vctFixedSizeVector<InputPointerType, _dimension> inputTargets;
                InputPointerType inputPointer = inputOwner.Pointer();

                dimension_type numberOfWrappedDimensions = 0;
                const dimension_type maxWrappedDimensions = inputOwner.dimension();

                CalculateSTND(inputSTND, inputSizes, inputStrides);
                InitializeTargets(inputTargets, inputSizes, inputStrides, inputPointer);

                OutputType incrementalResult = _incrementalOperationType::NeutralElement();

                while (numberOfWrappedDimensions != maxWrappedDimensions) {
                    incrementalResult =
                        _incrementalOperationType::Operate(incrementalResult,
                                                           _elementOperationType::Operate(*inputPointer, inputScalar) );

                    numberOfWrappedDimensions =
                        IncrementPointers(inputTargets, inputPointer, inputStrides, inputSTND);
                }
                return incrementalResult;
            }
        }   // Run method
    };  // SoNiSi class


    template <class _elementOperationType>
    class NoNiNi
    {
    public:
        template <class _outputNArrayType, class _input1NArrayType, class _input2NArrayType>
        static void Run(_outputNArrayType & outputNArray,
                        const _input1NArrayType & input1NArray,
                        const _input2NArrayType & input2NArray)
        {
            typedef _outputNArrayType OutputNArrayType;
            typedef typename OutputNArrayType::OwnerType OutputOwnerType;
            typedef typename OutputOwnerType::pointer OutputPointerType;
            typedef typename OutputOwnerType::const_pointer OutputConstPointerType;

            typedef _input1NArrayType Input1NArrayType;
            typedef typename Input1NArrayType::OwnerType Input1OwnerType;
            typedef typename Input1OwnerType::const_pointer Input1PointerType;

            typedef _input2NArrayType Input2NArrayType;
            typedef typename Input2NArrayType::OwnerType Input2OwnerType;
            typedef typename Input2OwnerType::const_pointer Input2PointerType;

            // retrieve owners
            OutputOwnerType & outputOwner = outputNArray.Owner();
            const Input1OwnerType & input1Owner = input1NArray.Owner();
            const Input2OwnerType & input2Owner = input2NArray.Owner();

            // check sizes
            const nsize_type & outputSizes = outputOwner.sizes();
            const nsize_type & input1Sizes = input1Owner.sizes();
            const nsize_type & input2Sizes = input2Owner.sizes();
            if (outputSizes.NotEqual(input1Sizes) || outputSizes.NotEqual(input2Sizes)) {
                ThrowSizeMismatchException();
            }

            // if compact and same strides
            const nstride_type & outputStrides = outputOwner.strides();
            const nstride_type & input1Strides = input1Owner.strides();
            const nstride_type & input2Strides = input2Owner.strides();

            if (outputOwner.IsCompact() && input1Owner.IsCompact() && input2Owner.IsCompact()
                && (outputOwner.strides() == input1Owner.strides())
                && (outputOwner.strides() == input2Owner.strides())) {
                vctDynamicCompactLoopEngines::CoCiCi<_elementOperationType>::Run(outputOwner, input1Owner, input2Owner);
            } else {
                // otherwise
                // declare all variables used for outputOwner
                nstride_type outputSTND;
                vctFixedSizeVector<OutputConstPointerType, _dimension> outputTargets;
                OutputPointerType outputPointer = outputOwner.Pointer();

                // declare all variables used for input1Owner
                nstride_type input1STND;
                nstride_type input1OTND;
                Input1PointerType input1Pointer = input1Owner.Pointer();

                // declare all variables used for input2Owner
                const nsize_type & input2Sizes = input2Owner.sizes();
                nstride_type input2STND;
                nstride_type input2OTND;
                Input2PointerType input2Pointer = input2Owner.Pointer();

                dimension_type numberOfWrappedDimensions = 0;
                const dimension_type maxWrappedDimensions = outputOwner.dimension();

                CalculateSTND(outputSTND, outputSizes, outputStrides);
                CalculateSTND(input1STND, input1Sizes, input1Strides);
                CalculateSTND(input2STND, input2Sizes, input2Strides);
                CalculateOTND(input1OTND, input1Strides, input1STND);
                CalculateOTND(input2OTND, input2Strides, input2STND);
                InitializeTargets(outputTargets, outputSizes, outputStrides, outputPointer);

                while (numberOfWrappedDimensions != maxWrappedDimensions) {
                    *outputPointer = _elementOperationType::Operate(*input1Pointer, *input2Pointer);

                    numberOfWrappedDimensions =
                        IncrementPointers(outputTargets, outputPointer, outputStrides, outputSTND);

                    SyncCurrentPointer(input1Pointer, input1OTND, numberOfWrappedDimensions);
                    SyncCurrentPointer(input2Pointer, input2OTND, numberOfWrappedDimensions);
                }
            }
        }   // Run method
    };  // NoNiNi class


    template <class _elementOperationType>
    class NoNiSi
    {
    public:
        template <class _outputNArrayType, class _inputNArrayType, class _inputScalarType>
        static void Run(_outputNArrayType & outputNArray,
                        const _inputNArrayType & inputNArray,
                        const _inputScalarType inputScalar)
        {
            typedef _outputNArrayType OutputNArrayType;
            typedef typename OutputNArrayType::OwnerType OutputOwnerType;
            typedef typename OutputOwnerType::pointer OutputPointerType;
            typedef typename OutputOwnerType::const_pointer OutputConstPointerType;

            typedef _inputNArrayType InputNArrayType;
            typedef typename InputNArrayType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owners
            OutputOwnerType & outputOwner = outputNArray.Owner();
            const InputOwnerType & inputOwner = inputNArray.Owner();

            // check sizes
            const nsize_type & outputSizes = outputOwner.sizes();
            const nsize_type & inputSizes = inputOwner.sizes();
            if (outputSizes.NotEqual(inputSizes)) {
                ThrowSizeMismatchException();
            }

            // if compact and same strides
            const nstride_type & outputStrides = outputOwner.strides();
            const nstride_type & inputStrides = inputOwner.strides();

            if (outputOwner.IsCompact() && inputOwner.IsCompact()
                && (outputOwner.strides() == inputOwner.strides())) {
                vctDynamicCompactLoopEngines::CoCiSi<_elementOperationType>::Run(outputOwner, inputOwner, inputScalar);
            } else {
                // otherwise
                // declare all variables used for outputOwner
                nstride_type outputSTND;
                vctFixedSizeVector<OutputConstPointerType, _dimension> outputTargets;
                OutputPointerType outputPointer = outputOwner.Pointer();

                // declare all variables used for inputOwner
                nstride_type inputSTND;
                nstride_type inputOTND;
                InputPointerType inputPointer = inputOwner.Pointer();

                dimension_type numberOfWrappedDimensions = 0;
                const dimension_type maxWrappedDimensions = outputOwner.dimension();

                CalculateSTND(outputSTND, outputSizes, outputStrides);
                CalculateSTND(inputSTND, inputSizes, inputStrides);
                CalculateOTND(inputOTND, inputStrides, inputSTND);
                InitializeTargets(outputTargets, outputSizes, outputStrides, outputPointer);

                while (numberOfWrappedDimensions != maxWrappedDimensions) {
                    *outputPointer = _elementOperationType::Operate(*inputPointer, inputScalar);

                    numberOfWrappedDimensions =
                        IncrementPointers(outputTargets, outputPointer, outputStrides, outputSTND);

                    SyncCurrentPointer(inputPointer, inputOTND, numberOfWrappedDimensions);
                }
            }
        }   // Run method
    };  // NoNiSi class


    template <class _elementOperationType>
    class NoSiNi
    {
    public:
        template <class _outputNArrayType, class _inputScalarType, class _inputNArrayType>
        static void Run(_outputNArrayType & outputNArray,
                        const _inputScalarType inputScalar,
                        const _inputNArrayType & inputNArray)
        {
            typedef _outputNArrayType OutputNArrayType;
            typedef typename OutputNArrayType::OwnerType OutputOwnerType;
            typedef typename OutputOwnerType::pointer OutputPointerType;
            typedef typename OutputOwnerType::const_pointer OutputConstPointerType;

            typedef _inputNArrayType InputNArrayType;
            typedef typename InputNArrayType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owners
            OutputOwnerType & outputOwner = outputNArray.Owner();
            const InputOwnerType & inputOwner = inputNArray.Owner();

            // check sizes
            const nsize_type & outputSizes = outputOwner.sizes();
            const nsize_type & inputSizes = inputOwner.sizes();
            if (outputSizes.NotEqual(inputSizes)) {
                ThrowSizeMismatchException();
            }

            // if compact and same strides
            const nstride_type & outputStrides = outputOwner.strides();
            const nstride_type & inputStrides = inputOwner.strides();

            if (outputOwner.IsCompact() && inputOwner.IsCompact()
                && (outputOwner.strides() == inputOwner.strides())) {
                vctDynamicCompactLoopEngines::CoSiCi<_elementOperationType>::Run(outputOwner, inputScalar, inputOwner);
            } else {
                // otherwise
                // declare all variables used for outputNArray
                nstride_type outputSTND;
                vctFixedSizeVector<OutputConstPointerType, _dimension> outputTargets;
                OutputPointerType outputPointer = outputNArray.Pointer();

                // declare all variables used for inputNArray
                nstride_type inputSTND;
                nstride_type inputOTND;
                InputPointerType inputPointer = inputNArray.Pointer();

                dimension_type numberOfWrappedDimensions = 0;
                const dimension_type maxWrappedDimensions = outputNArray.dimension();

                CalculateSTND(outputSTND, outputSizes, outputStrides);
                CalculateSTND(inputSTND, inputSizes, inputStrides);
                CalculateOTND(inputOTND, inputStrides, inputSTND);
                InitializeTargets(outputTargets, outputSizes, outputStrides, outputPointer);

                while (numberOfWrappedDimensions != maxWrappedDimensions) {
                    *outputPointer = _elementOperationType::Operate(inputScalar, *inputPointer);

                    numberOfWrappedDimensions =
                        IncrementPointers(outputTargets, outputPointer, outputStrides, outputSTND);

                    SyncCurrentPointer(inputPointer, inputOTND, numberOfWrappedDimensions);
                }
            }
        }   // Run method
    };  // NoSiNi class


    template <class _elementOperationType>
    class NioSi
    {
    public:
        template <class _inputOutputNArrayType, class _inputScalarType>
        static void Run(_inputOutputNArrayType & inputOutputNArray,
                        const _inputScalarType inputScalar)
        {
            typedef _inputOutputNArrayType InputOutputNArrayType;
            typedef typename InputOutputNArrayType::OwnerType InputOutputOwnerType;
            typedef typename InputOutputOwnerType::const_pointer InputOutputConstPointerType;
            typedef typename InputOutputOwnerType::pointer InputOutputPointerType;

            // retrieve owners
            InputOutputOwnerType & inputOutputOwner = inputOutputNArray.Owner();

            // if compact
            if (inputOutputOwner.IsCompact()) {
                vctDynamicCompactLoopEngines::CioSi<_elementOperationType>::Run(inputOutputOwner, inputScalar);
            } else {
                // declare all variables used for inputOwner
                const nsize_type & inputOutputSizes = inputOutputOwner.sizes();
                const nstride_type & inputOutputStrides = inputOutputOwner.strides();
                nstride_type inputOutputSTND;
                vctFixedSizeVector<InputOutputConstPointerType, _dimension> inputOutputTargets;
                InputOutputPointerType inputOutputPointer = inputOutputOwner.Pointer();

                dimension_type numberOfWrappedDimensions = 0;
                const dimension_type maxWrappedDimensions = inputOutputOwner.dimension();

                CalculateSTND(inputOutputSTND, inputOutputSizes, inputOutputStrides);
                InitializeTargets(inputOutputTargets, inputOutputSizes, inputOutputStrides, inputOutputPointer);

                while (numberOfWrappedDimensions != maxWrappedDimensions) {
                    _elementOperationType::Operate(*inputOutputPointer, inputScalar);

                    numberOfWrappedDimensions =
                        IncrementPointers(inputOutputTargets, inputOutputPointer, inputOutputStrides, inputOutputSTND);
                }
            }
        }   // Run method
    };  // NioSi class


    template <class _elementOperationType>
    class NioNi
    {
    public:
        template <class _inputOutputNArrayType, class _inputNArrayType>
        static void Run(_inputOutputNArrayType & inputOutputNArray,
                        const _inputNArrayType & inputNArray)
        {
            typedef _inputOutputNArrayType InputOutputNArrayType;
            typedef typename InputOutputNArrayType::OwnerType InputOutputOwnerType;
            typedef typename InputOutputOwnerType::const_pointer InputOutputConstPointerType;
            typedef typename InputOutputOwnerType::pointer InputOutputPointerType;

            typedef _inputNArrayType InputNArrayType;
            typedef typename InputNArrayType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owners
            InputOutputOwnerType & inputOutputOwner = inputOutputNArray.Owner();
            const InputOwnerType & inputOwner = inputNArray.Owner();

            // check sizes
            const nsize_type & inputOutputSizes = inputOutputOwner.sizes();
            const nsize_type & inputSizes = inputOwner.sizes();
            if (inputOutputSizes.NotEqual(inputSizes)) {
                ThrowSizeMismatchException();
            }

            // if compact and same strides
            const nstride_type & inputOutputStrides = inputOutputOwner.strides();
            const nstride_type & inputStrides = inputOwner.strides();

            if (inputOutputOwner.IsCompact() && inputOwner.IsCompact()
                && (inputOutputOwner.strides() == inputOwner.strides())) {
                vctDynamicCompactLoopEngines::CioCi<_elementOperationType>::Run(inputOutputOwner, inputOwner);
            } else {
                // otherwise
                // declare all variables used for inputOutputOwner
                nstride_type inputOutputSTND;
                vctFixedSizeVector<InputOutputConstPointerType, _dimension> inputOutputTargets;
                InputOutputPointerType inputOutputPointer = inputOutputOwner.Pointer();

                // declare all variables used for inputOwner
                nstride_type inputSTND;
                nstride_type inputOTND;
                InputPointerType inputPointer = inputOwner.Pointer();

                dimension_type numberOfWrappedDimensions = 0;
                const dimension_type maxWrappedDimensions = inputOutputOwner.dimension();

                CalculateSTND(inputOutputSTND, inputOutputSizes, inputOutputStrides);
                CalculateSTND(inputSTND, inputSizes, inputStrides);
                CalculateOTND(inputOTND, inputStrides, inputSTND);
                InitializeTargets(inputOutputTargets, inputOutputSizes, inputOutputStrides, inputOutputPointer);

                while (numberOfWrappedDimensions != maxWrappedDimensions) {
                    _elementOperationType::Operate(*inputOutputPointer, *inputPointer);

                    numberOfWrappedDimensions =
                        IncrementPointers(inputOutputTargets, inputOutputPointer, inputOutputStrides, inputOutputSTND);

                    SyncCurrentPointer(inputPointer, inputOTND, numberOfWrappedDimensions);
                }
            }
        }   // Run method
    };  // NioNi class


    template <class _elementOperationType>
    class NoNi
    {
    public:
        template <class _outputNArrayType, class _inputNArrayType>
        static inline void Run(_outputNArrayType & outputNArray,
                               const _inputNArrayType & inputNArray)
        {
            typedef _outputNArrayType OutputNArrayType;
            typedef typename OutputNArrayType::OwnerType OutputOwnerType;
            typedef typename OutputOwnerType::const_pointer OutputConstPointerType;
            typedef typename OutputOwnerType::pointer OutputPointerType;

            typedef _inputNArrayType InputNArrayType;
            typedef typename InputNArrayType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owners
            OutputOwnerType & outputOwner = outputNArray.Owner();
            const InputOwnerType & inputOwner = inputNArray.Owner();

            // check sizes
            const nsize_type & outputSizes = outputOwner.sizes();
            const nsize_type & inputSizes = inputOwner.sizes();
            if (inputSizes.NotEqual(outputSizes)) {
                ThrowSizeMismatchException();
            }

            // if compact and same strides
            const nstride_type & outputStrides = outputOwner.strides();
            const nstride_type & inputStrides = inputOwner.strides();

            if (outputOwner.IsCompact() && inputOwner.IsCompact()
                && (outputOwner.strides() == inputOwner.strides())) {
                vctDynamicCompactLoopEngines::CoCi<_elementOperationType>::Run(outputOwner, inputOwner);
            } else {
                // otherwise
                // declare all variables used for outputNArray
                nstride_type outputSTND;
                vctFixedSizeVector<OutputConstPointerType, _dimension> outputTargets;
                OutputPointerType outputPointer = outputOwner.Pointer();

                // declare all variables used for inputNArray
                nstride_type inputSTND;
                nstride_type inputOTND;
                InputPointerType inputPointer = inputOwner.Pointer();

                dimension_type numberOfWrappedDimensions = 0;
                const dimension_type maxWrappedDimensions = outputOwner.dimension();

                CalculateSTND(outputSTND, outputSizes, outputStrides);
                CalculateSTND(inputSTND, inputSizes, inputStrides);
                CalculateOTND(inputOTND, inputStrides, inputSTND);
                InitializeTargets(outputTargets, outputSizes, outputStrides, outputPointer);

                while (numberOfWrappedDimensions != maxWrappedDimensions) {
                    *outputPointer = _elementOperationType::Operate(*inputPointer);

                    numberOfWrappedDimensions =
                        IncrementPointers(outputTargets, outputPointer, outputStrides, outputSTND);

                    SyncCurrentPointer(inputPointer, inputOTND, numberOfWrappedDimensions);
                }
            }
        }   // Run method
    };  // NoNi class


    template <class _elementOperationType>
    class Nio
    {
    public:
        template <class _inputOutputNArrayType>
        static void Run(_inputOutputNArrayType & inputOutputNArray)
        {
            typedef _inputOutputNArrayType InputOutputNArrayType;
            typedef typename InputOutputNArrayType::OwnerType InputOutputOwnerType;
            typedef typename InputOutputOwnerType::const_pointer InputOutputConstPointerType;
            typedef typename InputOutputOwnerType::pointer InputOutputPointerType;

            // retrieve owners
            InputOutputOwnerType & inputOutputOwner = inputOutputNArray.Owner();

            // if compact and same strides
            const nstride_type & inputOutputStrides = inputOutputOwner.strides();

            if (inputOutputOwner.IsCompact()) {
                vctDynamicCompactLoopEngines::Cio<_elementOperationType>::Run(inputOutputOwner);
            } else {
                // otherwise
                const nsize_type & inputOutputSizes = inputOutputOwner.sizes();
                nstride_type inputOutputSTND;
                vctFixedSizeVector<InputOutputConstPointerType, _dimension> inputOutputTargets;
                InputOutputPointerType inputOutputPointer = inputOutputOwner.Pointer();

                dimension_type numberOfWrappedDimensions = 0;
                const dimension_type maxWrappedDimensions = inputOutputOwner.dimension();

                CalculateSTND(inputOutputSTND, inputOutputSizes, inputOutputStrides);
                InitializeTargets(inputOutputTargets, inputOutputSizes, inputOutputStrides, inputOutputPointer);

                while (numberOfWrappedDimensions != maxWrappedDimensions) {
                    _elementOperationType::Operate(*inputOutputPointer);

                    numberOfWrappedDimensions =
                        IncrementPointers(inputOutputTargets, inputOutputPointer, inputOutputStrides, inputOutputSTND);
                }
            }
        }   // Run method
    };  // Nio class


    template <class _inputOutputElementOperationType, class _scalarNArrayElementOperationType>
    class NioSiNi
    {
    public:
        template <class _inputOutputNArrayType, class _inputScalarType, class _inputNArrayType>
        static void Run(_inputOutputNArrayType & inputOutputNArray,
                        const _inputScalarType inputScalar,
                        const _inputNArrayType & inputNArray)
        {
            typedef _inputOutputNArrayType InputOutputNArrayType;
            typedef typename InputOutputNArrayType::OwnerType InputOutputOwnerType;
            typedef typename InputOutputOwnerType::const_pointer InputOutputConstPointerType;
            typedef typename InputOutputOwnerType::pointer InputOutputPointerType;

            typedef _inputNArrayType InputNArrayType;
            typedef typename InputNArrayType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owners
            InputOutputOwnerType & inputOutputOwner = inputOutputNArray.Owner();
            const InputOwnerType & inputOwner = inputNArray.Owner();

            // check sizes
            const nsize_type & inputOutputSizes = inputOutputOwner.sizes();
            const nsize_type & inputSizes = inputOwner.sizes();
            if (inputOutputSizes.NotEqual(inputSizes)) {
                ThrowSizeMismatchException();
            }

            // if compact and same strides
            const nstride_type & inputOutputStrides = inputOutputOwner.strides();
            const nstride_type & inputStrides = inputOwner.strides();

            if (inputOutputOwner.IsCompact() && inputOwner.IsCompact()
                && (inputOutputOwner.strides() == inputOwner.strides())) {
                vctDynamicCompactLoopEngines::CioSiCi<_inputOutputElementOperationType, _scalarNArrayElementOperationType>::Run(inputOutputOwner, inputScalar, inputOwner);
            } else {
                // otherwise
                // declare all variables used for inputOutputNArray
                nstride_type inputOutputSTND;
                vctFixedSizeVector<InputOutputConstPointerType, _dimension> inputOutputTargets;
                InputOutputPointerType inputOutputPointer = inputOutputNArray.Pointer();

                // declare all variables used for inputNArray
                nstride_type inputSTND;
                nstride_type inputOTND;
                InputPointerType inputPointer = inputNArray.Pointer();

                dimension_type numberOfWrappedDimensions = 0;
                const dimension_type maxWrappedDimensions = inputOutputNArray.dimension();

                CalculateSTND(inputOutputSTND, inputOutputSizes, inputOutputStrides);
                CalculateSTND(inputSTND, inputSizes, inputStrides);
                CalculateOTND(inputOTND, inputStrides, inputSTND);
                InitializeTargets(inputOutputTargets, inputOutputSizes, inputOutputStrides, inputOutputPointer);

                while (numberOfWrappedDimensions != maxWrappedDimensions) {
                    _inputOutputElementOperationType::Operate(*inputOutputPointer,
                                                              _scalarNArrayElementOperationType::Operate(inputScalar, *inputPointer) );

                    numberOfWrappedDimensions =
                        IncrementPointers(inputOutputTargets, inputOutputPointer, inputOutputStrides, inputOutputSTND);

                    SyncCurrentPointer(inputPointer, inputOTND, numberOfWrappedDimensions);
                }
            }
        }   // Run method
    };  // NioSiNi class



    template <class _inputOutputElementOperationType, class _nArrayElementOperationType>
    class NioNiNi
    {
    public:
        template <class _inputOutputNArrayType, class _input1NArrayType, class _input2NArrayType>
        static void Run(_inputOutputNArrayType & inputOutputNArray,
                        const _input1NArrayType & input1NArray,
                        const _input2NArrayType & input2NArray)
        {
            typedef _inputOutputNArrayType InputOutputNArrayType;
            typedef typename InputOutputNArrayType::OwnerType InputOutputOwnerType;
            typedef typename InputOutputOwnerType::const_pointer InputOutputConstPointerType;
            typedef typename InputOutputOwnerType::pointer InputOutputPointerType;

            typedef _input1NArrayType Input1NArrayType;
            typedef typename Input1NArrayType::OwnerType Input1OwnerType;
            typedef typename Input1OwnerType::const_pointer Input1PointerType;

            typedef _input2NArrayType Input2NArrayType;
            typedef typename Input2NArrayType::OwnerType Input2OwnerType;
            typedef typename Input2OwnerType::const_pointer Input2PointerType;

            // retrieve owners
            InputOutputOwnerType & inputOutputOwner = inputOutputNArray.Owner();
            const Input1OwnerType & input1Owner = input1NArray.Owner();
            const Input2OwnerType & input2Owner = input2NArray.Owner();

            // check sizes
            const nsize_type & inputOutputSizes = inputOutputOwner.sizes();
            const nsize_type & input1Sizes = input1Owner.sizes();
            const nsize_type & input2Sizes = input2Owner.sizes();
            if (inputOutputSizes.NotEqual(input1Sizes) || inputOutputSizes.NotEqual(input2Sizes)) {
                ThrowSizeMismatchException();
            }

            // if compact and same strides
            const nstride_type & inputOutputStrides = inputOutputOwner.strides();
            const nstride_type & input1Strides = input1Owner.strides();
            const nstride_type & input2Strides = input2Owner.strides();

            if (inputOutputOwner.IsCompact() && input1Owner.IsCompact() && input2Owner.IsCompact()
                && (inputOutputOwner.strides() == input1Owner.strides())
                && (inputOutputOwner.strides() == input2Owner.strides())) {
                vctDynamicCompactLoopEngines::CioCiCi<_inputOutputElementOperationType, _nArrayElementOperationType>::Run(inputOutputOwner, input1Owner, input2Owner);
            } else {
                // otherwise
                // declare all variables used for inputOutputNArray
                nstride_type inputOutputSTND;
                vctFixedSizeVector<InputOutputConstPointerType, _dimension> inputOutputTargets;
                InputOutputPointerType inputOutputPointer = inputOutputNArray.Pointer();

                // declare all variables used for input1NArray
                nstride_type input1STND;
                nstride_type input1OTND;
                Input1PointerType input1Pointer = input1NArray.Pointer();

                // declare all variables used for input2NArray
                nstride_type input2STND;
                nstride_type input2OTND;
                Input2PointerType input2Pointer = input2NArray.Pointer();

                dimension_type numberOfWrappedDimensions = 0;
                const dimension_type maxWrappedDimensions = inputOutputNArray.dimension();

                CalculateSTND(inputOutputSTND, inputOutputSizes, inputOutputStrides);
                CalculateSTND(input1STND, input1Sizes, input1Strides);
                CalculateOTND(input1OTND, input1Strides, input1STND);
                CalculateSTND(input2STND, input2Sizes, input2Strides);
                CalculateOTND(input2OTND, input2Strides, input2STND);
                InitializeTargets(inputOutputTargets, inputOutputSizes, inputOutputStrides, inputOutputPointer);

                while (numberOfWrappedDimensions != maxWrappedDimensions) {
                    _inputOutputElementOperationType::Operate(*inputOutputPointer,
                                                              _nArrayElementOperationType::Operate(*input1Pointer, *input2Pointer) );

                    numberOfWrappedDimensions =
                        IncrementPointers(inputOutputTargets, inputOutputPointer, inputOutputStrides, inputOutputSTND);

                    SyncCurrentPointer(input1Pointer, input1OTND, numberOfWrappedDimensions);
                    SyncCurrentPointer(input2Pointer, input2OTND, numberOfWrappedDimensions);
                }
            }
        }   // Run method
    };  // NioNiNi class


    class MinAndMax
    {
    public:
        template <class _inputNArrayType>
        static void Run(const _inputNArrayType & inputNArray,
                        typename _inputNArrayType::value_type & minValue,
                        typename _inputNArrayType::value_type & maxValue)
        {
            typedef _inputNArrayType InputNArrayType;
            typedef typename InputNArrayType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::value_type value_type;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owner
            const InputOwnerType & inputOwner = inputNArray.Owner();
            InputPointerType inputPointer = inputOwner.Pointer();

            if (inputPointer == 0)
                return;

            // if compact
            if (inputOwner.IsCompact()) {
                vctDynamicCompactLoopEngines::MinAndMax::Run(inputOwner, minValue, maxValue);
            } else {
                // otherwise
                const nsize_type & inputSizes = inputOwner.sizes();
                const nstride_type & inputStrides = inputOwner.strides();
                nstride_type inputSTND;
                vctFixedSizeVector<InputPointerType, _dimension> inputTargets;

                dimension_type numberOfWrappedDimensions = 0;
                const dimension_type maxWrappedDimensions = inputOwner.dimension();

                CalculateSTND(inputSTND, inputSizes, inputStrides);
                InitializeTargets(inputTargets, inputSizes, inputStrides, inputPointer);

                value_type minElement, maxElement, inputElement;
                minElement = maxElement = *inputPointer;

                while (numberOfWrappedDimensions != maxWrappedDimensions) {
                    inputElement = *inputPointer;

                    if (inputElement < minElement) {
                        minElement = inputElement;
                    } else if (inputElement > maxElement) {
                        maxElement = inputElement;
                    }

                    numberOfWrappedDimensions =
                        IncrementPointers(inputTargets, inputPointer, inputStrides, inputSTND);
                }
                minValue = minElement;
                maxValue = maxElement;
            }
        }   // Run method
    };  // MinAndMax class


};  // vctDynamicNArrayLoopEngines


#endif  // _vctDynamicNArrayLoopEngines_h
