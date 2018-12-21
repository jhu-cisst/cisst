/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2004-08-31

  (C) Copyright 2004-2018 Johns Hopkins University (JHU), All Rights Reserved.

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

// MJ: some thirdparty drivers on QNX make CMN_LOG macro throw exceptions
// (e.g., std::bad_cast) and the following preprocessor can be used to bypass
// this issue by replacing CMN_LOG with std::cout.
#if (CISST_OS == CISST_QNX)
#define BYPASS_CMN_LOG
#endif


/*! This macro is used to log human readable information within a
  registered class.  It can only be used within the scope of a method
  part of a class registered in the cmnClassRegister using the macros
  #CMN_DECLARE_SERVICES and #CMN_IMPLEMENT_SERVICES declared in
  cmnClassRegister.h.  For a non registered class of a global
  function, please use #CMN_LOG.

  The message is streamed along with a Level of Detail to cmnLogger
  provided that two conditions are met:

  - The overall log mask must allow the level of detail associated to
    the message.  The overall mask can be set using
    cmnLogger::SetMask.

  - The class specific log mask must allow the level of detail
    associated to the message.  Class specific masks can be set using
    cmnLogger::SetMaskClass, cmnLogger::SetMaskClassMatching or
    cmnLogger::SetMaskClassAll.

  The macro creates an output stream if the level of detail of the
  message satisfies the two criteria defined above.  It can be used as
  any other output stream:

  \code
  CMN_LOG_CLASS_INIT_DEBUG << "This is a message of LoD 4" << this->x << std::endl;
  \endcode

  \param lod The log level of detail of the message.

  \todo replace (CISST_OS == CISST_DARWIN) with a better solution for OS X logging compiler errors
*/
#ifdef BYPASS_CMN_LOG

  #undef CMN_LOG_CLASS_INSTANCE
  #define CMN_LOG_CLASS_INSTANCE(objectPointer, lod) std::cout

#else

  #define CMN_LOG_CLASS_INSTANCE(objectPointer, lod)                      \
if (!(cmnLogger::GetMask() & objectPointer->Services()->GetLogMask() & lod)) ; else \
    ((cmnLODOutputMultiplexer(objectPointer->GetLogMultiplexer(), lod).Ref()) << cmnLogLevelToString(lod) << " Class " << objectPointer->Services()->GetName() << ": ")

#endif

#define CMN_LOG_CLASS(lod) CMN_LOG_CLASS_INSTANCE(this, lod)


/*! Macros defined to use #CMN_LOG_CLASS_INSTANCE for a given level of detail. */
//@{
#define CMN_LOG_CLASS_INSTANCE_INIT_ERROR(objectPointer)   CMN_LOG_CLASS_INSTANCE(objectPointer, CMN_LOG_LEVEL_INIT_ERROR) << CMN_LOG_DETAILS << " "
#define CMN_LOG_CLASS_INSTANCE_INIT_WARNING(objectPointer) CMN_LOG_CLASS_INSTANCE(objectPointer, CMN_LOG_LEVEL_INIT_WARNING)
#define CMN_LOG_CLASS_INSTANCE_INIT_VERBOSE(objectPointer) CMN_LOG_CLASS_INSTANCE(objectPointer, CMN_LOG_LEVEL_INIT_VERBOSE)
#define CMN_LOG_CLASS_INSTANCE_INIT_DEBUG(objectPointer)   CMN_LOG_CLASS_INSTANCE(objectPointer, CMN_LOG_LEVEL_INIT_DEBUG)
#define CMN_LOG_CLASS_INSTANCE_RUN_ERROR(objectPointer)    CMN_LOG_CLASS_INSTANCE(objectPointer, CMN_LOG_LEVEL_RUN_ERROR) << CMN_LOG_DETAILS << " "
#define CMN_LOG_CLASS_INSTANCE_RUN_WARNING(objectPointer)  CMN_LOG_CLASS_INSTANCE(objectPointer, CMN_LOG_LEVEL_RUN_WARNING)
#define CMN_LOG_CLASS_INSTANCE_RUN_VERBOSE(objectPointer)  CMN_LOG_CLASS_INSTANCE(objectPointer, CMN_LOG_LEVEL_RUN_VERBOSE)
#define CMN_LOG_CLASS_INSTANCE_RUN_DEBUG(objectPointer)    CMN_LOG_CLASS_INSTANCE(objectPointer, CMN_LOG_LEVEL_RUN_DEBUG)
//@}


