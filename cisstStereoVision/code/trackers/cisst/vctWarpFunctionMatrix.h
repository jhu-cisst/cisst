/*
  
  Author(s):	Ofri Sadowsky, Anton Deguet
  Created on: 2004-07-01

--- begin cisst license - do not edit ---

CISST Software License Agreement(c)

Copyright 2005 Johns Hopkins University (JHU) All Rights Reserved.

This software ("CISST Software") is provided by The Johns Hopkins
University on behalf of the copyright holders and
contributors. Permission is hereby granted, without payment, to copy,
modify, display and distribute this software and its documentation, if
any, for research purposes only, provided that (1) the above copyright
notice and the following four paragraphs appear on all copies of this
software, and (2) that source code to any modifications to this
software be made publicly available under terms no more restrictive
than those in this License Agreement. Use of this software constitutes
acceptance of these terms and conditions.

The CISST Software has not been reviewed or approved by the Food and
Drug Administration, and is for non-clinical, IRB-approved Research
Use Only.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY
EXPRESS OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

--- end cisst license ---
*/

/*! TODO List
01 [ ]	Add filters for scalling (like guassian)
02 [ ]	Edge conditions
*/


#ifndef _vctWarpFunctionMatrix_h
#define _vctWarpFunctionMatrix_h

#include <iostream>
using namespace std;

#include "vctDynamicFunctionalMatrixOwner.h"
#include "vctDynamicFunctionalMatrixBase.h"



/*!
  \file
  \brief Declaration of vctWarpFunctionMatrix and vctWarpFunctionMatrixOwner
*/

/*!
	A Owner type providing a functional view of a matrix providing basic
	rotation, translation and scalling.

	\param _baseMatrixType The underlying matrix type

	\param RealType A floating point number type, for use in angle  and position calculation.
*/
template< class _baseMatrixType, class RealType = double >
class vctWarpFunctionMatrixOwner // : public vctDynamicFunctionalMatrixOwner<_baseMatrixType>
{
public:
    /*! define most types from vctContainerTraits */
	VCT_CONTAINER_TRAITS_TYPEDEFS(typename _baseMatrixType::value_type);

	/*! The underlying type of matrix this function decorates, should be a reference type*/
	typedef _baseMatrixType MatrixBaseType;

    /*! Type of the matrix owner itself. */
	typedef vctWarpFunctionMatrixOwner<_baseMatrixType, value_type> ThisType;

    /*! Constant foward iterator type */    
    typedef vctFunctionalMatrixConstIterator<ThisType> const_iterator;
    /*! Constant reverse iterator type */    
    typedef vctFunctionalMatrixConstIterator<ThisType> const_reverse_iterator;
    /*! foward iterator type */    
    typedef vctFunctionalMatrixConstIterator<ThisType> iterator;
    /*! reverse iterator type */    
    typedef vctFunctionalMatrixConstIterator<ThisType> reverse_iterator;


    /*! The type indicating a vector of this matrix accessed by (const)
      reference */
    typedef vctDynamicConstVectorRef<value_type> ConstVectorRefType;
    
    /*! The type indicating a vector of this matrix accessed by
      (non-const) reference */
    typedef vctDynamicVectorRef<value_type> VectorRefType;
    
	/*! The type of matrix indicating matrix access by (const)
	  reference */
    typedef ThisType ConstMatrixRefType;

	/*! The type of matrix indicating matrix access by (const)
	  reference */
    typedef ThisType MatrixRefType;

    /*! The type of a matrix returned by value from operations on this object */
    typedef ThisType MatrixValueType;

	/*! A floating point point type, for interpolated indexing*/
	typedef vctFixedSizeVector < RealType , 2> PointType;

	/*! An integer point type, for storing the center point*/
	typedef vctFixedSizeVector < index_type , 2> IndexPointType;



	/*! Default Initilization. Not valid to use */
	vctWarpFunctionMatrixOwner() :
        Rows(0),
        Cols(0),
        Data(0)
    {}
	
	/*! Initialize to a given size and underlying Matrix */
    vctWarpFunctionMatrixOwner(size_type rows, size_type cols, MatrixBaseType * data):
        Rows(rows),
        Cols(cols),
        Data(data)
    {}

	/*! Simple set routine for all data values */
    void SetRef(size_type rows, size_type cols, MatrixBaseType * data) {
        Rows = rows;
        Cols = cols;
        Data = data;
    }
	/*! Simple set routine for all data values */
    void SetRef(size_type rows, size_type cols, MatrixBaseType & data) {
        SetRef(rows,cols,&data);
    }
	
