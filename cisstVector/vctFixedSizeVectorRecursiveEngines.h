/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on: 2003-08-18

  (C) Copyright 2003-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctFixedSizeVectorRecursiveEngines_h
#define _vctFixedSizeVectorRecursiveEngines_h

/*!
  \file
  \brief Declaration of vctFixedSizeVectorRecursiveEngines
 */


/*!
  \brief Container class for the recursive engines.

  Recursive engines can be used for fixed size vectors (see
  vctFixedSizeVector) to apply similar operations (see
  vctBinaryOperations, vctUnaryOperations,
  vctStoreBackBinaryOperations, vctStoreBackUnaryOperations).  Each
  engine corresponds to an operation signature.

  Recursive engines are named according to the type of the parameters
  and their role (i.e. input/output).  The order reflects the
  mathematical expression.  For exemple, VoViVi stands for one output
  vector with two input vectors used in \f$v_o = v_i op v_i\f$ and VioSi
  stands for one input/output vector and one scalar input used in
  \f$v_{io} = v_{io} op s_i\f$.

  The implementation is based on recursive templates which means that
  the recursion is not translated into recursive calls in an
  executable but the compilers "unfold" the code during the
  compilation.  The templated format used here was somewhat imposed by
  the different compilers supported in cisst.  Mainly, Visual C++ 6
  doesn't support partial specialization.  Therefore, the size
  parameter has to be sub-templated.  It seems that the ANSI C++
  forbids the specialization of a sub-templated class (inner template)
  if the outer template is not specialized.  Therefore, the size
  parameter must be used to template the outer class,
  i.e. vctFixedSizeVectorRecursiveEngines.  The inner class are templated by the
  operation type and allows to plug any operation with a given
  signature.

  The operation is implemented inline using ``template
  metaprogramming'' in a recursive form.  The order of evaluation is
  from index 0 to index n-1.  The operation has to be declared as a
  static function, which means it cannot use a third object to contain
  additional data, and it has to be defined in compilation time.

  All vector types must support operator[](unsigned int) to access
  their elements.  The input vector types must have it as const
  method. The output vector type must have it as non-const method.

  \param size the size of the vector (determined at compilation time).

  \sa VoViVi VioVi VoViSi VoSiVi VioSi VoVi Vio SoVi SoViVi SoVoSi
*/
template <vct::size_type _size>
class vctFixedSizeVectorRecursiveEngines {

 public:

    /*!  \brief Implement operation of the form \f$v_o = op(v_{i1},
      v_{i2})\f$ for fixed size vectors

      This class uses template specialization to perform binary vector operations
      of the form
      \f[
      v_o = \mathrm{op}(v_{i1}, v_{i2})
      \f]

      where \f$v_o\f$ is the output vector, and \f$v_{i1}, v_{i2}\f$
      are input vectors, all of an equal fixed size, determined at
      compilation time, \em op stands for the a binary operation
      performed elementwise between \f$v_{i1}\f$ and \f$v_{i2}\f$, and
      whose result is stored elementwise into \f$v_o\f$.

      The operation type operationType must have a static method with
      the signature

      <PRE>
      static operationType Operate(const I1E & arg1, const I2E & arg2)
      </PRE>

      where OE, I1E, I2E are the types of vector elements in \f$v_o,
      v_{i1} v_{i2}\f$ respectively.  These types do not need to be
      defined explicitly for \em vctFixedSizeVectorRecursiveEngines class, as they
      are inferred from the signature of the corresponding operator[]
      for each of the vector types.  However, they may need to be
      defined for the class operationType.

      For examples of binary operations, see vctBinaryOperations.h.

      Usage example for vctFixedSizeVectorRecursiveEngines::VoViVi:
      <PRE>
      enum {SIZE = 3};
      int input1[SIZE];
      float input2[SIZE];
      double output[SIZE];

      vctFixedSizeVectorRecursiveEngines<SIZE>::VoViVi<vctBinaryOperations<int,float,double>::Addition, SIZE>::Unfold(output, input1, input2);
      </PRE>
      stores the sum of elements from input1 and input2 into output.

      \param _elementOperationType The type of the binary operation.

      \sa vctFixedSizeVectorRecursiveEngines
    */
    template <class _elementOperationType>
    class VoViVi {
    public:
        typedef typename
        vctFixedSizeVectorRecursiveEngines<_size-1>
        ::template VoViVi<_elementOperationType> RecursiveStep;

        /*! Unfold the recursion.  Performs the operation
        _elementOperationType elementwise on the last elements of the
        input/output vector(s) and call Unfold for the _size - 1
        elements left (i.e. unfold the recursive calls).

        \param output The output vector.
        \param input1 The first input vector.
        \param input2 The second input vector.
        */
        template <class _outputVectorType, class _inputVector1Type, class _inputVector2Type>
        static inline void Unfold(_outputVectorType & output,
                                  const _inputVector1Type & input1,
                                  const _inputVector2Type & input2) {
            RecursiveStep::Unfold(output, input1, input2);
            output[_size-1] = _elementOperationType::Operate(input1[_size-1], input2[_size-1]);
        }
    };




