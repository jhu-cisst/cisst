/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):	Ankur Kapoor
  Created on: 2004-10-26

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of nmrCallBackLSNonLinSolver
*/
#ifndef _nmrCallBackLSNonLinSolver_h
#define _nmrCallBackLSNonLinSolver_h


#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctDynamicVectorRef.h>
#include <cisstVector/vctDynamicMatrixRef.h>

#include <cisstNumerical/nmrNetlib.h>

#define nmrUNIQUE_IDENTIFIER_LINE ( __LINE__ + 0x0000 )
#if CISST_OS == CISST_WINDOWS
#define CISST_DECLSPEC_THREAD __declspec( thread )
#else
#define CISST_DECLSPEC_THREAD
#endif

template <int _instanceLine, class _elementType>
class nmrCallBackFunctionF  {
protected:
	_elementType *Object;
	int (_elementType::*MethodPointer)(vctDynamicVectorRef<double> &X,
			vctDynamicVectorRef<double> &Y, long int &Flag);
	CISST_DECLSPEC_THREAD static nmrCallBackFunctionF* This;
public:
	nmrCallBackFunctionF():Object(0), MethodPointer(0) {
		nmrCallBackFunctionF::This = NULL;
	}
	nmrCallBackFunctionF(_elementType *obj, int (_elementType::*methodPointer)
			(vctDynamicVectorRef<double> &X,
			vctDynamicVectorRef<double> &Y, long int &Flag)):Object(obj), 
			MethodPointer(methodPointer) {
		nmrCallBackFunctionF::This = this;
	}
	void Set(_elementType *obj, int (_elementType::*methodPointer)
			(vctDynamicVectorRef<double> &X,
			vctDynamicVectorRef<double> &Y, long int &Flag)) {
		Object = obj;
		MethodPointer = methodPointer;
		nmrCallBackFunctionF::This = this;
	}
	/* static function to be passed to lmdif */
	static int FunctionFlmdif (long int *m, long int *n, double *x, double *fvec,
			long int *iflag) {
		if (nmrCallBackFunctionF::This != NULL) {
			vctDynamicVectorRef<double> XRef(*n, x);
			vctDynamicVectorRef<double> FRef(*m, fvec);
			return (This->Object->*This->MethodPointer)(XRef, FRef, *iflag);
		}
		return -1;
	}
	/* static function to be passed to hybrd */
	static int FunctionFhybrd (long int *n, double *x, double *fvec,
			long int *iflag) {
		if (nmrCallBackFunctionF::This != NULL) {
			vctDynamicVectorRef<double> XRef(*n, x);
			vctDynamicVectorRef<double> FRef(*n, fvec);
			return (This->Object->*This->MethodPointer)(XRef, FRef, *iflag);
		}
		return -1;
	}
	/* static function to be passed to donlp2 */
	static int FunctionFdonlp2 (long int *n, double x[], double fgraf[]) {
		static long int iflag = 0;
		if (nmrCallBackFunctionF::This != NULL) {
			vctDynamicVectorRef<double> XRef(*n, x);
			vctDynamicVectorRef<double> FRef(*n, fgraf);
			return (This->Object->*This->MethodPointer)(XRef, FRef, iflag);
		}
		return -1;
	}
};

#ifndef DOXYGEN
template <int _instanceLine, class _elementType>
nmrCallBackFunctionF <_instanceLine, _elementType>* nmrCallBackFunctionF<_instanceLine, _elementType>::This = NULL;
#endif // DOXYGEN

template <int _instanceLine, class _elementType>
class nmrCallBackFunctionJ {
protected:
	/* pointer to object that contains the method to be called */
	_elementType* Object;

	/* pointer to method that needs to be called */
	int (_elementType::*MethodPointer)(vctDynamicVectorRef<double> &X,
			vctDynamicVectorRef<double> &J, long int &Flag);

	/* a static pointer to this object. Note only one type of this can be created.
	   uniqueness is ensured by modifying the template parameter
	 */
	CISST_DECLSPEC_THREAD static nmrCallBackFunctionJ* This;
public:

