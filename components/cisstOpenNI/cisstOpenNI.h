#ifndef _cisstOpenNI_h
#define _cisstOpenNI_h

#include <cisstVector.h>

#include <cisstOpenNI/cisstOpenNIExport.h>
#include <cisstOpenNI/cisstOpenNISkeleton.h>

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

private:

    //! Private OpenNI Data Structure
    cisstOpenNIData* Data;

    //! Identifier for the openNI Object
    std::string name;

    std::vector<cisstOpenNISkeleton> skeletons;


public:

    //! Default Constructor
    cisstOpenNI();

    //! Default DeConstructor
    ~cisstOpenNI();

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
    void UpdateAll();

    //! Get range data
    /**
    Query the depth generator to obtain a depth image and convert the image to a points
    cloud. If the depth image is MxN, then the point cloud is a matrix of size 3x(MxN) where
    each column are the X-Y-Z coordinate of a point. This method is non-const due to updating
    the context.
    \return A 3x(MxN) point cloud.
    */
    vctDynamicMatrix<double> GetRangeData();

    //! Get depth image
    /**
    Query the depth generator to obtain a depth image. The value at each pixel represents
    the depth of the picture element. This method is non-const due to updating the context.
    */
    vctDynamicMatrix<double> GetDepthImage();

    //! Get (interlaced) RGB image
    /**

    */
    vctDynamicMatrix<unsigned char> GetRGBImage();

    //! Get (planar) RGB image
    /**
    */
    vctDynamicNArray<unsigned char,3> GetRGBPlanarImage();

    //! Get Current User Skeletons
    /**
    */
    std::vector<cisstOpenNISkeleton> UpdateAndGetUserSkeletons();

};

#endif
