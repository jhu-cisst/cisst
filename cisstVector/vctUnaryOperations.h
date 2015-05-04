/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ofri Sadowsky
  Created on: 2003-08-18

  (C) Copyright 2003-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctUnaryOperations_h
#define _vctUnaryOperations_h

/*!
  \file
  \brief Declaration of vctUnaryOperations.
 */

/*!
  \brief Define unary operations on an object as classes.

  Class vctUnaryOperations is an envelope that wraps unary operations
  on an object as classes.  vctUnaryOperations defines internal
  classes such as Identity, AbsValue, etc., and each of the internal
  classes has one static function named Operate(), which wraps the
  corresponding operation.  The signature of a typical Operate() is

  <PRE> static inline OutputType Operate(const InputType & input)
  </PRE>

  Where OutputType, InputType, are the types of the result and
  the operand.

  By abstracting very simple operations as inline functions, we can
  plug the vctUnaryOperations internal classes into templated vector
  operation engines.  See an example of such engine in
  vctFixedSizeVectorRecursiveEngines.h , and an example of plugging an operation into
  the engine in vctFixedSizeVectorOperations.h .

  \param inputElementType the type of the first (left-side) operand
  \param outputElementType the type of the result

  \sa Identity AbsValue Square Negation
*/
template<class _outputElementType, class _inputElementType = _outputElementType>
class vctUnaryOperations
{
    public:
    typedef _outputElementType OutputElementType;
    typedef _inputElementType InputElementType;

    /*!
      \brief Returns the input as an OutputType object.
      \sa vctUnaryOperations
     */
    class Identity {
    public:
        /*!
          Execute the operation.
          \param input Input.
        */
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(input);
        }
    };

    /*!
      \brief Returns the absolute value of the input as an OutputType object.
      \sa vctUnaryOperations
     */
    class AbsValue {
    public:
        /*!
          Execute the operation.
          \param input Input.
        */
        static inline OutputElementType Operate(const InputElementType & input) {
            return (input > InputElementType(0)) ? OutputElementType(input) : OutputElementType(-input);
        }
    };

    /*! \brief Returns the floor of the input, that is, the largest integer less-than
      or equal to the input, as an OutputType object.
      \sa vctUnaryOperations
     */
    class Floor {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType( floor((double)input) );
        }
    };

    /*! \brief Returns the ceiling of the input, that is, the smallest integer greater-than
      or equal to the input, as an OutputType object.
      \sa vctUnaryOperations
     */
    class Ceil {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType( ceil((double)input) );
        }
    };



    /*!
      \brief Returns the square of the input as an OutputType object.
      \sa vctUnaryOperations
     */
    class Square {
    public:
        /*!
          Execute the operation.
          \param input Input.
        */
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(input * input);
        }
    };

    /*!
      \brief Returns the negative of the input as an OutputType object.
      \sa vctUnaryOperations
     */
    class Negation {
    public:
        /*!
          Execute the operation.
          \param input Input.
        */
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(-input);
        }
    };

    /*! Return true if the input element is strictly positive, false otherwise */
    class IsPositive {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(input > InputElementType(0));
        }
    };

    /*! Return true if the input element is non-negative, false otherwise */
    class IsNonNegative {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(input >= InputElementType(0));
        }
    };

    /*! Return true if the input element is non-positive, false otherwise */
    class IsNonPositive {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(input <= InputElementType(0));
        }
    };

    /*! Return true if the input element is strictly negative, false otherwise */
    class IsNegative {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(input < InputElementType(0));
        }
    };

    /*! Return true if the input element is nonzero */
    class IsNonzero {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(input != InputElementType(0));
        }
    };

    /*! Return true if the input element is finite */
    class IsFinite {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(cmnTypeTraits<InputElementType>::IsFinite(input));
        }
    };

    /*! Return true if the input element is NaN */
    class IsNaN {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(cmnTypeTraits<InputElementType>::IsNaN(input));
        }
    };

    /*    template<unsigned int EXPONENT>
    class Power {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            OutputElementType rootOfPower = Power<EXPONENT / 2>::OperationType(input);
            return ((EXPONENT % 2 == 0)
                    ? (rootOfPower * rootOfPower)
                    : (rootOfPower * rootOfPower * input));
        }
    };

    class Power<0> {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(1);
        }
    };
    */
};


#endif  // _vctUnaryOperations_h

