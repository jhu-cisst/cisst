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


#ifndef _vctExtraTemplateFunctions_h
#define _vctExtraTemplateFunctions_h



//*******************************************************************
//                   Extra Template Functions
//*******************************************************************

//Basic class for computing weighted sums, i.e. bulding block for correlation and convolution
template <class __matrixOwnerType1, class __matrixOwnerType2 /*= __matrixOwnerType1*/, class _elementType1 /*= _outputType*/, class _elementType2/* = _outputType*/ >
// _elementType=value_type if in class
//Change double to outputType, simple test passed
inline double SumOfElementwiseProductOf(const vctDynamicConstMatrixBase<__matrixOwnerType1, _elementType1> & matrix1,
                                        const vctDynamicConstMatrixBase<__matrixOwnerType2, _elementType2> & matrix2 ) /*const*/ {
    return vctDynamicMatrixLoopEngines::SoMiMi< typename vctBinaryOperations<double>::Addition,
	typename vctBinaryOperations<double,_elementType1,_elementType2>::Multiplication >::Run(matrix1,matrix2);
} 

//Might be a good member for Matrix class, particularly given the lack of references possible. Inline + private references?
template< class _input1MatrixType, class _input2MatrixType >
inline bool isSameSize(const _input1MatrixType & input1Matrix,
		       const _input2MatrixType & input2Matrix) {
    return (input1Matrix.rows()==input2Matrix.rows()) &&
	(input1Matrix.cols()==input2Matrix.cols());
}

//Might be a good member for Matrix class, particularly given the lack of references possible. Inline + private references?
template< class _input1MatrixType, class _input2MatrixType >
inline bool isDifferentSize(const _input1MatrixType & input1Matrix,
			    const _input2MatrixType & input2Matrix) {
    return (input1Matrix.rows()!=input2Matrix.rows()) ||
	(input1Matrix.cols()!=input2Matrix.cols());
}

//Might be a good member for Matrix class, particularly given the lack of references possible. Inline + private references?
//Could also do this with inputMatrix.size() != 0; Which is faster?
template< class _inputMatrixType >
inline bool isDegenerate(const _inputMatrixType & inputMatrix) {
    return ( inputMatrix.rows()<=0 || inputMatrix.cols()<=0 );
}

//From Dynamix Loop Engines header
inline static void ThrowException(void) throw(std::runtime_error) {
    throw std::runtime_error("vctDynamicMatrixLoopEngines: Sizes of matrices don't match");
}

// A dynamic matrix agorithm, combines i(r,c), o(r-1,c), o(r,c-1), o(r-1,c-1)
// Op1 and Op2 are Operators with overloaded functions for Dynamic Operation 
// out1, out2 and in are input/output matrixes
template<class Op1, class Op2>
class MoMoMiDyn {
public:
    template<class Out1Type, class Out2Type, class InType>
    static void Run(Out1Type & out1, Out2Type & out2, const InType & self) {
        // check sizes
        if (isDifferentSize(out1,self) || isDifferentSize(out2,self) || isDegenerate(self)) {
            ThrowException();
        }
	
	// otherwise
	typedef typename InType::size_type size_type;
        const size_type rows = self.rows();
        const size_type cols = self.cols();
	
	size_type rowIndex = 0, colIndex = 0, rowIndexLag, colIndexLag;
	
	//Perform the coner operation
        out1.Element(rowIndex, colIndex) = Op1::Operate(self.Element(rowIndex, colIndex));
        out2.Element(rowIndex, colIndex) = Op2::Operate(self.Element(rowIndex, colIndex));
	//Perform the first complete row
        for (colIndex = 1, colIndexLag = 0; colIndex < cols; ++colIndex,++colIndexLag) {
            out1.Element(rowIndex, colIndex) = Op1::Operate(
							    self.Element(rowIndex, colIndex), out1.Element(rowIndex, colIndexLag));
            out2.Element(rowIndex, colIndex) = Op2::Operate(
							    self.Element(rowIndex, colIndex), out2.Element(rowIndex, colIndexLag));
        }
	//Perform for the rest of the rows
	for (rowIndex = 1,rowIndexLag = 0; rowIndex < rows; ++rowIndex, ++rowIndexLag) {
            //Perform colindex = 0
	    colIndex = 0;
	    out1.Element(rowIndex, colIndex) = Op1::Operate(
							    self.Element(rowIndex, colIndex), out1.Element(rowIndexLag, colIndex));
            out2.Element(rowIndex, colIndex) = Op2::Operate(
							    self.Element(rowIndex, colIndex), out2.Element(rowIndexLag, colIndex));
	    //Perform for the rest of the colunms
	    for (colIndex = 1, colIndexLag = 0; colIndex < cols; ++colIndex, ++colIndexLag) {
                out1.Element(rowIndex, colIndex) = Op1::Operate( 
								self.Element(rowIndex, colIndex   ), out1.Element(rowIndexLag, colIndex   ),
								out1.Element(rowIndex, colIndexLag), out1.Element(rowIndexLag, colIndexLag));
                out2.Element(rowIndex, colIndex) = Op2::Operate( 
								self.Element(rowIndex, colIndex   ), out2.Element(rowIndexLag, colIndex   ),
								out2.Element(rowIndex, colIndexLag), out2.Element(rowIndexLag, colIndexLag));
            }
        }
    }
};

