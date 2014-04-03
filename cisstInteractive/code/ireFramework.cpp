/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Andrew LaMora, Peter Kazanzides
  Created on: 2005-02-28

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <Python.h>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnCallbackStreambuf.h>
#include <cisstCommon/cmnAssert.h>
#include <cisstInteractive/ireFramework.h>

#if (CISST_OS == CISST_LINUX)
#include <dlfcn.h>
#endif

// PyTextCtrlHook
//
// This class supports displaying cmnLogger output (e.g., from CMN_LOG)
// in an IRE wxTextCtrl window.  All member functions of this class
// are static so that they can easily be passed as function pointers
// (to Python or to cmnCallbackStreambuf).  The class was created primarily
// for organizational purposes -- a namespace may have been just
// as effective.
//
// The display of cmnLogger output is accomplished as follows:
//
//    1. The C++ code (LaunchIREShellInstance) initializes the ireLogger module,
//       starts the Python interpreter, and launches the IRE.
//
//    2. During initialization (in ireMain.py), the IRE calls the
//       PyTextCtrlHook::SetTextOutput C++ function to store the address of a Python
//       callback function (such as wx.PostEvent) as well as the information needed
//       to call PostEvent (window and event class).
//
//    3. The PyTextCtrlHook::SetTextOutput C++ code instantiates a new
//       cmnCallbackStreambuf, passing it the PyTextCtrlHook::PrintLog callback function.
//       After creating the cmnCallbackStreambuf instance, the C++ code adds it as a new
//       channel to the cmnLogger multiplexer.
//
//    4. When the C++ code logs a message, it is sent to the cmnCallbackStreambuf
//       class.  This class calls the callback function (PyTextCtrlHook::PrintLog)
//       whenever a newline is encountered or the buffer is full.
//
//    5. PyTextCtrlHook::PrintLog creates a new Python event instance, sets the character
//       string as an attribute for that event, and then calls the Pythoon callback function
///      (PostEvent) that was specified when PyTextCtrlHook::SetTextOutput was called.
//       This will cause an event to be queued for the GUI thread, which will handle
//       the event and write the text to the wx.TextCtrl object.
//       Note that if a "built-in" (i.e., C) function is specified for the callback,
//       the C function will be called directly via PyCFunction_Call().  In other words,
//       PrintLog will directly call the wxPython DLL, rather than via the Python interpreter.
//
//    6. The Python code calls PyTextCtrlHook::ClearTextOutput when the TextCtrl
//       window is destroyed.
//
// This method is needed because:
//
//    1. Although wxTextCtrl can be derived from a streambuf (via multiple inheritance),
//       that feature is often disabled during compilation of wxWidgets/wxPython.  Furthermore,
//       it is not threadsafe to call wxTextCtrl from different threads.  It is fine, however,
//       to call PostEvent from any thread.
//
//    2. We wish to avoid a dependency on wxWidgets for the C++ code. Thus, we create the event
//       object by calling the PyObject_CallObject, rather than by using the wxWidgets mechanism for
//       creating a new event.
//


class PyTextCtrlHook {
    static PyObject* PythonFunc;         // the Python callback function (i.e., PostEvent)
    static PyObject* PythonEventClass;   // the event object to pass to PostEvent
    static PyObject* PythonWindow;       // the window that will handle the event
    static PyMethodDef Methods[];
    static cmnCallbackStreambuf<char> *Streambuf;
    static cmnLogMask Mask;

public:
    // Specify the Python callback function
    static PyObject* SetTextOutput(PyObject* self, PyObject* args);
    // Remove the Python callback function
    static PyObject* ClearTextOutput(PyObject* self, PyObject* args);

    // Get the channel log mask
    static PyObject* GetMask(PyObject* self, PyObject* args);
    // Set the channel log mask
    static PyObject* SetMask(PyObject* self, PyObject* args);

    static void SetupStreambuf(void);

    // The C++ callback function that is passed to cmnCallbackStreambuf
    static void PrintLog(const char * str, int len);

    // Initialize the Python module
    static void InitModule(const char * name);

    // Cleanup
    static void Cleanup();
};

PyObject* PyTextCtrlHook::PythonFunc = 0;
PyObject* PyTextCtrlHook::PythonEventClass = 0;
PyObject* PyTextCtrlHook::PythonWindow = 0;
cmnCallbackStreambuf<char> *PyTextCtrlHook::Streambuf = 0;
cmnLogMask PyTextCtrlHook::Mask = CMN_LOG_ALLOW_DEFAULT;

