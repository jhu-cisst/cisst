#include <cisstNumerical/nmrKinematics.h>

CMN_IMPLEMENT_SERVICES(nmrKinematics);

/*! Constructor
\param f Frame pointer
\param cVel Cartesian Velocity pointer
\param j Jacobian pointer
\param jPos Joint Position pointer
\param jVel Joint Velocity pointer
*/
nmrKinematics::nmrKinematics(vctFrm3 * f, vctDoubleVec * cVel, JacobianType * j, vctDoubleVec * jPos, vctDoubleVec * jVel)
{
	frame = f;
	cartVel = cVel;
	jac = j;
	jointPos = jPos;
	jointVel = jVel;
}

//! Gets the frame. 
/*! getFrame
\return vctFrm3 Frame
*/
vctFrm3 nmrKinematics::getFrame()
{
	return *frame;
}

//! Gets cartesian velocity. 
/*! getCartVel
\return vctDoubleVec Cartesian velocity
*/
vctDoubleVec nmrKinematics::getCartVel()
{
	return *cartVel;
}

//! Gets the jacobian. 
/*! getJacobian
\return JacobianType Jacobian
*/
JacobianType nmrKinematics::getJacobian()
{
	return *jac;
}

//! Gets the joint positions. 
/*! getJointPos
\return vctDoubleVec getJointPos
*/
vctDoubleVec nmrKinematics::getJointPos()
{
	return *jointPos;
}

//! Gets the joint velocities. 
/*! getJointVel
\return vctDoubleVec Joint velocities
*/
vctDoubleVec nmrKinematics::getJointVel()
{
	return *jointVel;
}