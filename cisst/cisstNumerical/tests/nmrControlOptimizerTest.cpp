/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 $Id: $
 
 Author(s):  Paul Wilkening
 Created on:
 
 (C) Copyright 2013 Johns Hopkins University (JHU), All Rights Reserved.
 
 --- begin cisst license - do not edit ---
 
 This software is provided "as is" under an open source license, with
 no warranty.  The complete license can be found in license.txt and
 http://www.cisst.org/cisst/license.txt.
 
 --- end cisst license ---
 */

#include <cisstCommon/cmnPath.h>
#include "nmrControlOptimizerTest.h"

/*! Test of constructor. */
void nmrControlOptimizerTest::TestConstructor(void)
{
	size_t num_vars = 5;
	nmrControlOptimizer co(num_vars);
	//Check number of variables
	CPPUNIT_ASSERT_EQUAL(co.GetNumVars(),num_vars);
}

/*! Test of reset_indices. */
void nmrControlOptimizerTest::TestResetIndices(void)
{
	size_t zero = 0;
	size_t num_vars = 5;
	nmrControlOptimizer co(num_vars);
	co.reset_indices();
	//Check that all indices and row counters are reset
	CPPUNIT_ASSERT_EQUAL(co.GetObjectiveIndex(),zero);
	CPPUNIT_ASSERT_EQUAL(co.GetObjectiveRows(),zero);
	CPPUNIT_ASSERT_EQUAL(co.GetIneqConstraintIndex(),zero);
	CPPUNIT_ASSERT_EQUAL(co.GetIneqConstraintRows(),zero);
	CPPUNIT_ASSERT_EQUAL(co.GetEqConstraintIndex(),zero);
	CPPUNIT_ASSERT_EQUAL(co.GetEqConstraintRows(),zero);
	CPPUNIT_ASSERT_EQUAL(co.GetSlackIndex(),zero);
	CPPUNIT_ASSERT_EQUAL(co.GetSlacks(),zero);
}

/*! Test of Reserve Space. */
void nmrControlOptimizerTest::TestReserveSpace(void)
{
	size_t num_vars = 5;
	size_t slack = 2;
	size_t eq = 4;
	size_t ineq = 6;
	size_t obj = 8;
    nmrControlOptimizer co(num_vars);
    co.reset_indices();
	co.ReserveSpace(obj,ineq,eq,slack);	
	//Check that values set in ReserveSpace are saved in counter variables
	CPPUNIT_ASSERT_EQUAL(co.GetObjectiveRows(),obj);	
	CPPUNIT_ASSERT_EQUAL(co.GetIneqConstraintRows(),ineq);	
	CPPUNIT_ASSERT_EQUAL(co.GetEqConstraintRows(),eq);
	CPPUNIT_ASSERT_EQUAL(co.GetSlacks(),slack);
}

/*! Test of allocate. */
void nmrControlOptimizerTest::TestAllocate(void)
{
	size_t num_vars = 5;
	size_t slack = 2;
	size_t eq = 4;
	size_t ineq = 6;
	size_t obj = 8;
    nmrControlOptimizer co(num_vars);
    co.reset_indices();
	co.ReserveSpace(obj,ineq,eq,slack);	
	co.allocate();
	//Check that matrices and vectors are allocated with memory dictated by ReserveSpace
	CPPUNIT_ASSERT_EQUAL(co.GetObjectiveMatrix().rows(),obj);
	CPPUNIT_ASSERT_EQUAL(co.GetObjectiveMatrix().cols(),num_vars+slack);
	CPPUNIT_ASSERT_EQUAL(co.GetObjectiveVector().size(),obj);
	CPPUNIT_ASSERT_EQUAL(co.GetIneqConstraintMatrix().rows(),ineq);
	CPPUNIT_ASSERT_EQUAL(co.GetIneqConstraintMatrix().cols(),num_vars+slack);
	CPPUNIT_ASSERT_EQUAL(co.GetIneqConstraintVector().size(),ineq);
	CPPUNIT_ASSERT_EQUAL(co.GetEqConstraintMatrix().rows(),eq);
	CPPUNIT_ASSERT_EQUAL(co.GetEqConstraintMatrix().cols(),num_vars+slack);
	CPPUNIT_ASSERT_EQUAL(co.GetEqConstraintVector().size(),eq);
}

