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
  \brief Support classes for transformation manager.
  \sa prmTransformationManager
*/

#ifndef _prmTransformationBase_h
#define _prmTransformationBase_h


#include <cisstCommon/cmnGenericObjectProxy.h>
#include <cisstCommon/cmnClassRegisterMacros.h>
#include <cisstVector/vctTransformationTypes.h>

#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstParameterTypes/prmForwardDeclarations.h>

// probably best to include streams
#include <iostream>
#include <list>
#include <algorithm>

// Always include last
#include <cisstParameterTypes/prmExport.h>


typedef prmTransformationBase* prmTransformationBasePtr;                 //convenience type for base class

// typedef std::list<prmTransformationBasePtr> prmTransformationNodeList;   //list of nodes in the tree


/*! 
  \brief Transformation base class.
  
  The frame base class contains all the methods required to manage the
  relations between the different frames.  Each frame corresponds to a
  node in the transformation tree and contains a pointer to its
  reference frame (edge of the tree).
  
  The derived classes prmTransformationFixed and prmTransformationDynamic implement two
  different ways to compute the transformation between this frame and
  its reference, i.e. either a static transformation or a user
  provided command.

  Please see the Frame Manager architecture discussion to see why this does not derive from a vctFrm3
  
  \sa prmTransformationManager, prmTransformationFixed, prmTransformationDynamic, vctFrm3
*/
class CISST_EXPORT prmTransformationBase
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
    friend class prmTransformationManager;

public:
    typedef std::list<prmTransformationBasePtr> NodeListType;

private:
    /*! tree traversal helper 
    */
    bool Visited;
	
    /*! Is there a direct parent/child between me an him? 
	\param him prmTransformationBasePtr pointer to him
	\return bool true/false;
	*/
    bool IsLinked(const prmTransformationBasePtr & him) const; 

	/*! Is him one of my children? 
	\param him prmTransformationBasePtr pointer to him
	\return bool true/false;
	*/
    bool IsChildOf(const prmTransformationBasePtr & him) const;
	
    /*! Is him my immediate parent? 
	\param him prmTransformationBasePtr pointer to him
	\return bool true/false;
	*/
	bool IsParentOf(const prmTransformationBasePtr & him) const;

protected:
	
    /*! a describable name */
    std::string Name;
	
    /*! a reference to its parent and immediate reference frame, obtained during prmTransformationManager::Attach */
    prmTransformationBasePtr Parent;
	
    /*! frames that attach and refer to this frame */
    NodeListType Children;

public:

    /*! Default constructor.  Set the frame name as "Undefined" and
      uses the world frame as reference.  The frame is automatically
      added to the frame manager. 
    */
    inline prmTransformationBase(void):
        Name("Undefined"),
        Parent(NULL),
        Children()  
    {
		/* RK: No longer attaches itsself to the world by default, parent is NULL, until reference frame is set.	   
		*/
    }
    
    /*! Constructor with a name only.  */
    inline prmTransformationBase(const std::string & name):
        Name(name),
		Parent(NULL),
		Children()
    {
		/* RK: No longer attaches itsself to the world by default, parent is NULL until reference frame is set.	   
		*/
    }
    
    /*! \brief Destructor.  The frame is also removed from the frame manager.
      
	  Any frame using this frame as a reference will be modified to use its pparent as reference. 
	  If you need to change the reference frame of all depending frames, use  prmTransformationManager::ReplaceReference. 
	*/
    virtual ~prmTransformationBase();

	prmTransformationBasePtr WorldFrame(void);
    
    /*! Set the reference frame.  This method uses the prmTransformationManager
      to make sure there is no cyclic dependency.  If a cycle is
      found, returns false and doesn't update the reference
      frame. 
	  \param newReference prmTransformationBasePtr ptr to new location in the tree
	  \return bool success/failure code.
	  */
    bool SetReferenceFrame(const prmTransformationBasePtr & newReference); 
    bool SetReferenceFrame(const std::string & parentName); 
	
    /*! Get a pointer to the reference frame. */
    inline prmTransformationBasePtr GetReferenceFrame(void) const 
	{
        return Parent;
    }
    
    /*! Set/Get name */
    //@{
    inline const std::string & GetName(void) const 
	{
        return this->Name;
    }
    
	inline void SetName(const std::string & name) 
	{
        this->Name = name;
    }
    //@}
    
    /*! Get the transformation between the reference frame and this frame. */
    virtual vctFrm3 WRTReference(void) const = 0;

}; //prmTransformationBase


CMN_DECLARE_SERVICES_INSTANTIATION(prmTransformationBase);


#endif // _prmTransformationBase_h

