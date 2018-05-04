/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on: 2003-12-16

  (C) Copyright 2003-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctFixedSizeMatrixLoopEngines_h
#define _vctFixedSizeMatrixLoopEngines_h

/*!
  \file
  \brief Declaration of vctFixedSizeMatrixLoopEngines
 */

/*!
  \brief Container class for the matrix engines.

  \sa MoMiMi MioMi MoMiSi MoSiMi MioSi MoMi Mio SoMi SoMiMi
*/
class vctFixedSizeMatrixLoopEngines {

public:

    /*! Helper function to throw an exception whenever the output has
      the same base pointer as the output.  This enforces that a
      standard message is sent. */
    inline static void ThrowSharedPointersException(void) CISST_THROW(std::runtime_error) {
        cmnThrow(std::runtime_error("vctFixedSizeMatrixLoopEngines: Output base pointer is same as one of input base pointers."));
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
            typedef typename OutputMatrixType::size_type size_type;
            typedef typename OutputMatrixType::pointer OutputPointerType;

            typedef _input1MatrixType Input1MatrixType;
            typedef typename Input1MatrixType::const_pointer Input1PointerType;
            typedef _input2MatrixType Input2MatrixType;
            typedef typename Input2MatrixType::const_pointer Input2PointerType;

            enum {
                ROWS = OutputMatrixType::ROWS,
                COLS = OutputMatrixType::COLS,
                OUTPUT_COL_STRIDE = OutputMatrixType::COLSTRIDE,
                OUTPUT_ROW_STRIDE = OutputMatrixType::ROWSTRIDE,
                OUTPUT_STRIDE_TO_NEXT_ROW = OUTPUT_ROW_STRIDE - COLS * OUTPUT_COL_STRIDE
            };

            enum {
                INPUT1_COL_STRIDE = Input1MatrixType::COLSTRIDE,
                INPUT1_ROW_STRIDE = Input1MatrixType::ROWSTRIDE,
                INPUT1_STRIDE_TO_NEXT_ROW = INPUT1_ROW_STRIDE - COLS * INPUT1_COL_STRIDE
            };

            enum {
                INPUT2_COL_STRIDE = Input2MatrixType::COLSTRIDE,
                INPUT2_ROW_STRIDE = Input2MatrixType::ROWSTRIDE,
                INPUT2_STRIDE_TO_NEXT_ROW = INPUT2_ROW_STRIDE - COLS * INPUT2_COL_STRIDE
            };

            OutputPointerType outputPointer = outputMatrix.Pointer();
            Input1PointerType input1Pointer = input1Matrix.Pointer();
            Input2PointerType input2Pointer = input2Matrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                outputPointer += OUTPUT_STRIDE_TO_NEXT_ROW,
                input1Pointer += INPUT1_STRIDE_TO_NEXT_ROW,
                input2Pointer += INPUT2_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    outputPointer += OUTPUT_COL_STRIDE,
                    input1Pointer += INPUT1_COL_STRIDE,
                    input2Pointer += INPUT2_COL_STRIDE)
                {
                    *outputPointer = _elementOperationType::Operate(*input1Pointer, *input2Pointer);
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
            typedef typename OutputMatrixType::size_type size_type;
            typedef typename OutputMatrixType::pointer OutputPointerType;

            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::const_pointer InputPointerType;

            enum {
                ROWS = OutputMatrixType::ROWS,
                COLS = OutputMatrixType::COLS,
                OUTPUT_COL_STRIDE = OutputMatrixType::COLSTRIDE,
                OUTPUT_ROW_STRIDE = OutputMatrixType::ROWSTRIDE,
                OUTPUT_STRIDE_TO_NEXT_ROW = OUTPUT_ROW_STRIDE - COLS * OUTPUT_COL_STRIDE
            };

            enum {
                INPUT_COL_STRIDE = InputMatrixType::COLSTRIDE,
                INPUT_ROW_STRIDE = InputMatrixType::ROWSTRIDE,
                INPUT_STRIDE_TO_NEXT_ROW = INPUT_ROW_STRIDE - COLS * INPUT_COL_STRIDE
            };


            OutputPointerType outputPointer = outputMatrix.Pointer();
            InputPointerType inputPointer = inputMatrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                outputPointer += OUTPUT_STRIDE_TO_NEXT_ROW,
                inputPointer += INPUT_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    outputPointer += OUTPUT_COL_STRIDE,
                    inputPointer += INPUT_COL_STRIDE)
                {
                    *outputPointer = _elementOperationType::Operate(*inputPointer);
                }
            }
        }  // Run method
    };  // MoMi class


    template<class _elementOperationType>
    class Mio {
    public:
        template<class _inputOutputMatrixType>
            static inline void Run(_inputOutputMatrixType & inputOutputMatrix)
        {
            typedef _inputOutputMatrixType InputOutputMatrixType;
            typedef typename InputOutputMatrixType::size_type size_type;
            typedef typename InputOutputMatrixType::pointer InputOutputPointerType;

            enum {
                ROWS = InputOutputMatrixType::ROWS,
                COLS = InputOutputMatrixType::COLS,
                COL_STRIDE = InputOutputMatrixType::COLSTRIDE,
                ROW_STRIDE = InputOutputMatrixType::ROWSTRIDE,
                STRIDE_TO_NEXT_ROW = ROW_STRIDE - COLS * COL_STRIDE
            };

            InputOutputPointerType inputOutputPointer = inputOutputMatrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                inputOutputPointer += STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    inputOutputPointer += COL_STRIDE)
                {
                    _elementOperationType::Operate(*inputOutputPointer);
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
            typedef typename InputOutputMatrixType::size_type size_type;
            typedef typename InputOutputMatrixType::pointer InputOutputPointerType;

            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::const_pointer InputPointerType;

            enum {
                ROWS = InputOutputMatrixType::ROWS,
                COLS = InputOutputMatrixType::COLS,
                INPUT_OUTPUT_COL_STRIDE = InputOutputMatrixType::COLSTRIDE,
                INPUT_OUTPUT_ROW_STRIDE = InputOutputMatrixType::ROWSTRIDE,
                INPUT_OUTPUT_STRIDE_TO_NEXT_ROW = INPUT_OUTPUT_ROW_STRIDE - COLS * INPUT_OUTPUT_COL_STRIDE
            };

            enum {
                INPUT_COL_STRIDE = InputMatrixType::COLSTRIDE,
                INPUT_ROW_STRIDE = InputMatrixType::ROWSTRIDE,
                INPUT_STRIDE_TO_NEXT_ROW = INPUT_ROW_STRIDE - COLS * INPUT_COL_STRIDE
            };


            InputOutputPointerType inputOutputPointer = inputOutputMatrix.Pointer();
            InputPointerType inputPointer = inputMatrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                inputOutputPointer += INPUT_OUTPUT_STRIDE_TO_NEXT_ROW,
                inputPointer += INPUT_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    inputOutputPointer += INPUT_OUTPUT_COL_STRIDE,
                    inputPointer += INPUT_COL_STRIDE)
                {
                    _elementOperationType::Operate(*inputOutputPointer, *inputPointer);
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
            typedef typename OutputMatrixType::size_type size_type;
            typedef typename OutputMatrixType::pointer OutputPointerType;

            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::const_pointer InputPointerType;

            enum {
                ROWS = OutputMatrixType::ROWS,
                COLS = OutputMatrixType::COLS,
                OUTPUT_COL_STRIDE = OutputMatrixType::COLSTRIDE,
                OUTPUT_ROW_STRIDE = OutputMatrixType::ROWSTRIDE,
                OUTPUT_STRIDE_TO_NEXT_ROW = OUTPUT_ROW_STRIDE - COLS * OUTPUT_COL_STRIDE
            };

            enum {
                INPUT_COL_STRIDE = InputMatrixType::COLSTRIDE,
                INPUT_ROW_STRIDE = InputMatrixType::ROWSTRIDE,
                INPUT_STRIDE_TO_NEXT_ROW = INPUT_ROW_STRIDE - COLS * INPUT_COL_STRIDE
            };


            OutputPointerType outputPointer = outputMatrix.Pointer();
            InputPointerType inputPointer = inputMatrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                outputPointer += OUTPUT_STRIDE_TO_NEXT_ROW,
                inputPointer += INPUT_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    outputPointer += OUTPUT_COL_STRIDE,
                    inputPointer += INPUT_COL_STRIDE)
                {
                    *outputPointer = _elementOperationType::Operate(*inputPointer, inputScalar);
                }
            }
        }  // Run method
    };  // MoMiSi class


    template<class _elementOperationType>
    class MoSiMi {
    public:
        template<class _outputMatrixType, class _inputScalarType, class _inputMatrixType>
            static void Run(_outputMatrixType & outputMatrix,
            const _inputScalarType inputScalar,
            const _inputMatrixType & inputMatrix)
        {
            typedef _outputMatrixType OutputMatrixType;
            typedef typename OutputMatrixType::size_type size_type;
            typedef typename OutputMatrixType::pointer OutputPointerType;

            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::const_pointer InputPointerType;

            enum {
                ROWS = OutputMatrixType::ROWS,
                COLS = OutputMatrixType::COLS,
                OUTPUT_COL_STRIDE = OutputMatrixType::COLSTRIDE,
                OUTPUT_ROW_STRIDE = OutputMatrixType::ROWSTRIDE,
                OUTPUT_STRIDE_TO_NEXT_ROW = OUTPUT_ROW_STRIDE - COLS * OUTPUT_COL_STRIDE
            };

            enum {
                INPUT_COL_STRIDE = InputMatrixType::COLSTRIDE,
                INPUT_ROW_STRIDE = InputMatrixType::ROWSTRIDE,
                INPUT_STRIDE_TO_NEXT_ROW = INPUT_ROW_STRIDE - COLS * INPUT_COL_STRIDE
            };


            OutputPointerType outputPointer = outputMatrix.Pointer();
            InputPointerType inputPointer = inputMatrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                outputPointer += OUTPUT_STRIDE_TO_NEXT_ROW,
                inputPointer += INPUT_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    outputPointer += OUTPUT_COL_STRIDE,
                    inputPointer += INPUT_COL_STRIDE)
                {
                    *outputPointer = _elementOperationType::Operate(inputScalar, *inputPointer);
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
            typedef typename InputOutputMatrixType::size_type size_type;
            typedef typename InputOutputMatrixType::pointer InputOutputPointerType;

            enum {
                ROWS = InputOutputMatrixType::ROWS,
                COLS = InputOutputMatrixType::COLS,
                COL_STRIDE = InputOutputMatrixType::COLSTRIDE,
                ROW_STRIDE = InputOutputMatrixType::ROWSTRIDE,
                STRIDE_TO_NEXT_ROW = ROW_STRIDE - COLS * COL_STRIDE
            };

            InputOutputPointerType inputOutputPointer = inputOutputMatrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                inputOutputPointer += STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    inputOutputPointer += COL_STRIDE)
                {
                    _elementOperationType::Operate(*inputOutputPointer, inputScalar);
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
            typedef typename InputMatrixType::size_type size_type;
            typedef typename InputMatrixType::const_pointer InputPointerType;

            OutputType incrementalResult = _incrementalOperationType::NeutralElement();

            enum {
                ROWS = InputMatrixType::ROWS,
                COLS = InputMatrixType::COLS,
                INPUT_COL_STRIDE = InputMatrixType::COLSTRIDE,
                INPUT_ROW_STRIDE = InputMatrixType::ROWSTRIDE,
                INPUT_STRIDE_TO_NEXT_ROW = INPUT_ROW_STRIDE - COLS * INPUT_COL_STRIDE
            };

            InputPointerType inputPointer = inputMatrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                inputPointer += INPUT_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    inputPointer += INPUT_COL_STRIDE)
                {
                    incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                        _elementOperationType::Operate(*inputPointer) );
                }
            }
            return incrementalResult;
        }  // Run method
    };  // SoMi class


    template<class _incrementalOperationType, class _elementOperationType>
    class SoMiMi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;

        template<class _input1MatrixType, class _input2MatrixType>
        static OutputType Run(const _input1MatrixType & input1Matrix,
                              const _input2MatrixType & input2Matrix)
        {
            typedef _input1MatrixType Input1MatrixType;
            typedef typename Input1MatrixType::size_type size_type;
            typedef typename Input1MatrixType::const_pointer Input1PointerType;
            typedef _input2MatrixType Input2MatrixType;
            typedef typename Input2MatrixType::const_pointer Input2PointerType;

            OutputType incrementalResult = _incrementalOperationType::NeutralElement();

            enum {
                ROWS = Input1MatrixType::ROWS,
                COLS = Input1MatrixType::COLS,
                INPUT1_COL_STRIDE = Input1MatrixType::COLSTRIDE,
                INPUT1_ROW_STRIDE = Input1MatrixType::ROWSTRIDE,
                INPUT1_STRIDE_TO_NEXT_ROW = INPUT1_ROW_STRIDE - COLS * INPUT1_COL_STRIDE
            };

            enum {
                INPUT2_COL_STRIDE = Input2MatrixType::COLSTRIDE,
                INPUT2_ROW_STRIDE = Input2MatrixType::ROWSTRIDE,
                INPUT2_STRIDE_TO_NEXT_ROW = INPUT2_ROW_STRIDE - COLS * INPUT2_COL_STRIDE
            };

            Input1PointerType input1Pointer = input1Matrix.Pointer();
            Input2PointerType input2Pointer = input2Matrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                input1Pointer += INPUT1_STRIDE_TO_NEXT_ROW,
                input2Pointer += INPUT2_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    input1Pointer += INPUT1_COL_STRIDE,
                    input2Pointer += INPUT2_COL_STRIDE)
                {
                    incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                        _elementOperationType::Operate(*input1Pointer, *input2Pointer) );
                }
            }
            return incrementalResult;
        }  // Run method
    };  // SoMiMi class


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
    class
        MioSiMi {
    public:

        template<class _ioMatrixType, class _inputScalarType, class _inputMatrixType>
            static inline void Run(_ioMatrixType & ioMatrix,
            const _inputScalarType & inputScalar, const _inputMatrixType & inputMatrix)
        {
            typedef _ioMatrixType IoMatrixType;
            typedef typename IoMatrixType::size_type size_type;
            typedef typename IoMatrixType::pointer IoPointerType;
            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::const_pointer InputPointerType;

            enum {
                ROWS = IoMatrixType::ROWS,
                COLS = IoMatrixType::COLS,
                IO_COL_STRIDE = IoMatrixType::COLSTRIDE,
                IO_ROW_STRIDE = IoMatrixType::ROWSTRIDE,
                IO_STRIDE_TO_NEXT_ROW = IO_ROW_STRIDE - COLS * IO_COL_STRIDE,
                INPUT_COL_STRIDE = InputMatrixType::COLSTRIDE,
                INPUT_ROW_STRIDE = InputMatrixType::ROWSTRIDE,
                INPUT_STRIDE_TO_NEXT_ROW = INPUT_ROW_STRIDE - COLS * INPUT_COL_STRIDE
            };

            IoPointerType ioPointer = ioMatrix.Pointer();
            InputPointerType inputPointer = inputMatrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                ioPointer += IO_STRIDE_TO_NEXT_ROW,
                inputPointer += INPUT_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    ioPointer += IO_COL_STRIDE,
                    inputPointer += INPUT_COL_STRIDE)
                {
                    _ioElementOperationType::Operate(
                        *ioPointer,
                        _scalarMatrixElementOperationType::Operate(inputScalar, *inputPointer)
                        );
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
    class
        MioMiMi {
    public:

        template<class _ioMatrixType, class _input1MatrixType, class _input2MatrixType>
        static inline void Run(_ioMatrixType & ioMatrix,
                               const _input1MatrixType & input1Matrix,
                               const _input2MatrixType & input2Matrix)
        {
            typedef _ioMatrixType IoMatrixType;
            typedef typename IoMatrixType::size_type size_type;
            typedef typename IoMatrixType::pointer IoPointerType;
            typedef _input1MatrixType Input1MatrixType;
            typedef typename Input1MatrixType::const_pointer Input1PointerType;
            typedef _input2MatrixType Input2MatrixType;
            typedef typename Input2MatrixType::const_pointer Input2PointerType;

            enum {
                ROWS = IoMatrixType::ROWS,
                COLS = IoMatrixType::COLS,
                IO_COL_STRIDE = IoMatrixType::COLSTRIDE,
                IO_ROW_STRIDE = IoMatrixType::ROWSTRIDE,
                IO_STRIDE_TO_NEXT_ROW = IO_ROW_STRIDE - COLS * IO_COL_STRIDE,
                INPUT1_COL_STRIDE = Input1MatrixType::COLSTRIDE,
                INPUT1_ROW_STRIDE = Input1MatrixType::ROWSTRIDE,
                INPUT1_STRIDE_TO_NEXT_ROW = INPUT1_ROW_STRIDE - COLS * INPUT1_COL_STRIDE,
                INPUT2_COL_STRIDE = Input2MatrixType::COLSTRIDE,
                INPUT2_ROW_STRIDE = Input2MatrixType::ROWSTRIDE,
                INPUT2_STRIDE_TO_NEXT_ROW = INPUT2_ROW_STRIDE - COLS * INPUT2_COL_STRIDE
            };

            IoPointerType ioPointer = ioMatrix.Pointer();
            Input1PointerType input1Pointer = input1Matrix.Pointer();
            Input2PointerType input2Pointer = input2Matrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                     ioPointer += IO_STRIDE_TO_NEXT_ROW,
                     input1Pointer += INPUT1_STRIDE_TO_NEXT_ROW,
                     input2Pointer += INPUT2_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                         ioPointer += IO_COL_STRIDE,
                         input1Pointer += INPUT1_COL_STRIDE,
                         input2Pointer += INPUT2_COL_STRIDE)
                {
                    _ioElementOperationType::Operate(
                        *ioPointer,
                        _matrixElementOperationType::Operate(*input1Pointer, *input2Pointer)
                        );
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
            typedef typename InputMatrixType::size_type size_type;
            typedef typename InputMatrixType::const_pointer InputPointerType;

            OutputType incrementalResult = _incrementalOperationType::NeutralElement();

            enum {
                ROWS = InputMatrixType::ROWS,
                COLS = InputMatrixType::COLS,
                INPUT_COL_STRIDE = InputMatrixType::COLSTRIDE,
                INPUT_ROW_STRIDE = InputMatrixType::ROWSTRIDE,
                INPUT_STRIDE_TO_NEXT_ROW = INPUT_ROW_STRIDE - COLS * INPUT_COL_STRIDE
            };

            InputPointerType inputPointer = inputMatrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                inputPointer += INPUT_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    inputPointer += INPUT_COL_STRIDE)
                {
                    incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                        _elementOperationType::Operate(*inputPointer, inputScalar) );
                }
            }
            return incrementalResult;
        }  // Run method
    };  // SoMiSi class



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
            typedef typename OutputMatrixType::pointer OutputPointerType;

            typedef _input1MatrixType Input1MatrixType;
            typedef typename Input1MatrixType::const_pointer Input1PointerType;

            typedef _input2MatrixType Input2MatrixType;
            typedef typename Input2MatrixType::const_pointer Input2PointerType;

            enum {
                ROWS = OutputMatrixType::ROWS,
                COLS = OutputMatrixType::COLS,
                OUTPUT_COL_STRIDE = OutputMatrixType::COLSTRIDE,
                OUTPUT_ROW_STRIDE = OutputMatrixType::ROWSTRIDE,
                OUTPUT_STRIDE_TO_NEXT_ROW = OUTPUT_ROW_STRIDE - COLS * OUTPUT_COL_STRIDE
            };

            enum {
                INPUT1_COL_STRIDE = Input1MatrixType::COLSTRIDE,
                INPUT1_ROW_STRIDE = Input1MatrixType::ROWSTRIDE
            };

            enum {
                INPUT2_COL_STRIDE = Input2MatrixType::COLSTRIDE,
                INPUT2_ROW_STRIDE = Input2MatrixType::ROWSTRIDE
            };

            OutputPointerType outputPointer = outputMatrix.Pointer();

            Input1PointerType input1Pointer = input1Matrix.Pointer();
            Input2PointerType input2Pointer = input2Matrix.Pointer();

            typename Input1MatrixType::ConstRowRefType input1Row;
            typename Input2MatrixType::ConstColumnRefType input2Col;

            if ((outputPointer == input1Pointer) ||
                (outputPointer == input2Pointer)) {
                ThrowSharedPointersException();
            }

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                outputPointer += OUTPUT_STRIDE_TO_NEXT_ROW,
                input1Pointer += INPUT1_ROW_STRIDE,
                input2Pointer = input2Matrix.Pointer())
            {
                input1Row.SetRef(input1Pointer);
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    outputPointer += OUTPUT_COL_STRIDE,
                    input2Pointer += INPUT2_COL_STRIDE)
                {
                    input2Col.SetRef(input2Pointer);
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
            typedef typename InputMatrixType::size_type size_type;
            typedef typename InputMatrixType::const_pointer InputPointerType;
            typedef typename InputMatrixType::value_type value_type;

            enum {
                ROWS = InputMatrixType::ROWS,
                COLS = InputMatrixType::COLS,
                INPUT_COL_STRIDE = InputMatrixType::COLSTRIDE,
                INPUT_ROW_STRIDE = InputMatrixType::ROWSTRIDE,
                INPUT_STRIDE_TO_NEXT_ROW = INPUT_ROW_STRIDE - COLS * INPUT_COL_STRIDE
            };

            InputPointerType inputPointer = inputMatrix.Pointer();
            if (inputPointer == 0)
                return;

            value_type minElement, maxElement;
            maxElement = minElement = *inputPointer;

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                inputPointer += INPUT_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    inputPointer += INPUT_COL_STRIDE)
                {
                    const value_type element = *inputPointer;
                    if (element < minElement) {
                        minElement = element;
                    }
                    else if (maxElement < element) {
                        maxElement = element;
                    }
                }
            }

            minValue = minElement;
            maxValue = maxElement;
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
            typedef typename OutputMatrixType::size_type size_type;
            typedef typename OutputMatrixType::pointer OutputPointerType;

            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::const_pointer InputPointerType;
            typedef _indexVectorType IndexVectorType;
            typedef typename IndexVectorType::const_pointer IndexPointerType;

            enum {
                ROWS = OutputMatrixType::ROWS,
                COLS = OutputMatrixType::COLS,
                OUTPUT_COL_STRIDE = OutputMatrixType::COLSTRIDE,
                OUTPUT_ROW_STRIDE = OutputMatrixType::ROWSTRIDE,
                OUTPUT_STRIDE_TO_NEXT_ROW = OUTPUT_ROW_STRIDE - COLS * OUTPUT_COL_STRIDE
            };

            enum {
                INPUT_COL_STRIDE = InputMatrixType::COLSTRIDE,
            };

            enum {
                INDEX_STRIDE = IndexVectorType::STRIDE,
            };

            OutputPointerType outputPointer = outputMatrix.Pointer();
            InputPointerType inputPointer = inputMatrix.Pointer();
            IndexPointerType indexPointer = indexVector.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                outputPointer += OUTPUT_STRIDE_TO_NEXT_ROW,
                indexPointer += INDEX_STRIDE)
            {
                inputPointer = inputMatrix.Pointer(*indexPointer, 0);
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    outputPointer += OUTPUT_COL_STRIDE,
                    inputPointer += INPUT_COL_STRIDE)
                {
                    *outputPointer = *inputPointer;
                }
            }
        }
    };


};


#endif  // _vctFixedSizeMatrixLoopEngines_h