    /*!  \brief Implement operation of the form \f$v_{io} = op(v_{io},
      v_i)\f$ for fixed size vectors

      This class uses template specialization to perform binary vector operations
      of the form
      \f[
      v_{io} = \mathrm{op}(v_{io}, v_{i})
      \f]

      where \f$v_{io}\f$ is the input output vector, and \f$v_{i}\f$
      is the second input vector, all of an equal fixed size,
      determined at compilation time, \em op stands for the a binary
      operation performed elementwise between \f$v_{io}\f$ and
      \f$v_{i}\f$, and whose result is stored elementwise into
      \f$v_{io}\f$.

      \param _elementOperationType The type of the binary operation.

      \sa vctFixedSizeVectorRecursiveEngines
    */
    template <class _elementOperationType>
    class VioVi {
    public:
        typedef typename
        vctFixedSizeVectorRecursiveEngines<_size-1>
        ::template VioVi<_elementOperationType> RecursiveStep;

        /*! Unfold the recursion.  Performs the operation
        _elementOperationType elementwise on the last elements of the
        input/output vector(s) and call Unfold for the _size - 1
        elements left (i.e. unfold the recursive calls).

        \param inputOutput The input output vector.
        \param input2 The second input vector.
        */
        template <class _inputOutputVectorType, class _inputVector2Type>
        static inline void Unfold(_inputOutputVectorType & inputOutput,
                                  const _inputVector2Type & input2) {
            RecursiveStep::Unfold(inputOutput, input2);
            _elementOperationType::Operate(inputOutput[_size-1], input2[_size-1]);
        }
    };


    /*!  \brief Implement operation of the form \f$(v_{1}, v_{2}) = op(v_{1},
      v_{2})\f$ for fixed size vectors

      This class uses template specialization to perform binary vector operations
      of the form
      \f[
      (v_{1}, v_{2}) = \mathrm{op}(v_{1}, v_{2})
      \f]

      where both \f$v_{1}, v_{2}\f$ are input and output vectors
      of an equal fixed size, determined at compilation time.  The
      operation is evaluated elementwise, that is, \f$(v_{1}[i], v_{2}[i] =
      \mathrm{op}(v_{1}[i], v_{2}[i])\f$.  The typical operation in this case
      is swapping the elements of the two vectors.

      \param _elementOperationType The type of the binary operation that inputs and
      rewrites corresponding elements in both vectors.

      \sa vctFixedSizeVectorRecursiveEngines
    */
    template <class _elementOperationType>
    class VioVio {
    public:
        typedef typename
        vctFixedSizeVectorRecursiveEngines<_size-1>
        ::template VioVio<_elementOperationType> RecursiveStep;

        /*! Unfold the recursion.  Performs the operation
        _elementOperationType elementwise on the last elements of the
        input/output vector(s) and call Unfold for the _size - 1
        elements left (i.e. unfold the recursive calls).

        \param inputOutput1 The first input-output vector.
        \param inputOutput2 The second input-output vector.
        */
        template <class _inputOutputVector1Type, class _inputOutputVector2Type>
        static inline void Unfold(_inputOutputVector1Type & inputOutput1,
                                  _inputOutputVector2Type & inputOutput2) {
            RecursiveStep::Unfold(inputOutput1, inputOutput2);
            _elementOperationType::Operate(inputOutput1[_size-1], inputOutput2[_size-1]);
        }
    };



    /*!  \brief Implement operation of the form \f$vo = op(vi, si)\f$ for
      fixed size vectors

      This class uses template specialization to perform binary vector operations
      of the form
      \f[
      v_o = \mathrm{op}(v_i, s_i)
      \f]

      where \f$v_o\f$ is the output vector, and \f$v_i, s_i\f$ are
      input vector and scalar, all vectors are of an equal fixed size,
      determined at compilation time, \em op stands for the a binary
      operation performed elementwise between \f$v_i\f$ and \f$s_i\f$,
      and whose result is stored elementwise into \f$v_o\f$.

      The operation type operationType must have a static method with
      the signature

      <PRE>
      static operationType Operation(const I1E & arg1, const I2E & arg2)
      </PRE>

      where OE, I1E, I2E are the types of vector elements in \f$v_o,
      v_i s_i\f$ respectively.  These types do not need to be defined
      explicitly for \em vctFixedSizeVectorRecursiveEngines class, as they are
      inferred from the signature of the corresponding operator[] for
      each of the vector types.  However, they may need to be defined
      for the class operationType.

      For examples of binary operations, see vctBinaryOperations.h .

      Usage example for vctFixedSizeVectorRecursiveEngines::VoViSi:
      <PRE>
      enum {SIZE = 3};
      int input1[SIZE];
      float a;
      double output[SIZE];

      vctFixedSizeVectorRecursiveEngines<SIZE>::VoViSi<vctBinaryOperations<int,float,double>::Addition>::Unfold(output, input1, scalar);
      </PRE>
      stores the sum of elements of input1 and s into output.

      \param _elementOperationType The type of the binary operation.

      \sa vctFixedSizeVectorRecursiveEngines
    */
    template <class _elementOperationType>
    class VoViSi {
    public:
        typedef typename
        vctFixedSizeVectorRecursiveEngines<_size-1>
        ::template VoViSi<_elementOperationType> RecursiveStep;

