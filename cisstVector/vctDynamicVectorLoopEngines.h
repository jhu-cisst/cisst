/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on:  2004-07-01

  (C) Copyright 2004-2022 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctDynamicVectorLoopEngines_h
#define _vctDynamicVectorLoopEngines_h

/*!
  \file
  \brief Declaration of vctDynamicVectorLoopEngines
 */

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnThrow.h>
#include <cisstVector/vctDynamicCompactLoopEngines.h>

/*!
  \brief Container class for the vector loop based engines.

  Loop engines can be used for dynamic vectors (see vctDynamicVector)
  to apply similar operations (see vctBinaryOperations,
  vctUnaryOperations, vctStoreBackBinaryOperations,
  vctStoreBackUnaryOperations).  Each engine corresponds to an
  operation signature.

  Loop engines are named according to the type of the parameters
  and their role (i.e. input/output).  The order reflects the
  mathematical expression.  For exemple, VoViVi stands for one output
  vector with two input vectors used in \f$v_o = v_i op v_i\f$ and VioSi
  stands for one input/output vector and one scalar input used in
  \f$v_{io} = v_{io} op s_i\f$.

  The implementation is based on loops.  The inner class are templated
  by the operation type and allows to plug any operation with a given
  signature.

  All vector types must support the index operator ([]) to access
  their elements.  The input vector types must have it as const
  method. The output vector type must have it as non-const method.

  The method size() is also required.  For the operations based on two
  or more vectors, this method is used to check that the operands have
  the same size.  If the sizes don't match, an exception of type
  std::runtime_error is thrown.

  \sa vctFixedSizeVectorRecursiveEngines VoViVi VioVi VoViSi VoSiVi VioSi
  VoVi Vio SoVi SoViVi SoVoSi
*/
class vctDynamicVectorLoopEngines {

 public:

    /*! Helper function to throw an exception whenever sizes mismatch.
      This enforces that a standard message is sent. */
    inline static void ThrowException(const size_t expected, const size_t received) CISST_THROW(std::runtime_error) {
        std::stringstream message;
        message << "vctDynamicVectorLoopEngines: Sizes of vectors don't match, expected: " << expected
                << ", received: " << received;
        cmnThrow(std::runtime_error(message.str()));
    }


    /*!  \brief Implement operation of the form \f$v_o = op(v_{i1},
      v_{i2})\f$ for dynamic vectors.

      This class uses a loop to perform binary vector operations
      of the form
      \f[
      v_o = \mathrm{op}(v_{i1}, v_{i2})
      \f]

      where \f$v_o\f$ is the output vector, and \f$v_{i1}, v_{i2}\f$
      are input vectors, all of the same size, \em op stands for the a
      binary operation performed elementwise between
      \f$v_{i1}\f$ and \f$v_{i2}\f$, and whose result is stored
      elementwise into \f$v_o\f$.

      \param _elementOperationType The type of the binary operation.

      \sa vctDynamicVectorLoopEngines
    */
    template<class _elementOperationType>
    class VoViVi {
    public:
        /*! Unroll the loop
          \param outputVector The output vector.
          \param input1Vector The first input vector.
          \param input2Vector The second input vector.
        */
        template<class _outputVectorType, class _input1VectorType, class _input2VectorType>
        static inline void Run(_outputVectorType & outputVector,
                               const _input1VectorType & input1Vector,
                               const _input2VectorType & input2Vector) {
            // check sizes
            typedef _outputVectorType OutputVectorType;
            typedef typename OutputVectorType::OwnerType OutputOwnerType;
            typedef typename OutputOwnerType::pointer OutputPointerType;
            typedef typename OutputOwnerType::size_type size_type;
            typedef typename OutputOwnerType::stride_type stride_type;

            typedef _input1VectorType Input1VectorType;
            typedef typename Input1VectorType::OwnerType Input1OwnerType;
            typedef typename Input1OwnerType::const_pointer Input1PointerType;

            typedef _input2VectorType Input2VectorType;
            typedef typename Input2VectorType::OwnerType Input2OwnerType;
            typedef typename Input2OwnerType::const_pointer Input2PointerType;

            // retrieve owners
            const Input1OwnerType & input1Owner = input1Vector.Owner();
            const Input2OwnerType & input2Owner = input2Vector.Owner();
            OutputOwnerType & outputOwner = outputVector.Owner();

            const size_type size = outputOwner.size();
            if (size != input1Owner.size()) {
                ThrowException(size, input1Owner.size());
            } else if (size != input2Owner.size()) {
                ThrowException(size, input2Owner.size());
            }

            // if all are compact
            const stride_type outputStride = outputOwner.stride();
            const stride_type input1Stride = input1Owner.stride();
            const stride_type input2Stride = input2Owner.stride();

            if ((outputStride == 1) && (input1Stride == 1) && (input2Stride == 1)) {
                vctDynamicCompactLoopEngines::CoCiCi<_elementOperationType>::Run(outputOwner, input1Owner, input2Owner);
            } else {
                // otherwise
                OutputPointerType outputPointer = outputOwner.Pointer();
                const OutputPointerType outputEnd = outputPointer + size * outputStride;

                Input1PointerType input1Pointer = input1Owner.Pointer();
                Input2PointerType input2Pointer = input2Owner.Pointer();

                for (;
                     outputPointer != outputEnd;
                     outputPointer += outputStride,
                         input1Pointer += input1Stride,
                         input2Pointer += input2Stride) {
                    *outputPointer = _elementOperationType::Operate(*input1Pointer, *input2Pointer);
                }
            }
        }
    };


