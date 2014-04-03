/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Rajesh Kumar, Anton Deguet
  Created on:   2008-03-03

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
  \brief Declaration of prmTransformationFixed
*/

#ifndef _prmTransformationFixed_h
#define _prmTransformationFixed_h

#include <cisstParameterTypes/prmTransformationBase.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>


/*! Derived Class of prmTransformationBase for a fixed rigid
  transformation.  This class stores an actual frame which can be
  manually updated if needed.

  \sa prmTransformationBase, prmTransformationDynamic, prmTransformationManager, vctFrm3
 */
class CISST_EXPORT prmTransformationFixed: public prmTransformationBase
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
    friend class prmTransformationManager;

 protected:
    /*! a homogenous transform */
    vctFrm3 Transformation; 
    
 public:
    /*! Default constructor.  Set the frame name as "Undefined", and NULL reference frame, no children.
     */
    inline prmTransformationFixed(void):
        prmTransformationBase()
    {
		/* Transformation is identity by default */
	}
    
    /*! Constructor with a name only.  The frame is automatically
      added to the frame manager.  The transformation between this
      frame and its reference is set to Identity. 
      \param name const std::string describable name
	*/
    inline prmTransformationFixed(const std::string & name):
        prmTransformationBase(name)
    {
		/* Transformation is identity by default */
	}
    
    /*! Constructor with a name, a reference frame and the
      transformation from the reference frame. 
	  \param name cost std::string describable name
	  \param newTransformation vctFrm3 const Cartesian frame Transformation
	  \newReference prmTransformationBasePtr ptr to attachment point in the transformation manager
    */
    inline prmTransformationFixed(const std::string & name,
                                  const vctFrm3 & transformation,
                                  prmTransformationBasePtr newReference):
        prmTransformationBase(name),
        Transformation(transformation)
    {
        this->SetReferenceFrame(newReference);
    }
    
    /*! Destructor.  The frame is also removed from the frame manager.
      The children of this frame will inherit its parent when the
      frame is removed.
     */
    ~prmTransformationFixed(); 

    /*! Set transformation between this frame and its reference. */
    //@{
    inline void SetTransformation(const vctFrm3 & newTransformation) 
	{
        this->Transformation.Assign(newTransformation);
    }
    /*! Query the constant transformation */
	inline vctFrm3 GetTransformation(void) const 
	{
        return this->Transformation;
    }
    //@}
    
    /*! Get the transformation between the reference frame and this
      frame.  
    */
    inline vctFrm3 WRTReference(void) const 
	{
        return this->GetTransformation();
    }

};  // prmTransformationFixed


CMN_DECLARE_SERVICES_INSTANTIATION(prmTransformationFixed);


#endif // _prmTransformationFixed_h

