#ifndef _cisstOpenNI_h
#define _cisstOpenNI_h

#include <cisstVector.h>

#include <cisstOpenNI/cisstOpenNIExport.h>
#include <cisstOpenNI/cisstOpenNISkeleton.h>

#define WAIT_AND_UPDATE_ALL         1
#define WAIT_ANY_UPDATE_ONE         2
#define WAIT_NONE_UPDATE_ALL        3

#define CNI_USR_NEW         0
#define CNI_USR_LOST        1
#define CNI_USR_POSE        2
#define CNI_USR_CAL_START   3
#define CNI_USR_CAL_END     4
#define CNI_USR_SUCCESS     5
#define CNI_USR_FAIL        6
#define CNI_USR_WAIT        7
#define CNI_USR_IDLE        -1


/*! 
\todo move ctor code to Configure method
\todo move all use of OpenNI symbols to .cpp file, i.e. do not include Xn files in cisstOpenNI.h
\todo move openNISkeleton class as sub class of OpenNI
\todo add CMN_LOG_
\todo add std cisst headers
\todo cleanup CMakeLists, no auto find
\todo follow cisst naming convention
*/

class cisstOpenNIData;

class CISST_EXPORT cisstOpenNI {

    friend class cisstOpenNISkeleton;

 public: 
    
    enum Errno{ ESUCCESS, EFAILURE };

private:

    //! Private OpenNI Data Structure
    cisstOpenNIData* Data;

    //! Identifier for the openNI Object
    std::string name;

    std::vector<cisstOpenNISkeleton*> skeletons;
    
    int users;
    char userCalibDataPath;
    bool usingPrecapturedCalibration;


public:

    //! Default Constructor
    cisstOpenNI( int numUsers = 0 );
    
    //! Constructor for Predefined Skeleton Calibration FIles
    cisstOpenNI(int numUsers, char usrPath);
    
    //! Default DeConstructor
    ~cisstOpenNI();

    //! Clean Up and Exit
    void CleanupExit();

    //! Configure
    /**
    Creates all nodes, populates callback lists and establishes contexts for depth, rgbImage
    and users.
    */
    void Configure( const std::string& devname = "" );

    //! Update All
    /**
    Calls the wait and update all method of the XN wrapper.  
    This needs to be called each iteration.
    */
    void Update(int type);

    //! INit skeletons
    void InitSkeletons();

    //! Get range data
    /**
    Query the depth generator to obtain a depth image and convert the image to a points
    cloud. If the depth image is MxN, then the point cloud is a matrix of size 3x(MxN) where
    each column are the X-Y-Z coordinate of a point. This method is non-const due to updating
    the context.
    \return A 3x(MxN) point cloud.
    */
    cisstOpenNI::Errno GetRangeData( vctDynamicMatrix<double>& rangedata );

    //! Get Raw Depth Image
    /**
    Query the depth generator to obtain a depth image. Resulting image as 8-bit depth. 
    The value at each pixel represents the depth of the picture element. This method 
    is non-const due to updating the context.
    */
    cisstOpenNI::Errno GetDepthImageRaw( vctDynamicMatrix<double>& depthimage );

    //! Get Depth Image
    /**
    Query the depth generator to obtain a depth image. Resulting image as 11-bit depth. 
    The value at each pixel represents the depth of the picture element. This method 
    is non-const due to updating the context.
    */
    void GetDepthImage( vctDynamicMatrix<double>& placeHolder );

    //! Get (interlaced) RGB image
    /**

    */
    cisstOpenNI::Errno GetRGBImage( vctDynamicMatrix<unsigned char>& RGBinterlaced );

    //! Get (planar) RGB image
    /**
    */
    cisstOpenNI::Errno GetRGBPlanarImage( vctDynamicNArray<unsigned char,3>& RGBplanar );


    //! Get Current User Skeletons
    /**
     */
    std::vector<cisstOpenNISkeleton*> &UpdateAndGetUserSkeletons();

    //! Get Current User Skeletons
    /**
     */
    void UpdateUserSkeletons();

    //! Get Current User Skeletons
    /**
     */
    std::vector<cisstOpenNISkeleton*> &GetUserSkeletons();

};

#endif