    /*!  \brief Implement operation of the form \f$v_{io} = op(v_{io},
      v_i)\f$ for dynamic vectors

      This class uses a loop to perform binary vector operations
      of the form
      \f[
      v_{io} = \mathrm{op}(v_{io}, v_{i})
      \f]

      where \f$v_{io}\f$ is the input output vector, and \f$v_{i}\f$
      is the second input vector, all of same size, \em op stands for
      the a binary operation performed elementwise between
      \f$v_{io}\f$ and \f$v_{i}\f$, and whose result is stored
      elementwise into \f$v_{io}\f$.

      \param _elementOperationType The type of the binary operation.

      \sa vctDynamicVectorLoopEngines
    */
    template<class _elementOperationType>
    class VioVi {
    public:
        /*! Unroll the loop

        \param inputOutputVector The input output vector.
        \param inputVector The second input vector.
        */
        template<class _inputOutputVectorType, class _inputVectorType>
        static void Run(_inputOutputVectorType & inputOutputVector,
                        const _inputVectorType & inputVector) {
            // check size
            typedef _inputOutputVectorType InputOutputVectorType;
            typedef typename InputOutputVectorType::OwnerType InputOutputOwnerType;
            typedef typename InputOutputOwnerType::pointer InputOutputPointerType;
            typedef typename InputOutputOwnerType::size_type size_type;
            typedef typename InputOutputOwnerType::stride_type stride_type;

            typedef _inputVectorType InputVectorType;
            typedef typename InputVectorType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owners
            const InputOwnerType & inputOwner = inputVector.Owner();
            InputOutputOwnerType & inputOutputOwner = inputOutputVector.Owner();

            const size_type size = inputOutputOwner.size();
            if (size != inputOwner.size()) {
                ThrowException(size, inputOwner.size());
            }

            const stride_type inputOutputStride = inputOutputOwner.stride();
            const stride_type inputStride = inputOwner.stride();

            if ((inputOutputStride == 1) && (inputStride == 1)) {
                vctDynamicCompactLoopEngines::CioCi<_elementOperationType>::Run(inputOutputOwner, inputOwner);
            } else {
                // otherwise
                InputOutputPointerType inputOutputPointer = inputOutputOwner.Pointer();
                const InputOutputPointerType inputOutputEnd = inputOutputPointer + size * inputOutputStride;

                InputPointerType inputPointer = inputOwner.Pointer();

                for (;
                     inputOutputPointer != inputOutputEnd;
                     inputOutputPointer += inputOutputStride,
                         inputPointer += inputStride) {
                    *inputOutputPointer = _elementOperationType::Operate(*inputOutputPointer, *inputPointer);
                }
            }
        }
    };


    /*!  \brief Implement operation of the form \f$(v_{1}, v_{2}) = op(v_{1},
      v_{2})\f$ for dynamic vectors

      This class uses a loop to perform binary vector operations
      of the form
      \f[
      (v_{1}, v_{2}) = \mathrm{op}(v_{1}, v_{2})
      \f]

      where both \f$v_{1}, v_{2}\f$ are input and output vectors of an
      equal size.  The operation is evaluated elementwise, that is,
      \f$(v_{1}[i], v_{2}[i] = \mathrm{op}(v_{1}[i], v_{2}[i])\f$.
      The typical operation in this case is swapping the elements of
      the two vectors.

      \param _elementOperationType The type of the binary operation that inputs and
      rewrites corresponding elements in both vectors.

      \sa vctDynamicVectorLoopEngines
    */
    template<class _elementOperationType>
    class VioVio {
    public:
        /*! Unroll the loop

        \param inputOutput1Vector The input output vector.
        \param inputOutput2Vector The second input vector.
        */
        template<class _inputOutput1VectorType, class _inputOutput2VectorType>
        static void Run(_inputOutput1VectorType & inputOutput1Vector,
                        _inputOutput2VectorType & inputOutput2Vector) {
            // check size
            typedef _inputOutput1VectorType InputOutput1VectorType;
            typedef typename InputOutput1VectorType::OwnerType InputOutput1OwnerType;
            typedef typename InputOutput1OwnerType::pointer InputOutput1PointerType;
            typedef typename InputOutput1OwnerType::size_type size_type;
            typedef typename InputOutput1OwnerType::stride_type stride_type;

            typedef _inputOutput2VectorType InputOutput2VectorType;
            typedef typename InputOutput2VectorType::OwnerType InputOutput2OwnerType;
            typedef typename InputOutput2OwnerType::pointer InputOutput2PointerType;

            // retrieve owners
            InputOutput1OwnerType & inputOutput1Owner = inputOutput1Vector.Owner();
            InputOutput2OwnerType & inputOutput2Owner = inputOutput2Vector.Owner();

            const size_type size = inputOutput1Owner.size();
            if (size != inputOutput2Owner.size()) {
                ThrowException(size, inputOutput2Owner.size());
            }

            const stride_type inputOutput1Stride = inputOutput1Owner.stride();
            const stride_type inputOutput2Stride = inputOutput2Owner.stride();

            if ((inputOutput1Stride == 1) && (inputOutput2Stride == 1)) {
                vctDynamicCompactLoopEngines::CioCio<_elementOperationType>::Run(inputOutput1Owner, inputOutput2Owner);
            } else {
                // otherwise
                InputOutput1PointerType inputOutput1Pointer = inputOutput1Owner.Pointer();
                const InputOutput1PointerType inputOutput1End = inputOutput1Pointer + size * inputOutput1Stride;

                InputOutput2PointerType inputOutput2Pointer = inputOutput2Owner.Pointer();

                for (;
                     inputOutput1Pointer != inputOutput1End;
                     inputOutput1Pointer += inputOutput1Stride,
                         inputOutput2Pointer += inputOutput2Stride) {
                    _elementOperationType::Operate(*inputOutput1Pointer, *inputOutput2Pointer);
                }
            }
        }
    };


