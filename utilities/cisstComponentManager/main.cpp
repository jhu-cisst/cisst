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
#include <fstream>

#include <cisstCommon/cmnTokenizer.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsManagerGlobal.h>
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
    void SetNumArgs(int nArgs) { numArgs = nArgs; }
    bool IsValidNumArgs(int nArgs) const { return (numArgs<0)?true:(nArgs == numArgs); }

    virtual bool Execute(const std::vector<std::string> &) const 
    { 
        std::cout << "CommandEntryBase::Execute called" << std::endl;
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

    bool Execute(const std::vector<std::string> &) const
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

template <class _classType>
class CommandEntryMethodStr4 : public CommandEntryBase
{
    typedef bool (_classType::*ActionType)(const std::string &arg1, const std::string &arg2,
                                           const std::string &arg3, const std::string &arg4) const;
    ActionType Action;
    _classType *classInstance;

public:
    CommandEntryMethodStr4(const std::string &cmd, const std::string &argString,
                           ActionType action, _classType *ptr) :
        CommandEntryBase(cmd, argString, 4), Action(action), classInstance(ptr) {}
    ~CommandEntryMethodStr4() {}

    bool Execute(const std::vector<std::string> &args) const
    { return (classInstance->*Action)(args[0], args[1], args[2], args[3]); }
};

template <class _classType>
class CommandEntryMethodStr6 : public CommandEntryBase
{
    typedef bool (_classType::*ActionType)(const std::string &arg1, const std::string &arg2,
                                           const std::string &arg3, const std::string &arg4,
                                           const std::string &arg5, const std::string &arg6) const;
    ActionType Action;
    _classType *classInstance;

public:
    CommandEntryMethodStr6(const std::string &cmd, const std::string &argString,
                           ActionType action, _classType *ptr) :
        CommandEntryBase(cmd, argString, 6), Action(action), classInstance(ptr) {}
    ~CommandEntryMethodStr6() {}

    bool Execute(const std::vector<std::string> &args) const
    { return (classInstance->*Action)(args[0], args[1], args[2], args[3], args[4], args[5]); }
};

class shellTask : public mtsTaskContinuous
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    static std::string lastError;
    int argc;
    char **argv;

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
    // The constructor specifies that a new thread should be created (third parameter true)
    // because otherwise the system doesn't work. It seems that the blocking I/O (i.e., waiting
    // for input from cin) interferes with proper operation.
    shellTask(const std::string &name, int _argc, char **_argv) : mtsTaskContinuous(name, 10, true), argc(_argc), argv(_argv)
    { EnableDynamicComponentManagement(); }
    ~shellTask() {}

    void Configure(const std::string & CMN_UNUSED(filename));
    void Startup(void);
    void Run(void);
    void Cleanup(void) {}

    bool ExecuteLine(const std::string &curLine) const;
    bool Quit(void) const;
    bool Help(const std::vector<std::string> &args) const;
    bool List(const std::vector<std::string> &args) const;
    bool Connections(const std::vector<std::string> &args) const;
    bool System(const std::string &cmdString) const;
    bool ExecuteFile(const std::string &fileName) const;
    bool Sleep(const std::string &time) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(shellTask)
CMN_IMPLEMENT_SERVICES(shellTask)

std::string shellTask::lastError;

#if CISST_MTS_HAS_ICE
static bool gcmFunction(const std::vector<std::string> &args)
{
    return mtsManagerLocal::GetInstance(args[0], args[1]) != 0;
}
#endif

void shellTask::Configure(const std::string &)
{
    CommandList.insert(new CommandEntryMethodVoid<shellTask>("quit", "", &shellTask::Quit, this));
    CommandList.insert(new CommandEntryMethodArgv<shellTask>("help", "[<command_name>]", &shellTask::Help, this));
    CommandList.insert(new CommandEntryMethodArgv<shellTask>("list", "[<process_name>]",
                                                             &shellTask::List, this));
    CommandList.insert(new CommandEntryMethodArgv<shellTask>("connections",
                                                             "[<process_name>] [<component_name>]",
                                                             &shellTask::Connections, this));
#if CISST_MTS_HAS_ICE
    CommandList.insert(new CommandEntryFunction("gcm", "<ip_addr> <process_name>", gcmFunction, 2));
#endif
    CommandList.insert(new CommandEntryMethodStr1<shellTask>("system", "<\"string_to_execute\">",
                                                             &shellTask::System, this));
    CommandList.insert(new CommandEntryMethodStr1<shellTask>("execute", "<file_name>",
                                                             &shellTask::ExecuteFile, this));
    CommandList.insert(new CommandEntryMethodStr1<shellTask>("sleep", "<file_name>",
                                                             &shellTask::Sleep, this));
    mtsManagerComponentServices *Manager = GetManagerComponentServices();
    if (Manager) {
        CommandList.insert(new CommandEntryMethodStr2<mtsManagerComponentServices>(
                               "create", "<class_name> <component_name>",
                               &mtsManagerComponentServices::ComponentCreate, Manager));
        CommandList.insert(new CommandEntryMethodStr3<mtsManagerComponentServices>(
                               "create", "<process_name> <class_name> <component_name>",
                               &mtsManagerComponentServices::ComponentCreate, Manager));
        CommandList.insert(new CommandEntryMethodStr4<mtsManagerComponentServices>(
                               "connect",
                               "<component1> <component1_interface> <component2> <component2_interface>",
                               &mtsManagerComponentServices::Connect, Manager));
        CommandList.insert(new CommandEntryMethodStr6<mtsManagerComponentServices>(
                               "connect",
                               "<process1> <component1> <component1_interface> "
                               "<process2> <component2> <component2_interface>",
                               &mtsManagerComponentServices::Connect, Manager));
        CommandList.insert(new CommandEntryMethodStr4<mtsManagerComponentServices>(
                               "disconnect",
                               "<component1> <component1_interface> <component2> <component2_interface>",
                               &mtsManagerComponentServices::Disconnect, Manager));
        CommandList.insert(new CommandEntryMethodStr6<mtsManagerComponentServices>(
                               "disconnect",
                               "<process1> <component1> <component1_interface> "
                               "<process2> <component2> <component2_interface>",
                               &mtsManagerComponentServices::Disconnect, Manager));
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

bool shellTask::Quit(void) const
{
    shellTask *nonConstThis = const_cast<shellTask *>(this);
    nonConstThis->Kill();
    return true;
}

bool shellTask::ExecuteLine(const std::string &curLine) const
{
    static cmnTokenizer tokens;
    bool ret = true;
    shellTask::lastError = "";
    tokens.Parse(curLine);
    std::vector<const char *> argv;
    tokens.GetArgvTokens(argv);
    // GetArgvTokens sets argv[0]=0, and argv[argv.size()-1]=0
    if ((argv.size() > 2) && argv[1]) {
        ret = false;
        std::string command;
        std::vector<std::string> args;
        command = std::string(argv[1]);
        for (size_t i = 2; i < argv.size()-1; i++)
            args.push_back(std::string(argv[i]?argv[i]:"NULL"));
        CmdListType::const_iterator it;
        // First, check if command exists
        CommandEntryBase temp(command, "", -1);
        it = CommandList.find(&temp);
        if (it == CommandList.end())
            shellTask::lastError = std::string("Unknown command: ") + command;
        else {
            // if we didn't happen to get a match on number of args, try again
            if (!(*it)->IsValidNumArgs(args.size())) {
                temp.SetNumArgs(args.size());
                it = CommandList.find(&temp);
            }
            if (it == CommandList.end())
                shellTask::lastError = command + ": invalid number of parameters";
            else
                ret = (*it)->Execute(args);
        }
    }
    return ret;
}

void shellTask::Startup(void)
{
    std::cout << "cisst Component Manager - type \"help\" for more information" << std::endl;
}

void shellTask::Run(void)
{
    static std::string curLine;
    static bool firstTime = true;
    if (firstTime) {
        // Execute any startup file that had been passed
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-e") == 0) {
                i++;
                if (i < argc)
                    ExecuteFile(argv[i]);
            }
        }
        firstTime = false;
    }
    // Display prompt
    std::cout << mtsManagerLocal::GetInstance()->GetProcessName() << "> ";
    getline(std::cin, curLine);
    if (std::cin.good()) {
        if (!ExecuteLine(curLine)) {
            if (shellTask::lastError == "")
                std::cout << "Failed" << std::endl;
            else
                std::cout << shellTask::lastError << std::endl;
        }
    }
    else
        Kill();
}

bool shellTask::Help(const std::vector<std::string> &args) const
{
    CmdListType::const_iterator it;
    if (args.size() == 0) {
        std::cout << "List of commands (type \"help <command_name>\" for details): " << std::endl;
        std::string lastCommand;
        for (it = CommandList.begin(); it != CommandList.end(); it++) {
            std::string curCommand = (*it)->GetCommand();
            if (curCommand != lastCommand) {
                std::cout << "  " << curCommand << std::endl;
                lastCommand = curCommand;
            }
        }
    }
    else if (args.size() == 1) {
        for (it = CommandList.begin(); it != CommandList.end(); it++) {
            if ((*it)->GetCommand() == args[0])
                std::cout << "  " << (*it)->GetCommand() << " " << (*it)->GetArgInfo() << std::endl;
        }
    }
    else
        std::cout << "Syntax: help [<command_name>]" << std::endl;
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
        for (size_t j = 0; j < compList.size(); j++) {
            std::cout << "    " << compList[j] << " ("
                      << ManagerComponentServices->ComponentGetState(procList[i], compList[j])
                      << ")" << std::endl;
        }
    }
    return true;
}

