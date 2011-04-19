/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cscSpeechToCommands.h 2934 2011-04-19 03:29:58Z adeguet1 $

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

#ifndef _cscSpeechToCommands_h
#define _cscSpeechToCommands_h

#include <map>
#include <string>
#include <jni.h> // for friend forward declaration of callback WordRecognized
#include "cscConfig.h"
#include "cscContext.h"

#include <cisstMultiTask/mtsTaskContinuous.h>

// Always include last!
#include "cscExport.h"

class CISST_EXPORT cscSpeechToCommands: public mtsTaskContinuous
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend void JNICALL Java_cscSphinx4_WordRecognizedCallback(JNIEnv *, jobject, jlong, jstring);

    cscContext * CurrentContext;
    typedef cmnNamedMap<cscContext> ContextMap;
    ContextMap Contexts;
    unsigned int MicrophoneNumber;
    std::string ModelFile;
    std::string ModelTopDir;
    std::string SphinxFile;
    std::string TemplateFile;
    std::string ConfigName;
    std::string SphinxWrapperDir;

    mtsMulticastCommandWriteBase * WordRecognizedTrigger;
    mtsMulticastCommandVoid * NoWordRecognizedTrigger;
    mtsFunctionWrite ContextChangedTrigger;

    mtsStdString LastWordRecognized;

    JNIEnv * JNIEnvironment;
    JavaVM * JavaVirtualMachine;
    jobject Sphinx4Wrapper;
    jmethodID RecognizeWordJavaMethod;
    jmethodID SetCurrentContextJavaMethod;

    // method called by Java when a word has been recognized
    void WordRecognizedCallback(const std::string & word);

    void ReplaceAll(std::string & base, const std::string & s, const std::string & t);
    bool StartJava(void);
    void PrintAudioDevices(void);

public:
    cscSpeechToCommands(const std::string & componentName);

    cscContext * AddContext(const std::string & contextName);
    bool SetCurrentContext(const std::string & name);
    bool SetCurrentContext(const cscContext * context);
    void GetContexts(stdStringVec & placeHolder) const;
    void GetContextWords(const mtsStdString & contextName,
                         stdStringVec & placeHolder) const;

    unsigned int GetMicrophoneNumber(void) const;
    void SetMicrophoneNumber(unsigned int microphoneNumber);

    // methods required for all tasks
    void Configure(const std::string & filename = "");
    void Startup(void);
    void Run(void);
    void Cleanup(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(cscSpeechToCommands);

#endif // _cscSpeechToCommands_h
