/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Peter Kazanzides
  Created on: 2005-12-21

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstConfig.h>
#include <cisstCommon/cmnObjectRegister.h>
#include <cisstInteractive/ireFramework.h>

#ifdef CISST_OSATHREAD
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaSleep.h>
#endif

#if (CISST_OS == CISST_DARWIN)
#include <unistd.h>   // for execvp()
#endif

using namespace std;

#include "SineGenerator.h"

// This program demonstrates the following two cases:
//
// 1) The C++ program creates a separate thread for launching the IRE (CISST_OSATHREAD defined).
//
// 2) The C++ program asks Python to create a separate thread for the IRE (CISST_OSATHREAD not
//    defined).
//
// Note that the second parameter to ireFramework::LaunchIREShell() is false (the default) or true,
// corresponding to cases 1 and 2 respectively.
//

int main(int argc, char *argv[])
{

#if (CISST_OS == CISST_DARWIN)

    // Mac OS X is picky about allowing access to the screen.  It looks at the program name (in argv[0])
    // to determine whether to allow screen access.  A terminal program (such as this one) will not be
    // allowed access.  Therefore, we have to trick OS X by "stealing" the program name from pythonw.
    // One option is to set the new argv[0] to the correct pythonw path (see PYTHONW_PATH below).
    // This is not ideal because it relies on a hard-coded path, so that implementation is not used.
    // It is better to just let pythonw give us this information (via sys.executable).  So, the workaround
    // is as follows:
    //
    //    - pythonEmbeddedIRE is executed.  The "DARWIN_HACK" parameter will not be present, so it will
    //      do an execvp() to pythonw and pass a command string such as the following:
    //         "import sys, os; os.execvp('pythonEmbeddedIRE', (sys.executable, 'DARWIN_HACK'))"
    //
    //    - pythonw will process the command string, which causes it to execvp() to pythonEmbeddedIRE.  This
    //      time, "DARWIN_HACK" is present at argv[1], so pythonEmbeddedIRE continues and launches the IRE.
    //
    // This hack should not be necessary if pythonEmbeddedIRE is compiled differently (e.g., as a bundle).

    if ((argc < 2) || (strcmp(argv[1], "DARWIN_HACK") != 0)) {
#if 1
        // Implementation using pythonw to set argv[0] to correct path:
        // We use argv[0] to make sure that we can find this program (pythonEmbeddedIRE).
        // This is necessary if pythonEmbeddedIRE is not on the PATH.
        const char *cmdstr = "import sys, os; os.execvp('%s', (sys.executable, 'DARWIN_HACK'))";
        char *command = new char[strlen(cmdstr)+strlen(argv[0])-1];
        sprintf(command, cmdstr, argv[0]);
        char *new_argv[] = { "pythonw", "-c", command, 0};
        execvp("pythonw", new_argv);
#else
        // Implementation using a hard-coded path (PYTHONW_PATH) for argv[0]:
        // The program execvp's to itself to change argv[0] (writing directly to argv[0] does not work).
        char *PYTHONW_PATH = "/System/Library/Frameworks/Python.framework/Versions/Current/"
                             "Resources/Python.app/Contents/MacOS/Python";
        char *new_argv[] = { PYTHONW_PATH, "DARWIN_HACK", 0};
        execvp(argv[0], new_argv);
#endif
        perror("execv failed:");
        exit(-1);
    }
#endif  // (CISST_OS == CISST_DARWIN)

    cout << "*** Creating sine wave generator, amp = 5, freq = 0 ***" << endl;
    SineGenerator wave(5.0, 0.0);

    cout << "*** Registering sine wave generator ***" << endl;
    cmnObjectRegister::Register("SineGenerator", &wave);

#ifdef CISST_OSATHREAD
    cout << "*** Launching IRE shell (C++ Thread) ***" << endl;
    osaThread IreThread;
    IreThread.Create<const char *> (&ireFramework::RunIRE_wxPython, "from pythonEmbeddedIRE import *");
#else
    cout << "*** Launching IRE shell (Python Thread) ***" << endl;
    try {
        ireFramework::LaunchIREShell("from pythonEmbeddedIRE import *", true);
    }
    catch (...) {
        cout << "*** ERROR:  could not launch IRE shell ***" << endl;
    }
#endif

    // Wait for IRE to initialize itself
    while (ireFramework::IsStarting()) {
#ifdef CISST_OSATHREAD
        osaSleep(0.5);  // Wait 0.5 seconds
#else
        // Need following call to give the IRE thread some time to execute.
        ireFramework::JoinIREShell(0.001);
#endif
    }

    cout << "Use wave.SetFrequency(X), with X > 0, to enable display" << endl;
    cout << "Use wave.SetFrequency(0) to stop display" << endl;

    double time = 0.0;
    const double tdelta = 0.001;
    // Loop until the IRE is exited.
    while (ireFramework::IsActive()) {
        // Only display output if frequency is non-zero
        if (wave.GetFrequency()) {
            cout << "t = " << time << " (A = " << wave.GetAmplitude()
                      << ", F = " << wave.GetFrequency() << "): "
                      << wave.ComputeOutput(time) << endl;
            time += tdelta;
        }
#ifndef CISST_OSATHREAD
        // Need following call to give the IRE thread some time to execute.
        ireFramework::JoinIREShell(0.001);
#endif
    }

    cout << "*** Clean up and exit ***" << endl;
#ifdef CISST_OSATHREAD
    IreThread.Wait();
#else
    ireFramework::FinalizeShell();
#endif
    return 0;
}

