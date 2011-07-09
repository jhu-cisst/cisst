#ifndef _svlCCHandEyeCalibration_h
#define _svlCCHandEyeCalibration_h

#ifndef _cv_h
#include <cv.h>
#endif
#include <cisstCommon/cmnGetChar.h>
#include <cisstStereoVision/svlTypes.h>
#include <cisstStereoVision/svlCCCalibrationGrid.h>
#include <limits>

class svlCCHandEyeCalibration
{
	public:
		enum handEyeMethodEnum {DUAL_QUATERNION};
		svlCCHandEyeCalibration(std::vector<svlCCCalibrationGrid*> calibrationGrids);
		bool calibrate();
        vct4x4 tcp_T_camera;

	private:
		std::vector<svlCCCalibrationGrid*> calibrationGrids;
		float dualQuaternionMethod();
		bool getDualQuaternion(CvMat* matrix, CvMat* q, CvMat* qPrime);
		void populateComplexMatrixST(CvMat* a, CvMat* b, CvMat* aPrime, CvMat* bPrime, CvMat* s, CvMat* T, int index);
		void quaternionMul(CvMat* q1, CvMat* q2, CvMat* result);
		void quaternionToRMatrix(CvMat* rMatrix, CvMat* quaternion);
		void solveQuadratic(double a, double b, double c, CvMat* roots);
		void printData();
		void printCvMatDouble(CvMat* matrix);

		//parameters
		bool debug;
		int handEyeMethodFlag;
		std::vector<CvMat*> cameraMatrix;
		std::vector<CvMat*> worldToTCPMatrix;
		std::vector<CvMat*> tcpToWorldMatrix;
		CvMat* cameraToTCP;
};

#endif