//For quickly computing the area under the template, see fast NNC
//Algorithm or OpenCV Integral function
template<class OutputType, class Input1Type = OutputType, class Input2Type = Input1Type,
	 class Input3Type = Input1Type, class Input4Type = Input1Type>
class DynamicRunningSum {
public:
    static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2, 
				     const Input3Type & input3, const Input4Type & input4) {
	return OutputType(input1+input2+input3-input4);
    }
    static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
	return OutputType(input1+input2);
    }
    static inline OutputType Operate(const Input1Type & input1) {
	return OutputType(input1);
    }
};

//Similar to above, except that it computes the running sum of the image squared
template<class OutputType, class Input1Type = OutputType, class Input2Type = Input1Type,
	 class Input3Type = Input1Type, class Input4Type = Input1Type>
class DynamicRunningSqSum {
public:
    static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2, 
				     const Input3Type & input3, const Input4Type & input4) {
	return OutputType(input1*input1+input2+input3-input4);
    }
    static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
	return OutputType(input1*input1+input2);
    }
    static inline OutputType Operate(const Input1Type & input1) {
	return OutputType(input1*input1);
    }
};

//Calculates sum and sumsq in one nice easy to use function
template < class __matrixOwnerType1, class __matrixOwnerType2,
	   class __matrixOwnerType3 , class  _elementType1,
	   class _elementType2, class _elementType3>
    // _elementType=value_type if in class
    inline void runIntegral( const vctDynamicConstMatrixBase<__matrixOwnerType1, _elementType1> & self,
			     vctDynamicMatrixBase<__matrixOwnerType2, _elementType2> & sum,
			     vctDynamicMatrixBase<__matrixOwnerType3, _elementType3> & sqsum ) /*const*/ {
    return MoMoMiDyn<DynamicRunningSum<_elementType2, _elementType1>, 
	             DynamicRunningSqSum<_elementType3, _elementType1> >::Run(sum, sqsum, self);
} 

//Class instance based loop structure for more advance processing than a simple inline function
//Very simple modification from existing code base
//Use of pointer over a element + reference required for correct implementation
//As otherwise a copy of the data is generated and not a reference to the orignal
//memory structure.
template<class ElementOperation>
class MoMi_f {
public:
    template<class OutputMatrix, class InputMatrix>
    static inline void Run(ElementOperation & op, 
						OutputMatrix & out, 
                        const InputMatrix & self) {
        // check sizes
        if (isDifferentSize(out,self) || isDegenerate(self)) {
            ThrowException();
        }
        // otherwise
        typedef typename OutputMatrix::size_type size_type;
        const size_type rows = out.rows();
        const size_type cols = out.cols();
		size_type rowIndex, colIndex;
        for (rowIndex = 0; rowIndex < rows; ++rowIndex) {
            for (colIndex = 0; colIndex < cols; ++colIndex) {
				out.Element(rowIndex, colIndex) = op.operate(self.Pointer(rowIndex, colIndex));
            }
        }
    }
};
//This has a hard cast as an iput argument, should fix.
template<class Output, class Data, class Kernel >
class ApplyKernel {
private:
    typedef typename Data::Submatrix::Type SubMatrix;
    typedef typename Output::index_type index_type;
    typedef typename Output::value_type value_type;
    typedef typename SubMatrix::pointer subptr;
    //typedef typename Data::reference data_ref;
    SubMatrix window;
    Kernel *ptrKernel;
public:
    inline ApplyKernel() {}
    inline ~ApplyKernel() {}
    void inline setup(Data & data, Kernel & kernel) { 
        window.SetRef(kernel.rows(), kernel.cols(), data.row_stride(), data.col_stride(), data.Pointer());
        ptrKernel = &kernel;
    }
    template<class data_ref>
    inline value_type operate(data_ref windowCorner) {
        window.SetRef(window.rows(), window.cols(), window.row_stride(), window.col_stride(), subptr(windowCorner));
        return value_type(SumOfElementwiseProductOf(window,*ptrKernel));
    }
};

