/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides

  (C) Copyright 2007-2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

// This program registers the specified data sets and computes the resulting
// Fiducial Registration Error (FRE) and the mean, standard deviation and
// maximum Target Registration Error (TRE), if any targets exist (i.e.,
// if not all points are used for registration).
//

#include <iostream>

// cisst includes
#include <cisstNumerical/nmrSVD.h>
#include <cisstNumerical/nmrRegistrationRigid.h>

#include "DataSet.h"

using namespace std;

// AccuracyDataSet is derived from DataSet.  It adds the methods for
// computing the registration and related results (FRE, TRE, etc.).
class AccuracyDataSet: public DataSet {
protected:
    vctFrm3 transform;
    vct3 *deltas;
    double *dists;
    double fre;
    double tre_mean, tre_stddev, tre_max;
public:
    AccuracyDataSet() : deltas(0), dists(0) { }
    ~AccuracyDataSet() { delete [] deltas; delete [] dists; }
    bool ComputeRegistration();
    double ComputeResiduals();
    void PrintResults();
};

// ComputeRegistration:  computes registration by calling nmrRegistrationRigid.
bool AccuracyDataSet::ComputeRegistration()
{
    int nrpoints = GetNumPts();
    int nregpoints = GetNumRegPts();
    cout << "Computing registration with " << nregpoints << " points." << endl;

    vctDynamicVector<vct3> P1(nregpoints);
    vctDynamicVector<vct3> P2(nregpoints);

    int i;
    int num = 0;
    for (i = 0; i < nrpoints; i++) {
        if (IsRegpoint(i) && (num < nregpoints)) {
            P1[num].Assign(Data1(i));
            P2[num].Assign(Data2(i));
            num++;
        }
    }
    if (num != nregpoints) {
        CMN_LOG_RUN_ERROR << "ComputeRegistration: inconsistent number of points: " << num 
                          << ", " << nregpoints << endl;
        return false;
    }

    // Call the registration method. Note that if desired, we can also get
    // the FRE by passing the address of a double variable to this function.
    bool ret = nmrRegistrationRigid(P1, P2, transform);
    cout << transform << endl;
    return ret;
}

// ComputeResiduals: uses registration result to compute FRE and TRE. 
// Stores distance errors (and delta-XYZ values) for each point for later output.
// Returns FRE.
double AccuracyDataSet::ComputeResiduals()
{
    int i;
    tre_mean = 0.0;
    tre_max = 0.0;
    double err2 = 0.0; // error squared
    double dist2;   // distance squared
    int nrpoints = GetNumPts();
    if (!deltas) deltas = new vctDouble3[nrpoints];
    if (!dists) dists = new double[nrpoints];
    for (i = 0; i < nrpoints; i++) {
        deltas[i] = Data2(i) - transform*Data1(i);
        dist2 = deltas[i].NormSquare();
        dists[i] = sqrt(dist2);
        if (IsRegpoint(i))
            err2 += dist2;
        else {
            tre_mean += dists[i];
            if (dists[i] > tre_max)
                tre_max = dists[i];
        }
    }
    // Nreg = number of registration points
    // Ntarget = number of target points
    int Ntarget = nrpoints-GetNumRegPts();
    if (Ntarget > 0) {
        tre_mean /= Ntarget;                // Mean TRE
        tre_stddev = 0.0;
        for (i = 0; i < nrpoints; i++) {
          if (!IsRegpoint(i))
              tre_stddev += (dists[i]-tre_mean)*(dists[i]-tre_mean);
        }
        tre_stddev = sqrt(tre_stddev/(Ntarget-1)); // Standard deviation
    }
    fre = sqrt(err2/GetNumRegPts());
    return fre;
}

// PrintResults: prints the results.
void AccuracyDataSet::PrintResults()
{
    printf("\nPoint  \t   Error \t    X   \t    Y   \t    Z\n");
    for (unsigned int i=0; i < GetNumPts(); i++) {
        printf("%4s   \t%8.3lf\t%8.3lf\t%8.3lf\t%8.3lf\n",
                GetName(i).c_str(),
                dists[i], deltas[i].X(), deltas[i].Y(), deltas[i].Z());
    }
    if (GetNumRegPts() < GetNumPts())
        cout << endl << "TRE (mean, stddev, max) = " << tre_mean << ", "
             << tre_stddev << ", " << tre_max << endl;
}


int main(int argc, char* argv[])
{
    if (argc < 2) {
        cout << endl << "Syntax:  " << argv[0] << " input_file1 input_file2" << endl << endl;
        cout << "Performs registration between marker positions in specified input files." << endl;
        cout << "First line of input file is skipped (i.e., can be a comment)." << endl;
        cout << "Format of subsequent lines is:  name x y z" << endl;
        cout << "Registration points are denoted by a leading 'R' character." << endl;
        cout << "If less than 3 points have a leading 'R', then all points will be used." << endl << endl;
        return 0;
    }

    AccuracyDataSet DS;
    if (!DS.ReadFromFile(0, argv[1])) {
      return -1;
    }
    if (!DS.ReadFromFile(1, argv[2])) {
      return -1;
    }
    DS.FinalizeInput();

    if (DS.ComputeRegistration()) {
        double fre = DS.ComputeResiduals();
        cout << "FRE = " << fre << endl;
        DS.PrintResults();
    }

    return 0;
}
