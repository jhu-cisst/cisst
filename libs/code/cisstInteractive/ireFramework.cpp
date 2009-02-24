/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#include <cisstCommon/cmnCallbackStreambuf.h>
#include <cisstCommon/cmnAssert.h>
#include <cisstInteractive/ireFramework.h>

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
    static cmnLODMultiplexerStreambuf<char>::LodType LoD;

public:
    // Specify the Python callback function
    static PyObject* SetTextOutput(PyObject* self, PyObject* args);
    // Remove the Python callback function
    static PyObject* ClearTextOutput(PyObject* self, PyObject* args);

    // Get the channel level of detail
    static PyObject* GetLoD(PyObject* self, PyObject* args);
    // Set the channel level of detail
    static PyObject* SetLoD(PyObject* self, PyObject* args);

    // The C++ callback function that is passed to cmnCallbackStreambuf
    static void PrintLog(char *str, int len);

    // Initialize the Python module
    static void InitModule(char* name);

    // Cleanup
    static void Cleanup();
};

PyObject* PyTextCtrlHook::PythonFunc = 0;
PyObject* PyTextCtrlHook::PythonEventClass = 0;
PyObject* PyTextCtrlHook::PythonWindow = 0;
cmnCallbackStreambuf<char> *PyTextCtrlHook::Streambuf = 0;
cmnLODMultiplexerStreambuf<char>::LodType PyTextCtrlHook::LoD = CMN_LOG_DEFAULT_LOD;

