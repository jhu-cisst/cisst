/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on: 2003-12-16

  (C) Copyright 2003-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctDynamicMatrixLoopEngines_h
#define _vctDynamicMatrixLoopEngines_h

/*!
  \file
  \brief Declaration of vctDynamicMatrixLoopEngines
*/

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnThrow.h>
#include <cisstVector/vctDynamicCompactLoopEngines.h>

/*!
  \brief Container class for the dynamic matrix engines.

  \sa MoMiMi MioMi MoMiSi MoSiMi MioSi MoMi Mio SoMi SoMiMi
*/
class vctDynamicMatrixLoopEngines {

public:

    /*! Helper function to throw an exception whenever sizes mismatch.
      This enforces that a standard message is sent. */
    template <typename _nsize_type>
    inline static void ThrowSizeMismatchException(const _nsize_type & expected,
                                                  const _nsize_type & received) CISST_THROW(std::runtime_error) {
        std::stringstream message;
        message << "vctDynamicMatrixLoopEngines: Sizes of matrices don't match, expected: [" << expected
                << "], received: [" << received << "]";
        cmnThrow(std::runtime_error(message.str()));
    }

    /*! Helper function to throw an exception whenever the output has
      the same base pointer as the output.  This enforces that a
      standard message is sent. */
    inline static void ThrowSharedPointersException(void) CISST_THROW(std::runtime_error) {
        cmnThrow(std::runtime_error("vctDynamicMatrixLoopEngines: Output base pointer is same as one of input base pointers."));
    }


    /*! Perform elementwise operation between matrices of identical
      size and element type.  The operation semantics is
      \code
      output[row][column] = op(input1[row][column], input2[row][column]);
      \endcode

      This is a similar semantics to the vctFixedSizeVectorRecursiveEngines::VoViVi
      engine.  However, for matrice we implement the operation using
      a loop instead of recursion.

      The engine is declared as a protected class.  This enables us to
      use softer declaration of the template arguments, as opposed to
      the functions calling this engine, which have to declare
      their arguments as matrices templated over stride etc.

      To use the engine, assume that Mout, Mi1, and Mi2 are matrix
      types; that mout, mi1, and mi2 are objects of corresponding
      types; and that OP is a valid operation in the form
      \f[ value\_type \times value\_type \rightarrow value\_type \f]
      Then write
      \code
      MoMiMi<OP>::Run(mout, mi1, mi2);
      \endcode

      \param _elementOperationType a binary operation class.
      _elementOperationType must have a static method with the
      signature
      \code
      _elementOperationType::Operate(_elementType, _elementType)
      \endcode
     */
    template<class _elementOperationType>
    class MoMiMi {
    public:
        template<class _outputMatrixType, class _input1MatrixType, class _input2MatrixType>
        static void Run(_outputMatrixType & outputMatrix,
                        const _input1MatrixType & input1Matrix,
                        const _input2MatrixType & input2Matrix)
        {
            typedef _outputMatrixType OutputMatrixType;
            typedef typename OutputMatrixType::OwnerType OutputOwnerType;
            typedef typename OutputOwnerType::size_type size_type;
            typedef typename OutputOwnerType::stride_type stride_type;
            typedef typename OutputOwnerType::pointer OutputPointerType;

            typedef _input1MatrixType Input1MatrixType;
            typedef typename Input1MatrixType::OwnerType Input1OwnerType;
            typedef typename Input1OwnerType::const_pointer Input1PointerType;

            typedef _input2MatrixType Input2MatrixType;
            typedef typename Input2MatrixType::OwnerType Input2OwnerType;
            typedef typename Input2OwnerType::const_pointer Input2PointerType;

            // retrieve owners
            OutputOwnerType & outputOwner = outputMatrix.Owner();
            const Input1OwnerType & input1Owner = input1Matrix.Owner();
            const Input2OwnerType & input2Owner = input2Matrix.Owner();

            const size_type rows = outputOwner.rows();
            const size_type cols = outputOwner.cols();

            // check sizes
            if ((rows != input1Owner.rows())
                || (cols != input1Owner.cols())) {
                ThrowSizeMismatchException(outputOwner.sizes(),
                                           input1Owner.sizes());
            } else if ((rows != input2Owner.rows())
                       || (cols != input2Owner.cols())) {
                ThrowSizeMismatchException(outputOwner.sizes(),
                                           input2Owner.sizes());
            }

            // if compact and same strides
            if (outputOwner.IsCompact() && input1Owner.IsCompact() && input2Owner.IsCompact()
                && (outputOwner.strides() == input1Owner.strides())
                && (outputOwner.strides() == input2Owner.strides())) {
                vctDynamicCompactLoopEngines::CoCiCi<_elementOperationType>::Run(outputOwner, input1Owner, input2Owner);
            } else {
                const stride_type outputColStride = outputOwner.col_stride();
                const stride_type outputRowStride = outputOwner.row_stride();
                const stride_type outputStrideToNextRow = outputRowStride - cols * outputColStride;

                const stride_type input1ColStride = input1Owner.col_stride();
                const stride_type input1RowStride = input1Owner.row_stride();
                const stride_type input1StrideToNextRow = input1RowStride - cols * input1ColStride;

                const stride_type input2ColStride = input2Owner.col_stride();
                const stride_type input2RowStride = input2Owner.row_stride();
                const stride_type input2StrideToNextRow = input2RowStride - cols * input2ColStride;

                OutputPointerType outputPointer = outputOwner.Pointer();
                const OutputPointerType outputRowEnd = outputPointer + rows * outputRowStride;
                OutputPointerType outputColEnd = outputPointer + cols * outputColStride;

                Input1PointerType input1Pointer = input1Owner.Pointer();
                Input2PointerType input2Pointer = input2Owner.Pointer();

                for (;
                     outputPointer != outputRowEnd;
                     outputPointer += outputStrideToNextRow, input1Pointer += input1StrideToNextRow, input2Pointer += input2StrideToNextRow,
                         outputColEnd += outputRowStride) {
                    for (;
                         outputPointer != outputColEnd;
                         outputPointer += outputColStride, input1Pointer += input1ColStride, input2Pointer += input2ColStride) {
                        *outputPointer = _elementOperationType::Operate(*input1Pointer, *input2Pointer);
                    }
                }
            }
        }  // Run method
    };  // MoMiMi class


