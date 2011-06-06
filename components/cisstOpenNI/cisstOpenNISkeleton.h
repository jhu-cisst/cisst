#ifndef _cisstOpenNISkeleton_h
#define _cisstOpenNISkeleton_h

#include <cisstVector.h>
#include <XnCppWrapper.h>
#include <XnOS.h>

class CISST_EXPORT cisstOpenNISkeleton {

private:

	//! 3D Joint Coordinates
    std::vector<vct3> points3D;

	//! 2D Projective Coorinates
	std::vector<vctInt2> points2D;

	//! Joint Existance Confidence
	std::vector<bool> confidence;

	//! Skeleton has been populated coorectly
	bool exists;


public:

	//! Default Constructor
	cisstOpenNISkeleton();

	//! Default Deconstructor
	~cisstOpenNISkeleton();

	//! Build Skeleton Using XN Context for a given user
	void Update(	XnUserID id, 
				    xn::Context &context, 
				    xn::DepthGenerator &depthgenerator, 
				    xn::UserGenerator &usergenerator);

	void SetExists(bool val);

};

#endif