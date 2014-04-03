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

// This program computes the distances between all pairs of points in the first
// input file and compares them to the corresponding distances computed from all pairs
// of points in the second data file (ground truth).
// This evaluation does not require a registration to be performed.

// system includes
#include <iostream>
#include <stdio.h>

#include "DataSet.h"

using namespace std;

// DistanceDataSet is derived from DataSet.  It adds the method for
// computing the distances.
class DistanceDataSet: public DataSet {
protected:
    int num_dists;
    vct3 avg_err_xyz;
    double avg_err;
    double avg_err_mag;
    double max_err;
    double std_dev;
    vct3 std_dev_xyz;

public:
    DistanceDataSet() {}
    ~DistanceDataSet() {}
    void ComputeDistances(bool do_output, bool do_table);
    void ComputeStdDev(bool do_table);
    vct3 GetStdDevXYZ() const { return std_dev_xyz; }
};

// ComputeDistances: compute the distances between each pair of points.
void DistanceDataSet::ComputeDistances(bool do_output, bool do_table)
{
    unsigned int i,j;
    vct3 dist1_xyz;
    vct3 dist2_xyz;
    vct3 dist_err_xyz;

    num_dists = 0;
    avg_err = 0;
    max_err = 0;
    avg_err_mag = 0;
    avg_err_xyz.SetAll(0.0);

    if (do_output)
        printf("Distance errors (File1 - File2):\n");
    if (do_table) {
        printf("\n     ");
        for (i=1; i < GetNumPts(); i++) {
            printf("   %2s   ", GetName(i).c_str());
        }
        printf("\n");
    }
    for (i=0; i < GetNumPts()-1; i++) {
        if (do_table) {
            printf(" %2s  ", GetName(i).c_str());
            for (j=1; j < i+1; j++)
                printf("        ");
        }
        for (j=i+1; j < GetNumPts(); j++) {
            num_dists++;
            dist1_xyz = Data1(i) - Data1(j);
            dist2_xyz = Data2(i) - Data2(j);
            double dist1 = dist1_xyz.Norm();
            double dist2 = dist2_xyz.Norm();
            double dist_err = dist1 - dist2;
            dist_err_xyz.NormalizedOf(dist1_xyz);
            dist_err_xyz *= dist_err;
            avg_err_xyz += dist_err_xyz;
            avg_err += dist_err;
            avg_err_mag += fabs(dist_err);
            if (fabs(dist_err) > fabs(max_err))
                max_err = dist_err;
            if (do_table)
                printf(" %6.3lf ", dist_err);
            else if (do_output) {
                printf("%2s - %2s:  %8.3lf (dist1 = %8.3lf, dist2 = %8.3lf)\n", 
                   GetName(i).c_str(), GetName(j).c_str(), dist_err, dist1, dist2);
            }
        }
        if (do_table)
            printf("\n");
    }
    if (do_output)
        printf("\n");
    if (num_dists > 0) {
        avg_err /= num_dists;
        avg_err_mag /= num_dists;
        avg_err_xyz /= num_dists;
        if (do_output) {
            printf("Number of distances = %d\n", num_dists);
            printf("Maximum error   = %8.3lf\n", max_err);
            printf("Average error   = %8.3lf\n", avg_err);
            printf("Average |error| = %8.3lf\n", avg_err_mag);
            printf("Average error (X,Y,Z) = %8.3lf, %8.3lf, %8.3lf\n", avg_err_xyz.X(),
                avg_err_xyz.Y(), avg_err_xyz.Z());
        }
    }
}

// ComputeStdDev: compute the standard deviation of the distance errors
void DistanceDataSet::ComputeStdDev(bool do_output)
{
    unsigned int i, j;
    vct3 temp;
    vct3 dist1_xyz;
    vct3 dist2_xyz;
    vct3 dist_err_xyz;

    std_dev = 0;
    std_dev_xyz.SetAll(0.0);

    for (i=0; i < GetNumPts(); i++) {
        for (j=i+1; j < GetNumPts(); j++) {
            dist1_xyz = Data1(i) - Data1(j);
            dist2_xyz = Data2(i) - Data2(j);
            double dist1 = dist1_xyz.Norm();
            double dist2 = dist2_xyz.Norm();
            double dist_err = dist1 - dist2;
            double dist_err_mag = fabs(dist1 - dist2);
            dist_err_xyz.NormalizedOf(dist1_xyz);
            dist_err_xyz *= dist_err;
            temp.ElementwiseProductOf(dist_err_xyz-avg_err_xyz, dist_err_xyz-avg_err_xyz);
            std_dev_xyz += temp;
            std_dev += (dist_err_mag - avg_err_mag)*(dist_err_mag - avg_err_mag);
        }
    }
    std_dev = sqrt(std_dev/(num_dists-1));
    std_dev_xyz = vct3(sqrt(std_dev_xyz.X()), sqrt(std_dev_xyz.Y()), sqrt(std_dev_xyz.Z()))/sqrt((double)num_dists-1);
    if (do_output) {
        printf("Standard deviation of |error| = %8.3lf\n", std_dev);
        printf("Standard deviation in X, Y, Z = %8.3lf, %8.3lf, %8.3lf\n", std_dev_xyz.X(), std_dev_xyz.Y(),
            std_dev_xyz.Z());
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        cout << endl << "Syntax:  " << argv[0] << " input_file1 input_file2 [list]" << endl << endl;
        cout << "Displays distance errors in tabular format (default) or in" << endl;
        cout << "single column list if \"list\" option specified" << endl;
        return 0;
    }

    bool do_output = true;
    bool do_table = true;
    if ((argc == 4) && (strcmp(argv[3], "list") == 0))
        do_table = false;

    DistanceDataSet DS;
    if (!DS.ReadFromFile(0, argv[1])) {
      return -1;
    }
    if (!DS.ReadFromFile(1, argv[2])) {
      return -1;
    }
    DS.FinalizeInput();
    DS.ComputeDistances(do_output, do_table);
    DS.ComputeStdDev(do_output);
}
