#ifndef _ccalHandEyeCalibration_h
#define _ccalHandEyeCalibration_h
#include <string>
#include "ccalCalibrationGrid.h"
static const double pi = 3.14159265358979323846;

class ccalHandEyeCalibration
{
	public:
		enum handEyeMethodEnum { DUAL_QUATERNION};
		ccalHandEyeCalibration(std::vector<ccalCalibrationGrid*> calibrationGrids);
		void calibrate();

	private:
		std::vector<ccalCalibrationGrid*> calibrationGrids;
		float dualQuaternionMethod();
		void getDualQuaternion(CvMat* matrix, CvMat* q, CvMat* qPrime);
		void getDualQuaternionST(CvMat* a, CvMat* b, CvMat* aPrime, CvMat* bPrime, CvMat* s, CvMat* T, int index);
		void quaternionMul(CvMat* q1, CvMat* q2, CvMat* result);
		void quaternionToRMatrix(CvMat* rMatrix, CvMat* quaternion);
		void solveQuadratic(double a, double b, double c, CvMat* roots);
		void printData();
		void printCvMatDouble(CvMat* matrix);

		//parameters
		const static bool debug = false;
		int handEyeMethodFlag;
		std::vector<CvMat*> cameraMatrix;
		std::vector<CvMat*> worldToTCPMatrix;
		std::vector<CvMat*> tcpToWorldMatrix;
		CvMat* cameraToTCP;
};

#endif