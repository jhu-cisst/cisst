/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cscSphinx4JNI.cpp 2771 2011-03-11 19:45:58Z adeguet1 $

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

#include <string>
#include "cscSphinx4JNI.h"
#include "cscSpeechToCommands.h"

void JNICALL Java_cscSphinx4_WordRecognizedCallback(JNIEnv *env, jobject, jlong speechToCommandsCppPointer, jstring wordJava)
{
    // convert JString to std::string
    const char * wordCharPointer = env->GetStringUTFChars(wordJava, 0);
    const std::string word(wordCharPointer);
    cscSpeechToCommands * speechToCommands = reinterpret_cast<cscSpeechToCommands *>(speechToCommandsCppPointer);
    CMN_ASSERT(speechToCommands != 0);
    speechToCommands->WordRecognizedCallback(word);
    // release Java string
    env->ReleaseStringUTFChars(wordJava, wordCharPointer);
}
