/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Maneesh Dewan (?)
  Created on: 2007 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include "svlTrackerCisstWSSD.h"

// Temporary fix for ambiguous 'real' type declaration in cisstNetlib
#define real _real
#include <cisstNumerical/nmrLSqLin.h>

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


/**********************************/
/*** svlTrackerCisstWSSD class ****/
/**********************************/

svlTrackerCisstWSSD::svlTrackerCisstWSSD(svlTrackerCisstWSSD::Model trackmodel) :
    svlTrackerCisstBase()
{
	templateLookahead = 0;
	trackmodelused = trackmodel;
	numIter = 3;
	switch(trackmodelused){
		case TRANS:
			C.SetSize(2);
			C.SetAll(0);
			break;
		case TRANS_ROT:
			C.SetSize(3);
			C.SetAll(0);
			break;
		case TRANS_SCALE:
			C.SetSize(3);
			C.SetAll(0);
			break;
		case TRANS_ROT_SCALE:
			C.SetSize(4);
			C.SetAll(0);
			break;
		default:
			// default size is 2 (i.e. TRANS model)
			C.SetSize(2);
	}
	// Initialize the Current Location field, angle and scale
	CurLoc.X() = 0;
	CurLoc.Y() = 0;
	CurAngle = 0;
	CurScale = 1;

	// Create the X and Y derivative gaussian filters
	X_der_filter.SetSize(5,7,VCT_COL_MAJOR);
	X_der_filter.Assign(-0.00720940918123,   -0.01404201167817,   -0.01032694122157, 0.0, 0.01032694122157, 0.01404201167817,  0.00720940918123,
    -0.01404201167817,   -0.02735010415047,   -0.02011413495722, 0.0,  0.02011413495722,  0.02735010415047,  0.01404201167817,
    -0.01753635040280,   -0.03415614663539,   -0.02511951469227, 0.0,  0.02511951469227,  0.03415614663539,  0.01753635040280,
    -0.01404201167817,   -0.02735010415047,   -0.02011413495722, 0.0,  0.02011413495722,  0.02735010415047,  0.01404201167817,
    -0.00720940918123,   -0.01404201167817,   -0.01032694122157, 0.0,  0.01032694122157,  0.01404201167817,  0.00720940918123);
	
	Y_der_filter.SetSize(7,5,VCT_COL_MAJOR);
	Y_der_filter.Assign(-0.00720940918123,   -0.01404201167817,   -0.01753635040280,   -0.01404201167817,   -0.00720940918123,
   -0.01404201167817,   -0.02735010415047,   -0.03415614663539,   -0.02735010415047,  -0.01404201167817,
   -0.01032694122157,   -0.02011413495722,   -0.02511951469227,   -0.02011413495722,   -0.01032694122157,
    0.0, 0.0, 0.0, 0.0, 0.0,
	0.01032694122157,  0.02011413495722,  0.02511951469227,  0.02011413495722,  0.01032694122157,
	0.01404201167817,  0.02735010415047,  0.03415614663539,  0.02735010415047,  0.01404201167817,
	0.00720940918123,  0.01404201167817,  0.01753635040280,  0.01404201167817,  0.00720940918123);

	der_filter_offset = 6; // how much the size of the template to be increased
}

svlTrackerCisstWSSD::~svlTrackerCisstWSSD(void) {
	resetTrack();
}
//Clear stored data.
void svlTrackerCisstWSSD::resetTrack() {
	while(!selectedTemplates.empty()) {
		popSelectedTemplate();
	}
	while(!allTemplates.empty()) {
		popTemplate();
	}
	while(!derTemplates.empty()){
		derTemplates.pop_back();
	}
	// reset the values that were set in the constructor
	templateLookahead = 0;
	trackmodelused = TRANS;
	numIter = 3;
	// Initialize the Current Location field, angle and scale
	CurLoc.X() = 0;
	CurLoc.Y() = 0;
	CurAngle = 0;
	CurScale = 1;

#ifdef IS_LOGGING
	CMN_LOG_INIT_ERROR<<" I'm finished with resetTrack()"<<endl;
#endif

}
//Select the number of templates to search through, 
//should be at least 2 for multi template use
//Defaults to all if not set
void svlTrackerCisstWSSD::setTemplateLookahead(IndexType number) {
	templateLookahead = number;
}
//Set the size of the template. 
//All templates must be the same size.
void svlTrackerCisstWSSD::setTemplateSize(IndexType rows, IndexType cols) {
	templateSize.X() = cols;
	templateSize.Y() = rows;
}
//Set the center offset of the template.
void svlTrackerCisstWSSD::setTemplateCenter(IndexType row, IndexType col) {
	templateCenter.X() = col;
	templateCenter.Y() = row;
}
//Set the tracking window size
void svlTrackerCisstWSSD::setWindowSize(IndexType CMN_UNUSED(rows), IndexType CMN_UNUSED(cols)) {
}
//Set the center offset of the tracking window
void svlTrackerCisstWSSD::setWindowCenter(IndexType CMN_UNUSED(r), IndexType CMN_UNUSED(c)) {
}
//Set the initial position of the tracking window
void svlTrackerCisstWSSD::setWindowPosition(IndexType CMN_UNUSED(r), IndexType CMN_UNUSED(c)) {
}
// Set the intiial position for tracking
void svlTrackerCisstWSSD::setInitPosition(IndexType r, IndexType c)
{
	initPos.X() = c;
	initPos.Y() = r;
}
// Set the size of the current image
void svlTrackerCisstWSSD::setCurrentimageSize(IndexType rows, IndexType cols) {
	Current_image.SetSize(rows,cols, VCT_COL_MAJOR);
}
// Set the Image Size
void svlTrackerCisstWSSD::setImageSize(IndexType rows, IndexType cols) {
	imageSize.X() = cols;
	imageSize.Y() = rows;
}

