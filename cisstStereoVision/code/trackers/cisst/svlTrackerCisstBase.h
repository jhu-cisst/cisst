/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Robert Jacques & Maneesh Dewan (?)
  Created on: 2007 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#ifndef _svlTrackerCisstBase_h
#define _svlTrackerCisstBase_h

#include <cisstStereoVision/svlTypes.h>


class svlTrackerCisstBase
{
public:
	svlTrackerCisstBase() {}
	virtual ~svlTrackerCisstBase() {}

	// typedefs used

	// Data type for image to interface with Siemens Radbuilder and ICE programs
	typedef unsigned short InterfaceImType;
	//Basic numeric type for processing, should be a real type
	typedef double RealType;
	//A matrix data type
	typedef vctDynamicMatrix < RealType > MatrixType;
	//Datatype for indexes
	typedef MatrixType::index_type IndexType;


	virtual void setTemplateSize(IndexType rows, IndexType cols) = 0;
	virtual void setTemplateLookahead(IndexType number) = 0;
	virtual void setTemplateCenter(IndexType r, IndexType c) = 0;
	virtual void pushTemplate(InterfaceImType *src, IndexType src_rows, IndexType src_cols) = 0;
	virtual void pushSelectedTemplate(IndexType index) = 0;
	virtual void setWindowSize(IndexType rows, IndexType cols) = 0;
	virtual void setWindowCenter(IndexType r, IndexType c) = 0;
	virtual void setWindowPosition(IndexType r, IndexType c) = 0;
	virtual void setInitPosition(IndexType r, IndexType c) = 0;
	virtual void initializeTrack() =0;
	virtual void setCurrentimageSize(IndexType rows, IndexType cols) = 0;
	virtual void setImageSize(IndexType rows, IndexType cols) = 0;
	virtual IndexType getCurrentimageNumRows() = 0;
	virtual IndexType getCurrentimageNumCols() = 0;
	virtual void copyCurrentImage(InterfaceImType *src, IndexType src_rows, IndexType src_cols) = 0;
	virtual RealType getOutputPosX() = 0;
	virtual RealType getOutputPosY() = 0;
	virtual void updateTrack() = 0;
	virtual void resetTrack() = 0;
};

#endif // _svlTrackerCisstBase_h

