/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2007-07-07

  (C) Copyright 2007-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctDynamicCompactLoopEngines_h
#define _vctDynamicCompactLoopEngines_h

/*!
  \file
  \brief Declaration of vctDynamicCompactLoopEngines
 */

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnThrow.h>

/*!  \brief Container class for the loop based engines for compact
  containers.

  These engines are a simplified version of those found in
  vctDynamicVectorLoopEngines, vctDynamicMatrixLoopEngines and
  vctDynamicNArrayLoopEngines.  They can only operate on compact
  containers and provided that all parameters have the same memory
  layout, i.e. the same strides.  In this specific cases, a single
  loop can be used to operate on all the elements as opposed to nested
  loops or more complicated structures (as for NArrays).  This results
  on a significant speed gain.  Furthermore, as the stride is always
  "one", the operator "++" can be used which in some compilation mode
  can provide a slight speed boost.

  \note These engines don't perform any layout check as this is done
  by the other engines.

  \note These engines operate directly on the owners as the engines
  calling them already a pointer on the owner.

  \sa vctDynamicVectorLoopEngines, vctDynamicMatrixLoopEngines,
  vctDynamicNArrayLoopEngines.
*/
class vctDynamicCompactLoopEngines {

 public:

    /*!  \brief Implement operation of the form \f$v_o = op(v_{i1},
      v_{i2})\f$ for compact containers.

      This class uses a loop to perform binary vector operations
      of the form
      \f[
      v_o = \mathrm{op}(v_{i1}, v_{i2})
      \f]

      \param _elementOperationType The type of the binary operation.
    */
    template<class _elementOperationType>
    class CoCiCi {
    public:
        template<class _outputOwnerType, class _input1OwnerType, class _input2OwnerType>
        static inline void Run(_outputOwnerType & outputOwner,
                               const _input1OwnerType & input1Owner,
                               const _input2OwnerType & input2Owner) {

            typedef _outputOwnerType OutputOwnerType;
            typedef typename OutputOwnerType::pointer OutputPointerType;
            typedef typename OutputOwnerType::size_type size_type;

            typedef _input1OwnerType Input1OwnerType;
            typedef typename Input1OwnerType::const_pointer Input1PointerType;

            typedef _input2OwnerType Input2OwnerType;
            typedef typename Input2OwnerType::const_pointer Input2PointerType;

            const size_type size = outputOwner.size();

            OutputPointerType outputPointer = outputOwner.Pointer();
            const OutputPointerType outputEnd = outputPointer + size;

            Input1PointerType input1Pointer = input1Owner.Pointer();
            Input2PointerType input2Pointer = input2Owner.Pointer();

            for (;
                 outputPointer != outputEnd;
                 outputPointer++, input1Pointer++, input2Pointer++) {
                *outputPointer = _elementOperationType::Operate(*input1Pointer, *input2Pointer);
            }
        }
    };


    /*!  \brief Implement operation of the form \f$v_{io} = op(v_{io},
      v_i)\f$ for compact containers.

      This class uses a loop to perform binary vector operations
      of the form
      \f[
      v_{io} = \mathrm{op}(v_{io}, v_{i})
      \f]

      \param _elementOperationType The type of the binary operation.
    */
    template<class _elementOperationType>
    class CioCi {
    public:
        template<class _inputOutputOwnerType, class _inputOwnerType>
        static void Run(_inputOutputOwnerType & inputOutputOwner,
                        const _inputOwnerType & inputOwner) {

            typedef _inputOutputOwnerType InputOutputOwnerType;
            typedef typename InputOutputOwnerType::pointer InputOutputPointerType;
            typedef typename InputOutputOwnerType::size_type size_type;

            typedef _inputOwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            const size_type size = inputOutputOwner.size();

            InputOutputPointerType inputOutputPointer = inputOutputOwner.Pointer();
            const InputOutputPointerType inputOutputEnd = inputOutputPointer + size;

            InputPointerType inputPointer = inputOwner.Pointer();

            for (;
                 inputOutputPointer != inputOutputEnd;
                 inputOutputPointer++, inputPointer++) {
                *inputOutputPointer = _elementOperationType::Operate(*inputOutputPointer, *inputPointer);
            }
        }
    };