    /*!  \brief Implement operation of the form \f$vo = op(vi, si)\f$
      for dynamic vectors

      This class uses a loop to perform binary vector operations
      of the form
      \f[
      v_o = \mathrm{op}(v_i, s_i)
      \f]

      where \f$v_o\f$ is the output vector, and \f$v_i, s_i\f$ are
      input vector and scalar, all vectors are of same size, \em op
      stands for the a binary operation performed elementwise
      between \f$v_i\f$ and \f$s_i\f$, and whose result is stored
      elementwise into \f$v_o\f$.

      \param _elementOperationType The type of the binary operation.

      \sa vctDynamicVectorLoopEngines
    */
    template<class _elementOperationType>
        class VoViSi {
        public:
        /*! Unroll the loop

        \param outputVector The output vector.
        \param inputVector The input vector (first operand).
        \param inputScalar The input scalar (second operand).
        */
        template<class _outputVectorType, class _inputVectorType, class _inputScalarType>
        static inline void Run(_outputVectorType & outputVector,
                               const _inputVectorType & inputVector,
                               const _inputScalarType inputScalar) {
            // check sizes
            typedef _outputVectorType OutputVectorType;
            typedef typename OutputVectorType::OwnerType OutputOwnerType;
            typedef typename OutputOwnerType::pointer OutputPointerType;
            typedef typename OutputOwnerType::size_type size_type;
            typedef typename OutputOwnerType::stride_type stride_type;

            typedef _inputVectorType InputVectorType;
            typedef typename InputVectorType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owners
            const InputOwnerType & inputOwner = inputVector.Owner();
            OutputOwnerType & outputOwner = outputVector.Owner();

            const size_type size = outputOwner.size();
            if (size != inputOwner.size()) {
                ThrowException(size, inputOwner.size());
            }

            const stride_type outputStride = outputOwner.stride();
            const stride_type inputStride = inputOwner.stride();

            if ((outputStride == 1) && (inputStride == 1)) {
                vctDynamicCompactLoopEngines::CoCiSi<_elementOperationType>::Run(outputOwner, inputOwner, inputScalar);
            } else {
                // otherwise
                OutputPointerType outputPointer = outputOwner.Pointer();
                const OutputPointerType outputEnd = outputPointer + size * outputStride;

                InputPointerType inputPointer = inputOwner.Pointer();

                for (;
                     outputPointer != outputEnd;
                     outputPointer += outputStride,
                         inputPointer += inputStride) {
                    *outputPointer = _elementOperationType::Operate(*inputPointer, inputScalar);
                }
            }
        }
    };


