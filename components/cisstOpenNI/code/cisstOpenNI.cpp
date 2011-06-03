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
                                   void* pCookie)
{
    printf("New User %d\n", nId);

    cisstOpenNIData* openNIDataObject = reinterpret_cast<cisstOpenNIData*>(pCookie);

    openNIDataObject->NewUserCallback(generator,nId);
}

// Callback: An existing user was lost
void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, 
                                    XnUserID nId, 
                                    void* pCookie)
{
    printf("Lost user %d\n", nId);

    cisstOpenNIData* openNIDataObject = reinterpret_cast<cisstOpenNIData*>(pCookie);
}


// Callback: Detected a pose
void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, 
                                            const XnChar* strPose, 
                                            XnUserID nId, 
                                            void* pCookie)
{
    printf("Pose %s detected for user %d\n", strPose, nId);

    cisstOpenNIData* openNIDataObject = reinterpret_cast<cisstOpenNIData*>(pCookie);

    openNIDataObject->UserPoseDetectedCallback(capability,strPose,nId);
}

// Callback: Started calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, 
                                                       XnUserID nId, 
                                                       void* pCookie)
{
    printf("Calibration started for user %d\n", nId);
}

// Callback: Finished calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationEnd(xn::SkeletonCapability& capability, 
                                                     XnUserID nId, 
                                                     XnBool bSuccess, 
                                                     void* pCookie)
{
    cisstOpenNIData* openNIDataObject = reinterpret_cast<cisstOpenNIData*>(pCookie);

    openNIDataObject->UserCalibrationEndCallback(capability,bSuccess,nId);
}

cisstOpenNI::cisstOpenNI(){

    this->Data = new cisstOpenNIData;
    this->skeletons.resize(6);

}

cisstOpenNI::~cisstOpenNI()
{
    if(this->Data)
        delete this->Data;

}

void cisstOpenNI::Configure( const std::string& fname  ){

    // Store XML Config File
    this->name = fname;

    // Errors
    xn::EnumerationErrors errors;

    // Init
    XnStatus status = Data->context.InitFromXmlFile( this->name.data(), &errors );

    if( status == XN_STATUS_NO_NODE_PRESENT ){
        XnChar strError[1024];
        errors.ToString( strError, 1024 );
        std::cout << strError << std::endl;
        return;
    }

    else if( status != XN_STATUS_OK ){
        std::cout << "Failed to open " << this->name << " " 
            << xnGetStatusString( status ) << std::endl;
        return;
    }

    status = Data->context.FindExistingNode( XN_NODE_TYPE_DEPTH, Data->depthgenerator );
    status = Data->context.FindExistingNode( XN_NODE_TYPE_IMAGE, Data->rgbgenerator );
    status = Data->depthgenerator.GetAlternativeViewPointCap().SetViewPoint(Data->rgbgenerator);

    if( status != XN_STATUS_OK ){
        std::cout << "Failed to set viewpoint: "
            << xnGetStatusString(status) << std::endl;
        return;
    }

    status = Data->context.FindExistingNode(XN_NODE_TYPE_USER, Data->usergenerator);
    if (status != XN_STATUS_OK)
    {
        status = Data->usergenerator.Create(Data->context);
    }




    XnCallbackHandle hUserCallbacks, hCalibrationCallbacks, hPoseCallbacks;
    if (!Data->usergenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
    {
        printf("Supplied user generator doesn't support skeleton\n");
    }
    Data->usergenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, this->Data, hUserCallbacks);

    Data->usergenerator.GetSkeletonCap().RegisterCalibrationCallbacks(	UserCalibration_CalibrationStart,
                                                                        UserCalibration_CalibrationEnd,
                                                                        this->Data,
                                                                        hCalibrationCallbacks);

    if (Data->usergenerator.GetSkeletonCap().NeedPoseForCalibration()){
        Data->needPose = TRUE;
        if (!Data->usergenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION)){
            printf("Pose required, but not supported\n");
        }
        Data->usergenerator.GetPoseDetectionCap().RegisterToPoseCallbacks(	UserPose_PoseDetected, 
                                                                            NULL, 
                                                                            this->Data, 
                                                                            hPoseCallbacks);
        Data->usergenerator.GetSkeletonCap().GetCalibrationPose(Data->strPose);
    }
    

    Data->usergenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

    // Start Generating
    Data->context.StartGeneratingAll();

}

void cisstOpenNI::UpdateAll(){
    
    // Query the context
    Data->context.WaitAndUpdateAll();

}

vctDynamicMatrix<double> cisstOpenNI::GetDepthImage(){

    // Get data
    xn::DepthMetaData depthMD;
    Data->depthgenerator.GetMetaData( depthMD );
    const XnDepthPixel* pDepth = depthMD.Data();

    vctDynamicMatrix<double> depthimage( depthMD.YRes(), depthMD.XRes() );
    double* ptr = depthimage.Pointer();
    for( size_t i=0; i<depthMD.YRes()*depthMD.XRes(); i++ )
    { ptr[i] =  pDepth[i] / 1000.0; }

    return depthimage;

}

