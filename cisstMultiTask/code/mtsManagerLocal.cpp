/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Min Yang Jung
  Created on: 2009-12-07

  (C) Copyright 2009-2025 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsManagerLocal.h>

#include <cisstCommon/cmnThrow.h>
#include <cisstCommon/cmnPath.h>
#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaSocket.h>
#include <cisstOSAbstraction/osaTimeServer.h>
#include <cisstOSAbstraction/osaDynamicLoader.h>
#include <cisstOSAbstraction/osaMutex.h>

#include <cisstMultiTask/mtsConfig.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceOutput.h>
#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsTaskFromCallback.h>
#include <cisstMultiTask/mtsTaskFromSignal.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsInterfaceInput.h>
#include <cisstMultiTask/mtsManagerComponent.h>
#include <cisstMultiTask/mtsManagerComponentServices.h>
#include <cisstMultiTask/mtsLODMultiplexerStreambuf.h>

// Time server used by all tasks
osaTimeServer TimeServer;
bool TimeServerOriginSet = false;

mtsManagerLocal * mtsManagerLocal::Instance = 0;

// System-wide logging: Define logger-related variables here so that
// the logger doesn't have to call GetInstance() everytime it receives
// log messages. {{
mtsLODMultiplexerStreambuf * SystemLogMultiplexer = 0;
bool           LogForwardEnabled = false;
bool           LogDisabled = false;
osaMutex       LogMutex;

typedef std::list<mtsLogMessage> LogQueueType;
LogQueueType   LogQueue;

// }}

//************************* Protected Methods **************************************

mtsManagerLocal::mtsManagerLocal(void) : Name("LCM"), ManagerComponent(0), LocalComponent(0)
{
    Initialize();
}

mtsManagerLocal::~mtsManagerLocal()
{
    Cleanup();
}

void mtsManagerLocal::Initialize(void)
{
    __os_init();

    CurrentMainTask = 0;

    TimeServer.SetTimeOrigin();
    TimeServerOriginSet = true;

    SetupSystemLogger();

    ValidComponentTags.clear();
    ValidComponentTags.insert("ROS");
    ValidComponentTags.insert("UI");
    ValidComponentTags.insert("System");

    ValidInterfaceTags.clear();
    ValidInterfaceTags.insert("System");
    ValidInterfaceTags.insert("State table");

    // Leave ProcessName empty
}

void mtsManagerLocal::Cleanup(void)
{
    if (LogThreadFinishWaiting) return;

    LogThreadFinishWaiting = true;
    LogThreadFinished.Wait();

    if (ManagerComponent) {
        ManagerComponent->Kill();
        delete ManagerComponent;
        ManagerComponent = 0;
    }

    if (LocalComponent) {
        delete LocalComponent;
        LocalComponent = 0;
    }

    if (SystemLogMultiplexer) {
        cmnLogger::GetMultiplexer()->RemoveMultiplexer(SystemLogMultiplexer);
        SystemLogMultiplexer->RemoveAllChannels();
        delete SystemLogMultiplexer;
        SystemLogMultiplexer = 0;
    }

    __os_exit();
}

bool mtsManagerLocal::CreateManagerComponent(void)
{
    // Create manager component
    mtsManagerComponent * managerComponent = new mtsManagerComponent();
    CMN_LOG_CLASS_INIT_VERBOSE << "CreateManagerComponent: created " << managerComponent->GetName() << std::endl;

    managerComponent->Create();

    // Set up local component with dynamic component management
    LocalComponent = new mtsComponentWithManagement("LCM");

    std::string requiredName = mtsManagerComponentBase::GetNameOfInterfaceInternalRequired();
    mtsInterfaceRequired *required = LocalComponent->GetInterfaceRequired(requiredName);
    if (!required) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateManagerComponent: failed to find required interface "
                                 << requiredName << std::endl;
        return false;
    }
    std::string providedName = mtsManagerComponentBase::GetNameOfInterfaceComponentProvided();
    mtsInterfaceProvided *provided = managerComponent->GetInterfaceProvided(providedName);
    if (!provided) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateManagerComponent: failed to find provided interface "
                                 << providedName << std::endl;
        return false;
    }

    // Here, we know that Manager Component is not yet running, so can call internal ConnectTo method
    if (!required->ConnectTo(provided)) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateManagerComponent: failed to connect LCM to ComponentManager" << std::endl;
        return false;
    }

    // Add an interface to PrintLog
    std::string logInterfaceRequired = mtsManagerComponentBase::InterfaceNames::InterfaceSystemLoggerRequired;
    mtsInterfaceRequired *requiredLog = LocalComponent->AddInterfaceRequired(logInterfaceRequired);
    if (requiredLog) {
        requiredLog->AddFunction(mtsManagerComponentBase::CommandNames::PrintLog, Logger.PrintLog);
    }
    else {
        CMN_LOG_CLASS_INIT_WARNING << "CreateManagerComponent: failed to add Logger interface" << std::endl;
    }
    std::string logInterfaceProvided = mtsManagerComponentBase::InterfaceNames::InterfaceSystemLoggerProvided;
    mtsInterfaceProvided *providedLog = managerComponent->GetInterfaceProvided(logInterfaceProvided);
    if (!providedLog) {
        CMN_LOG_CLASS_INIT_WARNING << "CreateManagerComponent: failed to find provided interface "
                                 << providedName << std::endl;
    }
    if (requiredLog && providedLog) {
        // Here, we know that Manager Component is not yet running, so can call internal ConnectTo method
        if (!requiredLog->ConnectTo(providedLog)) {
            CMN_LOG_CLASS_INIT_WARNING << "CreateManagerComponent: failed to connect provided/required log interfaces" << std::endl;
        }
    }

    // Start manager component
    managerComponent->Start();

    return true;
}

mtsManagerComponentServices * mtsManagerLocal::GetManagerServices() const
{
    return LocalComponent->GetManagerComponentServices();
}

const mtsManagerLocal::LogInterface * mtsManagerLocal::GetLoggerServices() const
{
    return &Logger;
}

void mtsManagerLocal::GetInterfaceProvidedDescription(
    const std::string & componentName, const std::string & interfaceName,
    mtsInterfaceProvidedDescription & interfaceProvidedDescription)
{
    interfaceProvidedDescription = GetManagerServices()->GetInterfaceProvidedDescription(componentName, interfaceName);
}

void mtsManagerLocal::GetInterfaceRequiredDescription(
    const std::string & componentName, const std::string & interfaceName,
    mtsInterfaceRequiredDescription & interfaceRequiredDescription)
{
    interfaceRequiredDescription = GetManagerServices()->GetInterfaceRequiredDescription(componentName, interfaceName);
}

void mtsManagerLocal::SetupSystemLogger(void)
{
    LogThreadFinishWaiting = false;
    LogThread.Create<mtsManagerLocal, void *>(this, &mtsManagerLocal::LogDispatchThread);

    SystemLogMultiplexer = new mtsLODMultiplexerStreambuf();
    if (!cmnLogger::GetMultiplexer()->AddMultiplexer(SystemLogMultiplexer)) {
        CMN_LOG_INIT_ERROR << "Failed to add mts system logger" << std::endl;
    }
}