// Specify the Python callback function.
PyObject* PyTextCtrlHook::SetTextOutput(PyObject* CMN_UNUSED(self), PyObject* args)
{
    PyObject* func;
    PyObject* evtclass;    // event class
    PyObject* handler;     // handler for GUI window
    int iMask = (int)Mask;
    if (PyArg_ParseTuple(args,"OOO|i",&func,&evtclass,&handler,&iMask)) {
        if (!PyCallable_Check(func)) {
            PyErr_SetString(PyExc_TypeError, "expected a callable for parameter 1.");
            return NULL;
        }
        if (!PyCallable_Check(evtclass)) {
            PyErr_SetString(PyExc_TypeError, "expected a callable (class) for parameter 2.");
            return NULL;
        }
        Py_XINCREF(func);              // Save reference to callback
        Py_XDECREF(PythonFunc);        // Release any previous callback
        PythonFunc = func;
        Py_XINCREF(evtclass);          // Save reference to event
        Py_XDECREF(PythonEventClass);  // Release any previous event
        PythonEventClass = evtclass;
        Py_XINCREF(handler);           // Save reference to handler
        Py_XDECREF(PythonWindow);      // Release any previous handler
        PythonWindow = handler;
        Mask = static_cast<cmnLogMask>(iMask);
        Py_INCREF(Py_None);
        return Py_None;
    }
    return NULL;
}

// Remove the Python callback function
PyObject* PyTextCtrlHook::ClearTextOutput(PyObject* CMN_UNUSED(self), PyObject* CMN_UNUSED(args))
{
    Cleanup();
    Py_INCREF(Py_None);
    return Py_None;
}

// Get the channel log mask
PyObject* PyTextCtrlHook::GetMask(PyObject* CMN_UNUSED(self), PyObject* CMN_UNUSED(args))
{
    // If the channel is active, get the log mask from the multiplexer,
    // just in case it was changed from within the C++ code (very unlikely).
    if (Streambuf) {
        cmnLogger::GetMultiplexer()->GetChannelMask(Streambuf, Mask);
    }
    return Py_BuildValue("i", (int)Mask);
}

