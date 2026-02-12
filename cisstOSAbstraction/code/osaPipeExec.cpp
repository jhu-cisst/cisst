/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s): Martin Kelly
  Created on: 2010-09-23

  (C) Copyright 2010-2019 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstCommon/cmnAssert.h>
#include <cisstOSAbstraction/osaPipeExec.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <string.h>
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
#if (CISST_OS == CISST_QNX)
#include <errno.h>
#else
#include <sys/errno.h>
#include <sys/select.h>
#endif
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#elif (CISST_OS == CISST_WINDOWS)
#include <fcntl.h>
#include <io.h>
#include <process.h>
#include <windows.h>
#endif

enum {READ_END = 0, WRITE_END = 1};

struct osaPipeExecInternals {
    /*! OS dependent variables */
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
    int pid;
#elif (CISST_OS == CISST_WINDOWS)
    HANDLE hProcess;
    HANDLE readHandle;
#endif
};

#define INTERNALS(A) (reinterpret_cast<osaPipeExecInternals*>(Internals)->A)
#define INTERNALS_CONST(A) (reinterpret_cast<const osaPipeExecInternals*>(Internals)->A)

unsigned int osaPipeExec::SizeOfInternals(void)
{
    return sizeof(osaPipeExecInternals);
}

osaPipeExec::osaPipeExec(const std::string & name):
    Connected(false),
    Name(name)
{
    CMN_ASSERT(sizeof(Internals) >= SizeOfInternals());
    // Initialize to invalid values
    ToProgram[READ_END] = ToProgram[WRITE_END] = -1;
    FromProgram[READ_END] = FromProgram[WRITE_END] = -1;
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
    INTERNALS(pid) = -1;
#elif (CISST_OS == CISST_WINDOWS)
    INTERNALS(hProcess) = 0;
#endif
}

osaPipeExec::~osaPipeExec(void)
{
    Close();
}

int osaPipeExec::DoClose(int &n)
{
    int ret = 0;
    if (n >= 0) {
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
        int ret = close(n);
#elif (CISST_OS == CISST_WINDOWS)
        int ret = _close(n);
#else
        int ret = -1;
#endif
        if (ret == -1) {
            CMN_LOG_RUN_WARNING << "osaPipeExec: failed to close handle for \""
                                << this->Name << "\"" << std::endl;
        }
        n = -1;
    }
    return ret;
}

void osaPipeExec::CloseAllPipes(void)
{
    DoClose(ToProgram[READ_END]);
    DoClose(ToProgram[WRITE_END]);
    DoClose(FromProgram[READ_END]);
    DoClose(FromProgram[WRITE_END]);
}

void osaPipeExec::CloseUnusedHandles(void)
{
    /* We want input to come from parent to the program and output the other
       direction. Thus we don't need these ends of the pipe */
    DoClose(ToProgram[READ_END]);
    DoClose(FromProgram[WRITE_END]);
    if (!WriteFlag) DoClose(ToProgram[WRITE_END]);
    if (!ReadFlag)  DoClose(FromProgram[READ_END]);
}

char ** osaPipeExec::ParseCommand(const std::string & executable, const std::vector<std::string> & arguments)
{
    typedef char * charPointer;
    charPointer * command = new charPointer[arguments.size() + 2]; // executable name, arguments, 0
#if (CISST_OS == CISST_WINDOWS)
    /* Needed because Windows parses spaces strangely unless the whole string
    is quoted */
    std::string quotedString = '"' + executable + '"';
    command[0] = strdup(quotedString.c_str());
#else
    command[0] = const_cast<char *>(executable.c_str());
#endif
    for (size_t argumentCounter = 0;
         argumentCounter < arguments.size();
         argumentCounter++) {
#if (CISST_OS == CISST_WINDOWS)
    /* Windows needs each argument quoted or it will parse them as
    separate */
    quotedString = '"' + arguments[argumentCounter] + '"';
    command[argumentCounter + 1] = strdup(quotedString.c_str());
#else
    command[argumentCounter + 1] = const_cast<char *>(arguments[argumentCounter].c_str());
#endif
    }
    command[arguments.size() + 1] = 0; // null terminated list

    return command;
}

