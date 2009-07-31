/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Ben Mitchell
  Created on: 2006 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <iostream>
#include "ftImagePPM.h"

#ifdef _MSC_VER
    // Quick fix for Visual Studio Intellisense:
    // The Intellisense parser can't handle the CMN_UNUSED macro
    // correctly if defined in cmnPortability.h, thus
    // we should redefine it here for it.
    // Removing this part of the code will not effect compilation
    // in any way, on any platforms.
    #undef CMN_UNUSED
    #define CMN_UNUSED(argument) argument
#endif

using namespace std;

/*************************************/
/*** ftImagePPM class ****************/
/*************************************/

ftImagePPM::ftImagePPM()
{
    myrows = mycols = mycolors = mysize = 0;
    pgm = false;
}

ftImagePPM::~ftImagePPM()
{
    Close();
}

svlImageFile* ftImagePPM::GetInstance()
{
    ftImagePPM* instance = new ftImagePPM();
    return instance;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ftImagePPM::ExtractDimensions
// Limitations:
int ftImagePPM::ExtractDimensions(const char* CMN_UNUSED(filepath), int & CMN_UNUSED(width), int & CMN_UNUSED(height))
{
    // TODO
    return -1;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ftImagePPM::Open
// Limitations:
//             - only 24bpp PPMs and 8bpp PGMs are supported
//             - maximal image size: IMAGEPPM_MAX_DIMENISION x IMAGEPPM_MAX_DIMENISION (could be anything)
int ftImagePPM::Open(const char* filepath, svlImageProperties& properties)
{
    Close();

  string magic;

  myfile.open (filepath, fstream::in | fstream::binary);
  if (!myfile.is_open ()) {
    cerr << "Error opening file " << filepath << endl;
    goto labError;
  }

  // In this for loop, we read the header
  for (int i = 0; i < 4; i++) {
    while (myfile.peek() == '\n' ||  
           myfile.peek() == ' ' || 
           myfile.peek() == '#') { 
      if((myfile.peek() == '#')) //ignore comments
        myfile.ignore(1024, '\n');
      else
        myfile.get(); //ignore extra whitespace and endlines
    }
    switch(i) {
      case 0: //first real block of data = magic number
        myfile >> magic;
        break;
      case 1: //second is X size
        myfile >> mycols;
        break;
      case 2: //third is Y size
        myfile >> myrows;
        break;
      case 3: //fourth is the max. allowed value of a color 
              //component of a pixel (should be 255)
        myfile >> mycolors;
        break;
    }
  }

  //make sure we don't have extra junk before the actual data
  while (myfile.peek() == '\n' || 
         myfile.peek() == ' ' || 
         myfile.peek() == '#') {
    if((myfile.peek() == '#')) //ignore comments
      myfile.ignore(1024, '\n');
    else
      myfile.get(); //ignore extra whitespace and endlines
  }


  if( magic != "P5" && magic != "P6") {
    cerr << "Error reading file " << filepath << "; unknown magic number\n";
    goto labError;
  }


  mysize = myrows * mycols;

  if(magic == "P5") {
    tmpBuffer = new unsigned char[mysize];
    if(tmpBuffer == 0) 
      goto labError;
    pgm = true;
  }

  if (mycolors != 255) {
    cerr << "Colors wasn't 255 in file " << filepath << endl;
    goto labError;
  }


  properties.DataType = svlTypeImageRGB;
  properties.DataSize = mysize * sizeof(svlRGB);
  properties.Width = mycols;
  properties.Height = myrows;
  properties.Padding = 0;

  return SVL_OK;

    
labError:
    Close();
    return SVL_FAIL;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ftImagePPM::ReadAndClose
// Limitations:
//             - only 24bpp PPMs and 8bpp PGMs are supported
//             - maximal image size: IMAGEPPM_MAX_DIMENISION x IMAGEPPM_MAX_DIMENISION (could be anything)
int ftImagePPM::ReadAndClose(unsigned char* buffer, unsigned int size)
{
  if( ! myfile.is_open() ) {
    cerr << "ftImagePPM::ReadAndClose() called without an open file handle\n";
    Close();
    return SVL_FAIL;
  }
  
  if (size < mysize * sizeof(svlRGB) ) {
    cerr << "ftImagePPM::ReadAndClose() given a buffer that was too small\n";
    Close();
    return SVL_FAIL;
  }

  svlRGB* data = reinterpret_cast<svlRGB*>(buffer);
  if (pgm) { //it's a pgm
    myfile.read (reinterpret_cast<char*>(tmpBuffer), (sizeof(char) * mysize));

    for (unsigned int i = 0; i < mysize; i++) {
      data[i].R = data[i].G = data[i].B = tmpBuffer[i];
    }
  } else { //it's a ppm
    myfile.read (reinterpret_cast<char*>(data), (sizeof(svlRGB) * mysize));

    //Balazs - swapping R and B
    unsigned char colval;
    for (unsigned int i = 0; i < mysize; i++) {
      colval = data[i].R;
      data[i].R = data[i].B;
      data[i].B = colval;
    }
  }

  Close();
  return SVL_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ftImagePPM::Create
// Limitations:
//             - only 24bpp PPMs are supported
//             - maximal image size: IMAGEPPM_MAX_DIMENISION x IMAGEPPM_MAX_DIMENISION (could be anything)
int ftImagePPM::Create(const char* filepath, svlImageProperties* properties, unsigned char* buffer)
{
  int remlen;
  unsigned int currlen, i;
  unsigned long buflen;

    if (!filepath || !properties || !buffer) goto labError;

    mycols = properties->Width;
    myrows = properties->Height;
    mycolors = 255;

    if (properties->DataType != svlTypeImageRGB ||
        mycols < 1 ||
        mycols > IMAGEPPM_MAX_DIMENISION ||
        myrows < 1 ||
        myrows > IMAGEPPM_MAX_DIMENISION) goto labError;


    mysize = myrows * mycols;
    buflen = mysize * sizeof(svlRGB);

    if (properties->Padding) {
        cerr << "ftImagePPM:Create() called with properties->Padding != 0\n";
        goto labError;
    }
    else {
        if (properties->DataSize != buflen) goto labError;
    }

    // opening file
    myfile.open (filepath, fstream::out | fstream::binary);
    if (!myfile.is_open ()) {
      cerr << "Could not open " << filepath << " for writing.\n";
      goto labError;
    }
    //cerr << "Writing to file " << filename << "...\n";

    //write header
    myfile << "P6\n" << mycols << " " << myrows << endl << mycolors << endl;

    //Balazs - swapping R and B and saving
    //write data
    remlen = buflen;
    while (remlen > 0) {
        currlen = std::min(remlen, 12288);
        for (i = 0; i < currlen; i += 3) {
            tmpSwapBuffer[i] = buffer[2];
            tmpSwapBuffer[i + 1] = buffer[1];
            tmpSwapBuffer[i + 2] = buffer[0];
            buffer += 3;
        }
        myfile.write(reinterpret_cast<char*>(tmpSwapBuffer), currlen);
        remlen -= 12288;
    }

    myfile.close ();
    return SVL_OK;

labError:
    if (myfile.is_open()) myfile.close();
    return SVL_FAIL;
}

void ftImagePPM::Close()
{
    if(myfile.is_open()) myfile.close();
    if(pgm) {
      delete [] tmpBuffer;
    }
    myrows = mycols = mycolors = mysize = 0;
    pgm = false;
}

