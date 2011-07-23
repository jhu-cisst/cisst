#ifndef _cisstOpenNISkeleton_h
#define _cisstOpenNISkeleton_h

#include <cisstVector.h>

class cisstOpenNI;

class CISST_EXPORT cisstOpenNISkeleton {

private:

    cisstOpenNI* OpenNI;


public:
    //! 3D Joint Coordinates
    std::vector<vct3> points3D;
    
	//! 2D Projective Coorinates
	std::vector<vctInt2> points2D;
    
	//! Joint Existance Confidence
	std::vector<bool> confidence;
    
	//! Skeleton has been populated coorectly
	bool exists;

	//! Default Constructor
	cisstOpenNISkeleton( cisstOpenNI * openNI);

	//! Default Deconstructor
	~cisstOpenNISkeleton();

	//! Build Skeleton Using XN Context for a given user
	void Update(int id);

	void SetExists(bool val);
    
    int usrState;
    int calState;

};

#endif