        /*! Unfold the recursion.  Performs the operation
        _elementOperationType elementwise on the last elements of the
        input vector and scalar and call Unfold for the _size - 1
        elements left (i.e. unfold the recursive calls).

        \param output The output vector.
        \param inputVector The input vector (first operand).
        \param inputScalar The input scalar (second operand).
        */
        template <class _outputVectorType, class _inputVectorType, class _inputScalarType>
        static inline void Unfold(_outputVectorType & output,
                                  const _inputVectorType & inputVector,
                                  const _inputScalarType inputScalar) {
            RecursiveStep::Unfold(output, inputVector, inputScalar);
            output[_size-1] = _elementOperationType::Operate(inputVector[_size-1], inputScalar);
        }
    };


    /*!  \brief Implement operation of the form \f$v_o = op(s_i,
      v_i)\f$ for fixed size vectors

      This class uses template specialization to perform binary vector operations
      of the form
      \f[
      v_o = \mathrm{op}(s_i, v_i)
      \f]

      where \f$v_o\f$ is the output vector, and \f$s_i, v_i\f$ are
      input scalar and vector, all vectors are of an equal fixed size,
      determined at compilation time, \em op stands for the a binary
      operation performed elementwise between \f$s_i\f$ and \f$v_i\f$,
      and whose result is stored elementwise into \f$v_o\f$.

      The operation type operationType must have a static method with
      the signature

      <PRE>
      static operationType Operation(const I1E & arg1, const I2E & arg2)
      </PRE>

      where OE, I1E, I2E are the types of vector elements in \f$v_o,
      v_i s_i\f$ respectively.  These types do not need to be defined
      explicitly for \em vctFixedSizeVectorRecursiveEngines class, as they are
      inferred from the signature of the corresponding operator[] for
      each of the vector types.  However, they may need to be defined
      for the class operationType.

      For examples of binary operations, see vctBinaryOperations.h .

      Usage example for vctFixedSizeVectorRecursiveEngines::VoSiVi:
      <PRE>
      enum {SIZE = 3};
      int input1[SIZE];
      float a;
      double output[SIZE];

      vctFixedSizeVectorRecursiveEngines<SIZE>::VoSiVi<vctBinaryOperations<int,float,double>::Addition>::Unfold(output, scalar, input1);
      </PRE>
      stores the sum of elements of s and input1 into output.

      \param _elementOperationType The type of the binary operation.

      \sa vctFixedSizeVectorRecursiveEngines
    */
    template <class _elementOperationType>
    class VoSiVi {
    public:
        typedef typename
        vctFixedSizeVectorRecursiveEngines<_size-1>
        ::template VoSiVi<_elementOperationType> RecursiveStep;

        /*! Unfold the recursion.  Performs the operation
        _elementOperationType elementwise on the last elements of the
        input vector and scalar and call Unfold for the _size - 1
        elements left (i.e. unfold the recursive calls).

        \param output The output vector.
        \param inputScalar The input scalar (first operand).
        \param inputVector The input vector (second operand).
        */
        template <class _outputVectorType, class _inputScalarType, class _inputVectorType>
        static inline void Unfold(_outputVectorType & output,
                                  const _inputScalarType & inputScalar,
                                  const _inputVectorType inputVector) {
            RecursiveStep::Unfold(output, inputScalar, inputVector);
            output[_size-1] = _elementOperationType::Operate(inputScalar, inputVector[_size-1]);
        }
    };




    /*!  \brief Implement operation of the form \f$ v_{io} =
      op(v_{io}, s_i)\f$ for fixed size vectors

      This class uses template specialization to perform binary vector
      operations of the form

      \f[
      v_{io} = \mathrm{op}(v_{io}, s_{i})
      \f]

      where \f$v_{io}\f$ is the input output vector, and \f$s_{i}\f$
      is the scalar input.  The vector has a fixed size, determined at
      compilation time, \em op stands for the binary operation
      performed elementwise between \f$v_{io}[index]\f$ and
      \f$s_{i}\f$, and whose result is stored elementwise into
      \f$v_{io}[index]\f$.

      \param _elementOperationType the type of the binary operation

      \sa vctFixedSizeVectorRecursiveEngines
    */
    template <class _elementOperationType>
    class VioSi {
    public:
        typedef typename
        vctFixedSizeVectorRecursiveEngines<_size-1>
        ::template VioSi<_elementOperationType> RecursiveStep;

        /*! Unfold the recursion.  Performs the operation
          _elementOperationType elementwise on the last elements of the
          input output vector and the input scalar and call Unfold for
          the _size - 1 elements left (i.e. unfold the recursive calls).

        \param inputOutput The input output vector (first operand).
        \param inputScalar The input scalar (second operand).
        */
        template <class _inputOutputVectorType, class _inputScalarType>
        static inline void Unfold(_inputOutputVectorType & inputOutput,
                                  const _inputScalarType inputScalar) {
            RecursiveStep::Unfold(inputOutput, inputScalar);
            _elementOperationType::Operate(inputOutput[_size-1], inputScalar);
        }
    };



