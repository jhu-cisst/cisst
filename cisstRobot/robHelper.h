#ifndef _osaHelper_
#define _osaHelper_

#include <cisstVector/vctDynamicMatrixTypes.h>
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstVector/vctFixedSizeMatrixTypes.h>
#include <cisstVector/vctTransformationTypes.h>

namespace Jacobian{

static void ChangeBase(const vctDoubleMat & src, const vctRot3 & Rt, vctDoubleMat & out )
{
    if(src.cols() != 6) {
        std::cerr << "ChangeBase: Jacobian matrix size error" << std::endl;
        return;
    }

    out.SetSize(src.rows(), src.cols());

    vctDoubleMat Ad(6, 6);
    Ad.Zeros(); // fast zero using memset

    // upper left block
    Ad[0][0] = Rt[0][0];     Ad[0][1] = Rt[0][1];     Ad[0][2] = Rt[0][2];
    Ad[1][0] = Rt[1][0];     Ad[1][1] = Rt[1][1];     Ad[1][2] = Rt[1][2];
    Ad[2][0] = Rt[2][0];     Ad[2][1] = Rt[2][1];     Ad[2][2] = Rt[2][2];

    // upper right block
    Ad[0][3] = 0.0;
    Ad[0][4] = 0.0;
    Ad[0][5] = 0.0;

    Ad[1][3] = 0.0;
    Ad[1][4] = 0.0;
    Ad[1][5] = 0.0;

    Ad[2][3] = 0.0;
    Ad[2][4] = 0.0;
    Ad[2][5] = 0.0;

    // lower right block
    Ad[3][3] = Rt[0][0];     Ad[3][4] = Rt[0][1];     Ad[3][5] = Rt[0][2];
    Ad[4][3] = Rt[1][0];     Ad[4][4] = Rt[1][1];     Ad[4][5] = Rt[1][2];
    Ad[5][3] = Rt[2][0];     Ad[5][4] = Rt[2][1];     Ad[5][5] = Rt[2][2];

    out = Ad * src;
}

static void ChangeBase(const vctDoubleMat &src, const vctFrm4x4 & Frm, vctDoubleMat &out)
{
    vctRot3 rt;
    rt.FromNormalized(Frm.Rotation());
    ChangeBase(src, rt, out);
}

static vctDoubleMat Transpose(const vctDoubleMat & jac)
{
    vctDoubleMat trans;
    trans.SetSize(jac.cols(), jac.rows());

    for (int i = 0; i < trans.rows(); ++i) {
        for (int j = 0; j < trans.cols(); ++j) {
            trans[i][j] = jac[j][i];
        }
    }

    return trans;
}

}

namespace Util {

static void Skew(const vct3 & inVec, vctDouble3x3 & outSkew)
{
    //skew of an input vector
    outSkew.Element(0,0) = 0.0;          outSkew.Element(0,1) = -inVec.Z();       outSkew.Element(0,2) = inVec.Y();
    outSkew.Element(1,0) = inVec.Z();    outSkew.Element(1,1) = 0.0;              outSkew.Element(1,2) = -inVec.X();
    outSkew.Element(2,0 )= -inVec.Y();   outSkew.Element(2,1) = inVec.X();        outSkew.Element(2,2) = 0.0;
}

/*
  inFrame - Frame B to A
  inForce - Force at frame A
  outForce - Force at frame B
*/
static void ForceTransform(const vctFrm4x4 & inFrm4x4, const vct6 & inForce, vct6 & outForce)
{
    //see peter corke's book ( a T b is a transform to b wrt a)

    //rotation transpose

    vctDouble3x3 rotTrans;
    rotTrans = inFrm4x4.Rotation().Transpose();

    //translation of tipToHandleFrm

    vctDouble3 translation;
    translation = inFrm4x4.Translation();

    //3x3 0 matrix
    vct3x3 zeroMat;
    zeroMat.SetAll(0);

    //skew of tipToHandletranslation
    vctDouble3x3 skewTranslation;
    Skew(translation, skewTranslation);

    //adjoint matrix
    // - -
    // |  R(transpose)   R(transpose)*skew(p)(transpose)     | where p is translation from tip to handle,
    // |     0(3X3)            R(transpose)                  | R is rotation from tip to handle
    // - -

    vctFixedSizeMatrix<double, 6, 6, VCT_COL_MAJOR> adjoint;

    //put rotation transpose to the left upper corner
    vctDynamicMatrixRef<double> s00;
    s00.SetRef(adjoint, 0, 0, rotTrans.rows(), rotTrans.cols());
    s00.Assign(rotTrans); //need to convert to dynamic vector and assign.

    //put 0 matrix to the left lower corner
    vctDynamicMatrixRef<double> s01;
    s01.SetRef(adjoint, 3, 0, zeroMat.rows(), zeroMat.cols());
    s01.Assign(zeroMat); //need to convert to dynamic vector and assign.

    //put skew of translation*rotation transpose to the right upper corner
    vctDynamicMatrixRef<double> s10;
    s10.SetRef(adjoint, 0, 3, skewTranslation.rows(), skewTranslation.cols());
    s10.Assign( rotTrans * skewTranslation.Transpose()); //need to convert to dynamic vector and assign.

    vctDynamicMatrixRef<double> s11;
    //put rotation transpose to right lower corner
    s11.SetRef(adjoint, 3, 3, rotTrans.rows(), rotTrans.cols());
    s11.Assign(rotTrans); //need to convert to dynamic vector and assign.

    outForce.ProductOf(adjoint.Transpose(), inForce);

    //populate it with correct transform/rotations
}

}
#endif
