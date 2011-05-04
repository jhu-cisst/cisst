/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: main.cpp 2934 2011-04-19 03:29:58Z adeguet1 $

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

#include <iostream>
#include "cscSpeechToCommands.h"
#include "cscSpeechToCommandsQtComponent.h"

#include <QApplication>
#include <QTabWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QWidget>

int main(int argc, char ** argv) {

    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_DEBUG);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    cmnLogger::SetMaskClassMatching("csc", CMN_LOG_ALLOW_ALL);

    // create Qt user interface
    QApplication application(argc, argv);

    // create a vertical widget for quit button and tabs
    QWidget * mainWidget = new QWidget();
    mainWidget->setWindowTitle("cisstSpeechToCommands example");
    QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // get the component manager to add multiple sine generator tasks
    mtsManagerLocal * componentManager = mtsManagerLocal::GetInstance();

    cscSpeechToCommands * speechToCommands =
        new cscSpeechToCommands("mySpeechToCommands");

    speechToCommands->SetMicrophoneNumber(1);

    cscContext * areYouTalkingToMe = speechToCommands->AddContext("areYouTalkingToMe");
    areYouTalkingToMe->SetFiltering(true);
    cscContext * confirmStart = speechToCommands->AddContext("confirmStart");
    cscContext * constructiveConversation = speechToCommands->AddContext("constructiveConversation");
    cscContext * confirmStop = speechToCommands->AddContext("confirmStop");

    areYouTalkingToMe->AddWordWithTransition("voice control", "confirmStart");

    confirmStart->AddWordWithTransition("yes", "constructiveConversation");
    confirmStart->AddWordWithTransition("no", "areYouTalkingToMe");

    constructiveConversation->AddWordWithTransition("voice control", "confirmStop");
    constructiveConversation->AddWord("green");
    constructiveConversation->AddWord("green house");
    constructiveConversation->AddWord("blue");
    constructiveConversation->AddWord("blue lagoon");
    constructiveConversation->AddWord("red");
    constructiveConversation->AddWord("red light");
    constructiveConversation->AddWord("red apple");
    constructiveConversation->AddWord("yellow");
    constructiveConversation->AddWord("yellow tail");
    constructiveConversation->AddWord("white");
    constructiveConversation->AddWord("snow white");
    constructiveConversation->AddWord("black");
    constructiveConversation->AddWord("black sheep");

    confirmStop->AddWordWithTransition("yes", "areYouTalkingToMe");
    confirmStop->AddWordWithTransition("no", "constructiveConversation");

    speechToCommands->SetCurrentContext("areYouTalkingToMe");
    speechToCommands->Configure(); // creates all configurations files for Sphinx
    componentManager->AddComponent(speechToCommands);

    cscSpeechToCommandsQtComponent * speechQtComponent
        = new cscSpeechToCommandsQtComponent("SpeechQtComponent");
    mainLayout->addWidget(speechQtComponent->GetWidget());
    componentManager->AddComponent(speechQtComponent);

    componentManager->Connect(speechQtComponent->GetName(), "SpeechToCommands",
                              speechToCommands->GetName(), "Default");

    mainWidget->show();

    // create and start all tasks
    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY);

    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    application.exec();

    // kill all tasks and perform cleanup
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);

    componentManager->Cleanup();

    // stop all logs
    cmnLogger::SetMask(CMN_LOG_ALLOW_NONE);

    return 0;
}