    template<class _elementOperationType>
    class MoMi {
    public:
        template<class _outputMatrixType, class _inputMatrixType>
        static inline void Run(_outputMatrixType & outputMatrix,
                               const _inputMatrixType & inputMatrix)
        {
            typedef _outputMatrixType OutputMatrixType;
            typedef typename OutputMatrixType::OwnerType OutputOwnerType;
            typedef typename OutputOwnerType::size_type size_type;
            typedef typename OutputOwnerType::stride_type stride_type;
            typedef typename OutputOwnerType::pointer OutputPointerType;

            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owners
            const InputOwnerType & inputOwner = inputMatrix.Owner();
            OutputOwnerType & outputOwner = outputMatrix.Owner();

            const size_type rows = outputMatrix.rows();
            const size_type cols = outputMatrix.cols();

            // check sizes
            if ((rows != inputOwner.rows()) || (cols != inputOwner.cols())) {
                ThrowSizeMismatchException(outputOwner.sizes(), inputOwner.sizes());
            }

            // if compact and same strides
            if (outputOwner.IsCompact() && inputOwner.IsCompact()
                && (outputOwner.strides() == inputOwner.strides())) {
                vctDynamicCompactLoopEngines::CoCi<_elementOperationType>::Run(outputOwner, inputOwner);
            } else {
                // otherwise
                const stride_type outputColStride = outputOwner.col_stride();
                const stride_type outputRowStride = outputOwner.row_stride();
                const stride_type outputStrideToNextRow = outputRowStride - cols * outputColStride;

                const stride_type inputColStride = inputOwner.col_stride();
                const stride_type inputRowStride = inputOwner.row_stride();
                const stride_type inputStrideToNextRow = inputRowStride - cols * inputColStride;

                OutputPointerType outputPointer = outputOwner.Pointer();
                const OutputPointerType outputRowEnd = outputPointer + rows * outputRowStride;
                OutputPointerType outputColEnd = outputPointer + cols * outputColStride;

                InputPointerType inputPointer = inputOwner.Pointer();

                for (;
                     outputPointer != outputRowEnd;
                     outputPointer += outputStrideToNextRow, inputPointer += inputStrideToNextRow,
                         outputColEnd += outputRowStride) {
                    for (;
                         outputPointer != outputColEnd;
                         outputPointer += outputColStride, inputPointer += inputColStride) {
                        *outputPointer = _elementOperationType::Operate(*inputPointer);
                    }
                }
            } // Compact engine if
        }  // Run method
    };  // MoMi class


    template<class _elementOperationType>
    class Mio {
    public:
        template<class _inputOutputMatrixType>
        static inline void Run(_inputOutputMatrixType & inputOutputMatrix)
        {
            typedef _inputOutputMatrixType InputOutputMatrixType;
            typedef typename InputOutputMatrixType::OwnerType InputOutputOwnerType;
            typedef typename InputOutputOwnerType::size_type size_type;
            typedef typename InputOutputOwnerType::stride_type stride_type;
            typedef typename InputOutputOwnerType::pointer PointerType;

            // retrieve owner
            InputOutputOwnerType & inputOutputOwner = inputOutputMatrix.Owner();

            // if compact
            if (inputOutputOwner.IsCompact()) {
                vctDynamicCompactLoopEngines::Cio<_elementOperationType>::Run(inputOutputOwner);
            } else {
                const size_type rows = inputOutputOwner.rows();
                const size_type cols = inputOutputOwner.cols();

                const stride_type colStride = inputOutputOwner.col_stride();
                const stride_type rowStride = inputOutputOwner.row_stride();
                const stride_type strideToNextRow = rowStride - cols * colStride;

                PointerType inputOutputPointer = inputOutputOwner.Pointer();
                const PointerType rowEnd = inputOutputPointer + rows * rowStride;
                PointerType colEnd = inputOutputPointer + cols * colStride;

                for (;
                     inputOutputPointer != rowEnd;
                     inputOutputPointer += strideToNextRow, colEnd += rowStride) {
                    for (;
                         inputOutputPointer != colEnd;
                         inputOutputPointer += colStride) {
                        _elementOperationType::Operate(*inputOutputPointer);
                    }
                }
            }
        }  // Run method
    };  // Mio class