bool shellTask::Connections(const std::vector<std::string> &args) const
{
    std::string filterProcess;
    std::string filterComponent;
    if (args.size() == 0)
        std::cout << "Connections:" << std::endl;
    else if (args.size() == 1) {
        filterProcess = args[0];
        std::cout << "Connections to/from process " << filterProcess << ":" << std::endl;
    }
    else if (args.size() == 2) {
        filterProcess = args[0];
        filterComponent = args[1];
        std::cout << "Connections to/from process " << filterProcess << ", component "
                  << filterComponent << ":" << std::endl;
    }
    else {
        std::cout << "Invalid number of parameters: " << args.size() << std::endl;
        return false;
    }
    std::vector<mtsDescriptionConnection> connections = ManagerComponentServices->GetListOfConnections();
    for (size_t i = 0; i < connections.size(); i++) {
        mtsDescriptionConnection &connection = connections[i];
        if ((filterProcess == "") || 
            (filterProcess == connection.Client.ProcessName) ||
            (filterProcess == connection.Server.ProcessName)) {
            if ((filterComponent == "") ||
                (filterComponent == connection.Client.ComponentName) ||
                (filterComponent == connection.Server.ComponentName)) {
                // Following is same as mtsDescriptionConnection::ToStream, except that is does not
                // stream out the mtsGenericObject base.
                std::cout << "  (" << connection.ConnectionID << ") "
                          << mtsManagerGlobal::GetInterfaceUID(connection.Client.ProcessName,
                                               connection.Client.ComponentName, connection.Client.InterfaceName)
                          << " - "
                          << mtsManagerGlobal::GetInterfaceUID(connection.Server.ProcessName,
                                               connection.Server.ComponentName, connection.Server.InterfaceName)
                          << std::endl;
            }
        }
    }
    return true;
}

