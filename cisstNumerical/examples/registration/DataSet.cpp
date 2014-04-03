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

// system includes
#include <iostream>

using namespace std;

#include "DataSet.h"

// ReadFromFile: read the coordinates of the points
bool DataSet::ReadFromFile(int dnum, const char* fname)
{
    typedef std::pair<std::string, vct3> EntryType;
    FILE *fp;
    char line[100];
    char name[20];
    double x,y,z;
    
    if ((fp=fopen(fname, "rt"))==NULL)
    {
      cout << "Cannot open input file '" << fname << "'" << endl;
      return false;
    }

    cout << "Reading point set " << dnum+1 << " from file " << fname << endl;

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (line[0] == '#') continue;  // comment lines have # as first character
        if (sscanf(line, "%s %lf %lf %lf", name, &x, &y, &z) != 4) {
            cout << "Error processing:  " << line << endl;
            fclose(fp);
            return false;
        }
        int len = strlen(name);
        if (name[len-1] == ':')
          name[len-1] = 0;
        data[dnum].insert(EntryType(name, vct3(x,y,z)));
    }
    fclose(fp);
    return true;
}

// FinalizeInput: this must be called after the 2 data sets are read.
// It goes through the input data and eliminates all points that are
// not defined in both data sets.
void DataSet::FinalizeInput()
{
    MapType::iterator p, q;
    p = data[0].begin();
    q = data[1].begin();
    numRegPts = 0;
    datalist.clear();
    while ((p != data[0].end()) && (q != data[1].end())) {
      if (p->first < q->first) {
        data[0].erase(p++);
      }
      else if (q->first < p->first) {
        data[1].erase(q++);
      }
      else {
        datalist.push_back(DataList(p->first, p->second, q->second));
        if (p->first[0] == 'R')
          numRegPts++;
        p++;
        q++;
      }
    }
    // If not enough registration points identified, use all of them.
    if (numRegPts < 3)
       numRegPts = datalist.size();
    data[0].clear();
    data[1].clear();
    cout << "Total number of points = " << datalist.size() << endl;
}
