/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFrame4x4Base.h 75 2009-02-24 16:47:20Z adeguet1 $

  Author(s):	Anton Deguet
  Created on:	2007-09-13

  (C) Copyright 2007-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*! 
  \file 
  \brief Declaration of vctFrame4x4Base
 */


#ifndef _vctFrame4x4Base_h
#define _vctFrame4x4Base_h

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


    template <class _rotationType, class _translationType>
    ThisType & From(const _rotationType & rotation,
                    const _translationType & translation)
    {
        this->RotationRef.From(rotation);
        this->TranslationRef.Assign(translation);
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