    /*!  \brief Implement operation of the form \f$(v_{1}, v_{2}) =
      op(v_{1}, v_{2})\f$ for compact containers.

      This class uses a loop to perform binary vector operations
      of the form
      \f[
      (v_{1}, v_{2}) = \mathrm{op}(v_{1}, v_{2})
      \f]

      \param _elementOperationType The type of the binary operation that inputs and
      rewrites corresponding elements in both vectors.
    */
    template<class _elementOperationType>
    class CioCio {
    public:
        template<class _inputOutput1OwnerType, class _inputOutput2OwnerType>
        static void Run(_inputOutput1OwnerType & inputOutput1Owner,
                        _inputOutput2OwnerType & inputOutput2Owner) {

            typedef _inputOutput1OwnerType InputOutput1OwnerType;
            typedef typename InputOutput1OwnerType::pointer InputOutput1PointerType;
            typedef typename InputOutput1OwnerType::size_type size_type;

            typedef _inputOutput2OwnerType InputOutput2OwnerType;
            typedef typename InputOutput2OwnerType::pointer InputOutput2PointerType;

            const size_type size = inputOutput1Owner.size();

            InputOutput1PointerType inputOutput1Pointer = inputOutput1Owner.Pointer();
            const InputOutput1PointerType inputOutput1End = inputOutput1Pointer + size;

            InputOutput2PointerType inputOutput2Pointer = inputOutput2Owner.Pointer();

            for (;
                 inputOutput1Pointer != inputOutput1End;
                 inputOutput1Pointer++, inputOutput2Pointer++) {
                _elementOperationType::Operate(*inputOutput1Pointer, *inputOutput2Pointer);
            }
        }
    };


    /*!  \brief Implement operation of the form \f$vo = op(vi, si)\f$
      for compact containers.

      This class uses a loop to perform binary vector operations
      of the form
      \f[
      v_o = \mathrm{op}(v_i, s_i)
      \f]

      \param _elementOperationType The type of the binary operation.
    */
    template<class _elementOperationType>
    class CoCiSi {
    public:
        template<class _outputOwnerType, class _inputOwnerType, class _inputScalarType>
        static inline void Run(_outputOwnerType & outputOwner,
                               const _inputOwnerType & inputOwner,
                               const _inputScalarType inputScalar) {

            typedef _outputOwnerType OutputOwnerType;
            typedef typename OutputOwnerType::pointer OutputPointerType;
            typedef typename OutputOwnerType::size_type size_type;

            typedef _inputOwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            const size_type size = outputOwner.size();

            OutputPointerType outputPointer = outputOwner.Pointer();
            const OutputPointerType outputEnd = outputPointer + size;

            InputPointerType inputPointer = inputOwner.Pointer();

            for (;
                 outputPointer != outputEnd;
                 outputPointer++, inputPointer++) {
                *outputPointer = _elementOperationType::Operate(*inputPointer, inputScalar);
            }
        }
    };


    /*!  \brief Implement operation of the form \f$vo = op(si, vi)\f$ for
      compact containers.

      This class uses a loop to perform binary vector operations
      of the form
      \f[
      v_o = \mathrm{op}(s_i, v_i)
      \f]

      \param _elementOperationType The type of the binary operation.
    */
    template<class _elementOperationType>
    class CoSiCi {
    public:
        template<class _outputOwnerType, class _inputScalarType, class _inputOwnerType>
        static inline void Run(_outputOwnerType & outputOwner,
                               const _inputScalarType inputScalar,
                               const _inputOwnerType & inputOwner) {

            typedef _outputOwnerType OutputOwnerType;
            typedef typename OutputOwnerType::pointer OutputPointerType;
            typedef typename OutputOwnerType::size_type size_type;

            typedef _inputOwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            const size_type size = outputOwner.size();

            OutputPointerType outputPointer = outputOwner.Pointer();
            const OutputPointerType outputEnd = outputPointer + size;

            InputPointerType inputPointer = inputOwner.Pointer();

            for (;
                 outputPointer != outputEnd;
                 outputPointer++, inputPointer++) {
                *outputPointer = _elementOperationType::Operate(inputScalar, *inputPointer);
            }
        }
    };