    /*!  \brief Implement operation of the form \f$v_o = op(v_i)\f$ for
      fixed size vectors

      This class uses template specialization to perform unary vector operations
      of the form
      \f[
      v_{o} = \mathrm{op}(v_{i})
      \f]

      where \f$v_{o}\f$ is the output vector, and \f$v_{i}\f$ is the
      input vector both of a fixed size, determined at compilation
      time, \em op stands for the a unary operation performed
      elementwise on \f$v_{i}\f$, and whose result is stored
      elementwise into \f$v_{o}\f$.

      \param _elementOperationType The type of the unary operation.

      \sa vctFixedSizeVectorRecursiveEngines
    */
    template <class _elementOperationType>
    class VoVi {
    public:
        typedef typename
        vctFixedSizeVectorRecursiveEngines<_size-1>
        ::template VoVi<_elementOperationType> RecursiveStep;

        /*! Unfold the recursion.  Performs the operation
        _elementOperationType elementwise on the last elements of the
        input vector and call Unfold for the _size - 1 elements left
        (i.e. unfold the recursive calls).

        \param outputVector The output vector.
        \param inputVector The input vector.
        */
        template <class _outputVectorType, class _inputVectorType>
        static inline void Unfold(_outputVectorType & outputVector,
                                  const _inputVectorType & inputVector) {
            RecursiveStep::Unfold(outputVector, inputVector);
            outputVector[_size-1] = _elementOperationType::Operate(inputVector[_size-1]);
        }
    };



    /*!  \brief Implement operation of the form \f$v_{io} =
      op(v_{io})\f$ for fixed size vectors

      This class uses template specialization to perform unary store
      back vector operations of the form

      \f[
      v_{io} = \mathrm{op}(v_{io})
      \f]

      where \f$v_{io}\f$ is the input output vector.  The vector has a
      fixed size, determined at compilation time, \em op stands for
      the unary operation performed elementwise on \f$v_{io}\f$ and
      whose result is stored elementwise into \f$v_{io}[index]\f$.

      \param _elementOperationType The type of the unary operation.

      \sa vctFixedSizeVectorRecursiveEngines
    */
    template <class _elementOperationType>
    class Vio {
    public:
        typedef typename
        vctFixedSizeVectorRecursiveEngines<_size-1>
        ::template Vio<_elementOperationType> RecursiveStep;

        /*! Unfold the recursion.  Performs the operation
        _elementOperationType elementwise on the last elements of the
        input output vector and call Unfold for the _size - 1 elements
        left (i.e. unfold the recursive calls).

        \param inputOutput The input output vector.
        */
        template <class _inputOutputVectorType>
        static inline void Unfold(_inputOutputVectorType & inputOutput) {
            RecursiveStep::Unfold(inputOutput);
            _elementOperationType::Operate(inputOutput[_size-1]);
        }
    };




    /*!  \brief Implement operation of the form \f$s_o =
      op_{incr}(op(v_i))\f$ for fixed size vectors

      This class uses template specialization to perform incremental
      unary vector operations of the form

      \f[
      s_o = \mathrm{op_{incr}(\mathrm{op}(v_i))}
      \f]

      where \f$v_i\f$ is the input vector and \f$s_o\f$ is the scalar
      output.  The vector has a fixed size, determined at compilation
      time, \em op stands for the unary operation performed
      elementwise on \f$v_i\f$ and whose result are used incrementally
      as input for \f$op_{incr}\f$.  For a vector of size 3, the
      result is \f$s_o = op_{incr}(op_{incr}(op(v[1]), op(v[0])),
      op(v[2])) \f$.

      \param _incrementalOperationType The type of the incremental
      operation.

      \param _elementOperationType The type of the unary operation.

      \sa vctFixedSizeVectorRecursiveEngines
    */
    template <class _incrementalOperationType, class _elementOperationType>
    class SoVi {
    public:
        typedef typename
        vctFixedSizeVectorRecursiveEngines<_size-1>
        ::template SoVi<_incrementalOperationType, _elementOperationType> RecursiveStep;
        typedef typename _incrementalOperationType::OutputType OutputType;

        /*! Unfold the recursion.  Performs the operation
        _elementOperationType elementwise on the last elements of the
        input vector and call Unfold for the _size - 1 elements left
        (i.e. unfold the recursive calls).  The incremental operation
        is applied to the result of the recursive Unfold call and the
        result of _elementOperationType(inputVector[_size]).

        \param inputVector The input vector.
        */
        template <class _inputVectorType>
        static inline OutputType Unfold(_inputVectorType & inputVector) {
            return
                _incrementalOperationType
                ::Operate(RecursiveStep::Unfold(inputVector),
                          _elementOperationType::Operate(inputVector[_size-1]));
        }
    };