	/* constructor */
	/* default */
	nmrCallBackFunctionJ ():Object(0), MethodPointer(0) {
		nmrCallBackFunctionJ::This = NULL;
	}
	nmrCallBackFunctionJ (_elementType* obj, int (_elementType::*methodPointer)(vctDynamicVectorRef<double> &X,
				vctDynamicVectorRef<double> &J, long int &Flag)):
		Object(obj), MethodPointer(methodPointer) {
			nmrCallBackFunctionJ::This = this;
		}

	/* set value other than constructor */
	void Set(_elementType* obj, int (_elementType::*methodPointer)(vctDynamicVectorRef<double> &X,
				vctDynamicVectorRef<double> &J, long int &Flag)) {
		Object = obj;
		MethodPointer = methodPointer;
		nmrCallBackFunctionJ::This = this;
	}
	
	/* static function to be passed to hybrj2 */
	static int FunctionFhybrj2 (long int *n, double *x, double *fdjac,
			long int *ldfjac, long int *iflag) {
		if (nmrCallBackFunctionJ::This != NULL) {
			vctDynamicVectorRef<double> XRef(*n, x);
			vctDynamicVectorRef<double> JRef((*ldfjac)*(*n), fdjac);
			return (This->Object->*This->MethodPointer)(XRef, JRef, *iflag);
		}
		return -1;
	}
	
	/* static function to be passed to hybrj2 */
	static int FunctionFlmder2 (long int *m, long int *n, double *x, double *fdjac,
			long int *ldfjac, long int *iflag) {
		if (nmrCallBackFunctionJ::This != NULL) {
			vctDynamicVectorRef<double> XRef(*n, x);
			vctDynamicVectorRef<double> JRef((*ldfjac)*(*n), fdjac);
			return (This->Object->*This->MethodPointer)(XRef, JRef, *iflag);
		}
		return -1;
	}
};

#ifndef DOXYGEN
template <int _instanceLine, class _elementType>
nmrCallBackFunctionJ <_instanceLine, _elementType>* nmrCallBackFunctionJ<_instanceLine, _elementType>::This = NULL;
#endif // DOXYGEN

template <int _instanceLine, class _elementType>
class nmrCallBackFunctionF1 {
protected:
	_elementType *Object;
	void (_elementType::*MethodPointer)(vctDynamicVectorRef<double> &X, double *F);
	CISST_DECLSPEC_THREAD static nmrCallBackFunctionF1* This;
public:
	nmrCallBackFunctionF1():Object(0), MethodPointer(0) {
		nmrCallBackFunctionF1::This = NULL;
	}
	nmrCallBackFunctionF1(_elementType *obj, void (_elementType::*methodPointer)
			(vctDynamicVectorRef<double> &X, double *F)):Object(obj), 
			MethodPointer(methodPointer) {
		nmrCallBackFunctionF1::This = this;
	}
	void Set(_elementType *obj, void (_elementType::*methodPointer)
			(vctDynamicVectorRef<double> &X, double *F)) {
		Object = obj;
		MethodPointer = methodPointer;
		nmrCallBackFunctionF1::This = this;
	}
	/* static function to be passed to donlp2 */
	static void FunctionFdonlp2 (int n, double x[], double *fx) {
		if (nmrCallBackFunctionF1::This != NULL) {
			vctDynamicVectorRef<double> XRef(n+1, x);
			(This->Object->*This->MethodPointer)(XRef, fx);
		}
	}
};

#ifndef DOXYGEN
template <int _instanceLine, class _elementType>
nmrCallBackFunctionF1 <_instanceLine, _elementType>* nmrCallBackFunctionF1<_instanceLine, _elementType>::This = NULL;
#endif // DOXYGEN