/*! Macros defined to use #CMN_LOG_CLASS for a given level of detail. */
//@{
#define CMN_LOG_CLASS_INIT_ERROR   CMN_LOG_CLASS(CMN_LOG_LEVEL_INIT_ERROR) << CMN_LOG_DETAILS << " "
#define CMN_LOG_CLASS_INIT_WARNING CMN_LOG_CLASS(CMN_LOG_LEVEL_INIT_WARNING)
#define CMN_LOG_CLASS_INIT_VERBOSE CMN_LOG_CLASS(CMN_LOG_LEVEL_INIT_VERBOSE)
#define CMN_LOG_CLASS_INIT_DEBUG   CMN_LOG_CLASS(CMN_LOG_LEVEL_INIT_DEBUG)
#define CMN_LOG_CLASS_RUN_ERROR    CMN_LOG_CLASS(CMN_LOG_LEVEL_RUN_ERROR) << CMN_LOG_DETAILS << " "
#define CMN_LOG_CLASS_RUN_WARNING  CMN_LOG_CLASS(CMN_LOG_LEVEL_RUN_WARNING)
#define CMN_LOG_CLASS_RUN_VERBOSE  CMN_LOG_CLASS(CMN_LOG_LEVEL_RUN_VERBOSE)
#define CMN_LOG_CLASS_RUN_DEBUG    CMN_LOG_CLASS(CMN_LOG_LEVEL_RUN_DEBUG)
//@}


/*! This macro is used to log human readable information within the
  scope of a global function (e.g. main()).  It can also be used in
  classes which are not registered in cmnClassRegister (see also
  macros #CMN_DECLARE_SERVICES and #CMN_IMPLEMENT_SERVICES declared in
  cmnClassRegister.h).  For a registered class, please use
  #CMN_LOG_CLASS.

  The message is streamed along with a Level of Detail to cmnLogger
  provided that the function and overall log masks allow the level of
  detail associated to the message.  The overall mask can be set using
  cmnLogger::SetMask.  The function mask can be set using
  cmnLogger::SetMaskFunction.

  The macro creates an output stream if the level of detail of the
  message satisfies the criterion defined above.  It can be used as
  any other output stream:

  \code
  CMN_LOG_INIT_ERROR << "This is a message of LoD CMN_LOG_LEVEL_INIT_ERROR" << argc << std::endl;
  \endcode

  \param lod The log level of detail of the message.
*/
#ifdef BYPASS_CMN_LOG

  #undef CMN_LOG
  #define CMN_LOG(lod) std::cout

#else

  #define CMN_LOG(lod) \
  if (!(cmnLogger::GetMask() & cmnLogger::GetMaskFunction() & lod)) ; else \
    ((cmnLODOutputMultiplexer(cmnLogger::GetMultiplexer(), lod).Ref()) << cmnLogLevelToString(lod) << " ")

#endif

/*! Macros defined to use #CMN_LOG for a given level of detail. */
//@{
#define CMN_LOG_INIT_ERROR   CMN_LOG(CMN_LOG_LEVEL_INIT_ERROR) << CMN_LOG_DETAILS << " "
#define CMN_LOG_INIT_WARNING CMN_LOG(CMN_LOG_LEVEL_INIT_WARNING)
#define CMN_LOG_INIT_VERBOSE CMN_LOG(CMN_LOG_LEVEL_INIT_VERBOSE)
#define CMN_LOG_INIT_DEBUG   CMN_LOG(CMN_LOG_LEVEL_INIT_DEBUG)
#define CMN_LOG_RUN_ERROR    CMN_LOG(CMN_LOG_LEVEL_RUN_ERROR) << CMN_LOG_DETAILS << " "
#define CMN_LOG_RUN_WARNING  CMN_LOG(CMN_LOG_LEVEL_RUN_WARNING)
#define CMN_LOG_RUN_VERBOSE  CMN_LOG(CMN_LOG_LEVEL_RUN_VERBOSE)
#define CMN_LOG_RUN_DEBUG    CMN_LOG(CMN_LOG_LEVEL_RUN_DEBUG)
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
    "File: " << cmnLogger::ExtractFileName(__FILE__) << " Line: " << __LINE__ << " - "



/*!  \brief Class to glue the class register, the output multiplexer
  and the message level of detail to form the logging entity of cisst.

  \ingroup cisstCommon

  cmnLogger is defined as a singleton, i.e. there is only one instance
  of cmnLogger.  This unique object maintains the data required for
  the cisst logging system.  The design of the logging system
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

  - The global log mask defined in cmnLogger.  Any message with a
    level of detail incompatible with overall log mask will be ignored
    (at minimal execution time cost).  This mask is used by
    #CMN_LOG_CLASS and #CMN_LOG.  To modify the overall mask, use
    cmnLogger::SetMask(newMask).

  - Each class relies on its own log mask.  This allows to tune the
    log based on the user's needs.  One can for example allow all the
    messages (from errors to debug) from a given class and block all
    the messages from every other class.  As for the overall log mask,
    if a message's level of detail is incompatible with the class
    mask, it will be ignored (at minimal execution time cost).  This
    log mask is used only be #CMN_LOG_CLASS.  To modify a class log
    mask, use either the logger with
    cmnLogger::SetMaskClass("className", newMask),
    cmnLogger::SetMaskClassMatching("cmn", newMask),
    cmnLogger::SetMaskClassAll(newMask) or the class services with
    object.Services()->SetLogMask(newMask).

  - The output streams masks.  Each output stream has its own log mask
    and will stream only the messages with a compatible level of
    detail.  This allows for example to log everything to a file while
    printing only the high priority ones to std::cout.  To set the
    level of detail of an output stream, use
    cmnLogger::AddChannel(newStream, newMask).

  \sa cmnClassRegister cmnClassServicesBase cmnLODOutputMultiplexer
*/
class CISST_EXPORT cmnLogger {

