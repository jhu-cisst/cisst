/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Robert Jacques (?)
  Created on: 2007 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#ifndef _svlTrackerCisstNCC_h
#define _svlTrackerCisstNCC_h

#include "svlTrackerCisstBase.h"

#include <cisstConfig.h>

#include "vctExtraTemplateFunctions.h"
#include <vector>
#include <iostream>


//#define IS_LOGGING

class svlTrackerCisstNCC : public svlTrackerCisstBase
{
public:   // was private earlier
#if CISST_SVL_HAS_OPENCV
	//Incomming Data format, should be different from MatrixType, otherwise
	//eleminate copyImage functions.
	typedef IplImage* ImageType;
#endif // CISST_SVL_HAS_OPENCV
	// Data type for image to interface with Siemens Radbuilder and ICE programs
	typedef unsigned short InterfaceImType;
	
	//Basic numeric type for processing, should be a real type
	typedef double RealType;
	//A matrix data type
	typedef vctDynamicMatrix < RealType > MatrixType;
	//Datatype for indexes
	typedef MatrixType::index_type IndexType;
	//Submatrix Type
	typedef MatrixType::Submatrix::Type SubMatrixType;
	//Matrix data, if different from a Matrix view
    typedef std::vector < MatrixType > MatrixList;
	//Matrix data, if different from a Matrix view
    typedef std::vector < MatrixType * > MatrixRefList;

	//2D point data type for r,c
	typedef vctFixedSizeVector < IndexType , 2> PointType;
public:
	//Enum declarations
	enum ConvolutionMethod { Auto, FFT, Standard };

	//Constructor/Destructor
	svlTrackerCisstNCC(void);
	~svlTrackerCisstNCC(void);

	//Cisst Library Test Routines
	//void test();

	//Offine initialization routines

    //Select the number of templates to search through, 
	//should be at least 2 for multi template use
	//Defaults to all if not set
	void setTemplateLookahead(IndexType number);
	//Select the convolution method to use
	void setConvolutionMethodTo(ConvolutionMethod method);
	//Set the size of the template. 
	//All templates must be the same size.
	void setTemplateSize(IndexType rows, IndexType cols);
	//Set the center offset of the template.
	void setTemplateCenter(IndexType r, IndexType c);
	//Set the tracking window size
	void setWindowSize(IndexType rows, IndexType cols);
	//Set the center offset of the tracking window
	void setWindowCenter(IndexType r, IndexType c);
	//Set the initial position of the tracking window
	void setWindowPosition(IndexType r, IndexType c);
	// Set the initial position for tracking (empty in this case)
	void setInitPosition(IndexType r, IndexType c);
	// Set the image size field
	void setImageSize(IndexType rows, IndexType cols);
#if CISST_SVL_HAS_OPENCV
	//Add another template to the tracker (uses opencv)
	void pushTemplate_opencv(ImageType &src, IndexType r, IndexType c);
#endif // CISST_SVL_HAS_OPENCV
	//Add another template to the tracker 
	void pushTemplate(InterfaceImType *src, IndexType src_rows, IndexType src_cols, IndexType r, IndexType c);
	//Add another template to the tracker 
	void pushTemplate(InterfaceImType *src, IndexType src_rows, IndexType src_cols);
	//Remove last template from the tracker
	void popTemplate();
	//Get the number of templates loaded
	//Explicit unsigned int required??
	IndexType getNumberOfTemplates();
#if CISST_SVL_HAS_OPENCV
	//Copy a choosen template to an image (uses opencv)
	void getTemplate_opencv(IndexType index, ImageType &dst);
#endif // CISST_SVL_HAS_OPENCV
	//Copy a choosen template to an image 
	//void getTemplate(IndexType index, MatrixType &dst);
	//Add a loaded template to the end of the selected templates list
	void pushSelectedTemplate(IndexType index);
	//Remove the last selected template
	void popSelectedTemplate();
	// Set the size of the current image
	void setCurrentimageSize(IndexType rows, IndexType cols);
	// Copy the current image
	void copyCurrentImage(InterfaceImType *src, IndexType src_rows, IndexType src_cols);
	//Inititalize the pre-allocated data for tracking
	void initializeTrack();

	//Online tracking routines and feedback
#if CISST_SVL_HAS_OPENCV
	//Update the current window position (uses opencv)
	void updateTrack_opencv(ImageType &  src);
#endif // CISST_SVL_HAS_OPENCV
	//Update the current window position
	void updateTrack();
	//Get image location
	IndexType getWindowRow();
	IndexType getWindowCol();
	//Get the correlation score, bounded [-1,1]	
	RealType getTrackScore();

