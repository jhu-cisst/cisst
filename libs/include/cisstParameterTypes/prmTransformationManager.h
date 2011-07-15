/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Rajesh Kumar, Anton Deguet
  Created on:	2008-03-03

  (C) Copyright 2003-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

/*!
  \file
  \brief A singleton tree of transformations, and support classes.
*/


// transformation base and derived classes
#include <cisstParameterTypes/prmTransformationBase.h>
#include <cisstParameterTypes/prmTransformationFixed.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>

/*!
  This class is intended to support computation of relationships between different coordinate systems in a CIS system.  The tree consists of nodes (various coordinate systems )
  and the edges (static or dynamic transformations between the connected coordinate systems). Transformations between coordinate systems are  retrieved using a user provided command.
  \sa mtsCommandRead.

  Examples:
  <code>
  
  // setup joints transformation
  vctFrm3 setupPos;
  setupPos.Translation() = vct3(10.0, 4.0, 3.0);
  prmTransformationFixed setupFrame("Setup frame", setupPos);
  
  // robot transformation
  prmTransformationDynamic robotFrame("Robot frame", setupFrame, robot.GetCommandRead("GetTipPosition"));
  
  // add a camera defined in world frame
  vctFrm3 cameraPos;
  cameraPos.Translation() = vct3(-10.0, 3.4, 5.34);
  vctRandom(cameraPos.Rotation()); // artificial move!
  prmTransformationFixed cameraFrame("Camera frame", cameraPos);

  // Get the position of the camera wrt to the robot
  std::cout << prmWRTReference(cameraFrame, robotFrame) << std::endl;

  // Compute the robot motion wrt its base so that the tip
  // moves in the x direction of the camera.
  vctFrm3 moveWRTCamera;
  PositionCartesianArg goalArgs;

  move.Translation() = vct3(1.0, 0.0, 0.0);
  vctFrm3 moveWRTRobot = prmWRTReference(cameraFrame, setupFrame) * move; 
  goalArgs.SetGoal(moveWRTRobot); 
  myRobot.MovePositionCart(goalArgs);

  </code>

  \note There can only be one transformation manager per application,
  hence this class is implemented as a singleton.

*/


#ifndef _prmTransformationManager_h
#define _prmTransformationManager_h


class CISST_EXPORT prmTransformationManager: cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
    friend class prmTransformationBase;

public:
    typedef std::list<prmTransformationBasePtr> NodeListType;

protected:
    /*! List  of modes on the last transformation computed. */
	static NodeListType Path;
    /*! Tree traversal helper */
	static NodeListType VisitedNodes;
	static NodeListType VisitedNodes2;
	   
    /*! Add the frame to the tree in the transformation manager.  This
	  method is private and can only be accessed by the "friend" class
	  prmTransformationBase.
	  \param attachPoint prmTransformationBasePtr pointer to the new frame
	  \param newFrame prmTransformationBasePtr pointer to parent in the tree
	  \return bool success/failure boolean status.
	*/
    static bool Attach(const prmTransformationBasePtr & attachPoint, const prmTransformationBasePtr & newFrame);
    static bool Attach(const std::string & parentName, const prmTransformationBasePtr & newFrame);
    
    /*! Remove the frame from the tree 
	  Internally calls ReplaceReference(frame, theWorld). 
	  \param frame prmTransformationBase* pointer to the frame to be discarded.
	  \return bool success/failure code (When do we define status codes?)
	*/
    static bool Detach(const prmTransformationBasePtr & frame);
    