    /*!  \brief Implement operation of the form \f$ v_{io} =
      op(v_{io}, s_i)\f$ for compact containers.

      This class uses a loop to perform binary vector
      operations of the form

      \f[
      v_{io} = \mathrm{op}(v_{io}, s_{i})
      \f]

      \param _elementOperationType the type of the binary operation
    */
    template<class _elementOperationType>
    class CioSi {
    public:
        template<class _inputOutputOwnerType, class _inputScalarType>
        static void Run(_inputOutputOwnerType & inputOutputOwner,
                        const _inputScalarType inputScalar) {

            typedef _inputOutputOwnerType InputOutputOwnerType;
            typedef typename InputOutputOwnerType::pointer InputOutputPointerType;
            typedef typename InputOutputOwnerType::size_type size_type;

            const size_type size = inputOutputOwner.size();

            InputOutputPointerType inputOutputPointer = inputOutputOwner.Pointer();
            const InputOutputPointerType inputOutputEnd = inputOutputPointer + size;;

            for (;
                 inputOutputPointer != inputOutputEnd;
                 inputOutputPointer++) {
                _elementOperationType::Operate(*inputOutputPointer, inputScalar);
            }
        }
    };


    /*!  \brief Implement operation of the form \f$v_o = op(v_i)\f$ for
      compact containers.

      This class uses a loop to perform unary vector operations
      of the form
      \f[
      v_{o} = \mathrm{op}(v_{i})
      \f]

      \param _elementOperationType The type of the unary operation.
    */
    template<class _elementOperationType>
    class CoCi {
    public:
        template<class _outputOwnerType, class _inputOwnerType>
        static inline void Run(_outputOwnerType & outputOwner,
                               const _inputOwnerType & inputOwner) {

            typedef _outputOwnerType OutputOwnerType;
            typedef typename OutputOwnerType::pointer OutputPointerType;
            typedef typename OutputOwnerType::size_type size_type;

            typedef _inputOwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            OutputPointerType outputPointer = outputOwner.Pointer();
            const size_type size = outputOwner.size();
            const OutputPointerType outputEnd = outputPointer + size;

            InputPointerType inputPointer = inputOwner.Pointer();

            for (;
                 outputPointer != outputEnd;
                 outputPointer++, inputPointer++) {
                *outputPointer = _elementOperationType::Operate(*inputPointer);
            }
        }
    };



    /*!  \brief Implement operation of the form \f$v_{io} =
      op(v_{io})\f$ for compact containers

      This class uses a loop to perform unary store back vector
      operations of the form

      \f[
      v_{io} = \mathrm{op}(v_{io})
      \f]

      \param _elementOperationType The type of the unary operation.
    */
    template<class _elementOperationType>
    class Cio {
    public:
        template<class _inputOutputOwnerType>
        static inline void Run(_inputOutputOwnerType & inputOutputOwner) {

            typedef _inputOutputOwnerType InputOutputOwnerType;
            typedef typename InputOutputOwnerType::pointer InputOutputPointerType;
            typedef typename InputOutputOwnerType::size_type size_type;

            const size_type size = inputOutputOwner.size();

            InputOutputPointerType inputOutputPointer = inputOutputOwner.Pointer();
            const InputOutputPointerType inputOutputEnd = inputOutputPointer + size;

            for (;
                 inputOutputPointer != inputOutputEnd;
                 inputOutputPointer++) {
                _elementOperationType::Operate(*inputOutputPointer);
            }
        }
    };



    /*!  \brief Implement operation of the form \f$s_o =
      op_{incr}(op(v_i))\f$ for compact containers

      This class uses a loop to perform incremental
      unary vector operations of the form

      \f[
      s_o = \mathrm{op_{incr}(\mathrm{op}(v_i))}
      \f]

      \param _elementOperationType The type of the unary operation.
    */
    template<class _incrementalOperationType, class _elementOperationType>
    class SoCi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;