//*************************** Public Methods ****************************************

mtsManagerLocal * mtsManagerLocal::GetInstance(void)
{
    if (!Instance) {
        Instance = new mtsManagerLocal;
        Instance->MainThreadId = osaGetCurrentThreadId();

        // Create manager component
        if (!Instance->CreateManagerComponent()) {
            CMN_LOG_INIT_ERROR << "class mtsManagerLocal: GetInstance: Failed to add internal manager component" << std::endl;
        }
    }

    return Instance;
}

void mtsManagerLocal::DeleteInstance(void)
{
    if (Instance) {
        Instance->Cleanup();

        delete Instance;
        Instance = 0;
    }
}

mtsComponent * mtsManagerLocal::CreateComponentDynamically(const std::string & className, const std::string & componentName,
                                                           const std::string & constructorArgSerialized)
{
    cmnGenericObject * baseObject = 0;
    mtsComponent * newComponent = 0;
    const cmnClassServicesBase * services = cmnClassRegister::FindClassServices(className);
    if (!services) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamically: unable to create component of type \""
                                 << className << "\" -- no services (make sure the macros CMN_DECLARE_SERVICES and CMN_IMPLEMENT_SERVICES have been used)" << std::endl;
        return 0;
    }
    bool isComponent = (services->IsDerivedFrom<mtsComponent>()) || (className == "mtsComponent");
    const cmnClassServicesBase *argServices = services->GetConstructorArgServices();
    if (services->OneArgConstructorAvailable() && argServices) {
        if (!isComponent) {
            CMN_LOG_CLASS_INIT_WARNING << "Class " << className << " has one arg constructor, "
                                       << "but class services does not show inheritance from mtsComponent " << std::endl;
        }
        // We can create the object using the "one argument" constructor.  This includes the case where
        // the "one argument" constructor is just an std::string (including the combination of default
        // constructor and SetName method).
        cmnGenericObject *tempArg = 0;
        if (!constructorArgSerialized.empty()) {
            // Case 1: If the serialized constructor arg is not empty, then we just deserialize it and call
            //         CreateWithArg.  We could check if the arg is the correct type, but CreateWithArg will
            //         do it anyway.
            std::stringstream buffer(constructorArgSerialized);
            cmnDeSerializer deserializer(buffer);
            try {
                tempArg = dynamic_cast<cmnGenericObject *>(deserializer.DeSerialize());
            } catch (std::exception &e) {
                CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamically: failed to deserialize constructor arg for class \""
                                         << className << "\", error = " << e.what() << std::endl;
                return 0;
            }

            baseObject = services->CreateWithArg(*tempArg);
            delete tempArg;
        }
        else {
            // Case 2: If the serialized constructor arg is empty, then we just have the componentName.
            //         There are actually 2 sub-cases (see below)
            mtsGenericObjectProxyRef<std::string> tempRef(componentName);
            if (argServices == mtsStdString::ClassServices())
                // Case 2a: We just have a string (component name)
                baseObject = services->CreateWithArg(tempRef);
            else {
                // Case 2b: The componentName actually contains the streamed constructor arg (i.e., created
                //          with ToStreamRaw, rather than with SerializeRaw).
                tempArg = argServices->Create();
                if (tempArg) {
                    std::stringstream ss;
                    tempRef.ToStreamRaw(ss);
                    if (!tempArg->FromStreamRaw(ss)) {
                        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamically: Could not parse \""
                                                 << componentName << "\" for constructor of "
                                                 << className << std::endl;
                    }
                    else {
                        baseObject = services->CreateWithArg(*tempArg);
                    }
                    delete tempArg;
                }
                else
                    CMN_LOG_CLASS_INIT_ERROR << "Could not create constructor argument for " << className << std::endl;
            }
        }
        if (baseObject) {
            // If we were able to create an object, dynamic cast it to an mtsComponent so that we can return it.
            newComponent = dynamic_cast<mtsComponent *>(baseObject);
            if (newComponent) {
                CMN_LOG_CLASS_INIT_VERBOSE << "CreateComponentDynamically: successfully created new component: "
                               << "\"" << newComponent->GetName() << "\" of type \""
                                           << className << "\" with arg " << argServices->GetName() << std::endl;

                return newComponent;
            }
            else
                CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamically: class \"" << className
                                         << "\" is not derived from mtsComponent" << std::endl;
        }
    }
    else if (!constructorArgSerialized.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamically: class \"" << className
                                 << "\" cannot handle serialized constructor arg" << std::endl;
        return 0;
    }

    // Above should have worked, following is for backward compatibility
    // looking in class register to create this component
    baseObject = cmnClassRegister::Create(className);
    if (!baseObject) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamically: unable to create component of type \""
                                 << className << "\"" << std::endl;
        return 0;
    }

    // make sure this is an mtsComponent
    newComponent = dynamic_cast<mtsComponent *>(baseObject);
    if (!newComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamically: class \"" << className
                                 << "\" is not derived from mtsComponent" << std::endl;
        delete baseObject;
        return 0;
    }

    if (!isComponent) {
        CMN_LOG_CLASS_INIT_WARNING << "Class " << className << " is derived from mtsComponent, "
                                   << "but class services does not show inheritance from mtsComponent." << std::endl;
    }

    // rename the component
    newComponent->SetName(componentName);

    CMN_LOG_CLASS_INIT_VERBOSE << "CreateComponentDynamically: successfully created new component: "
                               << "\"" << newComponent->GetName() << "\" of type \""
                               << className << "\"" << std::endl;

    return newComponent;
}

mtsComponent * mtsManagerLocal::CreateComponentDynamically(const std::string & className,
                                                           const mtsGenericObject & constructorArg)
{
    std::stringstream buffer;
    cmnSerializer serializer(buffer);
    serializer.Serialize(constructorArg);
    return CreateComponentDynamically(className, "(serialized)", buffer.str());
}


#if CISST_HAS_JSON
bool mtsManagerLocal::ConfigureJSON(const std::string & filename)
{
    cmnPath configPath(cmnPath::GetWorkingDirectory());
    // make sure the file exists
    std::string fullname = configPath.Find(filename);
    if (fullname == "") {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureJSON: file \"" << filename
                                 << "\" not found in path: "<< std::endl
                                 << configPath << std::endl;
        return false;
    }
    // extract path of main json config file to search other files relative to it
    std::string configDir = fullname.substr(0, fullname.find_last_of('/'));
    configPath.Add(configDir, cmnPath::HEAD);

    // open json file
    std::ifstream jsonStream;
    jsonStream.open(filename.c_str());
    Json::Value jsonConfig;
    Json::Reader jsonReader;
    if (!jsonReader.parse(jsonStream, jsonConfig)) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureJSON: failed to parse configuration" << std::endl
                                 << "File: " << filename << std::endl
                                 << "Error(s):" << std::endl
                                 << jsonReader.getFormattedErrorMessages();
        return false;
    }

    if (jsonConfig.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureJSON: failed to configure component-manager, the file "
                                 << filename << " seems to be empty" << std::endl;
        return false;
    }

    return this->ConfigureJSON(jsonConfig, configPath);
}