    /*!  \brief Implement operation of the form \f$s_o =
      op_{incr}(op(v_{i1}, v_{i2}))\f$ for fixed size vectors

      This class uses template specialization to perform incremental
      binary vector operations of the form

      \f[
      s_o = \mathrm{op_{incr}(\mathrm{op}(v_{i1}, v_{i2}))}
      \f]

      where \f$v_{i1}\f$ and \f$v_{i2}\f$ are the input vectors and
      \f$s_o\f$ is the scalar output.  The vectors have a fixed size,
      determined at compilation time, \em op stands for the unary
      operation performed elementwise on \f$v_{i1}\f$ and \f$v_{i2}\f$
      and whose result are used incrementally as input for
      \f$op_{incr}\f$.  For a vector of size 3, the result is \f$s_o =
      op_{incr}(op_{incr}(op(v1[1], v2[1]), op(v1[0], v2[0])),
      op(v1[2], v2[2]))\f$.

      \param _incrementalOperationType The type of the incremental
      operation.

      \param _elementOperationType The type of the unary operation.

      \sa vctFixedSizeVectorRecursiveEngines
    */
    template <class _incrementalOperationType, class _elementOperationType>
    class SoViVi {
    public:
        typedef typename
        vctFixedSizeVectorRecursiveEngines<_size-1>
        ::template SoViVi<_incrementalOperationType, _elementOperationType> RecursiveStep;
        typedef typename _incrementalOperationType::OutputType OutputType;

        /*! Unfold the recursion.  Performs the operation
        _elementOperationType elementwise on the last elements of the
        input vectors and call Unfold for the _size - 1 elements left
        (i.e. unfold the recursive calls).  The incremental operation
        is applied to the result of the recursive Unfold call and the
        result of _elementOperationType(input1Vector[_size],
        input2Vector[_size]).

        \param input1Vector The first input vector (first operand for
        _elementOperationType).

        \param input2Vector The second input vector (second operand
        for _elementOperationType).
        */
        template <class _input1VectorType, class _input2VectorType>
        static inline OutputType Unfold(const _input1VectorType & input1Vector,
                                        const _input2VectorType & input2Vector) {
            return
                _incrementalOperationType
                ::Operate(RecursiveStep::Unfold(input1Vector, input2Vector),
                          _elementOperationType::Operate(input1Vector[_size-1], input2Vector[_size-1]));
        }
    };


    /*!  \brief Implement operation of the form \f$v_{io} =
      op_{io}(v_{io}, op_{sv}(s, v_i))\f$ for fixed size vectors

      This class uses template specialization to perform store-back
      vector-scalar-vector operations

      \f[
      v_{io} = \mathrm{op_{io}}(V_{io}, \mathrm{op_{sv}}(s, v_i))
      \f]

      where \f$v_{io}\f$ is an input-output (store-back) vector;
      \f$s\f$ is a scalar; and \f$v_i\f$ is an input vector.  A
      typical example is: \f$v_{io} += s \cdot v_i\f$.  The vectors
      have a fixed size, determined at compilation time; \f$op_{sv}\f$
      is an operation between \f$s\f$ and the elements of \f$v_i\f$;
      \f$op_{io}\f$ is an operation between the output of
      \f$op_{sv}\f$ and the elements of \f$v_{io}\f$.

      \param _ioOperationType The type of the store-back operation.

      \param _scalarVectorElementOperationType The type of the
      operation between scalar and input vector.

      \sa vctFixedSizeVectorRecursiveEngines
    */
    template <class _ioElementOperationType, class _scalarVectorElementOperationType>
    class VioSiVi {
    public:
        typedef typename
        vctFixedSizeVectorRecursiveEngines<_size-1>
        ::template VioSiVi<_ioElementOperationType, _scalarVectorElementOperationType>
        RecursiveStep;

        /*! Unfold the recursion.  Performs the operation
        _elementOperationType elementwise on the last elements of the
        input vector and the input scalar and call Unfold for the
        _size - 1 elements left (i.e. unfold the recursive calls).
        The incremental operation is applied to the result of the
        recursive Unfold call and the result of
        _elementOperationType(inputVector[_size],
        inputScalar[_size]).

        \param inputVector The input vector (first operand for
        _elementOperationType).

        \param inputScalar The input scalar (second operand
        for _elementOperationType).
        */
        template <class _ioVectorType, class _inputScalarType, class _inputVectorType>
        static inline void Unfold(_ioVectorType & ioVector,
                                  const _inputScalarType & inputScalar, const _inputVectorType & inputVector)
        {
            RecursiveStep::Unfold(ioVector, inputScalar, inputVector);
            _ioElementOperationType::Operate( ioVector[_size-1],
                _scalarVectorElementOperationType::Operate(inputScalar, inputVector[_size-1]) );
        }

    };



    /*!  \brief Implement operation of the form \f$v_{io} =
      op_{io}(v_{io}, op_{vv}(v_{i1}, v_{i2}))\f$ for fixed size vectors

      This class uses template specialization to perform store-back
      vector-vector-vector operations

      \f[
      v_{io} = \mathrm{op_{io}}(V_{io}, \mathrm{op_{vv}}(v_{i1}, v_{i2}))
      \f]

      where \f$v_{io}\f$ is an input-output (store-back) vector;
      \f$v_{i1}\f$ and \f$v_{i2}\f$ are input vectors.  A typical
      example is: \f$v_{io} += v_{i1} \cdot v_{i2}\f$.  The vectors
      have a fixed size, determined at compilation time; \f$op_{vv}\f$
      is an element wise operation between the elements of
      \f$v_{i1}\f$ and the elements of \f$v_{i2}\f$; \f$op_{io}\f$ is
      an operation between the output of \f$op_{vv}\f$ and the
      elements of \f$v_{io}\f$.

      \param _ioOperationType The type of the store-back operation.

      \param _vectorElementOperationType The type of the
      operation between scalar and input vector.

      \sa vctFixedSizeVectorRecursiveEngines
    */
    template <class _ioElementOperationType, class _vectorElementOperationType>
    class VioViVi {
    public:
        typedef typename
        vctFixedSizeVectorRecursiveEngines<_size-1>
        ::template VioViVi<_ioElementOperationType, _vectorElementOperationType>
        RecursiveStep;