#include <cisstMultiTask/mtsComponentViewer.h>

#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#endif

bool shellTask::System(const std::string &cmdString) const
{
#if (CISST_OS == CISST_WINDOWS) && !defined(_UNICODE)
    // Following should be moved to cisstOSAbstraction.
    // It creates a new console window.
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    char *cmd = new char[cmdString.size()+1];
    strcpy(cmd, cmdString.c_str());

    // Start the child process. 
    if( !CreateProcess( NULL,   // No module name (use command line)
        cmd,                    // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        CREATE_NEW_CONSOLE,   // Create a new console
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        std::cout << "CreateProcess returns " << GetLastError() << std::endl;
        return false;
    }
#else
    system(cmdString.c_str());  // return value may be platform-dependent
#endif
    return true;
}

bool shellTask::ExecuteFile(const std::string &fileName) const
{
    static std::string curLine;

    std::ifstream file(fileName.c_str());
    while (!file.eof()) {
        getline(file, curLine);
        if (!ExecuteLine(curLine))
            return false;
    }
    return true;
}

bool shellTask::Sleep(const std::string &time) const
{
    double dtime;
    if (sscanf(time.c_str(), "%lf", &dtime) == 1) {
        osaSleep(dtime);
        return true;
    }
    return false;
}

// Syntax:  cisstComponentManager [global|local|ip_addr] [process_name] [-e filename]
int main(int argc, char * argv[])
{
    mtsManagerGlobal *globalManager = 0;
    mtsManagerLocal * localManager = 0;;

    if ((argc < 2) || (strcmp(argv[1], "local") == 0) || (argv[1][0] == '-')) {
        // Local configuration
        std::string processName("ProcessShell");
        if ((argc >= 3) && (argv[2][0] != '-'))
            processName = argv[2];
        // For now, ignoring processName
        localManager = mtsManagerLocal::GetInstance();
    }
#if CISST_MTS_HAS_ICE
    else if (strcmp(argv[1], "global") == 0) {
        // This is the GCM
        globalManager = new mtsManagerGlobal;
        if (!globalManager->StartServer()) {
            std::cout << "Failed to start global component manager." << std::endl;
            return 1;
        }
        std::cout << "Global component manager started..." << std::endl;
        try {
            localManager = mtsManagerLocal::GetInstance(*globalManager);
        } catch (...) {
            std::cout << "Failed to initialize local component manager." << std::endl;
            return 1;
        }
    }
    else if (argv[1][0] != '-') {
        // Connecting to GCM at specified IP address
        std::string processName("ProcessShell");
        if ((argc >= 3) && (argv[2][0] != '-'))
            processName = argv[2];
        try {
            localManager = mtsManagerLocal::GetInstance(argv[1], processName);
        } catch (...) {
            std::cout << "Failed to initialize local component manager." << std::endl;
            return 1;
        }
    }
#else
    else {
        std::cout << "No network support -- set CISST_MTS_HAS_ICE via CMake" << std::endl;
        return 1;
    }
#endif

    shellTask *shell = new shellTask("cisstShell", argc, argv);
    localManager->AddComponent(shell);
    shell->Configure("");

    localManager->CreateAll();
    localManager->StartAll();

    while (!shell->IsTerminated())
        osaSleep(0.1);


#if CISST_MTS_HAS_ICE
    // Cleanup global component manager
    // We don't delete it because that is currently done in localManager->Cleanup
    if (globalManager) {
        if (!globalManager->StopServer())
            std::cout << "Failed to stop global component manager." << std::endl;
    }
#endif

    localManager->Cleanup();
    return 0;
}