void osaPipeExec::FreeCommand(char **command)
{
#if (CISST_OS == CISST_WINDOWS)
    /* Free memory allocated by strdup */
    for (int i = 0; command[i] != 0; i++)
        free(command[i]);
#endif
    delete[] command;
}

#if (CISST_OS == CISST_WINDOWS)
void osaPipeExec::RestoreIO(int newStdin, int newStdout)
{
    if (newStdin >= 0) {
        if (_dup2(newStdin, _fileno(stdin)) == -1) {
            CMN_LOG_INIT_ERROR << "Class osaPipeExec: failed to restore stdin" << std::endl;
            CloseAllPipes();
        }
        _close(newStdin);
    }
    if (newStdout >= 0) {
        if (_dup2(newStdout, _fileno(stdout)) == -1) {
            CMN_LOG_INIT_ERROR << "Class osaPipeExec: failed to restore stdout" << std::endl;
            CloseAllPipes();
        }
        _close(newStdout);
    }
}
#endif

bool osaPipeExec::Open(const std::string & executable, const std::string & mode, bool noWindow)
{
    std::vector<std::string> arguments;
    return Open(executable, arguments, mode, noWindow);
}

bool osaPipeExec::Open(const std::string & executable,
                       const std::vector<std::string> & arguments,
                       const std::string & mode,
                       bool noWindow)
{
    if (Connected) {
        return false;
    }

    ReadFlag = WriteFlag = false;
    std::string::const_iterator it;
    for (it = mode.begin(); it != mode.end(); it++) {
        switch (*it) {
        case 'r':
            ReadFlag = true;
            break;
        case 'w':
            WriteFlag = true;
            break;
        }
    }

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
    if (pipe(ToProgram) == -1) {
        CMN_LOG_INIT_ERROR << "Class osaPipeExec: Open: can't create pipe \"" << this->Name << "\" ("
                           << strerror(errno) << ")" << std::endl;
        return false;
    }
    if (pipe(FromProgram) == -1) {
        CMN_LOG_INIT_ERROR << "Class osaPipeExec: Open: can't create pipe \"" << this->Name << "\" ("
                           << strerror(errno) << ")" << std::endl;
        CloseAllPipes();
        return false;
    }

    /* Spawn a child and parent process for communication */
    INTERNALS(pid) = fork();

    /* If an error occurred */
    if (INTERNALS(pid) < 0) {
        CMN_LOG_INIT_ERROR << "Class osaPipeExec: failed to fork process" << std::endl;
        CloseAllPipes();
        return false;
    }

    /* Parent process to send and receive output */
    else if (INTERNALS(pid) > 0) {
        CloseUnusedHandles();
        Connected = true;
        return true;
    }

    /* Child thread to run the program */
    else if (INTERNALS(pid) == 0) {
        /* Replace stdin and stdout to receive from and write to the pipe */
        if ((dup2(ToProgram[READ_END], STDIN_FILENO) != -1) &&
            (dup2(FromProgram[WRITE_END], STDOUT_FILENO) != -1)) {
            char **cmd = ParseCommand(executable, arguments);
            execvp(cmd[0], cmd);
            /* If we get here then exec failed */
            FreeCommand(cmd);
        }
        CloseAllPipes();
        exit(-1);  // terminate the child program
    }

#elif (CISST_OS == CISST_WINDOWS)
    // On Windows, there are two implementations.
    //   noWindow:  This is a newer implementation that calls CreatePipe and CreateProcess, passing the
    //              CREATE_NO_WINDOW parameter to avoid creating an extra window for the child process.
    //   !noWindow: This is the original implementation that calls _pipe and _spawnvp. Although this could
    //              be replaced by the noWindow implementation (not specifying the CREATE_NO_WINDOW parameter),
    //              it is kept to avoid any possible issues with the new implementation.

    if (noWindow) {
        HANDLE childStdin_Read = NULL;
        HANDLE childStdin_Write = NULL;
        HANDLE childStdout_Read = NULL;
        HANDLE childStdout_Write = NULL;
        SECURITY_ATTRIBUTES saAttr;

        // Set the bInheritHandle flag so pipe handles are inherited.
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        // Create a pipe for the child process stdout.
        if (!CreatePipe(&childStdout_Read, &childStdout_Write, &saAttr, 0)) {
            CMN_LOG_INIT_ERROR << "Class osaPipeExec: failed to create stdout pipe" << std::endl;
            return false;
        }
        // Ensure the read handle to the pipe for stdout is not inherited.
        if (!SetHandleInformation(childStdout_Read, HANDLE_FLAG_INHERIT, 0))
            CMN_LOG_INIT_WARNING << "Class osaPipeExec: failed to set stdout handle info" << std::endl;

        // Save the read handle for use later
        INTERNALS(readHandle) = childStdout_Read;

        FromProgram[READ_END] = _open_osfhandle((intptr_t) childStdout_Read, _O_RDONLY|_O_BINARY);
        FromProgram[WRITE_END] = _open_osfhandle((intptr_t) childStdout_Write, _O_WRONLY|_O_BINARY);

        // Create a pipe for the child process stdin.
        if (!CreatePipe(&childStdin_Read, &childStdin_Write, &saAttr, 0)) {
            CMN_LOG_INIT_ERROR << "Class osaPipeExec: failed to create stdin pipe" << std::endl;
            CloseAllPipes();
            return false;
        }
        // Ensure the write handle to the pipe for STDIN is not inherited.
        if (!SetHandleInformation(childStdin_Write, HANDLE_FLAG_INHERIT, 0))
            CMN_LOG_INIT_WARNING << "Class osaPipeExec: failed to set stdin handle info" << std::endl;
        ToProgram[WRITE_END] = _open_osfhandle((intptr_t) childStdin_Write, _O_WRONLY|_O_BINARY);
        ToProgram[READ_END] = _open_osfhandle((intptr_t) childStdin_Read, _O_RDONLY|_O_BINARY);

        // This implementation calls CreateProcess, specifying CREATE_NO_WINDOW, to prevent console
        // window from being displayed (e.g., when a console program is called from a GUI program).
        std::string commandLine;
        // If the executable has a space, quote the string
        size_t pos = executable.find_first_of(' ');
        if (pos == std::string::npos)
            commandLine = executable;   // no space, so quotes not needed
        else
            commandLine = '"' + executable + '"';
        for (size_t i = 0; i < arguments.size(); i++) {
            commandLine.push_back(' ');
            // Quote arguments if necessary
            pos = arguments[i].find_first_of(' ');
            if (pos != std::string::npos) commandLine.push_back('"');
            commandLine.append(arguments[i]);
            if (pos != std::string::npos) commandLine.push_back('"');
        }
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput = childStdin_Read;
        si.hStdOutput = childStdout_Write;
        si.hStdError = childStdout_Write;
        ZeroMemory(&pi, sizeof(pi));
        char *cmdLine = strdup(commandLine.c_str());
        if (!CreateProcessA(NULL, cmdLine, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            CMN_LOG_INIT_ERROR << "osaPipeExec: failed to create child process " << executable
                               << ", error = " << GetLastError() << std::endl;
            CMN_LOG_INIT_ERROR << "commandLine: " << commandLine << std::endl;
            free(cmdLine);
            CloseAllPipes();
            return false;
        }
        free(cmdLine);
        INTERNALS(hProcess) = pi.hProcess;

        if (!INTERNALS(hProcess)) {
            CMN_LOG_INIT_ERROR << "Class osaPipeExec: Open: exec failed for pipe \"" << this->Name << "\"" << std::endl;
            CloseAllPipes();
            return false;
        }

        CloseUnusedHandles();
    }
    else {
        // Original implementation, which calls _pipe and _spawnvp
        if (_pipe(ToProgram, 4096, O_BINARY | O_NOINHERIT) == -1) {
            CMN_LOG_INIT_ERROR << "Class osaPipeExec: Open: can't create pipe \"" << this->Name << "\"" << std::endl;
            return false;
        }
        if (_pipe(FromProgram, 4096, O_BINARY | O_NOINHERIT) == -1) {
            CMN_LOG_INIT_ERROR << "Class osaPipeExec: Open: can't create pipe \"" << this->Name << "\"" << std::endl;
            CloseAllPipes();
            return false;
        }

        // Save the pipe read handle for later use
        INTERNALS(readHandle) = (HANDLE)_get_osfhandle(FromProgram[READ_END]);

        /* Copy stdin and stdout before we close them so we can restore them after the spawn */
        int stdinCopy = _dup(_fileno(stdin));
        if (stdinCopy < 0)
            CMN_LOG_INIT_WARNING << "Class osaPipeExec: Open: failed to copy stdin" << std::endl;
        int stdoutCopy = _dup(_fileno(stdout));
        if (stdoutCopy < 0)
            CMN_LOG_INIT_WARNING << "Class osaPipeExec: Open: failed to copy stdout" << std::endl;

        /* Replace stdin and stdout to receive from and write to the pipe */
        if (_dup2(ToProgram[READ_END], _fileno(stdin)) == -1) {
            CMN_LOG_INIT_ERROR << "Class osaPipeExec: failed to dup stdin" << std::endl;
            RestoreIO(stdinCopy, stdoutCopy);
            CloseAllPipes();
            return false;
        }
        if (_dup2(FromProgram[WRITE_END], _fileno(stdout)) == -1) {
            CMN_LOG_INIT_ERROR << "Class osaPipeExec: failed to dup stdout" << std::endl;
            RestoreIO(stdinCopy, stdoutCopy);
            CloseAllPipes();
            return false;
        }

        CloseUnusedHandles();
        char **cmd = ParseCommand(executable, arguments);
        /* We need to quote the arguments but not the file name. Evidently, Windows
           parses the two differently. Therefore we use executable.c_str() instead
           of Command[0]*/
        intptr_t ret = _spawnvp(P_NOWAIT, executable.c_str(), cmd);
        FreeCommand(cmd);
        if (ret == -1)
           CMN_LOG_INIT_ERROR << "Class osaPipeExec: failed to spawn executable: " << executable
                              << ", " << strerror(errno) << std::endl;
        else
           INTERNALS(hProcess) = (HANDLE) ret;

        if (!INTERNALS(hProcess)) {
            CMN_LOG_INIT_ERROR << "Class osaPipeExec: Open: exec failed for pipe \"" << this->Name << "\"" << std::endl;
            RestoreIO(stdinCopy, stdoutCopy);
            CloseAllPipes();
            return false;
        }

        RestoreIO(stdinCopy, stdoutCopy);
    }
#endif

    Connected = true;
    return true;
}


bool osaPipeExec::Close(bool killProcess)
{
    bool ret = Connected;

    if (WriteFlag && DoClose(ToProgram[WRITE_END]) == -1)
        ret = false;
    if (ReadFlag && DoClose(FromProgram[READ_END]) == -1)
        ret = false;
    ReadFlag = WriteFlag = false;

    if (killProcess) {
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
        if (INTERNALS(pid) > 0) {
            ret = true;
            if (IsProcessRunning()) {
                if (kill(INTERNALS(pid), SIGKILL) == -1)
                    ret = false;
            }
            INTERNALS(pid) = -1;
        }
#elif (CISST_OS == CISST_WINDOWS)
        if (INTERNALS(hProcess)) {
            ret = true;
            if (IsProcessRunning()) {
                /* It would be better to check the return value of TerminateProcess, but it
                   randomly fails with error code 5 ("access is denied"), causing failures
                   even though the process actually does terminate */
                TerminateProcess(INTERNALS(hProcess), ERROR_SUCCESS);
            }
            if (CloseHandle(INTERNALS(hProcess)) == 0)
                ret = false;
            INTERNALS(hProcess) = 0;
        }
#endif
    }

    Connected = false;
    return ret;
}


int osaPipeExec::Read(char *buffer, int maxLength) const
{
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
    ssize_t bytesRead = -1;
#elif (CISST_OS == CISST_WINDOWS)
    int bytesRead = -1;
#endif

    /* Check for ReadFlag before opening. This is unnecessary on Unix but it
       prevents a debug assertion error on Windows */
    if (Connected && ReadFlag) {
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
        bytesRead = read(FromProgram[READ_END], buffer, maxLength*sizeof(char));
#elif (CISST_OS == CISST_WINDOWS)
        bytesRead = _read(FromProgram[READ_END], buffer, maxLength*sizeof(char));
#endif
    }

    if (bytesRead == -1) {
        return -1;
    } else {
        return static_cast<int>(bytesRead / sizeof(char));
    }
}


std::string osaPipeExec::Read(int maxLength) const
{
    char * buffer = new char[maxLength+1];
    int charsRead = Read(buffer, maxLength);

    std::string result;
    if (charsRead != -1) {
        buffer[charsRead] = '\0';
        result = std::string(buffer);
    }
    delete[] buffer;
    return result;
}


int osaPipeExec::ReadUntil(char * buffer, int length, char stopChar, double timeoutSec) const
{
    char * s = buffer;
    int charsRead = 0;
    int result;
    char lastChar = stopChar+1; // dummy value that's not stopChar
    double endTime = (timeoutSec > 0.0) ? (osaGetTime() + timeoutSec) : 0.0;
    // Normally, the Read method is blocking, which makes it difficult to support a timeout
    // (i.e., a call to Read can block well beyond the timeout period).

#if (CISST_OS == CISST_WINDOWS)
    // On Windows, it is difficult to check if the pipe has waiting bytes.
    // The PeekNamedPipe method would be perfect, except that it does not work when using Qt.
    //     DWORD dw_bytes;
    //     PeekNamedPipe(readHandle, NULL, 0, NULL, &dw_bytes, NULL);
    // The recommended alternative is to use asynchronous (overlapped) I/O, but
    // the approach used here is to temporarily change the pipe to non-blocking mode and
    // then read from the pipe while characters are available. When the Read method returns -1
    // or 0 (no chars available) we use MsgWaitForMultipleObjects to poll for any Windows messages
    // while waiting for a short time. This allows Windows GUI programs to be responsive (e.g.,
    // to mouse clicks) while waiting on the pipe. Note that we also check the (child) process
    // handle so that we can detect whether the child process has exited.
    bool nonblocking = false;
    DWORD saved_pipe_mode;
    if (endTime > 0.0) {
        if (GetNamedPipeHandleState(INTERNALS_CONST(readHandle), &saved_pipe_mode, NULL, NULL, NULL, NULL, 0)) {
            if (saved_pipe_mode == PIPE_NOWAIT) {
                nonblocking = true;
            }
            else {
                // Change pipe to non-blocking
                DWORD pipe_mode = PIPE_NOWAIT;
                if (SetNamedPipeHandleState(INTERNALS_CONST(readHandle), &pipe_mode, NULL, NULL))
                    nonblocking = true;
                else
                    CMN_LOG_RUN_WARNING << "osaPipeExec::ReadUntil failed to set PIPE_NOWAIT, error = " << GetLastError() << std::endl;
            }
        }
        else
            CMN_LOG_RUN_WARNING << "osaPipeExec::ReadUntil failed to query pipe mode, error = " << GetLastError() << std::endl;
    }
    bool processEnded = false;  // Whether child process has ended
    HANDLE handles[1] = { INTERNALS_CONST(hProcess) };
#endif

    while ((charsRead < length) && (lastChar != stopChar)) {

#if (CISST_OS == CISST_WINDOWS)
        // On Windows, we first try to read from the (non-blocking) pipe, then poll for messages.
        // See comment above for further details.
        if (nonblocking && (osaGetTime() > endTime))
            break;  // timeout
        result = Read(s, 1);
        if (nonblocking && (result <= 0)) {
            // No bytes read; sleep a little and poll for events
            if (processEnded) {
                // If we have already detected that the child process has ended and we just
                // read 0 bytes, then we are finished.
                CMN_LOG_RUN_WARNING << "osaPipeExec::ReadUntil returning " << charsRead
                                    << " bytes after child process ended" << std::endl;
                break;
            }
            double timeLeft = endTime-osaGetTime();
            DWORD timeout_msec = static_cast<DWORD>(1000.0*timeLeft/100.0 + 0.5);
            DWORD ret = MsgWaitForMultipleObjects(1, handles, FALSE, timeout_msec, QS_ALLEVENTS);
            if (ret == WAIT_OBJECT_0) {
                CMN_LOG_RUN_WARNING << "osaPipeExec::ReadUntil detected that child process has ended" << std::endl;
                // We do not exit right away because there may still be some bytes in the pipe.
                processEnded = true;
            }
            else if (ret == WAIT_OBJECT_0+1) {
                MSG msg;
                // Read all the messages in this next loop, removing each message as we read it.
                while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                    // If it is a quit message, exit.
                    if (msg.message == WM_QUIT) {
                        CMN_LOG_RUN_WARNING << "osaPipeExec::ReadUntil got quit message" << std::endl;
                        endTime = osaGetTime();  // force a timeout
                    }
                    else {
                        // Otherwise, dispatch the message.
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                    }
                } // End of PeekMessage while loop.
            }
            continue;
        }
#else
        // On Linux (and other platforms), it is much easier because the select call can be used to
        // check whether any characters are available in the (blocking) pipe.
        if (endTime > 0.0) {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(FromProgram[READ_END], &readfds);
            int nfds = FromProgram[READ_END]+1;
            double timeLeft = endTime-osaGetTime();
            time_t sec = static_cast<time_t>(floor(timeLeft));
            suseconds_t usec = static_cast<suseconds_t>((timeLeft - sec) * 1e6);
            timeval timeout = { sec , usec };
            int retval = select(nfds, &readfds, NULL, NULL, &timeout);
            if (retval == -1) {
                CMN_LOG_RUN_WARNING << "osaPipeExec::ReadUntil select failed after " << charsRead
                                    << " bytes, " << strerror(errno) << std::endl;
                charsRead = -1;
                break;
            }
            else if (retval == 0) {
                break;  // timeout
            }
        }
        result = Read(s, 1);
#endif
        if (result == -1) {
            if (charsRead > 0) {
                CMN_LOG_RUN_WARNING << "osaPipeExec::ReadUntil discarding " << charsRead << " characters." << std::endl;
            }
            charsRead = -1;
            break;
        }
        else if (result == 1) {
            lastChar = *s;
            charsRead++;
            s++;
        }
    }

#if (CISST_OS == CISST_WINDOWS)
    if (nonblocking) {
        // Restore pipe mode (probably blocking)
        if (!SetNamedPipeHandleState(INTERNALS_CONST(readHandle), &saved_pipe_mode, NULL, NULL))
            CMN_LOG_RUN_WARNING << "osaPipeExec::ReadUntil failed to set PIPE_WAIT, error = " << GetLastError() << std::endl;
    }
#endif

    return charsRead;
}


