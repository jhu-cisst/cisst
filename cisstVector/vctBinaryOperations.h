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
#ifndef _vctBinaryOperations_h
#define _vctBinaryOperations_h

/*!
  \file
  \brief Declaration of vctBinaryOperations.
 */

#include <cisstCommon/cmnTypeTraits.h>

/*!
  \brief Define binary operations between objects as classes.

  Class vctBinaryOperations is an envelope that wraps binary
  operations between objects as classes.  BinaryOperations defines
  internal classes such as Addition, Subtraction, etc., and each of
  the internal classes has one static function named Operate(), which
  wraps the corresponding operation.  The signature of a typical
  Operate() is

  <PRE>
  static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2)
  </PRE>

  Where OutputType, Input1Type, Input2Type are the types of the result
  and the two operands.

  By abstracting very simple operations as inline functions, we can
  plug the BinaryOperations internal classes into templated vector
  operation engines.  See an example of such engine in
  vctFixedSizeVectorRecursiveEngines.h , and an example of plugging an operation into
  the engine in vctFixedSizeVectorOperations.h .

  \param outputType the type of the result
  \param input1Type the type of the first (left-side) operand
  \param input2Type the type of the second (right-side) operand)

  \sa Addition Subtraction Multiplication Division FirstOperand SecondOperand Maximum Minimum
*/
template<class _outputType, class _input1Type = _outputType, class _input2Type = _outputType>
class vctBinaryOperations {
    public:
    typedef _outputType OutputType;
    typedef _input1Type Input1Type;
    typedef _input2Type Input2Type;

