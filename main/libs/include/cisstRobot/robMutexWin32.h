
#ifndef _robMutexWin32_h
#define _robMutexWin32_h

#include <cisstRobot/robMutex.h>
#include <windows.h>

namespace cisstRobot{

  class robMutexWin32 : public robMutexBase {
  private:
    HANDLE mutex;
  public:

    robMutexWin32(){ mutex = CreateMutex(NULL, false, NULL); }
    virtual void Lock(){ WaitForSingleObject(mutex, INFINITE); }
    virtual void Unlock(){ ReleaseMutex(mutex); }

  };

  typedef robMutexWin32 robMutex;

}

#endif // _robMutexWin32_h