 public:
    /*! Type used to define the logging level of detail. */
    typedef cmnLODMultiplexerStreambuf<char> StreamBufType;

 private:
    /*! Global Level of Detail used to filter all messages.

      - CMN_LOG_LEVEL_INIT_ERROR: Errors during the initialization.
      - CMN_LOG_LEVEL_INIT_WARNING: Warnings during the initialization.
      - CMN_LOG_LEVEL_INIT_VERBOSE and CMN_LOG_LEVEL_INIT_DEBUG: Extra messages during the initialization.
      - CMN_LOG_LEVEL_RUN_ERROR: Errors during normal operations (also defined as #CMN_LOG_DEFAULT_LOD).
      - CMN_LOG_LEVEL_RUN_WARNING: Warnings during normal operations.
      - CMN_LOG_LEVEL_RUN_VERBOSE and CMN_LOG_LEVEL_RUN_DEBUG: Extra messages during normal operations.

      Users can define their log mask combining the different log
      levels (e.g. CMN_LOG_LEVEL_RUN_ERROR | CMN_LOG_LEVEL_INIT_ERROR)
      or use existing masks defined in cmnLogLoD.h
      (e.g. CMN_LOG_ALLOW_ERRORS_AND_WARNINGS).

      The idea is that for most classes, important errors happens
      during the initialization (constructor, opening a serial port,
      configuring some hardware device, open a grapical context, etc.)
      and during the normal operations, time can become critical.
    */
    cmnLogMask Mask;

    /*! Function mask.  Similar to the global mask, it is used for all
      logs not associated to a "registered" class.  This applies to
      all logs generated using the macros CMN_LOG_<level> but not to
      logs generated using the macros CMN_LOG_CLASS_<level>. */
    cmnLogMask FunctionMask;

    /*! Single multiplexer used to stream the log out */
    StreamBufType LoDMultiplexerStreambuf;

    /*! Default filename (possibly including path) for default log.
        Normally, cisstLog.txt in current directory. */
    static std::string DefaultLogFileName;

    /*! Whether cmnLogger instance has been created. */
    static bool InstanceCreated;

    /*! Instance specific implementation of SetMask.
      \sa SetMask */
    void SetMaskInstance(cmnLogMask mask);

    /*! Instance specific implementation of GetMaskInstance.
      \sa GetMask */
    cmnLogMask GetMaskInstance(void);

    /*! Instance specific implementation of SetMaskFunction.
      \sa SetMaskFunction */
    void SetMaskFunctionInstance(cmnLogMask mask);

    /*! Instance specific implementation of GetMaskFunctionInstance.
      \sa GetMaskFunction */
    cmnLogMask GetMaskFunctionInstance(void);

    /*! Instance specific implementation of GetMultiplexer.
      \sa GetMultiplexer */
    StreamBufType * GetMultiplexerInstance(void);

    /*! Create and get a pointer on the default log file. */
    std::ofstream * DefaultLogFile(const std::string & defaultLogFileName = DefaultLogFileName);

    /*! Instance specific implementation of HaltDefaultLog. */
    void HaltDefaultLogInstance(void);

    /*! Instance specific implementation of ResumeDefaultLog. */
    void ResumeDefaultLogInstance(cmnLogMask newMask);

    /*! Instance specific implementation of AddChannel */
    void AddChannelInstance(std::ostream & outputStream, cmnLogMask mask);

    /*! Instance specific implementation of RemoveChannel */
    void RemoveChannelInstance(std::ostream & outputStream);

    /*! Instance specific implementation of Kill */
    void KillInstance(void);

 protected:
    /*! Constructor.  The only constructor must be private in order to
      ensure that the class register is a singleton. */
    cmnLogger(const std::string & defaultLogFileName = DefaultLogFileName);

 public:
    /*! The log is instantiated as a singleton.  To access the unique
      instantiation, one needs to use this static method.  The
      instantiated log is created at the first call of this method
      since it is a static variable declared in this method's scope.

      \return A pointer to the logger. */
    static cmnLogger * Instance(void);


