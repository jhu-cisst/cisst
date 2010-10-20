/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*!
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

#include <cisstParameterTypes/prmTransformationManager.h>


//default world definition: NULL reference, identity transform.
prmTransformationFixed prmTransformationManager::TheWorld = prmTransformationFixed("The_World");

//initialization of helper and traversal members
prmTransformationManager::NodeListType prmTransformationManager::VisitedNodes;
prmTransformationManager::NodeListType prmTransformationManager::VisitedNodes2;
prmTransformationManager::NodeListType prmTransformationManager::Path;

/*! Clear the tree
*/
void prmTransformationManager::Clear()
{
	NodeListType::iterator iter;
	prmTransformationBasePtr current;

    VisitedNodes.clear();
	VisitedNodes.push_back(&TheWorld);
	while (!VisitedNodes.empty()) 
	{
		//get a node 
		current = VisitedNodes.front();
		VisitedNodes.pop_front();
			
		// examine this node
        if (current->Children.empty())
		{            
			Detach(current);
		}
        else
        {
		    for (iter = current->Children.begin();
                iter != current->Children.end();
                iter++)
		    {
		        // Look through neighbors.
		        prmTransformationBasePtr temp = *iter;
		        VisitedNodes.push_back(temp);                
		    } //for
            current->Children.clear();
        }
    }  //while

	//visited is empty anyway
	VisitedNodes.empty();
}
/*! Add the frame to the tree in the transformation manager.  
 */
bool prmTransformationManager::Attach(const prmTransformationBasePtr & attachPoint, const prmTransformationBasePtr & newFrame)
{
	//can't set reference for the world frame.
 	if (newFrame == &prmTransformationManager::TheWorld)
	{
		return false;
	}
	if (attachPoint == NULL)
	{
		/*RK: How do we usually do null-pointer error checking?
		*/
		return false;
	}
    if (prmTransformationManager::NodeCreatesCycle(attachPoint, newFrame)) 
	{
        /*RK: Need to push out a log message here */
        return false;
    }

    /* remove from old parent - or you will have cycles very soon
	*/
	if (newFrame->Parent != NULL)
	{
		/* remove from old parent's children */
		newFrame->Parent->Children.remove(newFrame);
	}

    /* set the new parent */
    newFrame->Parent = attachPoint;
	/* add me to may parent's list of children */
    attachPoint->Children.push_back(newFrame);

    /* return success */
    return true;
}

/*! Add the frame to the tree in the transformation manager.  
 */
bool prmTransformationManager::Attach(const std::string &parentName, const prmTransformationBasePtr & newFrame)
{
    prmTransformationBasePtr parent;
	//can't set reference for the world frame.
 	if (newFrame == &prmTransformationManager::TheWorld)
	{
		return false;
	}
    if (prmTransformationManager::NodeCreatesCycle(parentName, newFrame)) 
	{
        /*RK: Need to push out a log message here */
        return false;
    }

    /* remove from old parent - or you will have cycles very soon
	*/
	if (newFrame->Parent != NULL)
	{
		/* remove from old parent's children */
		newFrame->Parent->Children.remove(newFrame);
	}

    parent = GetTransformationNodePtr(parentName);
    
    if(parent==NULL) /*should never be NULL */
        return false;

    /* set the new parent*/
    newFrame->Parent = parent;
	/* add me to may parent's list of children */
    parent->Children.push_back(newFrame);

    /* return success */
    return true;
}