    /*!  \brief Implement operation of the form \f$vo = op(si, vi)\f$ for
      dynamic vectors

      This class uses a loop to perform binary vector operations
      of the form
      \f[
      v_o = \mathrm{op}(s_i, v_i)
      \f]

      where \f$v_o\f$ is the output vector, and \f$s_i, v_i\f$ are
      input scalar and vector, all vectors are of same size, \em op
      stands for the a binary operation performed elementwise between
      \f$s_i\f$ and \f$v_i\f$, and whose result is stored elementwise
      into \f$v_o\f$.

      \param _elementOperationType The type of the binary operation.

      \sa vctDynamicVectorLoopEngines
    */
    template<class _elementOperationType>
        class VoSiVi {
        public:
        /*! Unroll the loop

        \param outputVector The output vector.
        \param inputScalar The input scalar (first operand).
        \param inputVector The input vector (second operand).
        */
        template<class _outputVectorType, class _inputScalarType, class _inputVectorType>
            static inline void Run(_outputVectorType & outputVector,
                                   const _inputScalarType inputScalar,
                                   const _inputVectorType & inputVector) {
            // check sizes
            typedef _outputVectorType OutputVectorType;
            typedef typename OutputVectorType::OwnerType OutputOwnerType;
            typedef typename OutputOwnerType::pointer OutputPointerType;
            typedef typename OutputOwnerType::size_type size_type;
            typedef typename OutputOwnerType::stride_type stride_type;

            typedef _inputVectorType InputVectorType;
            typedef typename InputVectorType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owners
            const InputOwnerType & inputOwner = inputVector.Owner();
            OutputOwnerType & outputOwner = outputVector.Owner();

            const size_type size = outputOwner.size();
            if (size != inputOwner.size()) {
                ThrowException(size, inputOwner.size());
            }

            const stride_type outputStride = outputOwner.stride();
            const stride_type inputStride = inputOwner.stride();

            if ((outputStride == 1) && (inputStride == 1)) {
                vctDynamicCompactLoopEngines::CoSiCi<_elementOperationType>::Run(outputOwner, inputScalar, inputOwner);
            } else {
                // otherwise
                OutputPointerType outputPointer = outputOwner.Pointer();
                const OutputPointerType outputEnd = outputPointer + size * outputStride;

                InputPointerType inputPointer = inputOwner.Pointer();

                for (;
                     outputPointer != outputEnd;
                     outputPointer += outputStride,
                         inputPointer += inputStride) {
                    *outputPointer = _elementOperationType::Operate(inputScalar, *inputPointer);
                }
            }
        }
    };


    /*!  \brief Implement operation of the form \f$ v_{io} =
      op(v_{io}, s_i)\f$ for dynamic vectors

      This class uses a loop to perform binary vector
      operations of the form

      \f[
      v_{io} = \mathrm{op}(v_{io}, s_{i})
      \f]

      where \f$v_{io}\f$ is the input output vector, and \f$s_{i}\f$
      is the scalar input.  \em op stands for the binary operation
      performed elementwise between \f$v_{io}[index]\f$ and
      \f$s_{i}\f$, and whose result is stored elementwise into
      \f$v_{io}[index]\f$.

      \param _elementOperationType the type of the binary operation

      \sa vctDynamicVectorLoopEngines
    */
    template<class _elementOperationType>
        class VioSi {
        public:
        /*! Unroll the loop

        \param inputOutputVector The input output vector (first operand).
        \param inputScalar The input scalar (second operand).
        */
        template<class _inputOutputVectorType, class _inputScalarType>
        static void Run(_inputOutputVectorType & inputOutputVector,
                        const _inputScalarType inputScalar) {

            typedef _inputOutputVectorType InputOutputVectorType;
            typedef typename InputOutputVectorType::OwnerType InputOutputOwnerType;
            typedef typename InputOutputOwnerType::pointer InputOutputPointerType;
            typedef typename InputOutputOwnerType::size_type size_type;
            typedef typename InputOutputOwnerType::stride_type stride_type;

            InputOutputOwnerType & inputOutputOwner = inputOutputVector.Owner();
            const size_type size = inputOutputOwner.size();
            const stride_type inputOutputStride = inputOutputOwner.stride();

            if (inputOutputStride == 1) {
                vctDynamicCompactLoopEngines::CioSi<_elementOperationType>::Run(inputOutputOwner, inputScalar);
            } else {
                InputOutputPointerType inputOutputPointer = inputOutputOwner.Pointer();
                const InputOutputPointerType inputOutputEnd = inputOutputPointer + size * inputOutputStride;

                for (;
                     inputOutputPointer != inputOutputEnd;
                     inputOutputPointer += inputOutputStride) {
                    _elementOperationType::Operate(*inputOutputPointer, inputScalar);
                }
            }
        }
    };


    /*!  \brief Implement operation of the form \f$v_o = op(v_i)\f$
      for dynamic vectors.

      This class uses a loop to perform unary vector operations
      of the form
      \f[
      v_{o} = \mathrm{op}(v_{i})
      \f]

      where \f$v_{o}\f$ is the output vector, and \f$v_{i}\f$ is the
      input vector both of same size, \em op stands for the a unary
      operation performed elementwise on \f$v_{i}\f$, and whose result
      is stored elementwise into \f$v_{o}\f$.

      \param _elementOperationType The type of the unary operation.

      \sa vctDynamicVectorLoopEngines
    */
    template<class _elementOperationType>
    class VoVi {
    public:
        /*! Unroll the loop

        \param outputVector The output vector.
        \param inputVector The input vector.
        */
        template<class _outputVectorType, class _inputVectorType>
        static inline void Run(_outputVectorType & outputVector,
                               const _inputVectorType & inputVector) {
            // check sizes
            typedef _outputVectorType OutputVectorType;
            typedef typename OutputVectorType::OwnerType OutputOwnerType;
            typedef typename OutputOwnerType::pointer OutputPointerType;
            typedef typename OutputOwnerType::size_type size_type;
            typedef typename OutputOwnerType::stride_type stride_type;

            typedef _inputVectorType InputVectorType;
            typedef typename InputVectorType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owners
            const InputOwnerType & inputOwner = inputVector.Owner();
            OutputOwnerType & outputOwner = outputVector.Owner();

            const size_type size = outputOwner.size();
            if (size != inputOwner.size()) {
                ThrowException(size, inputOwner.size());
            }

            // if both are compact
            const stride_type outputStride = outputOwner.stride();
            const stride_type inputStride = inputOwner.stride();

            if ((outputStride == 1) && (inputStride == 1)) {
                vctDynamicCompactLoopEngines::CoCi<_elementOperationType>::Run(outputOwner, inputOwner);
            } else {
                // otherwise
                OutputPointerType outputPointer = outputOwner.Pointer();
                const OutputPointerType outputEnd = outputPointer + size * outputStride;

                InputPointerType inputPointer = inputOwner.Pointer();

                for (;
                     outputPointer != outputEnd;
                     outputPointer += outputStride,
                         inputPointer += inputStride) {
                    *outputPointer = _elementOperationType::Operate(*inputPointer);
                }
            }
        }
    };