    template<class _elementOperationType>
    class MioMi {
    public:
        template<class _inputOutputMatrixType, class _inputMatrixType>
        static inline void Run(_inputOutputMatrixType & inputOutputMatrix,
                               const _inputMatrixType & inputMatrix)
        {
            typedef _inputOutputMatrixType InputOutputMatrixType;
            typedef typename InputOutputMatrixType::OwnerType InputOutputOwnerType;
            typedef typename InputOutputOwnerType::size_type size_type;
            typedef typename InputOutputOwnerType::stride_type stride_type;
            typedef typename InputOutputOwnerType::pointer InputOutputPointerType;

            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owners
            InputOutputOwnerType & inputOutputOwner = inputOutputMatrix.Owner();
            const InputOwnerType & inputOwner = inputMatrix.Owner();

            const size_type rows = inputOutputOwner.rows();
            const size_type cols = inputOutputOwner.cols();

            // check sizes
            if ((rows != inputOwner.rows()) || (cols != inputOwner.cols())) {
                ThrowSizeMismatchException(inputOutputOwner.sizes(), inputOwner.sizes());
            }

            // if compact and same strides
            if (inputOutputOwner.IsCompact() && inputOwner.IsCompact()
                && (inputOutputOwner.strides() == inputOwner.strides())) {
                vctDynamicCompactLoopEngines::CioCi<_elementOperationType>::Run(inputOutputOwner, inputOwner);
            } else {
                const stride_type inputOutputColStride = inputOutputOwner.col_stride();
                const stride_type inputOutputRowStride = inputOutputOwner.row_stride();
                const stride_type inputOutputStrideToNextRow = inputOutputRowStride - cols * inputOutputColStride;

                const stride_type inputColStride = inputOwner.col_stride();
                const stride_type inputRowStride = inputOwner.row_stride();
                const stride_type inputStrideToNextRow = inputRowStride - cols * inputColStride;

                InputOutputPointerType inputOutputPointer = inputOutputOwner.Pointer();
                const InputOutputPointerType inputOutputRowEnd = inputOutputPointer + rows * inputOutputRowStride;
                InputOutputPointerType inputOutputColEnd = inputOutputPointer + cols * inputOutputColStride;

                InputPointerType inputPointer = inputOwner.Pointer();

                for (;
                     inputOutputPointer != inputOutputRowEnd;
                     inputOutputPointer += inputOutputStrideToNextRow, inputPointer += inputStrideToNextRow,
                         inputOutputColEnd += inputOutputRowStride) {
                    for (;
                         inputOutputPointer != inputOutputColEnd;
                         inputOutputPointer += inputOutputColStride, inputPointer += inputColStride) {
                        _elementOperationType::Operate(*inputOutputPointer, *inputPointer);
                    }
                }
            }
        }  // Run method
    };  // MioMi class


    template<class _elementOperationType>
    class MoMiSi {
    public:
        template<class _outputMatrixType, class _inputMatrixType, class _inputScalarType>
        static void Run(_outputMatrixType & outputMatrix,
                        const _inputMatrixType & inputMatrix,
                        const _inputScalarType inputScalar)
        {
            typedef _outputMatrixType OutputMatrixType;
            typedef typename OutputMatrixType::OwnerType OutputOwnerType;
            typedef typename OutputOwnerType::size_type size_type;
            typedef typename OutputOwnerType::stride_type stride_type;
            typedef typename OutputOwnerType::pointer OutputPointerType;

            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owners
            OutputOwnerType & outputOwner = outputMatrix.Owner();
            const InputOwnerType & inputOwner = inputMatrix.Owner();

            const size_type rows = outputOwner.rows();
            const size_type cols = outputOwner.cols();

            // check sizes
            if ((rows != inputOwner.rows()) || (cols != inputOwner.cols())) {
                ThrowSizeMismatchException(outputOwner.sizes(), inputOwner.sizes());
            }

            // if compact and same strides
            if (outputOwner.IsCompact() && inputOwner.IsCompact()
                && (outputOwner.strides() == inputOwner.strides())) {
                vctDynamicCompactLoopEngines::CoCiSi<_elementOperationType>::Run(outputOwner, inputOwner, inputScalar);
            } else {
                // otherwise
                const stride_type outputColStride = outputOwner.col_stride();
                const stride_type outputRowStride = outputOwner.row_stride();
                const stride_type outputStrideToNextRow = outputRowStride - cols * outputColStride;

                const stride_type inputColStride = inputOwner.col_stride();
                const stride_type inputRowStride = inputOwner.row_stride();
                const stride_type inputStrideToNextRow = inputRowStride - cols * inputColStride;

                OutputPointerType outputPointer = outputOwner.Pointer();
                const OutputPointerType outputRowEnd = outputPointer + rows * outputRowStride;
                OutputPointerType outputColEnd = outputPointer + cols * outputColStride;

                InputPointerType inputPointer = inputOwner.Pointer();

                for (;
                     outputPointer != outputRowEnd;
                     outputPointer += outputStrideToNextRow, inputPointer += inputStrideToNextRow,
                         outputColEnd += outputRowStride) {
                    for (;
                         outputPointer != outputColEnd;
                         outputPointer += outputColStride, inputPointer += inputColStride) {
                        *outputPointer = _elementOperationType::Operate(*inputPointer, inputScalar);
                    }
                }
            }
        }  // Run method
    };  // MoMiSi