bool mtsManagerLocal::ConfigureJSON(const std::list<std::string> & filenames)
{
    bool result = true;
    typedef std::list<std::string> listType;
    const listType::const_iterator endFile = filenames.end();
    for (listType::const_iterator iterFile = filenames.begin();
         iterFile != endFile;
         ++iterFile) {
        result = result && ConfigureJSON(*iterFile);
    }
    return result;
}

bool mtsManagerLocal::ConfigureJSON(const Json::Value & configuration, const cmnPath & configPath)
{
    const Json::Value components = configuration["components"];
    for (unsigned int index = 0;
         index < components.size();
         ++index) {
        if (!ConfigureComponentJSON(components[index], configPath)) {
            CMN_LOG_CLASS_INIT_ERROR << "ConfigureJSON: failed to configure component ["
                                     << index << "]" << std::endl;
            return false;
        }
    }
    const Json::Value connections = configuration["connections"];
    for (unsigned int index = 0;
         index < connections.size();
         ++index) {
        if (!ConfigureConnectionJSON(connections[index])) {
            CMN_LOG_CLASS_INIT_ERROR << "ConfigureJSON: failed to configure connection ["
                                     << index << "]" << std::endl;
            return false;
        }
    }
    return true;
}

bool mtsManagerLocal::ConfigureComponentJSON(const Json::Value & componentConfiguration, const cmnPath & configPath)
{
    std::string sharedLibrary, className, constructorArgJSON;
    Json::Value jsonValue;

    // shared library is optional
    jsonValue = componentConfiguration["shared-library"];
    if (!jsonValue.empty()){
        sharedLibrary = jsonValue.asString();
    } else {
        sharedLibrary = "";
    }
    // class name is required
    jsonValue = componentConfiguration["class-name"];
    if (!jsonValue.empty()) {
        className = jsonValue.asString();
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureComponentJSON: can't find \"class-name\"" << std::endl;
        return false;
    }
    // constructor argument is required
    jsonValue = componentConfiguration["constructor-arg"];
    if (!jsonValue.empty()) {
        Json::FastWriter fastWriter;
        constructorArgJSON = fastWriter.write(jsonValue);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureComponentJSON: can't find \"constructor-arg\"" << std::endl;
        return false;
    }
    // create (the method CreateComponentDynamicallyJSON should handle case w/o shared library
    mtsComponent * component
        = this->CreateComponentDynamicallyJSON(sharedLibrary,
                                               className,
                                               constructorArgJSON);
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureComponentJSON: failed to dynamically create component of type \""
                                 << className << "\"" << std::endl;
        return false;
    }
    // configure as needed
    Json::Value configureParameter = componentConfiguration["configure-parameter"];
    if (configureParameter.empty()) {
        component->Configure();
    } else {
        std::string configParam = configureParameter.asString();
        // see if we can find a file corresponding to string
        std::string configFile = configPath.Find(configParam);
        if (configFile == "") {
            // else pass the string as-is
            component->Configure(configParam);
        } else {
            component->Configure(configFile);
        }
    }
    // add if need, it is possible ctor or Configure already added the component itself to manager
    mtsComponent * existing = this->GetComponent(component->GetName());
    if (existing == component) {
        return true;
    }
    if (!this->AddComponent(component)) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureComponentJSON: failed to add component to component manager" << std::endl;
        return false;
    }
    return true;
}


bool mtsManagerLocal::ConfigureConnectionJSON(const Json::Value & connectionConfiguration)
{
    Json::Value provided, required, jsonValue;
    // required
    required = connectionConfiguration["required"];
    if (required.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: failed to find \"required\"" << std::endl;
        return false;
    }
    jsonValue = required["component"];
    if (jsonValue.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: failed to find \"required\", \"component\""
                                 << std::endl;
        return false;
    }
    const std::string requiredComponent = jsonValue.asString();
    if (requiredComponent.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: \"required\", \"component\" is not a valid string"
                                 << std::endl;
        return false;
    }
    jsonValue = required["interface"];
    if (jsonValue.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: failed to find \"required\", \"interface\""
                                 << std::endl;
        return false;
    }
    const std::string requiredInterface = jsonValue.asString();
    if (requiredInterface.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: \"required\", \"interface\" is not a valid string"
                                 << std::endl;
        return false;
    }
    // provided
    provided = connectionConfiguration["provided"];
    if (provided.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: failed to find \"provided\"" << std::endl;
        return false;
    }
    jsonValue = provided["component"];
    if (jsonValue.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: failed to find \"provided\", \"component\""
                                 << std::endl;
        return false;
    }
    const std::string providedComponent = jsonValue.asString();
    if (providedComponent.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: \"provided\", \"component\" is not a valid string"
                                 << std::endl;
        return false;
    }
    jsonValue = provided["interface"];
    if (jsonValue.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: failed to find \"provided\", \"interface\""
                                 << std::endl;
        return false;
    }
    const std::string providedInterface = jsonValue.asString();
    if (providedInterface.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: \"provided\", \"interface\" is not a valid string"
                                 << std::endl;
        return false;
    }
    // finally, request connection
    return this->Connect(requiredComponent, requiredInterface,
                         providedComponent, providedInterface);
}

mtsComponent * mtsManagerLocal::CreateComponentDynamicallyJSON(const std::string & sharedLibrary,
                                                               const std::string & className,
                                                               const std::string & constructorArgSerialized)
{
    // -1- try to dynamically load the library if specified
    if (!sharedLibrary.empty()) {
        // create load and path based on LD_LIBRARY_PATH (or PATH on Windows)
        osaDynamicLoader loader;
        std::string fullPath;
        // check if the file already exists, i.e. use provided a full path
        if (cmnPath::Exists(sharedLibrary)) {
            fullPath = sharedLibrary;
        } else {
            cmnPath path;
#if (CISST_OS == CISST_WINDOWS)
            path.AddFromEnvironment("PATH");
#else
            path.AddFromEnvironment("LD_LIBRARY_PATH");
#endif
            fullPath = path.Find(cmnPath::SharedLibrary(sharedLibrary));
            if (fullPath.empty())  {
                fullPath = sharedLibrary;
                CMN_LOG_CLASS_INIT_WARNING << "CreateComponentDynamicallyJSON: using path: "
                                           << path << ", couldn't find \""
                                           << cmnPath::SharedLibrary(sharedLibrary)
                                           << "\"" << std::endl;;
            } else {
                CMN_LOG_CLASS_INIT_VERBOSE << "CreateComponentDynamicallyJSON: using path: "
                                           << path << ", found full path name \""
                                           << fullPath << "\"" << std::endl;;
            }
        }
        if (!loader.Load(fullPath)) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamicallyJSON: failed to load shared library "
                                     << sharedLibrary << std::endl;
            return 0;
        }
    }

    // -2- try to dynamically create an instance of that class
    cmnClassServicesBase * componentClassServices = cmnClassRegister::FindClassServices(className);
    if (!componentClassServices) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamicallyJSON: unable to find class services for type "
                                 << className << std::endl;
        return 0;
    }

    // -3- check if we need to also create an argument for the constructor
    if (!componentClassServices->OneArgConstructorAvailable()) {
        return 0;
    }

    const cmnClassServicesBase * argumentClassServices = componentClassServices->GetConstructorArgServices();
    CMN_ASSERT(argumentClassServices); // this should not fail
    cmnGenericObject * argument = argumentClassServices->Create();
    if (!argument) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamicallyJSON: unable to create a constructor argument for "
                                 << className
                                 << ".  Make sure the macro CMN_IMPLEMENT_SERVICE_xx is correct for this class."
                                 << std::endl;
        return 0;
    }
    // then deserialize from JSON value...
    Json::Value jsonValue;
    Json::Reader reader;
    // parsing should work since the string has been generated after a previous parse