        template<class _inputOwnerType>
        static OutputType Run(const _inputOwnerType & inputOwner) {

            typedef _inputOwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;
            typedef typename InputOwnerType::size_type size_type;

            const size_type size = inputOwner.size();
            OutputType incrementalResult = _incrementalOperationType::NeutralElement();

            InputPointerType inputPointer = inputOwner.Pointer();
            const InputPointerType inputEnd = inputPointer + size;

            for (;
                 inputPointer != inputEnd;
                 inputPointer++) {
                incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                                                                       _elementOperationType::Operate(*inputPointer));
            }
            return incrementalResult;
        }
    };


    /*!  \brief Implement operation of the form \f$s_o =
      op_{incr}(op(v_{i1}, v_{i2}))\f$ for compact containers.

      This class uses a loop to perform incremental binary vector
      operations of the form

      \f[
      s_o = \mathrm{op_{incr}(\mathrm{op}(v_{i1}, v_{i2}))}
      \f]

      \param _incrementalOperationType The type of the incremental
      operation.

      \param _elementOperationType The type of the unary operation.
    */
    template<class _incrementalOperationType, class _elementOperationType>
    class SoCiCi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;

        template<class _input1OwnerType, class _input2OwnerType>
        static inline OutputType Run(const _input1OwnerType & input1Owner,
                                     const _input2OwnerType & input2Owner) {

            typedef _input1OwnerType Input1OwnerType;
            typedef typename Input1OwnerType::const_pointer Input1PointerType;
            typedef typename Input1OwnerType::size_type size_type;

            typedef _input2OwnerType Input2OwnerType;
            typedef typename Input2OwnerType::const_pointer Input2PointerType;

            const size_type size = input1Owner.size();

            OutputType incrementalResult = _incrementalOperationType::NeutralElement();

            Input1PointerType input1Pointer = input1Owner.Pointer();
            const Input1PointerType input1End = input1Pointer + size;

            Input2PointerType input2Pointer = input2Owner.Pointer();

            for (;
                 input1Pointer != input1End;
                 input1Pointer++, input2Pointer++) {
                incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                                                                       _elementOperationType::Operate(*input1Pointer,
                                                                                                      *input2Pointer));
            }
            return incrementalResult;
        }
    };


    /*!  \brief Implement operation of the form \f$v_{io} =
      op_{io}(v_{io}, op_{sv}(s, v_i))\f$ for compact containers.

      This class uses template specialization to perform store-back
      vector-scalar-vector operations

      \f[
      v_{io} = \mathrm{op_{io}}(V_{io}, \mathrm{op_{sv}}(s, v_i))
      \f]

      \param _ioOperationType The type of the store-back operation.

      \param _scalarOwnerElementOperationType The type of the
	  operation between scalar and input vector.
    */
	template<class _ioElementOperationType, class _scalarElementOperationType>
	class CioSiCi {
	public:
        template<class _ioOwnerType, class _inputScalarType, class _inputOwnerType>
        static inline void Run(_ioOwnerType & ioOwner,
                               const _inputScalarType inputScalar, const _inputOwnerType & inputOwner)
		{
			typedef _ioOwnerType IoOwnerType;
			typedef typename IoOwnerType::pointer IoPointerType;
			typedef typename IoOwnerType::size_type size_type;

            typedef _inputOwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;

            const size_type size = ioOwner.size();

            IoPointerType ioPointer = ioOwner.Pointer();
            const IoPointerType ioEnd = ioPointer + size;

            InputPointerType inputPointer = inputOwner.Pointer();

            for (;
                 ioPointer != ioEnd;
                 ioPointer++, inputPointer++)
			{
				_ioElementOperationType::Operate(*ioPointer,
                                                 _scalarElementOperationType::Operate(inputScalar, *inputPointer));
            }
		}

	};



    /*!  \brief Implement operation of the form \f$v_{io} =
      op_{io}(v_{io}, op_{vv}(v_{i1}, v_{i2}))\f$ for compact containers.

      This class uses template specialization to perform store-back
      vector-vector-vector operations

      \f[
      v_{io} = \mathrm{op_{io}}(V_{io}, \mathrm{op_{vv}}(v_{i1}, v_{i2}))
      \f]

      \param _ioOperationType The type of the store-back operation.

      \param _ownerElementOperationType The type of the
	  element wise operation between input vectors.
    */
	template<class _ioElementOperationType, class _ownerElementOperationType>
	class CioCiCi {
	public:
        template<class _ioOwnerType, class _input1OwnerType, class _input2OwnerType>
        static inline void Run(_ioOwnerType & ioOwner,
                               const _input1OwnerType & input1Owner, const _input2OwnerType & input2Owner)
		{
			typedef _ioOwnerType IoOwnerType;
			typedef typename IoOwnerType::pointer IoPointerType;
			typedef typename IoOwnerType::size_type size_type;

            typedef _input1OwnerType Input1OwnerType;
            typedef typename Input1OwnerType::const_pointer Input1PointerType;

            typedef _input2OwnerType Input2OwnerType;
            typedef typename Input2OwnerType::const_pointer Input2PointerType;

            const size_type size = ioOwner.size();

            IoPointerType ioPointer = ioOwner.Pointer();
            const IoPointerType ioEnd = ioPointer + size;

            Input1PointerType input1Pointer = input1Owner.Pointer();
            Input2PointerType input2Pointer = input2Owner.Pointer();

            for (;
                 ioPointer != ioEnd;
                 ioPointer++, input1Pointer++, input2Pointer++)
			{
				_ioElementOperationType::Operate(*ioPointer,
                                                 _ownerElementOperationType::Operate(*input1Pointer, *input2Pointer));
            }
		}

	};


    /*!  \brief Implement operation of the form \f$s_o =
      op_{incr}(op(v_i, s_i))\f$ for compact containers.

      This class uses a loop to perform incremental
      binary vector operations of the form

      \f[
      s_o = \mathrm{op_{incr}(\mathrm{op}(v_i, s_i))}
      \f]

      \param _incrementalOperationType The type of the incremental
      operation.

      \param _elementOperationType The type of the unary operation.
    */
    template<class _incrementalOperationType, class _elementOperationType>
    class SoCiSi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;

        template<class _inputOwnerType, class _inputScalarType>
        static inline OutputType Run(const _inputOwnerType & inputOwner,
                                     const _inputScalarType & inputScalar) {

            typedef _inputOwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;
            typedef typename InputOwnerType::size_type size_type;

            const size_type size = inputOwner.size();
            OutputType incrementalResult = _incrementalOperationType::NeutralElement();

            InputPointerType inputPointer = inputOwner.Pointer();
            const InputPointerType inputEnd = inputPointer + size;

            for (;
                 inputPointer != inputEnd;
                 inputPointer++) {
                incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                                                                       _elementOperationType::Operate(*inputPointer, inputScalar));
            }
            return incrementalResult;
        }
    };


    class MinAndMax {
    public:
        template<class _inputOwnerType>
        static void Run(const _inputOwnerType & inputOwner, typename _inputOwnerType::value_type & minValue,
                        typename _inputOwnerType::value_type & maxValue)
        {
            typedef _inputOwnerType InputOwnerType;
            typedef typename InputOwnerType::const_pointer InputPointerType;
            typedef typename InputOwnerType::size_type size_type;
            typedef typename InputOwnerType::value_type value_type;

            InputPointerType inputPointer = inputOwner.Pointer();

            const size_type size = inputOwner.size();
            const InputPointerType inputEnd = inputPointer + size;

            value_type minElement, maxElement;
            maxElement = minElement = *inputPointer;

            for (;
                 inputPointer != inputEnd;
                 inputPointer++) {
                const value_type element = *inputPointer;
                if (element < minElement) {
                    minElement = element;
                } else if (maxElement < element) {
                    maxElement = element;
                }
            }
            minValue = minElement;
            maxValue = maxElement;
        }
    };

};


#endif  // _vctDynamicCompactLoopEngines_h