        /*! Unfold the recursion.  Performs the operation
        _elementOperationType elementwise on the last elements of the
        input vector and the input scalar and call Unfold for the
        _size - 1 elements left (i.e. unfold the recursive calls).
        The incremental operation is applied to the result of the
        recursive Unfold call and the result of
        _elementOperationType(inputVector[_size],
        inputScalar[_size]).

        \param inputVector The input vector (first operand for
        _elementOperationType).

        \param inputScalar The input scalar (second operand
        for _elementOperationType).
        */
        template <class _ioVectorType, class _inputVectorType1, class _inputVectorType2>
        static inline void Unfold(_ioVectorType & ioVector,
                                  const _inputVectorType1 & vector1, const _inputVectorType2 & vector2)
        {
            RecursiveStep::Unfold(ioVector, vector1, vector2);
            _ioElementOperationType::Operate( ioVector[_size-1],
                _vectorElementOperationType::Operate(vector1[_size-1], vector2[_size-1]) );
        }

    };



    /*!  \brief Implement operation of the form \f$s_o =
      op_{incr}(op(v_i, s_i))\f$ for fixed size vectors

      This class uses template specialization to perform incremental
      binary vector operations of the form

      \f[
      s_o = \mathrm{op_{incr}(\mathrm{op}(v_i, s_i))}
      \f]

      where \f$v_i\f$ and \f$s_i\f$ are the input vector and scalar
      and \f$s_o\f$ is the scalar output.  The vector has a fixed
      size, determined at compilation time, \em op stands for the
      unary operation performed elementwise on \f$v_i\f$ and \f$s_i\f$
      and whose result are used incrementally as input for
      \f$op_{incr}\f$.  For a vector of size 3, the result is \f$s_o =
      op_{incr}(op_{incr}(op(v[1], s), op(v[0], s)), op(v, s))\f$.

      \param _incrementalOperationType The type of the incremental
      operation.

      \param _elementOperationType The type of the unary operation.

      \sa vctFixedSizeVectorRecursiveEngines
    */
    template <class _incrementalOperationType, class _elementOperationType>
    class SoViSi {
    public:
        typedef typename
        vctFixedSizeVectorRecursiveEngines<_size-1>
        ::template SoViSi<_incrementalOperationType, _elementOperationType> RecursiveStep;
        typedef typename _incrementalOperationType::OutputType OutputType;

        /*! Unfold the recursion.  Performs the operation
        _elementOperationType elementwise on the last elements of the
        input vector and the input scalar and call Unfold for the
        _size - 1 elements left (i.e. unfold the recursive calls).
        The incremental operation is applied to the result of the
        recursive Unfold call and the result of
        _elementOperationType(inputVector[_size],
        inputScalar[_size]).

        \param inputVector The input vector (first operand for
        _elementOperationType).

        \param inputScalar The input scalar (second operand
        for _elementOperationType).
        */
        template <class _inputVectorType, class _inputScalarType>
        static inline OutputType Unfold(const _inputVectorType & inputVector,
                                        const _inputScalarType & inputScalar) {
            return
                _incrementalOperationType
                ::Operate(RecursiveStep::Unfold(inputVector, inputScalar),
                          _elementOperationType::Operate(inputVector[_size-1], inputScalar));
        }
    };




    /*!  \brief Implement operation of the form \f$s_o =
      op_{incr}(op(v_o, s_i)\f$ for fixed size vectors.

      This class uses template specialization to perform incremental
      binary vector operations of the form

      \f[
      s_o = \mathrm{op_{incr}(v_{o}, s_i)}
      \f]

      where \f$v_{o}\f$ is the output vector and \f$s_o\f$ and
      \f$s_i\f$ are the scalar output and input.  The vector has a
      fixed size, determined at compilation time. \f$op_{incr}\f$ is
      the incremental operation using \f$v_{o}[i]\f$ and \f$s_i\f$.
      For a vector of size 3, the result is \f$s_o =
      op_{incr}(op_{incr}(v_o[2], op_{incr}(v_o[1], op_{incr}(v_o[0],
      s_i))))\f$.

      This engines is currently used by vctFixedSizeVector::SetAll
      with vctBinaryOperations::SecondOperand.  For a vector of size
      3, the result is: \f$s_o = v_o[2] = v_o[1] = v_o[0] = s_i\f$.

      \param _incrementalOperationType The type of the incremental
      operation.

      \sa vctFixedSizeVectorRecursiveEngines
    */
    template <class _incrementalOperationType>
    class SoVoSi {
    public:
        typedef typename
        vctFixedSizeVectorRecursiveEngines<_size-1>
        ::template SoVoSi<_incrementalOperationType> RecursiveStep;
        typedef typename _incrementalOperationType::OutputType OutputType;