//In furture incorporate this into the class to save on memory instances/time/etc
template <class __matrixOwnerType1, class __matrixOwnerType2 /*= __matrixOwnerType1*/, class __matrixOwnerType3 /*= __matrixOwnerType1*/, class _elementType1 /*= _outputType*/, class _elementType2/* = _outputType*/,  class _elementType3/* = _outputType*/>
inline void correlation( vctDynamicMatrixBase<__matrixOwnerType1, _elementType1> & data,
			 vctDynamicMatrixBase<__matrixOwnerType2, _elementType2> & kernel,
			 vctDynamicMatrixBase<__matrixOwnerType3, _elementType3> & output) /*const*/ {
    typedef  vctDynamicMatrixBase<__matrixOwnerType1, _elementType1> DataType;
    typedef  vctDynamicMatrixBase<__matrixOwnerType2, _elementType2> KernelType;
    typedef  vctDynamicMatrixBase<__matrixOwnerType3, _elementType3> OutputType;
    typedef typename DataType::Submatrix::Type SubMatrix;
    //Setup the sub window of initial data pointers, should be equal to the size of the output
    SubMatrix subData(data,0,0,(data.rows()-kernel.rows()+1),(data.cols()-kernel.cols()+1));
    //subData.SetRef((data.rows()-kernel.rows()+1), (data.cols()-kernel.cols()+1), data.row_stride(), data.col_stride(), data.Pointer());
    //Typecheck
    if (isDegenerate(kernel)) {
        ThrowException();
    }
    //Create an appropriate functional operation
    typedef ApplyKernel<OutputType,DataType,KernelType> operation;
    operation op;
    //Make it the size of the template
    op.setup(data,kernel);
    return MoMi_f< operation >::Run(op, output, subData);
} 


//Class instance based loop structure for more advance processing than a simple inline function
//Very simple modification from existing code base
//Use of pointer over a element + reference required for correct implementation
//As otherwise a copy of the data is generated and not a reference to the orignal
//memory structure.
//For what I'm using this for, we would want vector output, possible?
//Currently, data is updated/saved in the class structure
template<class ElementOperation>
class Mi_f {
public:
    template<class InputMatrix>
    static inline void Run(ElementOperation & op, 
			   const InputMatrix & self) {
        // check sizes
        if (isDegenerate(self)) {
            ThrowException();
        }
        // otherwise
        typedef typename InputMatrix::size_type size_type;
        const size_type rows = self.rows();
        const size_type cols = self.cols();
		size_type rowIndex, colIndex;
        for (rowIndex = 0; rowIndex < rows; ++rowIndex) {
            for (colIndex = 0; colIndex < cols; ++colIndex) {
		op.operate(rowIndex, colIndex, self.Pointer(rowIndex, colIndex));
            }
        }
    }
};

//Finds the locations goverened by the boolean function
// if op(Value,OldValue) Update location/ooldvalue
template<class BoolOp, class index_type, class value_type>
class FindOpLocation {
private:
    //	typedef typename BoolOp::Input1Type value_type; //Can't use this the way the library is written
    value_type previous;
    index_type r;
    index_type c;
public:
    inline FindOpLocation() {}
    inline ~FindOpLocation() {}
    inline void initialize( value_type * initial_condition, index_type row, index_type col) {
	previous = *initial_condition;
	r = row;
	c = col;
    }
    inline void operate(index_type &row, index_type &col, const value_type * next) {
	if( BoolOp::Operate(*next,previous) ) {
	    r = row;
	    c = col;
	    previous = *next;
	}
    }
    inline value_type getValue() {
	return previous;
    }
    inline index_type getRow() {
	return r;
    }
    inline index_type getCol() {
	return c;
    }
};

template <class mOwnerType, class value_type, class index_type>
inline void findMax( vctDynamicMatrixBase<mOwnerType, value_type> & data,
		     index_type & row, index_type & col, value_type & max_value ) /*const*/ {
    typedef vctDynamicMatrixBase<mOwnerType, value_type> Matrix;
    typedef FindOpLocation<typename vctBinaryOperations<bool,value_type,value_type>::Greater, 
	                   typename Matrix::index_type, typename Matrix::value_type > Op;
    Op op;
    op.initialize(data.Pointer(),0,0); //Use the 0,0 value;
    Mi_f<Op>::template Run<Matrix>(op,data);
    row = op.getRow();
    col = op.getCol();
    max_value = op.getValue();
}

//A set of functions and basic operations for doing elementwize sqrt's
//Not safe for vector input use, will have to specialize to give different function.
template<class InputOutputElementType>
class MakeSquareRoot {
public:
    static inline InputOutputElementType Operate(InputOutputElementType & inputOutput) {
	return (inputOutput = InputOutputElementType(sqrt(inputOutput)));
    }
};

template <class mOwnerType, class value_type>
inline vctDynamicMatrixBase<mOwnerType, value_type> &
SquareRootSelf(vctDynamicMatrixBase<mOwnerType, value_type> & self) {
    vctDynamicMatrixLoopEngines::Mio<MakeSquareRoot<value_type> >::Run(self);
    return self;
};
template<class _vectorOwnerType, typename _elementType>
inline const vctDynamicVectorBase<_vectorOwnerType,_elementType> & SquareRootSelf(
										  vctDynamicVectorBase<_vectorOwnerType,_elementType> & self) {
    vctDynamicVectorLoopEngines::
        Vio<MakeSquareRoot < _elementType > >::
        Run(self);
    return self;
};


#endif // _vctExtraTemplateFunctions_h