	/*! Set only the underlying data, useful for shifting / loop structures */
    void SetRef(MatrixBaseType * data) {
        Data = data;
    }
	/*! Set only the underlying data, useful for shifting / loop structures */
    void SetRef(MatrixBaseType & data) {
        SetRef(&data);
    }

	/*! The total size of the matrix */
    size_type size(void) const {
        return Rows * Cols;
    }
	
	/*! The number of rows in the matrix */
    size_type rows(void) const {
        return Rows;
    }
	
	/*! The number of columns in the matrix */
    size_type cols(void) const {
        return Cols;
    }

  	/*! A constant forward column major iterator at the root of the matrix */
	const_iterator begin(void) const {
        return const_iterator(this);
    }

  	/*! A constant forward column major iterator at the end of the matrix */
	const_iterator end(void) const {
        return const_iterator(this,rows());
    }

  	/*! A constant reverse column major iterator at the root of the reverse matrix */
	const_reverse_iterator rbegin(void) const {
        return const_reverse_iterator(this,rows() - 1,cols() - 1,-1);
    }

  	/*! A constant reverse column major iterator at the end of the reverse matrix */
	const_reverse_iterator rend(void) const {
        return const_reverse_iterator(this,-1,cols() - 1,-1);
    }

	//Start of Warped Specific Code


	/*! Get the current scale value. Relative to source data (1) */
   RealType GetScale() const{
	   return RowVector.Norm();
   }
   
   /*! Get the current scale value. Relative to source data (1) */
   RealType GetAngle() const{
	   return asin( RowVector.X() / GetScale() );
   }

   /*! Set the angle of the matrix view relative to the underlying data. */
   void SetAngle(RealType angle) {
		SetAngleAndScale(angle, GetScale());
   }
  
   /*! Set the angle of the matrix view relative to the underlying data. */
   void SetAngleAndScale(RealType angle, RealType Scale) {
		RowVector.Y() = cos(angle) * Scale;
		RowVector.X() = -sin(angle) * Scale;
		SetColumnVectorFromRowVector();
		CalculateAnchor();
   }

   /*! Set the position of the view in underlying data space */
   void SetPosition(RealType row, RealType col) {
		CenterVector.X() = col;
		CenterVector.Y() = row;
		CalculateAnchor();
   }
    /*! Set the position of the view by relative increase in local data space */
   void SetDeltaPosition(RealType row, RealType col) {
//		CenterVector.X() += row * RowVector.X() + col * ColVector.X();
//		CenterVector.Y() +=  row * RowVector.Y() + col * ColVector.Y();
		CenterVector.X() += col;
		CenterVector.Y() += row;
   }
     
   /*! Set the position of the view in relative data space */
   void SetPosition(PointType & position) {
	   CenterVector = position;
   }

   /*! Get row position relative to data space*/
   RealType GetPositionRow() const{
	   return CenterVector.Y();
   }

   /*! Get col position relative to data space*/
   RealType GetPositionCol() const{
	   return CenterVector.X();
   }
   
   /*! Set the logical center of the view, i.e. the rotation center*/
   void SetCenter(RealType row, RealType col) {
	   RelativeCenter.X() = col;
	   RelativeCenter.Y() = row;
   }
   /*! Set the logical center of the view, i.e. the rotation center*/
   RealType GetCenterRow() const{
	   return RelativeCenter.Y();
   }
   /*! Set the logical center of the view, i.e. the rotation center*/
   RealType GetCenterCol() const{
	   return RelativeCenter.X();
   }

	/*! Accessor for constant matrixes, virtual to facilitate 
	specilization based upon functional access type. Basic implementation simply
	returns the underline structure. Default implementation should be changed to
	use Data.Element as soon as possible */
	RealType Element(index_type rowIndex, index_type colIndex) const {
		PointType Index;
//		Index.X() = rowIndex * RowVector.X() + colIndex * ColVector.X() + Anchor.X();
//		Index.Y() = rowIndex * RowVector.Y() + colIndex * ColVector.Y() + Anchor.Y();
		Index.X() = colIndex + Anchor.X();
		Index.Y() = rowIndex + Anchor.Y();

		return LinearInterpolate(Index);
	}
protected:
	/*! The top corner of the matrix in underlying data coordinates */
	PointType Anchor;
	/*! A vector representing the scalling and angle of the view relative to the underlying data. 
	    Points in the direction of increasing rows. Scale is determined by vector length.*/
	PointType RowVector;
	/*! A vector representing the scalling and angle of the view relative to the underlying data. 
	    Points in the direction of increasing col. Scale is determined by vector length.*/
	PointType ColVector;