//Get the correlation score, bounded [-1,1]	
double svlTrackerCisstWSSD::getTrackScore() { return 1; }
//Get template dimentions
svlTrackerCisstWSSD::IndexType svlTrackerCisstWSSD::getTemplateNumRows() { return templateSize.Y(); }
svlTrackerCisstWSSD::IndexType svlTrackerCisstWSSD::getTemplateNumCols() { return templateSize.X(); }
//Get template center location
svlTrackerCisstWSSD::IndexType svlTrackerCisstWSSD::getTemplateCenterRow() { return static_cast<IndexType>(templateCenter.Y()+0.5); }
svlTrackerCisstWSSD::IndexType svlTrackerCisstWSSD::getTemplateCenterCol() { return static_cast<IndexType>(templateCenter.X()+0.5); }
//Get the current image size
svlTrackerCisstWSSD::IndexType svlTrackerCisstWSSD::getCurrentimageNumRows() { return imageSize.Y(); }
svlTrackerCisstWSSD::IndexType svlTrackerCisstWSSD::getCurrentimageNumCols() { return imageSize.X(); }
//Get the current (tracked) location
double svlTrackerCisstWSSD::getOutputPosX() {return outputPosX;}
double svlTrackerCisstWSSD::getOutputPosY() {return outputPosY;}

#if CISST_SVL_HAS_OPENCV

//Copy a input image into buffer matrix
void svlTrackerCisstWSSD::copyImage_opencv(ImageType &src, MatrixType &dst, IndexType startR, IndexType startC) {
	for(IndexType r = 0; r < dst.rows(); r++) {
		for(IndexType c = 0; c < dst.cols(); c++) {
			dst.Element(r,c) = cvGetReal2D(src,startR+r,startC+c);
		}
	}
}

//Copy a buffered image back to an input image (uses opencv)
void svlTrackerCisstWSSD::copyImage_opencv(MatrixType &src,ImageType &dst) {
	for(IndexType r = 0; r < src.rows(); r++) {
		for(IndexType c = 0; c < src.cols(); c++) {
			cvSetReal2D(dst, r, c, src.Element(r,c));
		}
	}
}

//Add another template to the tracker (uses opencv)
void svlTrackerCisstWSSD::pushTemplate_opencv(ImageType &src, IndexType r, IndexType c) {
	//Create a new matrix of the right size
	if(templateSize.X() <= 0 || templateSize.Y() <= 0){
		cmnThrow(std::runtime_error("svlTrackerCisstWSSD initializeTrack() function - degenerate template size"));
	}
	MatrixType * temp  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
	MatrixType * temp2 = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
	//Copy in the image data
	copyImage_opencv(src, *temp, r - static_cast<int>(templateCenter.Y()), c - static_cast<int>(templateCenter.X()));
	//Normalize the template and fill the allocated data
	WSSD_NormalizeTemplate(*temp,*temp2);
	//Add the template to the list
	allTemplates.push_back(temp2);
	delete temp;
}

//Copy a choosen template to an image (uses opencv)
void svlTrackerCisstWSSD::getTemplate_opencv(IndexType index, ImageType &dst) {
	if( index >= getNumberOfTemplates() ) {
		cmnThrow(std::runtime_error("svlTrackerCisstWSSD getTemplate_opencv function - incorrect index number"));
	}
	copyImage_opencv(*allTemplates[index],dst);
}

