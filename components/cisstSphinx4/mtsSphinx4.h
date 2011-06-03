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

#ifndef _mtsSphinx4_h
#define _mtsSphinx4_h

#include <map>
#include <string>
#include <cisstSphinx4/mtsSphinx4Config.h>

#include <cisstMultiTask/mtsTaskContinuous.h>

// Always include last!
#include <cisstSphinx4/mtsSphinx4Export.h>

// forward declaration of class containing all Java members
struct mtsSphinx4Java;
class mtsSphinx4;

class CISST_EXPORT mtsSphinx4: public mtsTaskContinuous
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);



 public:
    // forward declaration for context
    class Context;

    class WordActions: public cmnGenericObject
    {
        CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
        friend class mtsSphinx4::Context;

        bool IsTransition;
        std::string EndContext;

    public:
        WordActions(void);
        WordActions(const std::string & endContext);

        bool PerformActions(mtsSphinx4 * sphinx4) const;
    };


    class CISST_EXPORT Context: public cmnGenericObject
    {
        CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

        friend class mtsSphinx4; // to allow speech to command to set pointer to itself

        mtsSphinx4 * Sphinx4Wrapper;
        typedef cmnNamedMap<WordActions> WordMap;
        WordMap Words;
        std::string Name;
        bool OutOfGrammarFiltering;

        bool AddWordWithActions(const std::string & word, WordActions * wordActions);

        Context(const std::string & name);
        bool PerformActionsForWord(const std::string & word);

    public:
        bool AddWord(const std::string & word);
        bool AddWordWithTransition(const std::string & word, const std::string & endContext);
        const std::string & GetName(void) const;
        std::vector<std::string> GetVocabulary(void);
        bool FilteringEnabled(void);
        void SetFiltering(bool filter);
    };



    friend void mtsSphinx4JavaWordRecognizedCallback(mtsSphinx4 * sphinx4Wrapper,
                                                     const std::string & word);

    Context * CurrentContext;
    typedef cmnNamedMap<Context> ContextMap;
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

    bool NewWordRecognized;
    mtsStdString LastWordRecognized;

    mtsSphinx4Java * JavaData;

    // method called by Java when a word has been recognized
    void WordRecognizedCallback(const std::string & word);
    // method to actually process the word, called periodically by Run
    void HandleWord(const mtsStdString & word);

    bool ReplaceAll(std::string & base, const std::string & s, const std::string & t);
    bool StartJava(void);
    void PrintAudioDevices(void);

public:
    mtsSphinx4(const std::string & componentName);

    Context * AddContext(const std::string & contextName);
    bool SetCurrentContext(const std::string & name);
    bool SetCurrentContext(const Context * context);
    void GetContexts(stdStringVec & placeHolder) const;
    void GetContextWords(const mtsStdString & contextName,
                         stdStringVec & placeHolder) const;
    void WordTriggeredFromUI(const mtsStdString & word);

    unsigned int GetMicrophoneNumber(void) const;
    void SetMicrophoneNumber(unsigned int microphoneNumber);

    // methods required for all tasks
    void Configure(void);
    void Startup(void);
    void Run(void);
    void Cleanup(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsSphinx4::Context);
CMN_DECLARE_SERVICES_INSTANTIATION(mtsSphinx4::WordActions);
CMN_DECLARE_SERVICES_INSTANTIATION(mtsSphinx4);

#endif // _mtsSphinx4_h