/*! Remove the frame from the tree 
  Internally calls ReplaceReference(frame, theWorld). 
   \param frame prmTransformationBase* pointer to the frame to be discarded.
  \return bool success/failure code (When do we define status codes?)
*/
bool prmTransformationManager::Detach(const prmTransformationBasePtr & frame)
{
	NodeListType::iterator iter;

    if (frame == NULL)
	{
		/*RK: How do we usually do null-pointer error checking?
		*/
		return false;
	}

    //can't change the world frame.
	if (frame == &prmTransformationManager::TheWorld)
	{
		return false;
	}
    
    //not in the graph
    if (frame->Parent == NULL)
    {
		return false;
    }    
    
    //reattach children to my parent
    for (iter = frame->Children.begin();
         iter != frame->Children.end();
         iter++)
    {
        (*iter)->Parent = frame->Parent;
        frame->Parent->Children.push_back(*iter);
    }
    
    //cleanup
    frame->Parent->Children.remove(frame);
    frame->Parent = NULL;
    frame->Children.clear();
    
    return true;
}

/*!  find path between two nodes - assumes a connected tree
*/
bool prmTransformationManager::FindPathConnectedTree(const prmTransformationBasePtr & target, const prmTransformationBasePtr & reference)
{
	bool found = false;

	NodeListType::iterator iter;
	prmTransformationBasePtr current;
	prmTransformationBasePtr temp;

	//map of found edges
	std::map<prmTransformationBasePtr,prmTransformationBasePtr> edges;

	if (target == NULL || reference == NULL) 
	{
		return false;
	}

	//clear the tree from the root
	VisitedNodes.clear();
	prmTransformationManager::VisitedNodes.push_back(&prmTransformationManager::TheWorld);
	while (!VisitedNodes.empty()) 
	{
		current = VisitedNodes.front();
		current->Visited = false;
		VisitedNodes.pop_front();			
		for (iter = current->Children.begin();
             iter != current->Children.end();
             iter++)
		{				
			VisitedNodes.push_back(*iter);
		}
	}

	//clean edge map
	edges[target] = NULL;

    VisitedNodes.clear();
    VisitedNodes2.clear();

	//start searching from target
    current = target;    
	current->Visited = true;    
	VisitedNodes.push_back(current);
    //path fron target to root
    while( current->Parent != NULL)	 
    {
        current = current->Parent;
        current->Visited = true;
        VisitedNodes.push_back(current);
    }

    //path from source to root
    current = reference;
	current->Visited = true;    
	VisitedNodes2.push_back(current);
    //path fron target to root
    while( current->Parent != NULL)	 
    {
        current = current->Parent;
        current->Visited = true;
        VisitedNodes2.push_back(current);
    }

     Path.clear();
    //no path IF
    // lists not empty, and front dont match
    //while more nodes to visit, and common path to root
    if((!VisitedNodes.empty())&&(!VisitedNodes2.empty())&&
        (VisitedNodes.front() != VisitedNodes2.front()))
    {
        found = false;
        return found;

    }
    else
    {
        //while more nodes to visit, and common path to root
        while((!VisitedNodes.empty())&&(!VisitedNodes2.empty())&&
            (VisitedNodes.front() == VisitedNodes2.front()))
        {
            current = VisitedNodes.front();		
	        VisitedNodes.pop_front();
            temp    = VisitedNodes2.front();
            VisitedNodes2.pop_front();
        }
    }       
    // the path is VisitedNodes2+current+VisitedNodes
    //  target = reference - lists are empty path is current
    //  target or reference = root, one list is empty
    //  otherwise both lists have one or more elements
 	//generate the path
    while(!VisitedNodes2.empty())
    {
        temp=VisitedNodes2.back();
        VisitedNodes2.pop_back();
        Path.push_back(temp);
    }
    Path.push_back(current);
    while(!VisitedNodes.empty())
    {
        temp=VisitedNodes.front();
        VisitedNodes.pop_front();
        Path.push_back(temp);
    }
    return true;
}

