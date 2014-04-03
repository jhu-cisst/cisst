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
  \brief Declaration of prmTransformationDynamic
*/

#ifndef _prmTransformationDynamic_h
#define _prmTransformationDynamic_h

#include <cisstParameterTypes/prmTransformationBase.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>


/*! Derived class of prmTransformationBase for a dynamic
  transformation.  This class stores an command which gets invoked
  whenever the position of the frame with respect to its reference is
  queried.
  
  \sa prmWRTReference prmTransformationFixed, prmTransformationBase,
  prmTransformationManager
  */
class CISST_EXPORT prmTransformationDynamic: public prmTransformationBase
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
    friend class prmTransformationManager;

 public:
    typedef mtsFunctionRead CommandType;

 protected:
	/*! command to compute current value  with respect to its reference 
     */
    CommandType TransformationCommand;
    
 public:
    /*! Default constructor.  Set the frame name as "Undefined". 
	  The transformation command between this frame and its reference is set to NULL, which would return an identity transform if queried. 
	*/
    inline prmTransformationDynamic(void):
        prmTransformationBase(),
        TransformationCommand()
    {
	}
    
    /*! Constructor with a name only. The transformation between this
      frame and its reference is not defined and returns identity. 
	  \param name const std::string describable name
	 */
    inline prmTransformationDynamic(const std::string & name):
        prmTransformationBase(name),
        TransformationCommand()
    {
	}
    
    /*! Constructor with a name, a reference frame and the
      transformation command from the reference frame.  
	  \param name const std::string describable name
	  \param transformationCommdn const mtsCommandRead* ptr to new transformation command
	  \param reference prmTransformationBasePtr ptr to attachment point in the transformation manager.
	  */
    inline prmTransformationDynamic(const std::string & name,
                                    const CommandType & transformationCommand,
                                    prmTransformationBasePtr reference):
        prmTransformationBase(name),
        TransformationCommand(transformationCommand) 
    {
        this->SetReferenceFrame(reference);
    }

    /*! Destructor.  The frame is also detached from the frame manager.
      Any children of the detached frame will inherit its parent 
	 */
    ~prmTransformationDynamic(); 

    /*! Set/Get transformation command between this frame and its
        reference.
		\param transformationCommand const mtsCommandRead* ptr to new transformation computation method
		\return void.
	*/
    //@{
    inline void SetTransformationCommand(const CommandType & transformationCommand) 
	{
        this->TransformationCommand = transformationCommand;
    }
    
	/*! Get a pointer to the current transformation command method
	 \return mtsCommandRead* ptr to current transformation command
	 */
	inline const CommandType & GetTransformationCommand(void) const 
	{
        return this->TransformationCommand;
    }
    //@}
    
    /*! Get the transformation between the reference frame and this
      frame.  Same as GetTransformation()->Execute().
	  \return vctFrm3 relationship between this frame and its reference frame in the transformation manager.
	 */
    inline vctFrm3 WRTReference(void) const 
	{
        prmPositionCartesianGet result;  //identity transform by default
	    this->TransformationCommand(result);
        return result.Position();
    }

}; // prmTransformationDynamic


CMN_DECLARE_SERVICES_INSTANTIATION(prmTransformationDynamic);


#endif // _prmTransformationDynamic_h