        /*! Unfold the recursion.  The incremental operation is
        applied to the result of the recursive Unfold call and the
        result of _incrementalOperationType(outputVector[_size],
        inputScalar).

        \param outputVector The output vector (first operation of the
        incremental operation).

        \param inputScalar The input scalar (second operand of the
        incremental operation).
        */
        template <class _outputVectorType, class _inputScalarType>
        static inline OutputType Unfold(_outputVectorType & outputVector,
                                        const _inputScalarType & inputScalar) {
            return
                (outputVector[_size-1] =
                 _incrementalOperationType::Operate(outputVector[_size-1],
                                                    RecursiveStep::Unfold(outputVector, inputScalar)));
        }
    };

    /*! A specialized engine for computing the minimum and maximum
      elements of a matrix in one pass.  This implementation is more
      efficient than computing them separately.
     */
    class MinAndMax
    {
    public:
        typedef typename vctFixedSizeVectorRecursiveEngines<_size-1>::MinAndMax RecursiveStep;

        template <class _inputVectorType>
        static inline void Unfold(const _inputVectorType & inputVector,
                                  typename _inputVectorType::value_type & minValue,
                                  typename _inputVectorType::value_type & maxValue)
        {
            typedef typename _inputVectorType::value_type value_type;
            RecursiveStep::Unfold(inputVector, minValue, maxValue);
            const value_type element = inputVector[_size-1];
            if (element < minValue) {
                minValue = element;
            }
            else if (maxValue < element) {
                maxValue = element;
            }
        }
    };


    /*! A specialized engine to select elements from the input vector
     to the output vector. */
    class SelectByIndex {
    public:
        typedef typename
        vctFixedSizeVectorRecursiveEngines<_size-1>::SelectByIndex RecursiveStep;

        /*! Unfold the recursion: call Unfold for the first (_size - 1)
        elements, then assign value to the _size element.

        \param output The output vector.
        \param input The input vector from which elements are selected.
        \param index The index vector.
        */
        template <class _outputVectorType, class _inputVectorType, class _indexVectorType>
        static inline void Unfold(_outputVectorType & output,
                                  const _inputVectorType & input,
                                  const _indexVectorType & index)
        {
            RecursiveStep::Unfold(output, input, index);
            output[_size-1] = input[index[_size-1]];
        }
    };

    /*! The Find engine returns the first index in which a condition
      evaluates to 'true'.  The index is returned as an unsigned
      integer.  The condition is evaluated between each element of the
      input vector and the input scalar, in the form <tt>
      _conditionOperationType::Operate(inputVector[index],
      inputScalar)</tt>.  That is, the static method Operate is a
      binary predicate.

      The Find engine uses an auxiliary template argument \c
      _currentIndex which is incremented as the recursion nesting
      increases.  We need it so that we don't have to evaluate the
      function again to return the index.

      If the condition never evaluates to true, the function returns a
      value one past the length of the vector (\c _size).

      A typical example assigned \c _conditionOperation with an
      equality test, searching for the first occurence of a value in
      the vector.
    */
    template <class _conditionOperationType, vct::index_type _currentIndex>
    class Find
    {
    public:
        typedef typename
        vctFixedSizeVectorRecursiveEngines<_size-1>
        ::template Find<_conditionOperationType, _currentIndex+1> RecursiveStep;

        template <class _inputVectorType, class _inputScalarType>
        static inline vct::index_type Unfold(const _inputVectorType * inputVector,
                                             const _inputScalarType & inputScalar) {
            return ( _conditionOperationType::Operarate(inputVector[_currentIndex], inputScalar) )
                ? _currentIndex
                : RecursiveStep::Unfold(inputVector, inputScalar);
        }

    };

};


// Base of recursion, doesn't need to be documented with doxygen
#ifndef DOXYGEN

template <>
class vctFixedSizeVectorRecursiveEngines<0>
{
 public:

    template <class _elementOperationType>
    class VoViVi {
    public:
        template <class _outputVectorType, class _inputVector1Type, class _inputVector2Type>
        static inline void Unfold(_outputVectorType & CMN_UNUSED(output),
                                  const _inputVector1Type & CMN_UNUSED(input1),
                                  const _inputVector2Type & CMN_UNUSED(input2)) {
            return;
        }
    };


    template <class _elementOperationType>
    class VioVi {
    public:
        template <class _inputOutputVector1Type, class _inputVector2Type>
        static inline void Unfold(_inputOutputVector1Type & CMN_UNUSED(inputOutput),
                                  const _inputVector2Type & CMN_UNUSED(input2)) {
            return;
        }
    };


    template <class _elementOperationType>
    class VioVio {
    public:
        template <class _inputOutputVector1Type, class _inputOutputVector2Type>
        static inline void Unfold(_inputOutputVector1Type & CMN_UNUSED(inputOutput1),
                                  _inputOutputVector2Type & CMN_UNUSED(inputOutput2)) {
            return;
        }
    };