template <int _instanceLine, class _elementType>
class nmrCallBackFunctionC {
protected:
	_elementType *Object;
	void (_elementType::*MethodPointer)(int Type, vctDynamicVectorRef<int> &ListE,
			vctDynamicVectorRef<double> &X, vctDynamicVectorRef<double> &Con,
			vctDynamicVectorRef<int> &Err);
	CISST_DECLSPEC_THREAD static nmrCallBackFunctionC* This;
public:
	nmrCallBackFunctionC():Object(0), MethodPointer(0) {
		nmrCallBackFunctionC::This = NULL;
	}
	nmrCallBackFunctionC(_elementType *obj, void (_elementType::*methodPointer)
			(int Type, vctDynamicVectorRef<int> &ListE,
			vctDynamicVectorRef<double> &X, vctDynamicVectorRef<double> &Con,
			vctDynamicVectorRef<int> &Err)):Object(obj), 
			MethodPointer(methodPointer) {
		nmrCallBackFunctionC::This = this;
	}
	void Set(_elementType *obj, void (_elementType::*methodPointer)
			(int Type, vctDynamicVectorRef<int> &ListE,
			vctDynamicVectorRef<double> &X, vctDynamicVectorRef<double> &Con,
			vctDynamicVectorRef<int> &Err)) {
		Object = obj;
		MethodPointer = methodPointer;
		nmrCallBackFunctionC::This = this;
	}
	/* static function to be passed to donlp2 */
	static void FunctionFdonlp2 (int n, int nonlin, int type ,int liste[],
			double x[],double con[], int err[]) {
		if (nmrCallBackFunctionC::This != NULL) {
			vctDynamicVectorRef<double> XRef(n+1, x);
			vctDynamicVectorRef<double> ConRef(nonlin+1, con);
			vctDynamicVectorRef<int> ListERef(liste[0]+1, liste);
			vctDynamicVectorRef<int> ErrRef(nonlin+1, err);
			(This->Object->*This->MethodPointer)(type, ListERef, XRef, ConRef, ErrRef);
#if 0
			fprintf(stderr, "%d, %d, %d", n, nonlin, type);
			for (int l = 1; l <= liste[0]; l++) fprintf(stderr, "%d, ", liste[l]);
			for (int bb = 1; bb <= n; bb++) fprintf(stderr, "%f, ", x[bb]);
			for (int cc = 1; cc <= nonlin; cc++) fprintf(stderr, "%f, ", con[cc]);
			for (int dd = 1; dd <= nonlin; dd++) fprintf(stderr, "%d, ", err[dd]);
			fprintf(stderr, "\n");
#endif
		}
	}
};

#ifndef DOXYGEN
template <int _instanceLine, class _elementType>
nmrCallBackFunctionC <_instanceLine, _elementType>* nmrCallBackFunctionC<_instanceLine, _elementType>::This = NULL;
#endif // DOXYGEN

#if 0
template <int _instanceLine, class _elementType>
class nmrCallBackFunctionCG {
protected:
	_elementType *Object;
	int (_elementType::*MethodPointer)(vctDynamicVectorRef<int> &ListE, long int Shift,
			vctDynamicVectorRef<double> &X, vctDynamicMatrixRef<double> &Grad);
	CISST_DECLSPEC_THREAD static nmrCallBackFunctionCG* This;
public:
	nmrCallBackFunctionCG():Object(0), MethodPointer(0) {
		nmrCallBackFunctionCG::This = NULL;
	}
	nmrCallBackFunctionCG(_elementType *obj, int (_elementType::*methodPointer)
			(vctDynamicVectorRef<int> &ListE, long int Shift,
			 vctDynamicVectorRef<double> &X, vctDynamicVectorRef<double> &Grad)):
		Object(obj), 
			MethodPointer(methodPointer) {
		nmrCallBackFunctionCG::This = this;
	}
	void Set(_elementType *obj, int (_elementType::*methodPointer)
			(vctDynamicVectorRef<int> &ListE, long int Shift,
			 vctDynamicVectorRef<double> &X, vctDynamicVectorRef<double> &Grad))
	{
		Object = obj;
		MethodPointer = methodPointer;
		nmrCallBackFunctionCG::This = this;
	}
	/* static function to be passed to donlp2 */
	static int FunctionFdonlp2 (long int n, long int nonlin, long int liste[] ,long int shift ,  double x[],
			               double **grad) {
		static int iflag = 0;
		if (nmrCallBackFunctionCG::This != NULL) {
			vctDynamicVectorRef<double> XRef(n+1, x);
			vctDynamicMatrixRef<double> GradRef(*n, x);
			vctDynamicVectorRef<double> XRef(*n, x);
			vctDynamicVectorRef<double> XRef(*n, x);
			return (This->Object->*This->MethodPointer)(XRef, *fx, iflag);
		}
		return -1;
	}
};
template <int _instanceLine, class _elementType>
nmrCallBackFunctionCG <_instanceLine, _elementType>* nmrCallBackFunctionCG<_instanceLine, _elementType>::This = NULL;
#endif

#endif // _nmrCallBackLSNonLinSolver_h