#if CMN_ASSERT_IS_DEFINED
    bool parsedOk =
#endif
        reader.parse(constructorArgSerialized, jsonValue);
    CMN_ASSERT(parsedOk);
    try {
        argument->DeSerializeTextJSON(jsonValue);
    } catch (std::runtime_error & e) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamicallyJSON: unable to deserialize constructor for "
                                 << className << " from JSON file, got exception: "
                                 << e.what() << std::endl;
        delete argument;
        return 0;
    }
    // now, finally, construct the component!
    cmnGenericObject * componentBase
        = componentClassServices->CreateWithArg(*argument);
    if (!componentBase) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamicallyJSON: failed to create component of type "
                                 << className << std::endl;
        delete argument;
        return 0;
    }
    // cleanup argument
    delete argument;

    // make sure this is a component
    mtsComponent * component = dynamic_cast<mtsComponent *>(componentBase);
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamicallyJSON: failed to cast newly created object of type "
                                 << className << " to mtsComponent" << std::endl;
        delete componentBase;
        return 0;
    }

    // looks like it worked!
    return component;
}
#endif

bool mtsManagerLocal::AddComponent(mtsComponent * component)
{
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "AddComponent: invalid component" << std::endl;
        return false;
    }
    return GetManagerServices()->ComponentAdd(component);
}

bool CISST_DEPRECATED mtsManagerLocal::AddTask(mtsTask * component)
{
    return AddComponent(component);
}

bool CISST_DEPRECATED mtsManagerLocal::AddDevice(mtsComponent * component)
{
    return AddComponent(component);
}

bool mtsManagerLocal::RemoveComponent(mtsComponent * component)
{
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveComponent: invalid component" << std::endl;
        return false;
    }
    return GetManagerServices()->ComponentRemove(component);
}

bool mtsManagerLocal::RemoveComponent(const std::string & componentName)
{
    return GetManagerServices()->ComponentRemove(componentName);
}

size_t mtsManagerLocal::RemoveAllUserComponents(void)
{
    std::vector<std::string> componentNames;
    GetNamesOfComponents(componentNames);
    size_t numRemoved = 0;
    for (size_t i = 0; i < componentNames.size(); i++) {
        // Do not need to check if this is the manager component, because RemoveComponent will not remove it.
        if (!RemoveComponent(componentNames[i])) {
            CMN_LOG_CLASS_RUN_WARNING << "RemoveAllUserComponents: failed to remove "
                                      << componentNames[i] << std::endl;
        }
        else {
            numRemoved++;
        }
    }
    if (numRemoved > 0) {
        CMN_LOG_CLASS_RUN_VERBOSE << "RemoveAllUserComponents: removed " << numRemoved << " user components, leaving "
                                  << componentNames.size() << " system components" << std::endl;
    }
    return numRemoved;
}

mtsComponent * mtsManagerLocal::GetComponent(const std::string & componentName) const
{
    mtsManagerComponentServices *services = GetManagerServices();
    return services->ComponentGet(componentName);
}

mtsTask * mtsManagerLocal::GetComponentAsTask(const std::string & componentName) const
{
    mtsTask * componentTask = 0;

    mtsComponent * component = GetComponent(componentName);
    if (component) {
        componentTask = dynamic_cast<mtsTask*>(component);
    }

    return componentTask;
}

mtsComponent CISST_DEPRECATED * mtsManagerLocal::GetDevice(const std::string & deviceName)
{
    return GetComponent(deviceName);
}

mtsTask CISST_DEPRECATED * mtsManagerLocal::GetTask(const std::string & taskName)
{
    return GetComponentAsTask(taskName);
}

bool mtsManagerLocal::FindComponent(const std::string & componentName) const
{
    return (GetComponent(componentName) != 0);
}

bool mtsManagerLocal::WaitForStateAll(mtsComponentState desiredState, double timeout) const
{
    mtsManagerComponentServices *services = GetManagerServices();

    // wait for all components to be started if timeout is positive
    bool allAtState = true;
    if (timeout > 0.0) {
        // will iterate on all components
        std::vector<std::string> componentNames = GetNamesOfComponents();
        std::vector<std::string>::const_iterator iterator = componentNames.begin();
        const std::vector<std::string>::const_iterator end = componentNames.end();
        double timeStartedAll = TimeServer.GetRelativeTime();
        double timeEnd = timeStartedAll + timeout;
        bool timedOut = false;
        for (; (iterator != end) && allAtState && !timedOut; ++iterator) {
            if (mtsManagerComponentBase::IsManagerComponent(*iterator))
                continue;
            mtsComponent *component = services->ComponentGet(*iterator);
            // compute how much time do we have left based on when we started
            double timeLeft = timeEnd - TimeServer.GetRelativeTime();
            // skip in 2 cases, manager components and tasks with ExecIn
            mtsManagerComponentBase * isManager = dynamic_cast<mtsManagerComponentBase *>(component);
            bool isIndependent = true;
            mtsTask * task = dynamic_cast<mtsTask *>(component);
            if (task && task->ExecIn && task->ExecIn->GetConnectedInterface()) {
                isIndependent = false;
            }
            // wait if needed
            if (!isManager && isIndependent) {
                allAtState = component->WaitForState(desiredState, timeLeft);
                if (!allAtState) {
                    CMN_LOG_CLASS_INIT_ERROR << "WaitForStateAll: component \"" << (*iterator) << "\" failed to reach state \""
                                             << desiredState << "\"" << std::endl;
                }
            }
            if (TimeServer.GetRelativeTime() > timeEnd) {
                // looks like we don't have any time left to start the remaining components.
                timedOut = true;
                allAtState = false;
                CMN_LOG_CLASS_INIT_ERROR << "WaitForStateAll: timed out while waiting for state \""
                                         << desiredState << "\"" << std::endl;
            }
        }
        // report results
        if (allAtState && !timedOut) {
            CMN_LOG_CLASS_INIT_VERBOSE << "WaitForStateAll: all components reached state \""
                                       << desiredState << "\" in " << (TimeServer.GetRelativeTime() - timeStartedAll) << " seconds" << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "WaitForStateAll: failed to reached state \""
                                     << desiredState << "\" for all components" << std::endl;
        }
    } else {
        CMN_LOG_CLASS_INIT_VERBOSE << "WaitForStateAll: called with null timeout (not blocking)" << std::endl;
    }
    return allAtState;
}