    /*!  \brief Implement operation of the form \f$v_{io} =
      op(v_{io})\f$ for dynamic vectors

      This class uses a loop to perform unary store back vector
      operations of the form

      \f[
      v_{io} = \mathrm{op}(v_{io})
      \f]

      where \f$v_{io}\f$ is the input output vector.  \em op stands
      for the unary operation performed elementwise on \f$v_{io}\f$
      and whose result is stored elementwise into \f$v_{io}[index]\f$.

      \param _elementOperationType The type of the unary operation.

      \sa vctDynamicVectorLoopEngines
    */
    template<class _elementOperationType>
    class Vio {
    public:
        /*! Unroll the loop

        \param inputOutputVector The input output vector.
        */
        template<class _inputOutputVectorType>
        static inline void Run(_inputOutputVectorType & inputOutputVector) {

            typedef _inputOutputVectorType InputOutputVectorType;
            typedef typename InputOutputVectorType::OwnerType InputOutputOwnerType;
            typedef typename InputOutputOwnerType::pointer InputOutputPointerType;
            typedef typename InputOutputOwnerType::size_type size_type;
            typedef typename InputOutputOwnerType::stride_type stride_type;

            InputOutputOwnerType & inputOutputOwner = inputOutputVector.Owner();
            const size_type size = inputOutputOwner.size();
            const stride_type inputOutputStride = inputOutputOwner.stride();

            if (inputOutputStride == 1) {
                vctDynamicCompactLoopEngines::Cio<_elementOperationType>::Run(inputOutputOwner);
            } else {
                InputOutputPointerType inputOutputPointer = inputOutputOwner.Pointer();
                const InputOutputPointerType inputOutputEnd = inputOutputPointer + size * inputOutputStride;

                for (;
                     inputOutputPointer != inputOutputEnd;
                     inputOutputPointer += inputOutputStride) {
                    _elementOperationType::Operate(*inputOutputPointer);
                }
            }
        }
    };