// Set the channel log mask
PyObject* PyTextCtrlHook::SetMask(PyObject* CMN_UNUSED(self), PyObject* args)
{
    int iMask;
    if (!PyArg_ParseTuple(args,"i",&iMask)) {
        PyErr_SetString(PyExc_TypeError, "integer type expected");
        return NULL;
    }
    Mask = static_cast<cmnLogMask>(iMask);
    if (Streambuf) {
        cmnLogger::GetMultiplexer()->SetChannelMask(Streambuf, Mask);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

void PyTextCtrlHook::SetupStreambuf(void)
{
    if (!Streambuf)
        Streambuf = new cmnCallbackStreambuf<char>(PrintLog);
    cmnLogger::GetMultiplexer()->AddChannel(Streambuf, Mask);
}

// The C++ callback function that is passed to cmnCallbackStreambuf.
// It calls the Python callback function stored in PythonFunc.
// Parameters:
//     str:  a null-terminated string
//     len:  the length of the string
void PyTextCtrlHook::PrintLog(const char * str, int len)
{
    PyObject* result;

    // If PythonFunc is set, we know that PythonEventClass is also set
    // and that both of them are callable objects.
    if (PythonFunc) {
        // For thread safety (get Python global interpreter lock).
        // This function is defined in Python 2.3 or greater.
        PyGILState_STATE gstate = PyGILState_Ensure();

        PyObject* event = PyObject_CallObject(PythonEventClass, NULL);
        CMN_ASSERT(event);

        // Convert the C string to a Python string
        PyObject* pystr = Py_BuildValue("s", str);
        // Store the string as the "msg" attribute in the event object
        CMN_ASSERT(PyObject_SetAttrString(event, "msg", pystr) == 0);
        // Decrement "pystr" reference count (PyObject_SetAttrString incremented it).
        Py_DECREF(pystr);
        // Build the args for the Python function
        PyObject* args = Py_BuildValue("OO", PythonWindow, event);
        // Decrement "event" reference count (Py_BuildValue incremented it)
        Py_DECREF(event);
        // Call the callback function
        if (PyCFunction_Check(PythonFunc)) {
            // If it is a (wrapped) C function, call it directly.
            try {
                result = PyCFunction_Call(PythonFunc, args, 0);
            }
            catch(...) {
                std::cout << "PyCFunction_Call exception" << std::endl;
            }
        }
        else
            // Call the Python function (via the interpreter)
            result = PyEval_CallObject(PythonFunc, args);
        // Decrement reference count
        Py_DECREF(args);

        // If the result is NULL, we had a problem.  Do not write
        // to CMN_LOG because that could lead to an infinite loop
        // (since the logging functions call this function).
        if (result == NULL)
            std::cout << "CallObject returned NULL" << std::endl;
        else
            Py_DECREF(result);

        // Release Python global interpreter lock
        PyGILState_Release(gstate);
    }
}

// Cleanup:  remove logger channel, delete Streambuf object,
//           remove Python callback
void PyTextCtrlHook::Cleanup()
{
    if (Streambuf) {
        cmnLogger::GetMultiplexer()->RemoveChannel(Streambuf);
        delete Streambuf;
        Streambuf = 0;
    }
    if (PythonFunc) {
        Py_XDECREF(PythonFunc);   // Release any previous callback
        PythonFunc = 0;
    }
}

void ireFramework::SetActiveFlag(bool flag)
{
    IRE_State = (flag ? IRE_ACTIVE : IRE_FINISHED);
}

PyObject* SetActiveFlagWrap(PyObject* CMN_UNUSED(self), PyObject* args)
{
    PyObject* pyflag;
    if (PyArg_ParseTuple(args,"O",&pyflag))
        ireFramework::SetActiveFlag(pyflag == Py_True);
    return Py_None;
}

PyMethodDef PyTextCtrlHook::Methods[] = {
    { "SetTextOutput", SetTextOutput, METH_VARARGS,"SetTextOutput(func, evtclass, window, mask=current-mask)\n\n"
          "   Set callback function (func) for logging text.\n"
          "   Function should accept a window id and an event object (new instance of evtclass).\n"
          "   mask is channel log mask (defaults to current setting)."
 },
    { "ClearTextOutput", ClearTextOutput, METH_NOARGS, "Clear callback function for logging text."},
    { "GetMask", GetMask, METH_NOARGS, "Get the channel log mask."},
    { "SetMask", SetMask, METH_VARARGS, "Set the channel log mask."},
    { "SetActiveFlag", SetActiveFlagWrap, METH_VARARGS, "Set flag indicating whether IRE is initialized and active."},
    { NULL, NULL, 0, NULL }
};

void PyTextCtrlHook::InitModule(const char * name)
{
    Py_InitModule(name, Methods);
}

// ****************************************************************************
// ireFramework static member data
//
bool ireFramework::NewPythonThread = false;
ireFramework::IRE_STATES ireFramework::IRE_State = ireFramework::IRE_NOT_CONSTRUCTED;

void *ireFramework::IreThreadState = 0;

// pInstance was removed from the class so that ireFramework.h does not
// need to include <Python.h>.  This avoids some compiler warnings on Linux.
//PyObject* ireFramework::pInstance = 0;
static PyObject* pInstance = 0;

// ****************************************************************************
// ireFramework::Instance
//
// Return a reference to this one, only, instantiated singleton object.
//
ireFramework* ireFramework::Instance(void) {
    // create a static variable
    static ireFramework instance;
    return &instance;
}

// ****************************************************************************
// ireFramework::InitShellInstance
//
// Initialize the Python shell
//
void ireFramework::InitShellInstance(void)
{
    if (IsInitialized()) {
        CMN_LOG_INIT_WARNING << "ireFramework already initialized" << std::endl;
        return;
    }
#if (CISST_OS == CISST_LINUX)
    // Need following to avoid having PyExc_ValueError be an undefined symbol
    // when loading the IRE (wxPython version). The Python library is already loaded,
    // but the symbols are not global -- this is fixed by the dlopen call below with
    // RTLD_NOLOAD (doesn't load the library -- assumes it is already loaded).
    char libname[40];
    sprintf(libname, "libpython%d.%d.so", (int)PY_MAJOR_VERSION, (int)PY_MINOR_VERSION);
    dlopen(libname, RTLD_LAZY | RTLD_NOLOAD | RTLD_GLOBAL);
    const char *msg = dlerror();
    if (msg) {
        std::cerr << "InitShellInstance: dlerror when loading " << libname
                           << ": " << msg << std::endl;
        CMN_LOG_INIT_ERROR << "InitShellInstance: dlerror when loading " << libname
                           << ": " << msg << std::endl;
    }
#endif
    Py_Initialize();
    PyEval_InitThreads();
#if (CISST_OS == CISST_LINUX)
    // For Linux, change dlopenflags to avoid swig::stop_iterator exceptions
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyThreadState *threadState = PyThreadState_Get();
    threadState->interp->dlopenflags |= RTLD_GLOBAL;
    PyGILState_Release(gstate);
#endif
    IRE_State = IRE_INITIALIZED;
}

// ****************************************************************************
// ireFramework::FinalizeShellInstance
//
// Finalize (cleanup) the Python shell.  Also cleans up the PyTextCtrlHook.
//
void ireFramework::FinalizeShellInstance(void)
{
    PyTextCtrlHook::Cleanup();
    Py_XDECREF(pInstance);
    pInstance = 0;
    Py_Finalize();
    // Change the state to IRE_FINISHED (the IRE is probably already in this
    // state).
    IRE_State = IRE_FINISHED;
}

// ****************************************************************************
// LaunchIREShellInstance
//
// This method imports necessary modules and launches the IRE GUI.  It should
// be called AFTER InitShell.
//
// Modules Loaded:
//  - irepy
//
// Developers should take care to DECREF (decrease the reference counts)
// of any PyObject s that use hard references.
//
void ireFramework::LaunchIREShellInstance(const char * startup, bool newPythonThread, bool useIPython,
                                          bool useStreambuf) {
    //start python
    const char * python_args[] = { "", startup };

    if (IRE_State != IRE_INITIALIZED) {
        CMN_LOG_INIT_ERROR << "LaunchIREShellInstance:  IRE state is " << IRE_State << "." << std::endl;
        cmnThrow(std::runtime_error("LaunchIREShellInstance: invalid IRE state."));
    }
    IRE_State = IRE_LAUNCHED;
    NewPythonThread = newPythonThread;

    if (pInstance)
        Py_DECREF(pInstance);
    pInstance = 0;

    // Initialize ireLogger module, which is used for the cmnLogger output window
    PyTextCtrlHook::InitModule("ireLogger");
    PySys_SetArgv(2, const_cast<char **>(python_args));

    // Get global dictionary (pModule and pDict are borrowed references)
    PyObject *pModule = PyImport_AddModule("__main__");
    PyObject *pDict = PyModule_GetDict(pModule);
    if (pDict == NULL) {
        IRE_State = IRE_FINISHED;
        PyErr_Clear();
        cmnThrow(std::runtime_error("LaunchIREShellInstance: could not get global dictionary"));
    }

    char launchString[32];
    if (useIPython) {
        PyRun_SimpleString(startup);
        PyRun_SimpleString("from IPython.Shell import IPShellEmbed\n");
        PyRun_SimpleString("ipshell = IPShellEmbed()\n");
        strcpy(launchString, "ipshell(local_ns = globals())");
        IRE_State = IRE_ACTIVE;  // for now, instead of using SetActiveState callback
    }
    else {
        PyRun_SimpleString("from irepy import ireMain");
        strcpy(launchString, "ireMain.launchIrePython()");
        if (useStreambuf)
            PyTextCtrlHook::SetupStreambuf();
    }

    if (NewPythonThread) {
        PyRun_SimpleString("import threading\n");
        char buffer[300];
        sprintf(buffer, "class IreThread(threading.Thread):\n"
                        "    def __init__(self):\n"
                        "        threading.Thread.__init__(self)\n"
                        "    def run(self):\n"
                        "        %s",
                        launchString);
        PyRun_SimpleString(buffer);
        PyObject* pClass = PyDict_GetItemString(pDict, "IreThread");

        // Create an instance of the class
        if (PyCallable_Check(pClass))
            pInstance = PyObject_CallObject(pClass, NULL);
        if (pInstance) {
            PyObject_CallMethod(pInstance, "start", NULL);
        }
        else {
            IRE_State = IRE_FINISHED;
            PyErr_Clear();
            cmnThrow(std::runtime_error("LaunchIREShellInstance: Could not create IRE thread."));
        }
    }
    else {
        PyRun_SimpleString(launchString);
    }
}

void ireFramework::JoinIREShellInstance(double timeout)
{
    if (NewPythonThread && pInstance && ((IRE_State == IRE_LAUNCHED) || (IRE_State == IRE_ACTIVE))) {
        // First, check whether the Python thread is still alive.
        PyObject *result;
        result = PyObject_CallMethod(pInstance, "isAlive", NULL);
        bool isAlive = (result == Py_True);
        Py_DECREF(result);
        if (isAlive) {
            if (timeout >= 0)
                PyObject_CallMethod(pInstance, "join", "(f)", timeout);
            else
                PyObject_CallMethod(pInstance, "join", NULL);
        }
        else
            IRE_State = IRE_FINISHED;
    }
}

bool ireFramework::IsStarting()
{
    return (!IsActive() && !IsFinished());
}

bool ireFramework::IsActive()
{
    return (IRE_State == IRE_ACTIVE);
}

bool ireFramework::IsFinished()
{
    return (IRE_State == IRE_FINISHED);
}

bool ireFramework::IsInitialized()
{
    return (Py_IsInitialized() != 0);
}

void ireFramework::Reset()
{
    if (IRE_State == IRE_FINISHED) {
        if (!IsInitialized())
            InitShell();
        else
            IRE_State = IRE_INITIALIZED;
    }
}

void ireFramework::UnblockThreads()
{
    if ((IRE_State == IRE_LAUNCHED) || (IRE_State == IRE_ACTIVE))
        IreThreadState = static_cast<void *>(PyEval_SaveThread());
    else
        IreThreadState = 0;
}

void ireFramework::BlockThreads()
{
    if (IreThreadState)
        PyEval_RestoreThread(static_cast<PyThreadState *>(IreThreadState));
}

void ireFramework::PrintLog(const char * str, int len)
{
    PyTextCtrlHook::PrintLog(str, len);
}