/*! Test of GetObjectiveSpace. */
void nmrControlOptimizerTest::TestGetObjectiveSpace(void)
{
	size_t num_vars = 5;
	size_t slack = 2;
	size_t eq = 4;
	size_t ineq = 6;
	size_t obj = 8;
	double dataMat1 = 1;
	double slackMat1 = 2;
	double vec1 = 3;
	double dataMat2 = 4;
	double slackMat2 = 5;
	double vec2 = 6;
	double zero = 0;
    nmrControlOptimizer co(num_vars);
    co.reset_indices();
	co.ReserveSpace(obj,ineq,eq,slack);	
	co.allocate();

	//Get references to co tableau
	vctDynamicMatrixRef<double> objData1;
	vctDynamicMatrixRef<double> objSlack1;
	vctDynamicVectorRef<double> objVec1;
	vctDynamicMatrixRef<double> objData2;
	vctDynamicMatrixRef<double> objSlack2;
	vctDynamicVectorRef<double> objVec2;
	co.GetObjectiveSpace(4,0,1,objData1,objSlack1,objVec1);
	co.GetObjectiveSpace(4,1,1,objData2,objSlack2,objVec2);

	//Fill in some data, everywhere else is 0
	objData1.SetAll(dataMat1);
	objSlack1.SetAll(slackMat1);
	objVec1.SetAll(vec1);
	objData2.SetAll(dataMat2);
	objSlack2.SetAll(slackMat2);
	objVec2.SetAll(vec2);

	//Get tableau data
	vctDoubleMat objMat_in = co.GetObjectiveMatrix();
	vctDoubleVec objVec_in = co.GetObjectiveVector();
	//First set of vf data
	for(size_t i = 0; i < 4; i++)
	{
		for(size_t j = 0; j < num_vars; j++)
		{
			//Check that data portion of matrix is all correct
			CPPUNIT_ASSERT_EQUAL(objMat_in[i][j],dataMat1);
		}
		//Check that slacks are set
		CPPUNIT_ASSERT_EQUAL(objMat_in[i][num_vars],slackMat1);
		//Check that portions we don't have access to are set to 0 automatically
		CPPUNIT_ASSERT_EQUAL(objMat_in[i][num_vars+1],zero);
		//Check that the vector is set correctly
		CPPUNIT_ASSERT_EQUAL(objVec_in[i],vec1);
	}
	//Second set of vf data
	for(size_t i = 4; i < 8; i++)
	{
		for(size_t j = 0; j < num_vars; j++)
		{
			//Check that data portion of matrix is all correct
			CPPUNIT_ASSERT_EQUAL(objMat_in[i][j],dataMat2);
		}
		//Check that slacks are set
		CPPUNIT_ASSERT_EQUAL(objMat_in[i][num_vars+1],slackMat2);
		//Check that portions we don't have access to are set to 0 automatically
		CPPUNIT_ASSERT_EQUAL(objMat_in[i][num_vars],zero);
		//Check that the vector is set correctly
		CPPUNIT_ASSERT_EQUAL(objVec_in[i],vec2);
	}
}

