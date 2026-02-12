/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ofri Sadowsky
  Created on: 2004-11-04

  (C) Copyright 2004-2022 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _vctGenericRotationTest_h
#define _vctGenericRotationTest_h

#include <cisstCommon/cmnTypeTraits.h>
#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnRandomSequence.h>

#include <cisstVector/vctTypes.h>

// template<class _rotationType>
class vctGenericRotationTest
{
public:
    /*! Test the conversion between the other rotation type and the main rotation
      type of this instance.  The test is by converting the input initial rotation
      from this type to the other type, then back to this type.  The result is
      asserted to be an equal rotation (up to tolerance) to the initial rotation.
    */
    template<class _rotationType1, class _rotationType2>
    static void TestConversion(const _rotationType1 & initialRotation,
                               _rotationType2 & secondRotation,
                               typename _rotationType1::value_type tolerance
                               = cmnTypeTraits<typename _rotationType1::value_type>::Tolerance())
    {
        std::string message;
        CPPUNIT_ASSERT(initialRotation.IsNormalized());
        secondRotation.From(initialRotation);
        secondRotation.NormalizedSelf();
        _rotationType1 finalRotation(secondRotation);
        finalRotation.NormalizedSelf();
        message = std::string("finalRotation:\n") + finalRotation.ToString() + std::string("\ninitialRotation:\n") + initialRotation.ToString();
        CPPUNIT_ASSERT_MESSAGE(message, finalRotation.AlmostEquivalent(initialRotation, tolerance));
        CPPUNIT_ASSERT(initialRotation.AlmostEquivalent(finalRotation, tolerance));
        finalRotation.From(secondRotation);
        finalRotation.NormalizedSelf();
        CPPUNIT_ASSERT(finalRotation.AlmostEquivalent(initialRotation, tolerance));
        CPPUNIT_ASSERT(initialRotation.AlmostEquivalent(finalRotation, tolerance));

        _rotationType2 secondRotationBis(initialRotation);
        secondRotationBis.NormalizedSelf();
        finalRotation.From(secondRotationBis);
        finalRotation.NormalizedSelf();
        CPPUNIT_ASSERT(finalRotation.AlmostEquivalent(initialRotation, tolerance));
        CPPUNIT_ASSERT(initialRotation.AlmostEquivalent(finalRotation, tolerance));
    }



