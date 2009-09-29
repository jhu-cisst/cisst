#ifndef _robMutex_h
#define _robMutex_h

namespace cisstRobot{
  class robMutexBase{
  public:
    robMutexBase(){}
    ~robMutexBase(){}
    virtual void Lock() = 0;
    virtual void Unlock() = 0;
  };
}

#endif

#if (CISST_OS == CISST_LINUX_RTAI) || \
    (CISST_OS == CISST_LINUX)      || \
    (CISST_OS == CISST_DARWIN)     || \
    (CISST_OS == CISST_SOLARIS)

#include <cisstRobot/robMutexPOSIX.h>

#elif (CISST_OS == CISST_WINDOWS)

#include <cisstRobot/robMutexWin32.h>

#endif

