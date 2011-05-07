/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cscSphinx4JNI.h 2771 2011-03-11 19:45:58Z adeguet1 $

  Author(s):  Martin Kelly, Anton Deguet
  Created on: 2011-02-15

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <jni.h>

#ifndef _cscSphinx4JNI_h
#define _cscSphinx4JNI_h

// Always include last!
#include "cscExport.h"

#ifdef __cplusplus
extern "C" {
#endif

    CISST_EXPORT void JNICALL Java_cscSphinx4_WordRecognizedCallback(JNIEnv *, jobject, jlong, jstring);

#ifdef __cplusplus
}
#endif

#endif // __cplusplus
