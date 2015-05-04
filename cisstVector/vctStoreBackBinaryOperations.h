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
#ifndef _vctStoreBackBinaryOperations_h
#define _vctStoreBackBinaryOperations_h

/*!
  \file
  \brief Declaration of vctStoreBackBinaryOperations
 */

/*!
  \brief Define binary operations between objects as classes.

  Class vctStoreBackBinaryOperations is an envelope that wraps binary
  store back operations between objects as classes.
  StoreBackOperations defines internal classes such as Addition,
  Subtraction, etc., and each of the internal classes has one static
  function named Operate(), which wraps the corresponding operation.
  The signature of a typical Operate() is

  <PRE>
  static inline OutputType & Operate(Input1Type & input1, const Input2Type & input2)
  </PRE>

  Where OutputType, Input1Type, Input2Type are the types of the result
  and the two operands.

  By abstracting very simple operations as inline functions, we can
  plug the StoreBackOperations internal classes into templated vector
  operation engines.  See an example of such engine in
  vctFixedSizeVectorRecursiveEngines.h , and an example of plugging an
  operation into the engine in vctFixedSizeVectorOperations.h .

  \param inputOutputType the type of the first (left-side) operand, which is an input output
  \param input2Type the type of the second (right-side) operand)

  \note This class contains the binary operation Swap, whose operation signature
  is
  <PRE>
  static inline void Operate(Input1Type & input1, Input2Type & input2)
  </PRE>
  with no const or return type.  It was included here since we didn't want
  to have a special file for just one operation.

  \sa Addition Subtraction Multiplication Division FirstOperand SecondOperand Maximum Minimum
*/
template<class _inputOutputType, class _input2Type = _inputOutputType>
class vctStoreBackBinaryOperations {
    public:
    typedef _inputOutputType InputOutputType;
    typedef _input2Type Input2Type;

    /*!
      \brief Returns the sum of the two InputType object.
      \sa vctStoreBackBinaryOperations
    */
    class Addition {
    public:
        /*!
          Execute the operation.
          \param input1 First operand (input and output).
          \param input2 Second operand.
        */
        static inline InputOutputType & Operate(InputOutputType & input1, const Input2Type & input2) {
            input1 += input2;
            return input1;
        }
    };


    /*!
      \brief Returns the difference of the two InputType object.
      \sa vctStoreBackBinaryOperations
    */
    class Subtraction {
    public:
        /*!
          Execute the operation.
          \param input1 First operand (input and output).
          \param input2 Second operand.
        */
        static inline InputOutputType & Operate(InputOutputType & input1, const Input2Type & input2) {
            input1 -= input2;
            return input1;
        }
    };


    /*!
      \brief Returns the product of the two InputType object.
      \sa vctStoreBackBinaryOperations
    */
    class Multiplication {
    public:
        /*!
          Execute the operation.
          \param input1 First operand (input and output).
          \param input2 Second operand.
        */
        static inline InputOutputType & Operate(InputOutputType & input1, const Input2Type & input2) {
            input1 *= input2;
            return input1;
        }
    };


    /*!
      \brief Returns the ratio of the two InputType object.
      \sa vctStoreBackBinaryOperations
    */
    class Division {
    public:
        /*!
          Execute the operation.
          \param input1 First operand (input and output).
          \param input2 Second operand.
        */
        static inline InputOutputType & Operate(InputOutputType & input1, const Input2Type & input2) {
            input1 /= input2;
            return input1;
        }
    };


    /*!
      \brief Return the first operand of (input1, input2), i.e., input1.
      \sa vctStoreBackBinaryOperations
    */
    class FirstOperand {
    public:
        /*!
          Execute the operation.
          \param input1 First operand (input and output).
          \param input2 Second operand (unused)
        */
        static inline InputOutputType & Operate(InputOutputType & input1, const Input2Type & CMN_UNUSED(input2)) {
            return (input1);
        }
    };


    /*!
      \brief Return the first operand of (input1, input2), i.e., input2.
      \sa vctStoreBackBinaryOperations
      Usage example is in recursive assignment.  See the function FixedSizeVectorOperations::SetAll
    */
    class SecondOperand {
    public:
        /*!
          Execute the operation.
          \param input1 First operand (input and output).
          \param input2 Second operand.
        */
        static inline InputOutputType & Operate(InputOutputType & input1, const Input2Type & input2) {
            input1 = input2;
            return input1;
        }
    };


    /*!
      \brief Return the greater of (input1, input2).
      \sa vctStoreBackBinaryOperations
      Return the greater of (input1, input2).  Note that here all arguments are of the
      same type, because they have to be compared.
    */
    class Maximum {
    public:
        /*!
          Execute the operation.
          \param input1 First operand (input and output).
          \param input2 Second operand.
        */
        static inline const InputOutputType & Operate(InputOutputType & input1,
                                                      const InputOutputType & input2) {
            input1 = (input1 > input2) ? input1 : input2;
            return input1;
        }
    };



    class ClipIn {
    public:
        /*!
          Execute the operation.
          \param input First operand (input and output).
          \param bound Second operand.
        */
        static inline const InputOutputType & Operate(InputOutputType & input,
                                                      const InputOutputType & bound) {
            if (input > bound) {
                input = bound;
            } else if (input < -bound) {
                input = -bound;
            }
            return input;
        }
    };


    /*!
      \brief Return the of lesser (input1, input2).
      \sa vctStoreBackBinaryOperations
      Return the lesser of (input1, input2).  Note that here all arguments are of the
      same type, because they have to be compared.
    */
    class Minimum {
    public:
        /*!
          Execute the operation.
          \param input1 First operand (input and output).
          \param input2 Second operand.
        */
        static inline const InputOutputType & Operate(InputOutputType & input1,
                                                      const InputOutputType & input2) {
            input1 = (input1 < input2) ? input1 : input2;
            return input1;
        }
    };

    /*!
      \brief Swap the values of input1, input2.
      \sa vctStoreBackBinaryOperations
      Swap the values of input1 and input2.  The operation has no return value.
      The operands have to be of the same type.
    */
    class Swap {
    public:
        /*!
          Execute the operation.
          \param input1 First operand (input and output).
          \param input2 Second operand (input and output).
        */
        static inline void Operate(InputOutputType & input1,
                                   InputOutputType & input2) {
            const InputOutputType tmp(input1);
            input1 = input2;
            input2 = tmp;
        }
    };

};


#endif  // _vctStoreBackBinaryOperations_h

