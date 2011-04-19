/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cscSpeechToCommandsQtComponent.cpp 2934 2011-04-19 03:29:58Z adeguet1 $

  Author(s):  Anton Deguet
  Created on: 2011-03-07

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsInterfaceRequired.h>

#include "cscSpeechToCommandsQtComponent.h"

CMN_IMPLEMENT_SERVICES(cscSpeechToCommandsQtComponent);


cscSpeechToCommandsQtComponent::cscSpeechToCommandsQtComponent(const std::string & componentName):
    mtsComponent(componentName)
{
    // create the cisstMultiTask interface with commands and events
    mtsInterfaceRequired * interfaceRequired = AddInterfaceRequired("SpeechToCommands");
    if (interfaceRequired) {
        interfaceRequired->AddFunction("GetContexts", GetContexts);
        interfaceRequired->AddFunction("GetContextWords", GetContextWords);
        interfaceRequired->AddEventHandlerWrite(&cscSpeechToCommandsQtComponent::WordRecognizedHandler,
                                                this, "WordRecognized");
        interfaceRequired->AddEventHandlerVoid(&cscSpeechToCommandsQtComponent::NoWordRecognizedHandler,
                                               this, "NoWordRecognized");
        interfaceRequired->AddEventHandlerWrite(&cscSpeechToCommandsQtComponent::ContextChangedHandler,
                                                this, "ContextChanged");
    }
    // Connect Qt signals to slots
    QObject::connect(this, SIGNAL(WordRecognizedQSignal(QString)),
                     CentralWidget.ValueWordRecognized, SLOT(setText(QString)));

    QObject::connect(this, SIGNAL(ContextChangedQSignal(QString)),
                     CentralWidget.ValueContext, SLOT(setText(QString)));

    QObject::connect(this, SIGNAL(WordAddedQSignal(QString, QString)),
                     &CentralWidget, SLOT(AddWord(QString, QString)));
}


void cscSpeechToCommandsQtComponent::Start(void)
{
    stdStringVec contexts, words;
    std::string currentContext, currentWord;
    mtsExecutionResult result;
    // get list of contexts
    result = this->GetContexts(contexts);
    if (!result) {
        CMN_LOG_CLASS_INIT_ERROR << "Start: can not retrieve contexts, error \"" << result << "\"" << std::endl;
        return;
    }
    const size_t numberOfContexts = contexts.size();
    size_t numberOfWords;
    for (size_t contextIndex = 0;
         contextIndex < numberOfContexts;
         contextIndex++) {
        currentContext = contexts[contextIndex];
        result = this->GetContextWords(mtsStdString(currentContext), words);
        if (!result) {
            CMN_LOG_CLASS_INIT_ERROR << "Start: can not retrieve words for context \""
                                     << currentContext << "\", error \"" << result << "\"" << std::endl;
            return;
        }
        numberOfWords = words.size();
        for (size_t wordIndex = 0;
             wordIndex < numberOfWords;
             wordIndex++) {
            currentWord = words[wordIndex];
            emit WordAddedQSignal(QString(currentContext.c_str()), QString(currentWord.c_str()));
        }
    }
}


QWidget * cscSpeechToCommandsQtComponent::GetWidget(void)
{
    return &CentralWidget;
}


void cscSpeechToCommandsQtComponent::WordRecognizedHandler(const mtsStdString & word)
{
    emit WordRecognizedQSignal(QString(word.Data.c_str()));
}


void cscSpeechToCommandsQtComponent::NoWordRecognizedHandler(void)
{
    emit NoWordRecognizedQSignal();
}


void cscSpeechToCommandsQtComponent::ContextChangedHandler(const mtsStdString & context)
{
    emit ContextChangedQSignal(QString(context.Data.c_str()));
}