    template<class _elementOperationType>
    class MoSiMi {
    public:
        template<class _outputMatrixType, class _inputScalarType, class _inputMatrixType>
            static void Run(_outputMatrixType & outputMatrix,
            const _inputScalarType inputScalar,
            const _inputMatrixType & inputMatrix)
        {
            typedef _outputMatrixType OutputMatrixType;
            typedef typename OutputMatrixType::OwnerType OutputOwnerType;
            typedef typename OutputOwnerType::size_type size_type;
            typedef typename OutputOwnerType::stride_type stride_type;
            typedef typename OutputOwnerType::pointer OutputPointerType;

            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owners
            OutputOwnerType & outputOwner = outputMatrix.Owner();
            const InputOwnerType & inputOwner = inputMatrix.Owner();

            const size_type rows = outputOwner.rows();
            const size_type cols = outputOwner.cols();

            // check sizes
            if ((rows != inputOwner.rows()) || (cols != inputOwner.cols())) {
                ThrowSizeMismatchException(outputOwner.sizes(), inputOwner.sizes());
            }

            // if compact and same strides
            if (outputOwner.IsCompact() && inputOwner.IsCompact()
                && (outputOwner.strides() == inputOwner.strides())) {
                vctDynamicCompactLoopEngines::CoSiCi<_elementOperationType>::Run(outputOwner, inputScalar, inputOwner);
            } else {
                // otherwise
                const stride_type outputColStride = outputOwner.col_stride();
                const stride_type outputRowStride = outputOwner.row_stride();
                const stride_type outputStrideToNextRow = outputRowStride - cols * outputColStride;

                const stride_type inputColStride = inputOwner.col_stride();
                const stride_type inputRowStride = inputOwner.row_stride();
                const stride_type inputStrideToNextRow = inputRowStride - cols * inputColStride;

                OutputPointerType outputPointer = outputOwner.Pointer();
                const OutputPointerType outputRowEnd = outputPointer + rows * outputRowStride;
                OutputPointerType outputColEnd = outputPointer + cols * outputColStride;

                InputPointerType inputPointer = inputOwner.Pointer();

                for (;
                     outputPointer != outputRowEnd;
                     outputPointer += outputStrideToNextRow, inputPointer += inputStrideToNextRow,
                         outputColEnd += outputRowStride) {
                    for (;
                         outputPointer != outputColEnd;
                         outputPointer += outputColStride, inputPointer += inputColStride) {
                        *outputPointer = _elementOperationType::Operate(inputScalar, *inputPointer);
                    }
                }
            }
        }  // Run method
    };  // MoSiMi class


    template<class _elementOperationType>
    class MioSi {
    public:
        template<class _inputOutputMatrixType, class _inputScalarType>
        static void Run(_inputOutputMatrixType & inputOutputMatrix,
                        const _inputScalarType inputScalar)
        {
            typedef _inputOutputMatrixType InputOutputMatrixType;
            typedef typename InputOutputMatrixType::OwnerType InputOutputOwnerType;
            typedef typename InputOutputOwnerType::size_type size_type;
            typedef typename InputOutputOwnerType::stride_type stride_type;
            typedef typename InputOutputOwnerType::pointer InputOutputPointerType;

            // retrieve owner
            InputOutputOwnerType & inputOutputOwner = inputOutputMatrix.Owner();

            if (inputOutputOwner.IsCompact()) {
                vctDynamicCompactLoopEngines::CioSi<_elementOperationType>::Run(inputOutputOwner, inputScalar);
            } else {
                const size_type rows = inputOutputOwner.rows();
                const size_type cols = inputOutputOwner.cols();

                const stride_type colStride = inputOutputOwner.col_stride();
                const stride_type rowStride = inputOutputOwner.row_stride();
                const stride_type strideToNextRow = rowStride - cols * colStride;

                InputOutputPointerType inputOutputPointer = inputOutputOwner.Pointer();
                const InputOutputPointerType rowEnd = inputOutputPointer + rows * rowStride;
                InputOutputPointerType colEnd = inputOutputPointer + cols * colStride;

                for (;
                     inputOutputPointer != rowEnd;
                     inputOutputPointer += strideToNextRow, colEnd += rowStride) {
                    for (;
                         inputOutputPointer != colEnd;
                         inputOutputPointer += colStride) {
                        _elementOperationType::Operate(*inputOutputPointer, inputScalar);
                    }
                }
            }
        }  // Run method
    };  // MioSi class


    template<class _incrementalOperationType, class _elementOperationType>
    class SoMi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;

