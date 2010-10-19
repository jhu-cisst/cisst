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
#include <cisstOSAbstraction/osaPipe.h>
#include <string.h>
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
#include <signal.h>
#include <unistd.h>
#elif (CISST_OS == CISST_WINDOWS)
#include <io.h>
#include <fcntl.h>
#include <process.h>
#include <windows.h>
#endif

struct osaPipeInternals {
	/*! OS dependent variables */
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
		int pid;
	#elif (CISST_OS == CISST_WINDOWS)
		HANDLE hProcess;
	#endif
};

#define INTERNALS(A) (reinterpret_cast<osaPipeInternals*>(Internals)->A)

unsigned int osaPipe::SizeOfInternals(void) {
    return sizeof(osaPipeInternals);
}

osaPipe::osaPipe() {
    CMN_ASSERT(sizeof(Internals) >= SizeOfInternals());
}

void osaPipe::Open(char * const command[], const std::string & mode) {
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
		if (pipe(toProgram) < 0 || pipe(fromProgram) < 0)
			perror("Can't create pipe in osaPipe::Open");
	#elif (CISST_OS == CISST_WINDOWS)
		if (_pipe(toProgram, 4096, O_BINARY | O_NOINHERIT) < 0 || _pipe(fromProgram, 4096, O_BINARY | O_NOINHERIT) < 0)
			perror("Can't create pipe in osaPipe::Open");
	#endif

		readFlag = writeFlag = false;
		std::string::const_iterator it;
		for (it = mode.begin(); it != mode.end(); it++) {
			switch (*it) {
				case 'r':
					readFlag = true;
					break;
				case 'w':
					writeFlag = true;
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
			close(toProgram[READ_HANDLE]);
			close(fromProgram[WRITE_HANDLE]);

			if (!writeFlag)
				close(toProgram[WRITE_HANDLE]);
			if (!readFlag)
				close(fromProgram[READ_HANDLE]);
		}

		/* Child thread to run the program */
		else if (INTERNALS(pid) == 0) {
			/* Replace stdin and stdout to receive from and write to the pipe */
			dup2(toProgram[READ_HANDLE], 0);
			dup2(fromProgram[WRITE_HANDLE], 1);

			/* Start the command */
			if (command != NULL && command[0] != NULL)
				execvp(command[0], command);
			else
				perror("Exec failed in osaPipe::Open because command is empty");

			/* If we get here then exec failed */
			perror("Exec failed in osaPipe::Open");
		}
	#elif (CISST_OS == CISST_WINDOWS)
		/* Replace stdin and stdout to receive from and write to the pipe */
		_dup2(toProgram[READ_HANDLE], _fileno(stdin));
		_dup2(fromProgram[WRITE_HANDLE], _fileno(stdout));

		/* We want input to come from parent to the program and output the other
		direction. Thus we don't need these ends of the pipe */
		int ret = _close(fromProgram[WRITE_HANDLE]);
		ret = _close(toProgram[READ_HANDLE]);

		if (!writeFlag)
			ret = _close(toProgram[WRITE_HANDLE]);
		if (!readFlag)
			ret = _close(fromProgram[READ_HANDLE]);

		/* Spawn process */
		if (command != NULL && command[0] != NULL)
			INTERNALS(hProcess) = (HANDLE) _spawnvp(P_NOWAIT, command[0], command);
		else
			perror("Spawn failed in osaPipe::Open because command is empty");
		if (!INTERNALS(hProcess))
			perror("Spawn failed in osaPipe::Open");

		/* These aren't needed now */
		fclose(stdin);
		fclose(stdout);
	#endif
}

void osaPipe::Close(bool killProcess) {
	if (writeFlag)
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
		close(toProgram[WRITE_HANDLE]);
	#elif (CISST_OS == CISST_WINDOWS)
		_close(toProgram[WRITE_HANDLE]);
	#endif

	if (readFlag)
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
		close(fromProgram[READ_HANDLE]);
	#elif (CISST_OS == CISST_WINDOWS)
		_close(fromProgram[READ_HANDLE]);
	#endif

	if (killProcess)
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
		kill(INTERNALS(pid), SIGKILL);
	#elif (CISST_OS == CISST_WINDOWS)
		CloseHandle(INTERNALS(hProcess));
		TerminateProcess(INTERNALS(hProcess), 0);
	#endif
}

int osaPipe::Read(char *buffer, int maxLength) const {
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
		ssize_t ret = read(fromProgram[READ_HANDLE], buffer, sizeof(char)*maxLength);
	#elif (CISST_OS == CISST_WINDOWS)
		int ret = _read(fromProgram[READ_HANDLE], buffer, sizeof(char)*maxLength);
	#endif
	if (ret == -1)
		return -1;
	else
		return (int) (ret / sizeof(char));
	return -1;
}

int osaPipe::Write(const char *buffer) {
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
		ssize_t ret = write(toProgram[WRITE_HANDLE], buffer, sizeof(char)*strlen(buffer));
	#elif (CISST_OS == CISST_WINDOWS)
		int ret = _write(toProgram[WRITE_HANDLE], buffer, sizeof(char)*strlen(buffer));
	#endif
	if (ret == -1)
		return -1;
	else
		return (int) (ret / sizeof(char));
	return -1;
}
