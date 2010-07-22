/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):  Balazs P. Vagvolgyi
  Created on: 2010-07-22

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstStereoVision/svlFilterSourceTextFile.h>
#include <cisstStereoVision/svlFilterOutput.h>


/*************************************/
/*** svlFilterSourceTextFile class ***/
/*************************************/

CMN_IMPLEMENT_SERVICES(svlFilterSourceTextFile)

svlFilterSourceTextFile::svlFilterSourceTextFile() :
    svlFilterSourceBase(),
    Columns(10)
{
    AddOutput("output", true);
    SetAutomaticOutputType(false);
    GetOutput()->SetType(svlTypeMatrixFloat);
}

svlFilterSourceTextFile::~svlFilterSourceTextFile()
{
    Release();
}

int svlFilterSourceTextFile::SetColumns(const unsigned int columns)
{
    if (Columns < 1) return SVL_FAIL;
    Columns = columns;
    return SVL_OK;
}

unsigned int svlFilterSourceTextFile::GetColumns() const
{
    return Columns;
}

int svlFilterSourceTextFile::AddFile(const std::string& filepath, const int timestamp_column)
{
    if (IsInitialized()) return SVL_FAIL;

    const unsigned int size = FilePaths.size();
    FilePaths.resize(size + 1);
    FilePaths[size] = filepath;
    TimeColumns.resize(size + 1);
    TimeColumns[size] = timestamp_column;

    return SVL_OK;
}

void svlFilterSourceTextFile::RemoveFiles()
{
    if (!IsInitialized()) {
        FilePaths.SetSize(0);
        TimeColumns.SetSize(0);
    }
}

int svlFilterSourceTextFile::Initialize(svlSample* &syncOutput)
{
    const unsigned int file_count = FilePaths.size();
    if (file_count < 1) return SVL_FAIL;

    Files.SetSize(file_count);
    Files.SetAll(0);

    for (unsigned int i = 0; i < file_count; i ++) {

        Files[i] = new std::ifstream(FilePaths[i].c_str(), std::ios_base::in);
        if (!Files[i]->is_open()) {
            Release();
            return SVL_FAIL;
        }
    }

    OutputMatrix.SetSize(Columns, file_count);
    LineBuffer.SetSize(Columns * 32); // should be enough

    syncOutput = &OutputMatrix;
    return SVL_OK;
}

int svlFilterSourceTextFile::Process(svlProcInfo* procInfo, svlSample* &syncOutput)
{
    _OnSingleThread(procInfo)
    {
        const unsigned int file_count = FilePaths.size();
        unsigned int i, j;
        std::stringstream strstr;
        float fval;

        for (i = 0; i < file_count; i ++) {

            Files[i]->getline(LineBuffer.Pointer(), LineBuffer.size());
            Files[i]->clear();

            strstr.write(LineBuffer.Pointer(), Files[i]->gcount());

            for (j = 0; j < Columns; j ++) {
                strstr >> fval;
                if (strstr.eof()) OutputMatrix.Element(j, i) = 0.0f;
                else OutputMatrix.Element(j, i) = fval;
            }
        }
    }

    syncOutput = &OutputMatrix;
    return SVL_OK;
}

int svlFilterSourceTextFile::Release()
{
    const unsigned int file_count = FilePaths.size();
    for (unsigned int i = 0; i < file_count; i ++) {
        Files[i]->close();
        delete Files[i];
    }
    Files.SetSize(0);

    return SVL_OK;
}

