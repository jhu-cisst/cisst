/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2007-09-13

  (C) Copyright 2007-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctFrame4x4Base_h
#define _vctFrame4x4Base_h

/*!
  \file
  \brief Declaration of vctFrame4x4Base
 */

#include <cisstVector/vctFrame4x4ConstBase.h>
#include <cisstVector/vctExport.h>

/*!
  \brief Template base class for a 4x4 frame.

  This class allows to use a 4 by 4 matrix as a frame in 3D.  This is
  a limited case of the so called homegenous transformations as this
  class is intended to support only the translation and rotation parts
  of the transformation.  It is not intended to support perspective or
  scaling operations.  The different constructors and normalization
  methods provided will set the last row to [0 0 0 1].

  \param _matrixType The type of matrix used to store the elements

  \sa vctDynamicMatrix, vctFixedSizeMatrix, vctFrameBase
*/
template<class _containerType>
class vctFrame4x4Base:
    public vctFrame4x4ConstBase<_containerType>
{
public:
    enum {ROWS = 4, COLS = 4};
    enum {DIMENSION = 3};
    typedef _containerType ContainerType;
    typedef vctFrame4x4ConstBase<ContainerType> BaseType;
    typedef vctFrame4x4Base<ContainerType> ThisType;

    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(typename ContainerType::value_type);

    typedef typename BaseType::RowRefType RowRefType;
    typedef typename BaseType::ColumnRefType ColumnRefType;
    typedef typename BaseType::ConstRowRefType ConstRowRefType;
    typedef typename BaseType::ConstColumnRefType ConstColumnRefType;
    typedef typename BaseType::DiagonalRefType DiagonalRefType;
    typedef typename BaseType::ConstDiagonalRefType ConstDiagonalRefType;
    typedef typename BaseType::RefTransposeType RefTransposeType;
    typedef typename BaseType::ConstRefTransposeType ConstRefTransposeType;
    typedef typename BaseType::MatrixValueType MatrixValueType;

    typedef typename BaseType::TranslationRefType TranslationRefType;
    typedef typename BaseType::ConstTranslationRefType ConstTranslationRefType;
    typedef typename BaseType::PerspectiveRefType PerspectiveRefType;
    typedef typename BaseType::ConstPerspectiveRefType ConstPerspectiveRefType;
    typedef typename BaseType::RotationRefType RotationRefType;
    typedef typename BaseType::ConstRotationRefType ConstRotationRefType;

    typedef cmnTypeTraits<value_type> TypeTraits;

public:


    /*! Default constructor. Sets the rotation matrix to identity. */
    vctFrame4x4Base(void):
        BaseType()
    {}


    inline ConstTranslationRefType Translation(void) const {
        return this->TranslationRef;
    }

    inline ConstRotationRefType Rotation(void) const {
        return this->RotationRef;
    }

    inline ConstPerspectiveRefType Perspective(void) const {
        return this->PerspectiveRef;
    }

    inline TranslationRefType Translation(void) {
        return this->TranslationRef;
    }

    inline RotationRefType Rotation(void) {
        return this->RotationRef;
    }

    inline PerspectiveRefType Perspective(void) {
        return this->PerspectiveRef;
    }


    /*! Create from a rotation and a translation.  From will throw an
      exception if the rotation is not normalized.  FromNormalized
      will ensure the rotation part is normalized after assignment
      while FromRaw will not check nor normalize the rotation
      component. */
    //@{
    template <class __rotationType, class __translationType>
    ThisType & From(const __rotationType & rotation,
                    const __translationType & translation)
        CISST_THROW(std::runtime_error)
    {
        this->RotationRef.From(rotation);
        this->TranslationRef.Assign(translation);
        this->PerspectiveRef.SetAll(static_cast<value_type>(0.0));
        this->Element(DIMENSION, DIMENSION) = static_cast<value_type>(1.0);
        return *this;
    }

    template <class __rotationType, class __translationType>
    ThisType & FromNormalized(const __rotationType & rotation,
                              const __translationType & translation)
    {
        this->RotationRef.FromNormalized(rotation);
        this->TranslationRef.Assign(translation);
        this->PerspectiveRef.SetAll(static_cast<value_type>(0.0));
        this->Element(DIMENSION, DIMENSION) = static_cast<value_type>(1.0);
        return *this;
    }

    template <class __rotationType, class __translationType>
    ThisType & FromRaw(const __rotationType & rotation,
                       const __translationType & translation)
    {
        this->RotationRef.FromRaw(rotation);
        this->TranslationRef.Assign(translation);
        this->PerspectiveRef.SetAll(static_cast<value_type>(0.0));
        this->Element(DIMENSION, DIMENSION) = static_cast<value_type>(1.0);
        return *this;
    }
    //@}


    /*! Create from a frame derived from vctFrameBase
      (i.e. represented by a rotation and translation).  From will
      throw an exception if the rotation is not normalized.
      FromNormalized will ensure the rotation part is normalized after
      assignment while FromRaw will not check nor normalize the
      rotation component. */
    //@{
    template <class __rotationType>
    ThisType & From(const vctFrameBase<__rotationType> & frame)
        CISST_THROW(std::runtime_error)
    {
        this->From(frame.Rotation(), frame.Translation());
        return *this;
    }

    template <class __rotationType>
    ThisType & FromNormalized(const vctFrameBase<__rotationType> & frame)
    {
        this->FromNormalized(frame.Rotation(), frame.Translation());
        return *this;
    }

    template <class __rotationType>
    ThisType & FromRaw(const vctFrameBase<__rotationType> & frame)
    {
        this->FromRaw(frame.Rotation(), frame.Translation());
        return *this;
    }
    //@}


    /*! Normalizes this frame.  This method normalizes the rotation
      part of the matrix and resets the last row to [0 0 0 1].  For
      the rotation normalization, this method relies on
      vctMatrixRotation3Base normalization method, which converts to
      a unit quaternion to normalize. */
    ThisType & NormalizedSelf(void) {
        this->Rotation().NormalizedSelf();
        this->PerspectiveRef.SetAll(static_cast<value_type>(0.0));
        this->Element(DIMENSION, DIMENSION) = static_cast<value_type>(1.0);
        return *this;
    }


    /*! Inverse this frame. */
    inline ThisType & InverseSelf(void) {
        // R -> Rinv
        this->RotationRef.InverseSelf();
        // T -> Rinv * (-T)
        vctFixedSizeVector<value_type, DIMENSION> temp;
        temp.Assign(this->TranslationRef);
        temp.NegationSelf();
        this->RotationRef.ApplyTo(temp, this->TranslationRef);
        return *this;
    }


    template <class __containerType>
    inline ThisType & InverseOf(const vctFrame4x4ConstBase<__containerType> & otherFrame) {
        this->TranslationRef.Assign(otherFrame.Translation());
        this->RotationRef.Assign(otherFrame.Rotation());
        this->InverseSelf();
        return *this;
    }

};


#endif  // _vctFrame4x4Base_h
