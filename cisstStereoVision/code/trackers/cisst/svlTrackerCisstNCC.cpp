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


#include "svlTrackerCisstNCC.h"

#ifdef _MSC_VER
    // Quick fix for Visual Studio Intellisense:
    // The Intellisense parser can't handle the CMN_UNUSED macro
    // correctly if defined in cmnPortability.h, thus
    // we should redefine it here for it.
    // Removing this part of the code will not effect compilation
    // in any way, on any platforms.
    #undef CMN_UNUSED
    #define CMN_UNUSED(argument) argument
#endif


/*********************************/
/*** svlTrackerCisstNCC class ****/
/*********************************/

/********************************************************************
TODO / BUGS
1) Improve Dynamic Loop Engine. *Don't* use iterators. Possibly specific classes 
   for row/col order full matrix
2) Make sure the matrix loop engines, etc. can handle vector element data types 
   (i.e. rgb data)
3) Kernel engine??
   vctDynamicMatrixLoopEngines.h
4) Use the ownertype, element type, etc, in function defs as it allows for 
   Intellisense
5) Add row/colunm/slice? to fixed size vectors, for use in 2d point types
   And being allowed to passed them as appropriate arguments to mats
   i.e. Func(int row, int col) -> Func(vct2 point)
   Alternative is to switch to x,y,z terminalogy and forget row/col completely
6) Explicit unsigned int required on implementation (not declaration) of 
   getNumberOfTemplates()
7) Template NCCTracker for use with different datatypes??
8) Chained fixed vector operations like a = b/2 + c - 3; fail. Can we fix/want to?
9) dst.SumOf(sumTT,sumBB); <- This cannot be chained like other operators.
10) If templated, could implement the different convolution methods that way
    Instead of with a switch
11) Binary Ops (Perhasp all ops) should redefine all typedefs they use,
    as leaving them as inherited can leave to some problems, see findOpLocation code
12) Fast, i.e. less memory usage in fast Integral calculation by using additional views.
13) Method of chaining operands so that there is only one loop.
    - Operand joing, like:
	template < Op1, Op2 > {
		Operate(out,in1,in2,in3) {out = Op1::Operate(in1,Op2::Operate(in2,in3));}
	- Instantiated Operands
	- Loop engines with variable / multiple inputs
	- Structured inputs? Variable inputs? Speed costs?
14) Matrix Watch / Debugger with visulization. Python code?
15) Can't write a functional view (e.g. wapring) as a const matrix due to how 
    vctDynamicConstMatrixBase is written, might look into this.
	Could vctDynamicConstFunctionalMatrix be a template specilization of
	vctDynamicConstMatrix? (Thus solving problems) Though issues if loop engine
	is optimized. The major problem are the initial hard coded typedefs, not
	the functions. Also need to verify MatrixOwnerType code and usage.
16) A owner base class might be nice, given style of Functional matrix owner.
17) See vctFunctionalMatrixIterator.h for a better way of doing Matrix Iterators.
18) _elementType for otherMatrix on operations seem to require
		the same type. We might like that not to be the case. Goes
		for scaler code as well (Passed by ref). Test this.
19) Major change: Make Owner base derived from owner type. This has code reuse benifits,
    And due to typing, different owners already produce different classes.
	Most importantly, it makes the functionals nicer, as the extended base classes shouldn't
	need a seperate class. How would this work across the entire code base?
20) GetU and GetVt routines missing from nmrSVDRSSolver
21) Something like container typedefs but without value_type, etc, for index, stride, etc.
22) Add defines for all basic classes for automated retypedefing.
23) Intermediate storage of warped and copied values instead of a functional matrix??
24) a sqrt(matrix) and sqrt(vector) functions defined, not just function calles like vector.sqrt()
25) A row/column iterator that returns the kernel space position of each feature...?
26) SVD class's use of S matrix is inane. Actually a column vector stored as a matrix...
27) Boolean indexing or masking as an option, etc.
*********************************************************************
TODO with a Star i.e. for this code rev
*  Working properly at 17-18fps(debug), depending on template/window size
A) Fast integral algorithm majorly flawed, determine why.
[X]The integral matrix has to be zero paded by one row
   Use a setAll(0) on a slightly larger matrix, and change the current matrix
   Used for sums to a Reference. Update sumXX, sumsqXX approp.
[X]Find logic bug in the c++ code for energy, appears to be correct in matlab prototype.
[X]Add a unary sq root operation
   e.g. vctUnaryOperations< _outputElementType, _inputElementType >::Square
   Could alternatively square the input matrix (Also needs partial writting)
   Run test to see if this would be significantly faster??
       But there is a sign (+/-) issue to resolve.
[X]Double check that the running integral is working properly with the test routines.
[X] Setup to use more than two templates at once
[X] pre-inverse energy to do the division only once.
[ ] Improve the usage of ThrowException().
Later TODO
[ ]	Switch sum & sqsum to long or ulong. If int input data is used
********************************************************************/