std::string osaPipeExec::ReadUntil(int length, char stopChar, double timeoutSec) const
{
    char * buffer = new char[length+1];
    int charsRead = ReadUntil(buffer, length, stopChar, timeoutSec);
    std::string result;
    if ((charsRead > 0) && (buffer[charsRead-1] == stopChar)) {
        buffer[charsRead] = '\0';
        result = std::string(buffer);
    }
    delete[] buffer;
    return result;
}


std::string osaPipeExec::ReadString(int length, double timeoutSec) const
{
    return ReadUntil(length, '\0', timeoutSec);
}


int osaPipeExec::Write(const char * buffer, int n)
{
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
    ssize_t bytesWritten = -1;
#elif (CISST_OS == CISST_WINDOWS)
    int bytesWritten = -1;
#endif

    if (Connected && WriteFlag) {
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
        bytesWritten = write(ToProgram[WRITE_END], buffer, n*sizeof(char));
#elif (CISST_OS == CISST_WINDOWS)
        bytesWritten = _write(ToProgram[WRITE_END], buffer, n*sizeof(char));
#endif
    }

    if (bytesWritten == -1) {
        return -1;
    } else {
        return static_cast<int>(bytesWritten / sizeof(char));
    }
}


int osaPipeExec::Write(const char * buffer)
{
    return Write(buffer, static_cast<int>(strlen(buffer))+1);
}


int osaPipeExec::Write(const std::string & s)
{
    return Write(s.c_str());
}


int osaPipeExec::Write(const std::string & s, int n)
{
    return Write(s.c_str(), n);
}


bool osaPipeExec::IsConnected(void) const
{
    return this->Connected;
}

bool osaPipeExec::IsProcessRunning(void) const
{
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
    if (INTERNALS_CONST(pid) < 0)
        return false;
    return (waitpid(INTERNALS_CONST(pid), 0, WNOHANG) == 0);
#elif (CISST_OS == CISST_WINDOWS)
    // Starting with Windows XP, can call GetExitCodeProcess to determine whether process still active
    if (!INTERNALS_CONST(hProcess))
        return false;
    return (WaitForSingleObject(INTERNALS_CONST(hProcess), 0) == WAIT_TIMEOUT);
#else
    return true;
#endif
}

const std::string & osaPipeExec::GetName(void) const
{
    return this->Name;
}
