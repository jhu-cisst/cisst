/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Andrew LaMora
  Created on: 2005-02-28

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Declaration of ireFramework for managing Python IRE tools
*/


#ifndef _ireFramework_h
#define _ireFramework_h

#include <stdexcept>

#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnThrow.h>


#include <cisstInteractive/ireExport.h>


/*!
  \brief Class to manage the preparation of Python for the
  Interactive Robot Environment, and for launching the IRE IDE and
  shell.

  \ingroup cisstInteractive

  The goal of the IRE is to be mindlessly simple to incorporate in
  already complicated applications.

  The purpose of this class is to abstract the mechanics of preparing
  and launching Python in general and the IRE in particular from the
  end user.  It is also used to ensure creation of a single extended
  Python shell with global scope.

  This class is instantiated, and runs as a singleton object.  The
  method is borrowed from Design Principles and the CISST Logger.

  \note The IRE is *not* thread-safe; all threads will run the IRE in
  the Python shell created by this singleton class.  This behavior is
  DESIRED; we are attempting to prevent multiple copies of the Object
  Register from being created from within the same application.  Note
  that it is theoretically possible for the user to nevertheless
  create a new copy of the Object Register referenced by this
  application from within a separate Python shell (the IRE IDE permits
  spawning new shells).
*/
class CISST_EXPORT ireFramework {

private:

	//static PyObject* pInstance;
	// NewPythonThread is true if a new Python thread should be (or was) started for the IRE
	static bool NewPythonThread;

    // IRE States:
    //     IRE_NOT_CONSTRUCTED:  initial state (before Singleton object created)
    //     IRE_INITIALIZED:      once constructed and InitShell called (Python interpreter initialized)
    //     IRE_LAUNCHED:         once LaunchIREShell called
    //     IRE_ACTIVE:           set by callback from Python IRE code just before
    //                              entering main event loop
    //     IRE_FINISHED:         set by callback from Python IRE code just after
    //                              exiting main event loop. Also set by FinalizeShell.
    //
    // In general, the IRE is expected to transition in order through the above states.
    // Currently, there is little error checking for invalid transitions. The Reset method
    // can be used to transition from IRE_FINISHED to IRE_INITIALIZED.
    enum IRE_STATES { IRE_NOT_CONSTRUCTED, IRE_INITIALIZED, IRE_LAUNCHED,
                      IRE_ACTIVE, IRE_FINISHED };

    // The current state of the IRE (see above enum)
    static IRE_STATES IRE_State;

    static void *IreThreadState;

    void InitShellInstance(void);

    void FinalizeShellInstance(void);
    void LaunchIREShellInstance(const char * startup, bool newPythonThread, bool useIPython,
                                bool useStreambuf);

	void JoinIREShellInstance(double timeout);

protected:
	/*! Constructor for this singleton object.  Initialize the Python
      subsystem upon creation.  InitShellInstance changes the IRE state
      from IRE_NOT_CONSTRUCTED to IRE_INITIALIZED. */
	ireFramework() {
	    InitShellInstance();
	}

	/*! Dereferences everything and cleans Python shell.  Changes the IRE
        state to IRE_FINISHED. */
	~ireFramework() {
	    FinalizeShellInstance();
	}
public:

	/*! Return a pointer to the instantiated instance of this object. Note
        that the IRE state will change from IRE_NOT_CONSTRUCTED to IRE_INITIALIZED
        on the first call to this function. */
	static ireFramework* Instance(void);


	/*! Initializes the Python environment.  Called from the
      constructor, the user needs never call this function explicitly.
      As ireFramework is a singleton object, the underlying Python
      shell will have application scope ONLY.  The "irepy" package
      will remain instantiated for the duration of the application.
      All modules loaded and variables created from this class will
      remain in scope; note that this does NOT include objects created
      from within the IRE IDE shell. Changes IRE state to IRE_INITIALIZED. */
	static inline void InitShell(void)  throw(std::runtime_error) {
	    Instance()->InitShellInstance();
	}


	/*! Called explicitly from the class destructor, it removes all
      references to the Python shell.  Python is then free to clean
      itself up.  If the application wishes to restart the IRE,
	  it will be necessary to explicitly call InitShell(). Changes
      IRE state to IRE_FINISHED. */
	static inline void FinalizeShell(void)  throw(std::runtime_error) {
	    Instance()->FinalizeShellInstance();
	}