svlTrackerCisstNCC::svlTrackerCisstNCC(void) :
    svlTrackerCisstBase()
{
	convMethod = Standard;
	templateLookahead = 0;
	// Initialize the Current Location field
	CurLoc.X() = 0;
	CurLoc.Y() = 0;
}

svlTrackerCisstNCC::~svlTrackerCisstNCC(void) {
	resetTrack();
}
//Cear stored data.
void svlTrackerCisstNCC::resetTrack() {
	while(!selectedTemplates.empty()) {
		popSelectedTemplate();
	}
	while(!allTemplates.empty()) {
		popTemplate();
	}
	convMethod = Standard;
	templateLookahead = 0;
	// Initialize the Current Location field
	CurLoc.X() = 0;
	CurLoc.Y() = 0;
}

//*******************************************************************
//                 Initialization routines and data
//*******************************************************************
//Select the number of templates to search through, 
//should be at least 2 for multi template use
//Defaults to all if not set
void svlTrackerCisstNCC::setTemplateLookahead(IndexType number) {
	templateLookahead = number;
}
//Select the convolution method to use
void svlTrackerCisstNCC::setConvolutionMethodTo(ConvolutionMethod method) {
	convMethod = method;
}
//Set the size of the template. 
//All templates must be the same size.
void svlTrackerCisstNCC::setTemplateSize(IndexType rows, IndexType cols) {
	templateSize.X() = cols;
	templateSize.Y() = rows;
}
//Set the center offset of the template.
void svlTrackerCisstNCC::setTemplateCenter(IndexType r, IndexType c) {
	templateCenter.X() = c;
	templateCenter.Y() = r;
}
//Set the tracking window size
void svlTrackerCisstNCC::setWindowSize(IndexType rows, IndexType cols) {
	windowSize.X() = cols;
	windowSize.Y() = rows;
}
// Set the Image Size
void svlTrackerCisstNCC::setImageSize(IndexType rows, IndexType cols) {
	imageSize.X() = cols;
	imageSize.Y() = rows;
}
//Set the center offset of the tracking window
void svlTrackerCisstNCC::setWindowCenter(IndexType r, IndexType c) {
	windowCenter.X() = c;
	windowCenter.Y() = r;
}
//Set the initial position of the tracking window
void svlTrackerCisstNCC::setWindowPosition(IndexType r, IndexType c) {
	windowPosition.X() = c;
	windowPosition.Y() = r;
}
// Set the initial position for tracking (empty in this case)
void svlTrackerCisstNCC::setInitPosition(IndexType CMN_UNUSED(r), IndexType CMN_UNUSED(c)) {
}
// Set the size of the current image
void svlTrackerCisstNCC::setCurrentimageSize(IndexType rows, IndexType cols) {
	Current_image.SetSize(rows,cols, VCT_COL_MAJOR);
}

//Get image location
svlTrackerCisstNCC::IndexType svlTrackerCisstNCC::getWindowRow() { return windowPosition.Y(); }
svlTrackerCisstNCC::IndexType svlTrackerCisstNCC::getWindowCol() { return windowPosition.X(); }
//Get the correlation score, bounded [-1,1]	
double svlTrackerCisstNCC::getTrackScore() { return score; }
//Get template dimentions
svlTrackerCisstNCC::IndexType svlTrackerCisstNCC::getTemplateNumRows() { return templateSize.Y(); }
svlTrackerCisstNCC::IndexType svlTrackerCisstNCC::getTemplateNumCols() { return templateSize.X(); }
//Get template center location
svlTrackerCisstNCC::IndexType svlTrackerCisstNCC::getTemplateCenterRow() { return templateCenter.Y(); }
svlTrackerCisstNCC::IndexType svlTrackerCisstNCC::getTemplateCenterCol() { return templateCenter.X(); }
//Get the tracking window size
svlTrackerCisstNCC::IndexType svlTrackerCisstNCC::getWindowNumRows() { return windowSize.Y(); }
svlTrackerCisstNCC::IndexType svlTrackerCisstNCC::getWindowNumCols() { return windowSize.X(); }
//Get the relative center position of the window
svlTrackerCisstNCC::IndexType svlTrackerCisstNCC::getWindowCenterRow() { return windowCenter.Y(); }
svlTrackerCisstNCC::IndexType svlTrackerCisstNCC::getWindowCenterCol() { return windowCenter.X(); }
//Get the current image size
svlTrackerCisstNCC::IndexType svlTrackerCisstNCC::getCurrentimageNumRows() { return imageSize.Y(); }
svlTrackerCisstNCC::IndexType svlTrackerCisstNCC::getCurrentimageNumCols() { return imageSize.X(); }
//Get the current (tracked) location
double svlTrackerCisstNCC::getOutputPosX() {return outputPosX;}
double svlTrackerCisstNCC::getOutputPosY() {return outputPosY;}