    /*! Set the global mask used to filter the log messages.
      \param mask The overall mask used to filter the log.
      \sa SetMaskInstance */
    static void SetMask(cmnLogMask mask) {
        Instance()->SetMaskInstance(mask);
    }

    static void CISST_DEPRECATED SetLoD(cmnLogMask mask) {
        Instance()->SetMaskInstance(mask);
    }

    /*! Get the global mask used to filter the log messages.
      \return The global mask used to filter the log.
      \sa GetMaskInstance */
    static cmnLogMask GetMask(void) {
        return Instance()->GetMaskInstance();
    }

    static cmnLogMask CISST_DEPRECATED GetLoD(void) {
        return Instance()->GetMaskInstance();
    }


    /*! Set the "function" mask used to filter the log messages.
      \param mask The function mask used to filter the log.
      \sa SetMaskFunctionInstance */
    static void SetMaskFunction(cmnLogMask mask) {
        Instance()->SetMaskFunctionInstance(mask);
    }


    /*! Get the function mask used to filter the log messages.
      \return The function mask used to filter the log.
      \sa GetMaskInstance */
    static cmnLogMask GetMaskFunction(void) {
        return Instance()->GetMaskFunctionInstance();
    }


    /*! Specify the log mask for a specific class.  See
      cmnClassRegister::SetLogMaskClass.

      \param className The name of the class
      \param mask The log mask to be applied

      \return bool True if the class is registered.
    */
    static bool SetMaskClass(const std::string & className, cmnLogMask mask);


    /*! Specify the log mask for all registered classes.  See
        cmnClassRegister::SetLogMaskClassAll.

      \param mask The log mask to be applied

      \return bool True if there is at least one class mask was
      modified
    */
    static bool SetMaskClassAll(cmnLogMask mask);


    /*! Specify the log mask for all classes with a name matching a
      given string.  See cmnClassRegister::SetLogMaskClassMatching.

      \param stringToMatch A string found in class names (e.g. "cmn")
      \param mask The log mask to be applied

      \return bool True if there is at least one class LoD was
      modified
    */
    static bool SetMaskClassMatching(const std::string & stringToMatch, cmnLogMask mask);


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
    static void ResumeDefaultLog(cmnLogMask newLoD = CMN_LOG_ALLOW_DEFAULT) {
        Instance()->ResumeDefaultLogInstance(newLoD);
    }


    /*! Set a new mask for the default log file.  This method uses
      HaltDefaultLog followed by ResumeDefaultLog. */
    static void SetMaskDefaultLog(cmnLogMask newLoD = CMN_LOG_ALLOW_DEFAULT) {
        Instance()->HaltDefaultLogInstance();
        Instance()->ResumeDefaultLogInstance(newLoD);
    }


    /*! Add an output stream to the logger.  The level of detail
      provided is used to filter the messages, i.e. any message with a
      level of detail higher than the level associated to the output
      stream will not be streamed. */
    static inline void AddChannel(std::ostream & outputStream,
                                  cmnLogMask mask = CMN_LOG_ALLOW_ALL) {
        Instance()->AddChannelInstance(outputStream, mask);
    }


    /*! Add std::cout output stream to the logger. Useful when calling
      from Python, where redirecting sys.stdout to std::cout is nontrivial. */
    static inline void AddChannelToStdOut(cmnLogMask mask = CMN_LOG_ALLOW_ERRORS_AND_WARNINGS) {
        Instance()->AddChannelInstance(std::cout, mask);
    }

    /*! Add std::cerr output stream to the logger. Useful when calling
      from Python, where redirecting sys.stderr to std::cerr is nontrivial. */
    static inline void AddChannelToStdErr(cmnLogMask mask = CMN_LOG_ALLOW_ERRORS_AND_WARNINGS) {
        Instance()->AddChannelInstance(std::cerr, mask);
    }

    static inline void RemoveChannel(std::ostream & outputStream) {
        Instance()->RemoveChannelInstance(outputStream);
    }

    static const char * ExtractFileName(const char * file);

    /*! Kill the logger.  Set all masks to disable logs and remove all
      output streams. */
    static inline void Kill(void) {
        Instance()->KillInstance();
    }

    /*! Set the name of the default log file. This function must be called before the cmnLogger instance is created.
        \returns true if the default log file name can be set (i.e., cmnLogger instance not yet created); false otherwise.
    */
    static bool SetDefaultLogFileName(const std::string &defaultLogFileName);

    /*! Returns name of default log file. */
    static std::string GetDefaultLogFileName(void) { return DefaultLogFileName; }

    /*! Returns true if cmnLogger instance has been created (i.e., constructor called). */
    static bool IsCreated() { return InstanceCreated; }
};


#endif // _cmnLogger_h
