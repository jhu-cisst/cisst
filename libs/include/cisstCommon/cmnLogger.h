/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2004-08-31

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Declaration of cmnLogger amd macros for human readable logging
  \ingroup cisstCommon
*/
#pragma once

#ifndef _cmnLogger_h
#define _cmnLogger_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLogLoD.h>
#include <cisstCommon/cmnLODOutputMultiplexer.h>
#include <cisstCommon/cmnMultiplexerStreambuf.h>
#include <cisstCommon/cmnLODMultiplexerStreambuf.h>

#include <string>
#include <vector>
#include <fstream>

#include <cisstCommon/cmnExport.h>




/*! This macro is used to log human readable information within a
  registered class.  It can only be used within the scope of a method
  part of a class registered in the cmnClassRegister using the macros
  #CMN_DECLARE_SERVICES and #CMN_IMPLEMENT_SERVICES declared in
  cmnClassRegister.h.  For a non registered class of a global
  function, please use #CMN_LOG.

  The message is streamed along with a Level of Detail to cmnLogger
  provided that two conditions are met:

  - The level of detail associated to the message must be lower or
    equal to the level of detail used to filter the messages at the
    global level.  This global level of detail is set via the
    cmnLogger.

  - The level of detail associated to the message must be lower or
    equal to the level of detail used to filter the messages by the
    class itself.  To modify the class specific level of detail, see
    cmnClassServicesBase and cmnClassRegister

  The macro creates an output stream if the level of detail of the
  message satisfies the two criteria defined above.  It can be used as
  any other output stream:

  \code
  CMN_LOG_CLASS_INIT_DEBUG << "This is a message of LoD 4" << this->x << std::endl;
  \endcode

  \param lod The log level of detail of the message.
*/
#define CMN_LOG_CLASS(lod) \
    ((lod > cmnLogger::GetLoD()) || (lod > Services()->GetLoD()))?\
        (void*)0:\
    ((cmnLODOutputMultiplexer(cmnLogger::GetMultiplexer(), lod).Ref()) << cmnLogLoDString[lod] << " - Class " << Services()->GetName() << ": ")


/*! Macros defined to use #CMN_LOG_CLASS for a given level of detail. */
//@{
#define CMN_LOG_CLASS_INIT_ERROR   CMN_LOG_CLASS(CMN_LOG_LOD_INIT_ERROR)
#define CMN_LOG_CLASS_INIT_WARNING CMN_LOG_CLASS(CMN_LOG_LOD_INIT_WARNING)
#define CMN_LOG_CLASS_INIT_VERBOSE CMN_LOG_CLASS(CMN_LOG_LOD_INIT_VERBOSE)
#define CMN_LOG_CLASS_INIT_DEBUG   CMN_LOG_CLASS(CMN_LOG_LOD_INIT_DEBUG)
#define CMN_LOG_CLASS_RUN_ERROR    CMN_LOG_CLASS(CMN_LOG_LOD_RUN_ERROR)
#define CMN_LOG_CLASS_RUN_WARNING  CMN_LOG_CLASS(CMN_LOG_LOD_RUN_WARNING)
#define CMN_LOG_CLASS_RUN_VERBOSE  CMN_LOG_CLASS(CMN_LOG_LOD_RUN_VERBOSE)
#define CMN_LOG_CLASS_RUN_DEBUG    CMN_LOG_CLASS(CMN_LOG_LOD_RUN_DEBUG)
#define CMN_LOG_CLASS_VERY_VERBOSE CMN_LOG_CLASS(CMN_LOG_LOD_VERY_VERBOSE)
//@}


/*! This macro is used to log human readable information within the
  scope of a global function (e.g. main()).  It can also be used in
  classes which are not registered in cmnClassRegister (see also
  macros #CMN_DECLARE_SERVICES and #CMN_IMPLEMENT_SERVICES declared in
  cmnClassRegister.h).  For a registered class, please use
  #CMN_LOG_CLASS.

  The message is streamed along with a Level of Detail to cmnLogger
  provided that the level of detail associated to the message is lower
  or equal to the level of detail used to filter the messages at the
  global level.  This global level of detail is set via the cmnLogger.

  The macro creates an output stream if the level of detail of the
  message satisfies the criterion defined above.  It can be used as
  any other output stream:

  \code
  CMN_LOG_INIT_ERROR << "This is a message of LoD CMN_LOG_LOD_INIT_ERROR" << argc << std::endl;
  \endcode

  \param lod The log level of detail of the message.
*/
#define CMN_LOG(lod) \
    (lod > cmnLogger::GetLoD())? \
    (void*)0: \
    ((cmnLODOutputMultiplexer(cmnLogger::GetMultiplexer(), lod).Ref()) << cmnLogLoDString[lod] << " - ")