#if CISST_SVL_HAS_OPENCV

//Copy a input image into buffer matrix
void svlTrackerCisstNCC::copyImage_opencv(ImageType &src, MatrixType &dst, IndexType startR, IndexType startC) {
	for(IndexType r = 0; r < dst.rows(); r++) {
		for(IndexType c = 0; c < dst.cols(); c++) {
			dst.Element(r,c) = cvGetReal2D(src,startR+r,startC+c);
		}
	}
}

//Copy a buffered image back to an input image (uses opencv)
void svlTrackerCisstNCC::copyImage_opencv(MatrixType &src,ImageType &dst) {
	for(IndexType r = 0; r < src.rows(); r++) {
		for(IndexType c = 0; c < src.cols(); c++) {
			cvSetReal2D(dst, r, c, src.Element(r,c));
		}
	}
}

//Add another template to the tracker (uses opencv)
void svlTrackerCisstNCC::pushTemplate_opencv(ImageType &src, IndexType r, IndexType c) {
	//Create a new matrix of the right size
	if(templateSize.X() <= 0 || templateSize.Y() <= 0){
		cmnThrow(std::runtime_error("svlTrackerCisstNCC pushtemplate_opencv - degenerate template size"));
	}
	MatrixType * temp  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
	MatrixType * temp2 = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
	//Copy in the image data
	copyImage_opencv(src, *temp, r - templateCenter.Y(), c - templateCenter.X());
	//Normalize the template and fill the allocated data
	NCC_NormalizeTemplate(*temp,*temp2);
	//Add the template to the list
	allTemplates.push_back(temp2);
	delete temp;
}

//Copy a choosen template to an image (uses opencv)
void svlTrackerCisstNCC::getTemplate_opencv(IndexType index, ImageType &dst) {
	if( index >= getNumberOfTemplates() ) {
		cmnThrow(std::runtime_error("svlTrackerCisstNCC getTemplate_opencv - index invalid"));
	}
	copyImage_opencv(*allTemplates[index],dst);
}

//move the search window to the source image if near the edge (uses opencv)
void svlTrackerCisstNCC::checkRectangleBoundries_opencv(ImageType & src, PointType & size, PointType & position) {
	CvSize srcsize = cvGetSize(src);
	//Note: 1.0 to type convert and prevent signed / unsigned issues
	if(size.Y()+position.Y() > 1.0*srcsize.height) {
		position.Y() = srcsize.height - size.Y();
	} else if (position.Y() > 1.0*srcsize.height) { 
		position.Y() = 0;
	}
	if(size.X()+position.X() > 1.0*srcsize.width) {
		position.X() = srcsize.width - size.X();
	} else if (position.X() > 1.0*srcsize.width) { 
		position.X() = 0;
	}
//	if(position.Y() < 0)
//		position.Y() = 0;
//	if(position.X() < 0)
//		position.X() = 0;
}
//Update the current window position (uses opencv)
void svlTrackerCisstNCC::updateTrack_opencv(ImageType  & src) {
	RealType  currentMax = -2; //Note a threoretical minimum of -1 for NCC
	RealType  nextMax;
	PointType currentLoc(0, 0);
	PointType nextLoc;
	//Make a copy of the input data with ROI selection
	checkRectangleBoundries_opencv(src,windowSize,windowPosition);
	copyImage_opencv(src,imageCopy,windowPosition.Y(),windowPosition.X());
	//Choose the correct correlation method
	switch(convMethod) {
		case Auto:
		case Standard:
			NCC_Energy(imageCopy,energy);
			for(IndexType i = 0, index = currentTemplateIndex; i < templateLookahead;
			i++, ++index >= selectedTemplates.size() ? index -= selectedTemplates.size() : index) {
				NCC_Standard(imageCopy,*selectedTemplates[index],energy,convoledData);
				findMax(convoledData,nextLoc.Y(),nextLoc.X(),nextMax);
				if(nextMax >= currentMax) {
					currentMax = nextMax;
					currentTemplateIndex = index;
					currentLoc = nextLoc;
				}
			}
			break;
		case FFT:
			throw std::runtime_error("FFT Not Implemented Yet.");
			break;
	}
	score = currentMax;
	CurLoc = currentLoc;
	windowPosition.Add(currentLoc).Subtract(windowCenter).Add(templateCenter);
}

