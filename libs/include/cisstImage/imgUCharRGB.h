/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s): Anton Deguet
  Created on: 2006-03-28

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*! 
  \file 
  \brief Types for unsigned char based RGB pixmap
*/


#ifndef _imgUCharRGB_h
#define _imgUCharRGB_h


#include <iostream>
#include <ios>


#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctDynamicConstMatrixRef.h>

// Always include imgExport.h last
#include <cisstImage/imgExport.h>

/*! Unsigned char red green blue pixels and pixmaps */
namespace imgUCharRGB {

    /*! Pixel type */
    typedef vctFixedSizeVector<unsigned char, 3> Pixel;

    /*! Pixel map type, i.e. image. */
    typedef vctDynamicMatrix<Pixel> Pixmap;

    /*! Pixel map reference. */
    typedef vctDynamicMatrixRef<Pixel> PixmapRef;

    /*! Pixel map const reference. */
    typedef vctDynamicConstMatrixRef<Pixel> ConstPixmapRef;


    /*! Predefined constants for pixel values. */
    //@{
    extern CISST_EXPORT const Pixel Black;
    extern CISST_EXPORT const Pixel White;
    extern CISST_EXPORT const Pixel Red;
    extern CISST_EXPORT const Pixel Green;
    extern CISST_EXPORT const Pixel Blue;
    //@}


    /*! Write to file in PPM format */
    template <class _matrixOwnerType>
    bool WritePPM(const vctDynamicConstMatrixBase<_matrixOwnerType, Pixel> & pixmap, const char * fileName) {
        
        typedef vctDynamicConstMatrixBase<_matrixOwnerType, Pixel> PixmapType;
        typedef typename PixmapType::const_iterator const_iterator;
        
        std::ofstream file(fileName, std::ios::out | std::ios::binary);
        if (!file) {
            CMN_LOG_INIT_ERROR << "imgUCharRGB::WritePPM: Can't open file " << fileName << std::endl;
            return false;
        }
        
        file << "P6" << std::endl
             << pixmap.width() << " " << pixmap.height() << std::endl
             << "255" << std::endl;
        
        unsigned char *tempBuffer = new unsigned char [pixmap.width() * pixmap.height() * 3];
        const const_iterator end = pixmap.end();
        const_iterator iter = pixmap.begin();
        unsigned int bufferPosition;
        for (bufferPosition = 0; iter != end; ++iter) {
            tempBuffer[bufferPosition++] = (*iter).X();
            tempBuffer[bufferPosition++] = (*iter).Y();
            tempBuffer[bufferPosition++] = (*iter).Z();
        }
        
        file.write((char*)tempBuffer, pixmap.width() * pixmap.height() * 3);
        file.flush();
        file.close();
        delete[] tempBuffer;
        CMN_LOG_INIT_WARNING << "imgUCharRGB::WritePPM: Successfully saved as " << fileName << std::endl;
        return true;
    };

}


#endif  // _imgUCharRGB_h

