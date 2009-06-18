/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
Author(s):	Marcin Balicki
Created on:   2009-06-12

(C) Copyright 2008 Johns Hopkins University (JHU), All Rights
Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*! 
\file 
\brief generally we think of a masked element as an valid/selected/active one.
*/


#ifndef _prmMaskedDoubleVec_h
#define _prmMaskedDoubleVec_h

//basic includes
#include <cisstParameterTypes/prmTypes.h>
#include <cisstParameterTypes/prmMotionBase.h>
#include <cisstParameterTypes/prmExport.h>
#include <cisstMultiTask/mtsVector.h>

/*! motion command arguments for actuator angle based motion
Note: Get methods return a reference so that the objects can be directly accessed.
Be careful to have the same size for mask and value
*/
class CISST_EXPORT prmMaskedDoubleVec: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
protected:

    typedef unsigned int size_type;
    typedef prmMaskedDoubleVec ThisType;

public:

    /*! default constructor - does nothing for now */
    inline prmMaskedDoubleVec(){ }

    /*! use this if all the vectors are the same size*/
    prmMaskedDoubleVec (size_type size);

    /*! Set vector same size for each element, this is required if the above constructor is not used*/ 
    void SetSize(size_type size);

    /*! constructor with all possible parameters */
    prmMaskedDoubleVec(const mtsBoolVec& mask,
                   const mtsDoubleVec& value ):
        MaskMember(mask),
        ValueMember(value)
    {}

    /*! destructor */
    virtual ~prmMaskedDoubleVec();


    /*! Masks for the cooresponding vector. */
    //@{
    MTS_DECLARE_MEMBER_AND_ACCESSORS(mtsBoolVec, Mask);
    //@}

    /*! Vector of values for this container*/
    //@{
    MTS_DECLARE_MEMBER_AND_ACCESSORS(mtsDoubleVec, Value);
    //@}

    /*! Allocate memory based on an existing object of the same type.  The
    object is provided via a mtsGenericObject pointer.  If a dynamic cast
    to this type is not possible, this method returns false. */
    bool inline Allocate(const mtsGenericObject * model)
    {
        const ThisType * pointer = dynamic_cast<const ThisType *>(model);
        if (pointer == 0) {
            return false;
        }
        this->MaskMember.SetSize(pointer->Mask().size());
        this->ValueMember.SetSize(pointer->Value().size());
        return true;
    }


    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;

}; // _prmMaskedDoubleVec_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmMaskedDoubleVec);


#endif