/*!  find path between two nodes in the tree
*/
bool prmTransformationManager::FindPath(const prmTransformationBasePtr & target, const prmTransformationBasePtr & reference)
{
	bool found = false;

	NodeListType::iterator iter;
	prmTransformationBasePtr current;
	prmTransformationBasePtr temp;

	//map of found edges
	std::map<prmTransformationBasePtr,prmTransformationBasePtr> edges;

	if (target == NULL || reference == NULL) 
	{
		return false;
	}

	//clear the tree from the root
	VisitedNodes.clear();
	prmTransformationManager::VisitedNodes.push_back(&prmTransformationManager::TheWorld);
	while (!VisitedNodes.empty()) 
	{
		current = VisitedNodes.front();
		current->Visited = false;
		VisitedNodes.pop_front();			
		for (iter = current->Children.begin();
             iter != current->Children.end();
             iter++)
		{				
			VisitedNodes.push_back(*iter);
		}
	}


	VisitedNodes.clear();
	//start searching from target
	VisitedNodes.push_back(target);
	target->Visited = true;
	
	//clean edge map
	edges[target] = NULL;

	//while more nodes to visit
	while (!VisitedNodes.empty() && !found) 
	{
		//next node up for a visit
		current = VisitedNodes.front();		
		VisitedNodes.pop_front();

		//look at all adjancent nodes
		for (iter = current->Children.begin();
             (iter != current->Children.end() && (!found));
             iter++)
		{					
			//visit my children 
			temp = *iter;
			if (temp->Visited == false)
			{
				VisitedNodes.push_back(temp);
				temp->Visited = true;				
				edges[temp] = current;
				if (temp == reference)
				{
					found = true;
				}
			}			
		}		
		//visit the parent
		if ((!found) && (current->Parent != NULL)) //not the root of the tree
		{
			//if not visited already, look farther via my parent
			if (current->Parent->Visited == false)
			{
				VisitedNodes.push_back(current->Parent);
				current->Parent->Visited = true;				
				edges[current->Parent] = current;
				if (current->Parent == reference)
				{
					found = true;
				}
			}
		}
	}

	//no path between the two nodes
	if (!found)
	{
		Path.clear();
		return false;
	}

	//generate the path
	Path.clear();
	temp = reference;
	while (edges[temp] != NULL)
	{
		Path.push_back(temp);
		temp = edges[temp];
	}
	Path.push_back(temp);

	//cleanup
	edges.clear();
	return true;
}

/*! Check for cyclic relationships in the transformation manager
\param attachpoint prmTransformationBasePtr reference for the new frame
\param newFrame prmTransformationBasePtr frame to be attached to the tree
\return true/false
*/

prmTransformationBasePtr prmTransformationManager::GetTransformationNodePtr(const std::string pName)
{
	NodeListType::iterator iter;
	prmTransformationBasePtr current;

    VisitedNodes.clear();
	VisitedNodes.push_back(&TheWorld);
	while (!VisitedNodes.empty()) 
	{
		//get the first node in 
		current = VisitedNodes.front();
		VisitedNodes.pop_front();
			
		// examine this node
        if (current->Name == pName)
		{
			//cleanup
			VisitedNodes.empty();
			return current; //found the node
		}

		for (iter = current->Children.begin();
            iter != current->Children.end();
            iter++)
		{
		    // Look through neighbors.
		    prmTransformationBasePtr temp = *iter;
		   if((temp->Name == pName))  
		   {
			   //cleanup
			   VisitedNodes.empty();
			   return temp; //found a cycle
			}
			else
			{
			   VisitedNodes.push_back(temp);
			}
		} //for
    }  //while

	//visited is empty anyway
	VisitedNodes.empty();
	return NULL; //no cycle found
 }

/*! Check for cyclic relationships in the transformation manager
\param attachpoint prmTransformationBasePtr reference for the new frame
\param newFrame prmTransformationBasePtr frame to be attached to the tree
\return true/false
*/