#endif // CISST_SVL_HAS_OPENCV

//Copy a input image into buffer matrix (within cisst)
void svlTrackerCisstNCC::copyImage_cisst(MatrixType &src, MatrixType &dst, IndexType startR, IndexType startC) {
	for(IndexType r = 0; r < dst.rows(); r++) {
		for(IndexType c = 0; c < dst.cols(); c++) {
			dst.Element(r,c) = src.Element(startR+r,startC+c);
		}
	}
}
//Copy a input image into buffer matrix
void svlTrackerCisstNCC::copyImage(InterfaceImType *src, IndexType CMN_UNUSED(src_rows), IndexType src_cols, MatrixType &dst, IndexType startR, IndexType startC) {
	for(IndexType r = 0; r < dst.rows(); r++) {
		for(IndexType c = 0; c < dst.cols(); c++) {
			dst.Element(r,c) = src[(startR+r)*src_cols+(startC+c)];
		}
	}
}
// Copy the current image 
void svlTrackerCisstNCC::copyCurrentImage(InterfaceImType *src, IndexType src_rows, IndexType src_cols){
	copyImage(src, src_rows, src_cols, Current_image);
}

//Unit normalize a template, source and destination must be seperate
void svlTrackerCisstNCC::NCC_NormalizeTemplate(MatrixType &src, MatrixType &dst) {
	//t-t_bar
	dst.DifferenceOf(src, src.SumOfElements() / src.size() );
	// (t-t_bar)^2
	dst.ElementwiseMultiply(dst);
	// sum[(t-t_bar)^2]
	RealType sum = sqrt( dst.SumOfElements() );
	//(t-t_bar)/sqrt(len)
	dst.DifferenceOf(src, src.SumOfElements() / src.size() );
	dst.Divide(sum);
}

// Add another template to the tracker (from the src image)
void svlTrackerCisstNCC::pushTemplate(InterfaceImType *src, IndexType src_rows, IndexType src_cols, IndexType r, IndexType c) {
	//Create a new matrix of the right size
	if(templateSize.X() <= 0 || templateSize.Y() <= 0){
		cmnThrow(std::runtime_error("svlTrackerCisstNCC pushTemplate - degenerate template size"));
	}
	MatrixType * temp  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
	MatrixType * temp2 = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
	//Copy in the image data
	copyImage(src, src_rows, src_cols, *temp, r - templateCenter.Y(), c - templateCenter.X());
	//Normalize the template and fill the allocated data
	NCC_NormalizeTemplate(*temp,*temp2);
	//Add the template to the list
	allTemplates.push_back(temp2);
	delete temp;
}
// Add another template by copying to the list of the template
void svlTrackerCisstNCC::pushTemplate(InterfaceImType *src, IndexType src_rows, IndexType src_cols) {
	//Create a new matrix of the right size
	if(templateSize.X() <= 0 || templateSize.Y() <= 0){
		cmnThrow(std::runtime_error("svlTrackerCisstNCC pushTemplate - degenerate template size"));
	}
	MatrixType * temp  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
	MatrixType * temp2 = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
	//Copy in the image data
	copyImage(src, src_rows, src_cols, *temp);
	//Normalize the template and fill the allocated data
	NCC_NormalizeTemplate(*temp,*temp2);
	//Add the template to the list
	allTemplates.push_back(temp2);
#ifdef IS_LOGGING
	CMN_LOG_INIT_ERROR << " template : "<<endl;
	CMN_LOG_INIT_ERROR << temp->Row(0) <<endl;
	CMN_LOG_INIT_ERROR << temp->Column(0) <<endl;
#endif
	delete temp;
}

//Remove last template from the tracker
void svlTrackerCisstNCC::popTemplate() {
	if(allTemplates.empty()) {return;}
	delete allTemplates.back();
	allTemplates.pop_back();
}

//Get the number of templates loaded
//Explicit unsigned int required, can't use the typedef in MS.Net
svlTrackerCisstNCC::IndexType svlTrackerCisstNCC::getNumberOfTemplates() {
	return allTemplates.size();
}