    /*!  \brief Implement operation of the form \f$s_o =
      op_{incr}(op(v_i))\f$ for dynamic vectors

      This class uses a loop to perform incremental
      unary vector operations of the form

      \f[
      s_o = \mathrm{op_{incr}(\mathrm{op}(v_i))}
      \f]

      where \f$v_i\f$ is the input vector and \f$s_o\f$ is the scalar
      output.  \em op stands for the unary operation performed
      elementwise on \f$v_i\f$ and whose result are used incrementally
      as input for \f$op_{incr}\f$.  For a vector of size 3, the
      result is \f$s_o = op_{incr}(op_{incr}(op(v[1]), op(v[0])),
      op(v[2])) \f$.

      \param _elementOperationType The type of the unary operation.

      \sa vctDynamicVectorLoopEngines
    */
    template<class _incrementalOperationType, class _elementOperationType>
    class SoVi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;
        /*! Unroll the loop

        \param inputVector The input vector.
        */
        template<class _inputVectorType>
        static OutputType Run(const _inputVectorType & inputVector) {

            typedef _inputVectorType InputVectorType;
            typedef typename InputVectorType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;
            typedef typename InputOwnerType::size_type size_type;
            typedef typename InputOwnerType::stride_type stride_type;

            const InputOwnerType & inputOwner = inputVector.Owner();
            const size_type size = inputOwner.size();
            const stride_type inputStride = inputOwner.stride();

            if (inputStride == 1) {
                return vctDynamicCompactLoopEngines::SoCi<_incrementalOperationType, _elementOperationType>::Run(inputOwner);
            } else {
                OutputType incrementalResult = _incrementalOperationType::NeutralElement();
                InputPointerType inputPointer = inputOwner.Pointer();
                const InputPointerType inputEnd = inputPointer + size * inputStride;

                for (;
                     inputPointer != inputEnd;
                     inputPointer += inputStride) {
                    incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                                                                           _elementOperationType::Operate(*inputPointer));
                }
                return incrementalResult;
            }
        }
    };


    /*!  \brief Implement operation of the form \f$s_o =
      op_{incr}(op(v_{i1}, v_{i2}))\f$ for dynamic vectors

      This class uses a loop to perform incremental
      binary vector operations of the form

      \f[
      s_o = \mathrm{op_{incr}(\mathrm{op}(v_{i1}, v_{i2}))}
      \f]

      where \f$v_{i1}\f$ and \f$v_{i2}\f$ are the input vectors and
      \f$s_o\f$ is the scalar output.  The vectors have the same size,
      \em op stands for the unary operation performed elementwise on
      \f$v_{i1}\f$ and \f$v_{i2}\f$ and whose result are used
      incrementally as input for \f$op_{incr}\f$.  For a vector of
      size 3, the result is \f$s_o = op_{incr}(op_{incr}(op(v1[1],
      v2[1]), op(v1[0], v2[0])), op(v1[2], v2[2]))\f$.

      \param _incrementalOperationType The type of the incremental
      operation.

      \param _elementOperationType The type of the unary operation.

      \sa vctFixedSizeVectorRecursiveEngines
    */
    template<class _incrementalOperationType, class _elementOperationType>
    class SoViVi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;
        /*! Unroll the loop

        \param input1Vector The first input vector (first operand for
        _elementOperationType).

        \param input2Vector The second input vector (second operand
        for _elementOperationType).
        */
        template<class _input1VectorType, class _input2VectorType>
        static inline OutputType Run(const _input1VectorType & input1Vector,
                                     const _input2VectorType & input2Vector) {
            // check sizes
            typedef _input1VectorType Input1VectorType;
            typedef typename Input1VectorType::OwnerType Input1OwnerType;
            typedef typename Input1OwnerType::const_pointer Input1PointerType;
            typedef typename Input1OwnerType::size_type size_type;
            typedef typename Input1OwnerType::stride_type stride_type;

            typedef _input2VectorType Input2VectorType;
            typedef typename Input2VectorType::OwnerType Input2OwnerType;
            typedef typename Input2OwnerType::const_pointer Input2PointerType;

            // retrieve owners
            const Input1OwnerType & input1Owner = input1Vector.Owner();
            const Input2OwnerType & input2Owner = input2Vector.Owner();

            const size_type size = input1Owner.size();
            if (size != input2Owner.size()) {
                ThrowException(size, input2Owner.size());
            }

            const stride_type input1Stride = input1Owner.stride();
            const stride_type input2Stride = input2Owner.stride();

            if ((input1Stride == 1) && (input2Stride == 1)) {
                return vctDynamicCompactLoopEngines::SoCiCi<_incrementalOperationType, _elementOperationType>::Run(input1Owner, input2Owner);
            } else {
                // otherwise
                OutputType incrementalResult = _incrementalOperationType::NeutralElement();
                Input1PointerType input1Pointer = input1Owner.Pointer();
                const Input1PointerType input1End = input1Pointer + size * input1Stride;

                Input2PointerType input2Pointer = input2Owner.Pointer();

                for (;
                     input1Pointer != input1End;
                     input1Pointer += input1Stride,
                         input2Pointer += input2Stride) {
                    incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                                                                           _elementOperationType::Operate(*input1Pointer,
                                                                                                          *input2Pointer));
                }
                return incrementalResult;
            }
            // last return added to avoid buggy warning with gcc 4.0,
            // this should never be evaluated
#if (CISST_OS != CISST_WINDOWS)
            // Some versions of Windows compilers treat unreachable code as an error
            return _incrementalOperationType::NeutralElement();