        template<class _inputMatrixType>
        static OutputType Run(const _inputMatrixType & inputMatrix)
        {
            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::size_type size_type;
            typedef typename InputOwnerType::stride_type stride_type;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owner
            const InputOwnerType & inputOwner = inputMatrix.Owner();

            if (inputOwner.IsCompact()) {
                return vctDynamicCompactLoopEngines::SoCi<_incrementalOperationType, _elementOperationType>
                    ::Run(inputOwner);
            } else {
                const size_type rows = inputOwner.rows();
                const size_type cols = inputOwner.cols();

                OutputType incrementalResult = _incrementalOperationType::NeutralElement();

                const stride_type inputColStride = inputOwner.col_stride();
                const stride_type inputRowStride = inputOwner.row_stride();
                const stride_type inputStrideToNextRow = inputRowStride - cols * inputColStride;
                InputPointerType inputPointer = inputOwner.Pointer();
                const InputPointerType inputRowEnd = inputPointer + rows * inputRowStride;
                InputPointerType inputColEnd = inputPointer + cols * inputColStride;

                for (;
                     inputPointer != inputRowEnd;
                     inputPointer += inputStrideToNextRow, inputColEnd += inputRowStride) {
                    for (;
                         inputPointer != inputColEnd;
                         inputPointer += inputColStride) {
                        incrementalResult = _incrementalOperationType::Operate(
                                                                               incrementalResult, _elementOperationType::Operate(*inputPointer) );
                    }
                }
                return incrementalResult;
            }
        }  // Run method
    };  // SoMi class


    template<class _incrementalOperationType, class _elementOperationType>
    class SoMiMi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;

        template<class _input1MatrixType, class _input2MatrixType>
        static OutputType Run(const _input1MatrixType & input1Matrix,
                              const _input2MatrixType & input2Matrix) {
            typedef _input1MatrixType Input1MatrixType;
            typedef typename Input1MatrixType::OwnerType Input1OwnerType;
            typedef typename Input1OwnerType::size_type size_type;
            typedef typename Input1OwnerType::stride_type stride_type;
            typedef typename Input1OwnerType::const_pointer Input1PointerType;

            typedef _input2MatrixType Input2MatrixType;
            typedef typename Input2MatrixType::OwnerType Input2OwnerType;
            typedef typename Input2OwnerType::const_pointer Input2PointerType;

            // retrieve owners
            const Input1OwnerType & input1Owner = input1Matrix.Owner();
            const Input2OwnerType & input2Owner = input2Matrix.Owner();

            const size_type rows = input1Owner.rows();
            const size_type cols = input1Owner.cols();

            // check sizes
            if ((rows != input2Owner.rows()) || (cols != input2Owner.cols())) {
                ThrowSizeMismatchException(input1Owner.sizes(), input2Owner.sizes());
            }

            // if compact and same strides
            if (input1Owner.IsCompact() && input2Owner.IsCompact()
                && (input1Owner.strides() == input2Owner.strides())) {
                return vctDynamicCompactLoopEngines::SoCiCi<_incrementalOperationType, _elementOperationType>::Run(input1Owner, input2Owner);
            } else {
                // otherwise
                OutputType incrementalResult = _incrementalOperationType::NeutralElement();
                const stride_type input1ColStride = input1Owner.col_stride();
                const stride_type input1RowStride = input1Owner.row_stride();
                const stride_type input1StrideToNextRow = input1RowStride - cols * input1ColStride;

                const stride_type input2ColStride = input2Owner.col_stride();
                const stride_type input2RowStride = input2Owner.row_stride();
                const stride_type input2StrideToNextRow = input2RowStride - cols * input2ColStride;

                Input1PointerType input1Pointer = input1Owner.Pointer();
                const Input1PointerType input1RowEnd = input1Pointer + rows * input1RowStride;
                Input1PointerType input1ColEnd = input1Pointer + cols * input1ColStride;

                Input2PointerType input2Pointer = input2Owner.Pointer();

                for (;
                     input1Pointer != input1RowEnd;
                     input1Pointer += input1StrideToNextRow, input2Pointer += input2StrideToNextRow,
                         input1ColEnd += input1RowStride) {
                    for (;
                         input1Pointer != input1ColEnd;
                         input1Pointer += input1ColStride, input2Pointer += input2ColStride) {
                        incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                                                                               _elementOperationType::Operate(*input1Pointer, *input2Pointer) );
                    }
                }
                return incrementalResult;
            }
        }
    };


    /*!  \brief Implement operation of the form \f$m_{io} =
      op_{io}(m_{io}, op_{sm}(s, m_i))\f$ for fixed size matrices

      This class uses template specialization to perform store-back
      matrix-scalar-matrix operations

      \f[
      m_{io} = \mathrm{op_{io}}(m_{io}, \mathrm{op_{sm}}(s, m_i))
      \f]

      where \f$m_{io}\f$ is an input-output (store-back) matrix;
      \f$s\f$ is a scalar; and \f$m_i\f$ is an input matrix.  A
      typical example is: \f$m_{io} += s \cdot m_i\f$.  The matrices
      have a fixed size, determined at compilation time; \f$op_{sm}\f$
      is an operation between \f$s\f$ and the elements of \f$m_i\f$;
      \f$op_{io}\f$ is an operation between the output of
      \f$op_{sm}\f$ and the elements of \f$m_{io}\f$.

      \param _ioOperationType The type of the store-back operation.

      \param _scalarMatrixElementOperationType The type of the
      operation between scalar and input matrix.

      \sa vctFixedSizeMatrixRecursiveEngines
    */
    template<class _ioElementOperationType, class _scalarMatrixElementOperationType>
    class MioSiMi
    {
    public:

        template<class _ioMatrixType, class _inputScalarType, class _inputMatrixType>
        static inline void Run(_ioMatrixType & ioMatrix,
                               const _inputScalarType & inputScalar,
                               const _inputMatrixType & inputMatrix)
        {
            typedef _ioMatrixType IoMatrixType;
            typedef typename IoMatrixType::OwnerType IoOwnerType;
            typedef typename IoOwnerType::size_type size_type;
            typedef typename IoOwnerType::stride_type stride_type;
            typedef typename IoOwnerType::pointer IoPointerType;

            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owners
            IoOwnerType & ioOwner = ioMatrix.Owner();
            const InputOwnerType & inputOwner = inputMatrix.Owner();

            const size_type rows = ioOwner.rows();
            const size_type cols = ioOwner.cols();

            // check sizes
            if ((rows != inputOwner.rows()) || (cols != inputOwner.cols())) {
                ThrowSizeMismatchException(ioOwner.sizes(), inputOwner.sizes());
            }

            // if compact and same strides
            if (ioOwner.IsCompact() && inputOwner.IsCompact()
                && (ioOwner.strides() == inputOwner.strides())) {
                vctDynamicCompactLoopEngines::CioSiCi<_ioElementOperationType, _scalarMatrixElementOperationType>::Run(ioOwner, inputScalar, inputOwner);
            } else {
                // otherwise
                const stride_type ioColStride = ioOwner.col_stride();
                const stride_type ioRowStride = ioOwner.row_stride();
                const stride_type ioStrideToNextRow = ioRowStride - cols * ioColStride;

                const stride_type inputColStride = inputOwner.col_stride();
                const stride_type inputRowStride = inputOwner.row_stride();
                const stride_type inputStrideToNextRow = inputRowStride - cols * inputColStride;

                IoPointerType ioPointer = ioOwner.Pointer();
                InputPointerType inputPointer = inputOwner.Pointer();

                size_type rowIndex, colIndex;
                for (rowIndex = 0;
                     rowIndex < rows;
                     ++rowIndex, ioPointer += ioStrideToNextRow, inputPointer += inputStrideToNextRow) {
                    for (colIndex = 0;
                         colIndex < cols;
                         ++colIndex, ioPointer += ioColStride, inputPointer += inputColStride) {
                        _ioElementOperationType::Operate(*ioPointer,
                                                         _scalarMatrixElementOperationType::Operate(inputScalar, *inputPointer));
                    }
                }
            }
        }
    };


    /*!  \brief Implement operation of the form \f$m_{io} =
      op_{io}(m_{io}, op_{mm}(m_{i1}, m_{i2}))\f$ for fixed size matrices

      This class uses template specialization to perform store-back
      matrix-matrix-matrix operations

      \f[
      m_{io} = \mathrm{op_{io}}(m_{io}, \mathrm{op_{mm}}(m_{i1}, m_{i2}))
      \f]

      where \f$m_{io}\f$ is an input-output (store-back) matrix;
      \f$m_{i1}\f$ and \f$m_{i2}\f$ are input matrices.  A typical
      example is: \f$m_{io} += m_{i1} \cdot m_{i2}\f$.  The matrices
      have a fixed size, determined at compilation time; \f$op_{,m}\f$
      is an element wise operation between the elements of
      \f$m_{i1}\f$ and \f$m_{i2}\f$; \f$op_{io}\f$ is an operation
      between the output of \f$op_{mm}\f$ and the elements of
      \f$m_{io}\f$.

      \param _ioOperationType The type of the store-back operation.

      \param _matrixElementOperationType The type of the
      element wise operation between scalar and input matrix.

      \sa vctFixedSizeMatrixRecursiveEngines
    */
    template<class _ioElementOperationType, class _matrixElementOperationType>
    class MioMiMi
    {
    public:

        template<class _ioMatrixType, class _input1MatrixType, class _input2MatrixType>
        static inline void Run(_ioMatrixType & ioMatrix,
                               const _input1MatrixType & input1Matrix,
                               const _input2MatrixType & input2Matrix)
        {
            typedef _ioMatrixType IoMatrixType;
            typedef typename IoMatrixType::OwnerType IoOwnerType;
            typedef typename IoOwnerType::size_type size_type;
            typedef typename IoOwnerType::stride_type stride_type;
            typedef typename IoOwnerType::pointer IoPointerType;

            typedef _input1MatrixType Input1MatrixType;
            typedef typename Input1MatrixType::OwnerType Input1OwnerType;
            typedef typename Input1OwnerType::const_pointer Input1PointerType;

            typedef _input2MatrixType Input2MatrixType;
            typedef typename Input2MatrixType::OwnerType Input2OwnerType;
            typedef typename Input2OwnerType::const_pointer Input2PointerType;

            // retrieve owners
            IoOwnerType & ioOwner = ioMatrix.Owner();
            const Input1OwnerType & input1Owner = input1Matrix.Owner();
            const Input2OwnerType & input2Owner = input2Matrix.Owner();

            const size_type rows = ioOwner.rows();
            const size_type cols = ioOwner.cols();

            // check sizes
            if ((rows != input1Owner.rows())
                || (cols != input1Owner.cols())) {
                ThrowSizeMismatchException(ioOwner.sizes(),
                                           input1Owner.sizes());
            } else if ((rows != input2Owner.rows())
                       || (cols != input2Owner.cols())) {
                ThrowSizeMismatchException(ioOwner.sizes(),
                                           input2Owner.sizes());
            }

            // if compact and same strides
            if (ioOwner.IsCompact() && input1Owner.IsCompact()  && input2Owner.IsCompact()
                && (ioOwner.strides() == input1Owner.strides())
                && (ioOwner.strides() == input2Owner.strides())) {
                vctDynamicCompactLoopEngines::CioCiCi<_ioElementOperationType, _matrixElementOperationType>::Run(ioOwner, input1Owner, input2Owner);
            } else {
                // otherwise
                const stride_type ioColStride = ioOwner.col_stride();
                const stride_type ioRowStride = ioOwner.row_stride();
                const stride_type ioStrideToNextRow = ioRowStride - cols * ioColStride;

                const stride_type input1ColStride = input1Owner.col_stride();
                const stride_type input1RowStride = input1Owner.row_stride();
                const stride_type input1StrideToNextRow = input1RowStride - cols * input1ColStride;

                const stride_type input2ColStride = input2Owner.col_stride();
                const stride_type input2RowStride = input2Owner.row_stride();
                const stride_type input2StrideToNextRow = input2RowStride - cols * input2ColStride;

                IoPointerType ioPointer = ioOwner.Pointer();
                Input1PointerType input1Pointer = input1Owner.Pointer();
                Input2PointerType input2Pointer = input2Owner.Pointer();

                size_type rowIndex, colIndex;
                for (rowIndex = 0;
                     rowIndex < rows;
                     ++rowIndex, ioPointer += ioStrideToNextRow,
                         input1Pointer += input1StrideToNextRow,
                         input2Pointer += input2StrideToNextRow) {
                    for (colIndex = 0;
                         colIndex < cols;
                         ++colIndex, ioPointer += ioColStride,
                             input1Pointer += input1ColStride,
                             input2Pointer += input2ColStride) {
                        _ioElementOperationType::Operate(*ioPointer,
                                                         _matrixElementOperationType::Operate(*input1Pointer, *input2Pointer));
                    }
                }
            }
        }
    };


    template<class _incrementalOperationType, class _elementOperationType>
    class SoMiSi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;

        template<class _inputMatrixType, class _inputScalarType>
        static OutputType Run(const _inputMatrixType & inputMatrix,
                              const _inputScalarType & inputScalar)
        {
            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::size_type size_type;
            typedef typename InputOwnerType::stride_type stride_type;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owners
            const InputOwnerType & inputOwner = inputMatrix.Owner();

            const size_type rows = inputOwner.rows();
            const size_type cols = inputOwner.cols();

            // if compact and same strides
            if (inputOwner.IsCompact()) {
                return vctDynamicCompactLoopEngines::SoCiSi<_incrementalOperationType, _elementOperationType>::Run(inputOwner, inputScalar);
            } else {
                // otherwise
                OutputType incrementalResult = _incrementalOperationType::NeutralElement();

                const stride_type inputColStride = inputOwner.col_stride();
                const stride_type inputRowStride = inputOwner.row_stride();
                const stride_type inputStrideToNextRow = inputRowStride - cols * inputColStride;
                InputPointerType inputPointer = inputOwner.Pointer();
                const InputPointerType inputRowEnd = inputPointer + rows * inputRowStride;
                InputPointerType inputColEnd = inputPointer + cols * inputColStride;

                for (;
                     inputPointer != inputRowEnd;
                     inputPointer += inputStrideToNextRow, inputColEnd += inputRowStride) {
                    for (;
                         inputPointer != inputColEnd;
                         inputPointer += inputColStride) {
                        incrementalResult =
                            _incrementalOperationType::Operate(incrementalResult,
                                                               _elementOperationType::Operate(*inputPointer, inputScalar));
                    }
                }
                return incrementalResult;
            }
        }
    };



    template<class _operationType>
    class Product {
    public:
        template<class _outputMatrixType, class _input1MatrixType, class _input2MatrixType>
        static void Run(_outputMatrixType & outputMatrix,
                        const _input1MatrixType & input1Matrix,
                        const _input2MatrixType & input2Matrix)
        {
            typedef _outputMatrixType OutputMatrixType;
            typedef typename OutputMatrixType::size_type size_type;
            typedef typename OutputMatrixType::stride_type stride_type;
            typedef typename OutputMatrixType::pointer OutputPointerType;

            typedef _input1MatrixType Input1MatrixType;
            typedef typename Input1MatrixType::const_pointer Input1PointerType;

            typedef _input2MatrixType Input2MatrixType;
            typedef typename Input2MatrixType::const_pointer Input2PointerType;

            const size_type rows = outputMatrix.rows();
            const size_type cols = outputMatrix.cols();
            const size_type input1Cols = input1Matrix.cols();
            // check sizes
            if (rows != input1Matrix.rows()) {
                std::stringstream message;
                message << "vctDynamicMatrixLoopEngines::Product: Sizes of matrices don't match, expected rows: " << rows
                        << ", received: " << input1Matrix.rows();
                cmnThrow(std::runtime_error(message.str()));
            } else if ((cols != input2Matrix.cols())
                       || (input1Cols != input2Matrix.rows())) {
                std::stringstream message;
                message << "vctDynamicMatrixLoopEngines: Sizes of matrices don't match, expected: ["
                        << input1Cols << " " << cols << "], received: " << input2Matrix.sizes();
                cmnThrow(std::runtime_error(message.str()));
            }

            // Otherwise
            const stride_type outputColStride = outputMatrix.col_stride();
            const stride_type outputRowStride = outputMatrix.row_stride();
            const stride_type outputStrideToNextRow = outputRowStride - cols * outputColStride;

            const stride_type input1ColStride = input1Matrix.col_stride();
            const stride_type input1RowStride = input1Matrix.row_stride();

            const stride_type input2ColStride = input2Matrix.col_stride();
            const stride_type input2RowStride = input2Matrix.row_stride();

            OutputPointerType outputPointer = outputMatrix.Pointer();
            const OutputPointerType outputRowEnd = outputPointer + rows * outputRowStride;
            OutputPointerType outputColEnd = outputPointer + cols * outputColStride;

            Input1PointerType input1Pointer = input1Matrix.Pointer();
            Input2PointerType input2Pointer = input2Matrix.Pointer();

            typename Input1MatrixType::ConstRowRefType input1Row;
            typename Input2MatrixType::ConstColumnRefType input2Col;

            if ((outputPointer == input1Pointer) ||
                (outputPointer == input2Pointer)) {
                ThrowSharedPointersException();
            }

            for (; outputPointer != outputRowEnd;
                outputPointer += outputStrideToNextRow,
                input1Pointer += input1RowStride,
                input2Pointer = input2Matrix.Pointer(),
                outputColEnd += outputRowStride)
            {
                input1Row.SetRef(input1Cols, input1Pointer, input1ColStride);
                for (; outputPointer != outputColEnd;
                    outputPointer += outputColStride,
                    input2Pointer += input2ColStride)
                {
                    input2Col.SetRef(input1Cols, input2Pointer, input2RowStride);
                    *outputPointer = _operationType::Operate(input1Row, input2Col);
                }
            }
        }  // Run method
    };  // Product class


    /*! A specialized engine for computing the minimum and maximum
      elements of a matrix in one pass.  This implementation is more
      efficient than computing them separately.
    */
    class MinAndMax
    {
    public:
        template<class _inputMatrixType>
        static void Run(const _inputMatrixType & inputMatrix,
        typename _inputMatrixType::value_type & minValue, typename _inputMatrixType::value_type & maxValue)
        {
            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::size_type size_type;
            typedef typename InputOwnerType::stride_type stride_type;
            typedef typename InputOwnerType::const_pointer InputPointerType;
            typedef typename InputOwnerType::value_type value_type;

            // retrieve owner
            const InputOwnerType & inputOwner = inputMatrix.Owner();
            InputPointerType inputPointer = inputOwner.Pointer();

            if (inputPointer == 0)
                return;

            if (inputOwner.IsCompact()) {
                vctDynamicCompactLoopEngines::MinAndMax::Run(inputOwner, minValue, maxValue);
            } else {
                // otherwise
                const size_type rows = inputOwner.rows();
                const size_type cols = inputOwner.cols();

                const stride_type inputColStride = inputOwner.col_stride();
                const stride_type inputRowStride = inputOwner.row_stride();
                const stride_type inputStrideToNextRow = inputRowStride - cols * inputColStride;
                const InputPointerType inputRowEnd = inputPointer + rows * inputRowStride;
                InputPointerType inputColEnd = inputPointer + cols * inputColStride;

                value_type minElement, maxElement;
                maxElement = minElement = *inputPointer;

                for (;
                     inputPointer != inputRowEnd;
                     inputPointer += inputStrideToNextRow, inputColEnd += inputRowStride) {
                    for (;
                         inputPointer != inputColEnd;
                         inputPointer += inputColStride) {
                        const value_type element = *inputPointer;
                        if (element < minElement) {
                            minElement = element;
                        } else if (maxElement < element) {
                            maxElement = element;
                        }
                    }
                }
                minValue = minElement;
                maxValue = maxElement;
            }
        }  // Run method
    };  // MinAndMax class


    class SelectRowsByIndex
    {
    public:
        template<class _outputMatrixType, class _inputMatrixType, class _indexVectorType>
        static void Run(_outputMatrixType & outputMatrix, const _inputMatrixType & inputMatrix,
                        const _indexVectorType & indexVector)
        {
            typedef _outputMatrixType OutputMatrixType;
            // check sizes
            typedef _outputMatrixType OutputMatrixType;
            typedef typename OutputMatrixType::size_type size_type;
            typedef typename OutputMatrixType::stride_type stride_type;
            typedef typename OutputMatrixType::pointer OutputPointerType;

            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::const_pointer InputPointerType;
            typedef _indexVectorType IndexVectorType;
            typedef typename IndexVectorType::const_pointer IndexPointerType;

            const size_type rows = outputMatrix.rows();
            const size_type cols = outputMatrix.cols();
            if (cols != inputMatrix.cols()) {
                std::stringstream message;
                message << "vctDynamicMatrixLoopEngines::SelectRowsByIndex: Sizes of matrices don't match, expected cols: "
                        << cols << ", received: " << inputMatrix.cols();
                cmnThrow(std::runtime_error(message.str()));
            } else if (rows != indexVector.size()) {
                std::stringstream message;
                message << "vctDynamicMatrixLoopEngines::SelectRowsByIndex: Sizes of matrices don't match, expected rows: "
                        << rows << ", received: " << indexVector.size();
                cmnThrow(std::runtime_error(message.str()));
            }

            // otherwise
            const stride_type outputColStride = outputMatrix.col_stride();
            const stride_type outputRowStride = outputMatrix.row_stride();
            const stride_type outputStrideToNextRow = outputRowStride - cols * outputColStride;

            const stride_type inputColStride = inputMatrix.col_stride();
            const stride_type indexStride = indexVector.stride();

            OutputPointerType outputPointer = outputMatrix.Pointer();
            const OutputPointerType outputRowEnd = outputPointer + rows * outputRowStride;
            OutputPointerType outputColEnd = outputPointer + cols * outputColStride;

            InputPointerType inputPointer;
            IndexPointerType indexPointer = indexVector.Pointer();

            for (; outputPointer != outputRowEnd;
                outputPointer += outputStrideToNextRow,
                indexPointer += indexStride,
                outputColEnd += outputRowStride)
            {
                inputPointer = inputMatrix.Pointer(*indexPointer, 0);
                for (; outputPointer != outputColEnd;
                    outputPointer += outputColStride,
                    inputPointer += inputColStride)
                {
                    *outputPointer = *inputPointer;
                }
            }

        }
    };


};


#endif  // _vctDynamicMatrixLoopEngines_h
