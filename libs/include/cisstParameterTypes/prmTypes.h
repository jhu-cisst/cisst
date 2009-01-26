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
  \brief Simple types and types that are not currently fully defined.
*/


#ifndef _prmTypes_h
#define _prmTypes_h

/*-----------------------------------basic cisst includes----------------------------------------------*/
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstVector/vctTransformationTypes.h>

/*-----------------------------------necessary defines------------------------------------------------*/
#define PRM_NO_TOOL_INSTALLED           0      /*!temp definition while tool definitions are worked out*/
#define PRM_STATUS_BAD                  false  /*!temp definition while status definition is worked out*/
#define PRM_CART_DOFS                   12     /*! 12 member homogeneous matrices [Pxyz Rx Ry Rz]      */
#define PRM_FORCE_DOFS                  3      /*! X_Y_Z force vectors                                 */
#define PRM_TORQUE_DOFS                 3      /*! Torque vector dimensions                            */
#define PRM_FORCE_TORQUE_DOFS           6      /* Default F/T vector dimensions                        */

/*-----------------------------------enums------------------------------------------------------------ */
/*! Parameters type for command execution/completion behavior specification                            */
typedef enum Blocking { 
	NO_WAIT,      /*! non blocking */
	WAIT_START,   /*! blocking until start, non preemptive */
	WAIT_FINISH   /*! blocking type  */
} prmBlocking ;

/*-----------------------------------basic types-------------------------------------------------------*/
/*! Parameter type for command transition behavior specifications.
    This acts just as a boolean for now. Blending factors to be implemented in later versions.
 */
typedef unsigned int prmBlending; 

/*! Time
  Possible units are seconds, milliseconds, microseconds. Consensus leaning towards seconds.
  */
typedef double prmTime;       

/*! Forces
  Units are Newtons?
 */
typedef double prmForce;

/*! Torques
  Units are Newtons-m or Newton-mm?
 */
typedef double prmTorque;

/*! vector for multiple true/false returns
*/
typedef vctDynamicVector<bool> prmBoolVec;

/*! position and velocity vectors
*/
typedef vctDynamicVector<double> prmDoubleVec;

/*! Force/torque vectors.
 */
typedef vctDynamicVector<prmForce> prmForceVec;

/*! Joint angles and such.  Current suggested units are (mm,deg), SI
  (m,rad), or (mm,rad) - last one for sine/cosine+readability.
  Consensus leaning on (mm,deg) for readability as of 01/08.
*/
typedef prmDoubleVec prmPosition;   

/*! Velocities - separate type primarily for disambiguation for
  position vectors.  Units are time (unit seconds) derivatives of
  prmPosition.
*/
typedef prmDoubleVec prmVelocity;   

/*! Acceleration/deceleration - separate type primarily for
  disambiguation for position/velocity vectors. Units are time (unit
  seconds) derivatives of prmVelocity.
*/
typedef prmDoubleVec prmAcceleration;   

/*! Homogeneous Cartesian frames
*/
typedef vctFrm3 prmCartesianPosition;

/*! Disambiguation from positions - but cartesian velocities are
  expected to be only two values - one linear (vector to goal
  magnitude time derivative)
*/
typedef vct3 prmCartesianVelocity; 	

/*! Primarily for disambiguation from positions - these are expected
  to be only 2 values
*/
typedef prmDoubleVec prmCartesianAcceleration; 	

/*! Robot status will eventually be a class, for now just a bool -
  good = true, bad = false; Temporary definition.
*/
typedef bool prmStatus;

/*! tool will eventually be a class, for now just an int identifier
  for a tool.  Temporary definition.
*/
typedef unsigned int  prmTool; 


#endif // _prmTypes_h