/*! Macros defined to use #CMN_LOG for a given level of detail. */
//@{
#define CMN_LOG_INIT_ERROR   CMN_LOG(CMN_LOG_LOD_INIT_ERROR)
#define CMN_LOG_INIT_WARNING CMN_LOG(CMN_LOG_LOD_INIT_WARNING)
#define CMN_LOG_INIT_VERBOSE CMN_LOG(CMN_LOG_LOD_INIT_VERBOSE)
#define CMN_LOG_INIT_DEBUG   CMN_LOG(CMN_LOG_LOD_INIT_DEBUG)
#define CMN_LOG_RUN_ERROR    CMN_LOG(CMN_LOG_LOD_RUN_ERROR)
#define CMN_LOG_RUN_WARNING  CMN_LOG(CMN_LOG_LOD_RUN_WARNING)
#define CMN_LOG_RUN_VERBOSE  CMN_LOG(CMN_LOG_LOD_RUN_VERBOSE)
#define CMN_LOG_RUN_DEBUG    CMN_LOG(CMN_LOG_LOD_RUN_DEBUG)
#define CMN_LOG_VERY_VERBOSE CMN_LOG(CMN_LOG_LOD_VERY_VERBOSE)
//@}


/*! This macro is used to add useful information to Log macros
  (#CMN_LOG_CLASS and #CMN_LOG). It provides the additional
  functionality of logging the file name and line number at which the
  message was generated.

  For example:
  \code
  CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS << "Error occurs" << std::endl;
  \endcode

  For any error with a very common message, this helps the programmer
  to quickly find the origin of the message in the source code.
*/
#define CMN_LOG_DETAILS \
    "File: " << __FILE__ << " Line: " << __LINE__ << " - "



/*!  \brief Class to glue the class register, the output multiplexer
  and the message level of detail to form the logging entity of cisst.

  \ingroup cisstCommon

  cmnLogger is defined as a singleton, i.e. there is only one instance
  of cmnLogger.  This unique object maintains the data required for
  the cisst logging system.  The desing of the logging system
  relies on:

  - Human readable messages are associated to a Level of Detail.

  - The messages are sent and filtered based on their level of detail.
    The macros used to send the messages actually check if the message
    is to be sent or not before creating them in order to improve
    performances (see #CMN_LOG_CLASS and #CMN_LOG).

  - The messages are sent to a multiplexer owned by cmnLogger.  This
    multiplexer allows to send the message (along with their level of
    detail) to multiple output streams.

  To filter the messages, the user can use:

  - The global level of detail defined in cmnLogger.  Any message with
    a level of detail greater than the global level of detail will be
    ignored (at minimal execution time cost).  This level of detail is
    used by #CMN_LOG_CLASS and #CMN_LOG.  To modify the global level
    of detail, use cmnLogger::SetLoD(newLoD).

  - Each class defines a level of detail.  This allows to tune the log
    based on the user's needs.  One can for example allow all the
    messages (from errors to warnings) from a given class and block
    all the messages from every other class.  As for the global level
    of detail, if a message's level of detail is greater than the
    class level of detail, it will be ignored (at minimal execution
    time cost).  This level of detail is used only be #CMN_LOG_CLASS.
    To modify a class level of detail, use either the class register
    with cmnClassRegister::SetLoD("className", newLoD) or the class
    services with object.Services()->SetLoD(newLoD).

  - The output streams level of details.  Each output stream has its
    own level of detail and will stream only the messages with a level
    of details lesser or equal to his.  This allows for example to log
    everything to a file while printing only the high priority ones to
    std::cout.  To set the level of detail of an output stream, use
    cmnLogger::GetMultiplexer()->AddChannel(newStream, newLoD).

  \sa cmnClassRegister cmnClassServicesBase cmnLODOutputMultiplexer
*/
class CISST_EXPORT cmnLogger {

 public:
    /*! Type used to define the logging level of detail. */
    typedef cmnLODMultiplexerStreambuf<char> StreamBufType;

    typedef cmnLogLoD LogLoDType;