bool prmTransformationManager::NodeCreatesCycle(const std::string & pName, const prmTransformationBasePtr & newFrame)
{
	NodeListType::iterator iter;
	prmTransformationBasePtr current;

    //dont allow attachment to orphan frames
    if(newFrame->Parent == NULL)
    {
        return true;
    }
    //avoid attaching a node to itself or again to its parent
	if ((pName == newFrame->Name) || (pName == newFrame->Parent->Name))
	{		
		return true;
	}
	else
	{
		VisitedNodes.push_back(&TheWorld);
		while (!VisitedNodes.empty()) 
		{
			//get the first node in 
			current = VisitedNodes.front();
			VisitedNodes.pop_front();
			
			// examine this node
            if ((current == newFrame)||(current->Name == newFrame->Name))
			{
				//cleanup
				VisitedNodes.empty();
				return true; //found a cycle
			}

			for (iter = current->Children.begin();
                 iter != current->Children.end();
                 iter++)
			{
		       // Look through neighbors.
			   prmTransformationBasePtr temp = *iter;
			   if((temp == newFrame)||(current->Name == newFrame->Name))  
			   {
				   //cleanup
				   VisitedNodes.empty();
				   return true; //found a cycle
			   }
			   else
			   {
				   VisitedNodes.push_back(temp);
			   }
			} //for
       }  //while
     } //else

	//visited is empty anyway
	VisitedNodes.empty();
	return false; //no cycle found
 }

/*! Check for cyclic relationships in the transformation manager
\param attachpoint prmTransformationBasePtr reference for the new frame
\param newFrame prmTransformationBasePtr frame to be attached to the tree
\return true/false
*/

bool prmTransformationManager::NodeCreatesCycle(const prmTransformationBasePtr & attachPoint, const prmTransformationBasePtr & newFrame)
{
	NodeListType::iterator iter;
	prmTransformationBasePtr current;

	if (attachPoint->IsLinked(newFrame)||(attachPoint->Name == newFrame->Name))
	{		
		return true;
	}
	else
	{
		VisitedNodes.push_back(attachPoint);
		while (!VisitedNodes.empty()) 
		{
			//get the first node in 
			current = VisitedNodes.front();
			VisitedNodes.pop_front();
			
			// examine this node
            if ((current == newFrame)||(current->Name == newFrame->Name))
			{
				//cleanup
				VisitedNodes.empty();
				return true; //found a cycle
			}

			for (iter = current->Children.begin();
                 iter != current->Children.end();
                 iter++)
			{
		       // Look through neighbors.
			   prmTransformationBasePtr temp = *iter;
			   if((temp == newFrame)||(current->Name == newFrame->Name))  
			   {
				   //cleanup
				   VisitedNodes.empty();
				   return true; //found a cycle
			   }
			   else
			   {
				   VisitedNodes.push_back(temp);
			   }
			} //for
       }  //while
     } //else

	//visited is empty anyway
	VisitedNodes.empty();
	return false; //no cycle found
 }

unsigned int prmTransformationManager::ReplaceReference(const prmTransformationBasePtr & nodePtr, const prmTransformationBasePtr & newReference)
{
    unsigned int change_count = 0;
    if ((nodePtr == NULL) || (newReference == NULL))
	{
		/*RK: How do we usually do null-pointer error checking?
		*/
		return change_count;
	}

    //dont allow replacement with detached frames
    if((newReference->Parent == NULL) && (newReference != &TheWorld))
    {
        return change_count;
    }

    //can't create a cycle with a node itself.
	if (nodePtr == newReference)
	{
		return change_count;
	}
        
    if(nodePtr->Parent!=NULL)
    {
        //cleanup
        nodePtr->Parent->Children.remove(nodePtr);
        change_count = change_count + 1;
    }
    nodePtr->Parent = newReference;
    change_count = change_count + 1;
    
    return change_count;
}
unsigned int prmTransformationManager::ReplaceReference(const prmTransformationBasePtr & nodePtr, const std::string & parentName)
{
    prmTransformationBasePtr newReference = GetTransformationNodePtr(parentName);
    if(newReference != NULL)
    {
        return ReplaceReference(nodePtr,newReference);
    }
    return 0;
}