// Specify the Python callback function.
PyObject* PyTextCtrlHook::SetTextOutput(PyObject* CMN_UNUSED(self), PyObject* args)
{
    PyObject* func;
    PyObject* evtclass;    // event class
    PyObject* handler;     // handler for GUI window
    int iLoD = (int)LoD;
    if (PyArg_ParseTuple(args,"OOO|i",&func,&evtclass,&handler,&iLoD)) {
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
        LoD = (cmnLODMultiplexerStreambuf<char>::LodType) iLoD;
        if (!Streambuf)
            Streambuf = new cmnCallbackStreambuf<char>(PrintLog);
        cmnLogger::GetMultiplexer()->AddChannel(Streambuf, LoD);
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

// Get the channel level of detail
PyObject* PyTextCtrlHook::GetLoD(PyObject* CMN_UNUSED(self), PyObject* CMN_UNUSED(args))
{
    // If the channel is active, get the LOD from the multiplexer,
    // just in case it was changed from within the C++ code (very unlikely).
    if (Streambuf)
        cmnLogger::GetMultiplexer()->GetChannelLOD(Streambuf, LoD);
    return Py_BuildValue("i", (int)LoD);
}

// Set the channel level of detail
PyObject* PyTextCtrlHook::SetLoD(PyObject* CMN_UNUSED(self), PyObject* args)
{
    int iLoD;
    if (!PyArg_ParseTuple(args,"i",&iLoD)) {
        PyErr_SetString(PyExc_TypeError, "integer type expected");
        return NULL;
    }
    LoD = (cmnLODMultiplexerStreambuf<char>::LodType) iLoD;
    if (Streambuf)
        cmnLogger::GetMultiplexer()->SetChannelLOD(Streambuf, LoD);
    Py_INCREF(Py_None);
    return Py_None;
}


// The C++ callback function that is passed to cmnCallbackStreambuf.
// It calls the Python callback function stored in PythonFunc.
// Parameters:
//     str:  a null-terminated string
//     len:  the length of the string
void PyTextCtrlHook::PrintLog(char *str, int len)
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
    { "SetTextOutput", SetTextOutput, METH_VARARGS,"SetTextOutput(func, evtclass, window, LoD=current-lod)\n\n"
          "   Set callback function (func) for logging text.\n"
          "   Function should accept a window id and an event object (new instance of evtclass).\n"
          "   LoD is channel level of detail (defaults to current setting)."
 },
    { "ClearTextOutput", ClearTextOutput, METH_NOARGS, "Clear callback function for logging text."},
    { "GetLoD", GetLoD, METH_NOARGS, "Get the channel level of detail."},
    { "SetLoD", SetLoD, METH_VARARGS, "Set the channel level of detail."},
    { "SetActiveFlag", SetActiveFlagWrap, METH_VARARGS, "Set flag indicating whether IRE is initialized and active."},
    { NULL, NULL, 0, NULL }
};

void PyTextCtrlHook::InitModule(char* name)
{
    Py_InitModule(name, Methods);
}

// ****************************************************************************
// ireFramework static member data
//
bool ireFramework::NewPythonThread = false;
ireFramework::IRE_STATES ireFramework::IRE_State = ireFramework::IRE_NOT_CONSTRUCTED;

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
    Py_Initialize();
    PyEval_InitThreads();
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
void ireFramework::LaunchIREShellInstance(char *startup, bool newPythonThread, bool useIPython) {
    //start python
    char* python_args[] = { "IRE", startup };

    if (IRE_State != IRE_INITIALIZED) {
        CMN_LOG(1) << "LaunchIREShellInstance:  IRE state is " << IRE_State << "." << std::endl;
        cmnThrow(std::runtime_error("LaunchIREShellInstance: invalid IRE state."));
    }
    IRE_State = IRE_LAUNCHED;

    // Initialize ireLogger module, which is used for the cmnLogger output window
    PyTextCtrlHook::InitModule("ireLogger");
    PySys_SetArgv(2, python_args);

    if (useIPython) {
        PyRun_SimpleString(startup);
        PyRun_SimpleString("from IPython.Shell import IPShellEmbed\n");
        PyRun_SimpleString("ipshell = IPShellEmbed()\n");
        PyRun_SimpleString("ipshell()\n");
        IRE_State = IRE_ACTIVE;  // for now, instead of using SetActiveState callback
        return;
    }


    PyObject *pName, *pModule, *pDict, *pFunc;
    PyObject *pArgs, *pValue;

    pName = PyString_FromString("irepy");

    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pInstance)
        Py_DECREF(pInstance);
    pInstance = 0;

    NewPythonThread = newPythonThread;
    if (pModule != NULL) {
        // pDict is borrowed - no need to decref
        pDict = PyModule_GetDict(pModule);

        if (NewPythonThread) {
            PyObject* pClass = PyDict_GetItemString(pDict, "IreThread");

            // Create an instance of the class
            if (PyCallable_Check(pClass))
            {
                pInstance = PyObject_CallObject(pClass, NULL);
            }
            if (pInstance)
                PyObject_CallMethod(pInstance, "start", NULL);
            else {
                IRE_State = IRE_FINISHED;
                PyErr_Clear();
                cmnThrow(std::runtime_error("LaunchIREShellInstance: Could not create IRE thread."));
            }
        }
        else {
            // pFunc is borrowed
            pFunc = PyDict_GetItemString(pDict, "launch");

            if (pFunc && PyCallable_Check(pFunc)) {
                pArgs = PyTuple_New(0);

                // pValue is NOT borrowed - remember to decref
                pValue = PyObject_CallObject(pFunc, pArgs);
            }
            else {
                if (PyErr_Occurred()) {

                    IRE_State = IRE_FINISHED;
                    PyErr_Clear();
                    cmnThrow(std::runtime_error("LaunchIREShellInstance: Call to the launch() function failed."));
                }
            }

            //decref all PyObjects
            Py_DECREF(pValue);
            Py_DECREF(pArgs);
        }
        Py_DECREF(pModule);
    }
    else {
        IRE_State = IRE_FINISHED;
        PyErr_Clear();
        cmnThrow(std::runtime_error("LaunchIREShellInstance: import irepy has failed.  Check libraries."));
    }
#if 0
    // Set IRE_State to IRE_FINISHED on exit, unless IRE started in a new Python thread.
    // Not necessary because IRE_State is modified by callback from IRE Python call.
    if (!NewPythonThread)
        IRE_State = IRE_FINISHED;
#endif
}

void ireFramework::JoinIREShellInstance(double timeout)
{
    if (NewPythonThread && ((IRE_State == IRE_LAUNCHED) || (IRE_State == IRE_ACTIVE))) {
        if (timeout >= 0)
            PyObject_CallMethod(pInstance, "join", "(f)", timeout);
        else
            PyObject_CallMethod(pInstance, "join", NULL);
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
#if 0
    // Code for checking whether Python thread is still alive.
    // This is not needed because the IRE Python code calls
    // SetActiveFlag(False) on exit (via the callback function).
    if (NewPythonThread && pInstance) {
        PyObject *result;
        result = PyObject_CallMethod(pInstance, "isAlive", NULL);
        bool ret = (result == Py_True);
        Py_DECREF(result);
        return ret;
    }
#endif
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

