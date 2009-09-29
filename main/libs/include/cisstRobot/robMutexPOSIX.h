
#ifndef _robMutexPOSIX_h
#define _robMutexPOSIX_h

#include <cisstRobot/robMutex.h>
#include <pthread.h>

namespace cisstRobot{

  class robMutexPOSIX : public robMutexBase {
  private:
    pthread_mutex_t mutex;
  public:

    robMutexPOSIX(){ pthread_mutex_init(&mutex, NULL); }
    virtual void Lock(){ pthread_mutex_lock(&mutex); }
    virtual void Unlock(){ pthread_mutex_unlock(&mutex); }

  };

  typedef robMutexPOSIX robMutex;

}

#endif // _robMutexPOSIX_h