//Copy a choosen template to an image
//void svlTrackerCisstNCC::getTemplate(IndexType index, MatrixType &dst) {
//	if( index<0 && index >= getNumberOfTemplates() ) {
//		ThrowException();//If invalid index
//	}
//	copyImage(*allTemplates[index],dst);
//}
//Add a loaded template to the end of the selected templates list
void svlTrackerCisstNCC::pushSelectedTemplate(IndexType index) {
	selectedTemplates.push_back(allTemplates[index]);
}
//Remove the last selected template
void svlTrackerCisstNCC::popSelectedTemplate() {
	selectedTemplates.pop_back();
}
//*******************************************************************
//                 Tracking code
//*******************************************************************

//move the search window to the source image if near the edge 
void svlTrackerCisstNCC::checkRectangleBoundries(PointType & imSize, PointType & size, PointType & position) {
	// Note: X field of imSize corresponds to height (cols) and Y field to width (rows)
	if(size.Y()+position.Y() > imSize.X()) {
		position.Y() = imSize.X() - size.Y();
	} else if (position.Y() > imSize.X()) { 
		position.Y() = 0;
	}
	if(size.X()+position.X() > imSize.Y()) {
		position.X() = imSize.Y() - size.X();
	} else if (position.X() > imSize.Y()) { 
		position.X() = 0;
	}
	if(position.Y() <= 0)
		position.Y() = 0;
	if(position.X() <= 0)
		position.X() = 0;
}
//Inititalize the pre-allocated data for tracking
void svlTrackerCisstNCC::initializeTrack() {
	if(templateLookahead==0) {
		templateLookahead = selectedTemplates.size();
	}
	currentTemplateIndex = 0;

	if( selectedTemplates.size() < templateLookahead ) {
		// make template look ahead the same size as the size of selected template
		templateLookahead = selectedTemplates.size();
		
	}
	if( templateSize.X()<=0 || templateSize.Y()<=0 ) {
		cmnThrow(std::runtime_error("svlTrackerCisstNCC initializeTrack() function - degenerate template size"));
	}
	if( windowSize.X() < templateSize.X() && windowSize.Y() < templateSize.Y() ) {
		cmnThrow(std::runtime_error("svlTrackerCisstNCC initializeTrack() function - window size should be larger than template size"));
	}

	//Define the subwindow size for easy of use.
	//WishList: Be able to declare: PointType subWindowSize = windowSize - templateSize + 1;
	PointType subWindowSize = windowSize;
	subWindowSize.Subtract(templateSize).Add(1);

	neg_inv_templateArea = -1.0 / (templateSize.X()*templateSize.Y());

	//Resize fixed matrixes
	sharedDataSum.SetSize(windowSize.Y()+1, windowSize.X()+1, VCT_COL_MAJOR);
	sharedDataSqSum.SetSize(windowSize.Y()+1, windowSize.X()+1, VCT_COL_MAJOR);
	sharedDataSum.SetAll(0);
	sharedDataSqSum.SetAll(0);
	//Limited views of the sum and sumsq data
	sumView.SetRef(sharedDataSum,1,1,windowSize.Y(),windowSize.X());
	sumsqView.SetRef(sharedDataSqSum,1,1,windowSize.Y(),windowSize.X());

	energy.SetSize(subWindowSize.Y(), subWindowSize.X(), VCT_COL_MAJOR);

	convoledData.SetSize(subWindowSize.Y(), subWindowSize.X(), VCT_COL_MAJOR);

	//A default "centered" window center.
	windowCenter.Assign(windowSize).Divide(2);

	//This is to allow matrix calculation of sum and sqsum values for energy calculation
	sumTT.SetRef(    sharedDataSum,templateSize.Y(),templateSize.X(),subWindowSize.Y(),subWindowSize.X());
	sumTB.SetRef(    sharedDataSum,templateSize.Y(),0               ,subWindowSize.Y(),subWindowSize.X());
	sumBT.SetRef(    sharedDataSum,0               ,templateSize.X(),subWindowSize.Y(),subWindowSize.X());
	sumBB.SetRef(    sharedDataSum,0               ,0               ,subWindowSize.Y(),subWindowSize.X());
	sqsumTT.SetRef(sharedDataSqSum,templateSize.Y(),templateSize.X(),subWindowSize.Y(),subWindowSize.X());
	sqsumTB.SetRef(sharedDataSqSum,templateSize.Y(),0               ,subWindowSize.Y(),subWindowSize.X());
	sqsumBT.SetRef(sharedDataSqSum,0               ,templateSize.X(),subWindowSize.Y(),subWindowSize.X());
	sqsumBB.SetRef(sharedDataSqSum,0               ,0               ,subWindowSize.Y(),subWindowSize.X());

	//Sratch image data;
	imageCopy.SetSize(windowSize.Y(), windowSize.X(), VCT_COL_MAJOR);
#ifdef IS_LOGGING
	CMN_LOG_INIT_ERROR << "windowSize :" <<endl;
	CMN_LOG_INIT_ERROR << windowSize << endl;
	CMN_LOG_INIT_ERROR << "windowCenter :" <<endl;
	CMN_LOG_INIT_ERROR << windowCenter << endl;
	CMN_LOG_INIT_ERROR << "windowPosition :" <<endl;
	CMN_LOG_INIT_ERROR << windowPosition << endl;
	CMN_LOG_INIT_ERROR << "templateSize :" <<endl;
	CMN_LOG_INIT_ERROR << templateSize << endl;
	CMN_LOG_INIT_ERROR << "templateCenter :" <<endl;
	CMN_LOG_INIT_ERROR << templateCenter << endl;
#endif
}
//Calculate the energy under the source. Has pre-allocation.
void svlTrackerCisstNCC::NCC_Energy(MatrixType &src, MatrixType &dst) {
	runIntegral(src, sumView, sumsqView);

	//Calculate f_sum   =   sum(uT,vT) -   sum(uB,vT) -   sum(uT,vB) +   sum(uB,vB);
	dst.SumOf(sumTT,sumBB);
	dst.Subtract(sumTB).Subtract(sumBT);

	//- inv_templateArea * f_sum * f_sum
	dst.ElementwiseMultiply(dst).Multiply(neg_inv_templateArea);
	//energy = f_sqsum - 2 * inv_templateArea * f_sum * f_sum
	//f_sqsum = sqsum(uT,vT) - sqsum(uB,vT) - sqsum(uT,vB) + sqsum(uB,vB)
	dst.Add(sqsumTT).Subtract(sqsumTB).Subtract(sqsumBT).Add(sqsumBB);
	SquareRootSelf(dst);
	//Pre-inverse the energy
	dst.RatioOf(1.0,dst);
}
//Standard normalized cross-corelation
void svlTrackerCisstNCC::NCC_Standard(MatrixType &srcI, MatrixType &srcT, MatrixType &energy, MatrixType &dst) {
	//A: Perform convolution of srcI and srcT
	correlation(srcI, srcT, dst);
	//B: Multiply by the shared data (inverse of the denominator)
	dst.ElementwiseMultiply(energy);
}