public:
       
	
    /*! Frame used as reference for all other frames, root of the tree. 
	 All other frames are attached/detached by application code.
	*/
    static prmTransformationFixed TheWorld;

    /*! A method to clean up a transformation tree. Deletes all nodes and restores clean state.
     This should never need to be used as every node should attach or detach itself. 
     It is primarily meant for testing and debugging purposes.
     */
    static void Clear();

	/*! Test if attaching a reference frame at a particular node will
      introduce a circular dependency.  The implementation is a simple
      depth first search, that returns true if a node is revisited
      more than once.
	  \param  attachPoint prmTransformationBasePr new parent
	  \param  newFrame    prmTransformationBasePtr candidate frame
	  \return bool true if a newFrame node is already present in the tree AND is an ancestor of the attachPoint.
	*/
   static bool NodeCreatesCycle(const prmTransformationBasePtr & attachPoint, const prmTransformationBasePtr & newFrame);
   static bool NodeCreatesCycle(const std::string & pName, const prmTransformationBasePtr & newFrame);

    /*! Compute the transformation between two frames (WRT =
     With Respect To).  The computation will use registered get
     mtsCommandRead methods for the nodes on the path from node
     refFrame to node tipFrame.  The implementation is a breadth first
     search from refFrame to tipFrame.
     \param tipFrame prmTransformationBasePtr the frame transform to be computed
	 \param refFrame prmTransformationBasePtr the reference frame chosen
	 \return vctFrame3 homogenous transform representing current relationship between tipFrame and refFrame.
	*/
    static vctFrm3 WRTReference(const prmTransformationBasePtr & tipFrame, const prmTransformationBasePtr & refFrame);
    
    /*! Update all frames using a given frame as reference.  
	
	  May be useful before a call to prmTransformationManager::Detach
	  or its equivalent prmTransformationBase::~prmTransformationBase
	  \param currentReference prmTransformationBase* old Reference frame
	  \param newReference prmTransformationBase*  new Reference frame
      \return number of frames modified.
    */
    static unsigned int ReplaceReference(const prmTransformationBasePtr & nodePtr, const prmTransformationBasePtr & newReference);
    static unsigned int ReplaceReference(const prmTransformationBasePtr & nodePtr, const std::string & parentName);

    /*! Print the tree to a graphviz dot file
      
      If you have graphviz then "dot -Tjpg -o tree.jpg tree.dot" where
      tree.dot is the output of this function will print a tree
      representation of the transformation tree.
      \param outputStream std::ostream stream to dump dot file
      \return void
      */
	static void ToStreamDot(std::ostream & outputStream);
    /*! Helper function for printing the tree to a graphviz dot file      
      \return void
      */
    static void ToStreamDotHelper(std::ostream & outputStream, prmTransformationBasePtr parent);

    /*! Helper function for computing transformations. Traverses the
      tree and stores the path between target and reference in path
      \return bool true if path exists
      */
	static bool FindPath(const prmTransformationBasePtr & target, const prmTransformationBasePtr & reference);

    /*! Helper function for computing transformations. In contrast to above, this assumes a path exists and only 
      merges two traversals from target/reference to tree root
      \return bool true if path exists
      */
    static bool FindPathConnectedTree(const prmTransformationBasePtr & target, const prmTransformationBasePtr & reference);

    /*! Helper function for attaching with names
    */
    static prmTransformationBasePtr GetTransformationNodePtr(const std::string pName);

    /*! Print the path for last reference computation to a graphviz dot file
      \return void
    */
    //return path and take path here. If you throw away the path, then its lost.
    //make this thread safe
    //check ofri's old code to see if any other ideas were missed.
	static void PathToStreamDot(std::ostream & outputStream);

}; /* prmTransformationManager */

CMN_DECLARE_SERVICES_INSTANTIATION(prmTransformationManager);

/////////////////////////////////////////////////////////////////////////////////////

/*! Convenience function for computation of the current transformation between two frames using the  frame manager.  
\param tipFrame const prmTransformationBase& reference to end of the frame manager branch
\param refFrame const prmTransformationBase& reference to start of the frame manager branch
\return vctFrm3 homogeneous transform value of the Cartesian relationship between tip and ref frames.
*/
inline vctFrm3 prmWRTReference(const prmTransformationBasePtr & tipFrame, const prmTransformationBasePtr & refFrame) 
{
    return prmTransformationManager::WRTReference(tipFrame, refFrame);
}

/*! Convenience function for computation of the current transformation between a frame and the world
  coordinate system. 
  \param frame const prmTransformationBase& reference to target frame
  \return vctFrm3 homogeneous transform value of the Cartesian relationship between frame and theWorld.
*/
inline vctFrm3 prmWRTWorld(const prmTransformationBasePtr & frame) 
{
    return prmTransformationManager::WRTReference(frame, &prmTransformationManager::TheWorld);
}

/*! Convenience function allowing string names to be specified instead of pointers.
  \param tipFrame const std::string & tip frame name
  \param refFrame const std::string & reference frame name
  \return vctFrm3 homogeneous transform value of the Cartesian relationship between frame and theWorld.
*/
inline vctFrm3 prmWRTReference(const std::string & tipFrame, const std::string & refFrame)
{
    prmTransformationBasePtr tf1 = prmTransformationManager::GetTransformationNodePtr(tipFrame) ;
    prmTransformationBasePtr rf1 = prmTransformationManager::GetTransformationNodePtr(refFrame) ;
    return prmTransformationManager::WRTReference(tf1, rf1);
}

/*! Convenience function allowing tip string name to be specified instead of pointers.
  \param tipFrame const std::string & tip frame name
  \param refFrame const prmTransformationBase& reference to start of the frame manager branch
  \return vctFrm3 homogeneous transform value of the Cartesian relationship between frame and theWorld.
*/

inline vctFrm3 prmWRTReference(const std::string & tipFrame, const prmTransformationBasePtr & refFrame)
{
    prmTransformationBasePtr tf1 = prmTransformationManager::GetTransformationNodePtr(tipFrame) ;
    return prmTransformationManager::WRTReference(tf1, refFrame);
}
/*! Convenience function allowing tip string name to be specified instead of pointers.
  \param tipFrame const prmTransformationBase& reference to end of the frame manager branch
  \param refFrame const std::string & reference frame name
  \return vctFrm3 homogeneous transform value of the Cartesian relationship between frame and theWorld.
*/
inline vctFrm3 prmWRTReference(const prmTransformationBasePtr & tipFrame, const std::string & refFrame)
{
    prmTransformationBasePtr rf1 = prmTransformationManager::GetTransformationNodePtr(refFrame) ;
    return prmTransformationManager::WRTReference(tipFrame, rf1);
}

#endif // _prmTransformationManager_h

