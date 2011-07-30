#ifndef _svlCCHandEyeCalibration_h
#define _svlCCHandEyeCalibration_h

#include <cisstStereoVision/svlTypes.h>
#include <cisstCommon/cmnGetChar.h>
#include <cisstStereoVision/svlTypes.h>
#include <cisstStereoVision/svlCCCalibrationGrid.h>
#include <limits>

class svlCCHandEyeCalibration
{
    public:
		enum handEyeMethodEnum {DUAL_QUATERNION};
		svlCCHandEyeCalibration(std::vector<svlCCCalibrationGrid*> calibrationGrids);
		double calibrate();
        vct4x4 tcp_T_camera;

	private:
		std::vector<svlCCCalibrationGrid*> calibrationGrids;
        double optimizeDualQuaternionMethod();
        double getAvgHandEyeError(std::vector<CvMat*> aMatrix, std::vector<CvMat*> bMatrix);
		double dualQuaternionMethod(int* indicies, int indiciesSize, bool runCombination);
		bool getDualQuaternion(CvMat* matrix, CvMat* q, CvMat* qPrime);
		void populateComplexMatrixST(CvMat* a, CvMat* b, CvMat* aPrime, CvMat* bPrime, CvMat* s, CvMat* T, int index);
		void quaternionMul(CvMat* q1, CvMat* q2, CvMat* result);
		void quaternionToRMatrix(CvMat* rMatrix, CvMat* quaternion);
		void solveQuadratic(double a, double b, double c, CvMat* roots);
		void printData();
		void printCvMatDouble(CvMat* matrix);
        double checkAXXB(CvMat* A, CvMat* B);
        template <typename Iterator> bool next_combination(const Iterator first, Iterator k, const Iterator last);

		//parameters
		bool debug;
		int handEyeMethodFlag;
		std::vector<CvMat*> cameraMatrix;
		std::vector<CvMat*> worldToTCPMatrix;
		CvMat* cameraToTCP;
        double handEyeAvgError;
        int** valid;
};

#endif
