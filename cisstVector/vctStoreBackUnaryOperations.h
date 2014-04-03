/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky, Anton Deguet
  Created on:	2003-08-18

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctStoreBackUnaryOperations_h
#define _vctStoreBackUnaryOperations_h

/*!
  \file
  \brief Declaration of vctStoreBackUnaryOperations.
 */

/*!  \brief Define store back unary operations on an object as
  classes.

  Class vctStoreBackUnaryOperations is an envelope that wraps unary
  store back operations on an object as classes.
  vctStoreBackUnaryOperations defines internal classes such as
  MakeAbs, MakeNegation, etc., and each of the internal classes has
  one static function named Operate(), which wraps the corresponding
  operation.  The signature of a typical Operate() is

  <PRE> static inline InputOutputType Operate(const InputOutputType & inputOutput)
  </PRE>

  Where InputOutputType is the type of the result and the operand.

  By abstracting very simple operations as inline functions, we can
  plug the vctUnaryOperations internal classes into templated vector
  operation engines.  See an example of such engine in
  vctFixedSizeVectorRecursiveEngines.h , and an example of plugging an
  operation into the engine in vctFixedSizeVectorOperations.h .

  \param inputOutputElementType the type of the operand

  \sa MakeAbs MakeNegation
*/
template<class _inputOutputElementType>
class vctStoreBackUnaryOperations
{
    public:
    typedef _inputOutputElementType InputOutputElementType;

    /*!
      \brief Returns the absolute value of the input as an OutputType object.
      \sa vctStoreBackUnaryOperations
     */
    class MakeAbs {
    public:
        /*!
          Execute the operation.
          \param inputOutput Input output.
        */
        static inline InputOutputElementType Operate(InputOutputElementType & inputOutput) {
            return (inputOutput = (inputOutput > InputOutputElementType(0))
                    ?
                    InputOutputElementType(inputOutput)
                    :
                    InputOutputElementType(-inputOutput));
        }
    };

    /*!
      \brief Returns the negation of the input as an OutputType object.
      \sa vctStoreBackUnaryOperations
     */
    class MakeNegation {
    public:
        /*!
          Execute the operation.
          \param inputOutput Input output.
        */
        static inline InputOutputElementType Operate(InputOutputElementType & inputOutput) {
            return (inputOutput = InputOutputElementType(-inputOutput));
        }
    };

    /*! \brief Returns the floor of the input, that is, the largest integer less-than
      or equal to the input, as an OutputType object.
      \sa vctUnaryOperations
     */
    class MakeFloor {
    public:
        static inline InputOutputElementType Operate(InputOutputElementType & inputOutput) {
            return (inputOutput = InputOutputElementType( floor((double)inputOutput) ));
        }
    };

    /*! \brief Returns the ceiling of the input, that is, the smallest integer greater-than
      or equal to the input, as an OutputType object.
      \sa vctUnaryOperations
     */
    class MakeCeil {
    public:
        static inline InputOutputElementType Operate(InputOutputElementType & inputOutput) {
            return (inputOutput = InputOutputElementType( ceil((double)inputOutput) ));
        }
    };


};


#endif  // _vctStoreBackUnaryOperations_h