/*! Test of GetIneqConstraintSpace. */
void nmrControlOptimizerTest::TestGetIneqConstraintSpace(void)
{
	size_t num_vars = 5;
	size_t slack = 2;
	size_t eq = 4;
	size_t ineq = 6;
	size_t obj = 8;
	double dataMat1 = 1;
	double slackMat1 = 2;
	double vec1 = 3;
	double dataMat2 = 4;
	double slackMat2 = 5;
	double vec2 = 6;
	double zero = 0;
    nmrControlOptimizer co(num_vars);
    co.reset_indices();
	co.ReserveSpace(obj,ineq,eq,slack);	
	co.allocate();

	//Get references to co tableau
	vctDynamicMatrixRef<double> ineqData1;
	vctDynamicMatrixRef<double> ineqSlack1;
	vctDynamicVectorRef<double> ineqVec1;
	vctDynamicMatrixRef<double> ineqData2;
	vctDynamicMatrixRef<double> ineqSlack2;
	vctDynamicVectorRef<double> ineqVec2;
	co.GetIneqConstraintSpace(3,0,1,ineqData1,ineqSlack1,ineqVec1);
	co.GetIneqConstraintSpace(3,1,1,ineqData2,ineqSlack2,ineqVec2);

	//Fill in some data, everywhere else is 0
	ineqData1.SetAll(dataMat1);
	ineqSlack1.SetAll(slackMat1);
	ineqVec1.SetAll(vec1);
	ineqData2.SetAll(dataMat2);
	ineqSlack2.SetAll(slackMat2);
	ineqVec2.SetAll(vec2);

	//Get tableau data
	vctDoubleMat ineqMat_in = co.GetIneqConstraintMatrix();
	vctDoubleVec ineqVec_in = co.GetIneqConstraintVector();
	//First set of vf data
	for(size_t i = 0; i < 3; i++)
	{
		for(size_t j = 0; j < num_vars; j++)
		{
			//Check data portion of matrix
			CPPUNIT_ASSERT_EQUAL(ineqMat_in[i][j],dataMat1);
		}
		//Check slack portion of matrix
		CPPUNIT_ASSERT_EQUAL(ineqMat_in[i][num_vars],slackMat1);
		//Check that portions of matrix we don't have references to are set to 0 automatically
		CPPUNIT_ASSERT_EQUAL(ineqMat_in[i][num_vars+1],zero);
		//Check that the vector is set correctly
		CPPUNIT_ASSERT_EQUAL(ineqVec_in[i],vec1);
	}
	//Second set of vf data
	for(size_t i = 3; i < 6; i++)
	{
		for(size_t j = 0; j < num_vars; j++)
		{
			//Check data portion of matrix
			CPPUNIT_ASSERT_EQUAL(ineqMat_in[i][j],dataMat2);
		}
		//Check slack portion of matrix
		CPPUNIT_ASSERT_EQUAL(ineqMat_in[i][num_vars+1],slackMat2);
		//Check that portions of matrix we don't have references to are set to 0 automatically
		CPPUNIT_ASSERT_EQUAL(ineqMat_in[i][num_vars],zero);
		//Check that the vector is set correctly
		CPPUNIT_ASSERT_EQUAL(ineqVec_in[i],vec2);
	}
}

