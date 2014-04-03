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


#include <cisstParameterTypes/prmTransformationFixed.h>
#include <cisstParameterTypes/prmTransformationManager.h>


/*! \brief Destructor.  The frame is also removed from the frame manager.
      
  Any frame using this frame as a reference will be modified to use its pparent as reference. 
  If you need to change the reference frame of all depending frames, use  prmTransformationManager::ReplaceReference. 
*/
prmTransformationBase::~prmTransformationBase()
{
    prmTransformationManager::Detach(this);
}

/*! Helper function: return the world reference frame, root of the tree.
 */

prmTransformationBasePtr prmTransformationBase::WorldFrame(void)
{
	return &prmTransformationManager::TheWorld;
}

/*! check for immediate links between me and him
  \param him prmTransformationBasePtr node to check
  \return bool true/false
*/
bool prmTransformationBase::IsLinked(const prmTransformationBasePtr & him) const
{
	return (this->IsChildOf(him) || this->IsParentOf(him));
};

/*! Is him one of my immediate children? 
  \param him prmTransformationBasePtr pointer to him
  \return bool true/false;
*/
bool prmTransformationBase::IsParentOf(const prmTransformationBasePtr & him) const
{
	NodeListType::const_iterator iter = find(this->Children.begin(), this->Children.end(), him);
	return (iter != this->Children.end());
}

/*! Is him my immediate parent? 
  \param him prmTransformationBasePtr pointer to him
  \return bool true/false;
*/
bool prmTransformationBase::IsChildOf(const prmTransformationBasePtr & him) const
{
	//is him my parent?
	return (him == this->Parent);
}

/*! Set the reference frame.  This method uses the prmTransformationManager
  to make sure there is no cyclic dependency.  If a cycle is
  found, returns false and doesn't update the reference
  frame. 
  \param newReference prmTransformationBase * ptr to new location in the tree
  \return bool success/failure code.
*/
bool prmTransformationBase::SetReferenceFrame(const prmTransformationBasePtr & newReference) 
{
    return prmTransformationManager::Attach(newReference, this);
}

bool prmTransformationBase::SetReferenceFrame(const std::string &parentName) 
{
    return prmTransformationManager::Attach(parentName, this);
}