    template<class _rotationType1, class _rotationType2>
    static void TestFromSignatures(_rotationType1 & toRotation,
                                   const _rotationType2 & fromRotationNormalized,
                                   const _rotationType2 & fromRotationNotNormalized,
                                   const bool inputAlwaysNormalized = false,
                                   typename _rotationType1::value_type tolerance
                                   = cmnTypeTraits<typename _rotationType1::value_type>::Tolerance())
    {
        typedef typename _rotationType1::value_type value_type;
        _rotationType2 convertBack;

        // 6 cases possible, test all but FromRaw on not normalized input.
        bool gotException = false;
        try {
            toRotation.From(fromRotationNormalized);
        } catch (std::runtime_error &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(!gotException);
        convertBack.From(toRotation);
        CPPUNIT_ASSERT(convertBack.AlmostEquivalent(fromRotationNormalized, value_type(tolerance * 100.0)));

        gotException = false;
        try {
            toRotation.FromNormalized(fromRotationNormalized);
        } catch (std::runtime_error &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(!gotException);
        convertBack.From(toRotation);
        CPPUNIT_ASSERT(convertBack.AlmostEquivalent(fromRotationNormalized, value_type(tolerance * 100.0)));

        gotException = false;
        try {
            toRotation.FromRaw(fromRotationNormalized);
        } catch (std::runtime_error &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(!gotException);
        convertBack.From(toRotation);
        CPPUNIT_ASSERT(convertBack.AlmostEquivalent(fromRotationNormalized, value_type(tolerance * 100.0)));

        gotException = false;
        try {
            toRotation.FromNormalized(fromRotationNotNormalized);
        } catch (std::runtime_error &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(!gotException);
        convertBack.From(toRotation);
        CPPUNIT_ASSERT(convertBack.AlmostEquivalent(fromRotationNotNormalized.Normalized(),
                                                    value_type(tolerance * 100.0)));

        gotException = false;
        if (inputAlwaysNormalized) {
            try {
                toRotation.From(fromRotationNotNormalized);
            } catch (std::runtime_error &) {
                gotException = true;
            }
            CPPUNIT_ASSERT(!gotException);
            convertBack.From(toRotation);
            CPPUNIT_ASSERT(convertBack.AlmostEquivalent(fromRotationNotNormalized.Normalized(),
                                                        value_type(tolerance * 100.0)));
        } else {
            try {
                toRotation.From(fromRotationNotNormalized);
            } catch (std::runtime_error &) {
                gotException = true;
            }
            CPPUNIT_ASSERT(gotException);
        }
    }



    /*! Test the static Identity() function of the rotation class.  The identity
      element must have the following properties:  I*I=I, I*R=R*I=R, I*v=v .
      Finally, we verify that R*v != v.
      The properties are tested on an input random rotation R and random vector v.
      The tolerance for identity test is zero.
    */
    template <class _rotationType, class _vectorType>
    static void TestIdentity(const _rotationType & rotation,
                             const _vectorType & vector,
                             _vectorType & rotatedVector,
                             typename _rotationType::value_type tolerance
                             = cmnTypeTraits<typename _rotationType::value_type>::Tolerance())
    {
        const _rotationType & identity = _rotationType::Identity();
        _rotationType iSquare;
        identity.ApplyTo(identity, iSquare);
        CPPUNIT_ASSERT(iSquare.AlmostEquivalent(identity, tolerance));
        _rotationType IR;
        identity.ApplyTo(rotation, IR);
        CPPUNIT_ASSERT(IR.AlmostEquivalent(rotation, tolerance));
        rotation.ApplyTo(identity, IR);
        CPPUNIT_ASSERT(IR.AlmostEquivalent(rotation, tolerance));

        identity.ApplyTo(vector, rotatedVector);
        CPPUNIT_ASSERT(rotatedVector.AlmostEqual(vector, tolerance));

        if (rotation.AlmostEquivalent(identity, tolerance)) {
            return;
        }

        rotation.ApplyTo(vector, rotatedVector);
        CPPUNIT_ASSERT(! rotatedVector.AlmostEqual(vector, 0.0));
    }


    /*! Test that the default constructor creates identity transform.
      Also test that Equal, AlmostEqual and AlmostEquivalent exist and
      are not completely incorrect. */
    template <class _rotationType>
    static void TestDefaultConstructor(const _rotationType & CMN_UNUSED(rotation),
                                       typename _rotationType::value_type tolerance
                                       = cmnTypeTraits<typename _rotationType::value_type>::Tolerance())
    {
        const _rotationType & identity = _rotationType::Identity();
        _rotationType newRotation;
        CPPUNIT_ASSERT(newRotation.Equal(identity));
        CPPUNIT_ASSERT(newRotation == identity);
        CPPUNIT_ASSERT(newRotation.AlmostEqual(identity, tolerance));
        CPPUNIT_ASSERT(newRotation.AlmostEquivalent(identity, tolerance));
    }


    /*!  Assert that r and rInverse are mutual inverses of one another.  The
     validations include comparing the product of the two to the identity, and
     applying each one's inverse to the indeity to obtain the other.
    */
    template <class _rotationType>
    static void ValidateInverse(const _rotationType & rotation,
                                const _rotationType & rotationInverse,
                                typename _rotationType::value_type tolerance
                                = cmnTypeTraits<typename _rotationType::value_type>::Tolerance())
    {
        CPPUNIT_ASSERT(rotation.IsNormalized());
        CPPUNIT_ASSERT(rotationInverse.IsNormalized());
        const _rotationType & identity = _rotationType::Identity();
        _rotationType r_rInv;
        rotation.ApplyTo(rotationInverse, r_rInv);
        CPPUNIT_ASSERT(identity.AlmostEquivalent(r_rInv, tolerance));
        _rotationType rInv_r;
        rotationInverse.ApplyTo(rotation, rInv_r);
        CPPUNIT_ASSERT(identity.AlmostEquivalent(rInv_r, tolerance));

        _rotationType rInv;
        rotation.ApplyInverseTo(identity, rInv);
        CPPUNIT_ASSERT(rotationInverse.AlmostEquivalent(rInv, tolerance));
        CPPUNIT_ASSERT(rInv.AlmostEquivalent(rotationInverse, tolerance));

        _rotationType rotationInverseInv;
        rotationInverse.ApplyInverseTo(identity, rotationInverseInv);
        CPPUNIT_ASSERT(rotation.AlmostEquivalent(rotationInverseInv, tolerance));
        CPPUNIT_ASSERT(rotationInverseInv.AlmostEquivalent(rotation, tolerance));
    }



    /*! Create the inverse of r in all possible ways: InverseSelf, InverseOf, Inverse.
      Validate that the result is an inverse by callin ValidateInverse */
    template <class _rotationType>
    static void TestInverse(const _rotationType & rotation,
                            typename _rotationType::value_type tolerance
                            = cmnTypeTraits<typename _rotationType::value_type>::Tolerance())
    {
        _rotationType rotationInverse;
        rotationInverse.FromRaw(rotation);
        rotationInverse.InverseSelf();
        ValidateInverse(rotation, rotationInverse, tolerance);

        rotationInverse.InverseOf(rotation);
        ValidateInverse(rotation, rotationInverse, tolerance);

        rotationInverse = rotation.Inverse();
        ValidateInverse(rotation, rotationInverse, tolerance);
    }



    /*! Let R1, R2 be random rotations. Let v be a random vector.
      1. Compute u1=R2*(R1*v), u2=(R2*R1)*v. Assert that u1=u2 up to tolerance.
      2. Compute w1=R1^{-1}*(R2^{-1}*u1), w2=(R1^{-1}*R2^{-1})*u2, w3=(R2*R1)^{-1}*u1, w4=(R2*R1)^{-1})*u2.
      Assert that all these results are equal to v, up to tolerance.
    */
    template <class _rotationType, class _vectorType>
    static void TestComposition(const _rotationType & rotation1,
                                const _rotationType & rotation2,
                                const _vectorType & inputVector,
                                typename _rotationType::value_type tolerance
                                = cmnTypeTraits<typename _rotationType::value_type>::Tolerance())
    {
        _vectorType tmpVector(inputVector);

        rotation1.ApplyTo(inputVector, tmpVector);
        _vectorType u1(inputVector);
        rotation2.ApplyTo(tmpVector, u1);

        _rotationType rotation2_rotation1;
        rotation2.ApplyTo(rotation1, rotation2_rotation1);
        CPPUNIT_ASSERT(rotation2_rotation1.IsNormalized());
        _vectorType u2(inputVector);
        rotation2_rotation1.ApplyTo(inputVector, u2);

        CPPUNIT_ASSERT(u1.AlmostEqual(u2, tolerance));

        _rotationType rotation2Inv = rotation2.Inverse();
        _rotationType rotation1Inv = rotation1.Inverse();
        _rotationType rotation1Inv_rotation2Inv;
        rotation1Inv.ApplyTo(rotation2Inv, rotation1Inv_rotation2Inv);
        CPPUNIT_ASSERT(rotation1Inv_rotation2Inv.IsNormalized());
        _rotationType rotation2_rotation1_Inv = rotation2_rotation1.Inverse();
        CPPUNIT_ASSERT(rotation2_rotation1_Inv.IsNormalized());

        CPPUNIT_ASSERT(rotation1Inv_rotation2Inv.AlmostEquivalent(rotation2_rotation1_Inv, tolerance));

        rotation2Inv.ApplyTo(u1, tmpVector);
        _vectorType w1(inputVector);
        rotation1Inv.ApplyTo(tmpVector, w1);
        CPPUNIT_ASSERT(w1.AlmostEqual(inputVector, tolerance));

        _vectorType w2(inputVector);
        rotation1Inv_rotation2Inv.ApplyTo(u2, w2);
        CPPUNIT_ASSERT(w2.AlmostEqual(inputVector, tolerance));

        _vectorType w3(inputVector);
        rotation2_rotation1_Inv.ApplyTo(u1, w3);
        CPPUNIT_ASSERT(w3.AlmostEqual(inputVector, tolerance));
    }


    template <class _rotationType>
    static void TestRandom(const _rotationType & rotation,
                           typename _rotationType::value_type tolerance
                           = cmnTypeTraits<typename _rotationType::value_type>::Tolerance())

    {
        CPPUNIT_ASSERT(rotation.IsNormalized(tolerance));
        _rotationType rotation2, rotation3;
        vctRandom(rotation2);
        vctRandom(rotation3);
        CPPUNIT_ASSERT(rotation2.IsNormalized(tolerance));
        CPPUNIT_ASSERT(rotation3.IsNormalized(tolerance));
        // at least one should be different
        CPPUNIT_ASSERT((!rotation.AlmostEquivalent(rotation2, tolerance))
                       || (!rotation.AlmostEquivalent(rotation3, tolerance))
                       || (!rotation2.AlmostEquivalent(rotation3, tolerance)));
    }


    /*! Let R be a random rotation.
      1. For a random vector v, assert that ||_R*v_||=||_v_|| up to tolerance.
      2. For random vectors v1, v2, assert that v1.v2=(R*v1).(R*v2) (angle preservation).
      3. Non-reflection. Assert that R*(v1%v2)=(R*v1)%(R*v2)
    */
    template <class _rotationType, class _vectorType>
    static void TestRigidity(const _rotationType & rotation,
                             const _vectorType & vector1,
                             const _vectorType & vector2,
                             typename _rotationType::value_type tolerance
                             = cmnTypeTraits<typename _rotationType::value_type>::Tolerance())
    {
        typedef typename _vectorType::NormType NormType;
        NormType vector1Norm = vector1.Norm();
        NormType vector2Norm = vector2.Norm();
        _vectorType rotatedVector1(vector1);
        const _vectorType origin(0);
        _vectorType rotatedOrigin;
        rotation.ApplyTo(origin, rotatedOrigin);
        rotation.ApplyTo(vector1, rotatedVector1);
        NormType rotatedVector1Norm = (rotatedVector1 - rotatedOrigin).Norm();
        CPPUNIT_ASSERT(((rotatedVector1Norm - vector1Norm) < tolerance) && ((vector1Norm - rotatedVector1Norm) < tolerance));

        _vectorType rotatedVector2(vector1);
        rotation.ApplyTo(vector2, rotatedVector2);
        NormType rotatedVector2Norm = (rotatedVector2 - rotatedOrigin).Norm();
        CPPUNIT_ASSERT(((rotatedVector2Norm - vector2Norm) < tolerance) && ((vector2Norm - rotatedVector2Norm) < tolerance));

        NormType vector1_vector2 = vctDotProduct(vector1, vector2);
        NormType rvector1_rvector2 = vctDotProduct((rotatedVector1 - rotatedOrigin), (rotatedVector2 - rotatedOrigin));
        CPPUNIT_ASSERT(((rvector1_rvector2 - vector1_vector2) < tolerance) && ((vector1_vector2 - rvector1_rvector2) < tolerance));

        _vectorType vector1_x_vector2(vector1); vector1_x_vector2 = vctCrossProduct(vector1, vector2);
        _vectorType r__vector1_x_vector2(vector1); r__vector1_x_vector2 = rotation * vector1_x_vector2 - rotatedOrigin;
        _vectorType rvector1_x_rvector2(vector1); rvector1_x_rvector2 = vctCrossProduct((rotatedVector1 - rotatedOrigin), (rotatedVector2 - rotatedOrigin));
        _vectorType diff(vector1); diff = rvector1_x_rvector2 - r__vector1_x_vector2;
        CPPUNIT_ASSERT(diff.LinfNorm() < tolerance);
    }

    template <class _rotationType, class _vectorType>
    static void TestRigidity2D(const _rotationType & rotation,
			       const _vectorType & vector1,
			       const _vectorType & vector2,
			       typename _rotationType::value_type tolerance
			       = cmnTypeTraits<typename _rotationType::value_type>::Tolerance())
    {
        typedef typename _vectorType::NormType NormType;
        NormType vector1Norm = vector1.Norm();
        NormType vector2Norm = vector2.Norm();
        _vectorType rotatedVector1(vector1);
        rotation.ApplyTo(vector1, rotatedVector1);
        NormType rotatedVector1Norm = rotatedVector1.Norm();
        CPPUNIT_ASSERT(((rotatedVector1Norm - vector1Norm) < tolerance) && ((vector1Norm - rotatedVector1Norm) < tolerance));

        _vectorType rotatedVector2(vector1);
        rotation.ApplyTo(vector2, rotatedVector2);
        NormType rotatedVector2Norm = rotatedVector2.Norm();
        CPPUNIT_ASSERT(((rotatedVector2Norm - vector2Norm) < tolerance) && ((vector2Norm - rotatedVector2Norm) < tolerance));

        NormType vector1_vector2 = vctDotProduct(vector1, vector2);
        NormType rvector1_rvector2 = vctDotProduct(rotatedVector1, rotatedVector2);
        CPPUNIT_ASSERT(((rvector1_rvector2 - vector1_vector2) < tolerance) && ((vector1_vector2 - rvector1_rvector2) < tolerance));
    }


    /*! Test the various methods that apply rotation to a vector.  This includes
      ApplyTo, operator *, ApplyInverseOf, ...
    */
    template <class _rotationType, class _appliedToType>
    static void TestApplyMethodsOperatorsXform(const _rotationType & rotation,
                                               const _appliedToType & whatever,
                                               typename _rotationType::value_type tolerance
                                               = cmnTypeTraits<typename _rotationType::value_type>::Tolerance())
    {
        _appliedToType result1, result2;
        rotation.ApplyTo(whatever, result1);
        result2 = rotation.ApplyTo(whatever);
        CPPUNIT_ASSERT(result2.AlmostEquivalent(result1, tolerance));
        result2 = rotation * whatever;
        CPPUNIT_ASSERT(result2.AlmostEquivalent(result1, tolerance));

        rotation.ApplyInverseTo(whatever, result1);
        result2 = rotation.ApplyInverseTo(whatever);
        CPPUNIT_ASSERT(result2.AlmostEquivalent(result1, tolerance));
    }

    template <class _rotationType, class _appliedToType>
    static void TestApplyMethodsOperatorsObject(const _rotationType & rotation,
                                                const _appliedToType & whatever,
                                                typename _rotationType::value_type tolerance
                                                = cmnTypeTraits<typename _rotationType::value_type>::Tolerance())
    {
        // The two "result" objects are initialized with the "whatever" input.
        // This is so that if "_appliedToType" is a dynamic vector or matrix, then
        // memory will be allocated for the results.
        // Likewise, the Assign() method is used instead of operator = because
        // the left-side operand may be dynamic, while the right side is always
        // fixed size, and there may not be an immediate operator = between them.
        _appliedToType result1(whatever), result2(whatever);

        TestApplyMethodsNoReturnValue(rotation, whatever, tolerance);

        rotation.ApplyTo(whatever, result2);
        result1.Assign( rotation.ApplyTo(whatever) );
        CPPUNIT_ASSERT(result2.AlmostEqual(result1, tolerance));
        result2.Assign( rotation * whatever );
        CPPUNIT_ASSERT(result2.AlmostEqual(result1, tolerance));

        rotation.ApplyInverseTo(whatever, result1);
        result2.Assign( rotation.ApplyInverseTo(whatever) );
        CPPUNIT_ASSERT(result2.AlmostEqual(result1, tolerance));
    }

    template <class _rotationType, class _appliedToType>
    static void TestApplyMethodsNoReturnValue(const _rotationType & rotation,
                                              const _appliedToType & whatever,
                                              typename _rotationType::value_type tolerance
                                              = cmnTypeTraits<typename _rotationType::value_type>::Tolerance())
    {
        // The two "result" objects are initialized with the "whatever" input.
        // This is so that if "_appliedToType" is a dynamic vector or matrix, then
        // memory will be allocated for the results.
        // Likewise, the Assign() method is used instead of operator = because
        // the left-side operand may be dynamic, while the right side is always
        // fixed size, and there may not be an immediate operator = between them.
        _appliedToType result1(whatever), result2(whatever);

        rotation.ApplyTo(whatever, result1);
        rotation.ApplyInverseTo(result1, result2);
        CPPUNIT_ASSERT( result2.AlmostEqual(whatever, tolerance) );
    }


};


#endif  // _vctGenericRotationTest_h
