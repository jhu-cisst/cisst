/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/*
  $Id: SineTaskMain.cpp 

  Author(s): Tian Xia 
  Created on: 2010-03-01 

PyQt with CisstMultiTask Network example 
Includes: 
IPython 
PyQt running in same thread
CisstMultiTask Network with ICE 
*/ 

#include <cisstConfig.h>

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstCommon.h> 
#include <cisstOSAbstraction.h> 
#include <cisstInteractive.h> 
#include <cisstMultiTask.h>

#include <Python.h>

#include "sineTask.h" 

int main(int argc, char * argv[]) 
{
    if(argc < 2) {
        std::cerr << "Usage: " << argv[0] << " (global component manager IP)" << std::endl;
        return 1;
    }
    
    // Set global component manager IP
    const std::string globalComponentManagerIP(argv[1]);

    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_RUN_ERROR);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);

    // add a log per thread
    osaThreadedLogFile threadedLog("PyQtEmbeddedSineTaskMain-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);

    // set the log level of detail on select tasks
    cmnClassRegister::SetLoD("mtsTaskInterface", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsTaskManager", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("sineTask", CMN_LOG_LOD_VERY_VERBOSE);

    // Get the TaskManager instance and set operation mode
    // Get the local component manager
    std::cout << "CREATING LOCAL COMPONENT MANAGER" << std::endl;
    mtsManagerLocal * localManager;
    try {
        std::cout<<"Global Component Manager IP "<<globalComponentManagerIP<<std::endl; 
        localManager = mtsManagerLocal::GetInstance(globalComponentManagerIP, "SIN");
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }

    // Create task 
    const double PeriodSine = 1 * cmn_ms; // in milliseconds
    sineTask * sineTaskObject = new sineTask("SIN", PeriodSine);
    
    localManager->AddComponent(sineTaskObject);

    if(!localManager->Connect("SIN", "SIN", "Required", 
                              "DISP", "DISP", "Provided")) {
        CMN_LOG_INIT_ERROR << "Connect failed" << std::endl; 
        return 1; 
    }

    // mtsTaskManager needs some time 
    osaSleep(0.5 * cmn_s);

    // create and start all tasks
    localManager->CreateAll();
    localManager->StartAll();
    
    cmnObjectRegister::Register("LocalManager", localManager); 

    //--------------------------------------------------------------------------------------------
    Py_Initialize();
    PyRun_SimpleString("import sys; sys.path.append(\".\");");
    std::string command = std::string("sys.path.append(\"") + CISST_BUILD_LINK_DIRECTORIES + "\");";
    std::cout << "*** Path for cisst libraries: " << CISST_BUILD_LINK_DIRECTORIES << std::endl;
    PyRun_SimpleString(const_cast<char*>(command.c_str()));
        
    std::cout << "*** Load a Python script and execute it" << std::endl;
    bool FileFound = true;
    FILE *fp = NULL;
    cmnPath path;
    path.Add("/Users/Tian/Work/Devel/cisstversions/cisstmain_network/examples/interactiveTutorial/pyQtEmbedded");
    std::string fullName = path.Find("qtPlot.py");
    fp = fopen(fullName.c_str(), "r");
    if (fp == NULL) {
        std::cout << "*** Can't open qtPlot.py"<< std::endl;
		FileFound = false;
	}

    std::cout<<"Argv[2]:"<<argv[2]<<std::endl; 
    if(strcmp(argv[2], "ipython") == 0 ) {
        std::cout<<"IPython mode: "<<std::endl; 
        // start the IPython shell 
        PySys_SetArgv(argc, argv); 
        PyRun_SimpleString("from IPython.Shell import IPShellEmbed\n");
        PyRun_SimpleString("ipshell = IPShellEmbed()\n");
        PyRun_SimpleString("ipshell()\n");
    } else {
        // not in ipython 
        std::cout<<"Regular Python mode, loading python script:"<<std::endl; 
        PyRun_InteractiveLoop(fp, "qtPlot.py");
        fclose(fp);
    } 

    Py_Finalize();

    // kill all tasks and perform cleanup
    localManager->KillAll();
    localManager->Cleanup();

    return 0;
}