#endif
        }
    };


    /*!  \brief Implement operation of the form \f$v_{io} =
      op_{io}(v_{io}, op_{sv}(s, v_i))\f$ for dynamic vectors

      This class uses template specialization to perform store-back
      vector-scalar-vector operations

      \f[
      v_{io} = \mathrm{op_{io}}(V_{io}, \mathrm{op_{sv}}(s, v_i))
      \f]

      where \f$v_{io}\f$ is an input-output (store-back) vector;
      \f$s\f$ is a scalar; and \f$v_i\f$ is an input vector.  A
      typical example is: \f$v_{io} += s \cdot v_i\f$.  \f$op_{sv}\f$
      is an operation between \f$s\f$ and the elements of \f$v_i\f$;
      \f$op_{io}\f$ is an operation between the output of
      \f$op_{sv}\f$ and the elements of \f$v_{io}\f$.

      \param _ioOperationType The type of the store-back operation.

      \param _scalarVectorElementOperationType The type of the
      operation between scalar and input vector.

    */
    template<class _ioElementOperationType, class _scalarVectorElementOperationType>
    class VioSiVi {
    public:
        template<class _ioVectorType, class _inputScalarType, class _inputVectorType>
        static inline void Run(_ioVectorType & ioVector,
                               const _inputScalarType inputScalar, const _inputVectorType & inputVector)
        {
            // check sizes
            typedef _ioVectorType IoVectorType;
            typedef typename IoVectorType::OwnerType IoOwnerType;
            typedef typename IoOwnerType::pointer IoPointerType;
            typedef typename IoOwnerType::size_type size_type;
            typedef typename IoOwnerType::stride_type stride_type;

            typedef _inputVectorType InputVectorType;
            typedef typename InputVectorType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            // retrieve owners
            IoOwnerType & ioOwner = ioVector.Owner();
            const InputOwnerType & inputOwner = inputVector.Owner();

            const size_type size = ioOwner.size();
            if (size != inputOwner.size()) {
                ThrowException(size, inputOwner.size());
            }

            const stride_type ioStride = ioOwner.stride();
            const stride_type inputStride = inputOwner.stride();

            if ((ioStride == 1) && (inputStride == 1)) {
                vctDynamicCompactLoopEngines::CioSiCi<_ioElementOperationType, _scalarVectorElementOperationType>
                    ::Run(ioOwner, inputScalar, inputOwner);
            } else {
                IoPointerType ioPointer = ioOwner.Pointer();
                const IoPointerType ioEnd = ioPointer + size * ioStride;

                InputPointerType inputPointer = inputOwner.Pointer();

                for (;
                     ioPointer != ioEnd;
                     ioPointer += ioStride,
                         inputPointer += inputStride) {
                    _ioElementOperationType::Operate(*ioPointer,
                                                     _scalarVectorElementOperationType::Operate(inputScalar, *inputPointer)
                                                     );
                }
            }
        }
    };


    /*!  \brief Implement operation of the form \f$v_{io} =
      op_{io}(v_{io}, op_{v}(v_{i1}, v_{i2}))\f$ for dynamic vectors

      This class uses template specialization to perform store-back
      vector-vector-vector operations

      \f[
      v_{io} = \mathrm{op_{io}}(V_{io}, \mathrm{op_{vv}}(v_{i1}, v_{i2}))
      \f]

      where \f$v_{io}\f$ is an input-output (store-back) vector;
      \f$v_{i1}\f$ and \f$v_{i2}\f$ are two input vectors.  A
      typical example is: \f$v_{io} += v_{i1} \cdot v_{i2}\f$.  \f$op_{vv}\f$
      is an operation between the elements of \f$v_{i1}\f$ and \f$v_{i2}\f$;
      \f$op_{io}\f$ is an operation between the output of
      \f$op_{vv}\f$ and the elements of \f$v_{io}\f$.

      \param _ioOperationType The type of the store-back operation.

      \param _scalarVectorElementOperationType The type of the
      element wise operation between input vectors.

    */
    template<class _ioElementOperationType, class _vectorElementOperationType>
    class VioViVi {
    public:
        template<class _ioVectorType, class _input1VectorType, class _input2VectorType>
        static inline void Run(_ioVectorType & ioVector,
                               const _input1VectorType & input1Vector, const _input2VectorType & input2Vector)
        {
            // check sizes
            typedef _ioVectorType IoVectorType;
            typedef typename IoVectorType::OwnerType IoOwnerType;
            typedef typename IoOwnerType::pointer IoPointerType;
            typedef typename IoOwnerType::size_type size_type;
            typedef typename IoOwnerType::stride_type stride_type;

            typedef _input1VectorType Input1VectorType;
            typedef typename Input1VectorType::OwnerType Input1OwnerType;
            typedef typename Input1OwnerType::const_pointer Input1PointerType;

            typedef _input2VectorType Input2VectorType;
            typedef typename Input2VectorType::OwnerType Input2OwnerType;
            typedef typename Input2OwnerType::const_pointer Input2PointerType;

            // retrieve owners
            IoOwnerType & ioOwner = ioVector.Owner();
            const Input1OwnerType & input1Owner = input1Vector.Owner();
            const Input2OwnerType & input2Owner = input2Vector.Owner();

            const size_type size = ioOwner.size();
            if (size != input1Owner.size()) {
                ThrowException(size, input1Owner.size());
            } else if (size != input2Owner.size()) {
                ThrowException(size, input2Owner.size());
            }

            const stride_type ioStride = ioOwner.stride();
            const stride_type input1Stride = input1Owner.stride();
            const stride_type input2Stride = input2Owner.stride();

            if ((ioStride == 1) && (input1Stride == 1) && (input2Stride == 1)) {
                vctDynamicCompactLoopEngines::CioCiCi<_ioElementOperationType, _vectorElementOperationType>
                    ::Run(ioOwner, input1Owner, input2Owner);
            } else {
                IoPointerType ioPointer = ioOwner.Pointer();
                const IoPointerType ioEnd = ioPointer + size * ioStride;

                Input1PointerType input1Pointer = input1Owner.Pointer();
                Input2PointerType input2Pointer = input2Owner.Pointer();

                for (;
                     ioPointer != ioEnd;
                     ioPointer += ioStride,
                         input1Pointer += input1Stride,
                         input2Pointer += input2Stride) {
                    _ioElementOperationType::Operate(*ioPointer,
                                                     _vectorElementOperationType::Operate(*input1Pointer, *input2Pointer)
                                                     );
                }
            }
        }
    };


    /*!  \brief Implement operation of the form \f$s_o =
      op_{incr}(op(v_i, s_i))\f$ for dynamic vectors

      This class uses a loop to perform incremental
      binary vector operations of the form

      \f[
      s_o = \mathrm{op_{incr}(\mathrm{op}(v_i, s_i))}
      \f]

      where \f$v_i\f$ and \f$s_i\f$ are the input vector and scalar
      and \f$s_o\f$ is the scalar output. \em op stands for the
      unary operation performed elementwise on \f$v_i\f$
      and \f$s_i\f$ and whose result are used incrementally as
      input for \f$op_{incr}\f$.  For a vector of size 3, the result
      is \f$s_o = op_{incr}(op_{incr}(op(v[1], s), op(v[0],
      s)), op(v, s))\f$.

      \param _incrementalOperationType The type of the incremental
      operation.

      \param _elementOperationType The type of the unary operation.

      \sa vctDynamicVectorLoopEngines
    */
    template<class _incrementalOperationType, class _elementOperationType>
    class SoViSi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;
        /*! Unroll the loop.

        \param inputVector The input vector (first operand for
        _elementOperationType).

        \param inputScalar The input scalar (second operand
        for _elementOperationType).
        */
        template<class _inputVectorType, class _inputScalarType>
        static inline OutputType Run(const _inputVectorType & inputVector,
                                     const _inputScalarType & inputScalar) {

            typedef _inputVectorType InputVectorType;
            typedef typename InputVectorType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;
            typedef typename InputOwnerType::size_type size_type;
            typedef typename InputOwnerType::stride_type stride_type;

            const InputOwnerType & inputOwner = inputVector.Owner();
            const size_type size = inputOwner.size();
            OutputType incrementalResult = _incrementalOperationType::NeutralElement();

            const stride_type inputStride = inputOwner.stride();

            if (inputStride == 1) {
                return vctDynamicCompactLoopEngines::SoCiSi<_incrementalOperationType, _elementOperationType>
                    ::Run(inputOwner, inputScalar);
            } else {
                InputPointerType inputPointer = inputOwner.Pointer();
                const InputPointerType inputEnd = inputPointer + size * inputStride;

                for (;
                     inputPointer != inputEnd;
                     inputPointer += inputStride) {
                    incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                                                                           _elementOperationType::Operate(*inputPointer, inputScalar));
                }
                return incrementalResult;
            }
        }
    };

    class MinAndMax {
    public:
        /*! Unroll the loop

        \param inputVector The input vector.

        \param minValue The minimum value returned.  Passed by reference.

        \param maxValue The maximum value returned.  Passed by reference.
        */
        template<class _inputVectorType>
        static void Run(const _inputVectorType & inputVector, typename _inputVectorType::value_type & minValue,
                        typename _inputVectorType::value_type & maxValue)
        {
            typedef _inputVectorType InputVectorType;
            typedef typename InputVectorType::OwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;
            typedef typename InputOwnerType::size_type size_type;
            typedef typename InputOwnerType::stride_type stride_type;
            typedef typename InputOwnerType::value_type value_type;

            const InputOwnerType & inputOwner = inputVector.Owner();
            InputPointerType inputPointer = inputOwner.Pointer();

            if (inputPointer == 0)
                return;

            const stride_type inputStride = inputOwner.stride();
            if (inputStride == 1) {
                vctDynamicCompactLoopEngines::MinAndMax::Run(inputOwner, minValue, maxValue);
            } else {
                const size_type size = inputOwner.size();
                const InputPointerType inputEnd = inputPointer + size * inputStride;

                value_type minElement, maxElement;
                maxElement = minElement = *inputPointer;

                for (;
                     inputPointer != inputEnd;
                     inputPointer += inputStride)
                    {
                        const value_type element = *inputPointer;
                        if (element < minElement) {
                            minElement = element;
                        }
                        else if (maxElement < element) {
                            maxElement = element;
                        }
                    }

                minValue = minElement;
                maxValue = maxElement;
            }
        }
    };


    class SelectByIndex {
    public:
        template <class _outputVectorType, class _inputVectorType, class _indexVectorType>
        static void Run(_outputVectorType & output,
                        const _inputVectorType & input,
                        const _indexVectorType & index)
        {
            // check sizes
            typedef _outputVectorType OutputVectorType;
            typedef typename OutputVectorType::pointer OutputPointerType;
            typedef typename OutputVectorType::size_type size_type;
            typedef typename OutputVectorType::stride_type stride_type;

            typedef _inputVectorType InputVectorType;
            typedef typename InputVectorType::const_pointer InputPointerType;

            typedef _indexVectorType IndexVectorType;
            typedef typename IndexVectorType::const_pointer IndexPointerType;

            const size_type size = output.size();
            if (size != index.size()) {
                ThrowException(size, index.size());
            }

            const stride_type outputStride = output.stride();
            const stride_type indexStride = index.stride();

            OutputPointerType outputPointer = output.Pointer();
            const OutputPointerType outputEnd = outputPointer + size * outputStride;

            IndexPointerType indexPointer = index.Pointer();
            InputPointerType inputPointer;

            for (;
                 outputPointer != outputEnd;
                 outputPointer += outputStride,
                     indexPointer += indexStride) {
                inputPointer = input.Pointer(*indexPointer);
                *outputPointer = *inputPointer;
            }
        }
    };

};



#endif  // _vctDynamicVectorLoopEngines_h