    /*!
      \brief Returns the sum of the two InputType object.
      \sa vctBinaryOperations
    */
    class Addition {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1+input2);
        }

        static inline OutputType NeutralElement()
        {
            return OutputType(0);
        }
    };

    /*!
      \brief Returns the difference of the two InputType object.
      \sa vctBinaryOperations
    */
    class Subtraction {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1-input2);
        }

        static inline OutputType NeutralElement()
        {
            return OutputType(0);
        }
    };

    /*!
      \brief Returns the product of the two InputType object.
      \sa vctBinaryOperations
    */
    class Multiplication {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1*input2);
        }

        static inline OutputType NeutralElement()
        {
            return OutputType(1);
        }
    };

    /*!
      \brief Returns the ratio of the two InputType object.
      \sa vctBinaryOperations
    */
    class Division {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1 / input2);
        }

        static inline OutputType NeutralElement()
        {
            return OutputType(1);
        }
    };


    /*!
      \brief Return the first operand of (input1, input2), i.e., input1.
      \sa vctBinaryOperations
    */
    class FirstOperand {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & CMN_UNUSED(input2)) {
            return OutputType(input1);
        }
    };

    /*!
      \brief Return the second operand of (input1, input2), i.e., input2.
      \sa vctBinaryOperations
      Usage example is in recursive assignment.  See the function
      vctFixedSizeVectorOperations::SetAll
    */
    class SecondOperand {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & CMN_UNUSED(input1), const Input2Type & input2) {
            return OutputType(input2);
        }
    };

    /*!
      \brief Return the greater of (input1, input2).
      \sa vctBinaryOperations
      Return the greater of (input1, input2).  Note that here all
      arguments are of the same type, because they have to be
      compared.
    */
    class Maximum {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline const OutputType & Operate(const OutputType & input1, const OutputType & input2) {
            return (input1 > input2) ? input1 : input2;
        }

        static inline OutputType NeutralElement()
        {
            return cmnTypeTraits<OutputType>::MinNegativeValue();
        }
    };

    /*!
      \brief Return the lesser of (input1, input2).
      \sa vctBinaryOperations
      Return the lesser of (input1, input2).  Note that here all
      arguments are of the same type, because they have to be
      compared.
    */
    class Minimum {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline const OutputType & Operate(const OutputType & input1, const OutputType & input2) {
            return (input1 < input2) ? input1 : input2;
        }

        static inline OutputType NeutralElement()
        {
            return cmnTypeTraits<OutputType>::MaxPositiveValue();
        }
    };

    /*!  \brief Return the -input2 in input1 is lesser than -input2,
      input2 if input 1 is greater than input2 and input1 otherwise.
      The operator doesn't test if input2 is negative.

      \sa vctBinaryOperations
      input1 capped in range [-input2, input2].  Note that here all
      arguments are of the same type, because they have to be
      compared.
    */
    class ClipIn {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline const OutputType Operate(const OutputType & bound, const OutputType & input) {
            if (input > bound) {
                return bound;
            } else if (input < -bound) {
                return -bound;
            }
            return input;
        }

        static inline OutputType NeutralElement(void) {
            return cmnTypeTraits<OutputType>::MaxPositiveValue();
        }
    };


    /*!
      \brief Test for equality between input1 and input2
      \sa vctBinaryOperations

      Returns a boolean, i.e. true if the input1 is equal to input2.
      Note that input1 and input2 should be of the same type.
    */
    class Equal {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1 == input2);
        }
    };

    /*!
      \brief Test for non equality between input1 and input2
      \sa vctBinaryOperations

      Returns a boolean, i.e. true if the input1 is not equal to input2.
      Note that input1 and input2 should be of the same type.
    */
    class NotEqual {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1 != input2);
        }
    };

    /*!
      \brief Test if input1 is lesser than input2
      \sa vctBinaryOperations

      Returns a boolean, i.e. true if the input1 is lesser than input2.
      Note that input1 and input2 should be of the same type.
    */
    class Lesser {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1 < input2);
        }
    };

    /*!
      \brief Test if input1 is lesser than or equal to input2
      \sa vctBinaryOperations

      Returns a boolean, i.e. true if the input1 is lesser than or
      equal to input2.  Note that input1 and input2 should be of the
      same type.
    */
    class LesserOrEqual {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1 <= input2);
        }
    };

    /*!
      \brief Test if input1 is greater than input2
      \sa vctBinaryOperations

      Returns a boolean, i.e. true if the input1 is greater than input2.
      Note that input1 and input2 should be of the same type.
    */
    class Greater {
    public:
        typedef bool OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1 > input2);
        }
    };

    /*!
      \brief Test if input1 is greater than or equal to input2
      \sa vctBinaryOperations

      Returns a boolean, i.e. true if the input1 is greater than or
      equal to input2.  Note that input1 and input2 should be of the
      same type.
    */
    class GreaterOrEqual {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1 >= input2);
        }
    };

    /*!
      \brief Logical And operator between input1 and input2
      \sa vctBinaryOperations

      Note that here all arguments are of the same type.
    */
    class And {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1 && input2);
        }

        static inline OutputType NeutralElement()
        {
            return OutputType(true);
        }
    };

    /*!
      \brief Logical Or operator between input1 and input2
      \sa vctBinaryOperations

      Note that here all arguments are of the same type.
    */
    class Or {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const OutputType & input1, const OutputType & input2) {
            return OutputType(input1 || input2);
        }

        static inline OutputType NeutralElement()
        {
            return OutputType(false);
        }
    };

    /*!
      \brief Test if the first argument is bound by the second argument.
      \sa vctBinaryOperations

      Tests if if abs(input1) <= input2.  The test does not use the
      abs function, but tests if (-input2 <= input1) && (input1 <=
      input2).  Note that if input2 is negative, the result is always
      false.
    */
    class Bound {
    public:
        typedef _outputType OutputType;
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType( (-input2 <= input1) && (input1 <= input2) );
        }
    };


    /*!
      \brief Dot product.
      \sa vctBinaryOperations

      This method is basically a wrapper around the method
      input1.DotProduct(input2).  It is used for the multiplication of
      two matrices (see vctFixedSizeMatrix).
    */
    class DotProduct {
    public:
        typedef _outputType OutputType;
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return input1.DotProduct(input2);
        }
    };


};

// Specialize Addition for bool to avoid compiler warnings. Other binary operations
// can be specialized if needed.
template<>
class vctBinaryOperations<bool, bool, bool>::Addition {
    public:
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline bool Operate(const bool & input1, const bool & input2) {
            return (input1|input2);
        }

        static inline bool NeutralElement()
        {
            return false;
        }
};


#endif  // _vctBinaryOperations_h

