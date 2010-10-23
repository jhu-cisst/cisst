/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s): Martin Kelly
  Created on: 2010-09-23

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnAssert.h>
#include <cisstOSAbstraction/osaPipeExec.h>
#include <string.h>
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
#include <signal.h>
#include <unistd.h>
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
    #endif
};

#define INTERNALS(A) (reinterpret_cast<osaPipeExecInternals*>(Internals)->A)

unsigned int osaPipeExec::SizeOfInternals(void) {
    return sizeof(osaPipeExecInternals);
}

osaPipeExec::osaPipeExec() : Connected(false) {
    CMN_ASSERT(sizeof(Internals) >= SizeOfInternals());
}

osaPipeExec::~osaPipeExec(void) {
    Close();
}

void osaPipeExec::Abort(void) {
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
    close(ToProgram[READ_END]);
    close(ToProgram[WRITE_END]);
    close(FromProgram[READ_END]);
    close(FromProgram[WRITE_END]);
#elif (CISST_OS == CISST_WINDOWS)
    _close(ToProgram[READ_END]);
    _close(ToProgram[WRITE_END]);
    _close(FromProgram[READ_END]);
    _close(FromProgram[WRITE_END]);
#endif
}

bool osaPipeExec::Open(const std::string & cmd, const std::string & mode) {
    if (Connected)
        return false;
    else {
        #if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
            if (pipe(ToProgram) < 0) {
                CMN_LOG_INIT_ERROR << "Can't create pipe in osaPipeExec::Open" << std::endl;
                return false;
            }
            if (pipe(FromProgram) < 0) {
                CMN_LOG_INIT_ERROR << "Can't create pipe in osaPipeExec::Open" << std::endl;
                Abort();
                return false;
            }
        #elif (CISST_OS == CISST_WINDOWS)
            if (_pipe(ToProgram, 4096, O_BINARY | O_NOINHERIT) < 0) {
                CMN_LOG_INIT_ERROR << "Can't create pipe in osaPipeExec::Open" << std::endl;
                return false;
            }
            if (_pipe(FromProgram, 4096, O_BINARY | O_NOINHERIT) < 0) {
                CMN_LOG_INIT_ERROR << "Can't create pipe in osaPipeExec::Open" << std::endl;
                Abort();
                return false;
            }
        #endif

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

        char * const command[] = {(char * const) cmd.c_str(), NULL};
        #if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
            /* Spawn a child and parent process for communication */
            INTERNALS(pid) = fork();

            /* Parent process to send and receive output */
            if (INTERNALS(pid) > 0) {
                /* We want input to come from parent to the program and output the other
                direction. Thus we don't need these ends of the pipe */
                if (close(ToProgram[READ_END]) == -1) {
                    Abort();
                    return false;
                }
                if (close(FromProgram[WRITE_END]) == -1) {
                    Abort();
                    return false;
                }

                if (!WriteFlag && close(ToProgram[WRITE_END]) == -1) {
                    Abort();
                    return false;
                }
                if (!ReadFlag && close(FromProgram[READ_END]) == -1) {
                    Abort();
                    return false;
                }

                Connected = true;
                return true;
            }

            /* Child thread to run the program */
            else if (INTERNALS(pid) == 0) {
                /* Replace stdin and stdout to receive from and write to the pipe */
                if (dup2(ToProgram[READ_END], 0) == -1) {
                    Abort();
                    return false;
                }
                if (dup2(FromProgram[WRITE_END], 1) == -1) {
                    Abort();
                    return false;
                }

                /* Start the command */
                if (command != NULL && command[0] != NULL)
                    execvp(command[0], command);
                else
                    CMN_LOG_INIT_ERROR << "Exec failed in osaPipeExec::Open because command is empty" << std::endl;

                /* If we get here then exec failed */
                CMN_LOG_INIT_ERROR << "Exec failed in osaPipeExec::Open" << std::endl;
                Abort();
                return false;
            }
        #elif (CISST_OS == CISST_WINDOWS)
            /* Replace stdin and stdout to receive from and write to the pipe */
            if (_dup2(ToProgram[READ_END], _fileno(stdin)) == -1) {
                Abort();
                return false;
            }
            if (_dup2(FromProgram[WRITE_END], _fileno(stdout)) == -1) {
                Abort();
                return false;
            }

            /* We want input to come from parent to the program and output the other
            direction. Thus we don't need these ends of the pipe */
            if (_close(FromProgram[WRITE_END]) == -1) {
                Abort();
                return false;
            }
            if (_close(ToProgram[READ_END]) == -1) {
                Abort();
                return false;
            }

            if (!WriteFlag && _close(ToProgram[WRITE_END]) == -1) {
                Abort();
                return false;
            }
            if (!ReadFlag && _close(FromProgram[READ_END]) == -1) {
                Abort();
                return false;
            }

            /* Spawn process */
            if (command != NULL && command[0] != NULL)
                INTERNALS(hProcess) = (HANDLE) _spawnvp(P_NOWAIT, command[0], command);
            else {
                CMN_LOG_INIT_ERROR << "Spawn failed in osaPipeExec::Open because command is empty" << std::endl;
                Abort();
                return false;
            }
            if (!INTERNALS(hProcess)) {
                CMN_LOG_INIT_ERROR << "Spawn failed in osaPipeExec::Open" << std::endl;
                Abort();
                return false;
            }
        #endif

        Connected = true;
        return true;
    }
}

bool osaPipeExec::Close(bool killProcess) {
    if (!Connected)
        return false;
    else {
        if (WriteFlag)
        #if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
            if (close(ToProgram[WRITE_END]) == -1)
                return false;
        #elif (CISST_OS == CISST_WINDOWS)
            if (_close(ToProgram[WRITE_END]) == -1)
                return false;
        #endif

        if (ReadFlag)
        #if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
            if (close(FromProgram[READ_END]) == -1)
                return false;
        #elif (CISST_OS == CISST_WINDOWS)
            if (_close(FromProgram[READ_END]) == -1)
                return false;
        #endif

        if (killProcess) {
        #if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
            if (kill(INTERNALS(pid), SIGKILL) == -1)
                return false;
        #elif (CISST_OS == CISST_WINDOWS)
            if (CloseHandle(INTERNALS(hProcess)) == 0)
                return false;
            if (TerminateProcess(INTERNALS(hProcess), 0) == 0)
                return false;
        #endif
        }

        Connected = false;
        return true;
    }
}

int osaPipeExec::Read(char *buffer, int maxLength) const {
    #if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
        ssize_t bytesRead = read(FromProgram[READ_END], buffer, maxLength*sizeof(char));
    #elif (CISST_OS == CISST_WINDOWS)
        int bytesRead = _read(FromProgram[READ_END], buffer, maxLength*sizeof(char));
    #endif

    if (bytesRead == -1)
        return -1;
    else
        return static_cast<int>(bytesRead / sizeof(char));
    return -1;
}

std::string osaPipeExec::Read(int maxLength) const {
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

int osaPipeExec::Write(const char * buffer) {
    return Write(buffer, strlen(buffer)+1);
}

int osaPipeExec::Write(const char * buffer, int n) {
    #if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
        ssize_t bytesWritten = write(ToProgram[WRITE_END], buffer, n*(sizeof(char)));
    #elif (CISST_OS == CISST_WINDOWS)
        int bytesWritten = _write(ToProgram[WRITE_END], buffer, n*sizeof(char));
    #endif

    if (bytesWritten == -1)
        return -1;
    else
        return static_cast<int>(bytesWritten / sizeof(char));
    return -1;
}

int osaPipeExec::Write(const std::string & s) {
    return Write(s.c_str());
}