//Update the current window position
void svlTrackerCisstNCC::updateTrack() {
	RealType  currentMax = -2; //Note a threoretical minimum of -1 for NCC
	RealType  nextMax;
	PointType currentLoc(0, 0);
	PointType nextLoc;
	//Make a copy of the input data with ROI selection
	checkRectangleBoundries(imageSize,windowSize,windowPosition);
	copyImage_cisst(Current_image,imageCopy,windowPosition.Y(),windowPosition.X());
#ifdef IS_LOGGING
	CMN_LOG_INIT_ERROR << " current image : "<<endl;
	CMN_LOG_INIT_ERROR << imageCopy.Row(0) <<endl;
	CMN_LOG_INIT_ERROR << imageCopy.Column(0) <<endl;
#endif
	//Choose the correct correlation method
	switch(convMethod) {
		case Auto:
		case Standard:
			NCC_Energy(imageCopy,energy);
			for(IndexType i = 0, index = currentTemplateIndex; i < templateLookahead;
			i++, ++index >= selectedTemplates.size() ? index -= selectedTemplates.size() : index) {
				NCC_Standard(imageCopy,*selectedTemplates[index],energy,convoledData);
				findMax(convoledData,nextLoc.Y(),nextLoc.X(),nextMax);
				if(nextMax >= currentMax) {
					currentMax = nextMax;
					currentTemplateIndex = index;
					currentLoc = nextLoc;
				}
			}
			break;
		case FFT:
			throw std::runtime_error("FFT Not Implemented Yet.");
			break;
	}
	score = currentMax;
	CurLoc = currentLoc;
	windowPosition.Add(currentLoc).Subtract(windowCenter).Add(templateCenter);
	outputPosX = getWindowCol() + getWindowCenterRow();
	outputPosY = getWindowRow() + getWindowCenterRow();
#ifdef IS_LOGGING
	CMN_LOG_INIT_ERROR<< "I'm in updateTrack()"<<endl;
	CMN_LOG_INIT_ERROR << "CurLoc :" <<endl;
	CMN_LOG_INIT_ERROR << CurLoc << endl;
	CMN_LOG_INIT_ERROR << "windowPosition :" <<endl;
	CMN_LOG_INIT_ERROR << windowPosition << endl;
#endif
}
/********************************************************************
                   Kernel Test Routines
********************************************************************/