//Update the current window position
void svlTrackerCisstWSSD::updateTrack_opencv(ImageType &  src)
{
	copyImage_opencv(src,imagecopy);
	WarpedMatrix.SetRef(templateSize.Y(),templateSize.X(),Current_image);

	if(selectedTemplates.size() == 1)
	{
		for(IndexType i = 0; i < numIter; i++)
		{
			WarpedMatrix.SetPosition(CurLoc.Y(),CurLoc.X());
			WarpedMatrix.SetAngleAndScale(CurAngle,CurScale);
			copyCurrentRegion();
			A_lsq.Assign(derTemplates[0]->Pointer());
			Diff_mat.DifferenceOf(current_region,*selectedTemplates[0]);
			Diff_vec.SetRef(current_region.size(),Diff_mat.Pointer());
			B_lsq.Assign(Diff_vec);
			nmrLSqLin(A_lsq,Diff_vec,X_lsq);
			updateParameters();
		}
		
	}
	else
	{
		for(IndexType i = 0; i < numIter; i++)
		{
			WarpedMatrix.SetPosition(CurLoc.Y(),CurLoc.X());
			WarpedMatrix.SetAngleAndScale(CurAngle,CurScale);
			copyCurrentRegion();
			computeCurrentDerivative();
			
			// Add derivatives to the R and Q matrices
			IndexType last_template = selectedTemplates.size()-1;
			VectorRefType tempVec1;
			IndexType cur_index;
			for(IndexType i = 0; i < derCurrent_region.size(); i++)
			{
				cur_index = last_template + i;
                tempVec1.SetRef(current_region.size(), derCurrent_region.Column(i).Pointer());
				R_mat.Column(cur_index).Assign(tempVec1);
				Q_mat.Row(cur_index).Assign(tempVec1);
			}

			Diff_mat.DifferenceOf(*selectedTemplates[last_template-1],current_region);
			Diff_vec.SetRef(current_region.size(),Diff_mat.Pointer());
			A_lsq.ProductOf(Q_mat,R_mat);
			B_lsq.ProductOf(Q_mat,Diff_vec);
			nmrLSqLin(A_lsq,B_lsq,X_lsq);
			updateParameters();
		}

	}
	outputPosX = CurLoc.X();
	outputPosY = CurLoc.Y();

}


#endif // CISST_SVL_HAS_OPENCV

//Copy a input image into buffer matrix (within cisst)
void svlTrackerCisstWSSD::copyImage_cisst(MatrixType &src, MatrixType &dst, IndexType startR, IndexType startC) {
	for(IndexType r = 0; r < dst.rows(); r++) {
		for(IndexType c = 0; c < dst.cols(); c++) {
			dst.Element(r,c) = src.Element(startR+r,startC+c);
		}
	}
}

//Copy a input image into buffer matrix
void svlTrackerCisstWSSD::copyImage(InterfaceImType *src, IndexType CMN_UNUSED(src_rows), IndexType src_cols,
                             MatrixType &dst, IndexType startR, IndexType startC) {
	for(IndexType r = 0; r < dst.rows(); r++) {
		for(IndexType c = 0; c < dst.cols(); c++) {
			dst.Element(r,c) = src[(startR+r)*src_cols+(startC+c)];
			//CMN_LOG_INIT_ERROR<<"("<<r<<","<<c<<")="<<dst.Element(r,c)<<endl;
		}
	}
}
// Copy the current image 
void svlTrackerCisstWSSD::copyCurrentImage(InterfaceImType *src, IndexType src_rows, IndexType src_cols){
	copyImage(src, src_rows, src_cols, Current_image);
}

