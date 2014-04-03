/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides
  Created on: 2012-06-26

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsQtApplication.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

#include <QApplication>
#include <QBasicTimer>

CMN_IMPLEMENT_SERVICES(mtsQtApplicationConstructorArg)
CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mtsQtApplication, mtsTaskContinuous, mtsQtApplicationConstructorArg)

mtsQtApplicationConstructorArg::mtsQtApplicationConstructorArg() : mtsGenericObject(),
                                                                   Name("QtApplication")
{
    Args.push_back("Arg0");
}

mtsQtApplicationConstructorArg::mtsQtApplicationConstructorArg(const std::string &name, int argc, char **argv) :
    mtsGenericObject(),
    Name(name)
{
    for (int i = 0; i < argc; i++)
        Args.push_back(argv[i]);
}

mtsQtApplicationConstructorArg::mtsQtApplicationConstructorArg(const mtsQtApplicationConstructorArg &other) :
    mtsGenericObject(),
    Name(other.Name),
    Args(other.Args)
{
}

mtsQtApplicationConstructorArg::~mtsQtApplicationConstructorArg()
{
}

void mtsQtApplicationConstructorArg::GetArgv(char ***argvp) const
{
    *argvp = new char *[Args.size()];
    for (size_t i = 0; i < Args.size(); i++) {
        size_t len = Args[i].length();
        (*argvp)[i] = new char[len+1];
        Args[i].copy((*argvp)[i], len);
        (*argvp)[i][len] = 0;  // null terminate
    }
}

void mtsQtApplicationConstructorArg::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, Name);
    cmnSerializeRaw(outputStream, Args);
}

void mtsQtApplicationConstructorArg::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, Name);
    cmnDeSerializeRaw(inputStream, Args);
}

void mtsQtApplicationConstructorArg::ToStream(std::ostream & outputStream) const
{
    outputStream << "Name: " << Name
                 << ", Args:";
    for (size_t i = 0; i < Args.size(); i++)
        outputStream << " " << Args[i];
}

// Only streams out the argument size
void mtsQtApplicationConstructorArg::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                                bool headerOnly, const std::string & headerPrefix) const
{
    mtsGenericObject::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
    if (headerOnly) {
        outputStream << headerPrefix << "-name" << delimiter
                     << headerPrefix << "-argsize" << delimiter;
    } else {
        outputStream << this->Name << delimiter
                     << this->Args.size() << delimiter;
    }
}

bool mtsQtApplicationConstructorArg::FromStreamRaw(std::istream & inputStream, const char delimiter)
{
    Args.clear();
    mtsGenericObject::FromStreamRaw(inputStream, delimiter);
    if (inputStream.fail())
        return false;
    inputStream >> Name;
    if (inputStream.fail())
        return false;
    std::string temp;
    while (!inputStream.eof()) {
        temp.clear();
        inputStream >> temp;
        if (!temp.empty())
            Args.push_back(temp);
    }
    return (typeid(*this) == typeid(mtsQtApplicationConstructorArg));
}

class BasicTimerHandler : public QObject {
// Qt strongly recommends putting Q_OBJECT here
protected:
    mtsQtApplication * QtApp;
    void timerEvent(QTimerEvent * CMN_UNUSED(event)) {
        if (QtApp) {
            QtApp->Process();
        }
    }
public:
    BasicTimerHandler(mtsQtApplication * qa):
        QObject(0),
        QtApp(qa)
    {
        setObjectName("BasicTimerHandler");
    }
    ~BasicTimerHandler() {}
};

mtsQtApplication::mtsQtApplication(const std::string &name, int &argc, char **argv):
    mtsTaskContinuous(name, 16, false),
    ArgcCopy(0),
    ArgvCopy(0)
{
    QtApp = new QApplication(argc, argv);
    Timer = new QBasicTimer;
    TimerHandler = new BasicTimerHandler(this);
}

mtsQtApplication::mtsQtApplication(const mtsQtApplicationConstructorArg &arg):
    mtsTaskContinuous(arg.GetName(), 16, false)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "mtsQtApplication: " << arg << std::endl;
    ArgcCopy = arg.GetArgc();
    arg.GetArgv(&ArgvCopy);
    QtApp = new QApplication(ArgcCopy, ArgvCopy);
    Timer = new QBasicTimer;
    TimerHandler = new BasicTimerHandler(this);
}

mtsQtApplication::~mtsQtApplication()
{
    delete QtApp;
    delete Timer;
    delete TimerHandler;
    if (ArgvCopy) {
        for (int i = 0; i < ArgcCopy; i++) {
            delete [] ArgvCopy[i];
        }
        delete [] ArgvCopy;
    }
}

void mtsQtApplication::Startup(void)
{
    Timer->start(50, TimerHandler);  // Start 50 milliseconds second timer
}

#include <cisstOSAbstraction/osaSleep.h>

void mtsQtApplication::Run(void)
{
    QtApp->exec();
    osaSleep(1.0 * cmn_s);
    Timer->stop();
    Kill();
}

void mtsQtApplication::Process(void)
{
    if (InterfaceProvidedToManager) {
        InterfaceProvidedToManager->ProcessMailBoxes();
    }
    ProcessQueuedCommands();
}
