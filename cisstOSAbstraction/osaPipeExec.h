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

#ifndef _osaPipeExec_h
#define _osaPipeExec_h

#include <vector>
#include <string>

// Always include last
#include <cisstOSAbstraction/osaExport.h>

class CISST_EXPORT osaPipeExec {
    /*! Internals that are OS-dependent */
    enum {INTERNALS_SIZE = 32};
    char Internals[INTERNALS_SIZE];

    /*! Return the size of the actual object used by the OS.  This is
      used for testing only. */
    static unsigned int SizeOfInternals(void);
    friend class osaPipeExecTest;

    char ** Command;
    int ToProgram[2];
    int FromProgram[2];
    bool Connected;
    bool ReadFlag;
    bool WriteFlag;
    std::string Name;

    /*! Free resources before returning an error. Also free the command
      pointer */
    void CloseAllPipes(void);

    /*! Parse out the command and arguments and return an array in the
      form that execvp/_spawnvp accept */
    char ** ParseCommand(const std::string & executable,
                         const std::vector<std::string> & arguments);

#if (CISST_OS == CISST_WINDOWS)
	/*! Restore I/O to their original values before returning
      false. Not needed for Unix because it uses fork instead of
      spawn */
	void RestoreIO(int newStdin, int newStdout);
#endif

 public:
    /*! Constructor with name */
    osaPipeExec(const std::string & name = "unnamed");

    /*! Destructor calls Close() */
    ~osaPipeExec(void);

    /*! Open a pipe using an existing executable and specifying
      read/write mode.  Mode can be "r", "w", or "rw" specifying which
      direction(s) the pipe should go. Returns true if the Open
      succeeded, false otherwise */
    bool Open(const std::string & executable, const std::string & mode);

    /*! Open a pipe using an existing executable with a vector of
      arguments and specifying read/write mode.  Mode can be "r", "w",
      or "rw" specifying which direction(s) the pipe should
      go. Returns true if the Open succeeded, false otherwise */
    bool Open(const std::string & executable,
              const std::vector<std::string> & parameters,
              const std::string & mode);

    /*! Close the pipe. Returns true if the Close succeeded, false
      otherwise. */
    bool Close(bool killProcess = true);

    /*! Read at most maxLength characters from the pipe into
      buffer. Return number of characters read or -1 for an error */
    int Read(char * buffer, int maxLength) const;

    /*! Read at most maxLength characters from the pipe, including
      '\0', and return an std::string containing those characters or
      empty string for an error */
    std::string Read(int maxLength) const;

    /*! Read at most maxLength characters from the pipe, but stop reading if
    stopChar is read. Return the number of characters read */
    int ReadUntil(char * buffer, int maxLength, char stopChar) const;

    /*! std::string version of ReadUntil
    stopChar is read. Return the number of characters read */
    std::string ReadUntil(int maxLength, char stopChar) const;

    /*! Equivalent to ReadUntil(maxLength, '\0') */
    std::string ReadString(int maxLength) const;

    /*! Write the null-terminated buffer to the pipe. Return the
      number of characters written or -1 for an error */
    int Write(const char * buffer);

    /*! Write the first n characters of buffer to pipe. Return the
      number of characters written or -1 for an error */
    int Write(const char * buffer, int n);

    /*! Write s to the pipe and return the number of characters read
      or -1 for an error */
    int Write(const std::string & s);

    /*! Write the first n characters of s to pipe. Return the number of
      characters written or -1 for an error */
    int Write(const std::string & s, int n);

    /*! Indicate if the pipe is opened (or at least supposed to be
      opened) */
    bool IsConnected(void) const;

    /*! Get name provided in constructor. */
    const std::string & GetName(void) const;
};

#endif // _osaPipeExec_h