//Unit normalize a template, source and destination must be seperate
void svlTrackerCisstWSSD::WSSD_NormalizeTemplate(MatrixType &src, MatrixType &dst) {
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
void svlTrackerCisstWSSD::pushTemplate(InterfaceImType *src, IndexType src_rows, IndexType src_cols, IndexType r, IndexType c) {
	//Create a new matrix of the right size
	if(templateSize.X() <= 0 || templateSize.Y() <= 0){
		cmnThrow(std::runtime_error("svlTrackerCisstWSSD pushTemplate function - degenerate template size"));
	}
	MatrixType * temp  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
	//Copy in the image data
	copyImage(src, src_rows, src_cols, *temp, r - static_cast<IndexType>(templateCenter.Y()), c - static_cast<IndexType>(templateCenter.X()));
	//Add the template to the list
	allTemplates.push_back(temp);
}
// Add another template by copying to the list of the template
void svlTrackerCisstWSSD::pushTemplate(InterfaceImType *src, IndexType src_rows, IndexType src_cols) {
	//Create a new matrix of the right size
	if(templateSize.X() <= 0 || templateSize.Y() <= 0){
		cmnThrow(std::runtime_error("svlTrackerCisstWSSD pushTemplate function - degenerate template size"));
	}
#ifdef IS_LOGGING
	CMN_LOG_INIT_ERROR << " I'm before template copy "<<endl;
#endif
	MatrixType * temp  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
	//Copy in the image data
	copyImage(src, src_rows, src_cols, *temp);
#ifdef IS_LOGGING
	CMN_LOG_INIT_ERROR << " I'm after template copy "<<endl;
#endif
	//Add the template to the list
	allTemplates.push_back(temp);
#ifdef IS_LOGGING
	CMN_LOG_INIT_ERROR << " template : "<<endl;
	for(IndexType i = 0; i < templateSize.Y(); i++)
	{
		CMN_LOG_INIT_ERROR << temp->Row(i) <<endl;
	}
#endif
}
//Remove last template from the tracker
void svlTrackerCisstWSSD::popTemplate() {
	if(allTemplates.empty()) {return;}
	delete allTemplates.back();
	allTemplates.pop_back();
}
//Get the number of templates loaded
//Explicit unsigned int required, can't use the typedef in MS.Net
svlTrackerCisstWSSD::IndexType svlTrackerCisstWSSD::getNumberOfTemplates() {
	return allTemplates.size();
}
void svlTrackerCisstWSSD::pushSelectedTemplate(IndexType index) {
	selectedTemplates.push_back(allTemplates[index]);
}
//Remove the last selected template
void svlTrackerCisstWSSD::popSelectedTemplate() {
	selectedTemplates.pop_back();
}
//*******************************************************************
//                 Tracking code
//*******************************************************************


void svlTrackerCisstWSSD::setThreshold(RealType thres) {
	threshold = thres;
}

//Cisst Library Test Routines
//void svlTrackerCisstWSSD::test() {}

void svlTrackerCisstWSSD::computeXderivative(MatrixType &src, MatrixType &dst)
{
	// compute X derivative of the template
	dst.SetAll(0); // setting all values zeros for boundaries 
	for(IndexType r = 0; r < dst.rows(); r++) {
		for(IndexType c = 1; c < (dst.cols()-1); c++)  {
			dst.Element(r,c) = (src.Element(r,c+1) - src.Element(r,c-1))/2;
		//	CMN_LOG_INIT_ERROR<<"("<<r<<","<<c<<")="<<dst.Element(r,c)<<endl;
		}
	}
}
	
void svlTrackerCisstWSSD::computeXderivative_filtered(MatrixType & CMN_UNUSED(src), MatrixType &dst)
{
	// compute X derivative of the template
	dst.SetAll(0);
	RealType filtered_value = 0.0;
	IndexType row_cen = (X_der_filter.rows()-1)/2;
	IndexType col_cen = (X_der_filter.cols()-1)/2;

	for(IndexType r = 0; r < dst.rows(); r++) {
		for(IndexType c = 0; c < dst.cols(); c++)  {
			filtered_value = 0.0;
			for(IndexType rr = 0; rr < X_der_filter.rows(); rr++){
				for(IndexType cc = 0; cc < X_der_filter.cols(); cc++){
					filtered_value+=X_der_filter(rr,cc)*WarpedMatrix.Element(r+der_filter_offset/2+rr-row_cen,c+der_filter_offset/2+cc-col_cen);
					/* #ifdef IS_LOGGING
						CMN_LOG_INIT_ERROR<<"("<<r<<","<<c<<");"<<"("<<rr<<","<<cc<<");"<<"("<<r+der_filter_offset/2+rr-row_cen<<","<<c+der_filter_offset/2+cc-col_cen<<")="<<WarpedMatrix.Element(r+der_filter_offset/2+rr-row_cen,c+der_filter_offset/2+cc-col_cen)<<endl;
					#endif */
				}
			}
			dst.Element(r,c) = filtered_value;
		}
	}
}




void svlTrackerCisstWSSD::computeYderivative(MatrixType &src, MatrixType &dst)
{
	// compute Y derivative of the template
	dst.SetAll(0);
	for(IndexType r = 1; r < (dst.rows()-1); r++) {
		for(IndexType c = 0; c < dst.cols(); c++)  {
			dst.Element(r,c) = (src.Element(r+1,c) - src.Element(r-1,c))/2;
		}
	}
}
void svlTrackerCisstWSSD::computeYderivative_filtered(MatrixType & CMN_UNUSED(src), MatrixType &dst)
{
	// compute X derivative of the template
	dst.SetAll(0);
	RealType filtered_value = 0.0;
	IndexType row_cen = (Y_der_filter.rows()-1)/2;
	IndexType col_cen = (Y_der_filter.cols()-1)/2;
	for(IndexType r = 0; r < dst.rows(); r++) {
		for(IndexType c = 0; c < dst.cols(); c++)  {
			filtered_value = 0.0;
			for(IndexType rr = 0; rr < Y_der_filter.rows(); rr++){
				for(IndexType cc = 0; cc < Y_der_filter.cols(); cc++){
					filtered_value+=Y_der_filter(rr,cc)*WarpedMatrix.Element(r+der_filter_offset/2+rr-row_cen,c+der_filter_offset/2+cc-col_cen);
				}
			}
			dst.Element(r,c) = filtered_value;
		}
	}
}

void svlTrackerCisstWSSD::computeXYvalues(MatrixType &x_vals, MatrixType &y_vals)
{
	double mid_x = (x_vals.cols() - 1.0)/2.0;
	double mid_y = (x_vals.rows() - 1.0)/2.0;
	for(IndexType r = 0; r < x_vals.rows(); r++) {
		for(IndexType c = 0; c < x_vals.cols(); c++) {
			x_vals.Element(r,c) = c - mid_x;
			y_vals.Element(r,c) = r - mid_y;
		}
	}
}
void svlTrackerCisstWSSD::computeTemplateDerivative(IndexType index){
	// Note the derivatives are computed for the list of selected templates
	// compute x derivative
	MatrixType * xder  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
	computeXderivative(*selectedTemplates[index],*xder);
	
	MatrixType * yder  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
	computeYderivative(*selectedTemplates[index],*yder);
	
	VectorRefType tempVec1;
	MatrixType *der_mat = 0;
	MatrixType * x_vals = 0;
	MatrixType * y_vals = 0;
	MatrixType * rotder = 0;
	MatrixType * scder = 0;
	MatrixType * temp = 0;
	switch (trackmodelused) {
		case TRANS:
			der_mat = new MatrixType(templateSize.Y()*templateSize.X(),2, VCT_COL_MAJOR);
			tempVec1.SetRef(xder->size(),xder->Pointer());
			der_mat->Column(0).Assign(tempVec1);
			tempVec1.SetRef(yder->size(),yder->Pointer());
			der_mat->Column(1).Assign(tempVec1);
			derTemplates.push_back(der_mat);
			break;
		case TRANS_ROT:
			x_vals  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			y_vals  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			computeXYvalues(*x_vals,*y_vals);
			rotder  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			temp  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			rotder->ProductOf(*y_vals,*xder);
			temp->ProductOf(*x_vals,*yder);
			rotder->DifferenceOf(*rotder,*temp);
			der_mat = new MatrixType(templateSize.Y()*templateSize.X(),3, VCT_COL_MAJOR);
			tempVec1.SetRef(xder->size(),xder->Pointer());
			der_mat->Column(0).Assign(tempVec1);
			tempVec1.SetRef(yder->size(),yder->Pointer());
			der_mat->Column(1).Assign(tempVec1);
			tempVec1.SetRef(rotder->size(),rotder->Pointer());
			der_mat->Column(2).Assign(tempVec1);
			derTemplates.push_back(der_mat);
			delete x_vals;
			delete y_vals;
			delete temp;
			delete rotder;
			break;
		case TRANS_SCALE:
			x_vals  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			y_vals  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			computeXYvalues(*x_vals,*y_vals);
			scder  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			temp  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			scder->ProductOf(*x_vals,*xder);
			temp->ProductOf(*y_vals,*yder);
			scder->Add(*temp);
			der_mat = new MatrixType(templateSize.Y()*templateSize.X(),3, VCT_COL_MAJOR);
			tempVec1.SetRef(xder->size(),xder->Pointer());
			der_mat->Column(0).Assign(tempVec1);
			tempVec1.SetRef(yder->size(),yder->Pointer());
			der_mat->Column(1).Assign(tempVec1);
			tempVec1.SetRef(scder->size(),scder->Pointer());
			der_mat->Column(2).Assign(tempVec1);
			derTemplates.push_back(der_mat);
			delete x_vals;
			delete y_vals;
			delete temp;
			delete scder;
			break;
		case TRANS_ROT_SCALE:
			x_vals  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			y_vals  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			computeXYvalues(*x_vals,*y_vals);
			rotder  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			scder  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			temp  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			scder->ProductOf(*x_vals,*xder);
			temp->ProductOf(*y_vals,*yder);
			scder->Add(*temp);
			rotder->ProductOf(*y_vals,*xder);
			temp->ProductOf(*x_vals,*yder);
			rotder->DifferenceOf(*rotder,*temp);
			der_mat = new MatrixType(templateSize.Y()*templateSize.X(),4, VCT_COL_MAJOR);
			tempVec1.SetRef(xder->size(),xder->Pointer());
			der_mat->Column(0).Assign(tempVec1);
			tempVec1.SetRef(yder->size(),yder->Pointer());
			der_mat->Column(1).Assign(tempVec1);
			tempVec1.SetRef(scder->size(),scder->Pointer());
			der_mat->Column(2).Assign(tempVec1);
			tempVec1.SetRef(rotder->size(),rotder->Pointer());
			der_mat->Column(2).Assign(tempVec1);
			derTemplates.push_back(der_mat);
			delete x_vals;
			delete y_vals;
			delete temp;
			delete rotder;
			delete scder;
			break;

		default:
			der_mat = new MatrixType(templateSize.Y()*templateSize.X(),2, VCT_COL_MAJOR);
			tempVec1.SetRef(xder->size(),xder->Pointer());
			der_mat->Column(0).Assign(tempVec1);
			tempVec1.SetRef(yder->size(),yder->Pointer());
			der_mat->Column(1).Assign(tempVec1);
			derTemplates.push_back(der_mat);
			break;

	}		
	delete xder;
	delete yder;
}

void svlTrackerCisstWSSD::computeCurrentDerivative(){
	// Note the derivatives are computed for the list of selected templates
	// compute x derivative

	MatrixType * xder  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
	computeXderivative_filtered(current_region,*xder);
	//#ifdef IS_LOGGING
	//CMN_LOG_INIT_ERROR << " X derivative : "<<endl;
	//for(IndexType i = 0; i < templateSize.Y(); i++)
	//{
	//	CMN_LOG_INIT_ERROR << xder->Row(i) <<endl;
	//}
	//#endif
	MatrixType * yder  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
	computeYderivative_filtered(current_region,*yder);
	//#ifdef IS_LOGGING
	//CMN_LOG_INIT_ERROR << " Y derivative : "<<endl;
	//for(IndexType i = 0; i < templateSize.Y(); i++)
	//{
	//	CMN_LOG_INIT_ERROR << yder->Row(i) <<endl;
	//}
	//#endif
	VectorRefType tempVec1;
	MatrixType * x_vals = 0;
	MatrixType * y_vals = 0;
	MatrixType * rotder = 0;
	MatrixType * scder = 0;
	MatrixType * temp = 0;
	switch (trackmodelused) {
		case TRANS:
			tempVec1.SetRef(xder->size(),xder->Pointer());
			derCurrent_region.Column(0).Assign(tempVec1);
			tempVec1.SetRef(yder->size(),yder->Pointer());
			derCurrent_region.Column(1).Assign(tempVec1);
			break;
		case TRANS_ROT:
			x_vals  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			y_vals  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			computeXYvalues(*x_vals,*y_vals);
			rotder  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			temp  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			rotder->ProductOf(*y_vals,*xder);
			temp->ProductOf(*x_vals,*yder);
			rotder->DifferenceOf(*rotder,*temp);
			tempVec1.SetRef(xder->size(),xder->Pointer());
			derCurrent_region.Column(0).Assign(tempVec1);
			tempVec1.SetRef(yder->size(),yder->Pointer());
			derCurrent_region.Column(1).Assign(tempVec1);
			tempVec1.SetRef(rotder->size(),rotder->Pointer());
			derCurrent_region.Column(2).Assign(tempVec1);
			delete x_vals;
			delete y_vals;
			delete temp;
			break;
		case TRANS_SCALE:
			x_vals  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			y_vals  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			computeXYvalues(*x_vals,*y_vals);
			scder  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			temp  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			scder->ProductOf(*x_vals,*xder);
			temp->ProductOf(*y_vals,*yder);
			scder->Add(*temp);
			tempVec1.SetRef(xder->size(),xder->Pointer());
			derCurrent_region.Column(0).Assign(tempVec1);
			tempVec1.SetRef(yder->size(),yder->Pointer());
			derCurrent_region.Column(1).Assign(tempVec1);
			tempVec1.SetRef(scder->size(),scder->Pointer());
			derCurrent_region.Column(2).Assign(tempVec1);
			delete x_vals;
			delete y_vals;
			delete temp;
			break;
		case TRANS_ROT_SCALE:
			x_vals  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			y_vals  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			computeXYvalues(*x_vals,*y_vals);
			rotder  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			scder  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			temp  = new MatrixType(templateSize.Y(), templateSize.X(), VCT_COL_MAJOR);
			scder->ProductOf(*x_vals,*xder);
			temp->ProductOf(*y_vals,*yder);
			scder->Add(*temp);
			rotder->ProductOf(*y_vals,*xder);
			temp->ProductOf(*x_vals,*yder);
			rotder->DifferenceOf(*rotder,*temp);
			tempVec1.SetRef(xder->size(),xder->Pointer());
			derCurrent_region.Column(0).Assign(tempVec1);
			tempVec1.SetRef(yder->size(),yder->Pointer());
			derCurrent_region.Column(1).Assign(tempVec1);
			tempVec1.SetRef(scder->size(),scder->Pointer());
			derCurrent_region.Column(2).Assign(tempVec1);
			tempVec1.SetRef(rotder->size(),rotder->Pointer());
			derCurrent_region.Column(2).Assign(tempVec1);
			delete x_vals;
			delete y_vals;
			delete temp;
			break;

		default:
			tempVec1.SetRef(xder->size(),xder->Pointer());
			derCurrent_region.Column(0).Assign(tempVec1);
			tempVec1.SetRef(yder->size(),yder->Pointer());
			derCurrent_region.Column(1).Assign(tempVec1);
			break;

	}		

}

void svlTrackerCisstWSSD::copyCurrentRegion()
{
	// Copy current region 
	for(IndexType r = 0; r < current_region.rows(); r++){
		for(IndexType c = 0; c < current_region.cols(); c++) {
	//		CMN_LOG_INIT_ERROR << (WarpedMatrix.Element(r,c)) <<endl;
			current_region.Element(r,c) = WarpedMatrix.Element(r+der_filter_offset/2,c+der_filter_offset/2);
//			CMN_LOG_INIT_ERROR<< current_region.Element(r,c)<<endl;
		}
	}

}

//Inititalize the pre-allocated data for tracking
void svlTrackerCisstWSSD::initializeTrack() {
	if(templateLookahead==0) {
		templateLookahead = selectedTemplates.size();
	}
	currentTemplateIndex = 0;

	if( selectedTemplates.size() > templateLookahead ) {
		// make template look ahead the same size as the size of selected template
		templateLookahead = selectedTemplates.size();
	}
	if( templateSize.X()<=0 || templateSize.Y()<=0 ) {
		cmnThrow(std::runtime_error("svlTrackerCisstWSSD initializeTrack - degenerate template size"));
	}

	CurLoc.X() = initPos.X();
	CurLoc.Y() = initPos.Y();
	
	// WarpedMatrix.SetRef(templateSize.Y(),templateSize.X(),Current_image);
	WarpedMatrix.SetCenter(templateCenter.Y()+der_filter_offset/2,templateCenter.X()+der_filter_offset/2);
	WarpedMatrix.SetPosition(CurLoc.Y(),CurLoc.X());
	WarpedMatrix.SetAngleAndScale(CurAngle,CurScale);

	// set size of the current region
	current_region.SetSize(templateSize.Y(),templateSize.X(), VCT_COL_MAJOR);
	//copyCurrentRegion();
	IndexType columnSize;
	IndexType derColSize;
	switch(trackmodelused){
		case TRANS:
			columnSize = selectedTemplates.size() + 1;
			derColSize = 2;
			break;
		case TRANS_ROT:
			columnSize = selectedTemplates.size() + 2;
			derColSize = 3;
			break;
		case TRANS_SCALE:
			columnSize = selectedTemplates.size() + 2;
			derColSize = 3;
			break;
		case TRANS_ROT_SCALE:
			columnSize = selectedTemplates.size() + 3;
			derColSize = 4;
			break;
		default:
			columnSize = selectedTemplates.size() + 1;
			derColSize = 2;
	}

	if(selectedTemplates.size() == 1)
	{
		// compute the derivative of the 1 template
		computeTemplateDerivative(0);
		Diff_mat.SetSize(templateSize.Y(),templateSize.X(),VCT_COL_MAJOR);
		X_lsq.SetSize(columnSize);
		A_lsq.SetSize(templateSize.Y()*templateSize.X(),columnSize,VCT_COL_MAJOR);
		B_lsq.SetSize(templateSize.Y()*templateSize.X());
	}
	else
	{
		// create R and Q matrices
		MatrixType A_mat;
		A_mat.SetSize(templateSize.Y(),templateSize.X(),VCT_COL_MAJOR);
		A_mat.SetAll(0);
		R_mat.SetSize(templateSize.Y()*templateSize.X(), columnSize,VCT_COL_MAJOR);
		Q_mat.SetSize(columnSize, templateSize.Y()*templateSize.X(), VCT_ROW_MAJOR);
		for(IndexType i = 0; i < selectedTemplates.size(); i++)
		{
			A_mat.Add(*selectedTemplates[i]);
		}
		double div_factor = 1.0/selectedTemplates.size();
		A_mat.Multiply(div_factor);
//#ifdef IS_LOGGING
//		CMN_LOG_INIT_ERROR<<"A_mat ="<<endl;
//		for(IndexType i = 0; i < A_mat.rows(); i++)
//		{
//		CMN_LOG_INIT_ERROR << A_mat.Row(i) <<endl;
//		}
//#endif 
		MatrixType temp1, temp2;
		temp1.SetSize(templateSize.Y(),templateSize.X(),VCT_COL_MAJOR);
		temp2.SetSize(templateSize.Y(),templateSize.X(),VCT_COL_MAJOR);
		VectorRefType tempVec1, tempVec2;

		IndexType last_template = selectedTemplates.size()-1;
		for(IndexType i = 0; i < last_template; i++)
		{	
			temp1.DifferenceOf(*selectedTemplates[last_template],*selectedTemplates[i]);
			temp2.DifferenceOf(A_mat,*selectedTemplates[i]);
			tempVec1.SetRef(temp1.size(),temp1.Pointer());
			R_mat.Column(i).Assign(tempVec1);
			tempVec2.SetRef(temp2.size(),temp2.Pointer());
			Q_mat.Row(i).Assign(tempVec2);
		}
//#ifdef IS_LOGGING
//		CMN_LOG_INIT_ERROR<<"R_mat ="<<endl;
//		for(IndexType i = 0; i < R_mat.rows(); i++)
//		{
//		CMN_LOG_INIT_ERROR << R_mat.Row(i) <<endl;
//		}
//		CMN_LOG_INIT_ERROR<<"Q_mat ="<<endl;
//		for(IndexType i = 0; i < Q_mat.rows(); i++)
//		{
//		CMN_LOG_INIT_ERROR << Q_mat.Row(i) <<endl;
//		}
//#endif 
		// Set the A_lsq, B_lsq  and X_lsq matrix sizes
		A_lsq.SetSize(columnSize,columnSize,VCT_COL_MAJOR);
		B_lsq.SetSize(columnSize);
		X_lsq.SetSize(columnSize);
		// Set the size of the diff mat
		Diff_mat.SetSize(templateSize.Y(),templateSize.X(),VCT_COL_MAJOR);
		derCurrent_region.SetSize(templateSize.Y()*templateSize.X(),derColSize,VCT_COL_MAJOR);
	}
			
	
}

void svlTrackerCisstWSSD::updateParameters(){
	// Update the parameters from X_lsq
	IndexType last_template = selectedTemplates.size()-1;
	//CMN_LOG_INIT_ERROR<<"last_temp ="<<last_template<<endl;
	//CMN_LOG_INIT_ERROR<<"X_lsq = "<<X_lsq<<endl;
	switch(trackmodelused){
		case TRANS:
			CurLoc.X() = CurLoc.X() + X_lsq[last_template];
			CurLoc.Y() = CurLoc.Y() + X_lsq[last_template+1];
			break;
		case TRANS_ROT:
			CurLoc.X() = CurLoc.X() + X_lsq[last_template];
			CurLoc.Y() = CurLoc.Y() + X_lsq[last_template+1];
			CurAngle = CurAngle + X_lsq[last_template+2];
			break;
		case TRANS_SCALE:
			CurLoc.X() = CurLoc.X() + X_lsq[last_template];
			CurLoc.Y() = CurLoc.Y() + X_lsq[last_template+1];
			CurScale = CurScale + X_lsq[last_template+2];
			break;
		case TRANS_ROT_SCALE:
			CurLoc.X() = CurLoc.X() + X_lsq[last_template];
			CurLoc.Y() = CurLoc.Y() + X_lsq[last_template+1];
			CurScale = CurScale + X_lsq[last_template+2];
			CurAngle = CurAngle + X_lsq[last_template+2];
			break;
		default:
			// default size is 2 (i.e. TRANS model)
			CurLoc.X() = CurLoc.X() + X_lsq[last_template];
			CurLoc.Y() = CurLoc.Y() + X_lsq[last_template+1];
	}
}

//Update the current window position
void svlTrackerCisstWSSD::updateTrack() {

	WarpedMatrix.SetRef(templateSize.Y()+der_filter_offset,templateSize.X()+der_filter_offset,Current_image);
	if(selectedTemplates.size() == 1)
	{
		for(IndexType i = 0; i < numIter; i++)
		{
			WarpedMatrix.SetPosition(CurLoc.Y(),CurLoc.X());
			WarpedMatrix.SetAngleAndScale(CurAngle,CurScale);
			copyCurrentRegion();
			A_lsq.Assign(*(derTemplates[0]));
			Diff_mat.DifferenceOf(*(selectedTemplates[0]),current_region);
			Diff_vec.SetRef(current_region.size(),Diff_mat.Pointer());
			B_lsq.Assign(Diff_vec);
			nmrLSqLin(A_lsq,B_lsq,X_lsq);
			updateParameters();
		}
		
	}
	else
	{
		for(IndexType i = 0; i < numIter; i++)
		{
			WarpedMatrix.SetPosition(CurLoc.Y(),CurLoc.X());
			WarpedMatrix.SetAngleAndScale(CurAngle,CurScale);
			copyCurrentRegion();
			//#ifdef IS_LOGGING
			//CMN_LOG_INIT_ERROR << " current region : "<<endl;
			//for(IndexType i = 0; i < templateSize.Y(); i++)
			//{
			//	CMN_LOG_INIT_ERROR << current_region.Row(i) <<endl;
			//}
			//#endif
			computeCurrentDerivative();
			// Add derivatives to the R and Q matrices
			IndexType last_template = selectedTemplates.size()-1;
			VectorRefType tempVec1;
			IndexType cur_index;
			for(IndexType i = 0; i < derCurrent_region.cols(); i++)
			{
				cur_index = last_template + i;
				R_mat.Column(cur_index).Assign(derCurrent_region.Column(i));
				Q_mat.Row(cur_index).Assign(derCurrent_region.Column(i));
			}
			Diff_mat.DifferenceOf(*selectedTemplates[last_template],current_region);
			Diff_vec.SetRef(current_region.size(),Diff_mat.Pointer());
//#ifdef IS_LOGGING
//		//CMN_LOG_INIT_ERROR<<"R_mat ="<<endl;
//		//for(IndexType i = 0; i < R_mat.rows(); i++)
//		//{
//		//CMN_LOG_INIT_ERROR << R_mat.Row(i) <<endl;
//		//}
//		//CMN_LOG_INIT_ERROR<<"Q_mat ="<<endl;
//		//for(IndexType i = 0; i < Q_mat.rows(); i++)
//		//{
//		//CMN_LOG_INIT_ERROR << Q_mat.Row(i) <<endl;
//		//}
//#endif
//	#ifdef IS_LOGGING
//	CMN_LOG_INIT_ERROR << " Diff_vec = "<<Diff_vec<<endl;
//	#endif
			A_lsq.ProductOf(Q_mat,R_mat);
			B_lsq.ProductOf(Q_mat,Diff_vec);
#ifdef IS_LOGGING
	CMN_LOG_INIT_ERROR << " A_lsq : "<<endl;
	for(IndexType i = 0; i < A_lsq.rows(); i++)
	{
		CMN_LOG_INIT_ERROR << A_lsq.Row(i) <<endl;
	}
	CMN_LOG_INIT_ERROR << " B_lsq = "<<B_lsq<<endl;
#endif
			nmrLSqLin(A_lsq,B_lsq,X_lsq);
#ifdef IS_LOGGING
			CMN_LOG_INIT_ERROR<<" X_lsq = "<<X_lsq<<endl;
#endif
			updateParameters();
#ifdef IS_LOGGING
			CMN_LOG_INIT_ERROR<<" CurLoc"<<CurLoc.X()<<","<<CurLoc.Y()<<endl;
#endif
		}

	}
	outputPosX = CurLoc.X();
	outputPosY = CurLoc.Y();

}

