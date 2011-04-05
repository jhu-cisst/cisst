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
    std::string argInfo;
    int numArgs;   // number of parameters (negative means variable)
public:
    CommandEntryBase(const std::string &cmd, const std::string argString, int nArgs) :
                     command(cmd), argInfo(argString), numArgs(nArgs) {}
    virtual ~CommandEntryBase() {}

    std::string GetCommand(void) const { return command; }
    std::string GetArgInfo(void) const { return argInfo; }

    int GetNumArgs(void) const { return numArgs; }
    bool IsValidNumArgs(int nArgs) const { return (numArgs<0)?true:(nArgs == numArgs); }

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
    CommandEntryFunction(const std::string &cmd, const std::string &argString,
                         ActionType action, int nArgs = -1) :
        CommandEntryBase(cmd, argString, nArgs), Action(action) {}
    ~CommandEntryFunction() {}

    bool Execute(const std::vector<std::string> &args) const
    { return Action(args); }
};

template <class _classType>
class CommandEntryMethodArgv : public CommandEntryBase
{
    typedef bool (_classType::*ActionType)(const std::vector<std::string> &args) const;
    ActionType Action;
    _classType *classInstance;

public:
    CommandEntryMethodArgv(const std::string &cmd, const std::string &argString,
                           ActionType action, _classType *ptr, int nArgs = -1) :
        CommandEntryBase(cmd, argString, nArgs), Action(action), classInstance(ptr) {}
    ~CommandEntryMethodArgv() {}

    bool Execute(const std::vector<std::string> &args) const
    { return (classInstance->*Action)(args); }
};

template <class _classType>
class CommandEntryMethodVoid : public CommandEntryBase
{
    typedef bool (_classType::*ActionType)(void) const;
    ActionType Action;
    _classType *classInstance;

public:
    CommandEntryMethodVoid(const std::string &cmd, const std::string &argString,
                           ActionType action, _classType *ptr) :
        CommandEntryBase(cmd, argString, 0), Action(action), classInstance(ptr) {}
    ~CommandEntryMethodVoid() {}

    bool Execute(const std::vector<std::string> &args) const
    { return (classInstance->*Action)(); }
};

template <class _classType>
class CommandEntryMethodStr1 : public CommandEntryBase
{
    typedef bool (_classType::*ActionType)(const std::string &arg1) const;
    ActionType Action;
    _classType *classInstance;

public:
    CommandEntryMethodStr1(const std::string &cmd, const std::string &argString,
                           ActionType action, _classType *ptr) :
        CommandEntryBase(cmd, argString, 1), Action(action), classInstance(ptr) {}
    ~CommandEntryMethodStr1() {}

    bool Execute(const std::vector<std::string> &args) const
    { return (classInstance->*Action)(args[0]); }
};

template <class _classType>
class CommandEntryMethodStr2 : public CommandEntryBase
{
    typedef bool (_classType::*ActionType)(const std::string &arg1, const std::string &arg2) const;
    ActionType Action;
    _classType *classInstance;

public:
    CommandEntryMethodStr2(const std::string &cmd, const std::string &argString,
                           ActionType action, _classType *ptr) :
        CommandEntryBase(cmd, argString, 2), Action(action), classInstance(ptr) {}
    ~CommandEntryMethodStr2() {}

    bool Execute(const std::vector<std::string> &args) const
    { return (classInstance->*Action)(args[0], args[1]); }
};


template <class _classType>
class CommandEntryMethodStr3 : public CommandEntryBase
{
    typedef bool (_classType::*ActionType)(const std::string &arg1, const std::string &arg2,
                                           const std::string &arg3) const;
    ActionType Action;
    _classType *classInstance;

public:
    CommandEntryMethodStr3(const std::string &cmd, const std::string &argString,
                           ActionType action, _classType *ptr) :
        CommandEntryBase(cmd, argString, 3), Action(action), classInstance(ptr) {}
    ~CommandEntryMethodStr3() {}

    bool Execute(const std::vector<std::string> &args) const
    { return (classInstance->*Action)(args[0], args[1], args[2]); }
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
            if (p1->GetCommand() == p2->GetCommand()) {
                if ((p1->GetNumArgs() == -1) || (p2->GetNumArgs() == -1))
                    return false;
                return p1->GetNumArgs() < p2->GetNumArgs();
            }
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

