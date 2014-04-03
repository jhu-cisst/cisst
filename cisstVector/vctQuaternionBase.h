/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2005-08-18

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctQuaternionBase_h
#define _vctQuaternionBase_h

/*!
  \file
  \brief Declaration of vctQuaternionBase
 */

#include <cisstCommon/cmnTypeTraits.h>
#include <cisstVector/vctContainerTraits.h>
#include <cisstVector/vctDynamicVector.h>
// always the last file to include
#include <cisstVector/vctExport.h>


/*!
  \brief Define a quaternion container.

  It is important to note that the class vctQuaternion is for
  any quaternion, i.e. it does not necessary represent a unit
  quaternion.

  \param _baseType The base class used to contain the 4 elements.

  \sa vctFixedSizeVector, vctDynamicVector
*/
template <class _containerType>
class vctQuaternionBase: public _containerType
{
protected:
    /*!  Allocate memory for the underlying container if needed.  By
      default, this methods does nothing.  For any container requiring
      a memory allocation, it is necessary to specialize this
      method. */
    inline void Allocate(void) {}

 public:
    enum {SIZE = 4};
    typedef _containerType BaseType;
    typedef _containerType ContainerType;
    typedef vctQuaternionBase<_containerType> ThisType;

    /* no need to document, inherit doxygen documentation from base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(typename ContainerType::value_type);
    typedef cmnTypeTraits<value_type> TypeTraits;


    /*! Default constructor.  Does nothing. */
    inline vctQuaternionBase(void)
    {
        this->Allocate();
    }


    /*! Constructor from 4 elements.

      \param x The first imaginary component
      \param y The second imaginary component
      \param z The third imaginary component
      \param r The real component
    */
    inline vctQuaternionBase(const value_type & x,
                             const value_type & y,
                             const value_type & z,
                             const value_type & r)
    {
        this->Allocate();
        this->X() = x;
        this->Y() = y;
        this->Z() = z;
        this->R() = r;
    }


    /* Methods X, Y, Z are inherited, add methods R() */

    /*! Returns the last element of the quaternion, i.e. the real
      part.  This method is const.  The methods X(), Y() and Z() are
      inherited from the base class. */
    const_reference R(void) const {
        return *(this->Pointer(3));
    }

    /*! Access the last element of the quaternion, i.e. the real part.
      This method is not const. The methods X(), Y() and Z() are
      inherited from the base class. */
    reference R(void) {
        return *(this->Pointer(3));
    }


    /*! Sets this quaternion as the conjugate of another one.
      \param otherQuaternion Quaternion used to compute the conjugate. */
    template <class __containerTypeOther>
    inline ThisType & ConjugateOf(const vctQuaternionBase<__containerTypeOther> & otherQuaternion) {
        this->X() = - otherQuaternion.X();
        this->Y() = - otherQuaternion.Y();
        this->Z() = - otherQuaternion.Z();
        this->R() = otherQuaternion.R();
        return *this;
    }


    /*! Replaces this quaternion by its conjugate. */
    inline ThisType & ConjugateSelf(void) {
        this->X() = - this->X();
        this->Y() = - this->Y();
        this->Z() = - this->Z();
        return *this;
    }


    /*! Returns the conjugate of this quaternion.  This method returns
      a copy of the conjugate and does not modify this quaternion. */
    inline ThisType Conjugate(void) const {
        ThisType result;
        result.ConjugateOf(*this);
        return result;
    }


    /*! Set this quaternion as the product of two other ones.
      \param quat1 Left operand
      \param quat2 Right operand
      \note Quaternion product in non-commutative.
    */
    template <class __containerType1, class __containerType2>
    inline ThisType & ProductOf(const vctQuaternionBase<__containerType1> & quat1,
                                const vctQuaternionBase<__containerType2> & quat2) {
        this->X() = quat1.R() * quat2.X() +  quat1.X() * quat2.R() +  quat1.Y() * quat2.Z() -  quat1.Z() * quat2.Y();
        this->Y() = quat1.R() * quat2.Y() -  quat1.X() * quat2.Z() +  quat1.Y() * quat2.R() +  quat1.Z() * quat2.X();
        this->Z() = quat1.R() * quat2.Z() +  quat1.X() * quat2.Y() -  quat1.Y() * quat2.X() +  quat1.Z() * quat2.R();
        this->R() = quat1.R() * quat2.R() -  quat1.X() * quat2.X() -  quat1.Y() * quat2.Y() -  quat1.Z() * quat2.Z();
        return *this;
    }