 private:
    /*! Global Level of Detail used to filter all messages.

      - CMN_LOG_LOD_INIT_ERROR: Errors during the initialization.
      - CMN_LOG_LOD_INIT_WARNING: Warnings during the initialization.
      - CMN_LOG_LOD_INIT_VERBOSE and CMN_LOG_LOD_INIT_DEBUG: Extra messages during the initialization.
      - CMN_LOG_LOD_RUN_ERROR: Errors during normal operations (also defined as #CMN_LOG_DEFAULT_LOD).
      - CMN_LOG_LOD_RUN_WARNING: Warnings during normal operations.
      - CMN_LOG_LOD_RUN_VERBOSE and CMN_LOG_LOD_RUN_DEBUG: Extra messages during normal operations.
      - CMN_LOG_LOD_VERY_VERBOSE: Very verbose.

      The idea is that for most classes, important errors happens
      during the initialization (constructor, opening a serial port,
      configuring some hardware device, open a grapical context, etc.)
      and during the normal operations, time can become critical.
      Therefore a level CMN_LOG_LOD_RUN_ERROR would log a lot of
      information at the beginning and only the critical messages
      during the normal operations.
    */
    LogLoDType LoD;

    /*! Single multiplexer used to stream the log out */
    cmnLODMultiplexerStreambuf<char> LoDMultiplexerStreambuf;

    /*! Instance specific implementation of SetLoD.  \sa SetLoD */
    void SetLoDInstance(LogLoDType lod);

    /*! Instance specific implementation of GetLoD.  \sa GetLoD */
    inline LogLoDType GetLoDInstance(void) const {
        return LoD;
    }

    /*! Instance specific implementation of GetMultiplexer.
      \sa GetMultiplexer */
    inline StreamBufType * GetMultiplexerInstance(void) {
        return &(LoDMultiplexerStreambuf);
    }

    /*! Create and get a pointer on the default log file. */
    std::ofstream * DefaultLogFile(const std::string & defaultLogFileName = "cisstLog.txt");

    /*! Instance specific implementation of HaltDefaultLog. */
    inline void HaltDefaultLogInstance(void) {
        LoDMultiplexerStreambuf.RemoveChannel(*(DefaultLogFile()));
    }

    /*! Instance specific implementation of ResumeDefaultLog. */
    inline void ResumeDefaultLogInstance(LogLoDType newLoD = CMN_LOG_LOD_RUN_ERROR) {
        LoDMultiplexerStreambuf.AddChannel(*(DefaultLogFile()), newLoD);
    }

    /*! Instance specific implementation of AddChannel */
    inline void AddChannelInstance(std::ostream & outputStream, LogLoDType lod) {
        LoDMultiplexerStreambuf.AddChannel(outputStream, lod);
    }

 protected:
    /*! Constructor.  The only constructor must be private in order to
      ensure that the class register is a singleton. */
    cmnLogger(const std::string & defaultLogFileName = "cisstLog.txt");

 public:
    /*! The log is instantiated as a singleton.  To access
      the unique instantiation, one needs to use this static method.
      The instantiated log is created at the first call of
      this method since it is a static variable declared in this
      method's scope.

      \return A pointer to the log. */
    static cmnLogger * Instance(void);


    /*! Set the global Level of Detail to filter the log messages.

      \param lod The global level of detail used to filter the log.

      \sa SetLoDInstance */
    static void SetLoD(LogLoDType lod) {
        Instance()->SetLoDInstance(lod);
    }

    /*! Get the global Level of Detail used to filter the log
      messages.

      \return The global level of detail used to filter the log.

      \sa GetLoDInstance */
    static LogLoDType GetLoD(void) {
        return Instance()->GetLoDInstance();
    }

    /*! Returns the cmnLODMultiplexerStreambuf directly. This allows
      manipulation of the streambuffer for operations such as adding or
      deleting channels for the stream..

      \return cmnLODMultiplexerStreambuf<char>* The Streambuffer.

      \sa GetMultiplexerInstance
    */
    static StreamBufType * GetMultiplexer(void) {
        return Instance()->GetMultiplexerInstance();
    }


    /*! Disable the default log file "cisstLog.txt".  This method
      removes the default log from the output list of the multiplexer
      but doesn't close the default log file. */
    static void HaltDefaultLog(void) {
        Instance()->HaltDefaultLogInstance();
    }


    /*! Resume the default log file.  By default, the log is enabled
      (this is the default behavior of the cmnLogger constructor) but
      this can be halted by using HaltDefaultLog().  Using
      ResumeDefaultLog() allows to resume the log to "cisstLog.txt"
      without losing previous logs. */
    static void ResumeDefaultLog(LogLoDType newLoD = CMN_LOG_LOD_RUN_ERROR) {
        Instance()->ResumeDefaultLogInstance(newLoD);
    }

    /*! Add an output stream to the logger.  The level of detail
      provided is used to filter the messages, i.e. any message with a
      level of detail higher than the level associated to the output
      stream will not be streamed. */
    static void AddChannel(std::ostream & outputStream, LogLoDType lod = CMN_LOG_LOD_VERY_VERBOSE) {
        Instance()->AddChannelInstance(outputStream, lod);
    }

};


#endif // _cmnLogger_h

