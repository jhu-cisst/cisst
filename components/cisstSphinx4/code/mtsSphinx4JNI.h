/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#ifndef _mtsSphinx4JNI_h
#define _mtsSphinx4JNI_h

#ifdef __cplusplus
extern "C" {
#endif

    // class name in Java is cisstSphinx4
    void JNICALL Java_cisstSphinx4_WordRecognizedCallback(JNIEnv *, jobject, jlong, jstring);

#ifdef __cplusplus
}
#endif

#endif // __cplusplus
