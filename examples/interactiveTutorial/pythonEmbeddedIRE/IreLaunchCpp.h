// Launch IRE in C++-created thread (using osaThread)
// Note that it is best to do all IRE actions in the Run() method,
// including the calls to ireFramework::Instance (which should occur
// when LaunchIREShell is called) and FinalizeShell, because otherwise
// the IRE (Python interpreter) would be called from multiple threads.
class IreLaunch {
public:
    IreLaunch() {}
    ~IreLaunch() {}
    void *Run(char *startup) {
        try {
            ireFramework::LaunchIREShell(startup, false);
        }
        catch (...) {
            cout << "*** ERROR:  could not launch IRE shell ***" << endl;
        }
        ireFramework::FinalizeShell();
        return this;
    }
};