vctDynamicMatrix<double> cisstOpenNI::GetRangeData(){

    // Get data
    xn::DepthMetaData depthMD;
    Data->depthgenerator.GetMetaData( depthMD );

    // create arrays
    XnUInt32 cnt = depthMD.XRes()*depthMD.YRes();
    XnPoint3D* proj = NULL;
    XnPoint3D* wrld = NULL;
    try{ 
        proj = new XnPoint3D[ cnt ];
        wrld = new XnPoint3D[ cnt ];
    }
    catch( std::bad_alloc& ){}

    CMN_ASSERT( proj != NULL );
    CMN_ASSERT( wrld != NULL );

    // Create projective coordinates
    for( size_t i=0, x=0; x<depthMD.XRes(); x++ ){
        for( size_t y=0; y<depthMD.YRes(); i++, y++ ){
            proj[i].X = (XnFloat)x;
            proj[i].Y = (XnFloat)y;
            proj[i].Z = depthMD(x,y);
        }
    }

    // Convert projective to 3D
    XnStatus status = Data->depthgenerator.ConvertProjectiveToRealWorld(cnt, proj, wrld);
    if( status != XN_STATUS_OK ){
        std::cout << "Failed to convert projective to world: "
            << xnGetStatusString( status ) << std::endl;
    }

    // create matrix
    vctDynamicMatrix<double> rangedata( 3, cnt );
    for( size_t i=0; i<cnt; i++ ){
        rangedata[0][i] = -wrld[i].X/1000.0;
        rangedata[1][i] = wrld[i].Y/1000.0;
        rangedata[2][i] = wrld[i].Z/1000.0;
    }

    delete[] proj;
    delete[] wrld;

    return rangedata;
}

vctDynamicMatrix<unsigned char> cisstOpenNI::GetRGBImage(){

    // Get data
    xn::ImageMetaData rgbMD;
    Data->rgbgenerator.GetMetaData( rgbMD );

    // create image
    vctDynamicMatrix<unsigned char>  rgbimage( rgbMD.YRes(), rgbMD.XRes()*3 );
    memcpy( rgbimage.Pointer(), 
        rgbMD.Data(), 
        rgbMD.YRes()*rgbMD.XRes()*3*sizeof(unsigned char) );

    return rgbimage;
}

vctDynamicNArray<unsigned char,3> cisstOpenNI::GetRGBPlanarImage(){

    // Get data
    Data->context.WaitOneUpdateAll( Data->rgbgenerator );
    xn::ImageMetaData rgbMD;
    Data->rgbgenerator.GetMetaData( rgbMD );

    vctDynamicNArray<unsigned char, 3> rgbimage;
    rgbimage.SetSize( vctDynamicNArray<unsigned char, 3>::nsize_type( rgbMD.YRes(),
        rgbMD.XRes(),
        3 ) );
    memcpy( rgbimage.Pointer(), rgbMD.Data(), rgbMD.YRes()*rgbMD.XRes()*3 );

    return rgbimage;

}

std::vector<cisstOpenNISkeleton> cisstOpenNI::UpdateAndGetUserSkeletons(){

    // Initialize Users
    XnUserID aUsers[15];
    XnUInt16 nUsers = 15;
    Data->usergenerator.GetUsers(aUsers, nUsers);

    if(nUsers > 6) printf("More users than max allowance\n");

    for (int i = 0; i < 5; ++i)
    {
        if (Data->usergenerator.GetSkeletonCap().IsTracking(aUsers[i]))
        {
            this->skeletons[i].Update(aUsers[i],Data->context, Data->depthgenerator, Data->usergenerator);
        }else{
            this->skeletons[i].SetExists(false);
        }
    }

    return this->skeletons;

}

void cisstOpenNIData::NewUserCallback(  xn::UserGenerator& generator, 
                                    XnUserID nId)
{
    if (this->needPose)
    {
        usergenerator.GetPoseDetectionCap().StartPoseDetection(this->strPose, nId);
    }
    else
    {
        usergenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
    }
}

void cisstOpenNIData::UserPoseDetectedCallback( xn::PoseDetectionCapability& capability,
                                            const XnChar* strPose,
                                            XnUserID nId)
{ 
	usergenerator.GetPoseDetectionCap().StopPoseDetection(nId);
	usergenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}

void cisstOpenNIData::UserCalibrationEndCallback(   xn::SkeletonCapability& capability,
                                                XnBool bSuccess,  
                                                XnUserID nId)
{ 
	if (bSuccess)
	{
		// Calibration succeeded
		printf("Calibration complete, start tracking user %d\n", nId);
		usergenerator.GetSkeletonCap().StartTracking(nId);
	}
	else
	{
		// Calibration failed
		printf("Calibration failed for user %d\n", nId);
		if (this->needPose)
		{
			usergenerator.GetPoseDetectionCap().StartPoseDetection(this->strPose, nId);
		}
		else
		{
			usergenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
		}
	}
}

