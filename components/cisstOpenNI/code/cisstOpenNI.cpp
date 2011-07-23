#include <cisstOpenNI/cisstOpenNI.h>
#include "cisstOpenNIData.h"


cisstOpenNI::cisstOpenNI(int numUsers){

    this->Data = new cisstOpenNIData;
    this->Data->SetStates();
    this->users = numUsers;

}

cisstOpenNI::~cisstOpenNI()
{
    if(this->Data)
        delete this->Data;
}

void cisstOpenNI::CleanupExit(){
    Data->context.Shutdown();
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

void cisstOpenNI::Update(int type){
    // Query the context based on type
    if(type == WAIT_NONE_UPDATE_ALL)
        Data->context.WaitNoneUpdateAll();
    else if(type == WAIT_AND_UPDATE_ALL)
        Data->context.WaitAndUpdateAll();
    else if(type == WAIT_ANY_UPDATE_ONE)
        Data->context.WaitAnyUpdateAll();
}

void cisstOpenNI::InitSkeletons(){

    cisstOpenNISkeleton* skeleton;
    for(int i = 0; i<this->users; i++){
        skeleton = new cisstOpenNISkeleton(this);
        skeletons.push_back(skeleton);
        std::cout<<skeletons.size()<<std::endl;
    }

}

void cisstOpenNI::GetDepthImageRaw(vctDynamicMatrix<double> & placeHolder){

    // Get data
    xn::DepthMetaData depthMD;
    Data->depthgenerator.GetMetaData( depthMD );
    const XnDepthPixel* pDepth = depthMD.Data();

    placeHolder.SetSize( depthMD.YRes(), depthMD.XRes() );
    double* ptr = placeHolder.Pointer();
    const size_t end = depthMD.YRes()*depthMD.XRes();
    for( size_t i = 0; i < end; i++ )
    {
        (*ptr) = (*pDepth);
        ptr++; pDepth++;
    }
}

void cisstOpenNI::GetDepthImage(vctDynamicMatrix<double> & placeHolder){

    // Get data
    xn::DepthMetaData depthMD;
    Data->depthgenerator.GetMetaData( depthMD );
    const XnDepthPixel* pDepth = depthMD.Data();

    placeHolder.SetSize( depthMD.YRes(), depthMD.XRes() );
    double* ptr = placeHolder.Pointer();
    const size_t end = depthMD.YRes()*depthMD.XRes();
    for( size_t i = 0; i < end; i++ )
    {
        (*ptr) =  255.0 * (*pDepth) / 2048.0;
        ptr++; pDepth++;
    }
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

std::vector<cisstOpenNISkeleton*> &cisstOpenNI::UpdateAndGetUserSkeletons(){

    // Initialize Users
    XnUserID aUsers[this->users];
    XnUInt16 nUsers = this->users;
    Data->usergenerator.GetUsers(aUsers, nUsers);

    if(nUsers > this->users) printf("More users than max allowance\n");

    for (int i = 0; i < this->users; ++i)
    {
        if (Data->usergenerator.GetSkeletonCap().IsTracking(aUsers[i]))
        {
            this->skeletons[i]->Update(aUsers[i]);
        }else{
            this->skeletons[i]->SetExists(false);
        }
    }

    return skeletons;
}

void cisstOpenNI::UpdateUserSkeletons(){
    
    // Initialize Users
    XnUserID aUsers[this->users];
    XnUInt16 nUsers = this->users;
    Data->usergenerator.GetUsers(aUsers, nUsers);
    
    if(nUsers > this->users) printf("More users than max allowance\n");
    
    for (int i = 0; i < this->users; ++i)
    {
        if (Data->usergenerator.GetSkeletonCap().IsTracking(aUsers[i]))
        {
            this->skeletons[i]->Update(aUsers[i]);
            this->skeletons[i]->usrState = Data->usrState;
            this->skeletons[i]->calState = Data->usrCalState;
        }else{
            this->skeletons[i]->SetExists(false);
        }
    }    
}

std::vector<cisstOpenNISkeleton*> &cisstOpenNI::GetUserSkeletons(){
    return skeletons;
}