void mtsManagerLocal::CreateAll(void)
{
    std::vector<std::string> componentNames = GetNamesOfComponents();
    std::vector<std::string>::const_iterator iterator = componentNames.begin();
    const std::vector<std::string>::const_iterator end = componentNames.end();

    mtsManagerComponentServices *services = GetManagerServices();
    for (; iterator != end; ++iterator) {
        if (mtsManagerComponentBase::IsManagerComponent(*iterator))
            continue;
        // Could instead define a new ComponentCreate method
        mtsComponent *component = services->ComponentGet(*iterator);
        if (component) component->Create();
    }
}


bool mtsManagerLocal::CreateAllAndWait(double timeoutInSeconds)
{
    this->CreateAll();
    return this->WaitForStateAll(mtsComponentState::READY, timeoutInSeconds);
}


void mtsManagerLocal::StartAll(void)
{
    // Get the current thread id in order to check if any task will use the current thread.
    // If so, start that task last.
    const osaThreadId threadId = osaGetCurrentThreadId();
    if (threadId != this->MainThreadId) {
        CMN_LOG_CLASS_RUN_WARNING << "StartAll: current thread is not main thread." << std::endl;
    }

    mtsTask * lastTask = 0;   // Last task to be started (if non-zero)

    std::vector<std::string> componentNames = GetNamesOfComponents();
    std::vector<std::string>::const_iterator iterator = componentNames.begin();
    const std::vector<std::string>::const_iterator end = componentNames.end();

    mtsManagerComponentServices *services = GetManagerServices();

    for (; iterator != end; ++iterator) {
        if (mtsManagerComponentBase::IsManagerComponent(*iterator))
            continue;
        mtsComponent *component = services->ComponentGet(*iterator);
        // look for component
        mtsTask *componentTask = dynamic_cast<mtsTask*>(component);
        if (componentTask) {
            // Check if the task will use the current thread.
            if (componentTask->Thread.GetId() == threadId) {
                if (dynamic_cast<mtsTaskFromCallback*>(componentTask)) {
                    CMN_LOG_CLASS_INIT_VERBOSE << "StartAll: component \"" << (*iterator)
                                               << "\" uses current thread, but is a callback task;"
                                               << " expect that it will be called by dispatcher." << std::endl;
                    componentTask->Start();
                }
                else {
                    CMN_LOG_CLASS_INIT_WARNING << "StartAll: component \"" << (*iterator)
                                               << "\" uses current thread, will be started last." << std::endl;
                    if (lastTask) {
                        CMN_LOG_CLASS_INIT_ERROR << "StartAll: found another task using current thread (\""
                                                 << (*iterator) << "\"), only first will be started (\""
                                                 << lastTask->GetName() << "\")." << std::endl;
                        // PK: I don't think this task should be started if it uses the current thread
                        componentTask->Start();
                    } else {
                        // set pointer to last task to be started
                        lastTask = componentTask;
                    }
                }
            } else {
                CMN_LOG_CLASS_INIT_DEBUG << "StartAll: starting task \"" << (*iterator) << "\"" << std::endl;
                if (componentTask->Thread.GetId() == MainThreadId) {
                    if (dynamic_cast<mtsTaskContinuous *>(componentTask)) {
                        CMN_LOG_CLASS_INIT_WARNING << "StartAll: is the main task really " << (*iterator) << "???" << std::endl;
                    }
                }
                componentTask->Start();  // If task will not use current thread, start it immediately.
            }
        } else {
            CMN_LOG_CLASS_INIT_DEBUG << "StartAll: starting component \"" << (*iterator) << "\"" << std::endl;
            component->Start();  // this is a component, it doesn't have a thread
        }
    }

    if (lastTask) {
        lastTask->Start();
    }
}


bool mtsManagerLocal::StartAllAndWait(double timeoutInSeconds)
{
    this->StartAll();
    return this->WaitForStateAll(mtsComponentState::ACTIVE, timeoutInSeconds);
}


void mtsManagerLocal::KillAll(void)
{
    std::vector<std::string> componentNames = GetNamesOfComponents();
    std::vector<std::string>::const_iterator iterator = componentNames.begin();
    const std::vector<std::string>::const_iterator end = componentNames.end();

    mtsManagerComponentServices *services = GetManagerServices();

    for (; iterator != end; ++iterator) {
        if (mtsManagerComponentBase::IsManagerComponent(*iterator))
            continue;
        mtsComponent *component = services->ComponentGet(*iterator);
        if (component) {
            component->Kill();
        }
        else {
            CMN_LOG_CLASS_RUN_DEBUG << "KillAll: null component \""
                                    << (*iterator) << "\"" << std::endl;
        }
    }

    // Block further logs
    LogDisabled = true;
    SetLogForwarding(false);
}


bool mtsManagerLocal::KillAllAndWait(double timeoutInSeconds)
{
    this->KillAll();
    return this->WaitForStateAll(mtsComponentState::FINISHED, timeoutInSeconds);
}


bool mtsManagerLocal::Connect(const std::string & clientComponentName, const std::string & clientInterfaceName,
                              const std::string & serverComponentName, const std::string & serverInterfaceName)
{
    return GetManagerServices()->Connect(clientComponentName, clientInterfaceName,
                                         serverComponentName, serverInterfaceName);
}

bool mtsManagerLocal::Connect(const std::string & clientProcessName,
                              const std::string & clientComponentName, const std::string & clientInterfaceName,
                              const std::string & serverProcessName,
                              const std::string & serverComponentName, const std::string & serverInterfaceName)
{
    return GetManagerServices()->Connect(clientProcessName, clientComponentName, clientInterfaceName,
                                         serverProcessName, serverComponentName, serverInterfaceName);
}

bool mtsManagerLocal::Disconnect(const ConnectionIDType connectionID)
{
    return GetManagerServices()->Disconnect(connectionID);
}

bool mtsManagerLocal::Disconnect(const std::string & clientComponentName, const std::string & clientInterfaceName,
                                 const std::string & serverComponentName, const std::string & serverInterfaceName)
{
    return GetManagerServices()->Disconnect(clientComponentName, clientInterfaceName,
                                            serverComponentName, serverInterfaceName);
}

bool mtsManagerLocal::Disconnect(
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceName)
{
    return GetManagerServices()->Disconnect(clientProcessName, clientComponentName, clientInterfaceName,
                                            serverProcessName, serverComponentName, serverInterfaceName);
}

void mtsManagerLocal::GetNamesOfProcesses(std::vector<std::string> & namesOfProcesses) const
{
    namesOfProcesses = GetManagerServices()->GetNamesOfProcesses();
}

std::vector<std::string> mtsManagerLocal::GetNamesOfComponents(void) const
{
    return GetManagerServices()->GetNamesOfComponents();
}