    bool Quit(void) const
    { /*Kill();*/ return true; }
    bool Help(void) const;
    bool List(const std::vector<std::string> &args) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(shellTask)
CMN_IMPLEMENT_SERVICES(shellTask)

static bool gcmFunction(const std::vector<std::string> &args)
{
    return mtsManagerLocal::GetInstance(args[0], args[1]) != 0;
}

void shellTask::Configure(const std::string &)
{
    CommandList.insert(new CommandEntryMethodVoid<shellTask>("quit", "", &shellTask::Quit, this));
    CommandList.insert(new CommandEntryMethodVoid<shellTask>("help", "", &shellTask::Help, this));
    CommandList.insert(new CommandEntryMethodArgv<shellTask>("list", "[<process_name>]",
                                                             &shellTask::List, this));
    CommandList.insert(new CommandEntryFunction("gcm", "<ip_addr> <process_name>", gcmFunction, 2));
    mtsManagerComponentServices *Manager = GetManagerComponentServices();
    if (Manager) {
        CommandList.insert(new CommandEntryMethodStr2<mtsManagerComponentServices>(
                               "create", "<class_name> <component_name>",
                               &mtsManagerComponentServices::ComponentCreate, Manager));
        CommandList.insert(new CommandEntryMethodStr3<mtsManagerComponentServices>(
                               "create", "<process_name> <class_name> <component_name>",
                               &mtsManagerComponentServices::ComponentCreate, Manager));
        CommandList.insert(new CommandEntryMethodStr1<mtsManagerComponentServices>(
                               "start", "<component_name>",
                               &mtsManagerComponentServices::ComponentStart, Manager));
        CommandList.insert(new CommandEntryMethodStr2<mtsManagerComponentServices>(
                               "start", "<process_name> <component_name>",
                               &mtsManagerComponentServices::ComponentStart, Manager));
        CommandList.insert(new CommandEntryMethodStr1<mtsManagerComponentServices>(
                               "stop", "<component_name>",
                                &mtsManagerComponentServices::ComponentStop, Manager));
        CommandList.insert(new CommandEntryMethodStr2<mtsManagerComponentServices>(
                               "stop", "<process_name> <component_name>",
                                &mtsManagerComponentServices::ComponentStop, Manager));
        CommandList.insert(new CommandEntryMethodStr1<mtsManagerComponentServices>(
                               "load", "<file_name>",
                                &mtsManagerComponentServices::Load, Manager));
        CommandList.insert(new CommandEntryMethodStr2<mtsManagerComponentServices>(
                               "load", "<process_name> <file_name>",
                                &mtsManagerComponentServices::Load, Manager));
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
                // First, check if command exists
                it = CommandList.find(&CommandEntryBase(command, "", -1));
                if (it == CommandList.end())
                    std::cout << "Unknown command: " << command << std::endl;
                else {
                    // if we didn't happen to get a match on number of args, try again
                    if (!(*it)->IsValidNumArgs(args.size()))
                        it = CommandList.find(&CommandEntryBase(command, "", args.size()));
                    if (it == CommandList.end())
                        std::cout << command << ": invalid number of parameters: " << args.size() << std::endl;
                    else
                        (*it)->Execute(args);  // could check return value
                }
            }
        }
    }
    else
        Kill();
}

bool shellTask::Help(void) const
{
    std::cout << "List of commands: " << std::endl;
    CmdListType::const_iterator it;
    for (it = CommandList.begin(); it != CommandList.end(); it++)
        std::cout << " " << (*it)->GetCommand() << "\t" << (*it)->GetArgInfo() << std::endl;
    return true;
}

bool shellTask::List(const std::vector<std::string> &args) const
{
    std::vector<std::string> procList;
    if (args.size() > 0) {
        procList.push_back(args[0]);
        std::cout << "List of all process components:" << std::endl;
    }
    else {
        std::cout << "List of all processes/components:" << std::endl;
        procList = ManagerComponentServices->GetNamesOfProcesses();
    }
    for (size_t i = 0; i < procList.size(); i++) {
        std::cout << "  " << procList[i] << std::endl;
        std::vector<std::string> compList;
        compList = ManagerComponentServices->GetNamesOfComponents(procList[i]);
        for (size_t j = 0; j < compList.size(); j++)
            std::cout << "    " << compList[j] << std::endl;
    }
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
