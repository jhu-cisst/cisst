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

/*
  DataSet class that handles reading of points from input file. Note that a point can be
 "hidden" by editing the input file and changing its name to one that is not present
 in the list maintained by the Phantom class.
*/

#ifndef _DataSet_h
#define _DataSet_h

#include <map>

// cisst includes
#include <cisstVector/vctFixedSizeVectorTypes.h>

// Class for the two data sets
class DataSet {
public:
    typedef  std::map<std::string, vct3> MapType;

protected:
    MapType data[2];

    struct DataList {
        std::string name;
        vct3 data[2];
        DataList(const std::string &n, const vct3 &d1, const vct3 &d2)
        { name = n; data[0] = d1; data[1] = d2; }
        ~DataList() { }
        // Consider a point to be a registration point if the first character is an 'R'.
        // If less than 3 points have a leading 'R', then all points are used for registration.
    };
 
    std::vector<DataList> datalist;
    unsigned int numRegPts;

public:
    DataSet() : numRegPts(0) { }
    virtual ~DataSet() { }
    bool ReadFromFile(int dnum, const char* fname);
    void FinalizeInput();

    unsigned int GetNumPts()    { return datalist.size(); }
    unsigned int GetNumRegPts() { return numRegPts; }
    const std::string& GetName(int idx) { return datalist[idx].name; }
    const vct3& Data1(int idx) { return datalist[idx].data[0]; }
    const vct3& Data2(int idx) { return datalist[idx].data[1]; }
    bool IsRegpoint(int idx)    { return (numRegPts == datalist.size()) ||
                                         (datalist[idx].name[0] == 'R'); }
};

#endif