	//Other useful functions

	//Cear stored data.
	void resetTrack();
	//Get template dimentions
	IndexType getTemplateNumRows();
	IndexType getTemplateNumCols();
	//Get template center location
	IndexType getTemplateCenterRow();
	IndexType getTemplateCenterCol();
	//Get the tracking window size
	IndexType getWindowNumRows();
	IndexType getWindowNumCols();
	//Get the relative center position of the window
	IndexType getWindowCenterRow();
	IndexType getWindowCenterCol();
	//Get the current image size
	IndexType getCurrentimageNumRows();
	IndexType getCurrentimageNumCols();
	// Get the current (tracked) location 
	RealType getOutputPosX();
	RealType getOutputPosY();
protected:
	//Initialization routines
	//Unit normalize a template, source and destination must be seperate
	void NCC_NormalizeTemplate(MatrixType &src, MatrixType &dst);

	//Online Routines
	//Calculate the energy under the source. Has pre-allocation.
	void NCC_Energy(MatrixType &src, MatrixType &dst);
	//Standard normalized cross-corelation
	void NCC_Standard(MatrixType &srcI, MatrixType &srcT, MatrixType &energy, MatrixType &dst);

	//Other helper routines
#if CISST_SVL_HAS_OPENCV
	//move the search window to the source image if near the edge (uses opencv)
	void checkRectangleBoundries_opencv(ImageType & src, PointType & size, PointType & position);
	//Copy a input image into buffer matrix (uses opencv)
	void copyImage_opencv(ImageType &src, MatrixType &dst, IndexType startR = 0, IndexType startC = 0);
		//Copy a buffered image back to an input image (uses opencv)
	void copyImage_opencv(MatrixType &src, ImageType &dst);
#endif // CISST_SVL_HAS_OPENCV
	//move the search window to the source image if near the edge
	void checkRectangleBoundries(PointType & imSize, PointType & size, PointType & position);
	//Copy a input image into buffer matrix
	void copyImage(InterfaceImType *src, IndexType src_rows, IndexType src_cols, MatrixType &dst, IndexType startR = 0, IndexType startC = 0);
	//Copy a input image into buffer matrix (within cisst)
	void copyImage_cisst(MatrixType &src, MatrixType &dst, IndexType startR = 0, IndexType startC = 0);
private:
	//Initialization data
	ConvolutionMethod convMethod;//

	// Current image 
	MatrixType Current_image;

	//Storage of all the choosen templates, normalized, and the same dimentions
	MatrixRefList allTemplates; //
	//Order list of the templates selected, standard or FT domain
	MatrixRefList selectedTemplates; //
	//The index of next template
	IndexType currentTemplateIndex;//
	//The number of templates to search per update
	IndexType templateLookahead;
	
	//Shared scratch data storage. 
	//Sum data requires high precision, i.e. longs or doubles
	MatrixType sharedDataSum;
	MatrixType sharedDataSqSum;
	MatrixType energy; //Smaller than sum, or sqsum, the energy unde rthe template
	//Sub region pointers for scratch data, same size as template Energy
	SubMatrixType sumView;
	SubMatrixType sumsqView;
	SubMatrixType sumTT;
	SubMatrixType sumTB;
	SubMatrixType sumBT;
	SubMatrixType sumBB;
	SubMatrixType sqsumTT;
	SubMatrixType sqsumTB;
	SubMatrixType sqsumBT;
	SubMatrixType sqsumBB;
	//NCC Standard scratch variables
	//Convolution results, Size of source
	MatrixType convoledData;

	//Scratch copy of the image, used in update
	MatrixType imageCopy;
	
	//X = cols, Y = rows
	//Tracked window location
	PointType windowSize; //
	PointType windowPosition; //
	//Last similarity measure
	RealType score;
	// Tracked location in the current image
	PointType CurLoc;
	//The position of the center of the tracking window relative to nncResults
	PointType windowCenter;//
	//Size of all the templates.
	PointType templateSize;//
	//The "center" of the template for convolution purposes
	PointType templateCenter;//
	// Image size (X corresponds to height and Y to width)
	PointType imageSize;
	//The inverse of the area under the template times -2.
	RealType neg_inv_templateArea;//
	// Output Location
	RealType outputPosX;
	RealType outputPosY;
};

#endif // _svlTrackerCisstNCC_h