void mtsManagerLocal::GetNamesOfComponents(std::vector<std::string> & namesOfComponents) const
{
    namesOfComponents = GetManagerServices()->GetNamesOfComponents();
}

void mtsManagerLocal::GetNamesOfComponents(const std::string & processName, std::vector<std::string> & namesOfComponents) const
{
    namesOfComponents = GetManagerServices()->GetNamesOfComponents(processName);
}

std::vector<mtsDescriptionComponent> mtsManagerLocal::GetDescriptionsOfComponents(const std::string & processName) const
{
    return GetManagerServices()->GetDescriptionsOfComponents(processName);
}

bool mtsManagerLocal::GetNamesOfInterfaces(const std::string & processName,
                                           const std::string & componentName,
                                           std::vector<std::string> & namesOfInterfacesRequired,
                                           std::vector<std::string> & namesOfInterfacesProvided) const
{
    return GetManagerServices()->GetNamesOfInterfaces(processName, componentName,
                                                      namesOfInterfacesRequired, namesOfInterfacesProvided);
}

bool mtsManagerLocal::GetDescriptionsOfInterfaces(const std::string & processName,
                                                  const std::string & componentName,
                                                  std::vector<mtsDescriptionInterfaceFullName> & descriptionsRequired,
                                                  std::vector<mtsDescriptionInterfaceFullName> & descriptionsProvided) const
{
    return GetManagerServices()->GetDescriptionsOfInterfaces(processName, componentName,
                                                             descriptionsRequired, descriptionsProvided);
}

std::vector<mtsDescriptionConnection> mtsManagerLocal::GetListOfConnections(void) const
{
    return GetManagerServices()->GetListOfConnections();
}

std::vector<mtsDescriptionComponentClass> mtsManagerLocal::GetListOfComponentClasses(const std::string & processName) const
{
    return GetManagerServices()->GetListOfComponentClasses();
}

mtsInterfaceProvidedDescription mtsManagerLocal::GetInterfaceProvidedDescription(const std::string & processName,
                                const std::string & componentName, const std::string & interfaceName) const
{
    return GetManagerServices()->GetInterfaceProvidedDescription(processName, componentName, interfaceName);
}

mtsInterfaceRequiredDescription mtsManagerLocal::GetInterfaceRequiredDescription(const std::string & processName,
                                const std::string & componentName, const std::string & interfaceName) const
{
    return GetManagerServices()->GetInterfaceRequiredDescription(processName, componentName, interfaceName);
}

const osaTimeServer & mtsManagerLocal::GetTimeServer(void) const
{
    return TimeServer;
}

void mtsManagerLocal::PushCurrentMainTask(mtsTaskContinuous *cur)
{
    if (!cur) {
        CMN_LOG_CLASS_RUN_ERROR << "PushCurrentMainTask: null parameter" << std::endl;
        return;
    }
    if (cur == CurrentMainTask) {
        CMN_LOG_CLASS_RUN_WARNING << "PushCurrentMainTask: duplicate call to push " << cur->GetName() << std::endl;
        return;
    }
    if (CurrentMainTask)
         CMN_LOG_CLASS_RUN_WARNING << "CurrentMainTask changing from " << CurrentMainTask->GetName()
                                      << " to " << cur->GetName() << std::endl;
    else
         CMN_LOG_CLASS_RUN_VERBOSE << "Setting CurrentMainTask to " << cur->GetName() << std::endl;
    CurrentMainTask = cur;
    MainTaskNames.push(CurrentMainTask->GetName());
}

mtsTaskContinuous *mtsManagerLocal::PopCurrentMainTask(void)
{
    mtsTaskContinuous *previousMainTask = 0;
    while (!previousMainTask && !MainTaskNames.empty()) {
        previousMainTask = dynamic_cast<mtsTaskContinuous *>(GetComponent(MainTaskNames.top()));
        if (!previousMainTask) {
            CMN_LOG_CLASS_RUN_WARNING << "PopCurrentMainTask: could not find " << MainTaskNames.top() << std::endl;
        }
        MainTaskNames.pop();
    }
    if (previousMainTask) {
        CMN_LOG_CLASS_RUN_VERBOSE << CurrentMainTask->GetName() << " is exiting, so main task reverts to "
                                  << previousMainTask->GetName() << std::endl;
    }
    else {
        CMN_LOG_CLASS_RUN_VERBOSE << CurrentMainTask->GetName() << " is exiting, no main task remaining" << std::endl;
    }
    CurrentMainTask = previousMainTask;
    return CurrentMainTask;
}

void mtsManagerLocal::GetNamesOfCommands(std::vector<std::string>& namesOfCommands,
                                         const std::string & componentName,
                                         const std::string & interfaceName)
{
    mtsInterfaceProvidedDescription desc;
    GetInterfaceProvidedDescription(componentName, interfaceName, desc);

    std::string name;
    for (size_t i = 0; i < desc.CommandsVoid.size(); ++i) {
        name = "V) ";
        name += desc.CommandsVoid[i].Name;
        namesOfCommands.push_back(name);
    }
    for (size_t i = 0; i < desc.CommandsWrite.size(); ++i) {
        name = "W) ";
        name += desc.CommandsWrite[i].Name;
        namesOfCommands.push_back(name);
    }
    for (size_t i = 0; i < desc.CommandsRead.size(); ++i) {
        name = "R) ";
        name += desc.CommandsRead[i].Name;
        namesOfCommands.push_back(name);
    }
    for (size_t i = 0; i < desc.CommandsQualifiedRead.size(); ++i) {
        name = "Q) ";
        name += desc.CommandsQualifiedRead[i].Name;
        namesOfCommands.push_back(name);
    }
    for (size_t i = 0; i < desc.CommandsVoidReturn.size(); ++i) {
        name = "v) ";
        name += desc.CommandsVoidReturn[i].Name;
        namesOfCommands.push_back(name);
    }
    for (size_t i = 0; i < desc.CommandsWriteReturn.size(); ++i) {
        name = "w) ";
        name += desc.CommandsWriteReturn[i].Name;
        namesOfCommands.push_back(name);
    }
}

void mtsManagerLocal::GetNamesOfEventGenerators(std::vector<std::string> & namesOfEventGenerators,
                                                const std::string & componentName,
                                                const std::string & interfaceName)
{
    mtsInterfaceProvidedDescription desc;
    GetInterfaceProvidedDescription(componentName, interfaceName, desc);

    std::string name;
    for (size_t i = 0; i < desc.EventsVoid.size(); ++i) {
        name = "V) ";
        name += desc.EventsVoid[i].Name;
        namesOfEventGenerators.push_back(name);
    }
    for (size_t i = 0; i < desc.EventsWrite.size(); ++i) {
        name = "W) ";
        name += desc.EventsWrite[i].Name;
        namesOfEventGenerators.push_back(name);
    }
}