    /*! Compute the quotient quat1 / quat2.  The reciprocal of
      a quaternion q is  conj(q) / norm(q) */
    template <class __containerType1, class __containerType2>
    inline ThisType & QuotientOf(const vctQuaternionBase<__containerType1> & quat1,
                                 const vctQuaternionBase<__containerType2> & quat2) {
        const value_type quat2Norm = value_type(quat2.Norm());
        BaseType quat2I;
        quat2I.Assign(-quat2.X() / quat2Norm, -quat2.Y() / quat2Norm, -quat2.Z() / quat2Norm, quat2.R() / quat2Norm);
        this->X() = quat1.R() * quat2I.X() +  quat1.X() * quat2.R() +  quat1.Y() * quat2I.Z() -  quat1.Z() * quat2I.Y();
        this->Y() = quat1.R() * quat2I.Y() -  quat1.X() * quat2I.Z() +  quat1.Y() * quat2.R() +  quat1.Z() * quat2I.X();
        this->Z() = quat1.R() * quat2I.Z() +  quat1.X() * quat2I.Y() -  quat1.Y() * quat2I.X() +  quat1.Z() * quat2.R();
        this->R() = quat1.R() * quat2.R() -  quat1.X() * quat2I.X() -  quat1.Y() * quat2I.Y() -  quat1.Z() * quat2I.Z();
        return *this;
    }

    /*! Post-multiply this quaternion by another, i.e., this = this * other.
      \note Quaternion product is non-commutative.
    */
    template <class __containerTypeOther>
    inline ThisType & PostMultiply(const vctQuaternionBase<__containerTypeOther> & other) {
        ThisType result;
        result = ProductOf(*this, other);
        *this = result;
        return *this;
    }

    /*! Pre-multiply this quaternion by another, i.e., this = other * this.
      \note Quaternion product is non-commutative.
    */
    template <class __containerTypeOther>
    inline ThisType & PreMultiply(const vctQuaternionBase<__containerTypeOther> & other) {
        ThisType result;
        result = ProductOf(other, *this);
        *this = result;
        return *this;
    }

    /*! Divide this quaternion by another, i.e., this = this / other. */
    template <class __containerTypeOther>
    inline ThisType & Divide(const vctQuaternionBase<__containerTypeOther> & other) {
        ThisType result;
        result.QuotientOf(*this, other);
        *this = result;
        return *this;
    }

    /*! Divide this quaternion by a scalar: equal to elementwise division.
      The method re-implements vector elementwise division, which is otherwise
      shadowed by the Divide(ThisType) method.
    */
    inline ThisType & Divide(const value_type s) {
        (static_cast<BaseType *>(this))->Divide(s);
        return *this;
    }

};


/* Specialization of Allocate for a dynamic matrix */
template<>
inline void vctQuaternionBase<vctDynamicVector<double> >::Allocate(void)
{
    this->SetSize(SIZE);
}

template<>
inline void vctQuaternionBase<vctDynamicVector<float> >::Allocate(void)
{
    this->SetSize(SIZE);
}



/*! Product of two quaternions.  This operator relies on the method
  vctQuaternion::ProductOf.

  \param quat1 Left operand
  \param quat2 Right operand
*/
template <class _containerType>
inline vctQuaternionBase<_containerType>
operator * (const vctQuaternionBase<_containerType> & quat1,
            const vctQuaternionBase<_containerType> & quat2) {
    vctQuaternionBase<_containerType> result;
    result.ProductOf(quat1, quat2);
    return result;
}


/*! Quotient of two quaternions.  This operator relies on the method
  vctQuaternion::QuotientOf.

  \param quat1 Left operand
  \param quat2 Right operand
*/
template <class _containerType>
inline vctQuaternionBase<_containerType>
operator / (const vctQuaternionBase<_containerType> & quat1,
            const vctQuaternionBase<_containerType> & quat2) {
    vctQuaternionBase<_containerType> result;
    result.QuotientOf(quat1, quat2);
    return result;
}


#endif  // _vctQuaternionBase_h