    template <class _elementOperationType>
    class VoViSi {
    public:
        template <class _outputVectorType, class _inputVectorType, class _inputScalarType>
        static inline void Unfold(_outputVectorType & CMN_UNUSED(output),
                                  const _inputVectorType & CMN_UNUSED(inputVector),
                                  const _inputScalarType & CMN_UNUSED(inputScalar)) {
            return;
        }
    };


    template <class _elementOperationType>
    class VoSiVi {
    public:
        template <class _outputVectorType, class _inputScalarType, class _inputVectorType>
        static inline void Unfold(_outputVectorType & CMN_UNUSED(output),
                                  const _inputScalarType & CMN_UNUSED(inputScalar),
                                  const _inputVectorType & CMN_UNUSED(inputVector)) {
            return;
        }
    };


    template <class _elementOperationType>
    class VioSi {
    public:
        template <class _inputOutputVectorType, class _inputScalarType>
        static inline void Unfold(_inputOutputVectorType & CMN_UNUSED(inputOutput),
                                  const _inputScalarType & CMN_UNUSED(inputScalar)) {
            return;
        }
    };


    template <class _elementOperationType>
    class VoVi {
    public:
        template <class _outputVectorType, class _inputVectorType>
        static inline void Unfold(_outputVectorType & CMN_UNUSED(outputVector),
                                  const _inputVectorType & CMN_UNUSED(inputVector)) {
            return;
        }
    };


    template <class _elementOperationType>
    class Vio {
    public:
        template <class _inputOutputVectorType>
        static inline void Unfold(_inputOutputVectorType & CMN_UNUSED(inputOutput)) {
            return;
        }
    };


    template <class _incrementalOperationType, class _elementOperationType>
    class SoVi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;
        template <class _inputVectorType>
        static inline OutputType Unfold(const _inputVectorType & CMN_UNUSED(inputVector)) {
            return _incrementalOperationType::NeutralElement();
        }
    };


    template <class _incrementalOperationType, class _elementOperationType>
    class SoViVi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;
        template <class _input1VectorType, class _input2VectorType>
        static inline OutputType Unfold(const _input1VectorType & CMN_UNUSED(input1Vector),
                                        const _input2VectorType & CMN_UNUSED(input2Vector)) {
            return _incrementalOperationType::NeutralElement();
        }
    };


    class MinAndMax
    {
    public:
        template <class _inputVectorType>
        static inline void Unfold(const _inputVectorType & inputVector,
                                  typename _inputVectorType::value_type & minValue,
                                  typename _inputVectorType::value_type & maxValue)
        {
            maxValue = minValue = inputVector[0];
        }
    };


    class SelectByIndex {
    public:
        template <class _outputVectorType, class _inputVectorType, class _indexVectorType>
        static inline void Unfold(_outputVectorType & CMN_UNUSED(output),
                                  const _inputVectorType & CMN_UNUSED(input),
                                  const _indexVectorType & CMN_UNUSED(index))
        {
            return;
        }
    };


    template <class _incrementalOperationType, class _elementOperationType>
    class SoViSi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;
        template <class _inputVectorType, class _inputScalarType>
        static inline OutputType Unfold(const _inputVectorType & CMN_UNUSED(inputVector),
                                        const _inputScalarType & CMN_UNUSED(inputScalar)) {
            return _incrementalOperationType::NeutralElement();
        }
    };


    template <class _ioElementOperationType, class _scalarVectorElementOperationType>
    class VioSiVi {
    public:
        template <class _ioVectorType, class _inputScalarType, class _inputVectorType>
        static inline void Unfold(_ioVectorType & CMN_UNUSED(ioVector),
                                  const _inputScalarType & CMN_UNUSED(inputScalar),
                                  const _inputVectorType & CMN_UNUSED(inputVector))
        {
            return;
        }
    };


    template <class _ioElementOperationType, class _vectorElementOperationType>
    class VioViVi {
    public:
        template <class _ioVectorType, class _inputVectorType1, class _inputVectorType2>
        static inline void Unfold(_ioVectorType & CMN_UNUSED(ioVector),
                                  const _inputVectorType1 & CMN_UNUSED(vector1),
                                  const _inputVectorType2 & CMN_UNUSED(vector2))
        {
            return;
        }
    };


    template <class _incrementalOperationType>
    class SoVoSi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;
        template <class _outputVectorType, class _inputScalarType>
        static inline OutputType Unfold(_outputVectorType & CMN_UNUSED(outputVector),
                                        const _inputScalarType & inputScalar) {
            return OutputType(inputScalar);
        }
    };


    template <class _conditionOperationType, vct::index_type _currentIndex>
    class Find
    {
    public:
        template <class _inputVectorType, class _inputScalarType>
        static inline vct::index_type Unfold(const _inputVectorType * CMN_UNUSED(inputVector),
                                             const _inputScalarType & CMN_UNUSED(inputScalar)) {
            return _currentIndex + 1;
        }
    };

};

#endif  // DOXYGEN

#endif  // _vctFixedSizeVectorRecursiveEngines_h