void mtsManagerLocal::GetNamesOfFunctions(std::vector<std::string> & namesOfFunctions,
                                          const std::string & componentName,
                                          const std::string & requiredInterfaceName)
{
    mtsInterfaceRequiredDescription desc;
    GetInterfaceRequiredDescription(componentName, requiredInterfaceName, desc);

    std::string name;
    for (size_t i = 0; i < desc.FunctionVoidNames.size(); ++i) {
        name = "V) ";
        name += desc.FunctionVoidNames[i];
        namesOfFunctions.push_back(name);
    }
    for (size_t i = 0; i < desc.FunctionWriteNames.size(); ++i) {
        name = "W) ";
        name += desc.FunctionWriteNames[i];
        namesOfFunctions.push_back(name);
    }
    for (size_t i = 0; i < desc.FunctionReadNames.size(); ++i) {
        name = "R) ";
        name += desc.FunctionReadNames[i];
        namesOfFunctions.push_back(name);
    }
    for (size_t i = 0; i < desc.FunctionQualifiedReadNames.size(); ++i) {
        name = "Q) ";
        name += desc.FunctionQualifiedReadNames[i];
        namesOfFunctions.push_back(name);
    }
    for (size_t i = 0; i < desc.FunctionVoidReturnNames.size(); ++i) {
        name = "v) ";
        name += desc.FunctionVoidReturnNames[i];
        namesOfFunctions.push_back(name);
    }
    for (size_t i = 0; i < desc.FunctionWriteReturnNames.size(); ++i) {
        name = "w) ";
        name += desc.FunctionWriteReturnNames[i];
        namesOfFunctions.push_back(name);
    }
}

void mtsManagerLocal::GetNamesOfEventHandlers(std::vector<std::string> & namesOfEventHandlers,
                                              const std::string & componentName,
                                              const std::string & requiredInterfaceName)
{
    mtsInterfaceRequiredDescription desc;
    GetInterfaceRequiredDescription(componentName, requiredInterfaceName, desc);

    std::string name;
    for (size_t i = 0; i < desc.EventHandlersVoid.size(); ++i) {
        name = "V) ";
        name += desc.EventHandlersVoid[i].Name;
        namesOfEventHandlers.push_back(name);
    }
    for (size_t i = 0; i < desc.EventHandlersWrite.size(); ++i) {
        name = "W) ";
        name += desc.EventHandlersWrite[i].Name;
        namesOfEventHandlers.push_back(name);
    }
}

void mtsManagerLocal::GetDescriptionOfCommand(std::string & description,
                                              const std::string & componentName,
                                              const std::string & interfaceName,
                                              const std::string & commandName)
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) return;

    mtsInterfaceProvided * interfaceProvided = component->GetInterfaceProvided(interfaceName);
    if (!interfaceProvided) return;

    // Get command type
    char commandType = *commandName.c_str();
    std::string actualCommandName = commandName.substr(3, commandName.size() - 2);

    description = "Argument type: ";
    switch (commandType) {
        case 'V':
            {
                mtsCommandVoid * command = interfaceProvided->GetCommandVoid(actualCommandName);
                if (!command) {
                    description = "No void command found for ";
                    description += actualCommandName;
                    return;
                }
                description += "(none)";
            }
            break;
        case 'W':
            {
                mtsCommandWriteBase * command = interfaceProvided->GetCommandWrite(actualCommandName);
                if (!command) {
                    description = "No write command found for ";
                    description += actualCommandName;
                    return;
                }
                description += command->GetArgumentClassServices()->GetName();
            }
            break;
        case 'R':
            {
                mtsCommandRead * command = interfaceProvided->GetCommandRead(actualCommandName);
                if (!command) {
                    description = "No read command found for ";
                    description += actualCommandName;
                    return;
                }
                description += command->GetArgumentPrototype()->Services()->GetName();
            }
            break;
        case 'Q':
            {
                mtsCommandQualifiedRead * command = interfaceProvided->GetCommandQualifiedRead(actualCommandName);
                if (!command) {
                    description = "No qualified read command found for ";
                    description += actualCommandName;
                    return;
                }
                description = "Argument1 type: ";
                description += command->GetArgument1Prototype()->Services()->GetName();
                description += "\nArgument2 type: ";
                description += command->GetArgument2Prototype()->Services()->GetName();
            }
            break;
        default:
            CMN_LOG_CLASS_INIT_ERROR << "GetDescriptionOfCommand: type of command not handled for command \"" << commandName << "\"" << std::endl;
            description = "Failed to get command description";
            return;
    }
}

void mtsManagerLocal::GetDescriptionOfEventGenerator(std::string & description,
                                                     const std::string & componentName,
                                                     const std::string & interfaceName,
                                                     const std::string & eventGeneratorName)
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) return;

    mtsInterfaceProvided * interfaceProvided = component->GetInterfaceProvided(interfaceName);
    if (!interfaceProvided) return;

    // Get event generator type
    char eventGeneratorType = *eventGeneratorName.c_str();
    std::string actualEventGeneratorName = eventGeneratorName.substr(3, eventGeneratorName.size() - 2);

    description = "Argument type: ";
    switch (eventGeneratorType) {
        case 'V':
            {
                mtsCommandVoid * eventGenerator = interfaceProvided->EventVoidGenerators.GetItem(actualEventGeneratorName, CMN_LOG_LEVEL_NONE);
                if (!eventGenerator) {
                    description = "No void event generator found";
                    return;
                }
                description += "(none)";
            }
            break;
        case 'W':
            {
                mtsCommandWriteBase * eventGenerator = interfaceProvided->EventWriteGenerators.GetItem(actualEventGeneratorName, CMN_LOG_LEVEL_NONE);
                if (!eventGenerator) {
                    description = "No write event generator found";
                    return;
                }
                description += eventGenerator->GetArgumentClassServices()->GetName();
            }
            break;
        default:
            description = "Failed to get event generator description";
            return;
    }
}

void mtsManagerLocal::GetDescriptionOfFunction(std::string & description,
                                               const std::string & componentName,
                                               const std::string & requiredInterfaceName,
                                               const std::string & functionName)
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) return;

    mtsInterfaceRequired * requiredInterface = component->GetInterfaceRequired(requiredInterfaceName);
    if (!requiredInterface) return;

    // Get function type
    //char functionType = *functionName.c_str();
    std::string actualFunctionName = functionName.substr(3, functionName.size() - 2);

    description = "Resource argument type: ";
