/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides
  Created on: 2011-04-03

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <string>
#include <set>
#include <functional>

#include <cisstCommon/cmnTokenizer.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstMultiTask/mtsManagerComponentServices.h>

class CommandEntryBase {
    std::string command;
public:
    CommandEntryBase(const std::string &cmd) : command(cmd) {}
    virtual ~CommandEntryBase() {}

    std::string GetCommand(void) const { return command; }
    virtual bool Execute(const std::vector<std::string> &args) const 
    { 
        CMN_LOG_RUN_ERROR << "CommandEntryBase::Execute called" << std::endl;
        return false;
    }
};

class CommandEntryFunction : public CommandEntryBase
{
    typedef bool (*ActionType)(const std::vector<std::string> &args);
    ActionType Action;
public:
    CommandEntryFunction(const std::string &cmd, ActionType action) :
        CommandEntryBase(cmd), Action(action) {}
    ~CommandEntryFunction() {}

    bool Execute(const std::vector<std::string> &args) const
    { return Action(args); }
};

template <class _classType>
class CommandEntryMethod : public CommandEntryBase
{
    typedef bool (_classType::*ActionType)(const std::vector<std::string> &args) const;
    ActionType Action;
    _classType *classInstance;
public:
    CommandEntryMethod(const std::string &cmd, ActionType action, _classType *ptr) :
        CommandEntryBase(cmd), Action(action), classInstance(ptr) {}
    ~CommandEntryMethod() {}

    bool Execute(const std::vector<std::string> &args) const
    { return (classInstance->*Action)(args); }
};

class shellTask : public mtsTaskContinuous
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    std::string curLine;
    cmnTokenizer tokens;

    // Comparison operator for std::set
    struct CmdListLess: public std::binary_function<const CommandEntryBase*, const CommandEntryBase*, bool>
    {
        result_type operator()(first_argument_type p1, second_argument_type p2) const
        {
            return p1->GetCommand() < p2->GetCommand();
        }
    };
   typedef std::set<CommandEntryBase *, CmdListLess> CmdListType;
   CmdListType CommandList;

public:
    shellTask(const std::string &name) : mtsTaskContinuous(name, 10, false)
    { EnableDynamicComponentManagement(); }
    ~shellTask() {}

    void Configure(const std::string & CMN_UNUSED(filename));
    void Startup(void) {}
    void Run(void);
    void Cleanup(void) {}

    bool Quit(const std::vector<std::string> &args) const
    { /*Kill();*/ return true; }
    bool Help(const std::vector<std::string> &args) const;
    bool Debug(const std::vector<std::string> &args) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(shellTask)
CMN_IMPLEMENT_SERVICES(shellTask)

static bool gcmFunction(const std::vector<std::string> &args)
{
    if (args.size() == 2)
        return (mtsManagerLocal::GetInstance(args[0], args[1]) != 0);
    else {
        std::cout << "Syntax: gcm ipAddr processName" << std::endl;
        return false;
    }
}

void shellTask::Configure(const std::string &)
{
    CommandList.insert(new CommandEntryMethod<shellTask>("quit", &shellTask::Quit, this));
    CommandList.insert(new CommandEntryMethod<shellTask>("help", &shellTask::Help, this));
    CommandList.insert(new CommandEntryMethod<shellTask>("debug", &shellTask::Debug, this));
    CommandList.insert(new CommandEntryFunction("gcm", gcmFunction));
    mtsManagerComponentServices *Manager = GetManagerComponentServices();
    if (Manager) {
        CommandList.insert(new CommandEntryMethod<mtsManagerComponentServices>(
                               "create", &mtsManagerComponentServices::ComponentCreate, Manager));
        CommandList.insert(new CommandEntryMethod<mtsManagerComponentServices>(
                               "start", &mtsManagerComponentServices::ComponentStart, Manager));
    }
}

void shellTask::Run(void)
{
    // Display prompt
    std::cout << "cisst> ";
    getline(std::cin, curLine);
    if (std::cin.good()) {
        tokens.Parse(curLine);
        std::vector<const char *> argv;
        tokens.GetArgvTokens(argv);
        // GetArgvTokens sets argv[0]=0, and argv[argv.size()-1]=0
        if ((argv.size() > 2) && argv[1]) {
            std::string command;
            std::vector<std::string> args;
            command = std::string(argv[1]);
            for (size_t i = 2; i < argv.size()-1; i++)
                args.push_back(std::string(argv[i]?argv[i]:"NULL"));
            if (command == "quit")
                Kill();
            else {
                CmdListType::iterator it;
                it = CommandList.find(&CommandEntryBase(command));
                if (it == CommandList.end())
                    std::cout << "Unknown command: " << command << std::endl;
                else
                    (*it)->Execute(args);  // could check return value
            }
        }
    }
    else
        Kill();
}

bool shellTask::Help(const std::vector<std::string> &args) const
{
    // For now, ignore args
    std::cout << "List of commands: " << std::endl;
    CmdListType::const_iterator it;
    for (it = CommandList.begin(); it != CommandList.end(); it++)
        std::cout << " " << (*it)->GetCommand() << std::endl;
    return true;
}

bool shellTask::Debug(const std::vector<std::string> &args) const
{
    std::cout << "List of components: " << std::endl;
    std::vector<std::string> compList;
    compList = ManagerComponentServices->GetNamesOfComponents(mtsManagerLocal::GetInstance()->GetProcessName());
    for (size_t i = 0; i < compList.size(); i++)
        std::cout << "  " << compList[i] << std::endl;
    return true;
}

int main(int argc, char * argv[])
{
    std::string globalComponentManagerIP;

    // Get the TaskManager instance and set operation mode
    mtsManagerLocal * taskManager;
    if (argc < 2)  // local configuration
        taskManager = mtsManagerLocal::GetInstance();
    else {         // network configuration
        try {
            taskManager = mtsManagerLocal::GetInstance(argv[1], "ProcessShell");
        } catch (...) {
            CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
            return 1;
        }
    }

    shellTask *shell = new shellTask("cisstShell");
    taskManager->AddComponent(shell);
    shell->Configure("");

    taskManager->CreateAll();
    taskManager->StartAll();
    /// does not return until shell task is exited

    taskManager->Cleanup();
    return 0;
}