	/*! The center of the matrix relative to the underlying data */
	PointType CenterVector;

	/*! The center of the matrix relative to itself, often 1/2 width|height */
	PointType RelativeCenter;

	/*! This function rotates a vector 90 degrees. For creating ColVector from RowVector */
	void SetColumnVectorFromRowVector() {
		ColVector.Y() = -RowVector.X();
		ColVector.X() = RowVector.Y();
	}

	/*! Determine the correct Anchor position */
	void CalculateAnchor() {
		PointType Index;
//		Index.X() = - RelativeCenter.Y() * RowVector.X() - RelativeCenter.X() * ColVector.X();
//		Index.Y() = - RelativeCenter.Y() * RowVector.Y() - RelativeCenter.X() * ColVector.Y();
		Index = -RelativeCenter;
		Anchor.SumOf(Index, CenterVector);
	}

	/*! Get an element, returning 0 if out of bound*/
	inline RealType GetElement(index_type row, index_type col) const{
		if(Data->ValidIndex(row,col)) {
		//	CMN_LOG_INIT_ERROR<<"row = "<<row<<",col = "<<col<<",data = "<<Data->Element(row,col)<<endl;
			return Data->Element(row, col);
		} else {
			return value_type(0);
		}
	}

	/*! Find the corect value for a point using linear interpolation */
	inline RealType LinearInterpolate(PointType & index) const{
		index_type row = static_cast<unsigned int>(index.Y());
		index_type col = static_cast<unsigned int>(index.X());
		RealType pHRow = index.Y() - row;
		RealType pHCol = index.X() - col;
		RealType pLRow = 1 - pHRow;
		RealType pLCol = 1 - pHCol;
	
		RealType result = pLRow*pLCol*GetElement(row  ,col  ) + 	
				pLRow*pHCol*GetElement(row  ,col+1) + 	
				pHRow*pLCol*GetElement(row+1,col  ) + 	
				pHRow*pHCol*GetElement(row+1,col+1);
		//CMN_LOG_INIT_ERROR<<"row = "<<row<<",col = "<<col<<"result = "<<result<<endl;
		return	result;
	}
	
	/*! The number of rows in this pseudo matrix*/
 	size_type Rows;
	/*! The number of columns in this pseudo matrix*/
    size_type Cols;

	/*! The underlying matrix.
	*/
	MatrixBaseType * Data;

};
/*!
	A Functional view of a matrix providing basic rotation, translation and scalling.
	Basically just initial headers for vctWarpFunctionMatrixOwner.
	
	\param _baseMatrixType The underlying matrix type

	\param RealType A floating point number type, for use in angle  and position calculation.
*
template< class _baseMatrixType, class RealType = double >
class vctWarpFunctionMatrix : public vctDynamicFunctionalMatrixBase<vctWarpFunctionMatrixOwner<_baseMatrixType, RealType> > {
	typedef typename vctDynamicFunctionalMatrixBase<vctWarpFunctionMatrixOwner<_baseMatrixType, RealType> > BaseType
	typedef typename _baseMatrixType
public:
	
	/! Default Initilization. Not valid to use *
	vctWarpFunctionMatrix() :
        Rows(0),
        Cols(0),
        Data(0),
		RowVector(1,0),
		ColVector(0,1),
		Anchor(0,0),
		CenterVector(0,0),
		RelativeCenter(0,0)
    {}
	
	/! Initialize to a given size and underlying Matrix *
    vctWarpFunctionMatrix(size_type rows, size_type cols, _baseMatrixType * data) :
        Rows(rows),
        Cols(cols),
        Data(data)
 		RowVector(0,1),
		ColVector(1,0),
		Anchor(0,0)
		CenterVector(rows/2, cols/2)
		RelativeCenter(0,0)
   {
   }
  
   vctWarpFunctionMatrix(size_type rows, size_type cols, _baseMatrixType * data) :
        Rows(rows),
        Cols(cols),
        Data(data)
 		RowVector(1,0),
		ColVector(0,1),
		Anchor(0,0),
		RelativeCenter(cols/2.0,rows/2.0)
   {}
};
*/
#endif // _vctWarpFunctionMatrix_h