#if 0 // adeguet1 todo fix --- this is using internal values of the interface, this should be done otherwise
    switch (functionType) {
        case 'V':
            {
                mtsInterfaceRequired::CommandInfo<mtsCommandVoidBase> * function = requiredInterface->CommandPointersVoid.GetItem(actualFunctionName, CMN_LOG_LEVEL_NONE);
                if (!function) {
                    description = "No void function found";
                    return;
                }
                description += "(none)";
            }
            break;
        case 'W':
            {
                mtsInterfaceRequired::CommandInfo<mtsCommandWriteBase> * function = requiredInterface->CommandPointersWrite.GetItem(actualFunctionName, CMN_LOG_LEVEL_NONE);
                if (!function) {
                    description = "No write function found";
                    return;
                }
                if (*function->CommandPointer) {
                    description += (*function->CommandPointer)->GetArgumentClassServices()->GetName();
                } else {
                    description += "(unbounded write function)";
                }
            }
            break;
        case 'R':
            {
                mtsInterfaceRequired::CommandInfo<mtsCommandReadBase> * function = requiredInterface->CommandPointersRead.GetItem(actualFunctionName, CMN_LOG_LEVEL_NONE);
                if (!function) {
                    description = "No read function found";
                    return;
                }
                if (*function->CommandPointer) {
                    description += (*function->CommandPointer)->GetArgumentClassServices()->GetName();
                } else {
                    description += "(unbounded read function)";
                }
            }
            break;
        case 'Q':
            {
                mtsInterfaceRequired::CommandInfo<mtsCommandQualifiedReadBase> * function = requiredInterface->CommandPointersQualifiedRead.GetItem(actualFunctionName, CMN_LOG_LEVEL_NONE);
                if (!function) {
                    description = "No qualified read function found";
                    return;
                }
                if (*function->CommandPointer) {
                    description = "Resource argument1 type: ";
                    description += (*function->CommandPointer)->GetArgument1ClassServices()->GetName();
                    description += "\nResource argument2 type: ";
                    description += (*function->CommandPointer)->GetArgument2ClassServices()->GetName();
                } else {
                    description = "Resource argument1 type: ";
                    description += "(unbounded qualified read function)";
                    description += "\nResource argument2 type: ";
                    description += "(unbounded qualified read function)";
                }

            }
            break;
        default:
            CMN_LOG_CLASS_INIT_ERROR << "GetDescriptionOfFunction: type of function not handled for command \"" << functionName << "\"" << std::endl;
            description = "Failed to get function description";
            return;
    }
#endif
}

void mtsManagerLocal::GetDescriptionOfEventHandler(std::string & description,
                                                   const std::string & componentName,
                                                   const std::string & requiredInterfaceName,
                                                   const std::string & eventHandlerName)
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) return;

    mtsInterfaceRequired * requiredInterface = component->GetInterfaceRequired(requiredInterfaceName);
    if (!requiredInterface) return;

    // Get event handler type
    char eventHandlerType = *eventHandlerName.c_str();
    std::string actualEventHandlerName = eventHandlerName.substr(3, eventHandlerName.size() - 2);

    description = "Argument type: ";
    switch (eventHandlerType) {
        case 'V':
            {
                mtsCommandVoid * command = requiredInterface->EventHandlersVoid.GetItem(actualEventHandlerName, CMN_LOG_LEVEL_NONE);
                if (!command) {
                    description = "No void event handler found";
                    return;
                }
                description += "(none)";
            }
            break;
        case 'W':
            {
                mtsCommandWriteBase * command = requiredInterface->EventHandlersWrite.GetItem(actualEventHandlerName, CMN_LOG_LEVEL_NONE);
                if (!command) {
                    description = "No write event handler found";
                    return;
                }
                description += command->GetArgumentClassServices()->GetName();
            }
            break;
        default:
            description = "Failed to get event handler description";
            return;
    }
}

std::vector<std::string> mtsManagerLocal::GetIPAddressList(void)
{
    std::vector<std::string> ipAddresses;
    GetIPAddressList(ipAddresses);
    return ipAddresses;
}

void mtsManagerLocal::GetIPAddressList(std::vector<std::string> & ipAddresses)
{
    osaSocket::GetLocalhostIP(ipAddresses);
}

bool mtsManagerLocal::IsLogAllowed(void) {
    return !LogDisabled;
}

bool mtsManagerLocal::IsLogForwardingEnabled(void) {
    return LogForwardEnabled;
}

void mtsManagerLocal::SetLogForwarding(bool activate) {
    LogForwardEnabled = activate;
}

void mtsManagerLocal::GetLogForwardingState(bool & state) {
    state = IsLogForwardingEnabled();
}

bool mtsManagerLocal::GetLogForwardingState(void) {
    return IsLogForwardingEnabled();
}

void mtsManagerLocal::LogDispatcher(const char * str, int len)
{
    if (!LogForwardEnabled) return;

    bool deadlockAvoidance = false;
    if (Instance) {
        if (LogMutex.IsLocker()) {
            deadlockAvoidance = true;
        }
    }

    if (!deadlockAvoidance) {
        LogMutex.Lock();
    }

    if (len == 1 && str[0] == '\n') {
        if (!deadlockAvoidance) {
            LogMutex.Unlock();
        }
        return;
    }

    // MJ TODO: Deal with cases that len > 1000
    mtsLogMessage log(str, len);
    // Timestamping (as early as possible)
    if (TimeServerOriginSet) {
        log.SetTimestamp(TimeServer.GetAbsoluteTimeInSeconds());
        log.SetValid(true);
    } else {
        log.SetTimestamp(0);
        log.SetValid(false);
    }
    log.ProcessName = "";

    if (!deadlockAvoidance) {
        // Queue log message and return immediately
        LogQueue.push_back(log);
    } else {
        // If current thread locked this mutex earlier, forward the log immediately
        // to avoid deadlock.  Note that all validity checks are already done
        // in the log dispatch thread in this case.
        Instance->GetLoggerServices()->PrintLog(log);
    }

    if (!deadlockAvoidance) {
        LogMutex.Unlock();
    }
}

void * mtsManagerLocal::LogDispatchThread(void * CMN_UNUSED(arg))
{
    int count = 0;

    while (!LogThreadFinishWaiting) {
        if (LogQueue.size() == 0) {
            osaSleep(1.0 * cmn_ms);
            continue;
        }

        // Wait for MCC to be ready (activated and connected) before starting log fowarding
        if (!ManagerComponent || !ManagerComponent->IsRunning()) {
            osaSleep(100.0 * cmn_ms);
            continue;
        }

        LogMutex.Lock();
        count = 0;
        for (LogQueueType::iterator it = LogQueue.begin();
             it != LogQueue.end();
             ++count)
        {
            if (Instance->GetLoggerServices()->PrintLog(*it)) {
                ++it;
                LogQueue.pop_front(); // FIFO
            }
            // MJ: after 30 log messages forwarded, give other threads a chance to queue
            // logs by releasing the lock (30 is arbitrary)
            if (count == 30)
                break;
        }
        LogMutex.Unlock();
    }

    LogThreadFinished.Raise();

    return 0;
}

bool mtsManagerLocal::IsValidComponentTag(const std::string & tag) const {
    return (ValidComponentTags.find(tag) != ValidComponentTags.end());
}

bool mtsManagerLocal::IsValidInterfaceTag(const std::string & tag) const {
    return (ValidInterfaceTags.find(tag) != ValidInterfaceTags.end());
}

void mtsManagerLocal::AddValidComponentTag(const std::string & tag) {
    ValidComponentTags.insert(tag);
}

void mtsManagerLocal::AddValidInterfaceTag(const std::string & tag) {
    ValidInterfaceTags.insert(tag);
}

const std::set<std::string> & mtsManagerLocal::GetValidComponentTags(void) const {
    return ValidComponentTags;
}

const std::set<std::string> & mtsManagerLocal::GetValidInterfaceTags(void) const {
    return ValidInterfaceTags;
}
