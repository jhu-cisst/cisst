/*

  Author(s): Simon Leonard, Min Yang Jung
  Created on: April 29 2013

  (C) Copyright 2009-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstRobot/robModifiedHayati.h>

#include <iomanip>
#include <iostream>

robModifiedHayati::robModifiedHayati() 
    : robKinematics(robKinematics::MODIFIED_HAYATI),
      beta(0.0), alpha(0.0), a(0.0), d(0.0), theta(0.0)
{} 

robModifiedHayati::robModifiedHayati(double _beta, 
                         double _alpha, 
                         double _a, 
                         double _d,
                         double _theta,
                         const robJoint& joint) :
    robKinematics(joint, robKinematics::MODIFIED_HAYATI),
    beta(_beta),
    alpha(_alpha),
    a(_a),
    d(_d),
    theta(_theta)
{}

robModifiedHayati::~robModifiedHayati()
{}


robKinematics* robModifiedHayati::Clone() const 
{
    return (robKinematics*) new robModifiedHayati(*this);
}

vctFixedSizeVector<double,3> robModifiedHayati::PStar() const
{
    return vctFixedSizeVector<double,3>(a*cos(beta) + d*cos(alpha)*sin(beta),
                                        -d*sin(alpha),
                                        d*cos(alpha)*cos(beta) - a*sin(beta));
}

vctFrame4x4<double> robModifiedHayati::ForwardKinematics(double q) const
{ 
    double a = this->a;
    double d = this->d;
    double theta = this->theta;

    // Add the position offset to the joint value
    switch (GetType()) {
    case robJoint::HINGE:
        {
            theta = theta + PositionOffset() + q;

            double ca = cos(this->alpha); double sa = sin(this->alpha);	
            double cb = cos(this->beta);  double sb = sin(this->beta);	
            double ct = cos(theta);       double st = sin(theta);

            vctFrame4x4<double> Ry(
                                   vctMatrixRotation3<double>( cb,  0.0, sb,
                                                               0.0, 1.0, 0.0,
                                                               -sb, 0.0, cb),
                                   vctFixedSizeVector<double,3>(0.0)
                                  );

            vctFrame4x4<double> Rx(
                                   vctMatrixRotation3<double>(1.0, 0.0, 0.0,
                                                              0.0, ca, -sa,
                                                              0.0, sa,  ca),
                                   vctFixedSizeVector<double,3>(0.0)
                                  );

            vctFrame4x4<double> Tx(
                                   vctMatrixRotation3<double>(),
                                   vctFixedSizeVector<double,3>(a, 0.0, 0.0)
                                  );

            vctFrame4x4<double> Rz(
                                   vctMatrixRotation3<double>( ct, -st, 0.0,
                                                               st,  ct, 0.0,
                                                               0.0, 0.0, 1.0),
                                   vctFixedSizeVector<double,3>(0.0)
                                  );

            return (Ry * Rx * Tx * Rz);
        }

    case robJoint::SLIDER:
        {
            d = d + PositionOffset() + q;

            double ca = cos(this->alpha); double sa = sin(this->alpha);	
            double cb = cos(this->beta);  double sb = sin(this->beta);	

            vctFrame4x4<double> Ry(
                                   vctMatrixRotation3<double>( cb,  0.0, sb,
                                                               0.0, 1.0, 0.0,
                                                               -sb, 0.0, cb),
                                   vctFixedSizeVector<double,3>(0.0)
                                  );

            vctFrame4x4<double> Rx(
                                   vctMatrixRotation3<double>(1.0, 0.0, 0.0,
                                                              0.0,  ca, -sa,
                                                              0.0,  sa,  ca),
                                   vctFixedSizeVector<double,3>(0.0)
                                  );

            vctFrame4x4<double> Tx(
                                   vctMatrixRotation3<double>(),
                                   vctFixedSizeVector<double,3>(a, 0.0, 0.0)
                                  );

            vctFrame4x4<double> Tz(
                                   vctMatrixRotation3<double>(),
                                   vctFixedSizeVector<double,3>(0.0, 0.0, d)
                                  );

            return (Ry * Rx * Tx * Tz);
        }

    default:
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
            << ": Unsupported joint type" << std::endl;

        return vctFrame4x4<double>();
    }
}

vctMatrixRotation3<double> robModifiedHayati::Orientation(double q) const 
{
    vctFrame4x4<double> Rt = ForwardKinematics(q);
    return vctMatrixRotation3<double>(Rt[0][0], Rt[0][1], Rt[0][2],
                                      Rt[1][0], Rt[1][1], Rt[1][2],
                                      Rt[2][0], Rt[2][1], Rt[2][2]);
}

void robModifiedHayati::ReadParameters(std::istream& is)
{
    is >> this->beta
        >> this->alpha
        >> this->a
        >> this->d
        >> this->theta;

    // just make sure we're accureate
    if (this->alpha == -1.5708) this->alpha = -cmnPI_2;
    if (this->alpha ==  1.5708) this->alpha =  cmnPI_2;
}

#if CISST_HAS_JSON
void robModifiedHayati::ReadParameters(const Json::Value &config)
{
    this->beta  = config.get("beta",  "0.00").asDouble();
    this->alpha = config.get("alpha", "0.00").asDouble();
    this->a     = config.get("a",     "0.00").asDouble();
    this->d     = config.get("d",     "0.00").asDouble();
    this->theta = config.get("theta", "0.00").asDouble();

    // just make sure we're accureate
    if (this->alpha == -1.5708) this->alpha = -cmnPI_2;
    if (this->alpha ==  1.5708) this->alpha =  cmnPI_2;
}
#endif

void robModifiedHayati::WriteParameters(std::ostream& os) const
{
    os << std::setw(10) << "MODIFIED_HAYATI"
        << std::setw(10) << beta
        << std::setw(10) << alpha
        << std::setw(10) << a
        << std::setw(10) << d
        << std::setw(10) << theta;
}

void robModifiedHayati::PrintParams(void) const
{
    std::cout << (int)GetType();
    std::cout << std::setw(10) << beta;
    std::cout << std::setw(10) << alpha;
    std::cout << std::setw(10) << a;
    std::cout << std::setw(10) << d;
    std::cout << std::setw(10) << theta << std::endl;
}
