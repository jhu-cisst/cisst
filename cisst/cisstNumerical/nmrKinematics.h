#ifndef _nmrKinematics_h
#define _nmrKinematics_h


/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 $Id: $
 
 Author(s):  Paul Wilkening
 Created on:
 
 (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.
 
 --- begin cisst license - do not edit ---
 
 This software is provided "as is" under an open source license, with
 no warranty.  The complete license can be found in license.txt and
 http://www.cisst.org/cisst/license.txt.
 
 --- end cisst license ---
 */

#include <cisstCommon/cmnGenericObject.h> 
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctDynamicMatrixTypes.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstParameterTypes/prmTransformationManager.h>
#include <cisstParameterTypes/prmRobotState.h>

typedef vctFixedSizeMatrix<double, 6, 5, VCT_COL_MAJOR> JacobianType;

//! This is a collection of joint and cartesian data relating to a frame
/*! \brief nmrKinematics: A class that makes updating nmrVFData with the robot state at a given point easier
 */
class nmrKinematics : public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_VERBOSE);

public:
	
	/*! Constructor	
	*/
	nmrKinematics(){}; 
	/*! Constructor
	\param f Frame pointer
	\param cVel Cartesian Velocity pointer
	\param j Jacobian pointer
	\param jPos Joint Position pointer
	\param jVel Joint Velocity pointer
	*/
	nmrKinematics(vctFrm3 * f, vctDoubleVec * cVel, JacobianType * j, vctDoubleVec * jPos, vctDoubleVec * jVel);
	//! Gets the frame. 
	/*! getFrame
	\return vctFrm3 Frame
	*/
	vctFrm3 getFrame();
	//! Gets cartesian velocity. 
	/*! getCartVel
	\return vctDoubleVec Cartesian velocity
	*/
	vctDoubleVec getCartVel();
	//! Gets the jacobian. 
	/*! getJacobian
	\return JacobianType Jacobian
	*/
	JacobianType getJacobian();
	//! Gets the joint positions. 
	/*! getJointPos
	\return vctDoubleVec getJointPos
	*/
	vctDoubleVec getJointPos();
	//! Gets the joint velocities. 
	/*! getJointVel
	\return vctDoubleVec Joint velocities
	*/
	vctDoubleVec getJointVel();

private:

	//! Frame
	vctFrm3 * frame;
	//! Cartesian velocities
	vctDoubleVec * cartVel;
	//! Jacobian
	JacobianType * jac;
	//! Joint positions
	vctDoubleVec * jointPos;
	//! Joint velocities
	vctDoubleVec * jointVel;

};

CMN_DECLARE_SERVICES_INSTANTIATION(nmrKinematics);

#endif
