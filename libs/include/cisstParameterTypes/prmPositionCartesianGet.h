/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):	Rajesh Kumar, Anton Deguet
  Created on:	2008-03-12

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
  \brief Cartesian position get parameters.
*/


#ifndef _prmPositionCartesianGet_h
#define _prmPositionCartesianGet_h

#include <cisstParameterTypes/prmTransformationBase.h>
#include <cisstParameterTypes/prmTransformationManager.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>

/*! Cartesian position get command argument */
class CISST_EXPORT prmPositionCartesianGet: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 public:
    /*! Base type */
    typedef mtsGenericObject BaseType;

    /*! default constructor */
    inline prmPositionCartesianGet(void):
        BaseType(),
        MovingFrameMember(0),
        ReferenceFrameMember(0),
        PositionMember()
    {}
    
    /*! constructor with all parameters */
    inline prmPositionCartesianGet(const prmTransformationBasePtr & movingFrame, 
                                   const prmTransformationBasePtr & referenceFrame, 
                                   const vctFrm3 & position):
        MovingFrameMember(movingFrame),
        ReferenceFrameMember(referenceFrame),
        PositionMember(position)
    {}
    
    /*!destructor
     */
    virtual ~prmPositionCartesianGet();
    

    /*! Set and Get methods for the reference frame for current
        position.  This is defined by a node in the transformation
        tree. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(prmTransformationBasePtr, MovingFrame);
    //@}


    /*! Set and Get methods for the moving frame for current
        position.  This is defined by a node in the transformation
        tree. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(prmTransformationBasePtr, ReferenceFrame);
    //@}


    /*! Set and Get methods for position */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctFrm3, Position);
    //@}

 public:

    /*! Set moving to a node in the transformation tree.  The actual
      position is computed with respect to the reference frame carried
      by this object.
      \param newPosition node in the tree
    */
    inline void SetPosition(const prmTransformationBasePtr & newPosition)
    {
        this->PositionMember = prmWRTReference(newPosition, this->ReferenceFrameMember);
    } 

    
    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;

    /*! To stream raw data. */
    void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                     bool headerOnly = false, const std::string & headerPrefix = "") const;

    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const;

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream);

    /*! Return a number of data (which can be visualized, i.e., type-casted 
        to double). */
    unsigned int GetNumberOfScalar(const bool visualizable = true) const;

    /*! Return the index-th (zero-based) value of data typecasted to double. */
    double GetScalarAsDouble(const size_t index) const;

    /*! Return the name of index-th (zero-based) data typecasted to double. */
    std::string GetScalarName(const size_t index) const;

}; // _prmPositionCartesianGet_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmPositionCartesianGet);


#endif // _prmPositionCartesianGet_h

