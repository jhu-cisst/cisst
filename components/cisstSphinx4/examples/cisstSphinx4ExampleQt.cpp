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

#include <cisstSphinx4/mtsSphinx4.h>
#include <cisstSphinx4/mtsSphinx4QtComponent.h>

#include <QApplication>

int main(int argc, char ** argv)
{
    int microphoneNumber = 1; // default
    if (argc == 2) {
        microphoneNumber = atoi(argv[1]);
    } else {
        std::cout << "Usage: " << argv[0]
                  << " <microphone-number> (default is " << microphoneNumber
                  << ")" << std::endl;
    }
    std::cout << "Using microphone " << microphoneNumber << std::endl;
        
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    cmnLogger::SetMaskClassMatching("mtsSphinx4", CMN_LOG_ALLOW_ALL);

    // create Qt user interface
    QApplication application(argc, argv);

    // create a vertical widget for quit button and tabs
    QWidget * mainWidget = new QWidget();
    mainWidget->setWindowTitle("cisstSphinx4 example");
    QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // get the component manager to add multiple sine generator tasks
    mtsManagerLocal * componentManager = mtsManagerLocal::GetInstance();

    mtsSphinx4 * sphinx4 = new mtsSphinx4("Sphinx4");

    sphinx4->SetMicrophoneNumber(microphoneNumber);

    mtsSphinx4::Context * areYouTalkingToMe = sphinx4->AddContext("areYouTalkingToMe");
    areYouTalkingToMe->SetFiltering(true);
    mtsSphinx4::Context * confirmStart = sphinx4->AddContext("confirmStart");
    mtsSphinx4::Context * constructiveConversation = sphinx4->AddContext("constructiveConversation");
    mtsSphinx4::Context * confirmStop = sphinx4->AddContext("confirmStop");

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

    sphinx4->SetCurrentContext("areYouTalkingToMe");
    sphinx4->Configure(); // creates all configurations files for Sphinx
    componentManager->AddComponent(sphinx4);

    mtsSphinx4QtComponent * sphinx4QtComponent
        = new mtsSphinx4QtComponent("Sphinx4QtComponent");
    mainLayout->addWidget(sphinx4QtComponent->GetWidget());
    componentManager->AddComponent(sphinx4QtComponent);

    componentManager->Connect(sphinx4QtComponent->GetName(), "Sphinx4",
                              sphinx4->GetName(), "Default");

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