	/*!  Loads the "irepy" Python IRE package and
      calls the "launch()" method to launch the GUI.

      \note Although the "irepy" package itself remains loaded for the
      duration of the application, any variables/objects instantiated
      within the IRE interpreter are contained in a separate,
      session-scope interpreter; they will not be available from the
      C++ environment after the IRE GUI is shut down.

	  The Python shell will first run the "startup" string.  This can
      be used, for example, to import application-specific Python scripts.
	  The "startup" string is not declared const because that would not
	  be compatible with PySys_SetArgv().

      If the newPythonThread parameter is true, the IRE will be started in
	  a separate thread.  Note, however, that the C++ code will have to
	  periodically call JoinIREShell(); otherwise, the C++ program will
	  not relinquish any execution time to the IRE thread.
	  An alternate implementation would be to create the IRE thread in
	  the C++ code before launching the IRE.  This, however, is less
	  portable and would best be implemented using cisstOSAbstraction.

	  If the useIPython parameter is true, then IPython shell will be started instead
	  of the IRE (GUI). IPython is an enhanced interactive Python shell, allowing autocompletion, etc.
	  For more information, see http://ipython.scipy.org/moin/

      If the useStreambuf field is true, the IRE logger will show locally-generated
      log messages (using cmnCallbackStreambuf).  When using the system-wide logger,
      it is recommended to set this to false.

      If the IRE state on entry is IRE_INITIALIZED, this method changes
      it to IRE_LAUNCHED; otherwise, it throws an exception.
      When the Python code finishes its initialization, it will call a
      callback function to change the state to IRE_ACTIVE prior to entering
      the wxPython main event loop.  On exit from the event loop, it will
      call the callback function to change the state to IRE_FINISHED. */
	static inline void LaunchIREShell(const char *startup = "", bool newPythonThread = false, bool useIPython = false,
                                      bool useStreambuf = true) throw(std::runtime_error) {
	    Instance()->LaunchIREShellInstance(startup, newPythonThread, useIPython, useStreambuf);
	}

    /*! Single argument function that can be passed directly to osaThread::Create, e.g.,:
        \code
        osaThread IreThread;
        IreThread.Create<char *> (&ireFramework::RunIRE_wxPython, "print 'using wxPython'");
        \endcode
    */
    static inline void *RunIRE_wxPython(const char *startup) {
        try {
            LaunchIREShell(startup, false, false, true);
        }
        catch (...) {
            CMN_LOG_INIT_ERROR << "Could not launch IRE shell (wxPython)" << std::endl;
        }
        FinalizeShell();
        return 0;
    }

    /*! Single argument function that can be passed directly to osaThread::Create, e.g.,:
        \code
        osaThread IreThread;
        IreThread.Create<char *> (&ireFramework::RunIRE_IPython, "print 'using IPython'");
        \endcode
    */
    static inline void *RunIRE_IPython(const char *startup) {
        try {
            LaunchIREShell(startup, false, true, false);
        }
        catch (...) {
            CMN_LOG_INIT_ERROR << "Could not launch IRE shell (IPython)" << std::endl;
        }
        FinalizeShell();
        return 0;
    }

	/*! Wait for IRE shell to finish (if started in a new Python thread).  Specify a negative
	    timeout to wait forever. */
	static inline void JoinIREShell(double timeout) {
		Instance()->JoinIREShellInstance(timeout);
	}

    /*! Check whether the IRE shell is (or could be) starting (i.e., not active and not finished).
        We do not check the "obvious" case of the IRE state being IRE_LAUNCHED because if the IRE
        is being launched from a new C++ thread, the thread may not yet have been created. */
    static bool IsStarting();

    /*! Check whether IRE shell is active (i.e., all initialization complete and wxPython app loop entered).
        Note that there will be some time delay between when LaunchIREShell() is invoked and when IsActive()
        becomes true. */
    static bool IsActive();

    /*! Check whether the IRE has been exited. */
    static bool IsFinished();

    /*! This is provided as a callback to the Python code that launches the IRE, so that it can change
        the state to IRE_ACTIVE or IRE_FINISHED. */
    static void SetActiveFlag(bool flag);

    /*! Check whether the Python interpreter has been initialized (i.e., the ireFramework instance has
        been created and FinalizeIREShell() has not been called). */
    static bool IsInitialized();

    /*! If the current IRE state is IRE_FINISHED, change it to IRE_INITIALIZED (calling InitShell if
        necessary). This is provided to facililate relaunching of the IRE. */
    static void Reset();

    /*! Release the global interpreter lock. This is intended to be used as a callback (e.g., in osaThreadSignal) */
    static void UnblockThreads();

    /*! Re-acquire the global interpreter lock. This is intended to be used as a callback (e.g., in osaThreadSignal) */
    static void BlockThreads();

    /*! Write a string to the logger window */
    static void PrintLog(const char * str, int len);
};

#endif // _ireFramework_h