/*! Test of GetEqConstraintSpace. */
void nmrControlOptimizerTest::TestGetEqConstraintSpace(void)
{
	size_t num_vars = 5;
	size_t slack = 2;
	size_t eq = 4;
	size_t ineq = 6;
	size_t obj = 8;
	double dataMat1 = 1;
	double slackMat1 = 2;
	double vec1 = 3;
	double dataMat2 = 4;
	double slackMat2 = 5;
	double vec2 = 6;
	double zero = 0;
    nmrControlOptimizer co(num_vars);
    co.reset_indices();
	co.ReserveSpace(obj,ineq,eq,slack);	
	co.allocate();

	//Get references to tableau
	vctDynamicMatrixRef<double> eqData1;
	vctDynamicMatrixRef<double> eqSlack1;
	vctDynamicVectorRef<double> eqVec1;
	vctDynamicMatrixRef<double> eqData2;
	vctDynamicMatrixRef<double> eqSlack2;
	vctDynamicVectorRef<double> eqVec2;
	co.GetEqConstraintSpace(2,0,1,eqData1,eqSlack1,eqVec1);
	co.GetEqConstraintSpace(2,1,1,eqData2,eqSlack2,eqVec2);

	//Fill in some data
	eqData1.SetAll(dataMat1);
	eqSlack1.SetAll(slackMat1);
	eqVec1.SetAll(vec1);
	eqData2.SetAll(dataMat2);
	eqSlack2.SetAll(slackMat2);
	eqVec2.SetAll(vec2);
	//Get tableau data
	vctDoubleMat eqMat_in = co.GetEqConstraintMatrix();
	vctDoubleVec eqVec_in = co.GetEqConstraintVector();
	//First set of vf data
	for(size_t i = 0; i < 2; i++)
	{
		for(size_t j = 0; j < num_vars; j++)
		{
			//Check data portion of matrix
			CPPUNIT_ASSERT_EQUAL(eqMat_in[i][j],dataMat1);
		}
		//Check slack portion of matrix
		CPPUNIT_ASSERT_EQUAL(eqMat_in[i][num_vars],slackMat1);
		//Check that portions of matrix we don't have references to are set to 0 automatically
 		CPPUNIT_ASSERT_EQUAL(eqMat_in[i][num_vars+1],zero);
		//Check that the vector is set correctly
		CPPUNIT_ASSERT_EQUAL(eqVec_in[i],vec1);
	}
	//Second set of vf data
	for(size_t i = 2; i < 4; i++)
	{
		for(size_t j = 0; j < num_vars; j++)
		{
			//Check data portion of matrix
			CPPUNIT_ASSERT_EQUAL(eqMat_in[i][j],dataMat2);
		}
		//Check slack portion of matrix
		CPPUNIT_ASSERT_EQUAL(eqMat_in[i][num_vars+1],slackMat2);
		//Check that portions of matrix we don't have references to are set to 0 automatically
		CPPUNIT_ASSERT_EQUAL(eqMat_in[i][num_vars],zero);
		//Check that the vector is set correctly
		CPPUNIT_ASSERT_EQUAL(eqVec_in[i],vec2);
	}
}

/*! Test of Slacks. */
void nmrControlOptimizerTest::TestSlacks(void)
{
	//Make sure we can increment the slack index manually by specific amounts
	size_t zero = 0;
	size_t one = 1;
	size_t three = 3;
	nmrControlOptimizer co(5);
	co.reset_indices();
	CPPUNIT_ASSERT_EQUAL(co.GetSlackIndex(),zero);
	co.IncSlackIndex(1);
	CPPUNIT_ASSERT_EQUAL(co.GetSlackIndex(),one);
	co.IncSlackIndex(2);
	CPPUNIT_ASSERT_EQUAL(co.GetSlackIndex(),three);
}

/*! Test of Solve. */
void nmrControlOptimizerTest::TestSolve(void)
{
	//Simple test case of min||I*J - vec||
	vctDoubleVec q(5);
	nmrControlOptimizer co(5);
	co.reset_indices();
	co.ReserveSpace(5,0,0,0);
	co.allocate();
	vctDynamicMatrixRef<double> Identity;
	vctDynamicMatrixRef<double> Slack;
	vctDynamicVectorRef<double> DesiredMotion;
	co.GetObjectiveSpace(5,0,0,Identity,Slack,DesiredMotion);
	Identity.SetAll(0);
	for(size_t i = 0; i < 5; i++)
	{
		Identity[i][i] = 1;
		DesiredMotion[i] = i;
	}
	co.Solve(q);
	for(size_t i = 0; i < 5; i++)
	{
		CPPUNIT_ASSERT_DOUBLES_EQUAL(q[0],DesiredMotion[0],0.05);
	}
}

CPPUNIT_TEST_SUITE_REGISTRATION(nmrControlOptimizerTest);