#if 0

#include "vctDynamicFunctionalMatrixOwnerWrapper.h"
#include "vctFunctionalMatrixIterator.h"
#include "vctDynamicFunctionalMatrixOwnerRef.h"
#include "vctDynamicFunctionalMatrixOwner.h"
#include "vctDynamicFunctionalMatrixBase.h"
#include "vctWarpFunctionMatrix.h"
#include "cvcTerminationCriteria.h"
#include "cvcSSD.h"
#include "cvcBasic2DKernel_XYRoof.h"
#include "cvcFeatureLifter.h"
#include "cvcBasicFeatureLifter.h"
#include "cvcBinnedFeatures.h"
#include "cvcBinnedFeaturesBasic.h"
#include "cvcLinearFeatureBinner.h"
#include "cvcKernel.h"
#include "cvcBasic2DKernel.h"
#include "cvcSSD_Kernel.h"

//Cisst Library Test Routines
void svlTrackerCisstNCC::test() {
	try{
	typedef vctDynamicMatrix < PointType > TestType;

	IndexType rows = 6;
	IndexType cols = 6;
	IndexType rowsC = 5;
	IndexType colsC = 5;
	IndexType rowsK = 2;
	IndexType colsK = 2;
	
	// here we create the 3x3 matrix ( column major !!)
	MatrixType m(rows , cols , VCT_COL_MAJOR );
	MatrixType n(rows , cols , VCT_COL_MAJOR );
	MatrixType sum(rows , cols , VCT_COL_MAJOR );
	MatrixType sqsum(rows , cols , VCT_COL_MAJOR );
	MatrixType result(rowsC , colsC , VCT_COL_MAJOR );
	MatrixType kernel(rowsK , colsK , VCT_COL_MAJOR );

	typedef vctDynamicVector< double > VectorType;
	VectorType p(rows);

/*	//New matrix test stuff
	vctFunctionalMatrixConstIterator<MatrixType> test_FMCI(&kernel);
	vctDynamicFunctionalMatrixOwner<MatrixType>  test_DFMO;
	
	typedef vctDynamicFunctionalMatrixOwner<MatrixType> fm_Owner;
	fm_Owner test_DFMO1;
	
	vctDynamicFunctionalMatrixOwnerRef<MatrixType> test_DFMOR;
	vctDynamicFunctionalMatrixOwnerWrapper<MatrixType> test_FMOW;
	
	typedef vctDynamicFunctionalMatrixBase< fm_Owner > fm_Type;
	fm_Type test_DFMB;

	typedef vctDynamicVector< double > vectorType;
	//New Kernel test stuff
	cvcTerminationCriteria_Norm<vectorType> test_TC;
	typedef vctWarpFunctionMatrixOwner<MatrixType> WFMO_Type;
	vctWarpFunctionMatrixOwner<MatrixType> test_WFMO;
	typedef vctDynamicFunctionalMatrixBase< WFMO_Type > WFM_Type;
	WFM_Type test_WFM;
	cvcSSD test_SSD;
	//cvcBasic2DKernel_XYRoof<MatrixType,double> test_B2DKXYR;
	typedef cvcFeatureLifter<WFM_Type::value_type,MatrixType::value_type> FL_Type;
	
	typedef cvcBinnedFeatures<MatrixType, WFM_Type::const_iterator > test_BF;
	typedef cvcBinnedFeaturesBasic< FL_Type, MatrixType, WFM_Type::const_iterator > test_BFB;
	typedef cvcLinear1DFeatureBinner< FL_Type, MatrixType, WFM_Type::const_iterator > LFB_Type;
	LFB_Type test_L1DFB;
//	typedef vctDynamicMatrix< double > MatrixType2;
//	cvcLinear1DFeatureBinner< FL_Type,MatrixType2,MatrixType2> test_L1DFB;
	typedef cvcKernel<WFM_Type,MatrixType,vectorType,MatrixType,MatrixType,vectorType,vectorType> K_Type;
	typedef cvcBasic2DKernel<MatrixType> BK_type;
	typedef cvcBasic2DKernel_XYRoof<MatrixType> BKR_Type;
	BKR_Type test_BKR;
	typedef cvcSSD_Kernel<vectorType,MatrixType,vectorType,BKR_Type> SSDK_Type;
	SSDK_Type test_SSDK;

	vectorType params;
	params.SetSize(2);

	//test_WFM
	//test_SSD,test_SSDK,test_BKR,test_L1DFB,test_TC
	//test_WFM.
	test_WFM.SetRef(5,5,m);
	test_WFM.SetAngleAndScale(0,1);
	test_WFM.SetCenter(2.5,2.5);
	test_WFM.SetPosition(params.Y(),params.X());

	test_TC.SetThreshold(0.5);

	test_L1DFB.SetNumberOfBins(32);
	test_L1DFB.SetBounds(256,0);
	
	test_BKR.SetData(test_WFM);
	test_BKR.SetBinFunction( test_L1DFB );
	test_BKR.InitializeKernel(params);

	test_SSDK.SetKernel(test_BKR);
	
	test_SSD.SetFunction(test_SSDK);
	test_SSD.Initialize(params);
	test_SSD.SetTerminator(test_TC);
	test_SSD.SetInitialCondition( test_SSDK.GetX() );

	test_SSD.Optimize();






*/

//TODO MAKE KERNEL to SVD FUNCITON Converter


	//Old ExtraTemplateFunctions test
	TestType multi(rowsK , colsK , VCT_COL_MAJOR );

	
	svlTrackerCisstNCC::IndexType indexRow , indexColumn ;
	for ( indexRow = 0; indexRow < rowsK ; ++ indexRow ) {
		for ( indexColumn = 0; indexColumn < colsK ; ++ indexColumn ) {
			kernel.at( indexRow , indexColumn ) = indexRow;
			multi.Element( indexRow , indexColumn ).X() = indexRow;
			multi.Element( indexRow , indexColumn ).Y() = indexColumn;
		}
	}

	for ( indexRow = 0; indexRow < rows ; ++ indexRow ) {
		p.at(indexRow) = indexRow;
		for ( indexColumn = 0; indexColumn < cols ; ++ indexColumn ) {
			// values are between -10 and 10
			if ( indexRow >= indexColumn) {
				m.at( indexRow , indexColumn ) = indexRow;
				n.at( indexRow , indexColumn ) = indexColumn;
			} else {
				m.at( indexRow , indexColumn ) = 0;
				n.at( indexRow , indexColumn ) = 0;
			}
		}
	}

	cout << "m" << endl << m << endl;
	SquareRootSelf(m);
	cout << "m sqrt" << endl << m << endl;
	cout << "p" << endl << p << endl;
	SquareRootSelf(p);
	cout << "p sqrt" << endl << p << endl;


return;
	MatrixType::iterator i = m.begin();
	MatrixType::iterator j = n.begin();
	for ( ; i != m.end(); ++i, ++j) {
		cout << *i << *j<<endl;
	}


	// display the current matrix
	cout << " Matrix M :" << endl << m << endl ;
	cout << " Matrix N :" << endl << n << endl ;

	cout << " Correlation(m,n) : " << SumOfElementwiseProductOf(m,n) << endl ;
	cout << " Correlation(n,n) : " << SumOfElementwiseProductOf(n,n) << endl ;
	cout << " Correlation(m,m) : " << SumOfElementwiseProductOf(m,m) << endl ;

	runIntegral(m,sum,sqsum);

	cout << "   sum(m) : " <<endl<< sum << endl;
	cout << " sqsum(m) : " <<endl<< sqsum << endl;

	runIntegral(n,sum,sqsum);

	cout << "   sum(n) : " <<endl<< sum << endl;
	cout << " sqsum(n) : " <<endl<< sqsum << endl;
	
	cout << "Kernel: " << endl << kernel << endl;
	correlation(m,kernel,result);
	cout << "Result: " << endl << result << endl;
	
	typedef MatrixType::Submatrix::Type SubMatrix;
	SubMatrix subM(m,0,0,2,2);
	cout << "sub" << endl << subM << endl;
	subM.SetRef(subM.rows(), subM.cols(), subM.row_stride(), subM.col_stride(), &m.at(1,1));
	cout << "sub" << endl << subM << endl;

	RealType v;
	IndexType c;
	IndexType r;
	m.ElementwiseMultiply(n);
	findMax(m,r,c,v);
	cout << "M : " << endl << m << endl;
	cout << "Max : " << v << " at [" << r << "," << c << "]." << endl;

	cout << "Multi channel" << endl;
	cout << multi << endl;

	} catch (std::exception exception) {
        cout << "Exception received: " << exception.what() << cout;
	}
}


#endif

