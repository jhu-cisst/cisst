#ifndef _cisstOpenNIData_h
#define _cisstOpenNIData_h
#include <cisstOpenNI/cisstOpenNI.h>

#include <XnCppWrapper.h>
#include <XnOS.h>

class cisstOpenNIData
{

public:

    //! OpenNI context
    xn::Context context;

    //! Depth image generator
    xn::DepthGenerator depthgenerator;

    //! RGB image generator
    xn::ImageGenerator rgbgenerator;

    //! User Skeleton generator
    xn::UserGenerator usergenerator;

    //! User Pose State
    XnBool needPose;

    //! Pose Callback String
    XnChar strPose[20];

    //! New User Callback
    /**
    */
    void NewUserCallback(           xn::UserGenerator& generator, 
                                    XnUserID nId);

    //! New User Calibration Pose Detected
    /**
    */
    void UserPoseDetectedCallback(  xn::PoseDetectionCapability& capability,
                                    const XnChar* strPose,
                                    XnUserID nId);

    //! User Calibration End
    /**
    Cues that the calibration has ended, either with success or failure.
    */
    void UserCalibrationEndCallback(xn::SkeletonCapability& capability,
                                    XnBool bSuccess,  
                                    XnUserID nId);

};

//----------------------------------------------------------------------------/
// BEGIN Callbacks
//----------------------------------------------------------------------------/

// Callback: New user was detected
void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& generator, 
                                   XnUserID nId, 
                                   void* pCookie);
// Callback: An existing user was lost
void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, 
                                    XnUserID nId, 
                                    void* pCookie);


// Callback: Detected a pose
void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, 
                                            const XnChar* strPose, 
                                            XnUserID nId, 
                                            void* pCookie);

// Callback: Started calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, 
                                                       XnUserID nId, 
                                                       void* pCookie);

// Callback: Finished calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationEnd(xn::SkeletonCapability& capability, 
                                                     XnUserID nId, 
                                                     XnBool bSuccess, 
                                                     void* pCookie);


#endif