/*! \brief Compute the transformation between two frames (WRT = With Respect To). 
     The computation will use registered get mtsCommandRead methods for the nodes on the path from node refFrame to node tipFrame.
	 The implementation is a breadth first search from refFrame to tipFrame.
     \param tipFrame prmTransformationBasePtr the frame transform to be computed
	 \param refFrame prmTransformationBasePtr the reference frame chosen
	 \return vctFrame3 homogenous transform representing current relationship between tipFrame and refFrame.
*/
vctFrm3 prmTransformationManager::WRTReference(const prmTransformationBasePtr & tipFrame, const prmTransformationBasePtr & refFrame)
{
	vctFrm3 xform; //identity
	vctFrm3 curxform;
	prmTransformationBasePtr previous, current;
	//perform a tree traversal to get all the frames needed
	if (prmTransformationManager::FindPathConnectedTree(refFrame, tipFrame))
	{
		//multiply and return the result
		NodeListType::iterator iter = prmTransformationManager::Path.begin();
		previous = *iter;
		xform = previous->WRTReference();
		++iter;
		for (; iter!= prmTransformationManager::Path.end(); ++iter) 
		{
			current = *iter;
			curxform = current->WRTReference();
			if (previous->IsParentOf(current))
			{
				xform = curxform.Inverse() * xform;
			}
			else
			{
				xform = xform * curxform;
			}
			previous = current;
		}
	}
	return xform; 
}

/*! helper function for printing to graphviz files
*/
void prmTransformationManager::ToStreamDotHelper(std::ostream & outputStream, prmTransformationBasePtr parent)
{
	NodeListType::const_iterator iter = parent->Children.begin();
	const NodeListType::const_iterator iterEnd  = parent->Children.end();
	if (parent->Children.size() != 0)
	{
		for (; iter!= iterEnd; ++iter) 
		{
			outputStream << "node[style=filled,color=white,shape=box];" << std::endl
                         << "style=filled;" << std::endl
                         << "color=lightgrey;" << std::endl; 
			outputStream << parent->GetName() << " -> " << (*iter)->GetName() <<";" << std::endl;
			//recurse
			ToStreamDotHelper(outputStream, *iter);
		}
	}
}
								
/*! print to graphviz dot files
*/
void prmTransformationManager::ToStreamDot(std::ostream & outputStream)
{
    // dot header
    outputStream << "/* Automatically generated by cisstMultiTask, mtsTaskManager::ToStreamDot.\n"
                 << "   Use Graphviz utility \"dot\" to generate a graph of tasks/devices interactions. */"
                 << std::endl;
    outputStream << "digraph theWorld{" << std::endl;

    prmTransformationBasePtr parent = &TheWorld;
    // create nodes for all children of this frame
    ToStreamDotHelper(outputStream, parent);
    // end of file
    outputStream << "}" << std::endl;
}

/*print a path to graphviz dot file
*/
void prmTransformationManager::PathToStreamDot(std::ostream & outputStream)
{
 	NodeListType::const_iterator iter = Path.begin();
	const NodeListType::const_iterator iterEnd  = Path.end();
	prmTransformationBasePtr last;
    // dot header
    outputStream << "/* Automatically generated by cisstMultiTask, mtsTaskManager::ToStreamDot.\n"
                 << "   Use Graphviz utility \"dot\" to generate a graph of tasks/devices interactions. */"
                 << std::endl;
    outputStream << "digraph thePath{" << std::endl;
	if (Path.size() != 0)
    {
        last = *iter;
		++iter;
		for (; iter != iterEnd; ++iter) 
		{
			outputStream << "node[style=filled,color=white,shape=box];" << std::endl
                         << "style=filled;" << std::endl
                         << "color=lightgrey;" << std::endl; 
			
			if (last->IsParentOf(*iter))
			{
				outputStream << last->GetName() << " -> " << (*iter)->GetName() << ";" << std::endl;
			}
			else
			{
				outputStream << (*iter)->GetName() << " ->" << last->GetName() << ";" << std::endl;
			}
			last = *iter;
		}
	} //if have some nodes in the list
    // end of file
    outputStream << "}" << std::endl;
}

