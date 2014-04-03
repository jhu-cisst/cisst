/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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
#if (CISST_OS == CISST_QNX)
#include <errno.h>
#else
#include <sys/errno.h>
#endif
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

unsigned int osaPipeExec::SizeOfInternals(void)
{
    return sizeof(osaPipeExecInternals);
}

osaPipeExec::osaPipeExec(const std::string & name):
    Command(0),
    Connected(false),
    Name(name)
{
    CMN_ASSERT(sizeof(Internals) >= SizeOfInternals());
}

osaPipeExec::~osaPipeExec(void)
{
    Close();
}

void osaPipeExec::CloseAllPipes(void)
{
    CMN_LOG_INIT_ERROR << "Class osaPipeExec: CloseAllPipes: called for pipe \"" << this->Name << "\"" << std::endl;
    delete[] Command;
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

#if (CISST_OS == CISST_WINDOWS)
void osaPipeExec::RestoreIO(int newStdin, int newStdout)
{
    if (_dup2(newStdin, _fileno(stdin)) == -1) {
        CloseAllPipes();
    }
    if (_dup2(newStdout, _fileno(stdout)) == -1) {
        CloseAllPipes();
    }
}
#endif

bool osaPipeExec::Open(const std::string & executable, const std::string & mode)
{
    std::vector<std::string> arguments;
    return Open(executable, arguments, mode);
}

bool osaPipeExec::Open(const std::string & executable,
                       const std::vector<std::string> & arguments,
                       const std::string & mode)
{
    if (Connected) {
        return false;
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
#elif (CISST_OS == CISST_WINDOWS)
    if (_pipe(ToProgram, 4096, O_BINARY | O_NOINHERIT) == -1) {
        CMN_LOG_INIT_ERROR << "Class osaPipeExec: Open: can't create pipe \"" << this->Name << "\"" << std::endl;
        return false;
    }
    if (_pipe(FromProgram, 4096, O_BINARY | O_NOINHERIT) == -1) {
        CMN_LOG_INIT_ERROR << "Class osaPipeExec: Open: can't create pipe \"" << this->Name << "\"" << std::endl;
        CloseAllPipes();
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

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
    /* Spawn a child and parent process for communication */
    INTERNALS(pid) = fork();

    /* Parent process to send and receive output */
    if (INTERNALS(pid) > 0) {
        /* We want input to come from parent to the program and output the other
           direction. Thus we don't need these ends of the pipe */
        if (close(ToProgram[READ_END]) == -1) {
            CloseAllPipes();
            return false;
        }
        if (close(FromProgram[WRITE_END]) == -1) {
            CloseAllPipes();
            return false;
        }

        if (!WriteFlag && close(ToProgram[WRITE_END]) == -1) {
            CloseAllPipes();
            return false;
        }
        if (!ReadFlag && close(FromProgram[READ_END]) == -1) {
            CloseAllPipes();
            return false;
        }

        Connected = true;
        return true;
    }

    /* Child thread to run the program */
    else if (INTERNALS(pid) == 0) {
        /* Replace stdin and stdout to receive from and write to the pipe */
        if (dup2(ToProgram[READ_END], STDIN_FILENO) == -1) {
            CloseAllPipes();
            return false;
        }
        if (dup2(FromProgram[WRITE_END], STDOUT_FILENO) == -1) {
            CloseAllPipes();
            return false;
        }

        Command = ParseCommand(executable, arguments);
        if (Command != 0 && Command[0] != 0) {
            execvp(Command[0], Command);
        } else {
            CMN_LOG_INIT_ERROR << "Class osaPipeExec: Open: exec failed for pipe \"" << this->Name
                               << "\" because the program name is empty" << std::endl;
        }

        /* If we get here then exec failed */
        CMN_LOG_INIT_ERROR << "Class osaPipeExec: Open: exec failed for pipe \"" << this->Name << "\"" << std::endl;
        CloseAllPipes();
        return false;
    }
#elif (CISST_OS == CISST_WINDOWS)
    /* Copy stdin and stdout before we close them so we can restore them after the spawn */
    int stdinCopy = _dup(_fileno(stdin));
    int stdoutCopy = _dup(_fileno(stdout));

    /* Replace stdin and stdout to receive from and write to the pipe */
    if (_dup2(ToProgram[READ_END], _fileno(stdin)) == -1) {
		RestoreIO(stdinCopy, stdoutCopy);
        CloseAllPipes();
        return false;
    }
    if (_dup2(FromProgram[WRITE_END], _fileno(stdout)) == -1) {
		RestoreIO(stdinCopy, stdoutCopy);
        CloseAllPipes();
        return false;
    }

    /* We want input to come from parent to the program and output the other
       direction. Thus we don't need these ends of the pipe */
    if (_close(FromProgram[WRITE_END]) == -1) {
		RestoreIO(stdinCopy, stdoutCopy);
        CloseAllPipes();
        return false;
    }

    if (_close(ToProgram[READ_END]) == -1) {
		RestoreIO(stdinCopy, stdoutCopy);
        CloseAllPipes();
        return false;
    }

    if (!WriteFlag && _close(ToProgram[WRITE_END]) == -1) {
		RestoreIO(stdinCopy, stdoutCopy);
        CloseAllPipes();
        return false;
    }
    if (!ReadFlag && _close(FromProgram[READ_END]) == -1) {
		RestoreIO(stdinCopy, stdoutCopy);
        CloseAllPipes();
        return false;
    }

    Command = ParseCommand(executable, arguments);
    if (Command != 0 && Command[0] != 0) {
		/* We need to quote the arguments but not the file name. Evidently, Windows
		parses the two differently. Therefore we use executable.c_str() instead
		of Command[0]*/
        INTERNALS(hProcess) = (HANDLE) _spawnvp(P_NOWAIT, executable.c_str(), Command);
    } else {
        CMN_LOG_INIT_ERROR << "Class osaPipeExec: Open: exec failed for pipe \"" << this->Name
                           << "\" because the program name is empty" << std::endl;
		RestoreIO(stdinCopy, stdoutCopy);
        CloseAllPipes();
        return false;
    }
    if (!INTERNALS(hProcess)) {
        CMN_LOG_INIT_ERROR << "Class osaPipeExec: Open: exec failed for pipe \"" << this->Name << "\"" << std::endl;
		RestoreIO(stdinCopy, stdoutCopy);
        CloseAllPipes();
        return false;
    }

    RestoreIO(stdinCopy, stdoutCopy);
#endif

    delete[] Command;
    Connected = true;
    return true;
}


bool osaPipeExec::Close(bool killProcess)
{
    if (!Connected) {
        return false;
    } else {
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
        if (WriteFlag && close(ToProgram[WRITE_END]) == -1) {
            return false;
        }
#elif (CISST_OS == CISST_WINDOWS)
        if (WriteFlag && _close(ToProgram[WRITE_END]) == -1) {
            return false;
        }
#endif

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
        if (ReadFlag && close(FromProgram[READ_END]) == -1)
            return false;
#elif (CISST_OS == CISST_WINDOWS)
        if (ReadFlag && _close(FromProgram[READ_END]) == -1)
            return false;
#endif

        ReadFlag = WriteFlag = false;

        if (killProcess) {
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
            if (kill(INTERNALS(pid), SIGKILL) == -1)
                return false;
#elif (CISST_OS == CISST_WINDOWS)
			/* It would be better to check the return value of TerminateProcess, but it
               randomly fails with error code 5 ("access is denied"), causing failures
               even though the process actually does terminate */
			TerminateProcess(INTERNALS(hProcess), ERROR_SUCCESS);
            if (CloseHandle(INTERNALS(hProcess)) == 0)
                return false;
#endif
        }

        Connected = false;
        return true;
    }
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


int osaPipeExec::ReadUntil(char * buffer, int length, char stopChar) const
{
    char * s = buffer;
    int charsRead = 0;
    int result;
    char lastChar = stopChar+1; // dummy value that's not stopChar
    while (charsRead < length && lastChar != stopChar) {
        result = Read(s, 1);
        if (result == -1)
            return -1;
        charsRead += result;
        s += result;
        lastChar = *(s-1);
    }
    return charsRead;
}


std::string osaPipeExec::ReadUntil(int length, char stopChar) const
{
    char * buffer = new char[length+1];
    int charsRead = ReadUntil(buffer, length, stopChar);
    std::string result;
    if (charsRead > 0 && buffer[charsRead-1] == stopChar) {
        buffer[charsRead] = '\0';
        result = std::string(buffer);
    }
    delete[] buffer;
    return result;
}


std::string osaPipeExec::ReadString(int length) const
{
    return ReadUntil(length, '\0');
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


const std::string & osaPipeExec::GetName(void) const
{
    return this->Name;
}
