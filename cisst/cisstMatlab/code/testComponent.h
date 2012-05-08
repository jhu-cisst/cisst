#ifndef _testComponent_h
#define _testComponent_h

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsVector.h>

class testComponent: public mtsTaskPeriodic {

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_ALL);

 protected:
    mtsDoubleVec Data;
    size_t Counter;

    inline void Zero(void) {
	Data.SetAll(0.0);
	CMN_LOG_CLASS_RUN_DEBUG << "Data set to " << Data << std::endl;
    }
    inline void Hundred(void) {
	Data.SetAll(100.0);
	CMN_LOG_CLASS_RUN_DEBUG << "Data set to " << Data << std::endl;
    }
    inline void Set(const mtsDoubleVec & data) {
	Data.ForceAssign(data);
	CMN_LOG_CLASS_RUN_DEBUG << "Data set to " << Data << std::endl;
    }

 public:
    // provide a name for the task and define the frequency (time
    // interval between calls to the periodic Run).  Also used to
    // populate the interface(s)
    testComponent(const std::string & name);

    ~testComponent() {};
    // all four methods are pure virtual in mtsTask
    inline void Configure(const std::string & CMN_UNUSED(filename)) {};
    void Startup(void);
    void Run(void);
    inline void Cleanup(void) {};
};

CMN_DECLARE_SERVICES_INSTANTIATION(testComponent);


// this component proxy should be generated automaticaly at runtime
class testComponentProxy: public mtsComponent {

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_ALL);

 public:
    mtsFunctionVoid Zero;
    mtsFunctionVoid Hundred;
    mtsFunctionRead Get;
    mtsFunctionWrite Set;

    testComponentProxy(const std::string & name);
    ~testComponentProxy() {};
};

CMN_DECLARE_SERVICES_INSTANTIATION(testComponentProxy);

#endif // _testComponent_h
